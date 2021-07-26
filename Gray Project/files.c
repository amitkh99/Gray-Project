#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif 

#include "files.h"


/*reads a PGM file and returns a grayImage that contains the image*/
grayImage* readPGM(char* fname) {
	int num_row = 0, num_col = 0, max_gray = 0;
	FILE* in_fptr = fopen(fname, "r");
	fileOpenCheck(in_fptr, fname);
	char readchar[PGM_TYPE_MAX_LENGTH], c;
	fscanf(in_fptr, "%s", readchar);     /*open the PGM file*/
	checkFileTypeValid(readchar);
	skipFileComments(in_fptr);
	fscanf(in_fptr, "%d", &num_col);
	fscanf(in_fptr, "%d", &num_row);
	fscanf(in_fptr, "%d", &max_gray);
	grayImage* re_img;  /*creates a returned grayImage*/
	re_img = createNewImg(num_row, num_col, in_fptr);

	fclose(in_fptr);
	return (re_img);
}
/*checks if the file opens as required*/
static void fileOpenCheck(FILE* fi, char* name) {
	if (!fi) {
		fprintf(stderr,"Opening %s file failed", name);
		exit(FILE_OPEN_ERROR);
	}
}
/*checks if the PGM file is P2 type*/
static void checkFileTypeValid(char* type) {
	if (!(strcmp(type, "P2") == 0)) {
		fprintf(stderr,"The file isn't valid");
		exit(FILE_OPEN_ERROR);
	}
}
/*checks for comments in the file and skips them*/
static void skipFileComments(FILE* in_fptr) {
	char c;
	getc(in_fptr);
	c = getc(in_fptr);
	while (c == '#')          /* skip comment lines */
	{
		c = getc(in_fptr);
		while (c != '\n') {
			c = getc(in_fptr);
		}                     /* skip to end of comment line*/
		c = getc(in_fptr);
	}
	ungetc(c, in_fptr);
}

/**Q5***/
/*Function get binary file name, ptr to image, and num of reduce_gray_level desired. Function reduce gray level of image pixels, and save it compressed into the binary file*/
void saveCompressed(char* file_name, grayImage* image, unsigned char reduced_gray_levels) {
	int bite_per_pixels;
	grayImage* reduced_image;
	reduced_image = createReducedImage(image, reduced_gray_levels, &bite_per_pixels);
	compressBinary(file_name, reduced_image, bite_per_pixels);
	freeImg(reduced_image);

}
/*Get binary file name, ptr to compressed image, num of bits that represent pixel. Function create compressed binary file with all image data*/
static void compressBinary(char* file_name, grayImage* img, int bits_per_pix) {

	FILE* out_ptr = fopen(file_name, "wb");
	fileOpenCheck(out_ptr, file_name);
	fwrite(&(img->rows), sizeof(unsigned short), 1, out_ptr);
	fwrite(&(img->cols), sizeof(unsigned short), 1, out_ptr);
	unsigned char max_gray_level = pow(2, bits_per_pix);
	fwrite(&(max_gray_level), sizeof(unsigned char), 1, out_ptr);

	int i, k;
	unsigned char byte_to_write = 0;
	unsigned char byte_to_write_left_bits = BYTE_SIZE, split_byte_bits_left = 0;
	char is_left_byte_to_write;
	for (i = 0; i < img->rows; i++) {
		for (k = 0; k < img->cols; k++) {
			is_left_byte_to_write = TRUE;
			if (byte_to_write_left_bits - bits_per_pix >= 0) { /*Means that all pixel data will written in the same byte */
				byte_to_write_left_bits -= bits_per_pix;
				byte_to_write = (byte_to_write | (img->pixels[i][k] << byte_to_write_left_bits));

				if (byte_to_write_left_bits == 0) { /*The byte has been completely filled, write it into the file */
					fwrite(&byte_to_write, sizeof(unsigned char), 1, out_ptr);
					is_left_byte_to_write = FALSE;
					byte_to_write_left_bits = BYTE_SIZE;
					byte_to_write = 0;
				}
			}
			else { /*Need to split pixel data between 2 bytes*/
				split_byte_bits_left = bits_per_pix - byte_to_write_left_bits;
				byte_to_write = (byte_to_write | (img->pixels[i][k] >> split_byte_bits_left));
				fwrite(&byte_to_write, sizeof(unsigned char), 1, out_ptr);
				byte_to_write_left_bits = BYTE_SIZE - split_byte_bits_left;
				byte_to_write = (img->pixels[i][k] << byte_to_write_left_bits);

			}
		}
	}
	if (is_left_byte_to_write) /*Means that last pixel data isnt written*/
		fwrite(&byte_to_write, sizeof(unsigned char), 1, out_ptr);

	fclose(out_ptr);
}


