#pragma once
#ifndef ID3V2_IMAGE_EXTRACT_H
#define ID3V2_IMAGE_EXTRACT_H

#include "global.h"

typedef struct ID3V2_HEADER {
	// this should always be "ID3"
	char ID3[3];
	// version number
	uint8_t major_version;
	uint8_t minor_version;
	// flags
	uint8_t unsynchronisation : 1;
	uint8_t extended_header : 1;
	uint8_t experimental : 1;
	uint8_t unused_d : 1;
	uint8_t unused_e : 1;
	uint8_t unused_f : 1;
	uint8_t unused_g : 1;
	uint8_t unused_h : 1;
	// size
	uint8_t size_a : 7;
} ID3V2_HEADER;

char* autorhythm_id3v2_image_extract(const char* file_path);

// create new id3v2 image extract object in godot
void* ext_autorhythm_image_extract_new(godot_object* p_instance, void* p_method_data);
// destructor for id3v2 image extract object in godot
void ext_autorhythm_image_extract_del(godot_object* p_instance, void* p_method_data, void* p_user_data);

// extract image from mp3 file with id3v2 tags, from godot
godot_variant ext_autorhythm_image_extract(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

#endif