#include "sketch/STCore.h"

#include "Engine/Util/SysUtil.h"
#include "Engine/Util/WinUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Common/EventPool.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Interface/IF_App.h"
#include "sketch/View/Controller/Handle/HandleProxy.h"
#include "sketch/View/ControllerManager.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/IRender.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
#include <Engine/Util/StrUtil.h>
#include <Windows.h>

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC void appMessageLoop() { STCore::inst->messageLoop(); }

extern "C" CORE_DLL_DECLSPEC void disposeApp() { STCore::inst->dispose(); }

extern "C" CORE_DLL_DECLSPEC void quitApp() { STCore::inst->quit(); }

extern "C" CORE_DLL_DECLSPEC void appUpdateAndRender()
{
    STCore::inst->update();
    STCore::inst->onViewRender();
}

extern "C" CORE_DLL_DECLSPEC void appUpdate() { STCore::inst->update(); }

extern "C" CORE_DLL_DECLSPEC void appRender() { STCore::inst->onViewRender(); }

extern "C" CORE_DLL_DECLSPEC void onCanvasResize(int width, int height) { STCore::inst->onViewResize(width, height); }

extern "C" CORE_DLL_DECLSPEC void showModalDialog(void* ptr)
{
    HWND hwnd = (HWND)ptr;

    ShowWindow(hwnd, SW_SHOW);
    SetCapture(hwnd);

    MSG msg;
    while(true)
    {
        if(!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            continue;

        if(msg.message == WM_QUIT)
        {
            PostMessage(NULL, WM_QUIT, 0, 0);
            break;
        }

        // if (msg.message != WM_MOUSEMOVE && msg.message != WM_NCMOUSEMOVE)
        //	Log::print("%d\n", msg.message);

        switch(msg.message)
        {
        case WM_NCMOUSEMOVE:
        case WM_NCMOUSEHOVER:
        case WM_NCMOUSELEAVE:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
            if(hwnd != msg.hwnd && !IsChild(hwnd, msg.hwnd))
                continue;
        default: break;
        }

        if(msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
        {
            if(hwnd != msg.hwnd && !IsChild(hwnd, msg.hwnd))
                continue;
        }

        if(!IsDialogMessage(hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ReleaseCapture();
}

extern "C" CORE_DLL_DECLSPEC long long STCoreFrame() { return STCore::inst->frame(); }

extern "C" CORE_DLL_DECLSPEC HANDLE mainHandle() { return STCore::inst->mainHandle(); }

extern "C" CORE_DLL_DECLSPEC RECT screenWorkAreaRelativeToPrimary(void* ptr) { return WinUtil::screenWork((HWND)ptr); }

extern "C" CORE_DLL_DECLSPEC void setProgress(const std::wstring& progName, float curProgress) { STCore::inst->setUIProgress(progName, curProgress); }

void sketch_on_view_key_down(int key, bool bCtrl, bool bAlt, bool bShift) { STCore::inst->onViewKeyDown(key, bCtrl, bAlt, bShift); }

void sketch_on_view_mouse_down(int button, bool bCtrl, bool bAlt, bool bShift) { STCore::inst->onViewMouseDown(button, bCtrl, bAlt, bShift); }
void sketch_on_view_mouse_move(int button, bool bCtrl, bool bAlt, bool bShift) { STCore::inst->onViewMouseMove(button, bCtrl, bAlt, bShift); }
void sketch_on_view_mouse_up(int button, bool bCtrl, bool bAlt, bool bShift) { STCore::inst->onViewMouseUp(button, bCtrl, bAlt, bShift); }
void sketch_on_view_mouse_double_click(int button, bool bCtrl, bool bAlt, bool bShift)
{
    STCore::inst->onViewMouseDoubleClick(button, bCtrl, bAlt, bShift);
}
void sketch_on_view_mouse_wheel(int button, int delta, bool bCtrl, bool bAlt, bool bShift)
{
    STCore::inst->onViewMouseWheel(button, delta, bCtrl, bAlt, bShift);
}

void sketch_set_mouse_pos(int x, int y) { STCore::inst->setMousePos(x, y); }

void sketch_send_view_event(const std::string& event, const std::string& params_json)
{
    SketchSerializer serializer;
    if(!serializer.from_string(params_json.c_str()))
    {
        return;
    }
    SketchParamMap params;
    params.load(serializer);

    if(event == "delete")
    {
        sketch_delete_select();
    }
    else
    {
        if(const auto ctrl_manager = STCore::inst->ctrlMgr())
        {
            ctrl_manager->event_filter(event, params);
        }
    }
}

void sketch_register_callback(const std::string& event, std::function<void(const std::string&)> func)
{
    EventPool::instance()->subscribe(event, [func](const SketchParamMap& param_map) {
        SketchSerializer serializer;
        param_map.save(serializer);
        func(serializer.to_string().c_str());
    });
}
void sketch_unregister_callback(const std::string& event) { EventPool::instance()->unsubscribe(event); }

void sketch_init() { STCore::inst->init_standalone(); }

void sketch_tick() { STCore::inst->tick(); }

void sketch_set_render(void* render) { STCore::inst->set_render(static_cast<sketch::IRender*>(render)); }

void sketch_view_resize(int width, int height) { STCore::inst->onViewResize(width, height); }

void sketch_undo_focused_view()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    auto pCell = pViewMgr->layout()->focusedCell();
    if(!pCell)
        return;

    switch(pCell->cellType())
    {
    case Define::CELL_VIEW: STCore::inst->cmdMgr()->undo(((View*)pCell)->scene()->doc()->rvtId()); break;
    }
}

void sketch_redo_focused_view()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    auto pCell = pViewMgr->layout()->focusedCell();
    if(!pCell)
        return;

    switch(pCell->cellType())
    {
    case Define::CELL_VIEW: STCore::inst->cmdMgr()->redo(((View*)pCell)->scene()->doc()->rvtId()); break;
    default: break;
    }
}

void sketch_close_group()
{
    sketch::View* view = STCore::inst->viewMgr()->layout()->focusedView();
    TFObjectDoc* current_group = view->working_group_area();
    if(current_group)
    {
        view->selector()->clear();
        view->set_working_group_area(current_group->parent());
    }
}

void sketch_create_group()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->create_group_from_select_elemnt();
}

void sketch_disperse_group()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->disperse_group_from_select();
}

void sketch_weld_lines()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->weld_lines_from_select();
}

