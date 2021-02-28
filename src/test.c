#include "test.h"

// This code is adapted from the official Godot Docs section on GDNative in C
// https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-c-example.html

void* autorhythm_test_constructor(godot_object* p_instance, void* p_method_data) {
	autorhythm_test_data_struct* dat = api->godot_alloc(sizeof(autorhythm_test_data_struct));
	strcpy(dat->data, "This is a test string to test sending data from a C GDNative extension to the Godot engine. Cool, huh?");

	return (void*)dat;
}

void autorhythm_test_destructor(godot_object* p_instance, void* p_method_data, void* p_user_data) {
	api->godot_free(p_user_data);
}

godot_variant autorhythm_test_get_data(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args) {
    godot_string data;
    godot_variant ret;
    autorhythm_test_data_struct* user_data = (autorhythm_test_data_struct*)p_user_data;

    api->godot_string_new(&data);
    api->godot_string_parse_utf8(&data, user_data->data);
    api->godot_variant_new_string(&ret, &data);
    api->godot_string_destroy(&data);

    return ret;
}