#include "sketch-render/adapter/sketch_render.h"
#include "sketch-render/render/render_passes/draw_triangle_pass.h"
#include "sketch-render/render/render_passes/editor_immediate_ui_pass.h"
#include "sketch/log.h"

namespace sketch::render
{

void SketchRender::init()
{
    m_render_context = std::make_unique<SketchRenderContext>(std::move(m_surface));

    // add render pass
    auto draw_triangle_pass = std::make_unique<DrawTrianglePass>();
    auto immediate_ui_pass = std::make_unique<EditorImmediateScreenUIPass>();
    auto immediate_ui_scene_pass = std::make_unique<EditorImmediateSceneUIPass>();
    m_render_context->add_render_pass(std::move(draw_triangle_pass));
    m_render_context->add_render_pass(std::move(immediate_ui_pass));
    m_render_context->add_render_pass(std::move(immediate_ui_scene_pass));
    m_render_context->init();
}

void SketchRender::resize(uint32_t width, uint32_t height)
{
    if(!m_render_context)
    {
        log_error("render context is not create!");
        return;
    }
    m_render_context->resize(width, height);
}

void SketchRender::render(sketch::ViewCamera* camera, sketch::Define::ViewRenderMode render_mode, bool dirty)
{
    (void)(render_mode);
    (void)(dirty);

    if(!m_render_context)
    {
        log_error("render context is not create!");
        return;
    }

    auto pass = m_render_context->get_pass<EditorImmediateSceneUIPass>();
    pass->set_dirty(dirty);

    m_render_context->render(camera);
    append_line = false;
}

void SketchRender::append_mesh(sketch::RenderMesh mesh)
{
    // log_info("[TODO: remove]: append_mesh");
}

void SketchRender::append_render_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                      const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool clip, bool occlude)
{
    (void)(alpha);
    (void)(pattern);
    (void)(factor);
    (void)(clip);
    (void)(occlude);
    append_line = true;

    auto pass = m_render_context->get_pass<EditorImmediateSceneUIPass>();
    pass->add_line(vec3{ v0[0], v0[1], v0[2] }, vec3{ v1[0], v1[1], v1[2] }, sketch::color(color[0], color[1], color[2]), width);
}

void SketchRender::append_render_point(int idx_mode, bool is_sel, bool is_hover, const std::array<float, 3>& pos_w)
{
    // log_info("[TODO: remove]: append_render_point");
}

void SketchRender::append_render_screen_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                             const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool occlude)
{
    (void)(alpha);
    (void)(pattern);
    (void)(factor);
    (void)(occlude);

    auto pass = m_render_context->get_pass<EditorImmediateScreenUIPass>();
    pass->add_line(vec2{ v0[0], v0[1] }, vec2{ v1[0], v1[1] }, sketch::color(color[0], color[1], color[2]), width);
    // log_info("append_render_screen_line: {}, {} -> {}, {}", v0[0], v0[1], v1[0], v1[1]);
}

void SketchRender::append_render_triangle(const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                                          const std::array<float, 3>& v2, bool occlude, bool twoSide, bool clip, float alpha)
{
    // log_info("[TODO: remove]: append_render_triangle");
}

}// namespace sketch::render
