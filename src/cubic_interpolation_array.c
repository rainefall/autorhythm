#include "cubic_interpolation_array.h"

AUTORHYTHM_CUBIC_ARRAY* autorhythm_cubic_array_new(size_t initial_size, uint8_t position_mode, uint8_t data_mode)
{
	AUTORHYTHM_CUBIC_ARRAY* obj = (AUTORHYTHM_CUBIC_ARRAY*)malloc(sizeof(AUTORHYTHM_CUBIC_ARRAY));
	obj->position_mode = position_mode;
	obj->data_mode = data_mode;
	obj->size = initial_size;
	obj->used = 0;

	// i give up. just port the C++ version over and make a C wrapper around it

	return obj
}

void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY* obj, AUTORHYTHM_NUM* position, AUTORHYTHM_NUM* value)
{
}

AUTORHYTHM_NUM autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY* obj, AUTORHYTHM_NUM* position)
{
	AUTORHYTHM_NUM ret;
	ret.unsigned_int = 0u;

	return ret;
}
