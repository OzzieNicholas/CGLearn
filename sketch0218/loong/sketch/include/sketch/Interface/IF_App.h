#pragma once

#include "../Core.h"
#include <Windows.h>
#include <functional>
#include <string>

extern "C"
{
    CORE_DLL_DECLSPEC long long STCoreFrame();
    CORE_DLL_DECLSPEC HANDLE mainHandle();
    CORE_DLL_DECLSPEC RECT screenWorkAreaRelativeToPrimary(void* ptr);
    CORE_DLL_DECLSPEC void setProgress(const std::wstring& progName, float curProgress);

    CORE_DLL_DECLSPEC void sketch_on_view_key_down(int key, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_on_view_mouse_down(int button, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_on_view_mouse_move(int button, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_on_view_mouse_up(int button, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_on_view_mouse_double_click(int button, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_on_view_mouse_wheel(int button, int delta, bool bCtrl, bool bAlt, bool bShift);
    CORE_DLL_DECLSPEC void sketch_set_mouse_pos(int x, int y);
    CORE_DLL_DECLSPEC void sketch_send_view_event(const std::string& event, const std::string& params_json);
    CORE_DLL_DECLSPEC void sketch_register_callback(const std::string& event, std::function<void(const std::string&)> func);
    CORE_DLL_DECLSPEC void sketch_unregister_callback(const std::string& event);

    CORE_DLL_DECLSPEC void sketch_init();
    CORE_DLL_DECLSPEC void sketch_tick();
    CORE_DLL_DECLSPEC void sketch_set_render(void* render);
    CORE_DLL_DECLSPEC void sketch_view_resize(int width, int height);
    CORE_DLL_DECLSPEC void sketch_undo_focused_view();
    CORE_DLL_DECLSPEC void sketch_redo_focused_view();
    CORE_DLL_DECLSPEC void sketch_close_group();
    CORE_DLL_DECLSPEC void sketch_create_group();
    CORE_DLL_DECLSPEC void sketch_disperse_group();
    CORE_DLL_DECLSPEC void sketch_weld_lines();
    CORE_DLL_DECLSPEC void sketch_disperse_weld_line();
    CORE_DLL_DECLSPEC void sketch_soft_lines();
    CORE_DLL_DECLSPEC void sketch_disperse_smooth_group();
    CORE_DLL_DECLSPEC void sketch_convert_to_geodata();
    CORE_DLL_DECLSPEC void sketch_delete_select();

    CORE_DLL_DECLSPEC void sketch_use_internal_gizmo(bool use);
    CORE_DLL_DECLSPEC bool sketch_is_gizmo_enabled();
    CORE_DLL_DECLSPEC void sketch_set_gizmo_position(float x, float y, float z);
    CORE_DLL_DECLSPEC void sketch_set_gizmo_manipulate_position(float x, float y, float z);
    CORE_DLL_DECLSPEC void sketch_set_gizmo_manipulate_rotation(float x, float y, float z, float w);
    CORE_DLL_DECLSPEC void sketch_set_gizmo_manipulate_scale(float x, float y, float z);

    CORE_DLL_DECLSPEC void sketch_clear_clipboard();
    CORE_DLL_DECLSPEC void sketch_paste_clipboard();
    CORE_DLL_DECLSPEC void sketch_copy_select_to_clipboard();
    CORE_DLL_DECLSPEC void sketch_escape(const std::string& params_json);
    CORE_DLL_DECLSPEC void sketch_enter(const std::string& params_json);

    CORE_DLL_DECLSPEC void sketch_active_controller(const std::wstring& progName);
    CORE_DLL_DECLSPEC void sketch_deactive_current_controller();

    CORE_DLL_DECLSPEC const char* sketch_get_obj_list();
    CORE_DLL_DECLSPEC void sketch_attach_reference_triangle(const std::vector<float>& triangles, bool point, bool line, bool face);
    CORE_DLL_DECLSPEC void sketch_clear_reference_triangle();
}

CORE_DLL_DECLSPEC std::array<float, 3> sketch_get_gizmo_position();
CORE_DLL_DECLSPEC void sketch_set_gizmo_drag_type(unsigned int gizmoType);