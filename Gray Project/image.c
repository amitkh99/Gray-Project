#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif 
#include "image.h"
#include "linked_list.h"
#include "tree.h"


/***Q1**/
/*Get ptr to image, kernel pos and threshold, Return ptr to Segment*/
Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold) {
	char** occupied_pixels = createOccupiedPixelsArr(img->cols, img->rows); /* Get matrix in size of image, fill with 0 to indicate occupied pixels*/
	Segment* segment = createNewSegment();
	segment->root = createNewTreeNode(kernel);
	occupied_pixels[kernel[0]][kernel[1]] = TRUE; /*Update the kernel position in the occupied_pixels matrix*/
	segment->size = 1;
	scanNeighborsAux(segment, img, occupied_pixels, segment->root, img->pixels[kernel[0]][kernel[1]], threshold);
	freeOccPix(*img,occupied_pixels);
	return segment;

}

/*Get size of cols and rows, Return ptr to matrix of char - all values set to 0*/
static char** createOccupiedPixelsArr(short cols, short rows) {/*Get size of cols and rows, Return ptr to matrix of char - all values set to 0*/
	unsigned char** table = (char**)malloc(sizeof(char**) * rows);
	allocatingCheck(table);
	int i, k;
	for (i = 0; i < rows; i++) {
		table[i] = (char*)malloc(sizeof(char) * cols);
		allocatingCheck(table[i]);
		for (k = 0; k < cols; k++)
			table[i][k] = FALSE;
	}
	return table;
}
/*Get ptr to grayImage, ptr to occupied_pixels matrix, ptr to treeNode, kernel pixel and threshold. Function check all relevant neighboor and add them into root.similar_neighboor array*/
static void scanNeighborsAux(Segment* seg,grayImage* img, char** occupied_pixels, treeNode* root,short kernel_pixel,char threshold) {
	unsigned short neighboors_size = 0;
	
	imgPos test_pix = { root->position[0] - 1 ,root->position[1] - 1};
	int i,children_size,arr_size;

	for (i = children_size = 0; i < 9; i++, test_pix[1]++) {
		if (!(i % 3) && i != 0) {
			test_pix[0] += 1;
			test_pix[1] -= 3;
		}
		if (isNeighboorRelevant(occupied_pixels,img,test_pix) && isPixelBelongToSegment(img->pixels, kernel_pixel, test_pix, threshold))
			arr_size = addPixelToSegmant(occupied_pixels, root, test_pix, &children_size);
	}
	if (children_size == 0) {
		root->similar_neighbors = NULL;
		return;
	}
	seg->size += children_size; /*Updating Segment size*/
	if (children_size != arr_size - 1) {
	root->similar_neighbors = reallocNeighboorsArr(root->similar_neighbors, children_size + 1, children_size + 1);/*Realloc the array to logic size*/
	root->similar_neighbors[children_size] = NULL;
	}

	for (i = 0;root->similar_neighbors[i];i++) /*Recursive call with roots childrens*/
		scanNeighborsAux(seg,img, occupied_pixels, root->similar_neighbors[i], kernel_pixel, threshold);

}

