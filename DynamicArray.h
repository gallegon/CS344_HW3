#ifndef __DYNAMIC_ARRAY_H
#define __DYNAMIC_ARRAY_H

typedef struct DynamicArray DYNARR;

struct DynamicArray {
    int* array;
    int count;
    int array_size;
};

void init_dynamic_array(struct DynamicArray*);

void resize_array(struct DynamicArray*);

void add_array(struct DynamicArray*, int);

void remove_array(struct DynamicArray*, int);

void free_dynamic_array(struct DynamicArray*);

#endif