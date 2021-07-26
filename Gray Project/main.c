#define _CRT_SECURE_NO_WARNINGS
#include "definitions.h"
#include "tree.h"
#include "image.h"
#include "linked_list.h"
#include "files.h"


/***TEST FUNCTION****/
void printImg(grayImage* img) {
	int i, k;
	for (i = 0; i < img->rows; i++) {
		for (k = 0; k < img->cols; k++) {
			printf("%3d ", img->pixels[i][k]);
		}
		putchar('\n');
	}
	putchar('\n');
	putchar('\n');
}

void Q1_Test(grayImage* img);
unsigned int Q2_Test(grayImage* img, imgPosCell*** segments);
grayImage* Q3_Test(grayImage* img, imgPosCell** segments, unsigned int size);
grayImage* Q4_Test();
void Q5_TEST(char* binary_file_name, grayImage* img);
void Q6_TEST(char* binary_file_name, char *pgm_file_name);
/****TEST FUNCTION***/

void main() {
	grayImage* img = Q4_Test(); /*Creating grayImage from file*/
	printImg(img); /*Print image pixels*/
	imgPosCell** segments;
	unsigned int size;

	Q1_Test(img); /*Get single segment*/
	
	size = Q2_Test(img, &segments); /*Get all image segments*/

	grayImage* new_img = Q3_Test(img, segments, size); /*Updating segments pixels according to segments*/
	printf("New image after updating:\n");
	printImg(new_img); /*Print new image after updating*/

	freeImg(new_img);
	freeSegments(segments, size);
	freeImg(img);

	img = readPGM("lena.ascii.pgm"); /*Reading other pgm file for next questions*/
	char binary_file_name[] = "compressed_img.bin";
	char pgm_file_name[] = "converted_img.pgm";
	Q5_TEST(binary_file_name,img); /*Get image and compressed it into binary file*/
	Q6_TEST(binary_file_name, pgm_file_name); /*Create PGM file from last compressed binary file*/
	freeImg(img);
}

void Q1_Test(grayImage *img) {
	unsigned char threshold;
	int x, y;
	imgPos kernel;
	printf("Please enter kernel position (Row,Col):\n");
	scanf("%d %d", &x, &y);
	while (0 > x || x >= img->rows || 0 > y || y >= img->cols) {
		printf("Wrong position. Please enter position again:\n");
		scanf("%d %d", &x, &y);
	}
	kernel[0] = x;
	kernel[1] = y;
	printf("Please segment threshold:\n");
	scanf("%d", &x);
	threshold = x;

	Segment* seg = findSingleSegment(img, kernel, threshold);
	printf("Number of pixels in the segment: %d\n\n", seg->size);
	freeTreeSegment(seg);
}

unsigned int Q2_Test(grayImage* img,imgPosCell ***segments) {
	unsigned char threshold;
	unsigned int x;
	printf("Please segment threshold:\n");
	scanf("%d", &x);
	threshold = x;

	int size_of_segments = findAllSegments(img, threshold, segments);
	printf("Size of segments in the img: %d\n\n", size_of_segments);
	return size_of_segments;
}

grayImage* Q3_Test(grayImage *img,imgPosCell **segments,unsigned int size) {
	return colorSegments(img, segments, size);
}

grayImage* Q4_Test() {
	char file_name[] = "feep.ascii.pgm";
	return readPGM(file_name);
}

void Q5_TEST(char* binary_file_name, grayImage* img) {
	unsigned int reduced_gray_level;
	printf("Please enter reduced max gray level: (Must be a pow of 2)\n");
	scanf("%d", &reduced_gray_level);
	saveCompressed(binary_file_name, img, reduced_gray_level);
	printf("Image compressed check %s file in project folder\n\n",binary_file_name);
}

void Q6_TEST(char* binary_file_name, char* pgm_file_name) {
	convertCompressedImageToPGM(binary_file_name, pgm_file_name);
	printf("PGM created, check %s in project folder\n\n",pgm_file_name);

}



