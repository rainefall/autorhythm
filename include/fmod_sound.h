/*
	Wrapper from FMOD Core to Godot
*/

#pragma once
#ifndef FMOD_AUDIO_H
#define FMOD_AUDIO_H

#include "global.h"

// structure holding a pointer to an FMOD sound object as well as an FMOD channel object (if the sound is being played)
typedef struct AUTORHYTHM_SOUND {
	FMOD_SOUND* sound;
	FMOD_CHANNEL* channel;
} AUTORHYTHM_SOUND;

// returns a hash value for an FMOD sound
uint32_t fmod_sound_hash(FMOD_SOUND* sound);

// constructor for autorhythm fmod sound in godot
void* ext_fmod_sound_new(godot_object* p_instance, void* p_method_data);
// destructor for autorhythm fmod sound in godot
void ext_fmod_sound_del(godot_object* p_instance, void* p_method_data, void* p_user_data);

// load fmod sound (this has to be done separate from the constructor due to limitations of GDNative)
godot_variant ext_fmod_sound_load(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// play fmod sound
godot_variant ext_fmod_sound_play(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
// pause fmod sound
godot_variant ext_fmod_sound_pause(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
// unpause fmod sound
godot_variant ext_fmod_sound_unpause(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
// stop fmod sound
godot_variant ext_fmod_sound_stop(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// returns the current position in pcm samples
godot_variant ext_fmod_sound_channel_position(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// returns a hash value for a *GODOT* FMOD sound object TO GODOT
godot_variant ext_fmod_sound_hash(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

// register class with godot
void autorhythm_register_fmod_sound(void* p_handle);

#endif