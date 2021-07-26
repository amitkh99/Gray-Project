#ifndef ___FILES_H___
#define ___FILES_H___
/****INCLUDES*****/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "image.h"
#include "definitions.h"
/****DEFINES*****/
#define PGM_TYPE_MAX_LENGTH 3
#define BYTE_SIZE 8
#define SET_N_MSB_1(TYPE,N) ((TYPE)~((((unsigned char)~(0)) >> N)))
#define COVERT_N_MSB_TO_INT(BYTE,N) ((SET_N_MSB_1(unsigned char,N) & BYTE) >> (BYTE_SIZE - N))

/****TYPEDEF*****/


/****FUNCTION*****/
static void fileOpenCheck(FILE* fi, char* name);
static void checkFileTypeValid(char* type);
static void skipFileComments(FILE* in_fptr);
grayImage* readPGM(char* fname);

void saveCompressed(char* file_name, grayImage* image, unsigned char reduced_gray_levels);
static void compressBinary(char* file_name, grayImage* img, int bits_per_pix);
static int getImgDefinitions(grayImage* img, FILE* in_ptr);
static grayImage* convertFileToCompressedImg(char* file_name, unsigned char* reduced_max_gray_levels);
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name);
static grayImage* changeReducedImage(grayImage* reduced_image, char reduced_gray_levels);
static unsigned char* createBigPixels(unsigned char* old_pixels, int set_of_values, int size);
static void writeToPGM_File(grayImage* image, char* pgm_file_name);
static void makePGM_File(grayImage* reduced_image, char reduced_gray_levels, char* pgm_file_name);

#endif