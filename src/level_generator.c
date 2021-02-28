#include "level_generator.h"

AUTORHYTHM_LEVEL* autorhythm_generate_level(FMOD_SOUND* snd)
{
	// create arrays for storing data
	// array of onsets [position, lane]
	godot_array onsets;
	api->godot_array_new(&onsets);
	godot_array tmp_onset;
	api->godot_array_new(&tmp_onset);
	// array of average beats per second at a position [position, value]
	godot_array lane_shape;
	api->godot_array_new(&lane_shape);
	godot_array tmp_lane_shape;
	api->godot_array_new(&tmp_lane_shape);

	// setup values for audio analysis
	float sample_rate = 44100;
	int bit_rate = 0;
	uint_t win_s = 512; // window size
	uint_t hop_size = win_s / 4;
	uint_t read = 0;
	uint_t pos = 0;
	int tempOnsets = 0;
	int deltas = sample_rate * 5;

	// get sample rate from audio file
	FMOD_Sound_GetDefaults(snd, &sample_rate, NULL);
	// get bit depth (bits per sample) from audio file, this is required to convert units between samples (aubio) and bytes (fmod)
	FMOD_Sound_GetFormat(snd, NULL, NULL, NULL, &bit_rate);

	fvec_t* in = new_fvec(hop_size); // input audio buffer

	// create onset object
	aubio_onset_t* o = new_aubio_onset("wphase", win_s, hop_size, sample_rate);
	aubio_onset_set_minioi_ms(o, 100); // 100 = MININTERVAL, please replace this with a user controllable variable at some point!
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

			// push the onset to onsets array
			if (out->data[0] != 0) {
				tempOnsets++;
				godot_variant position;
				godot_variant lane;
				godot_variant onset;
				api->godot_variant_new_int(&position, aubio_onset_get_last(o));
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
			}

			// update positions
			pos += hop_size;
			deltas -= hop_size;

			// push onsets per minute if approximately 5 seconds have passed
			if (deltas <= 0) {
				deltas = sample_rate * 5000;
				//intensity_curve[pos] = (tempOnsets * 60.f / 5.f) / tempo.GetValue(pos);
				tempOnsets = 0;
			}

			// check if reached end of track
			if (read != sizeof(unsigned int) * hop_size)
				break;
		}
	}

	return NULL;
}
