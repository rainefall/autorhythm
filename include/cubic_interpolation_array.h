#pragma once
#ifndef CUBARRAY_H
#define CUBARRAY_H

typedef struct AUTORHYTHM_CUBIC_ARRAY {
	int* positions;
	void* data;
	size_t used;
	size_t size;
} AUTORHYTHM_CUBIC_ARRAY;

// create a new cubic array
void autorhythm_cubic_array_new(AUTORHYTHM_CUBIC_ARRAY* obj, size_t initial_size);

// add a single value to a cubic array
void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY* obj, int* position, )

#endif