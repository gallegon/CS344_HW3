#include "DynamicArray.h"
#include <stdio.h>

int main() {
	DYNARR* arr = (DYNARR*) malloc(sizeof(DYNARR));
	init_dynamic_array(arr, 4);
	printf("Array Capacity: %d\nArray items: %d\n", arr->capacity, arr->count);
	add_array(arr, "sandwich");
	add_array(arr, "money");
	add_array(arr, "array");
	add_array(arr, "array");
	
	for (int i = 0; i < arr->count; ++i) {
		printf("%s\n", arr->strings[i]);
	}
	printf("Array Capacity: %d\nArray items: %d\n", arr->capacity, arr->count);
	add_array(arr, "milk");
	add_array(arr, "sandwich");
	add_array(arr, "sandwich");
	add_array(arr, "money");
	add_array(arr, "array");
	add_array(arr, "array");
	add_array(arr, "money");
	add_array(arr, "array");
	add_array(arr, "array");
	
	for (int i = 0; i < arr->count; ++i) {
		printf("%s\n", arr->strings[i]);
	}
	printf("Array Capacity: %d\nArray items: %d\n", arr->capacity, arr->count);
	free_dynamic_array(arr);
	free(arr);
	return 0;
}
