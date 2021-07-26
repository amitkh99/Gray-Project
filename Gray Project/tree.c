#include "tree.h"

/*Return ptr to new Segment*/
Segment* createNewSegment() {
	Segment* seg = (Segment*)malloc(sizeof(Segment));
	allocatingCheck(seg);
	seg->root = NULL;
	seg->size = 0;
	return seg;

}
/*Get imgPos, Return ptr to dynamic treeNode fill with data*/
treeNode* createNewTreeNode(imgPos position) {
	static int node_created = 0;
	treeNode* node = (treeNode*)malloc(sizeof(treeNode));
	node_created++;
	allocatingCheck(node);
	node->position[0] = position[0];
	node->position[1] = position[1];
	node->similar_neighbors = NULL;
	return node;
}
/*Get ptr to segment. Dealloc all nodes and segment*/
void freeTreeSegment(Segment* seg) {
	freeTreeNodesAux(seg->root);
	free(seg);
}
/*Recursive function. Get ptr to node, Dealloc inorder method*/
static void freeTreeNodesAux(treeNode* root) {
	int i;
	for (i = 0; root->similar_neighbors && root->similar_neighbors[i]; i++) {
		if (root->similar_neighbors[i]->similar_neighbors)
			freeTreeNodesAux(root->similar_neighbors[i]);
		else
			free(root->similar_neighbors[i]);
	}
	free(root->similar_neighbors);
	free(root);
}
/*Get ptr to Segment, Using Aux function eventually return array of ptr to treeNode contain all Segments pixels*/
treeNode** createSegmentPtrArr(Segment* seg) {
	treeNode** arr = (treeNode**)malloc(sizeof(treeNode*) * seg->size);
	allocatingCheck(arr);
	arr[0] = seg->root;
	int write_index = 1;
	if (seg->size > 1)
		createSegmentPtrArrAux(seg->root, arr, &write_index);
	return arr;
}
/*Recursive function, get ptr to treeNode, ptr to array of treeNodes, ptr to write_index. Function insert all roots children into the array. And make recursive call to childrens who have childrens. (write_index update after every insertion to the array)*/
static void createSegmentPtrArrAux(treeNode* root, treeNode** arr, int* write_index) {
	int i;
	for (i = 0; root->similar_neighbors[i]; i++, (*write_index)++)
		arr[*write_index] = root->similar_neighbors[i];
	for (i = 0; root->similar_neighbors[i]; i++) {
		if (root->similar_neighbors[i]->similar_neighbors)
			createSegmentPtrArrAux(root->similar_neighbors[i], arr, write_index);
	}
}
