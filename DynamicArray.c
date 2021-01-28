#include "DynamicArray.h"

#include <stdlib.h>
#include <string.h>

void init_dynamic_array(struct DynamicArray* d) {
    d->array = (int*) malloc(sizeof(int) * 20);
    d->count = 0;

    memset(d->array, 0, 20);

    d->array_size = 20;
}

void resize_array(struct DynamicArray* d) {
    int new_size = d->array_size + 10;
    int* temp = (int*) malloc(sizeof(int) * new_size);

    memset(temp, 0, new_size);

    for (int i = 0; i < d->array_size; ++i) {
        temp[i] = d->array[i];
    }

    free(d->array);

    d->array = (int*) malloc(sizeof(int) * new_size);
    d->array_size = new_size;

    for (int i = 0; i < new_size; ++i) {
        d->array[i] = temp[i];
    }

    free(temp);
}

void add_array(struct DynamicArray* d, int n) {
    if (d->array_size == d->count) {
        resize_array(d);
    }

    d->array[d->count] = n;
    d->count++;
}

void remove_array(struct DynamicArray* d, int n) {
    for (int i = 0; i < d->count; ++i) {
        if (d->array[i] == n && i == 0 && d->count == 1) {
            d->array[0] = 0;
            d->count--;
        }
        else if (d->array[i] == n) {
            d->array[i] = d->array[(d->count) - 1];
            d->array[(d->count) - 1] = 0;
            d->count--;
        }
    }
}

void free_dynamic_array(struct DynamicArray* d) {
    free(d->array);
}