#pragma once

// c standard library
#include <wchar.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

// aubio [audio analysis library]
// 2003-2018 | aubio team
#include <aubio/aubio.h>

// FMOD [audio middleware]
// 1995-2021 | Firelight Technologies
#include <fmod.h>

// GDNative [Godot Engine C API]
// 2007-2021 | Juan Linietsky, Ariel Manzur and Godot contributors 
#include <gdnative_api_struct.gen.h> // GDNative API struct header
#include <gdnative/string.h> // godot string

// libCRC [checksum functions]
// 2016(?)-2021 | Lammert Bies
#include "libcrc/checksum.h"

// cubic interpolation array, used to smoothen detected values from aubio
#include "cubic_interpolation_array.h"

// functions to test GDNative
// adapted from godot gdnative tutorials, so it is not included in the final code listing as code that i wrote
#include "test.h"

// fmod sound object
#include "fmod_sound.h"

// level generator
#include "level_generator.h"

// gdnative api objects
const godot_gdnative_core_api_struct* api;
const godot_gdnative_ext_nativescript_api_struct* nativescript_api;
// fmod root object
const FMOD_SYSTEM* fmod_system;

// error codes
enum AUTORHYTHM_ERROR_CODES {
	AUTORHYTHM_OK,
	AUTORHYTHM_VALUE_ERROR,
	AUTORHYTHM_TYPE_ERROR,
	AUTORHYTHM_ARGUMENT_COUNT_ERROR
};