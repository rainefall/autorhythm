#include "level_generator.h"

void debug_print(const char* out) {
	godot_string str;
	api->godot_string_new(&str);
	api->godot_string_parse_utf8(&str, out);
	api->godot_print(&str);
	api->godot_string_destroy(&str);
}

godot_dictionary autorhythm_generate_level(FMOD_SOUND* snd, AUTORHYTHM_LEVEL_GENERATOR* settings)
{
	// create arrays for storing data
	// array of onsets [position, lane]
	godot_array onsets;
	api->godot_array_new(&onsets);
	godot_array tmp_onset;
	api->godot_array_new(&tmp_onset);
	api->godot_array_resize(&tmp_onset, 2);
	// array of average beats per second at a position [position, value]
	godot_array lane_shape;
	api->godot_array_new(&lane_shape);
	godot_array tmp_lane_shape;
	api->godot_array_new(&tmp_lane_shape);
	api->godot_array_resize(&tmp_lane_shape, 2);

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

	fvec_t* in = new_fvec(hop_size); // input audio buffer

	// create onset object
	aubio_onset_t* o = new_aubio_onset("wphase", win_s, hop_size, sample_rate);
	aubio_onset_set_minioi_ms(o, settings->min_interval);
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
				godot_variant position;
				godot_variant lane;
				godot_variant onset;
				api->godot_variant_new_int(&position, (long)aubio_onset_get_last(o));
				// ^ everything above this WORKS ^

				api->godot_array_set(&tmp_onset, 0, &position);
				// pick random number for lane if the pitch is too big or small or pick an appropriate lane based on the pitch
				if (pout->data[0] <= 0.f || pout->data[0] > 5000.f) {
					api->godot_variant_new_int(&lane, rand() % 3);
				}
				else {
					api->godot_variant_new_int(&lane, FreqToLane(pout->data[0]));
				}
				api->godot_array_set(&tmp_onset, 1, &lane);
				api->godot_variant_new_array(&onset, &tmp_onset);
				// append to the onsets array
				api->godot_array_append(&onsets, &onset);

				// destroy unused godot variants
				api->godot_variant_destroy(&position);
				api->godot_variant_destroy(&lane);
				api->godot_variant_destroy(&onset);
			}

			// update positions
			pos += hop_size;
			deltas -= hop_size;

			// push onsets per minute if approximately 5 seconds have passed
			if (deltas <= 0) {
				debug_print("send help\n");
				deltas = sample_rate * 5000;
				godot_variant position;
				godot_variant value;
				godot_variant variant_tmp_lane_shape;

				// position
				api->godot_variant_new_int(&position, pos);
				api->godot_array_set(&tmp_lane_shape, 0, &position);
				// value
				api->godot_variant_new_real(&value, (double)((tempOnsets * 60.f / 5.f) / autorhythm_cubic_array_get_value(tempo, pos)));
				api->godot_array_set(&tmp_lane_shape, 1, &value);
				// append to the lane shape array
				api->godot_variant_new_array(&variant_tmp_lane_shape, &tmp_onset);
				api->godot_array_append(&onsets, &variant_tmp_lane_shape);
				tempOnsets = 0;

				// destroy unused godot variants
				api->godot_variant_destroy(&position);
				api->godot_variant_destroy(&value);
				api->godot_variant_destroy(&variant_tmp_lane_shape);
			}

			// check if reached end of track
			if (read != sizeof(unsigned int) * hop_size)
				break;
		}
	}

	debug_print("Successfully generated level! Converting to Godot compliant format\n");

	godot_dictionary dict;
	godot_string tmp_string_key_onsets, tmp_string_key_lane_shape;
	godot_variant tmp_variant_key_onsets, tmp_variant_key_lane_shape, tmp_variant_onsets, tmp_variant_lane_shape;

	// godot dictionary, will be returned by this function
	api->godot_dictionary_new(&dict);

	// onsets
	// create string for key
	api->godot_string_new(&tmp_string_key_onsets);
	api->godot_string_parse_utf8(&tmp_string_key_onsets, "onsets");
	api->godot_variant_new_string(&tmp_variant_key_onsets, &tmp_string_key_onsets);
	api->godot_string_destroy(&tmp_string_key_onsets);
	// set array in dictionary
	api->godot_variant_new_array(&tmp_variant_onsets, &onsets);
	api->godot_dictionary_set(&dict, &tmp_variant_key_onsets, &tmp_variant_onsets);

	// lane shape
	// create string for key
	api->godot_string_new(&tmp_string_key_lane_shape);
	api->godot_string_parse_utf8(&tmp_string_key_lane_shape, "shape");
	api->godot_variant_new_string(&tmp_variant_key_lane_shape, &tmp_string_key_lane_shape);
	api->godot_string_destroy(&tmp_string_key_lane_shape);
	// set array in dictionary
	api->godot_variant_new_array(&tmp_variant_lane_shape, &lane_shape);
	api->godot_dictionary_set(&dict, &tmp_variant_key_lane_shape, &tmp_variant_lane_shape);

	// destroy all remaining godot values (except the dictionary)
	// variants
	api->godot_variant_destroy(&tmp_variant_onsets);
	api->godot_variant_destroy(&tmp_variant_key_onsets);
	api->godot_variant_destroy(&tmp_variant_lane_shape);
	api->godot_variant_destroy(&tmp_variant_key_lane_shape);
	// arrays
	api->godot_array_destroy(&onsets);
	api->godot_array_destroy(&tmp_onset);
	api->godot_array_destroy(&lane_shape);
	api->godot_array_destroy(&tmp_lane_shape);

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
	obj->min_interval = 100;
	return (void*)obj;
}

void ext_autorhythm_level_generator_del(godot_object* p_instance, void* p_method_data, void* p_user_data)
{
	api->godot_free(p_user_data);
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
	// register test method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "LevelGenerator", "generate_level", attributes, method);
}