/*Get image pixel matrix, segments kernel pixel value, imgPos of pixel, and threshold. Return TRUE if ( |kernel - data_to_compare| <= thresold ), else False*/
static char isPixelBelongToSegment(char** pixel, char kernel_pixel, imgPos data_to_compare, unsigned char threshold) {
	char compare = pixel[data_to_compare[0]][data_to_compare[1]];
	if (compare >= (kernel_pixel - threshold) && compare <= (kernel_pixel + threshold))
		return (TRUE);
	return (FALSE);
}
/*Get ptr to occupied_pixels matrix, ptr to treeNode, ptr to imgPos and ptr to children_size. Add the pixel into root similar_neighboors array and update occupied_pixels.*/
static int addPixelToSegmant(char** occupied_pixels, treeNode* root, imgPos *pixel_to_add, int *children_size) {
	static int arr_physic_size;
	occupied_pixels[(*pixel_to_add)[0]][(*pixel_to_add)[1]] = TRUE;
	if (*children_size == 0) {/*Create dynamic array in the first call*/
		arr_physic_size = INITIAL_SIZE;
		root->similar_neighbors = (treeNode**)malloc(sizeof(treeNode*) * INITIAL_SIZE);
     	allocatingCheck(root->similar_neighbors);
	}
	if (*children_size == (arr_physic_size - 1)) {
		root->similar_neighbors = reallocNeighboorsArr(root->similar_neighbors, *children_size, arr_physic_size + 2);
		arr_physic_size += 2;
	}
	root->similar_neighbors[*children_size] = createNewTreeNode(pixel_to_add); /*Creating new node*/
	(*children_size)++;
	root->similar_neighbors[*children_size] = NULL;
	return arr_physic_size;
}

/*Check if allocating success*/
void allocatingCheck(void* ptr) {
	if (!ptr) {
		fprintf(stderr, "Allocating Failed");
		exit(ALLOC_ERROR);
	}
}

/*Get ptr to treeNode* arr and desired new size. Allocating new array and copy data, put in the end of array NULL*/
static treeNode** reallocNeighboorsArr(treeNode** arr, int size,int new_size) {
	treeNode** new_arr = (treeNode**)malloc(sizeof(treeNode*) * (new_size));
	allocatingCheck(new_arr);

	int i;
	for (i = 0; i < size; i++) {
		if (arr[i] != NULL) {
			new_arr[i] = arr[i];
			new_arr[i]->similar_neighbors = arr[i]->similar_neighbors; /*NOT NECCESSARY*/
		}
		else
			new_arr[i] = NULL;
	}
	free(arr);
	return new_arr;
}


/*Get img and ptr to occupied_pixels matrix, Dealloc occupied_pixels matrix*/
static void freeOccPix(grayImage img, char** occ) {
	int i;
	for (i = 0; i < img.rows; i++) 
		free(occ[i]);
	free(occ);
}

/****Q2***/
/*Get ptr to img, threshold, ptr to ////segments///. Create dynamic arr of ptrs to imgPosCell - pointered by segments. Return arr size*/
unsigned int findAllSegments(grayImage* img, unsigned char threshold, imgPosCell*** segments) {
	Pix* sore = createAscendSortedPixelArr(img);
	char** occupied_pixels = createOccupiedPixelsArr(img->cols, img->rows);
	int segments_arr_size = INITIAL_SIZE;

	*segments = (imgPosCell**)malloc(sizeof(imgPosCell*) * segments_arr_size);
	allocatingCheck(segments);
	unsigned int* segmentsSize = (unsigned int*)malloc(sizeof(unsigned int) * segments_arr_size);
	allocatingCheck(segmentsSize);

	int i, write_ind;

	for (i = write_ind = 0; i < (img->rows * img->cols); i++) {
		if (!(occupied_pixels[sore[i].pos[0]][sore[i].pos[1]])) {
			addSegmentToArr(img, segments, &segmentsSize, &write_ind, &segments_arr_size, occupied_pixels, sore[i].pos, threshold);
		}

	}

	/*IF NO SEGMENTS HAVE TO ADD*/
	segments_arr_size = reallocImgPosCell(segments,&segmentsSize, segments_arr_size, write_ind);
	sortSegmentsByDescendingOrder(*segments, segmentsSize, segments_arr_size);
	free(segmentsSize);
	free(sore);
	freeOccPix(*img, occupied_pixels);
	return segments_arr_size;
}
/*Get ptr to img, ptr to segments arr, ptr to segments size arr, ptr to write_index, ptr to physic arr size, occupied_pixels matrix, imgPos kernel of segment, threshold. Scanning segment from kernel, add it to segments arr and update segmentsSize. Function handle arr size and dealloc all not neccessary values*/
static void addSegmentToArr(grayImage* img,imgPosCell*** segments,unsigned int** segmentsSize,int* write_ind,int* arr_size, char** occupied_pixels, imgPos kernel, unsigned char threshold) {
	linkedList* curr_list;
	Segment* curr_seg;
	curr_seg = getSegment(img, occupied_pixels, kernel, threshold); /*FIND SINGLE SEGMENT*/
	curr_list = convertTreeToLinkedList(curr_seg);
	if (*write_ind == *arr_size) 
		*arr_size = reallocImgPosCell(segments, segmentsSize, *arr_size, *arr_size * 2);
	
	(*segments)[*write_ind] = curr_list->head;
	(*segmentsSize)[*write_ind] = curr_seg->size;
	(*write_ind)++;
	free(curr_list);
	freeTreeSegment(curr_seg); /*FREE TREE*/
}
/*Get ptr to img, occupied_pixels matrix, kernel position, threshold. Return ptr to DYNAMIC segment tree*/
static Segment* getSegment(grayImage* img,char** occupied_pixels, imgPos kernel, unsigned char threshold) {
	Segment* segment = createNewSegment();
	segment->root = createNewTreeNode(kernel);
	occupied_pixels[kernel[0]][kernel[1]] = TRUE; /*Update the kernel position in the occupied_pixels matrix*/
	segment->size = 1;
	scanNeighborsAux(segment, img, occupied_pixels, segment->root, img->pixels[kernel[0]][kernel[1]], threshold);
	return segment;
}

