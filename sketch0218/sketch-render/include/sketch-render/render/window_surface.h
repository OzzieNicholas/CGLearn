#pragma once

#include "sketch-render/render/surface_interface.h"
#include "sketch-render/sketch_render_api.h"

namespace sketch::render
{

class SKETCH_RENDER_API WindowSurface : public ISurface
{
public:
    explicit WindowSurface(void* handle = nullptr);
    ~WindowSurface() override;

public:
    void present() final;

    void get_current_texture(WGPUSurfaceTexture& texture) final;

    void get_capabilities(WGPUAdapter adapter, WGPUSurfaceCapabilities& capabilities) final;

    WGPUSurface create_surface(WGPUInstance instance) final;

    void configure(WGPUSurfaceConfiguration& config) final;

    void unconfigure() final;

    void release() final;

    WGPUSurface get_wgpu_surface() final;

private:
    WGPUSurface m_surface = nullptr;
    void* m_handle = nullptr;
};

}// namespace sketch::render
