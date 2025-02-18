#include "sketch-render/render/render_passes/draw_triangle_pass.h"
#include "../utils.h"

namespace sketch::render
{

void DrawTrianglePass::initialize(const SketchRenderContext& context)
{
    auto device = context.get_device();
    auto shader_module = read_shader(device, "shaders/draw_triangle.wgsl");

    // create pipeline
    WGPURenderPipelineDescriptor pipeline_desc = {};
    pipeline_desc.nextInChain = nullptr;
    pipeline_desc.vertex.module = shader_module;
    pipeline_desc.vertex.entryPoint = "vs_main";
    pipeline_desc.vertex.buffers = nullptr;
    pipeline_desc.vertex.bufferCount = 0;

    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = nullptr;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.multisample.alphaToCoverageEnabled = false;
    pipeline_desc.layout = nullptr;

    WGPUFragmentState fragment_state = {};
    fragment_state.module = shader_module;
    fragment_state.entryPoint = "fs_main";
    fragment_state.constantCount = 0;
    fragment_state.constants = nullptr;

    WGPUBlendState blend_state = {};
    blend_state.color = WGPUBlendComponent{};
    blend_state.alpha = WGPUBlendComponent{};
    blend_state.color.operation = WGPUBlendOperation_Add;
    blend_state.color.srcFactor = WGPUBlendFactor_One;
    blend_state.color.dstFactor = WGPUBlendFactor_Zero;
    blend_state.alpha.operation = WGPUBlendOperation_Add;
    blend_state.alpha.srcFactor = WGPUBlendFactor_One;
    blend_state.alpha.dstFactor = WGPUBlendFactor_Zero;

    WGPUColorTargetState color_target_state = {};
    color_target_state.format = context.get_surface_format();
    color_target_state.blend = &blend_state;
    color_target_state.writeMask = WGPUColorWriteMask_All;

    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target_state;

    pipeline_desc.fragment = &fragment_state;
    m_pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_desc);

    wgpuShaderModuleRelease(shader_module);
}

void DrawTrianglePass::destroy() {}

void DrawTrianglePass::on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder)
{
    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;

    WGPURenderPassColorAttachment render_pass_color_attachment = {};
    render_pass_color_attachment.view = context.get_current_texture_view();
    render_pass_color_attachment.resolveTarget = nullptr;
    render_pass_color_attachment.loadOp = WGPULoadOp_Clear;
    render_pass_color_attachment.storeOp = WGPUStoreOp_Store;
    render_pass_color_attachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };
    render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &render_pass_color_attachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.timestampWrites = nullptr;

    auto render_pass_encoder = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
    wgpuRenderPassEncoderSetPipeline(render_pass_encoder, m_pipeline);
    wgpuRenderPassEncoderDraw(render_pass_encoder, 3, 1, 0, 0);
    wgpuRenderPassEncoderEnd(render_pass_encoder);
    wgpuRenderPassEncoderRelease(render_pass_encoder);
}

}// namespace sketch::render
