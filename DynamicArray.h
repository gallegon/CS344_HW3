#ifndef __DYNAMIC_ARRAY_H
#define __DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <string.h>

#define _INITIAL_SIZE 16

typedef struct DynamicArray DYNARR;

struct DynamicArray {
    char** strings;
    int count;
    int capacity;
};

void init_dynamic_array(struct DynamicArray*, int);

void _resize_array(struct DynamicArray*);

void add_array(struct DynamicArray*, char*);

void remove_array(struct DynamicArray*, int);

void free_dynamic_array(struct DynamicArray*);

#endif
