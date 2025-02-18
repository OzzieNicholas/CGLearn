#pragma once

#include "sketch_rs_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    SKETCH_RS_API void sketch_core_init();

    SKETCH_RS_API void sketch_core_tick();

    SKETCH_RS_API void sketch_core_create_project(const wchar_t* path, const wchar_t* env);

    SKETCH_RS_API void sketch_core_active_controller(const wchar_t* name);

#ifdef __cplusplus
}
#endif
