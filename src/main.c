#include "global.h"

// initialize GDNative
void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* p_options) {
    api = p_options->api_struct;

    // Now find our extensions.
    for (int i = 0; i < api->num_extensions; i++) {
        switch (api->extensions[i]->type) {
        case GDNATIVE_EXT_NATIVESCRIPT: {
            nativescript_api = (godot_gdnative_ext_nativescript_api_struct*)api->extensions[i];
        }; break;
        default: break;
        }
    }

    // now we can initialise FMOD
    FMOD_System_Create(&fmod_system);
    FMOD_RESULT result = FMOD_System_Init(fmod_system, 512, FMOD_INIT_NORMAL, NULL);
    printf("FMOD Init result: %d\n", result);
}

// terminate GDNative
void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* p_options) {
    api = NULL;
    nativescript_api = NULL;

    FMOD_System_Release(fmod_system);
}

// tell Godot what functions we have
void GDN_EXPORT godot_nativescript_init(void* p_handle) {
    // Class : test class, not used in game

    // create reference to test constructor function
    godot_instance_create_func create = { NULL, NULL, NULL };
    create.create_func = &ext_autorhythm_test_new;
    // create reference to test destructor function
    godot_instance_destroy_func destroy = { NULL, NULL, NULL };
    destroy.destroy_func = &ext_autorhythm_test_del;
    // register test class with godot
    nativescript_api->godot_nativescript_register_class(p_handle, "Test", "Reference", create, destroy);

    // create reference to test class function
    godot_instance_method get_data = { NULL, NULL, NULL };
    get_data.method = &ext_autorhythm_test_get_data;
    // set some attribute that i don't understand but the godot manual told me to do this so
    godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };
    // register test method with godot
    nativescript_api->godot_nativescript_register_method(p_handle, "Test", "get_data", attributes, get_data);

    // Class : FMODSound
    autorhythm_register_fmod_sound(p_handle);
    // Class : LevelGenerator
    autorhythm_register_level_generator(p_handle);
}