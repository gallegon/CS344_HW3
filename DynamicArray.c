#include "DynamicArray.h"

/*
void init_dynamic_array(struct DynamicArray* d, int size) {
    //d->array = (int*) malloc(sizeof(int) * 20);
	d->strings = (char**) malloc(sizeof(char*) * size);
	d->count = 0;

	memset(d->array, 0, 20);

	d->capacity = 20;
}
*/

void init_dynamic_array(DYNARR* d, int size) {
	d->strings = (char**) malloc(sizeof(char*) * size);
	d->count = 0;

	for (int i = 0; i < size; ++i) {
		d->strings[i] = NULL;
	}
	
	d->capacity = size;
}

void add_array(DYNARR* d, char* str) {
	//resize array if full
	if (d->capacity == d->count) {
		_resize_array(d);
	}
	
	// allocate memory at end of array for a string the same size as str
	d->strings[d->count] = (char*) malloc(sizeof(char) * (strlen(str) + 1));
	
	//copy string to end of array
	strcpy(d->strings[d->count], str);
	d->count++;
}

void _resize_array(DYNARR* d) {
	int new_size = d->capacity + 10;
	int old_size = d->count;
	
	char** temp = (char**) malloc(sizeof(char*) * old_size);
	

	for (int i = 0; i < old_size; ++i) {
		temp[i] = (char*) malloc(sizeof(char) * (strlen(d->strings[i]) + 1));
		strcpy(temp[i], d->strings[i]);
	}

	//free strings in array and re-initialize with new size
	free_dynamic_array(d);
	init_dynamic_array(d, new_size);

	//copy array back into re-sized array and free strings in temp
	for (int i = 0; i < old_size; ++i) {
		add_array(d, temp[i]);
		free(temp[i]);
	}

	//free temp pointer
	free(temp);
	temp = NULL;
}

void free_dynamic_array(DYNARR* d) {
	for (int i = 0; i < d->capacity; ++i) {
		free(d->strings[i]);
		d->strings[i] = NULL;
	}
	
	free(d->strings);

}
/*
void resize_array(struct DynamicArray* d) {
	int new_size = d->capacity + 10;
	int* temp = (int*) malloc(sizeof(int) * new_size);

	memset(temp, 0, new_size);

   for (int i = 0; i < d->capacity; ++i) {
       temp[i] = d->array[i];
    }

    free(d->array);

    d->array = (int*) malloc(sizeof(int) * new_size);
    d->capacity = new_size;

    for (int i = 0; i < new_size; ++i) {
        d->array[i] = temp[i];
    }

    free(temp);
}


void add_array(struct DynamicArray* d, int n) {
    if (d->capacity == d->count) {
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
*/

