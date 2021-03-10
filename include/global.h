#pragma once

// c standard library
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

// aubio [audio analysis library]
// 2003-2018 | aubio team
#include <aubio/aubio.h>

// FMOD [audio middleware]
// 1995-2021 | Firelight Technologies
#include <fmod.h>

// Godot Engine headers
// 2007-2021 | Juan Linietsky, Ariel Manzur and Godot contributors 
#include <gdnative_api_struct.gen.h> // GDNative API struct header
#include <gdnative/string.h> // godot string

// cubic interpolation array, used to smoothen detected values from aubio
#include "cubic_interpolation_array.h"

// functions to test GDNative
#include "test.h"

// fmod sound object
#include "fmod_sound.h"

// level generator
#include "level_generator.h"

const godot_gdnative_core_api_struct* api;
const godot_gdnative_ext_nativescript_api_struct* nativescript_api;

const FMOD_SYSTEM* fmod_system;