void sketch_disperse_weld_line()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->disperse_weld_line_from_select();
}

void sketch_soft_lines()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->soft_polygon_from_select();
}

void sketch_disperse_smooth_group()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->disperse_smooth_group_from_select();
}

void sketch_convert_to_geodata()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->convert_to_geodata_from_select();
}

CORE_DLL_DECLSPEC void sketch_delete_select()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    pView->delete_obj_from_select();
}

void sketch_use_internal_gizmo(bool use) { STCore::inst->use_internal_gizmo(use); }

bool sketch_is_gizmo_enabled()
{
    if(auto handle = HandleProxy::current_handle())
    {
        return handle->is_enabled();
    }
    return false;
}

void sketch_set_gizmo_position(float x, float y, float z)
{
    if(auto handle = HandleProxy::current_handle())
    {
        handle->set_gizmo_position({ x, y, z });
    }
}

void sketch_set_gizmo_manipulate_position(float x, float y, float z)
{
    if(auto handle = HandleProxy::current_handle())
    {
        handle->set_manipulate_position({ x, y, z });
    }
}

void sketch_set_gizmo_manipulate_rotation(float x, float y, float z, float w)
{
    if(auto handle = HandleProxy::current_handle())
    {
        handle->set_manipulate_rotation({ x, y, z, w });
    }
}

void sketch_set_gizmo_manipulate_scale(float x, float y, float z)
{
    if(auto handle = HandleProxy::current_handle())
    {
        handle->set_manipulate_scale({ x, y, z });
    }
}

std::array<float, 3> sketch_get_gizmo_position()
{
    if(auto handle = HandleProxy::current_handle())
    {
        auto position = handle->get_gizmo_position();
        return { position[0], position[1], position[2] };
    }
    return { { 0, 0, 0 } };
}

void sketch_clear_clipboard() {}

void sketch_paste_clipboard() {}

void sketch_copy_select_to_clipboard() {}

void sketch_escape(const std::string& params_json)
{
    if(const auto ctrl_manager = STCore::inst->ctrlMgr())
    {
        SketchParamMap params;
        SketchSerializer serializer;
        if(serializer.from_string(params_json.c_str()))
        {
            params.load(serializer);
        }
        ctrl_manager->escape(params);
    }
}

void sketch_enter(const std::string& params_json)
{
    if(const auto ctrl_manager = STCore::inst->ctrlMgr())
    {
        SketchParamMap params;
        SketchSerializer serializer;
        if(serializer.from_string(params_json.c_str()))
        {
            params.load(serializer);
        }
        ctrl_manager->enter(params);
    }
}

void sketch_active_controller(const std::wstring& progName)
{
    if(const auto ctrl_manager = STCore::inst->ctrlMgr())
    {
        ctrl_manager->active_controller(progName, SketchParamMap());
    }
}

void sketch_deactive_current_controller()
{
    if(const auto ctrl_manager = STCore::inst->ctrlMgr())
    {
        ctrl_manager->deactive_current_controller();
    }
}

static std::string sketch_obj_list = "";
const char* sketch_get_obj_list()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    SketchSerializer serializer;
    pView->serialize_view_list(&serializer);
    sketch_obj_list = serializer.to_string().c_str();
    return sketch_obj_list.c_str();
}

void sketch_attach_reference_triangle(const std::vector<float>& triangles, bool point, bool line, bool face)
{
    sketch::View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->inferHelper()->attach_reference_triangle(triangles, point, line, face);
    return;
}

void sketch_clear_reference_triangle()
{
    sketch::View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->inferHelper()->clear_reference_triangle();
    return;
}

CORE_DLL_DECLSPEC void sketch_set_gizmo_drag_type(unsigned int gizmoType)
{
    if(auto handle = HandleProxy::current_handle())
    {
        handle->set_gizmo_drag_type(gizmoType);
    }
}
