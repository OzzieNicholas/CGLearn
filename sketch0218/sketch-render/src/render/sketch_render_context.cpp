#include "sketch-render/render/sketch_render_context.h"
#include "sketch/log.h"
#include "utils.h"
#include "webgpu.h"
#include <nameof.hpp>
#include <tbb/parallel_for.h>

namespace sketch::render
{

static void render_log_callback(WGPULogLevel level, char const* message, [[maybe_unused]] void* pUserData)
{
    switch(level)
    {
    case WGPULogLevel_Off: log_off("wgpu off: {}", message); break;
    case WGPULogLevel_Error: log_error("wgpu error: {}", message); break;
    case WGPULogLevel_Warn: log_warn("wgpu warn: {}", message); break;
    case WGPULogLevel_Info: log_info("wgpu info: {}", message); break;
    case WGPULogLevel_Debug: log_debug("wgpu debug: {}", message); break;
    case WGPULogLevel_Trace: log_trace("wgpu trace: {}", message); break;
    case WGPULogLevel_Force32: log_error("wgpu force32: {}", message); break;
    }
}

void SketchRenderContext::init()
{
    wgpuSetLogCallback(render_log_callback, nullptr);
    wgpuSetLogLevel(WGPULogLevel_Warn);

    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    m_instance = wgpuCreateInstance(&desc);

    // create surface and adapter
    m_surface->create_surface(m_instance);
    query_physical_device();
    create_logic_device();
    m_queue = wgpuDeviceGetQueue(m_device);

    // config the surface
    WGPUSurfaceCapabilities surface_capabilities = {};
    m_surface->get_capabilities(m_adapter, surface_capabilities);
    m_surface_format = surface_capabilities.formats[0];
    m_present_mode = surface_capabilities.presentModes[0];
    m_alpha_mode = surface_capabilities.alphaModes[0];

    m_width = 800;
    m_height = 600;

    WGPUSurfaceConfiguration surface_config = {};
    surface_config.alphaMode = m_alpha_mode;
    surface_config.presentMode = m_present_mode;
    surface_config.format = m_surface_format;
    surface_config.height = m_height;
    surface_config.width = m_width;
    surface_config.usage = WGPUTextureUsage_RenderAttachment;
    surface_config.device = m_device;
    m_surface->configure(surface_config);

    // create_staing_buffer(1024);

    for(auto& pass: m_passes_order)
    {
        pass->initialize(*this);
    }
}

void SketchRenderContext::finish()
{
    for(auto& pass: m_passes_order)
    {
        pass->destroy();
    }

    m_surface->unconfigure();
    m_surface->release();

    if(m_queue)
    {
        wgpuQueueReference(m_queue);
    }

    if(m_device)
    {
        wgpuDeviceRelease(m_device);
    }

    if(m_adapter)
    {
        wgpuAdapterRelease(m_adapter);
    }

    if(m_instance)
    {
        wgpuInstanceRelease(m_instance);
    }
}

void SketchRenderContext::on_resize()
{
    // config the surface
    m_width = std::get<0>(*m_new_size);
    m_height = std::get<1>(*m_new_size);
    WGPUSurfaceConfiguration surface_config = {};
    surface_config.alphaMode = m_alpha_mode;
    surface_config.presentMode = m_present_mode;
    surface_config.format = m_surface_format;
    surface_config.width = m_width;
    surface_config.height = m_height;
    surface_config.usage = WGPUTextureUsage_RenderAttachment;
    surface_config.device = m_device;
    m_surface->configure(surface_config);

    m_new_size = std::nullopt;
}

void SketchRenderContext::create_logic_device()
{
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if(status == WGPURequestDeviceStatus_Success)
        {
            userData.device = device;
        }
        else
        {
            log_error("Could not get WebGPU device: {}", message);
        }
        userData.requestEnded = true;
    };

    WGPUDeviceDescriptor deviceDesc = {};
    wgpuAdapterRequestDevice(m_adapter, &deviceDesc, onDeviceRequestEnded, (void*)&userData);
    assert(userData.requestEnded);
    m_device = userData.device;
    m_surface->set_device(m_device);

    // print device info
    std::vector<WGPUFeatureName> features;
    size_t featureCount = wgpuDeviceEnumerateFeatures(m_device, nullptr);
    features.resize(featureCount);
    wgpuDeviceEnumerateFeatures(m_device, features.data());

