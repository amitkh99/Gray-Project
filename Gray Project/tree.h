#ifndef ___TREE_H___
#define ___TREE_H___
/****INCLUDES*****/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "image.h"
#include "definitions.h"
/****DEFINES*****/


/****TYPEDEF*****/


/****FUNCTION*****/
Segment* createNewSegment();
treeNode* createNewTreeNode(imgPos position);
void freeTreeSegment(Segment* seg);
static void freeTreeNodesAux(treeNode* root);
treeNode** createSegmentPtrArr(Segment* seg);
static void createSegmentPtrArrAux(treeNode* root, treeNode** arr, int* write_index);


#endif