#include "sketch-render/render/window_surface.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace sketch::render
{

WindowSurface::WindowSurface(void* handle) : m_handle(handle) {}

WindowSurface::~WindowSurface() = default;

void WindowSurface::present() { wgpuSurfacePresent(m_surface); }

void WindowSurface::get_current_texture(WGPUSurfaceTexture& texture) { wgpuSurfaceGetCurrentTexture(m_surface, &texture); }

void WindowSurface::get_capabilities(WGPUAdapter adapter, WGPUSurfaceCapabilities& capabilities)
{
    wgpuSurfaceGetCapabilities(m_surface, adapter, &capabilities);
}

WGPUSurface WindowSurface::create_surface(WGPUInstance instance)
{
#ifdef _WIN32
    WGPUSurfaceDescriptorFromWindowsHWND surface_hwnd_desc = {};
    surface_hwnd_desc.hwnd = reinterpret_cast<HWND>(m_handle);
    surface_hwnd_desc.hinstance = GetModuleHandle(nullptr);
    surface_hwnd_desc.chain.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;

    WGPUSurfaceDescriptor surface_desc = {};
    surface_desc.label = "Surface";
    surface_desc.nextInChain = &surface_hwnd_desc.chain;
#endif

    m_surface = wgpuInstanceCreateSurface(instance, &surface_desc);
    return m_surface;
}

void WindowSurface::configure(WGPUSurfaceConfiguration& config) { wgpuSurfaceConfigure(m_surface, &config); }

void WindowSurface::unconfigure() { wgpuSurfaceUnconfigure(m_surface); }

void WindowSurface::release() { wgpuSurfaceRelease(m_surface); }

WGPUSurface WindowSurface::get_wgpu_surface() { return m_surface; }

}// namespace sketch::render
