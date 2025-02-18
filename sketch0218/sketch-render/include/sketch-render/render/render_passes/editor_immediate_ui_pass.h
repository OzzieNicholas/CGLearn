#pragma once

#include "sketch-render/flags.h"
#include "sketch/types.h"
#include "sketch_render_pass.h"

namespace sketch::render
{

enum class DrawFlagBits : std::uint16_t
{
    None = 0,
    Closed = 1 << 0,
    AntiAliasedLines = 1 << 1,
    AntiAliasedFill = 1 << 2,
    RoundCornersTopLeft = 1 << 4,
    RoundCornersTopRight = 1 << 5,
    RoundCornersBottomLeft = 1 << 6,
    RoundCornersBottomRight = 1 << 7,
    RoundCornersNone = 1 << 8,
    RoundCornersTop = RoundCornersTopLeft | RoundCornersTopRight,
    RoundCornersBottom = RoundCornersBottomLeft | RoundCornersBottomRight,
    RoundCornersLeft = RoundCornersBottomLeft | RoundCornersTopLeft,
    RoundCornersRight = RoundCornersBottomRight | RoundCornersTopRight,
    RoundCornersAll = RoundCornersTopLeft | RoundCornersTopRight | RoundCornersBottomLeft | RoundCornersBottomRight,
    RoundCornersDefault = RoundCornersAll,
    RoundCornersMask = RoundCornersAll | RoundCornersNone,
};
using DrawFlags = Flags<DrawFlagBits>;

class SKETCH_RENDER_API EditorImmediateSceneUIPass : public ISketchRenderPass<EditorImmediateSceneUIPass>
{
public:
    EditorImmediateSceneUIPass() = default;
    ~EditorImmediateSceneUIPass() override = default;

public:
    void initialize(const SketchRenderContext& context) override;

    void destroy() override;

    void on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder) override;

    void clear_data()
    {
        m_draw_cmds.clear();
        m_vertices.clear();
        m_indices.clear();
    }

    void set_dirty(bool dirty) { m_is_dirty = dirty; }

    void clear_gpu_data();

public:
    void add_line(const vec3& from, const vec3& to, const color& color, float thickness = 1.0f);

private:
    void create_pipeline(const SketchRenderContext& context);

    void create_uniform_data(const SketchRenderContext& context);

    void record_render_bundle_encoder(const SketchRenderContext& context);

private:
    /*
     * @brief Typically, 1 command = 1 GPU draw call
     *
     * @note Similar with the ImDrawCmd In imgui
     */
    struct DrawCmd
    {
        vec4 clip_rect;     // Clipping rectangle (x1, y1, x2, y2). get clipping rectangle in "viewport" coordinates
        uint32_t vtx_offset;// Start offset in vertex buffer.
        uint32_t idx_offset;// Start offset in index buffer.
        uint32_t elem_count;// Number of indices (multiple of 3) to be rendered as triangles.
    };

    struct Vertex
    {
        vec3 pos;
        vec2 uv;
        vec4 color;
        uint32_t category;// 0: triangle, 1: line, 2:point
    };

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<DrawCmd> m_draw_cmds;

    WGPUBuffer m_vertex_buffer = nullptr;
    WGPUBuffer m_index_buffer = nullptr;

    struct Uniform
    {
        mat4c mvp;
        alignas(16) float gamma;
    } m_uniform_data;

    WGPUBuffer m_uniform_srv = nullptr;
    WGPUBindGroup m_uniform_bind_group = nullptr;
    WGPUBindGroupLayout m_uniform_layout = nullptr;
    WGPURenderBundle m_render_bundle = nullptr;

    WGPURenderPipeline m_pipeline = nullptr;
    bool m_is_dirty = false;
};

class SKETCH_RENDER_API EditorImmediateScreenUIPass : public ISketchRenderPass<EditorImmediateScreenUIPass>
{
public:
    EditorImmediateScreenUIPass() = default;
    ~EditorImmediateScreenUIPass() override = default;

public:
    void initialize(const SketchRenderContext& context) override;

    void destroy() override;

    void on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder) override;

    void add_line(const vec2& from, const vec2& to, const color& color, float thickness = 1.0f);

    void add_polyline(const std::vector<vec2>& points, const color& color, DrawFlags flags, float thickness = 1.0f);

    void add_rect();

private:
    void create_pipeline(const SketchRenderContext& context);

    void create_uniform_data(const SketchRenderContext& context);

private:
    /*
     * @brief Typically, 1 command = 1 GPU draw call
     *
     * @note Similar with the ImDrawCmd In imgui
     */
    struct DrawCmd
    {
        vec4 clip_rect;     // Clipping rectangle (x1, y1, x2, y2). get clipping rectangle in "viewport" coordinates
        uint32_t vtx_offset;// Start offset in vertex buffer.
        uint32_t idx_offset;// Start offset in index buffer.
        uint32_t elem_count;// Number of indices (multiple of 3) to be rendered as triangles.
    };

    struct Vertex
    {
        vec2 pos, uv;
        vec4 color;
    };

    struct Uniform
    {
        mat4c mvp;
        alignas(16) float gamma;
    } m_uniform_data;

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<DrawCmd> m_draw_cmds;

    WGPUBuffer m_vertex_buffer = nullptr;
    WGPUBuffer m_index_buffer = nullptr;

    WGPUBuffer m_uniform_srv = nullptr;
    WGPUBindGroup m_uniform_bind_group = nullptr;
    WGPUBindGroupLayout m_uniform_layout = nullptr;

    WGPURenderPipeline m_pipeline = nullptr;
};

}// namespace sketch::render
