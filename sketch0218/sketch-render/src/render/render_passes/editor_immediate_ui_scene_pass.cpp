#include "../utils.h"
#include "sketch-render/render/render_passes/editor_immediate_ui_pass.h"
#include "webgpu.h"
#include <glm/glm.hpp>

namespace sketch::render
{

inline static mat4 convert(const Engine::Mat43& m)
{
    mat4 mat;
    mat.col(0) = nous::vec4(m.col[0].x, m.col[0].y, m.col[0].z, 0.0f);
    mat.col(1) = nous::vec4(m.col[1].x, m.col[1].y, m.col[1].z, 0.0f);
    mat.col(2) = nous::vec4(m.col[2].x, m.col[2].y, m.col[2].z, 0.0f);
    mat.col(3) = nous::vec4(m.col[3].x, m.col[3].y, m.col[3].z, 1.0f);
    return mat;
}

void EditorImmediateSceneUIPass::initialize(const SketchRenderContext& context)
{
    create_pipeline(context);
    create_uniform_data(context);

    auto surface_format = context.get_surface_format();
    auto surface_format_str = nameof::nameof_enum(context.get_surface_format());
    m_uniform_data.gamma = 1.0f;
    if(surface_format_str.size() > 4 && surface_format_str.substr(surface_format_str.size() - 4) == "Srgb")
    {
        m_uniform_data.gamma = 1 / 2.2f;
    }
}

void EditorImmediateSceneUIPass::destroy() {}

void EditorImmediateSceneUIPass::on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder)
{
    if(m_is_dirty)
    {
        record_render_bundle_encoder(context);
    }

    auto camera = context.get_camera();
    if(!camera)
    {
        return;
    }

    const auto fov = camera->fov();
    const auto& up = camera->upDir();
    const auto& forward = camera->forwardDir();
    const auto& camera_pos = camera->pos();
    const auto& near_plane = camera->zNear();
    const auto& far_plane = camera->zFar();
    const auto& aspect = camera->viewportRatio();

    auto scale_matrix = convert(camera->d2vMatrix());
    auto view_matrix = look_at(vec3(camera_pos.x, camera_pos.y, camera_pos.z),
                               vec3(camera_pos.x + forward.x, camera_pos.y + forward.y, camera_pos.z + forward.z), vec3(up.x, up.y, up.z));
    auto projection_matrix = projection(fov, aspect, near_plane, far_plane);
    m_uniform_data.mvp = projection_matrix * view_matrix * scale_matrix;

    // write buffer
    auto queue = context.get_queue();
    wgpuQueueWriteBuffer(queue, m_uniform_srv, 0, &m_uniform_data, sizeof(m_uniform_data));

    // record command
    WGPURenderPassColorAttachment render_pass_color_attachment = {};
    render_pass_color_attachment.view = context.get_current_texture_view();
    render_pass_color_attachment.resolveTarget = nullptr;
    render_pass_color_attachment.loadOp = WGPULoadOp_Load;
    render_pass_color_attachment.storeOp = WGPUStoreOp_Store;
    render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &render_pass_color_attachment;

    auto render_pass_encoder = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    wgpuRenderPassEncoderExecuteBundles(render_pass_encoder, 1, &m_render_bundle);
    wgpuRenderPassEncoderEnd(render_pass_encoder);
    wgpuRenderPassEncoderRelease(render_pass_encoder);

    m_draw_cmds.clear();
    m_vertices.clear();
    m_indices.clear();
}

