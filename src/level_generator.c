#include "level_generator.h"

// print to godot's console
void debug_print(const char* out) {
	godot_string str;
	api->godot_string_new(&str);
	api->godot_string_parse_utf8(&str, out);
	api->godot_print(&str);
	api->godot_string_destroy(&str);
}

// currently unused, potentially unsafe
void str_toupper(const char* str) {
	// this is could be the worst thing i have ever written
	while (*str != 0) {
		if ((int)*str >= 96) {
			char buf = (char)toupper((int)*str);
			memcpy(str, &buf, sizeof(char));
		}
		str++;
	}
}

// helper function to make adding things to dictionaries less tedious
void autorhythm_godot_dictionary_set(godot_dictionary* dict, const char* key, godot_variant* value) {
	godot_string gdstring_key;
	godot_variant gdvariant_key;
	api->godot_string_new(&gdstring_key);
	api->godot_string_parse_utf8(&gdstring_key, key);
	api->godot_variant_new_string(&gdvariant_key, &gdstring_key);
	api->godot_string_destroy(&gdstring_key);

	api->godot_dictionary_set(dict, &gdvariant_key, value);
}

// internal level generator function, not exposed to the engine
godot_dictionary autorhythm_generate_level(FMOD_SOUND* snd, AUTORHYTHM_LEVEL_GENERATOR* settings)
{
	// create arrays for storing data
	// multimesh transform array
	godot_pool_real_array onset_transforms;
	api->godot_pool_real_array_new(&onset_transforms);
	
	// base transform array, this will never change as the blocks will never have different sizes
	godot_pool_real_array base_transforms;
	api->godot_pool_real_array_new(&base_transforms);
	// generate a 3x3 identity matrix
	for (int i = 0; i < 9; i++)
		api->godot_pool_real_array_append(&base_transforms, (godot_real)(i % 4 == 0));

	// array of average beats per second at a position [position, value]
	godot_array lane_shape;
	api->godot_array_new(&lane_shape);

	debug_print("created godot arrays\n");

	// setup values for audio analysis
	float sample_rate = 44100;
	int bit_rate = 0;
	uint_t win_s = 512; // window size
	uint_t hop_size = win_s / 4;
	uint_t read = 0;
	uint_t pos = 0;
	int tempOnsets = 0;
	int deltas = sample_rate * 5;

	// tempo cubic array
	AUTORHYTHM_CUBIC_ARRAY tempo = autorhythm_cubic_array_new();
	debug_print("created cubic array\n");

	// get sample rate from audio file
	FMOD_Sound_GetDefaults(snd, &sample_rate, NULL);
	// get bit depth (bits per sample) from audio file, this is required to convert units between samples (aubio) and bytes (fmod)
	FMOD_Sound_GetFormat(snd, NULL, NULL, NULL, &bit_rate);

	// create metadata strings
	godot_array metadata;
	godot_string title;
	godot_string artist;
	api->godot_array_new(&metadata);
	api->godot_string_new(&title);
	api->godot_string_parse_utf8(&title,"Unknown Title"); // in case the file is not tagged
	api->godot_string_new(&artist);
	api->godot_string_parse_utf8(&artist, "Unknown Artist"); // again, in case the file is not tagged
	// read metadata
	FMOD_TAG tag;
	while (FMOD_Sound_GetTag(snd, NULL, -1, &tag) == FMOD_OK)
	{
		//str_toupper(tag.name);
		if (!strcmp(tag.name, "TITLE") || !strcmp(tag.name, "TIT2"))
		{
			api->godot_string_parse_utf8_with_len(&title, tag.data, tag.datalen);
		}
		if (!strcmp(tag.name, "ARITST") || !strcmp(tag.name, "TPE1"))
		{
			api->godot_string_parse_utf8_with_len(&artist, tag.data, tag.datalen);
		}
	}
	// append metadata strings to array
	godot_variant title_variant;
	godot_variant artist_variant;
	api->godot_variant_new_string(&title_variant, &title);
	api->godot_variant_new_string(&artist_variant, &artist);
	api->godot_array_append(&metadata, &title_variant);
	api->godot_array_append(&metadata, &artist_variant);

	// append song length in samples to metadata array
	godot_variant song_length_variant;
	unsigned int len;
	FMOD_Sound_GetLength(snd, &len, FMOD_TIMEUNIT_PCM);
	api->godot_variant_new_int(&song_length_variant, len);
	api->godot_array_append(&metadata, &song_length_variant);

	// destroy temporary godot values
	api->godot_variant_destroy(&title_variant);
	api->godot_variant_destroy(&artist_variant);
	api->godot_string_destroy(&title);
	api->godot_string_destroy(&artist);

	fvec_t* in = new_fvec(hop_size); // input audio buffer

	// create onset object
	aubio_onset_t* o = new_aubio_onset("wphase", win_s, hop_size, sample_rate);
	char buf[32];
	sprintf_s(buf, 32*sizeof(char), "%f", aubio_onset_get_threshold(o));
	debug_print(buf);
	aubio_onset_set_minioi_ms(o, settings->min_interval);
	aubio_onset_set_threshold(o, settings->sensitivity);
	fvec_t* out = new_fvec(2); // output

	// create pitch detection object
	aubio_pitch_t* p = new_aubio_pitch("yin", win_s, hop_size, sample_rate);
	fvec_t* pout = new_fvec(1); // output

	// create tempo detection object
	aubio_tempo_t* t = new_aubio_tempo("default", win_s, hop_size, sample_rate);
	fvec_t* fout = new_fvec(1);

	assert(bit_rate == 16); // ????
	// temporary sound buffer storing audio with integer samples, used to convert audio into floating point samples
	unsigned int* temp_int = (unsigned int*)malloc(sizeof(unsigned int) * hop_size);
	// check that we allocated memory for that buffer
	if (temp_int != NULL) {
		// go back to the beginning of the file, just in case
		FMOD_Sound_SeekData(snd, 0);
		while (1) {
			// read audio data into aubio vector using fmod
			FMOD_Sound_ReadData(snd, temp_int, sizeof(unsigned int) * hop_size, &read);
			for (int i = 0; i < hop_size; i++) // eeee for loop is inefficient but i do not know how to optimise this!!
				fvec_set_sample(in, (temp_int[i] - 2147483647) / 32767.0f, i);

			// execute onset detection
			aubio_onset_do(o, in, out);
			// execute pitch detection
			aubio_pitch_do(p, in, pout);
			// execute tempo detection
			aubio_tempo_do(t, in, fout);
			if (aubio_tempo_get_bpm(t) != 0.f)
				autorhythm_cubic_array_add_value(tempo, pos, aubio_tempo_get_bpm(t));

			// push the onset to onsets array
			if (out->data[0] != 0) {
				tempOnsets++;
				// append base transform for this array
				api->godot_pool_real_array_append_array(&onset_transforms, &base_transforms);
				// x position
				// pick random number for lane if the pitch is too big or small or pick an appropriate lane based on the pitch
				if (pout->data[0] <= 0.f || pout->data[0] > 5000.f || 1) {
					api->godot_pool_real_array_append(&onset_transforms, (godot_real)(rand() % 3) * 2.f - 2.f);
				}
				else {
					api->godot_pool_real_array_append(&onset_transforms, FreqToLane(pout->data[0]));
				}
				// y position (will always be zero!)
				api->godot_pool_real_array_append(&onset_transforms, 0.f);
				// z position
				api->godot_pool_real_array_append(&onset_transforms, (godot_real)aubio_onset_get_last(o) / 500.f);
			}

			// update positions
			pos += hop_size;
			deltas -= hop_size;

			// push onsets per minute if approximately 5 seconds have passed
			if (deltas <= 0) {
				// create godot objects
				godot_variant position;
				godot_variant value;
				godot_variant variant_tmp_lane_shape;
				godot_array tmp_lane_shape;
				api->godot_array_new(&tmp_lane_shape);
				api->godot_array_resize(&tmp_lane_shape, 2);

				// position
				api->godot_variant_new_int(&position, pos);
				api->godot_array_set(&tmp_lane_shape, 0, &position);
				// value
				api->godot_variant_new_real(&value, (double)((tempOnsets / 5.f * 60.f) / autorhythm_cubic_array_get_value(tempo, pos)));
				api->godot_array_set(&tmp_lane_shape, 1, &value);
				// append to the lane shape array
				api->godot_variant_new_array(&variant_tmp_lane_shape, &tmp_lane_shape);
				api->godot_array_append(&lane_shape, &variant_tmp_lane_shape);
				tempOnsets = 0;

				// destroy unused godot objects
				api->godot_variant_destroy(&position);
				api->godot_variant_destroy(&value);
				api->godot_variant_destroy(&variant_tmp_lane_shape);
				api->godot_array_destroy(&tmp_lane_shape);

				// reset timer
				deltas = sample_rate * 5;
			}

			// check if reached end of track
			if (read != sizeof(unsigned int) * hop_size)
				break;
		}
	}

	debug_print("Successfully generated level! Converting to Godot compliant format\n");

	godot_dictionary dict;
	godot_variant tmp_variant_onsets, tmp_variant_lane_shape, tmp_variant_metadata;

	// godot dictionary, will be returned by this function
	api->godot_dictionary_new(&dict);

	// metadata
	// set array in dictionary
	api->godot_variant_new_array(&tmp_variant_metadata, &metadata);
	autorhythm_godot_dictionary_set(&dict, "metadata", &tmp_variant_metadata);

	// onsets
	// set array in dictionary
	api->godot_variant_new_pool_real_array(&tmp_variant_onsets, &onset_transforms);
	autorhythm_godot_dictionary_set(&dict, "onsets", &tmp_variant_onsets);

	// lane shape
	// set array in dictionary
	api->godot_variant_new_array(&tmp_variant_lane_shape, &lane_shape);
	autorhythm_godot_dictionary_set(&dict, "shape", &tmp_variant_lane_shape);

	// destroy all remaining godot values (except the dictionary)
	// variants
	api->godot_variant_destroy(&tmp_variant_onsets);
	api->godot_variant_destroy(&tmp_variant_lane_shape);
	api->godot_variant_destroy(&tmp_variant_metadata);
	// arrays
	api->godot_pool_real_array_destroy(&onset_transforms);
	api->godot_array_destroy(&lane_shape);
	api->godot_array_destroy(&metadata);

	// destroy cubic array
	autorhythm_cubic_array_destroy(tempo);

	// destroy aubio objects
	del_aubio_onset(o);
	del_aubio_tempo(t);
	del_aubio_pitch(p);
	del_fvec(in);
	del_fvec(out);
	del_fvec(pout);
	del_fvec(fout);

	// return godot dictionary containing the lane shape and onsets
	return dict;
}

