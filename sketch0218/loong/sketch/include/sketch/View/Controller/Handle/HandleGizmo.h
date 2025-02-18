#pragma once
#include "Handle.h"

namespace sketch
{
class HandleGizmo : public HandleBase
{
    enum GizmoPart : uint8_t
    {
        NONE = 0,
        TRANSLATE_X,
        TRANSLATE_Y,
        TRANSLATE_Z,
        SCALE_X,
        SCALE_Y,
        SCALE_Z,
        ROTATE_X,
        ROTATE_Y,
        ROTATE_Z
    };

public:
    HandleGizmo(ViewCamera* cam, GizmoMode mode);
    ~HandleGizmo() override;

    inline void setPixelRange(int size) { m_pixelRange = size; }

    void onMouseDown(const nous::vec2& pickPixel) override;
    void onMouseMove(const nous::vec2& pickPixel) override;
    void onMouseUp(const nous::vec2& pickPixel) override;
    void update() override;

    void get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                         std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines) override;

    bool is_hovered() const override { return m_gizmoHoverType != GizmoPart::NONE; }
    bool is_picked() const override { return m_gizmoPickType != GizmoPart::NONE; }

    void set_gizmo_drag_type(unsigned int gizmoType) override { m_gizmoHoverType = (GizmoPart)gizmoType; }

protected:
    void _updateMesh();
    GizmoPart _checkIntersect(const nous::vec2& pickPixel);
    void _translate(const nous::vec2& pickPixel);
    void _rotate(const nous::vec2& pickPixel);
    void _scale(const nous::vec2& pickPixel);

protected:
    int m_pixelRange;

    GizmoPart m_gizmoHoverType;
    GizmoPart m_gizmoPickType;

    nous::vec3 m_transPickPos;
    nous::vec3 m_transOrgPos;
    nous::vec3 m_transChangeOrgPos;

    nous::vec3 m_scale_dir;
    nous::vec3 m_scalePickPos;
    nous::vec3 m_scaleBase;

    nous::vec3 m_rotate_normal;
    nous::vec3 m_rotateBaseVec;
    nous::quat m_rotateOrg;

    /// translate
    // x
    nous::vec3 m_xTransConeTopOrgPosition;
    std::vector<nous::vec3> m_xTransConeBottomFaceOrgPosition;
    nous::vec3 m_xTransConeTopPosition;
    std::vector<nous::vec3> m_xTransConeBottomFacePosition;
    std::vector<std::vector<nous::vec3>> m_xTransTriangles;
    std::pair<nous::vec3, nous::vec3> m_xTransLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_xTransLinePosition;
    // y
    nous::vec3 m_yTransConeTopOrgPosition;
    std::vector<nous::vec3> m_yTransConeBottomFaceOrgPosition;
    nous::vec3 m_yTransConeTopPosition;
    std::vector<nous::vec3> m_yTransConeBottomFacePosition;
    std::vector<std::vector<nous::vec3>> m_yTransTriangles;
    std::pair<nous::vec3, nous::vec3> m_yTransLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_yTransLinePosition;
    // z
    nous::vec3 m_zTransConeTopOrgPosition;
    std::vector<nous::vec3> m_zTransConeBottomFaceOrgPosition;
    nous::vec3 m_zTransConeTopPosition;
    std::vector<nous::vec3> m_zTransConeBottomFacePosition;
    std::vector<std::vector<nous::vec3>> m_zTransTriangles;
    std::pair<nous::vec3, nous::vec3> m_zTransLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_zTransLinePosition;

    /// scale
    // x
    std::pair<nous::vec3, nous::vec3> m_xScaleLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_xScaleLinePosition;
    std::vector<nous::vec3> m_xScaleCubeOrgPosition;
    std::vector<nous::vec3> m_xScaleCubePosition;
    std::vector<std::vector<nous::vec3>> m_xScaleCubeTriangles;
    // y
    std::pair<nous::vec3, nous::vec3> m_yScaleLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_yScaleLinePosition;
    std::vector<nous::vec3> m_yScaleCubeOrgPosition;
    std::vector<nous::vec3> m_yScaleCubePosition;
    std::vector<std::vector<nous::vec3>> m_yScaleCubeTriangles;
    // z
    std::pair<nous::vec3, nous::vec3> m_zScaleLineOrgPosition;
    std::pair<nous::vec3, nous::vec3> m_zScaleLinePosition;
    std::vector<nous::vec3> m_zScaleCubeOrgPosition;
    std::vector<nous::vec3> m_zScaleCubePosition;
    std::vector<std::vector<nous::vec3>> m_zScaleCubeTriangles;

    /// rotate
    // x
    std::vector<nous::vec3> m_xRotateLinesOrgPosition;
    std::vector<nous::vec3> m_xRotateLinesPosition;
    // y
    std::vector<nous::vec3> m_yRotateLinesOrgPosition;
    std::vector<nous::vec3> m_yRotateLinesPosition;
    // z
    std::vector<nous::vec3> m_zRotateLinesOrgPosition;
    std::vector<nous::vec3> m_zRotateLinesPosition;

    GizmoMode m_mode = GizmoMode::None;
};
}// namespace sketch