#pragma once

#include <webgpu.h>

namespace sketch::render
{

class ISurface
{
public:
    virtual ~ISurface() = default;

public:
    virtual void present() = 0;

    virtual void get_current_texture(WGPUSurfaceTexture& texture) = 0;

    virtual void get_capabilities(WGPUAdapter adapter, WGPUSurfaceCapabilities& capabilities) = 0;

    virtual void configure(WGPUSurfaceConfiguration& config) = 0;

    virtual void unconfigure() = 0;

    virtual WGPUSurface create_surface(WGPUInstance instance) = 0;

    virtual void release() = 0;

    virtual WGPUSurface get_wgpu_surface() = 0;

    virtual void set_device(WGPUDevice device) {}
};

}// namespace sketch::render
