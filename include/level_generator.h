#pragma once
#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#define FreqToLane(x) (int)fmin(round(log(0.01 * x + 2.0) - 1.0),2.0)

#include "global.h"

typedef struct AUTORHYTHM_LEVEL_GENERATOR {
	int min_interval;
} AUTORHYTHM_LEVEL_GENERATOR;

// internal level generator function, not exposed to the engine
godot_dictionary autorhythm_generate_level(FMOD_SOUND* snd);

// constructor for autorhythm level generator in godot
void* ext_autorhythm_level_generator_new(godot_object* p_instance, void* p_method_data);
// destructor for autorhythm level generator in godot
void ext_autorhythm_level_generator_del(godot_object* p_instance, void* p_method_data, void* p_user_data);

// generate level from godot fmodsound object
godot_variant ext_autorhythm_generate_level(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// register this very small class with godot
void autorhythm_register_level_generator(void* p_handle);

#endif