void EditorImmediateSceneUIPass::record_render_bundle_encoder(const SketchRenderContext& context)
{
    // update render data
    if(!m_vertex_buffer || wgpuBufferGetSize(m_vertex_buffer) < m_vertices.size() * sizeof(Vertex))
    {
        if(m_vertex_buffer)
        {
            wgpuBufferRelease(m_vertex_buffer);
        }

        // create vertex buffer
        WGPUBufferDescriptor buffer_desc = {};
        buffer_desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
        buffer_desc.size = m_vertices.size() * sizeof(Vertex);
        buffer_desc.mappedAtCreation = false;
        m_vertex_buffer = wgpuDeviceCreateBuffer(context.get_device(), &buffer_desc);
    }
    wgpuQueueWriteBuffer(context.get_queue(), m_vertex_buffer, 0, m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    if(!m_index_buffer || wgpuBufferGetSize(m_index_buffer) < m_indices.size() * sizeof(uint32_t))
    {
        if(m_index_buffer)
        {
            wgpuBufferRelease(m_index_buffer);
        }

        WGPUBufferDescriptor buffer_desc = {};
        buffer_desc.usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst;
        buffer_desc.size = m_indices.size() * sizeof(uint32_t);
        buffer_desc.mappedAtCreation = false;
        m_index_buffer = wgpuDeviceCreateBuffer(context.get_device(), &buffer_desc);
    }
    wgpuQueueWriteBuffer(context.get_queue(), m_index_buffer, 0, m_indices.data(), m_indices.size() * sizeof(uint32_t));

    // record command
    auto surface_format = context.get_surface_format();
    WGPURenderBundleEncoderDescriptor desc = {};
    desc.colorFormats = &surface_format;
    desc.colorFormatCount = 1;
    desc.sampleCount = 1;
    auto render_bundle_encoder = wgpuDeviceCreateRenderBundleEncoder(context.get_device(), &desc);

    if(!m_vertices.empty() && !m_indices.empty())
    {
        wgpuRenderBundleEncoderSetPipeline(render_bundle_encoder, m_pipeline);
        wgpuRenderBundleEncoderSetVertexBuffer(render_bundle_encoder, 0, m_vertex_buffer, 0, sizeof(Vertex) * m_vertices.size());
        wgpuRenderBundleEncoderSetIndexBuffer(render_bundle_encoder, m_index_buffer, WGPUIndexFormat_Uint32, 0, sizeof(uint32_t) * m_indices.size());

        wgpuRenderBundleEncoderSetBindGroup(render_bundle_encoder, 0, m_uniform_bind_group, 0, nullptr);
        for(auto& cmd: m_draw_cmds)
        {
            wgpuRenderBundleEncoderDrawIndexed(render_bundle_encoder, cmd.elem_count, 1, cmd.idx_offset, cmd.vtx_offset, 0);
        }
    }

    m_render_bundle = wgpuRenderBundleEncoderFinish(render_bundle_encoder, nullptr);
    wgpuRenderBundleEncoderRelease(render_bundle_encoder);
}

void EditorImmediateSceneUIPass::create_pipeline(const SketchRenderContext& context)
{
    auto shader_module = read_shader(context.get_device(), "shaders/editor/scene_ui.wgsl");

    // set pipeline layout
    WGPUBindGroupLayoutEntry bind_group_layout_entries[1] = {};
    bind_group_layout_entries[0].sampler.type = WGPUSamplerBindingType_Undefined;
    bind_group_layout_entries[0].storageTexture.access = WGPUStorageTextureAccess_Undefined;
    bind_group_layout_entries[0].binding = 0;
    bind_group_layout_entries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bind_group_layout_entries[0].texture.sampleType = WGPUTextureSampleType_Undefined;
    bind_group_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    bind_group_layout_entries[0].buffer.hasDynamicOffset = false;

    WGPUBindGroupLayoutDescriptor bind_group_layout_desc = {};
    bind_group_layout_desc.entries = bind_group_layout_entries;
    bind_group_layout_desc.entryCount = 1;
    m_uniform_layout = wgpuDeviceCreateBindGroupLayout(context.get_device(), &bind_group_layout_desc);

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = {};
    pipeline_layout_desc.bindGroupLayouts = &m_uniform_layout;
    pipeline_layout_desc.bindGroupLayoutCount = 1;
    auto pipeline_layout = wgpuDeviceCreatePipelineLayout(context.get_device(), &pipeline_layout_desc);

    // set vertex attributes and vertex state
    WGPUVertexAttribute vertex_attributes[4] = {};
    vertex_attributes[0].format = WGPUVertexFormat_Float32x3;
    vertex_attributes[0].offset = 0;
    vertex_attributes[0].shaderLocation = 0;
    vertex_attributes[1].format = WGPUVertexFormat_Float32x2;
    vertex_attributes[1].offset = offsetof(Vertex, uv);
    vertex_attributes[1].shaderLocation = 1;
    vertex_attributes[2].format = WGPUVertexFormat_Float32x4;
    vertex_attributes[2].offset = offsetof(Vertex, color);
    vertex_attributes[2].shaderLocation = 2;
    vertex_attributes[3].format = WGPUVertexFormat_Uint32;
    vertex_attributes[3].offset = offsetof(Vertex, category);
    vertex_attributes[3].shaderLocation = 3;

    WGPUVertexBufferLayout vertex_buffer_layout = {};
    vertex_buffer_layout.attributes = vertex_attributes;
    vertex_buffer_layout.arrayStride = sizeof(Vertex);
    vertex_buffer_layout.attributeCount = sizeof(vertex_attributes) / sizeof(WGPUVertexAttribute);
    vertex_buffer_layout.stepMode = WGPUVertexStepMode_Vertex;

    WGPURenderPipelineDescriptor pipeline_desc = {};
    pipeline_desc.nextInChain = nullptr;
    pipeline_desc.vertex.module = shader_module;
    pipeline_desc.vertex.entryPoint = "vs_main";
    pipeline_desc.vertex.buffers = &vertex_buffer_layout;
    pipeline_desc.vertex.bufferCount = 1;

    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_LineList;
    pipeline_desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = nullptr;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.multisample.alphaToCoverageEnabled = false;
    pipeline_desc.layout = pipeline_layout;

    WGPUFragmentState fragment_state = {};
    fragment_state.module = shader_module;
    fragment_state.entryPoint = "fs_main";

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
    m_pipeline = wgpuDeviceCreateRenderPipeline(context.get_device(), &pipeline_desc);

    wgpuPipelineLayoutRelease(pipeline_layout);
    wgpuShaderModuleRelease(shader_module);
}

void EditorImmediateSceneUIPass::create_uniform_data(const SketchRenderContext& context)
{
    auto device = context.get_device();

    // create buffer
    WGPUBufferDescriptor buffer_desc = {};
    buffer_desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    buffer_desc.size = sizeof(Uniform);
    buffer_desc.mappedAtCreation = false;
    m_uniform_srv = wgpuDeviceCreateBuffer(device, &buffer_desc);

    // create bind group
    WGPUBindGroupEntry bind_group_entries[1] = {};
    bind_group_entries[0].binding = 0;
    bind_group_entries[0].buffer = m_uniform_srv;
    bind_group_entries[0].offset = 0;
    bind_group_entries[0].size = sizeof(Uniform);

    WGPUBindGroupDescriptor bind_group_desc = {};
    bind_group_desc.layout = m_uniform_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = bind_group_entries;
    m_uniform_bind_group = wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}

void EditorImmediateSceneUIPass::add_line(const vec3& from, const vec3& to, const color& color, float thickness)
{
    (void)(thickness);

    auto vtx_offset = m_vertices.size();
    auto idx_offset = m_indices.size();

    Vertex vtx;
    vtx.pos = from;
    vtx.uv = vec2(0.0f, 0.0f);
    vtx.color = color.data();
    vtx.category = 1;
    m_vertices.push_back(vtx);

    vtx.pos = to;
    vtx.uv = vec2(0.0f, 0.0f);
    vtx.color = color.data();
    vtx.category = 1;
    m_vertices.push_back(vtx);

    m_indices.push_back(0);
    m_indices.push_back(1);

    DrawCmd cmd;
    cmd.vtx_offset = vtx_offset;
    cmd.idx_offset = idx_offset;
    cmd.elem_count = 2;
    m_draw_cmds.push_back(cmd);
}

}// namespace sketch::render
