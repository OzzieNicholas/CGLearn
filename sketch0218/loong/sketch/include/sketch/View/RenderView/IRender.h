#pragma once

#include "../../Define.h"

#include "../RenderRes/RenderMesh.h"
#include "../ViewLayout/ViewCamera.h"

namespace sketch
{
class CORE_DLL_DECLSPEC IRender
{
public:
    virtual ~IRender() = default;

    virtual void init() = 0;
    virtual void clear() = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void render(ViewCamera* camera, Define::ViewRenderMode render_mode, bool dirty) = 0;

    virtual void append_mesh(const RenderMesh& mesh) = 0;
    virtual void append_render_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                    const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool clip, bool occlude) = 0;
    virtual void append_render_point(int idx_mode, bool is_sel, bool is_hover, const std::array<float, 3>& pos_w) = 0;
    virtual void append_render_screen_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                           const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool occlude) = 0;
    virtual void append_render_triangle(const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                                        const std::array<float, 3>& v2, bool occlude, bool twoSide, bool clip, float alpha) = 0;
    virtual void append_delete_mesh(long long id) = 0;
};
}// namespace sketch
