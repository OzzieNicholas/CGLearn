#pragma once

#include "sketch-render/render/sketch_render_context.h"
#include "sketch-render/types.h"
#include "sketch/log.h"
#include <nameof.hpp>

namespace sketch::render
{

template<typename RenderPass> class ISketchRenderPass : public ISketchRenderPassBase
{
    static constexpr std::string_view pass_name = nameof::nameof_type<RenderPass>();
    inline const static string encoder_label_name = format("{} encoder", pass_name);
    inline const static string command_label_name = format("{} command", pass_name);

public:
    ~ISketchRenderPass() override = default;

public:
    WGPUCommandBuffer render(const SketchRenderContext& context) override
    {
        // Create a command encoder for the draw call
        WGPUCommandEncoderDescriptor encoderDesc = {};
        encoderDesc.nextInChain = nullptr;
        encoderDesc.label = encoder_label_name.c_str();
        auto encoder = wgpuDeviceCreateCommandEncoder(context.get_device(), &encoderDesc);

        on_record_command(context, encoder);

        WGPUCommandBufferDescriptor commandBufferDesc = {};
        commandBufferDesc.label = command_label_name.c_str();
        auto command = wgpuCommandEncoderFinish(encoder, &commandBufferDesc);
        wgpuCommandEncoderRelease(encoder);

        return command;
    };

    string_view name() const final { return pass_name; }

    virtual void on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder) = 0;
};

}// namespace sketch::render
