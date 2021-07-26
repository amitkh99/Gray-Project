#ifndef ___LINKED_LIST_H___
#define ___LINKED_LIST_H___
/****INCLUDES*****/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "image.h"
#include "definitions.h"
/****DEFINES*****/



/****TYPEDEF*****/


/****FUNCTION*****/
static imgPosCell* createNewCell(imgPos position);
static linkedList* createNewList();
static void addCellToList(linkedList* list, imgPosCell* cell_to_add, imgPosCell* prev);
linkedList* convertTreeToLinkedList(Segment* seg);
int printList(grayImage* img, imgPosCell* cell);
void swapCells(treeNode** arr, int a, int b);
void freeList(imgPosCell* cell);

#endif