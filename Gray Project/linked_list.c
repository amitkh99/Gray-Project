#include "linked_list.h"
#include "tree.h"

/*Get imgPos and return ptr to dynamic imgPosCell set with imgPos data*/
static imgPosCell* createNewCell(imgPos position) {
	imgPosCell* cell = (imgPosCell*)malloc(sizeof(imgPosCell));
	allocatingCheck(cell);
	cell->position[0] = position[0];
	cell->position[1] = position[1];
	cell->prev = cell->next = NULL;
	return cell;

}

/*Return ptr to dynamic linkedList, head set to NULL*/
static linkedList* createNewList() {
	linkedList* list = (linkedList*)malloc(sizeof(linkedList));
	allocatingCheck(list);
	list->head = NULL;
	return list;
}
/*Get ptr to list, ptr to cell_to_add, ptr to desired prev cell. Add cell_to_add after prev cell. if prev == NULL - function set cell_to_add as head of list*/
static void addCellToList(linkedList* list,imgPosCell* cell_to_add, imgPosCell* prev) {
	if (!list->head) /*If list is empty*/
		list->head = cell_to_add;
	else if (!prev) { /*Set the cell as head*/
		cell_to_add->next = list->head;
		list->head = cell_to_add;
	}
	else {
		cell_to_add->prev = prev;
		cell_to_add->next = prev->next;
		prev->next = cell_to_add;
		if (cell_to_add->next) /*Set next cell prev to cell to add if exsict*/
			cell_to_add->next->prev = cell_to_add;
	}

}

/*Get ptr to Segment, Return a sorted linked list in asending order by col witch contain all Segments pixels.*/
linkedList* convertTreeToLinkedList(Segment* seg) {
	treeNode** arr = createSegmentPtrArr(seg);
	sortArrByAscendingCol(arr,seg->size);
	linkedList* lst = createNewList();
	int i;
	imgPosCell *curr,*prev = NULL;
	for (i = 0; i < seg->size; i++) {
		curr = createNewCell(arr[i]->position);
		addCellToList(lst, curr, prev);
		prev = curr;
	}
	return lst;
}


/*Get ptr to array of treeNodes, index of two wanted swap nodes. Function swap between nodes*/
void swapCells(treeNode** arr, int a, int b) {
	treeNode* temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
	
}


int printList(grayImage* img,imgPosCell* cell) {
	imgPosCell* curr = cell;
	int x, y,i = 0;
	while (curr) {
		x = curr->position[0];
		y = curr->position[1];
		printf(" *(%d,%d)-%d* ", x, y, img->pixels[x][y]);
		curr = curr->next;
		i++;
	}
	return i;
}

/*Get head cell of the list and dealloc the list*/
void freeList(imgPosCell* cell) {
	imgPosCell* prev;
	while (cell) {
		prev = cell;
		cell = cell->next;
		free(prev);
	}
}


