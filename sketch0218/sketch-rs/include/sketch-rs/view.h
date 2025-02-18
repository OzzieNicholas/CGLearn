#pragma once

#include "sketch_rs_api.h"
#include <webgpu.h>

#include <cstdint>

#define SKETCH_KEY_ACTION_PRESS 0
#define SKETCH_KEY_ACTION_RELEASE 1

#ifdef __cplusplus
extern "C"
{
#endif

    using PresentCallbackFn = void (*)(const WGPUTextureDataLayout*, const void*, uint32_t, uint32_t, uint64_t, void*);

    SKETCH_RS_API void set_mouse_button_down_event(float x, float y, int button, bool ctrl, bool alt, bool shift);

    SKETCH_RS_API void set_mouse_button_up_event(float x, float y, int button, bool ctrl, bool alt, bool shift);

    SKETCH_RS_API void set_mouse_move_event(float x, float y, int button, bool ctrl, bool alt, bool shift);

    SKETCH_RS_API void set_resize_surface_event(uint32_t width, uint32_t height);

    SKETCH_RS_API void set_keyboard_event(int key, int action, int mods);

    SKETCH_RS_API void sketch_render_init();

    SKETCH_RS_API void sketch_get_surface_size(uint32_t* width, uint32_t* height);

    SKETCH_RS_API void sketch_set_callback_after_present(PresentCallbackFn fn, void* user_data);

#ifdef __cplusplus
}
#endif
