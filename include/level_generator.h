#pragma once
#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#define FreqToLane(x) (int)fmin(round(log(0.01 * x + 2.0) - 1.0),2.0)

#include "global.h"

typedef struct AUTORHYTHM_BLOCK {
	int position;
	int lane;
} AUTORHYTHM_BLOCK;

typedef struct AUTORHYTHM_LEVEL {
	size_t blocks_s;
	unsigned int lane_shape_s;
	char SongHash[256];
	AUTORHYTHM_BLOCK* blocks;
	int* lane_shape_position; // storing these separately
	float* lane_shape_value;
} AUTORHYTHM_LEVEL;

AUTORHYTHM_LEVEL* autorhythm_generate_level(FMOD_SOUND* snd);

godot_variant ext_autorhythm_generate_level(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

#endif