/*Get segments arr, segmentsSize arr, and size. Sort segments decsend order by size*/
static void sortSegmentsByDescendingOrder(imgPosCell** segments, int* segmentsSize, unsigned int size) {
	int i, j, k;
	imgPosCell* temp;
	for (i = size - 1; i > 0; i--) {
		for (j = 0; j < i; j++) {
			if (segmentsSize[j] < segmentsSize[i]) {
				temp = segments[j];
				segments[j] = segments[i];
				segments[i] = temp;

				k = segmentsSize[j];
				segmentsSize[j] = segmentsSize[i];
				segmentsSize[i] = k;
			}
		}
	}
}

/*Get imgPos_arr and segmentSize_arr by reference, get current size and new size desired. Function realloc and copy the arrays into new in new_size size. Return new size of arrays*/
static int reallocImgPosCell(imgPosCell*** imgPos_arr, unsigned int** segmentSize_arr, int size, int new_size) {
	imgPosCell** new = (imgPosCell**)malloc(sizeof(imgPosCell*) * new_size);
	allocatingCheck(new);
	unsigned int* size_arr_new = (unsigned int*)malloc(sizeof(unsigned int) * new_size);
	allocatingCheck(size_arr_new);

	int min = size;
	if (new_size < size) /*Increase or decrease the arrays*/
		min = new_size;

	int i;
	for (i = 0; i < min; i++) {
		new[i] = (*imgPos_arr)[i];
		size_arr_new[i] = (*segmentSize_arr)[i];
	}

	free(*segmentSize_arr);
	*segmentSize_arr = size_arr_new;
	free(*imgPos_arr);
	*imgPos_arr = new;
	return new_size;
}
/*Function get ptr to array of treeNodes and size. Sort the array in asending order by col*/
void sortArrByAscendingCol(treeNode** arr, int size) {
	//qsort(arr, size, sizeof(treeNode*), cmpImgPos);

	int i, j;
	for (i = size - 1; i > 0; i--) {
		for (j = 0; j < i; j++) {
			if (arr[j]->position[1] > arr[i]->position[1])
				swapCells(arr, j, i);
			else if (arr[j]->position[1] == arr[i]->position[1]) {
				if (arr[j]->position[0] > arr[i]->position[0])
					swapCells(arr, j, i);
			}
		}
	}
}
/*Get segments_arr and size. Dealloc all segments and eventually dealloc the array*/
void freeSegments(imgPosCell** segments_arr,unsigned int size) {
	int i;
	for (i = 0; i < size; i++) 
		freeList(segments_arr[i]);
	free(segments_arr);
}

