#include "sketch-rs/view.h"
#include "sketch-render/adapter/sketch_render.h"
#include "sketch-render/streaming/memory_streaming.h"
#include "sketch/STCore.h"
#include "sketch/View/ViewManager.h"
#include "sketch/log.h"
#include <memory>
#include <stringzilla/stringzilla.hpp>

static std::unique_ptr<sketch::IRender> m_render = nullptr;
static sketch::render::MemoryStreaming* m_surface = nullptr;

void set_mouse_button_down_event(float x, float y, int button, bool ctrl, bool alt, bool shift)
{
    sketch::ViewManager* view_mgr = sketch::STCore::inst->viewMgr();
    view_mgr->onMouseDown(button, x, y, ctrl, alt, shift);
}

void set_mouse_button_up_event(float x, float y, int button, bool ctrl, bool alt, bool shift)
{
    sketch::ViewManager* view_mgr = sketch::STCore::inst->viewMgr();
    view_mgr->onMouseUp(button, x, y, ctrl, alt, shift);
}

void set_mouse_move_event(float x, float y, int button, bool ctrl, bool alt, bool shift)
{
    sketch::ViewManager* view_mgr = sketch::STCore::inst->viewMgr();
    view_mgr->onMouseMove(button, x, y, ctrl, alt, shift);
}

void set_resize_surface_event(uint32_t width, uint32_t height)
{
    sketch::STCore::inst->onViewResize(width, height);
    sketch::STCore::inst->onViewRender();
}

void set_keyboard_event(int key, int action, [[maybe_unused]] int mods)
{
    sketch::ViewManager* view_mgr = sketch::STCore::inst->viewMgr();
    if(action == SKETCH_KEY_ACTION_PRESS)
    {
        view_mgr->onKeyDown(key, false, false, false);
    }
}

void sketch_render_init()
{
    auto surface = std::make_unique<sketch::render::MemoryStreaming>();
    m_surface = surface.get();
    m_render = std::make_unique<sketch::render::SketchRender>(std::move(surface));
    sketch::STCore::inst->set_render(m_render.get());
    sketch::STCore::inst->init_standalone();
}

void sketch_get_surface_size(uint32_t* width, uint32_t* height)
{
    auto& extent = m_surface->get_extent();
    *height = extent.height;
    *width = extent.width;
}

void sketch_set_callback_after_present(PresentCallbackFn fn, void* user_data)
{
    m_surface->set_present_callback([=](const WGPUTextureDataLayout* layout, const void* data, uint32_t width, uint32_t height, size_t length) {
        fn(layout, data, width, height, length, user_data);
    });
}

// void sketch_set_target_buf_addr(void* addr, uint32_t size)
// {
//     m_surface->set_present_callback([&](const WGPUTextureDataLayout*, const void* data, uint32_t, uint32_t, size_t length) {
//         ashvardanian::stringzilla::memcpy(addr, data, length);
//     });
// }
