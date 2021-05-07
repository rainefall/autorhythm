/*
	C interface to C++ cubic array class
*/

#pragma once
#ifndef CUBARRAY_H
#define CUBARRAY_H

#include "global.h"

typedef void* AUTORHYTHM_CUBIC_ARRAY;

float cubic_interpolation(float v[4], float t);

// internal cubic array
// create a new cubic array
AUTORHYTHM_CUBIC_ARRAY autorhythm_cubic_array_new();

// add a single value to a cubic array
void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY obj, int position, float value);

// get interpolated value from array
float autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY obj, int position);

// destroy array object
void autorhythm_cubic_array_destroy(AUTORHYTHM_CUBIC_ARRAY obj);


// external cubic array
// constructor for cubic array in godot
void* ext_autorhythm_cubic_array_new(godot_object* p_instance, void* p_method_data);

// destructor for cubic array in godot
void ext_autorhythm_cubic_array_del(godot_object* p_instance, void* p_method_data, void* p_user_data);

// add a single value to cubic array from godot
godot_variant ext_autorhythm_cubic_array_add_value(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// get interpolated/extrapolated value from cubic array from godot
godot_variant ext_autorhythm_cubic_array_get_value(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);


// register class with godot
void autorhythm_register_cubic_array(void* p_handle);

#endif