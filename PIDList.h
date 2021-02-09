/* File: PIDList.h
 * Author: Nicholai Gallegos
 * Date: 2/8/2021
 * Desc: Struct definition for PID and PIDList.  Function headers for
 * PIDList.c
 */
#ifndef __PID_LIST_H
#define __PID_LIST_H

/* Name: Nicholai Gallegos
 * File: PIDList.h
 * Date: 2/8/2021
 */
#include <stdlib.h>

/* struct definitions, type definitions, function prototypes for
 * PIDLIST and PID types */
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
