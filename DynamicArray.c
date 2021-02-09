/* Name: Nicholai Gallegos
 * File: DynamicArray.h
 * Date: 2/8/2021
 */
#include "DynamicArray.h"

/* Name: init_dynamic_array
 * Desc: initializes a dynamic array struct with an array
 * of n = size char* pointers
 */ 
void init_dynamic_array(DYNARR* d, int size) {
	d->strings = (char**) malloc(sizeof(char*) * size);
	d->count = 0;

	for (int i = 0; i < size; ++i) {
		d->strings[i] = NULL;
	}
	
	d->capacity = size;
}

/* Name: add_array
 * Desc: adds a string str to the string array for a DYNARR struct
 */
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

/* Name _resize_array
 * Desc: resizes the dynamic array to be 10 larger than previous.  This is 
 * used when the array reaches capacity 
 * */
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

/* Name: free_dynamic_array
 * Desc: frees the string array within the DYNARR struct 
 */
void free_dynamic_array(DYNARR* d) {
	for (int i = 0; i < d->capacity; ++i) {
		free(d->strings[i]);
		d->strings[i] = NULL;
	}
	
	free(d->strings);

}