/*Get ptr to img, Return Pix array sorted by ascend pixel color*/
static Pix* createAscendSortedPixelArr(grayImage* img) {
	int size_arr = img->rows * img->cols;
	Pix* pixels = (Pix*)malloc(sizeof(Pix) * size_arr);
	allocatingCheck(pixels);

	int i, k, write_ind;
	for (i = write_ind = 0; i < img->rows; i++) {
		for (k = 0; k < img->cols; k++, write_ind++) {
			pixels[write_ind].pos[0] = i;
			pixels[write_ind].pos[1] = k;
			pixels[write_ind].pix = img->pixels[i][k];
		}
	}


	mergeSort(img, pixels, size_arr);
	return pixels;
}
/*Get img, ptr to arr to sort and size. Sorting the array*/
static void mergeSort(grayImage* img, Pix* arr, int size) {
	Pix* sorted_arr;
	int i;

	if (size <= 1)
		return;
	mergeSort(img, arr, size / 2);
	mergeSort(img, arr + (size / 2), (size - (size / 2)));

	sorted_arr = (Pix*)malloc(sizeof(Pix) * size);
	merge(img, sorted_arr, arr, size / 2, arr + (size / 2), size - (size / 2));

	for (i = 0; i < size; i++) 
		copyPix(&(arr[i]), sorted_arr[i]);
	

	free(sorted_arr);
}
/*Merge*/
static void merge(grayImage* img, Pix* dest, Pix* left_arr, unsigned int left_size, Pix* right_arr, unsigned int right_size) {
	int i, j;
	i = j = 0;
	unsigned p1, p2;

	while ((i < left_size) && (j < right_size)) {

		if (left_arr[i].pix < right_arr[j].pix) {
			copyPix(&(dest[i + j]), left_arr[i]);
			i++;
		}
		else {
			copyPix(&(dest[i + j]), right_arr[j]);
			j++;
		}
	}
	while (i < left_size) {
		copyPix(&(dest[i + j]), left_arr[i]);
		i++;
	}
	while (j < right_size) {
		copyPix(&(dest[i + j]), right_arr[j]);
		j++;
	}
}

/*Get ptr to dest pix, and Pix to copy. Update dest Pix data according to source*/
static void copyPix(Pix* dest, Pix source) {
	dest->pos[0] = source.pos[0];
	dest->pos[1] = source.pos[1];
	dest->pix = source.pix;

}

/*Get occupied_pixels matrix, ptr to img, and pos of pix. Return false if pos isnt valid\already belong to segment, else true*/
static char isNeighboorRelevant(char** occupied_pixels, grayImage* img, imgPos pos) {
	int x, y;
	x = pos[0];
	y = pos[1];
	if (x < 0 || x >= img->rows || y < 0 || y >= img->cols)
		return FALSE;
	if (occupied_pixels[x][y])
		return FALSE;
	return TRUE;
}


