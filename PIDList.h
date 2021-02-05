#ifndef __PID_LIST_H
#define __PID_LIST_H

#include <stdlib.h>

typedef struct PIDList PIDLIST;
typedef struct PID PID;

struct PID {
	int processID;
	PID* next;
};

struct PIDList {
	PID* head;
	PID* tail;
	int size;
};

void init_PIDList(PIDLIST*);
void add_PIDList(PIDLIST*, int);
void remove_PIDList(PIDLIST*, int);
void free_PIDList(PIDLIST*);


#endif
