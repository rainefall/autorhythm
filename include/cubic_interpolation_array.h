#pragma once
#ifndef CUBARRAY_H
#define CUBARRAY_H

#include "global.h"

typedef union AUTORHYTHM_NUM {
	int32_t _int;
	uint32_t unsigned_int;
	float _float;
} AUTORHYTHM_NUM;

typedef struct AUTORHYTHM_CUBIC_ARRAY {
	AUTORHYTHM_NUM* positions;
	AUTORHYTHM_NUM* data;
	size_t used;
	size_t size;
	uint8_t position_mode : 2;
	uint8_t data_mode : 2;
} AUTORHYTHM_CUBIC_ARRAY;

// create a new cubic array (*_mode represents int uint or float, 0 to 2 respectively)
AUTORHYTHM_CUBIC_ARRAY* autorhythm_cubic_array_new(size_t initial_size, uint8_t position_mode, uint8_t data_mode);

// add a single value to a cubic array
void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY* obj, AUTORHYTHM_NUM* position, AUTORHYTHM_NUM* value);

// get interpolated value from array
AUTORHYTHM_NUM autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY* obj, AUTORHYTHM_NUM* position);

#endif