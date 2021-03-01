#pragma once
#ifndef CUBARRAY_H
#define CUBARRAY_H

#include "global.h"


typedef void* AUTORHYTHM_CUBIC_ARRAY;

float cubic_interpolation(float v[4], float t);

// create a new cubic array
AUTORHYTHM_CUBIC_ARRAY autorhythm_cubic_array_new();

// add a single value to a cubic array
void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY obj, int position, float value);

// get interpolated value from array
float autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY obj, int position);

// destroy array object
void autorhythm_cubic_array_destroy(AUTORHYTHM_CUBIC_ARRAY obj);

#endif