#ifndef ___IMAGE_H___
#define ___IMAGE_H___
/****INCLUDES*****/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "definitions.h"


/****DEFINES*****/
#define MAX_NEIGHBORS 8
#define GRAY_LEVEL 256

/****TYPEDEF*****/
typedef unsigned short imgPos[2];

typedef struct _treeNode {
	imgPos position;
	struct _treeNode** similar_neighbors;
}treeNode;

typedef struct _grayImage {
	unsigned short rows, cols;
	unsigned char **pixels;
}grayImage;

typedef struct _segment
{
	treeNode *root;
	unsigned int size;
}Segment;

/*NOT HERE*/
typedef struct _imgPosCell {
	imgPos position;
	struct _imgPosCell* next, * prev;
}imgPosCell;
typedef struct _linkedList {
	imgPosCell* head;
}linkedList;

/***CHANGES***/
typedef struct _pix {
	imgPos pos;
	unsigned char pix;
}Pix;


/****FUNCTION*****/
Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold);
static int addPixelToSegmant(char** occupied_pixels, treeNode* root, imgPos* pixel_to_add, int *children_size);
static char** createOccupiedPixelsArr(short cols, short rows);
static void scanNeighborsAux(Segment* seg,grayImage* img, char** occupied_pixels, treeNode* root, short kernel_pixel, char threshold);
static char isPixelBelongToSegment(char** pixel, char kernel_pixel, imgPos data_to_compare, unsigned char threshold);
static treeNode** reallocNeighboorsArr(treeNode** arr, int size, int new_size);
static void freeOccPix(grayImage img, char** occ);

void printOcc(char** occ);
void allocatingCheck(void* ptr);


/**Q2**/
unsigned int findAllSegments(grayImage* img, unsigned char threshold, imgPosCell*** segments);
static void addSegmentToArr(grayImage* img, imgPosCell*** segments, unsigned int** segmentsSize, int* write_ind, int* arr_size, char** occupied_pixels, imgPos kernel, unsigned char threshold);
static Segment* getSegment(grayImage* img, char** occupied_pixels, imgPos kernel, unsigned char threshold);
static void sortSegmentsByDescendingOrder(imgPosCell** segments, int* segmentsSize, unsigned int size);
static int reallocImgPosCell(imgPosCell*** imgPos_arr, unsigned int** segmentSize_arr, int size, int new_size);
void sortArrByAscendingCol(treeNode** arr, int size);
void freeSegments(imgPosCell** segments_arr, unsigned int size);
static void copyPix(Pix* dest, Pix source);
static void merge(grayImage* img, Pix* dest, Pix* left_arr, unsigned int left_size, Pix* right_arr, unsigned int right_size);
static void mergeSort(grayImage* img, Pix* arr, int size);
static Pix* createAscendSortedPixelArr(grayImage* img);
static char isNeighboorRelevant(char** occupied_pixels, grayImage* img, imgPos pos);

/**Q3**/
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size);
grayImage* duplicate_size_img(grayImage* img);
static void change_value(grayImage* img, unsigned char new_value, imgPosCell* single_seg);

/**Q4**/
void freeImg(grayImage* img);
grayImage* createNewImg(int num_row, int num_col, FILE* in_fptr);

/**Q5**/
grayImage* createReducedImage(grayImage* image, unsigned char reduced_gray_levels, int* bites_per_pixel);
static unsigned char* createReducedPixels(unsigned char* old_pixels, int set_of_values, unsigned char reduced_gray_levels, int size);
int findPow(unsigned char num);


#endif