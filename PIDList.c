#include "PIDList.h"

void init_PIDList(PIDLIST* p) {
	p->head = NULL;
	p->tail = NULL;
	p->size = 0;
}

void add_PIDList(PIDLIST* list, int data) {
	
	PID* new_node = (PID*) malloc(sizeof(PID));
	new_node->processID = data;
	new_node->next = NULL;

	//new_node->next = list->head;


	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
		
		//new_node->next = NULL;
		list->size++;
	}
	else {
		list->tail->next = new_node;
		list->tail = new_node;
		//new_node->next = NULL;
		list->size++;
	}

}

void remove_PIDList(PIDLIST* list, int to_remove) {
	// only remove if the list is not empty;
	PID* curr_node;
	PID* prev_node;
	
	curr_node = list->head;
	
	if (curr_node != NULL) {
		if (curr_node->processID == to_remove) {
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

void free_PIDList(PIDLIST* list) {
	
	if (list->head != NULL) {
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