    log_info("Device feature:");
    for(auto f: features)
    {
        log_info("\t {}", nameof::nameof_enum(f));
    }

    WGPUSupportedLimits limits = {};
    limits.nextInChain = nullptr;

    bool success = wgpuDeviceGetLimits(m_device, &limits);

    if(success)
    {
        log_info("Device limit:");
        log_info("\t maxTextureDimension1D: {}", limits.limits.maxTextureDimension1D);
        log_info("\t maxTextureDimension2D: {}", limits.limits.maxTextureDimension2D);
        log_info("\t maxTextureDimension3D: {}", limits.limits.maxTextureDimension3D);
        log_info("\t maxTextureArrayLayers: {}", limits.limits.maxTextureArrayLayers);
    }
}

void SketchRenderContext::query_physical_device()
{
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto on_adapter_request_ended = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if(status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = adapter;
            userData.requestEnded = true;
        }
        else
        {
            log_error("Could not get WebGPU adapter: {}", message);
            userData.requestEnded = false;
        }
    };

    WGPURequestAdapterOptions options = {};
    options.compatibleSurface = m_surface->get_wgpu_surface();
    options.backendType = WGPUBackendType_Vulkan;

    wgpuInstanceRequestAdapter(m_instance, &options, on_adapter_request_ended, static_cast<void*>(&userData));
    if(!userData.requestEnded)
    {
        throw std::runtime_error("Could not get WebGPU adapter");
    }
    m_adapter = userData.adapter;

    if(!wgpuAdapterGetLimits(m_adapter, &m_adapter_limits))
    {
        log_warn("Could not get WebGPU adapter limits");
    }
    wgpuAdapterGetInfo(m_adapter, &m_adapter_info);
    log_info("physical device vendor: {}, physical device vendor Id: {}", m_adapter_info.vendor, m_adapter_info.vendorID);
    log_info("physical device: {}, physical device Id: {}", m_adapter_info.device, m_adapter_info.deviceID);
    log_info("physical device description: {}, architecture: {}", m_adapter_info.description, m_adapter_info.architecture);
    log_info("physical device type: {}, wgpu backend: {}", NAMEOF_ENUM(m_adapter_info.adapterType), NAMEOF_ENUM(m_adapter_info.backendType));

    auto features_count = wgpuAdapterEnumerateFeatures(m_adapter, nullptr);
    std::vector<WGPUFeatureName> features(features_count);
    wgpuAdapterEnumerateFeatures(m_adapter, features.data());
    log_info("Adapter feature:");
    for(auto f: features)
    {
        auto feature_name = nameof::nameof_enum(f);
        if(feature_name.empty())
        {
            auto native_feature_name = static_cast<WGPUNativeFeature>(f);
            log_info("\t {}", get_feature_name(native_feature_name));
        }
        else
        {
            log_info("\t {}", nameof::nameof_enum(f));
        }
    }
}

void SketchRenderContext::render(ViewCamera* camera)
{

    m_camera = camera;
    if(m_new_size.has_value())
    {
        if(std::get<0>(m_new_size.value()) == 0 || std::get<1>(m_new_size.value()) == 0)
        {
            return;
        }
        on_resize();
    }

    WGPUSurfaceTexture surface_texture;
    m_surface->get_current_texture(surface_texture);
    if(surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_Success)
    {
        return;
    }

    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = "Surface texture view";
    viewDescriptor.format = wgpuTextureGetFormat(surface_texture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    m_current_texture_view = wgpuTextureCreateView(surface_texture.texture, &viewDescriptor);

    std::vector<WGPUCommandBuffer> commands(m_passes_order.size());
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_passes_order.size()), [&](const auto& range) {
        for(auto i = range.begin(); i != range.end(); ++i)
        {
            commands[i] = m_passes_order[i]->render(*this);
        }
    });
    wgpuQueueSubmit(m_queue, commands.size(), commands.data());
    std::for_each(commands.begin(), commands.end(), [](WGPUCommandBuffer command) { wgpuCommandBufferRelease(command); });

    m_surface->present();
    wgpuTextureViewRelease(m_current_texture_view);
}

}// namespace sketch::render