void* ext_autorhythm_level_generator_new(godot_object* p_instance, void* p_method_data)
{
	AUTORHYTHM_LEVEL_GENERATOR* obj = (AUTORHYTHM_LEVEL_GENERATOR*)api->godot_alloc(sizeof(AUTORHYTHM_LEVEL_GENERATOR));
	// minimum value for this will be 100, maximum value will be 500
	obj->min_interval = 150;
	// minimum value for this will be ?? maximum value will be 0.4 or 0.3 i will decide later
	obj->sensitivity = 0.3;
	return (void*)obj;
}

void ext_autorhythm_level_generator_del(godot_object* p_instance, void* p_method_data, void* p_user_data)
{
	// free the settings struct
	api->godot_free(p_user_data);
}

godot_variant ext_autorhythm_level_generator_settings(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	// check if there is an incorrect number of arguments
	if (p_num_args != 3) {
		api->godot_variant_new_int(&ret, AUTORHYTHM_ARGUMENT_COUNT_ERROR);
	}
	else {
		// cast data pointer
		AUTORHYTHM_LEVEL_GENERATOR* dat = (AUTORHYTHM_LEVEL_GENERATOR*)p_user_data;
		dat->min_interval = api->godot_variant_as_int(p_args[0]);
		dat->sensitivity = api->godot_variant_as_real(p_args[1]);
		dat->balance = api->godot_variant_as_real(p_args[2]);

		// return 0
		api->godot_variant_new_int(&ret, AUTORHYTHM_OK);
	}
	return ret;
}

