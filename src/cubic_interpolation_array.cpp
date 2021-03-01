#include "cubic_interpolation_array.hpp"

float cubic_interpolation(float v[4], float t)
{
	return v[1] + 0.5f * t * (v[2] - v[0] + t * (2.f * v[0] - 5.f * v[1] + 4.f * v[2] - v[3] + t * (3.f * (v[1] - v[2]) + v[3] - v[0])));
}

AUTORHYTHM_CUBIC_ARRAY autorhythm_cubic_array_new()
{
	AutoRhythm_CppCubicArray* obj = (AutoRhythm_CppCubicArray*)malloc(sizeof(AutoRhythm_CppCubicArray));
	obj->arr = new CubicArray<int, float>();
	return (AUTORHYTHM_CUBIC_ARRAY)obj;
}

void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY obj, int position, float value)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray* cppobj = (AutoRhythm_CppCubicArray*)obj;
	cppobj->arr->AddValue(position, value);
}

float autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY obj, int position)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray* cppobj = (AutoRhythm_CppCubicArray*)obj;
	// get value from cubicarray and return it
	return cppobj->arr->GetValue(position);
}

void autorhythm_cubic_array_destroy(AUTORHYTHM_CUBIC_ARRAY obj)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray* cppobj = (AutoRhythm_CppCubicArray*)obj;
	// delete cubicarray
	delete cppobj->arr;
	// free struct
	free(cppobj);
}