/**Q3**/
/*segment will be at the same gray level*/
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size) {
	int i;
	unsigned char new_value;
	grayImage* re_img = (grayImage*)malloc(sizeof(grayImage));
	allocatingCheck(re_img);
	re_img = duplicate_size_img(img);  /*returns image with same size as the existing image with all thepixels 0*/
	if (size <= 1) {
		return (re_img);
	}
	else {
		for (i = 0; i < size; i++) {
			new_value = i * (255 / (size - 1));
			change_value(re_img, new_value, segments[i]);
		}
	}
	return re_img;

}
/*takes rows and columns from an existing image and creates a new image in which all pixels are 0*/
grayImage* duplicate_size_img(grayImage* img) {
	unsigned char* si_pix;
	int i, j;
	grayImage* re_img = (grayImage*)malloc(sizeof(grayImage));
	allocatingCheck(re_img);
	re_img->cols = img->cols;
	re_img->rows = img->rows;
	re_img->pixels = (char**)malloc(sizeof(char*) * img->rows);
	for (i = 0; i < img->rows; i++) {
		si_pix = (unsigned char*)malloc(sizeof(unsigned char) * img->cols);/*assigns space to a single pixel*/
		allocatingCheck(si_pix);
		for (j = 0; j < img->cols; j++) {
			si_pix[j] = 0;     /*initializes all pixels to 0*/
		}
		re_img->pixels[i] = si_pix;/*insert the single pixel in place in the array*/
	}
	return (re_img);
}
/*changes the value of the segments in the new image according to the calculation*/
static void change_value(grayImage* img, unsigned char new_value, imgPosCell* single_seg) {
	int i, row, col;
	for (i = 0; single_seg != NULL; i++) {
		row = single_seg->position[0];
		col = single_seg->position[1];
		img->pixels[row][col] = new_value;
		single_seg = single_seg->next;
	}
}


/***Q4**/
/*creates an image based on the PGM file*/
grayImage* createNewImg(int num_row, int num_col, FILE* in_fptr) {
	grayImage* re_img = (grayImage*)malloc(sizeof(grayImage));  /*creates a returned grayImage*/
	allocatingCheck(re_img);
	re_img->cols = num_col;
	re_img->rows = num_row;
	re_img->pixels = (unsigned char**)malloc(sizeof(unsigned char*) * num_row); /*creates a returned grayImage*/
	int index_read, index_row, j,temp;
	unsigned char* si_pix;
	for (index_row = 0; index_row < num_row; index_row++) {
		si_pix = (unsigned char*)malloc(sizeof(unsigned char) * num_col);/*assigns space to a single pixel*/
		allocatingCheck(si_pix);
		for (j = 0; j < num_col; j++) {
			fscanf(in_fptr, "%d", &temp);
			si_pix[j] = temp;
		}
		re_img->pixels[index_row] = si_pix;  /*insert the single pixel in place in the array*/

	}
	return re_img;
}
void freeImg(grayImage* img) {
	int i, j;
	unsigned char* si_pix;
	for (i = 0; i < img->rows; i++) {
		free(img->pixels[i]);
	}
	free(img->pixels);
	free(img);
}


/**Q5***/
/*gets the image and the new maximum gray value and returns an image with limited values*/
grayImage* createReducedImage(grayImage* image, unsigned char reduced_gray_levels, int* bites_per_pixel) {
	int pow_two, i;
	grayImage* re_img = duplicate_size_img(image);
	*bites_per_pixel = pow_two = findPow(reduced_gray_levels);
	int set_of_values = GRAY_LEVEL / (pow(2, pow_two));
	for (i = 0; i < re_img->rows; i++) {
		re_img->pixels[i] = createReducedPixels(image->pixels[i],
			set_of_values, reduced_gray_levels, re_img->cols);
	}
	return re_img;
}
int findPow(unsigned char num) {
	int i = 0;
	unsigned char two = 2;
	if (num == 1) {
		return 1;
	}
	while (two != num) {
		two *= 2;
		i++;
	}
	return (i + 1);
}

/*receives an array of pixels from the old image and returns an array of pixels with a limited gray level*/
unsigned char* createReducedPixels(unsigned char* old_pixels, int set_of_values, unsigned char reduced_gray_levels, int size) {

	unsigned char* compressed_pixels = (unsigned char*)malloc(sizeof(unsigned char) * size);
	allocatingCheck(compressed_pixels);
	int i, j;
	unsigned char comp_value;
	for (i = 0; i < size; i++) {
		comp_value = 0;
		while (old_pixels[i] < set_of_values * comp_value || old_pixels[i] >= set_of_values * (comp_value + 1)) {
			comp_value++;
		}
		compressed_pixels[i] = comp_value;
	}
	return (compressed_pixels);
}
/*finds the power value*/