godot_variant ext_autorhythm_generate_level(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_object* godot_FMODSound;
	AUTORHYTHM_SOUND* sound;
	// get sound from object userdata from object from variant
	godot_FMODSound = api->godot_variant_as_object(p_args[0]);
	sound = (AUTORHYTHM_SOUND*)nativescript_api->godot_nativescript_get_userdata(godot_FMODSound);

	// generate level from sound
	godot_dictionary dict = autorhythm_generate_level(sound->sound, (AUTORHYTHM_LEVEL_GENERATOR*)p_user_data);

	// return value
	godot_variant ret;
	api->godot_variant_new_dictionary(&ret, &dict);
	api->godot_dictionary_destroy(&dict);
	return ret;
}

void autorhythm_register_level_generator(void* p_handle)
{
	// we dont need to do anything in the constructor or destructor so they are just null pointers
	godot_instance_create_func init = { NULL, NULL, NULL };
	init.create_func = &ext_autorhythm_level_generator_new;
	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &ext_autorhythm_level_generator_del;
	// register class with godot
	nativescript_api->godot_nativescript_register_class(p_handle, "LevelGenerator", "Reference", init, destroy);

	// register methods

	godot_instance_method method = { NULL, NULL, NULL };
	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	// Method : generate_level
	// reference to c function 
	method.method = &ext_autorhythm_generate_level;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "LevelGenerator", "generate_level", attributes, method);

	// Method : set_settings
	// reference to c function 
	method.method = &ext_autorhythm_level_generator_settings;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "LevelGenerator", "set_settings", attributes, method);
}
