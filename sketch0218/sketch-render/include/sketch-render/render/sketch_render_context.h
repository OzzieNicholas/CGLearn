#pragma once

#include "sketch-render/sketch_render_api.h"
#include "sketch-render/types.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "surface_interface.h"
#include <memory>
#include <nameof.hpp>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <webgpu.h>
#include <wgpu.h>

namespace sketch::render
{

class SketchRenderContext;
class SKETCH_RENDER_API ISketchRenderPassBase
{
public:
    virtual ~ISketchRenderPassBase() = default;

public:
    virtual void initialize(const SketchRenderContext& context) = 0;

    virtual void destroy() = 0;

    virtual WGPUCommandBuffer render(const SketchRenderContext& context) = 0;

    virtual string_view name() const = 0;
};

class SKETCH_RENDER_API SketchRenderContext
{
public:
    explicit SketchRenderContext(std::unique_ptr<ISurface> surface) : m_surface(std::move(surface)) {}
    ~SketchRenderContext() = default;

    SketchRenderContext(const SketchRenderContext&) = delete;
    SketchRenderContext(SketchRenderContext&&) = delete;
    SketchRenderContext& operator=(const SketchRenderContext&) = delete;
    SketchRenderContext& operator=(SketchRenderContext&&) = delete;

    /**
     * @brief initialize render context
     *
     * @param callback
     */
    void init();

    void finish();

    void render(ViewCamera* camera);

    void resize(uint32_t width, uint32_t height) { m_new_size = std::make_tuple(width, height); }

    void add_render_pass(std::unique_ptr<ISketchRenderPassBase> pass)
    {
        auto&& [iter, success] = m_passes.insert_or_assign(pass->name(), std::move(pass));
        if(success)
        {
            m_passes_order.push_back(iter->second.get());
        }
    }

    ISketchRenderPassBase* get_pass(const string_view& name)
    {
        auto iter = m_passes.find(name);
        if(iter != m_passes.end())
        {
            return iter->second.get();
        }
        return nullptr;
    }

    const ISketchRenderPassBase* get_pass(const string_view& name) const
    {
        auto iter = m_passes.find(name);
        if(iter != m_passes.end())
        {
            return iter->second.get();
        }
        return nullptr;
    }

    template<typename Pass> bool has_pass() const
    {
        static_assert(std::is_base_of_v<ISketchRenderPassBase, Pass>, "does not inherit from ISketchRenderPassBase");
        return m_passes.count(to_stringview(nameof::nameof_type<Pass>())) != 0;
        return false;
    }

    template<typename Pass> const Pass* get_pass() const
    {
        static_assert(std::is_base_of_v<ISketchRenderPassBase, Pass>, "does not inherit from ISketchRenderPassBase");
        const string_view name = nameof::nameof_type<Pass>();
        return static_cast<const Pass*>(get_pass(name));
    }

    template<typename Pass> Pass* get_pass()
    {
        static_assert(std::is_base_of_v<ISketchRenderPassBase, Pass>, "does not inherit from ISketchRenderPassBase");
        return static_cast<Pass*>(get_pass(nameof::nameof_type<Pass>()));
    }

public:
    WGPUDevice get_device() const { return m_device; }

    WGPUQueue get_queue() const { return m_queue; }

    uint32_t get_width() const { return m_width; }

    uint32_t get_height() const { return m_height; }

    // TODO: should not be const
    ViewCamera* get_camera() const { return m_camera; }

    ViewCamera* get_camera() { return m_camera; }

    WGPUTextureView get_current_texture_view() const { return m_current_texture_view; }

    WGPUTextureFormat get_surface_format() const { return m_surface_format; }

private:
    void create_logic_device();

    void query_physical_device();

    void on_resize();

private:
    WGPUInstance m_instance = nullptr;
    WGPUAdapter m_adapter = nullptr;
    WGPUDevice m_device = nullptr;
    WGPUQueue m_queue = nullptr;
    std::unique_ptr<ISurface> m_surface;
    // WGPUSurface m_surface = nullptr;
    WGPUTextureFormat m_surface_format = WGPUTextureFormat_BGRA8Unorm;
    WGPUPresentMode m_present_mode = WGPUPresentMode_Fifo;
    WGPUCompositeAlphaMode m_alpha_mode = WGPUCompositeAlphaMode_Opaque;
    WGPUSupportedLimits m_adapter_limits = {};
    WGPUAdapterInfo m_adapter_info = {};
    WGPUTextureView m_current_texture_view = nullptr;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    std::optional<std::tuple<uint32_t, uint32_t>> m_new_size = std::nullopt;

    std::unordered_map<string_view, std::unique_ptr<ISketchRenderPassBase>> m_passes;
    std::vector<ISketchRenderPassBase*> m_passes_order;

    //
    ViewCamera* m_camera = nullptr;
};

}// namespace sketch::render
