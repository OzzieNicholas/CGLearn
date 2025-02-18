#include "sketch-render/streaming/memory_streaming.h"
#include "sketch/log.h"
#include "webgpu.h"
#include "wgpu.h"
#include <algorithm>

namespace sketch::render
{

MemoryStreaming::MemoryStreaming(uint16_t texture_count) : m_texture_count(texture_count) {}

void MemoryStreaming::present()
{
    if(m_current_texture_idx >= m_texture_count || !m_textures[m_current_texture_idx])
    {
        log_error("No current texture to present");
        return;
    }

    m_texture_status[m_current_texture_idx] = TextureStatus::Presenting;
    auto current_buffer = m_buffers[m_current_texture_idx];

    // copy current texture to buffer
    WGPUCommandEncoderDescriptor encoder_desc = { nullptr, "present" };
    auto encoder = wgpuDeviceCreateCommandEncoder(m_device, nullptr);

    WGPUImageCopyTexture image_copy_texture = {};
    image_copy_texture.texture = m_textures[m_current_texture_idx];
    image_copy_texture.aspect = WGPUTextureAspect_All;
    image_copy_texture.mipLevel = 0;
    image_copy_texture.origin = { 0, 0, 0 };

    WGPUImageCopyBuffer image_copy_buffer = {};
    image_copy_buffer.buffer = current_buffer;
    image_copy_buffer.layout.bytesPerRow = std::ceil(m_extent.width / 256.0) * 256 * 4;
    image_copy_buffer.layout.rowsPerImage = m_extent.height;

    wgpuCommandEncoderCopyTextureToBuffer(encoder, &image_copy_texture, &image_copy_buffer, &m_extent);

    WGPUCommandBufferDescriptor command_buffer_desc{ nullptr, "present" };
    auto command_buffer = wgpuCommandEncoderFinish(encoder, &command_buffer_desc);

    wgpuQueueSubmit(m_queue, 1, &command_buffer);

    // map buffer to cpu and handle callback to simulate present
    auto callback = [](WGPUBufferMapAsyncStatus status, void* userdata) {
        if(status != WGPUBufferMapAsyncStatus_Success)
        {
            log_error("can't map buffer when present image");
            return;
        }

        auto context = reinterpret_cast<MemoryStreaming*>(userdata);
        auto current_buffer = context->m_buffers[context->m_current_texture_idx];
        auto* data = wgpuBufferGetConstMappedRange(current_buffer, 0, wgpuBufferGetSize(current_buffer));
        auto current_texture = context->m_textures[context->m_current_texture_idx];
        auto& extent = context->m_extent;

        if(context->m_present_callback)
        {
            WGPUTextureDataLayout layout;
            layout.rowsPerImage = context->m_extent.height;
            layout.offset = 0;
            layout.bytesPerRow = std::ceil(context->m_extent.width / 256.0) * 256 * 4;
            context->m_present_callback(&layout, data, extent.width, extent.height, wgpuBufferGetSize(current_buffer));
        }
        wgpuBufferUnmap(current_buffer);

        context->m_texture_status[context->m_current_texture_idx] = TextureStatus::Idle;
    };

    wgpuBufferMapAsync(current_buffer, WGPUMapMode_Read, 0, wgpuBufferGetSize(current_buffer), callback, this);
}

void MemoryStreaming::get_current_texture(WGPUSurfaceTexture& texture)
{
    auto idle_texture_idx = 0;
    for(auto i = 0; i < m_texture_count; ++i)
    {
        if(m_texture_status[i] == TextureStatus::Idle)
        {
            idle_texture_idx = i;
            break;
        }
    }

    texture.texture = m_textures[idle_texture_idx];
    texture.status = WGPUSurfaceGetCurrentTextureStatus_Success;
    texture.suboptimal = false;
    m_texture_status[idle_texture_idx] = TextureStatus::Using;
    m_current_texture_idx = idle_texture_idx;
}

void MemoryStreaming::get_capabilities([[maybe_unused]] WGPUAdapter adapter, WGPUSurfaceCapabilities& capabilities)
{
    static WGPUCompositeAlphaMode supported_composite_modes[] = {
        WGPUCompositeAlphaMode_Opaque,
        WGPUCompositeAlphaMode_Auto,
        WGPUCompositeAlphaMode_Inherit,
    };

    static WGPUTextureFormat supported_formats[] = {
        WGPUTextureFormat_RGBA8UnormSrgb,
        WGPUTextureFormat_BGRA8UnormSrgb,
        WGPUTextureFormat_RGBA8Unorm,
        WGPUTextureFormat_BGRA8Unorm,
    };

    static WGPUPresentMode supported_present_modes[] = {
        WGPUPresentMode_Fifo,
        WGPUPresentMode_Mailbox,
        WGPUPresentMode_Immediate,
    };

    capabilities.alphaModes = supported_composite_modes;
    capabilities.alphaModeCount = 3;
    capabilities.formats = supported_formats;
    capabilities.formatCount = 2;
    capabilities.presentModes = supported_present_modes;
    capabilities.presentModeCount = 1;
    capabilities.usages = WGPUTextureUsage_RenderAttachment;
}

void MemoryStreaming::configure(WGPUSurfaceConfiguration& config)
{
    m_extent.width = config.width;
    m_extent.height = config.height;
    m_extent.depthOrArrayLayers = 1;

    WGPUTextureDescriptor texture_descriptor{};
    texture_descriptor.viewFormats = config.viewFormats;
    texture_descriptor.viewFormatCount = config.viewFormatCount;
    texture_descriptor.format = config.format;
    texture_descriptor.dimension = WGPUTextureDimension_2D;
    texture_descriptor.mipLevelCount = 1;
    texture_descriptor.sampleCount = 1;
    texture_descriptor.size.width = config.width;
    texture_descriptor.size.height = config.height;
    texture_descriptor.size.depthOrArrayLayers = 1;
    texture_descriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc | WGPUTextureUsage_CopyDst;

    while(!std::all_of(m_texture_status.begin(), m_texture_status.end(), [](auto status) { return status == TextureStatus::Idle; }))
    {
        WGPUWrappedSubmissionIndex index = {};
        // index.queue = m_queue;
        wgpuDevicePoll(m_device, true, nullptr);
    }

    for(auto& current_texture: m_textures)
    {
        wgpuTextureRelease(current_texture);
    }

    for(auto& buffer: m_buffers)
    {
        wgpuBufferRelease(buffer);
    }

    m_textures.clear();
    m_texture_status.clear();
    m_buffers.clear();

    for(auto i = 0; i < m_texture_count; ++i)
    {
        auto texture = wgpuDeviceCreateTexture(m_device, &texture_descriptor);
        m_textures.push_back(texture);
        m_texture_status.push_back(TextureStatus::Idle);

        WGPUBufferDescriptor buffer_desc = {};
        buffer_desc.size = static_cast<uint64_t>(std::ceil(config.width / 256.0) * 256 * config.height) * 4;
        buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
        auto buffer = wgpuDeviceCreateBuffer(m_device, &buffer_desc);
        m_buffers.push_back(buffer);
    }
}

}// namespace sketch::render
