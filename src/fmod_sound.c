#include "fmod_sound.h"

#include <stdio.h>

uint32_t fmod_sound_hash(FMOD_SOUND* sound)
{
	// load entire fmod sound into memory
	unsigned int len;
	FMOD_Sound_GetLength(sound, &len, FMOD_TIMEUNIT_PCMBYTES);
	void* buf = (void*)malloc(len);
	// run libcrc's crc32 on that buffer
	uint32_t hash = crc_32(buf, len);
	// delete the buffer
	free(buf);
	// return the resulting uint32
	return hash;
}

void* ext_fmod_sound_new(godot_object* p_instance, void* p_method_data) {
	AUTORHYTHM_SOUND* sound_dat = api->godot_alloc(sizeof(AUTORHYTHM_SOUND));
	// we can't really do much with the sound right now due to the lack of constructor arguments
	// so we just return the empty sound struct
	return (void*)sound_dat;
}

void ext_fmod_sound_del(godot_object* p_instance, void* p_method_data, void* p_user_data)
{
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;
	// free the FMOD_SOUND object if applicable
	if (sound_dat->sound != NULL)
		FMOD_Sound_Release(sound_dat->sound);
	// now we can free the memory allocated for the sound struct
	api->godot_free(p_user_data);
}

godot_variant ext_fmod_sound_load(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// godot API means i have to convert the godot variant in the arguments to a godot string
	godot_string godot_string_path = api->godot_variant_as_string(p_args[0]);
	// then convert that godot string to a godot char string, which can be used to get a C string
	godot_char_string path = api->godot_string_ascii(&godot_string_path);

	// create sound with fmod
	FMOD_RESULT result = FMOD_System_CreateSound(fmod_system, api->godot_char_string_get_data(&path), FMOD_CREATESTREAM | FMOD_OPENONLY /*| FMOD_MPEGSEARCH*/ , NULL, &sound_dat->sound);

	// return fmod result
	api->godot_variant_new_int(&ret, (int64_t)result);
	return ret;
}

godot_variant ext_fmod_sound_play(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// check if a sound has been loaded into this object
	// this is as simple as checking if the sound parameter of sound_dat is null
	if (sound_dat->sound != NULL) {

		// play sound with fmod
		FMOD_RESULT result = FMOD_System_PlaySound(fmod_system, sound_dat->sound, NULL, false, &sound_dat->channel);
		// set position to zero
		FMOD_Channel_SetPosition(sound_dat->channel, 0, FMOD_TIMEUNIT_PCM);
		// set return value to be the fmod result returned by FMOD_System_PlaySound
		api->godot_variant_new_int(&ret, (int)result);
	}
	else {
		// return 1, which is equivalent to FMOD_ERR_BADCOMMAND
		api->godot_variant_new_int(&ret, 1);
	}
	return ret;
}

godot_variant ext_fmod_sound_pause(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// check if a channel object is present in the sound struct
	// (there should be no way to call this function without having a channel object but just in case)
	if (sound_dat->channel != NULL) {
		// pause fmod channel
		FMOD_RESULT result = FMOD_Channel_SetPaused(sound_dat->channel, true);
		// set return value to be the fmod result returned by FMOD_Channel_SetPaused
		api->godot_variant_new_int(&ret, (int)result);
	}
	else {
		// return 1, which is equivalent to FMOD_ERR_BADCOMMAND
		api->godot_variant_new_int(&ret, 1);
	}
	// return fmod result
	return ret;
}

godot_variant ext_fmod_sound_unpause(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// check if a channel object is present in the sound struct
	// (there should be no way to call this function without having a channel object but just in case)
	if (sound_dat->channel != NULL) {
		// pause fmod channel
		FMOD_RESULT result = FMOD_Channel_SetPaused(sound_dat->channel, false);
		// set return value to be the fmod result returned by FMOD_Channel_SetPaused
		api->godot_variant_new_int(&ret, (int)result);
	}
	else {
		// return 1, which is equivalent to FMOD_ERR_BADCOMMAND
		api->godot_variant_new_int(&ret, 1);
	}
	// return fmod result
	return ret;
}

godot_variant ext_fmod_sound_stop(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// check if a channel object is present in the sound struct
	// (there should be no way to call this function without having a channel object but just in case)
	if (sound_dat->channel != NULL) {
		// stop fmod channel
		FMOD_RESULT result = FMOD_Channel_Stop(sound_dat->channel);
		// set return value to be the fmod result returned by FMOD_Channel_Stop
		api->godot_variant_new_int(&ret, (int)result);
	}
	else {
		// return 1, which is equivalent to FMOD_ERR_BADCOMMAND
		api->godot_variant_new_int(&ret, 1);
	}
	// return fmod result
	return ret;
}

godot_variant ext_fmod_sound_channel_position(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound_dat = (AUTORHYTHM_SOUND*)p_user_data;

	// get position from fmod channel
	unsigned int pos = -1;
	FMOD_Channel_GetPosition(sound_dat->channel, &pos, FMOD_TIMEUNIT_PCM);
	api->godot_variant_new_int(&ret, (int64_t)pos);
	
	return ret;
}

godot_variant ext_fmod_sound_hash(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	AUTORHYTHM_SOUND* sound;
	sound = (AUTORHYTHM_SOUND*)p_user_data;
	// get hash of object
	api->godot_variant_new_int(&ret, fmod_sound_hash(sound->sound));
	// return it
	return ret;
}

void autorhythm_register_fmod_sound(void* p_handle)
{
	// references to constructor & destructor
	// somehow this block of code worked with the wrong create and delete functions attached?
	godot_instance_create_func init = { NULL, NULL, NULL };
	init.create_func = &ext_fmod_sound_new;
	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &ext_fmod_sound_del;
	// register class with godot
	nativescript_api->godot_nativescript_register_class(p_handle, "FMODSound", "Reference", init, destroy);

	// register methods

	godot_instance_method method = { NULL, NULL, NULL };
	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	// Method : create
	// reference to c function 
	method.method = &ext_fmod_sound_load;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "create", attributes, method);

	// Method : play
	// reference to c function 
	method.method = &ext_fmod_sound_play;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "play", attributes, method);

	// Method : pause
	// reference to c function 
	method.method = &ext_fmod_sound_pause;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "pause", attributes, method);

	// Method : unpause
	// reference to c function 
	method.method = &ext_fmod_sound_unpause;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "unpause", attributes, method);

	// Method : stop
	// reference to c function 
	method.method = &ext_fmod_sound_stop;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "stop", attributes, method);

	// Method : channel_position
	// reference to c function
	method.method = &ext_fmod_sound_channel_position;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "channel_position", attributes, method);

	// Method : hash
	// reference to c function
	method.method = &ext_fmod_sound_hash;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "FMODSound", "hash", attributes, method);
}
