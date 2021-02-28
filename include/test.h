// This code is adapted from the official Godot Docs section on GDNative in C
// https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-c-example.html

#pragma once
#ifndef TEST_H
#define TEST_H

#include "global.h"

typedef struct autorhythm_test_data_struct {
    char data[256];
} autorhythm_test_data_struct;

void* autorhythm_test_constructor(godot_object* p_instance, void* p_method_data);
void autorhythm_test_destructor(godot_object* p_instance, void* p_method_data, void* p_user_data);
godot_variant autorhythm_test_get_data(godot_object* p_instance, void* p_method_data,
    void* p_user_data, int p_num_args, godot_variant** p_args);

#endif