/***Q6****/
/*Function get name of compressed binary file, name of pgm file. Function create a PGM file from data storaged in the file*/
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name) {
	unsigned char reduced_max_gray_levels;
	grayImage* compressed_img = convertFileToCompressedImg(compressed_file_name, &reduced_max_gray_levels);
	makePGM_File(compressed_img, reduced_max_gray_levels, pgm_file_name);
}
/*Get binary file name, ptr to unsigned char. function update by reference reduces_max_gray_levels and return ptr to Dynamic img witch all pixels are still reduce. */
static grayImage* convertFileToCompressedImg(char* file_name, unsigned char* reduced_max_gray_levels) {
	FILE* in_ptr = fopen(file_name, "rb");
	fileOpenCheck(in_ptr, file_name);
	grayImage* img = (grayImage*)malloc(sizeof(grayImage));
	allocatingCheck(img);

	unsigned char bits_per_pix;
	*reduced_max_gray_levels = getImgDefinitions(img, in_ptr); /*Read image definitons from file*/
	bits_per_pix = findPow(*reduced_max_gray_levels);

	img->pixels = (unsigned char**)malloc(sizeof(unsigned char*) * img->rows);
	allocatingCheck(img->pixels);

	int i, k, bits_left = 0;
	unsigned char pixel, curr_byte, split_bits_left;
	for (i = 0; i < img->rows; i++) {
		img->pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * img->cols);
		allocatingCheck(img->pixels[i]);
		for (k = 0; k < img->cols; k++) {
			if (bits_left == 0) { /*Means that all previous byte has been read, reading a new byte*/
				fread(&curr_byte, sizeof(unsigned char), 1, in_ptr);
				bits_left = BYTE_SIZE;
			}
			if (bits_left - bits_per_pix >= 0) { /*All pixel data storage in the current byte*/
				pixel = COVERT_N_MSB_TO_INT(curr_byte, bits_per_pix);
				img->pixels[i][k] = pixel;
				curr_byte <<= bits_per_pix;
				bits_left -= bits_per_pix;
			}
			else { /*Pixel data split between two different bytes*/
				split_bits_left = bits_per_pix - bits_left;
				pixel = COVERT_N_MSB_TO_INT(curr_byte, bits_per_pix);
				fread(&curr_byte, sizeof(unsigned char), 1, in_ptr);
				pixel = (pixel | COVERT_N_MSB_TO_INT(curr_byte, split_bits_left));
				img->pixels[i][k] = pixel;
				bits_left = BYTE_SIZE - split_bits_left;
				curr_byte <<= split_bits_left;
			}
		}

	}
	fclose(in_ptr);
	return img;
}
/*Get ptr to image, ptr to FILE. Function updated image definitons from file and return max_gray_level of image*/
static int getImgDefinitions(grayImage* img, FILE* in_ptr) {
	unsigned char max_gray_level;
	fread(&(img->rows), sizeof(unsigned short), 1, in_ptr);
	fread(&(img->cols), sizeof(unsigned short), 1, in_ptr);
	fread(&max_gray_level, sizeof(unsigned char), 1, in_ptr);
	return max_gray_level;
}
/*receives an image with the reduced gray value turns it into an image with a gray value up to 256 and writes Ruth to a PGM file */
static void makePGM_File(grayImage* reduced_image, char reduced_gray_levels, char* pgm_file_name) {
	grayImage* big_image = changeReducedImage(reduced_image, reduced_gray_levels);
	writeToPGM_File(big_image, pgm_file_name);
	freeImg(big_image);
}
/* gets an image and turns Ruth into a pgm P2 file */
static void writeToPGM_File(grayImage* image, char* pgm_file_name) {
	FILE* out_ptr = fopen(pgm_file_name, "w");
	fileOpenCheck(out_ptr, pgm_file_name);
	fprintf(out_ptr, "%s\n", "P2");
	fprintf(out_ptr, "%d %d\n", image->cols, image->rows);
	fprintf(out_ptr, "%d\n", GRAY_LEVEL - 1);

	int i, j;
	for (i = 0; i < image->rows; i++) {
		for (j = 0; j < image->cols; j++) {
			fprintf(out_ptr, "%d ", image->pixels[i][j]);
		}
		fprintf(out_ptr, "\n");
	}

	fclose(out_ptr);
}
/*receives an image with the reduced gray value and returns an image with a gray value up to 256 */
static grayImage* changeReducedImage(grayImage* reduced_image, char reduced_gray_levels) {
	grayImage* re_img = duplicate_size_img(reduced_image);
	int pow_two = findPow(reduced_gray_levels);
	int set_of_values = GRAY_LEVEL / (pow(2, pow_two));
	int i;
	for (i = 0; i < re_img->rows; i++) {
		re_img->pixels[i] = createBigPixels(reduced_image->pixels[i],
			set_of_values, re_img->cols);
	}
	freeImg(reduced_image);
	return (re_img);
}
/*gets an array of pixels from the image with the reduced gray value and returns an array of pixels up to 256*/
static unsigned char* createBigPixels(unsigned char* old_pixels, int set_of_values, int size) {

	unsigned char* big_pixels = (unsigned char*)malloc(sizeof(unsigned char) * size);
	allocatingCheck(big_pixels);
	int i, j;
	unsigned char comp_value;
	for (i = 0; i < size; i++) {
		comp_value = ((old_pixels[i] + 1) * set_of_values) - 1;
		big_pixels[i] = comp_value;
	}
	return (big_pixels);
}
