#pragma once

#include "sketch-render/render/surface_interface.h"
#include "sketch-render/sketch_render_api.h"
#include <functional>
#include <utility>
#include <vector>
#include <webgpu.h>

namespace sketch::render
{

using MemoryStreamingCallback = std::function<void(const WGPUTextureDataLayout*, const void*, uint32_t, uint32_t, size_t)>;

class SKETCH_RENDER_API MemoryStreaming : public ISurface
{
    enum class TextureStatus : uint8_t
    {
        Idle,
        Presenting,
        Using,
    };

public:
    explicit MemoryStreaming(uint16_t texture_count = 3);

    MemoryStreaming(const MemoryStreaming&) = delete;
    MemoryStreaming& operator=(const MemoryStreaming&) = delete;

public:
    void present() override;

    void get_current_texture(WGPUSurfaceTexture& texture) override;

    void get_capabilities(WGPUAdapter adapter, WGPUSurfaceCapabilities& capabilities) override;

    void configure(WGPUSurfaceConfiguration& config) override;

    void set_device(WGPUDevice device) override
    {
        m_device = device;
        m_queue = wgpuDeviceGetQueue(m_device);
    }

    void unconfigure() override {};

    WGPUSurface create_surface([[maybe_unused]] WGPUInstance instance) override { return nullptr; }

    void release() override {}

    WGPUSurface get_wgpu_surface() override { return nullptr; }

public:
    void set_present_callback(MemoryStreamingCallback callback) { m_present_callback = std::move(callback); }

    const WGPUExtent3D& get_extent() const { return m_extent; }

private:
    std::vector<WGPUTexture> m_textures;
    std::vector<TextureStatus> m_texture_status;
    MemoryStreamingCallback m_present_callback;
    uint16_t m_texture_count;
    WGPUExtent3D m_extent;
    WGPUDevice m_device;
    uint16_t m_current_texture_idx;

    std::vector<WGPUBuffer> m_buffers;
    WGPUQueue m_queue;
};

}// namespace sketch::render
