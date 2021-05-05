#include "cubic_interpolation_array.hpp"

float cubic_interpolation(float v[4], float t)
{
	return v[1] + 0.5f * t * (v[2] - v[0] + t * (2.f * v[0] - 5.f * v[1] + 4.f * v[2] - v[3] + t * (3.f * (v[1] - v[2]) + v[3] - v[0])));
}


// these procedures are for the INTERNAL cubic array object (e.g. for the level generator)

AUTORHYTHM_CUBIC_ARRAY autorhythm_cubic_array_new()
{
	// create new cubic array object and return it as a void pointer
	AutoRhythm_CppCubicArray<int, float>* obj = new AutoRhythm_CppCubicArray<int, float>();
	return (AUTORHYTHM_CUBIC_ARRAY)obj;
}

void autorhythm_cubic_array_add_value(AUTORHYTHM_CUBIC_ARRAY obj, int position, float value)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)obj;
	// add value
	cppobj->AddValue(position, value);
}

float autorhythm_cubic_array_get_value(AUTORHYTHM_CUBIC_ARRAY obj, int position)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)obj;
	// get value from cubicarray and return it
	return cppobj->GetValue(position);
}

void autorhythm_cubic_array_destroy(AUTORHYTHM_CUBIC_ARRAY obj)
{
	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)obj;
	// delete cubicarray
	delete cppobj;
}


// these procedures are for the EXTERNAL cubic array object (for use within Godot)

void* ext_autorhythm_cubic_array_new(godot_object* p_instance, void* p_method_data)
{
	// return new cubic array as userdata
	AutoRhythm_CppCubicArray<int, float>* obj = new AutoRhythm_CppCubicArray<int, float>();
	return (void*)obj;
}

void ext_autorhythm_cubic_array_del(godot_object* p_instance, void* p_method_data, void* p_user_data)
{
	// destroy internal cubic array object
	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)p_user_data;
	// delete cubicarray
	delete cppobj;
}

godot_variant ext_autorhythm_cubic_array_add_value(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	api->godot_variant_new_int(&ret, 0); // return value doesnt matter

	// get position from arguments
	int position = 0;
	position = api->godot_variant_as_int(p_args[0]);
	// get value from arguments
	float value = 0.f;
	value = (float)api->godot_variant_as_real(p_args[1]);

	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)p_user_data;
	// add value
	cppobj->AddValue(position, value);

	// we dont actually need to return anything but godot doesnt let you not return anything
	return ret;
}

godot_variant ext_autorhythm_cubic_array_get_value(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
	godot_variant ret;
	// get position from arguments
	int position = 0;
	position = api->godot_variant_as_int(p_args[0]);

	// get object from void pointer
	AutoRhythm_CppCubicArray<int, float>* cppobj = (AutoRhythm_CppCubicArray<int, float>*)p_user_data;
	// get value from cubicarray and return it
	api->godot_variant_new_real(&ret, cppobj->GetValue(position));
	return ret;
}

void autorhythm_register_cubic_array(void* p_handle)
{
	// references to constructor & destructor
	godot_instance_create_func init = { NULL, NULL, NULL };
	init.create_func = &ext_autorhythm_cubic_array_new;
	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &ext_autorhythm_cubic_array_del;
	// register class with godot
	nativescript_api->godot_nativescript_register_class(p_handle, "CubicArray", "Reference", init, destroy);

	// register methods

	godot_instance_method method = { NULL, NULL, NULL };
	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	// Method : add_value
	// reference to c function 
	method.method = &ext_autorhythm_cubic_array_add_value;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "CubicArray", "add_value", attributes, method);

	// Method : add_value
	// reference to c function 
	method.method = &ext_autorhythm_cubic_array_get_value;
	// register method with godot
	nativescript_api->godot_nativescript_register_method(p_handle, "CubicArray", "get_value", attributes, method);
}
