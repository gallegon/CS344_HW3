/* File: PIDList.c
 * Name: Nicholai Gallegos
 * Date: 2/8/2021
 */
#include "PIDList.h"

/* Name: init_PIDList
 * Desc: initializes a linked list of integers struct PIDList
 */
void init_PIDList(PIDLIST* p) {
	p->head = NULL;
	p->tail = NULL;
	p->size = 0;
}

/* Name: add_PIDList
 * Desc: adds a new struct PID node to a PIDList linked list
 */
void add_PIDList(PIDLIST* list, int data) {
	/* allocate new node */
	PID* new_node = (PID*) malloc(sizeof(PID));
	new_node->processID = data;
	new_node->next = NULL;
	/* check if the list is empty, if so make the new node the head */
	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
		list->size++;
	}
	/* if not empty add it to the end of the list */
	else {
		list->tail->next = new_node;
		list->tail = new_node;
		list->size++;
	}

}

/* Name: remove_PIDList
 * Desc: removes a struct PID node containing the integer to_remove if
 * such a node exists.
 */
void remove_PIDList(PIDLIST* list, int to_remove) {
	PID* curr_node;
	PID* prev_node; /* to insert a node */
	
	/* set pointer to walk along the list */
	curr_node = list->head;
	
	if (curr_node != NULL) {
		/* check if the node to remove is the head of the list */
		if (curr_node->processID == to_remove) {
			/* if the node is the head of the list, check to see if it is the
			 * only node in the list, then free accordingly */
			if (curr_node->next != NULL) {
				list->head = curr_node->next;
				free(curr_node);
				list->size--;
				return;
			}
			else {
				free(curr_node);
				list->head = NULL;
				list->size--;
				return;
			}	
		}
		
		/* walk along list and keep track of the previous node if to_remove
		 * is not found in the current node */
		while (curr_node != NULL && curr_node->processID != to_remove) {
			prev_node = curr_node;
			curr_node = curr_node->next;
		}
		if (curr_node != NULL) {
			prev_node->next = curr_node->next;
			free(curr_node);
			list->size--;
		}
	}
}

/* Name: free_PIDList
 * Desc: frees linked list and alloc'd PID nodes
 */ 
void free_PIDList(PIDLIST* list) {
	/* check if the list if empty */
	if (list->head != NULL) {
		/* if not empty, free the node */
		PID* curr_node = list->head;
		PID* next_node;
		while (curr_node != NULL) {
			next_node = curr_node->next;
			free(curr_node);
			curr_node = next_node;
		}
		list->head = NULL;
		list->tail = NULL;
	}
}

