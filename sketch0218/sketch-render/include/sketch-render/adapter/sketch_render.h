#pragma once

#include "sketch-render/render/sketch_render_context.h"
#include "sketch-render/sketch_render_api.h"
#include "sketch/View/RenderView/IRender.h"

namespace sketch::render
{

class SKETCH_RENDER_API SketchRender : public sketch::IRender
{
public:
    explicit SketchRender(std::unique_ptr<ISurface> surface) : m_surface(std::move(surface)) {}
    ~SketchRender()
    {
        if(m_render_context)
        {
            m_render_context->finish();
        }
    }

    void init() override;

    void resize(uint32_t width, uint32_t height) override;

    void render(sketch::ViewCamera* camera, sketch::Define::ViewRenderMode render_mode, bool dirty) override;

    void append_mesh(sketch::RenderMesh mesh) override;
    void append_render_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                            uint16_t pattern, uint8_t factor, bool clip, bool occlude) override;
    void append_render_point(int idx_mode, bool is_sel, bool is_hover, const std::array<float, 3>& pos_w) override;
    void append_render_screen_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                   const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool occlude) override;
    void append_render_triangle(const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                                const std::array<float, 3>& v2, bool occlude, bool twoSide, bool clip, float alpha) override;

private:
    std::unique_ptr<SketchRenderContext> m_render_context;
    std::unique_ptr<ISurface> m_surface;

    bool append_line = false;
};

}// namespace sketch::render
