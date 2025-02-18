#include "sketch/View/Controller/Handle/HandleGizmo.h"
#include "nous_mesh/algorithm/intersect.h"

#include <algorithm>
#include <map>

namespace sketch
{
HandleGizmo::HandleGizmo(ViewCamera* cam, const GizmoMode mode)
    : HandleBase(cam), m_pixelRange(10 * 10), m_gizmoHoverType(GizmoPart::NONE), m_gizmoPickType(GizmoPart::NONE), m_mode(mode)
{
    // trans
    constexpr float coneTop = 48.0f;
    constexpr float coneBottom = 38.0f;
    constexpr int coneFaceCount = 8;
    m_xTransConeTopOrgPosition = nous::vec3(coneTop, 0.0f, 0.0f);
    m_yTransConeTopOrgPosition = nous::vec3(0.0f, coneTop, 0.0f);
    m_zTransConeTopOrgPosition = nous::vec3(0.0f, 0.0f, coneTop);
    m_xTransLineOrgPosition = std::make_pair(nous::vec3(0.0f, 0.0f, 0.0f), nous::vec3(coneBottom, 0.0f, 0.0f));
    m_yTransLineOrgPosition = std::make_pair(nous::vec3(0.0f, 0.0f, 0.0f), nous::vec3(0.0f, coneBottom, 0.0f));
    m_zTransLineOrgPosition = std::make_pair(nous::vec3(0.0f, 0.0f, 0.0f), nous::vec3(0.0f, 0.0f, coneBottom));
    for(int i = 0; i < coneFaceCount; ++i)
    {
        constexpr float coneRadius = 2.0f;
        float radians = 3.141592f * coneRadius * static_cast<float>(i) / coneFaceCount;
        m_xTransConeBottomFaceOrgPosition.emplace_back(coneBottom, coneRadius * cos(radians), coneRadius * sin(radians));
        m_yTransConeBottomFaceOrgPosition.emplace_back(coneRadius * cos(radians), coneBottom, coneRadius * sin(radians));
        m_zTransConeBottomFaceOrgPosition.emplace_back(coneRadius * cos(radians), coneRadius * sin(radians), coneBottom);
    }

    // scale
    float lineBegin = 5.0f;
    float lineEnd = 20.0f;
    float halfLength = 1.6f;
    float center = lineEnd + halfLength;
    m_xScaleLineOrgPosition = std::make_pair(nous::vec3(lineBegin, 0.0f, 0.0f), nous::vec3(lineEnd, 0.0f, 0.0f));
    m_yScaleLineOrgPosition = std::make_pair(nous::vec3(0.0f, lineBegin, 0.0f), nous::vec3(0.0f, lineEnd, 0.0f));
    m_zScaleLineOrgPosition = std::make_pair(nous::vec3(0.0f, 0.0f, lineBegin), nous::vec3(0.0f, 0.0f, lineEnd));
    m_xScaleCubeOrgPosition = { nous::vec3(center, 0.f, 0.f) + nous::vec3(halfLength, halfLength, halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(halfLength, halfLength, -halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(halfLength, -halfLength, halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(halfLength, -halfLength, -halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(-halfLength, halfLength, halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(-halfLength, halfLength, -halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(-halfLength, -halfLength, halfLength),
                                nous::vec3(center, 0.f, 0.f) + nous::vec3(-halfLength, -halfLength, -halfLength) };
    m_yScaleCubeOrgPosition = { nous::vec3(0.f, center, 0.f) + nous::vec3(halfLength, halfLength, halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(halfLength, halfLength, -halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(halfLength, -halfLength, halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(halfLength, -halfLength, -halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(-halfLength, halfLength, halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(-halfLength, halfLength, -halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(-halfLength, -halfLength, halfLength),
                                nous::vec3(0.f, center, 0.f) + nous::vec3(-halfLength, -halfLength, -halfLength) };
    m_zScaleCubeOrgPosition = { nous::vec3(0.f, 0.f, center) + nous::vec3(halfLength, halfLength, halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(halfLength, halfLength, -halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(halfLength, -halfLength, halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(halfLength, -halfLength, -halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(-halfLength, halfLength, halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(-halfLength, halfLength, -halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(-halfLength, -halfLength, halfLength),
                                nous::vec3(0.f, 0.f, center) + nous::vec3(-halfLength, -halfLength, -halfLength) };

    // rotate
    float rotateCircleRadius = center + halfLength + 5.0f;
    int rotateLineCount = 24;
    for(int i = 0; i < rotateLineCount; ++i)
    {
        float radians = 3.141592f * 2 * static_cast<float>(i) / static_cast<float>(rotateLineCount);
        m_xRotateLinesOrgPosition.emplace_back(0.0f, rotateCircleRadius * cos(radians), rotateCircleRadius * sin(radians));
        m_yRotateLinesOrgPosition.emplace_back(rotateCircleRadius * cos(radians), 0.0f, rotateCircleRadius * sin(radians));
        m_zRotateLinesOrgPosition.emplace_back(rotateCircleRadius * cos(radians), rotateCircleRadius * sin(radians), 0.0f);
    }
}

HandleGizmo::~HandleGizmo() = default;

void HandleGizmo::onMouseDown(const nous::vec2& pickPixel)
{
    m_gizmoPickType = GizmoPart::NONE;
    if(m_gizmoHoverType != GizmoPart::NONE)
    {
        nous::transform3 trans(m_gizmo_rotation);
        m_gizmoPickType = m_gizmoHoverType;
        if(m_gizmoPickType >= GizmoPart::TRANSLATE_X && m_gizmoPickType <= GizmoPart::TRANSLATE_Z)
        {
            nous::vec3 od;
            if(m_gizmoPickType == GizmoPart::TRANSLATE_X)
            {
                od = trans.matrix().col(0).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::TRANSLATE_Y)
            {
                od = trans.matrix().col(1).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::TRANSLATE_Z)
            {
                od = trans.matrix().col(2).head<3>();
            }
            nous::vec3 o;
            nous::vec3 d;
            nous::vec3 intersect0;
            nous::vec3 intersect1;
            this->_getCameraPickRay(pickPixel, o, d);
            nous::algo::line_intersect_3d(o, o + d, m_gizmo_position, m_gizmo_position + od, intersect0, intersect1, false);
            m_transPickPos = intersect1;
            m_transOrgPos = m_gizmo_position;
            m_transChangeOrgPos = m_manipulate_position;
        }
        else if(m_gizmoPickType >= GizmoPart::SCALE_X && m_gizmoPickType <= GizmoPart::SCALE_Z)
        {
            // nous::vec3 od;
            if(m_gizmoPickType == GizmoPart::SCALE_X)
            {
                m_scale_dir = trans.matrix().col(0).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::SCALE_Y)
            {
                m_scale_dir = trans.matrix().col(1).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::SCALE_Z)
            {
                m_scale_dir = trans.matrix().col(2).head<3>();
            }
            nous::vec3 o;
            nous::vec3 d;
            nous::vec3 intersect0;
            nous::vec3 intersect1;
            this->_getCameraPickRay(pickPixel, o, d);
            nous::algo::line_intersect_3d(o, o + d, m_gizmo_position, m_gizmo_position + m_scale_dir, intersect0, intersect1, false);
            m_scalePickPos = intersect1;
            m_scaleBase = m_manipulate_scale;
        }
        else if(m_gizmoPickType >= GizmoPart::ROTATE_X && m_gizmoPickType <= GizmoPart::ROTATE_Z)
        {
            if(m_gizmoPickType == GizmoPart::ROTATE_X)
            {
                m_rotate_normal = trans.matrix().col(0).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::ROTATE_Y)
            {
                m_rotate_normal = trans.matrix().col(1).head<3>();
            }
            else if(m_gizmoPickType == GizmoPart::ROTATE_Z)
            {
                m_rotate_normal = trans.matrix().col(2).head<3>();
            }
            nous::vec3 o;
            nous::vec3 d;
            this->_getCameraPickRay(pickPixel, o, d);
            float denominator = d.dot(m_rotate_normal);
            nous::vec3 intersectPos;
            if(std::abs(denominator) > std::numeric_limits<float>::epsilon())
            {
                float t = -(o - m_gizmo_position).dot(m_rotate_normal) / denominator;
                intersectPos = o + t * d;
            }
            m_rotateBaseVec = (intersectPos - m_gizmo_position).normalized();
            m_rotateOrg = m_manipulate_rotation;
        }
    }
}

void HandleGizmo::onMouseMove(const nous::vec2& pickPixel)
{
    m_gizmoHoverType = GizmoPart::NONE;
    if(m_gizmoPickType == GizmoPart::NONE)
    {
        m_gizmoHoverType = _checkIntersect(pickPixel);
    }
    else if(m_gizmoPickType >= GizmoPart::TRANSLATE_X && m_gizmoPickType <= GizmoPart::TRANSLATE_Z)
    {
        _translate(pickPixel);
    }
    else if(m_gizmoPickType >= GizmoPart::SCALE_X && m_gizmoPickType <= GizmoPart::SCALE_Z)
    {
        _scale(pickPixel);
    }
    else if(m_gizmoPickType >= GizmoPart::ROTATE_X && m_gizmoPickType <= GizmoPart::ROTATE_Z)
    {
        _rotate(pickPixel);
    }
}

void HandleGizmo::onMouseUp(const nous::vec2& pickPixel) { m_gizmoPickType = GizmoPart::NONE; }

void HandleGizmo::update() { _updateMesh(); }

void HandleGizmo::get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                                  std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines)
{
    _updateMesh();
    float bcf = 0.3f;
    float bcc = 0.7f;
    float ac = 0.3f;
    float transColorFactor_x = 0.0f;
    float transColorFactor_y = 0.0f;
    float transColorFactor_z = 0.0f;
    float scaleColorFactor_x = 0.0f;
    float scaleColorFactor_y = 0.0f;
    float scaleColorFactor_z = 0.0f;
    float rotateColorFactor_x = 0.0f;
    float rotateColorFactor_y = 0.0f;
    float rotateColorFactor_z = 0.0f;
    if(m_gizmoHoverType == GizmoPart::TRANSLATE_X)
    {
        transColorFactor_x = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::TRANSLATE_Y)
    {
        transColorFactor_y = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::TRANSLATE_Z)
    {
        transColorFactor_z = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::SCALE_X)
    {
        scaleColorFactor_x = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::SCALE_Y)
    {
        scaleColorFactor_y = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::SCALE_Z)
    {
        scaleColorFactor_z = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::ROTATE_X)
    {
        rotateColorFactor_x = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::ROTATE_Y)
    {
        rotateColorFactor_y = ac;
    }
    else if(m_gizmoHoverType == GizmoPart::ROTATE_Z)
    {
        rotateColorFactor_z = ac;
    }

    // translate
    if(m_mode & GizmoMode::Translate)
    {
        auto getTranslateHandleMesh = [&](const nous::vec4& color, const auto& transTrianglesPosition, auto& tris) {
            for(auto& tri: transTrianglesPosition)
            {
                tris.emplace_back(color, std::move(tri));
            }
        };
        getTranslateHandleMesh(nous::vec4(bcc + transColorFactor_x, bcf, bcf, 1.0f), m_xTransTriangles, colorTris);
        getTranslateHandleMesh(nous::vec4(bcf, bcc + transColorFactor_y, bcf, 1.0f), m_yTransTriangles, colorTris);
        getTranslateHandleMesh(nous::vec4(bcf, bcf, bcc + transColorFactor_z, 1.0f), m_zTransTriangles, colorTris);

        if(!(m_mode & GizmoMode::Scale))
        {
            colorLines.emplace_back(nous::vec4(bcc + scaleColorFactor_x, bcf, bcf, 1.0f),
                                    std::vector<nous::vec3>{ m_xTransLinePosition.first, m_xTransLinePosition.second });
            colorLines.emplace_back(nous::vec4(bcf, bcc + transColorFactor_y, bcf, 1.0f),
                                    std::vector<nous::vec3>{ m_yTransLinePosition.first, m_yTransLinePosition.second });
            colorLines.emplace_back(nous::vec4(bcf, bcf, bcc + transColorFactor_z, 1.0f),
                                    std::vector<nous::vec3>{ m_zTransLinePosition.first, m_zTransLinePosition.second });
        }
    }

    if(m_mode & GizmoMode::Scale)
    {
        // scale
        auto getScaleHandleMesh = [&](const nous::vec4& color, const auto& scaleTrianglesPosition, const auto& transLinePosition, auto& tris,
                                      auto& lines) {
            for(auto& tri: scaleTrianglesPosition)
            {
                tris.emplace_back(color, std::move(tri));
            }
            lines.emplace_back(color, std::vector<nous::vec3>{ transLinePosition.first, transLinePosition.second });
        };
        getScaleHandleMesh(nous::vec4(bcc + scaleColorFactor_x, bcf, bcf, 1.0f), m_xScaleCubeTriangles, m_xScaleLinePosition, colorTris, colorLines);
        getScaleHandleMesh(nous::vec4(bcf, bcc + scaleColorFactor_y, bcf, 1.0f), m_yScaleCubeTriangles, m_yScaleLinePosition, colorTris, colorLines);
        getScaleHandleMesh(nous::vec4(bcf, bcf, bcc + scaleColorFactor_z, 1.0f), m_zScaleCubeTriangles, m_zScaleLinePosition, colorTris, colorLines);
    }

    if(m_mode & GizmoMode::Rotate)
    {
        // rotate
        auto getRotateHandleMesh = [&](const nous::vec4& color, const auto& rotateLinesPosition, auto& lines) {
            for(size_t i = 0; i < rotateLinesPosition.size(); ++i)
            {
                lines.emplace_back(color,
                                   std::vector<nous::vec3>{ rotateLinesPosition[i], rotateLinesPosition[(i + 1) % rotateLinesPosition.size()] });
            }
        };
        getRotateHandleMesh(nous::vec4(bcc + rotateColorFactor_x, bcf, bcf, 1.f), m_xRotateLinesPosition, colorLines);
        getRotateHandleMesh(nous::vec4(bcf, bcc + rotateColorFactor_y, bcf, 1.f), m_yRotateLinesPosition, colorLines);
        getRotateHandleMesh(nous::vec4(bcf, bcf, bcc + rotateColorFactor_z, 1.f), m_zRotateLinesPosition, colorLines);
    }
}

void HandleGizmo::_updateMesh()
{
    this->_synCameraInfo();

    float scaleFactor;
    if(m_camera_project_type == 0)
    {
        scaleFactor = (m_camera_pos - m_gizmo_position).norm() * 0.0035f * (700.0f / m_camera_view_port.z());
    }
    else
    {
        scaleFactor = 100.0f * (m_camera_ortho_view.y() - m_camera_ortho_view.x()) / m_camera_view_port.w();
    }

    nous::transform3 transform = nous::transform3::Identity();
    transform.translate(m_gizmo_position);
    transform.rotate(m_gizmo_rotation);
    transform.scale(m_gizmo_scale * scaleFactor);
    const nous::mat4& mat = transform.matrix();

    // translate
    auto fillTranslateHandleMesh =
        [&mat](const nous::vec3& transConeTopOrgPos, const std::vector<nous::vec3>& transConeBottomFaceOrgPos, nous::vec3& m_transConeTopPosition,
               std::vector<nous::vec3>& m_transConeBottomFacePosition, std::vector<std::vector<nous::vec3>>& transTrianglesPosition,
               const std::pair<nous::vec3, nous::vec3>& transLineOrgPos, std::pair<nous::vec3, nous::vec3>& transLinePosition) {
            nous::vec4 lineTransPostion0 = mat * nous::vec4(transLineOrgPos.first.x(), transLineOrgPos.first.y(), transLineOrgPos.first.z(), 1.0);
            nous::vec4 lineTransPostion1 = mat * nous::vec4(transLineOrgPos.second.x(), transLineOrgPos.second.y(), transLineOrgPos.second.z(), 1.0);
            transLinePosition.first = lineTransPostion0.head<3>() /= lineTransPostion0.w();
            transLinePosition.second = lineTransPostion1.head<3>() /= lineTransPostion1.w();

            // cone
            m_transConeBottomFacePosition.clear();
            for(const auto& transConeBottomFaceOrgPo: transConeBottomFaceOrgPos)
            {
                nous::vec4 p = mat * nous::vec4(transConeBottomFaceOrgPo.x(), transConeBottomFaceOrgPo.y(), transConeBottomFaceOrgPo.z(), 1.0);
                m_transConeBottomFacePosition.emplace_back(p.head<3>() /= p.w());
            }
            nous::vec4 coneTopTransPostion = mat * nous::vec4(transConeTopOrgPos.x(), transConeTopOrgPos.y(), transConeTopOrgPos.z(), 1.0);
            transTrianglesPosition.clear();
            for(size_t i = 0; i < m_transConeBottomFacePosition.size(); ++i)
            {
                auto& tri = transTrianglesPosition.emplace_back();
                tri.emplace_back(coneTopTransPostion.head<3>() /= coneTopTransPostion.w());
                tri.emplace_back(m_transConeBottomFacePosition[i]);
                tri.emplace_back(m_transConeBottomFacePosition[(i + 1) % m_transConeBottomFacePosition.size()]);
                m_transConeTopPosition = tri.front();
            }
        };

    fillTranslateHandleMesh(m_xTransConeTopOrgPosition, m_xTransConeBottomFaceOrgPosition, m_xTransConeTopPosition, m_xTransConeBottomFacePosition,
                            m_xTransTriangles, m_xTransLineOrgPosition, m_xTransLinePosition);
    fillTranslateHandleMesh(m_yTransConeTopOrgPosition, m_yTransConeBottomFaceOrgPosition, m_yTransConeTopPosition, m_yTransConeBottomFacePosition,
                            m_yTransTriangles, m_yTransLineOrgPosition, m_yTransLinePosition);
    fillTranslateHandleMesh(m_zTransConeTopOrgPosition, m_zTransConeBottomFaceOrgPosition, m_zTransConeTopPosition, m_zTransConeBottomFacePosition,
                            m_zTransTriangles, m_zTransLineOrgPosition, m_zTransLinePosition);

    // scale
    auto fillScaleHandleMesh = [&mat](const std::pair<nous::vec3, nous::vec3>& scaleLineOrgPos, const std::vector<nous::vec3>& scaleCubeOrgPos,
                                      std::pair<nous::vec3, nous::vec3>& scaleLinePosition, std::vector<nous::vec3>& scaleCubePos,
                                      std::vector<std::vector<nous::vec3>>& scaleCubeTris) {
        // line
        nous::vec4 lineTransPostion0 = mat * nous::vec4(scaleLineOrgPos.first.x(), scaleLineOrgPos.first.y(), scaleLineOrgPos.first.z(), 1.0);
        nous::vec4 lineTransPostion1 = mat * nous::vec4(scaleLineOrgPos.second.x(), scaleLineOrgPos.second.y(), scaleLineOrgPos.second.z(), 1.0);
        scaleLinePosition.first = lineTransPostion0.head<3>() /= lineTransPostion0.w();
        scaleLinePosition.second = lineTransPostion1.head<3>() /= lineTransPostion1.w();
        // cube
        scaleCubePos.clear();
        for(const auto& scaleCubeOrgPo: scaleCubeOrgPos)
        {
            nous::vec4 p = mat * nous::vec4(scaleCubeOrgPo.x(), scaleCubeOrgPo.y(), scaleCubeOrgPo.z(), 1.0);
            scaleCubePos.emplace_back(p.head<3>() /= p.w());
        }
        scaleCubeTris.clear();
        // front
        auto& tri0 = scaleCubeTris.emplace_back();
        tri0.emplace_back(scaleCubePos[0]);
        tri0.emplace_back(scaleCubePos[1]);
        tri0.emplace_back(scaleCubePos[2]);
        auto& tri1 = scaleCubeTris.emplace_back();
        tri1.emplace_back(scaleCubePos[0]);
        tri1.emplace_back(scaleCubePos[2]);
        tri1.emplace_back(scaleCubePos[3]);
        // back
        auto& tri2 = scaleCubeTris.emplace_back();
        tri2.emplace_back(scaleCubePos[4]);
        tri2.emplace_back(scaleCubePos[6]);
        tri2.emplace_back(scaleCubePos[5]);
        auto& tri3 = scaleCubeTris.emplace_back();
        tri3.emplace_back(scaleCubePos[6]);
        tri3.emplace_back(scaleCubePos[5]);
        tri3.emplace_back(scaleCubePos[7]);
        // top
        auto& tri4 = scaleCubeTris.emplace_back();
        tri4.emplace_back(scaleCubePos[0]);
        tri4.emplace_back(scaleCubePos[5]);
        tri4.emplace_back(scaleCubePos[1]);
        auto& tri5 = scaleCubeTris.emplace_back();
        tri5.emplace_back(scaleCubePos[0]);
        tri5.emplace_back(scaleCubePos[4]);
        tri5.emplace_back(scaleCubePos[5]);
        // bottom
        auto& tri6 = scaleCubeTris.emplace_back();
        tri6.emplace_back(scaleCubePos[2]);
        tri6.emplace_back(scaleCubePos[7]);
        tri6.emplace_back(scaleCubePos[3]);
        auto& tri7 = scaleCubeTris.emplace_back();
        tri7.emplace_back(scaleCubePos[2]);
        tri7.emplace_back(scaleCubePos[6]);
        tri7.emplace_back(scaleCubePos[7]);
        // left
        auto& tri8 = scaleCubeTris.emplace_back();
        tri8.emplace_back(scaleCubePos[0]);
        tri8.emplace_back(scaleCubePos[4]);
        tri8.emplace_back(scaleCubePos[2]);
        auto& tri9 = scaleCubeTris.emplace_back();
        tri9.emplace_back(scaleCubePos[4]);
        tri9.emplace_back(scaleCubePos[6]);
        tri9.emplace_back(scaleCubePos[2]);
        // right
        auto& tri10 = scaleCubeTris.emplace_back();
        tri10.emplace_back(scaleCubePos[1]);
        tri10.emplace_back(scaleCubePos[5]);
        tri10.emplace_back(scaleCubePos[3]);
        auto& tri11 = scaleCubeTris.emplace_back();
        tri11.emplace_back(scaleCubePos[5]);
        tri11.emplace_back(scaleCubePos[7]);
        tri11.emplace_back(scaleCubePos[3]);
    };

    fillScaleHandleMesh(m_xScaleLineOrgPosition, m_xScaleCubeOrgPosition, m_xScaleLinePosition, m_xScaleCubePosition, m_xScaleCubeTriangles);
    fillScaleHandleMesh(m_yScaleLineOrgPosition, m_yScaleCubeOrgPosition, m_yScaleLinePosition, m_yScaleCubePosition, m_yScaleCubeTriangles);
    fillScaleHandleMesh(m_zScaleLineOrgPosition, m_zScaleCubeOrgPosition, m_zScaleLinePosition, m_zScaleCubePosition, m_zScaleCubeTriangles);

    // rotate
    auto fillRotateHandleMesh = [&mat](const std::vector<nous::vec3>& rotateLinesOrgPos, std::vector<nous::vec3>& rotateLinesPosition) {
        rotateLinesPosition.clear();
        for(auto& line: rotateLinesOrgPos)
        {
            nous::vec4 lineTransPostion0 = mat * nous::vec4(line.x(), line.y(), line.z(), 1.0);
            rotateLinesPosition.emplace_back(lineTransPostion0.head<3>() /= lineTransPostion0.w());
        }
    };

    fillRotateHandleMesh(m_xRotateLinesOrgPosition, m_xRotateLinesPosition);
    fillRotateHandleMesh(m_yRotateLinesOrgPosition, m_yRotateLinesPosition);
    fillRotateHandleMesh(m_zRotateLinesOrgPosition, m_zRotateLinesPosition);
}

HandleGizmo::GizmoPart HandleGizmo::_checkIntersect(const nous::vec2& pickPixel)
{
    auto distcanePointToPixel = [&](const nous::vec3& point, const nous::vec2& pick_pixel) {
        nous::vec4 viewPos = m_camera_matrix * nous::vec4(point.x(), point.y(), point.z(), 1.0f);
        if(viewPos.w() < 0.0)
            return FLT_MAX;
        nous::vec3 nvp = viewPos.head<3>() / viewPos.w();
        nous::vec2 vpn = (nvp.head<2>() + nous::vec2(1.0f, 1.0f)) * 0.5f;
        vpn.x() *= m_camera_view_port.z();
        vpn.y() *= m_camera_view_port.w();
        return (vpn - pick_pixel).squaredNorm();
    };
    auto distanceLineToPixel = [&](const nous::vec3& p0, const nous::vec3& p1, const nous::vec2& pick_pixel) {
        nous::vec3 o;
        nous::vec3 d;
        nous::vec3 intersect0;
        nous::vec3 intersect1;
        this->_getCameraPickRay(pick_pixel, o, d);
        if(nous::algo::line_intersect_3d(o, o + d * 10000.0f, p0, p1, intersect0, intersect1, true))
        {

            nous::vec4 lineIntersectViewPos = m_camera_matrix * nous::vec4(intersect1.x(), intersect1.y(), intersect1.z(), 1.0f);
            if(lineIntersectViewPos.w() < 0.0)
                return FLT_MAX;
            nous::vec3 nvp = lineIntersectViewPos.head<3>() / lineIntersectViewPos.w();
            nous::vec2 vpn = (nvp.head<2>() + nous::vec2(1.0f, 1.0f)) * 0.5f;
            vpn.x() *= m_camera_view_port.z();
            vpn.y() *= m_camera_view_port.w();
            return (vpn - pick_pixel).squaredNorm();
        }
        return FLT_MAX;
    };

    auto getTransPixelDistance = [&](const nous::vec3& top, const std::vector<nous::vec3>& bottom) {
        float minDistance = FLT_MAX;
        float td = distcanePointToPixel(top, pickPixel);
        minDistance = std::min(td, minDistance);
        for(auto& bp: bottom)
        {
            float bd = distcanePointToPixel(bp, pickPixel);
            minDistance = std::min(bd, minDistance);
        }
        return minDistance;
    };

    auto getTransPixelDistanceWithLine = [&](const std::pair<nous::vec3, nous::vec3>& line) {
        float minDistance = FLT_MAX;
        float ld = distanceLineToPixel(line.first, line.second, pickPixel);
        minDistance = std::min(ld, minDistance);
        return minDistance;
    };

    auto getScalePixelDistance = [&](const std::pair<nous::vec3, nous::vec3>& line, const std::vector<nous::vec3>& cube) {
        float minDistance = FLT_MAX;
        float ld = distanceLineToPixel(line.first, line.second, pickPixel);
        minDistance = std::min(ld, minDistance);
        for(auto& cp: cube)
        {
            float cpd = distcanePointToPixel(cp, pickPixel);
            minDistance = std::min(cpd, minDistance);
        }
        return minDistance;
    };

    auto getRotatePixelDistance = [&](const std::vector<nous::vec3>& lines) {
        float minDistance = FLT_MAX;
        for(size_t i = 0; i < lines.size(); ++i)
        {
            float ld = distanceLineToPixel(lines[i], lines[(i + 1) % lines.size()], pickPixel);
            minDistance = std::min(ld, minDistance);
        }
        return minDistance;
    };

    std::map<float, GizmoPart> m_distanceToType;
    if(m_mode & GizmoMode::Translate)
    {
        m_distanceToType[getTransPixelDistance(m_xTransConeTopPosition, m_xTransConeBottomFacePosition)] = GizmoPart::TRANSLATE_X;
        m_distanceToType[getTransPixelDistance(m_yTransConeTopPosition, m_yTransConeBottomFacePosition)] = GizmoPart::TRANSLATE_Y;
        m_distanceToType[getTransPixelDistance(m_zTransConeTopPosition, m_zTransConeBottomFacePosition)] = GizmoPart::TRANSLATE_Z;
        if(!(m_mode & GizmoMode::Scale))
        {
            m_distanceToType[getTransPixelDistanceWithLine(m_xTransLinePosition)] = GizmoPart::TRANSLATE_X;
            m_distanceToType[getTransPixelDistanceWithLine(m_yTransLinePosition)] = GizmoPart::TRANSLATE_Y;
            m_distanceToType[getTransPixelDistanceWithLine(m_zTransLinePosition)] = GizmoPart::TRANSLATE_Z;
        }
    }
    if(m_mode & GizmoMode::Rotate)
    {
        m_distanceToType[getRotatePixelDistance(m_xRotateLinesPosition)] = GizmoPart::ROTATE_X;
        m_distanceToType[getRotatePixelDistance(m_yRotateLinesPosition)] = GizmoPart::ROTATE_Y;
        m_distanceToType[getRotatePixelDistance(m_zRotateLinesPosition)] = GizmoPart::ROTATE_Z;
    }
    if(m_mode & GizmoMode::Scale)
    {
        m_distanceToType[getScalePixelDistance(m_xScaleLinePosition, m_xScaleCubePosition)] = GizmoPart::SCALE_X;
        m_distanceToType[getScalePixelDistance(m_yScaleLinePosition, m_yScaleCubePosition)] = GizmoPart::SCALE_Y;
        m_distanceToType[getScalePixelDistance(m_zScaleLinePosition, m_zScaleCubePosition)] = GizmoPart::SCALE_Z;
    }

    if(!m_distanceToType.empty() && m_distanceToType.begin()->first < static_cast<float>(m_pixelRange))
    {
        return m_distanceToType.begin()->second;
    }

    return GizmoPart::NONE;
}

void HandleGizmo::_translate(const nous::vec2& pickPixel)
{
    nous::vec3 o;
    nous::vec3 d;
    nous::vec3 p0;
    nous::vec3 movePos;
    this->_getCameraPickRay(pickPixel, o, d);
    nous::algo::line_intersect_3d(o, o + d, m_gizmo_position, m_gizmo_position + (m_transPickPos - m_transOrgPos), p0, movePos, false);
    m_gizmo_position = movePos - (m_transPickPos - m_transOrgPos);
    m_manipulate_position = movePos - (m_transPickPos - m_transChangeOrgPos);
}

void HandleGizmo::_rotate(const nous::vec2& pickPixel)
{
    nous::vec3 o;
    nous::vec3 d;
    this->_getCameraPickRay(pickPixel, o, d);
    float denominator = d.dot(m_rotate_normal);
    nous::vec3 ctrlPos;
    if(std::abs(denominator) > std::numeric_limits<float>::epsilon())
    {
        float t = -(o - m_gizmo_position).dot(m_rotate_normal) / denominator;
        ctrlPos = o + t * d;
    }

    nous::vec3 ctrlVec = (ctrlPos - m_gizmo_position).normalized();
    double dotProduct = m_rotateBaseVec.dot(ctrlVec);
    dotProduct = std::clamp(dotProduct, -1.0, 1.0);
    double radian = std::acos(dotProduct);
    nous::vec3 crossProduct = m_rotateBaseVec.cross(ctrlVec);
    if(crossProduct.dot(m_rotate_normal) < 0.f)
    {
        radian = -radian;
    }

    double halfAngle = radian / 2.0;
    double sinHalfAngle = std::sin(halfAngle);
    double cosHalfAngle = std::cos(halfAngle);

    nous::quat changeQuat;
    changeQuat.w() = static_cast<float>(cosHalfAngle);
    changeQuat.x() = static_cast<float>(m_rotate_normal.x() * sinHalfAngle);
    changeQuat.y() = static_cast<float>(m_rotate_normal.y() * sinHalfAngle);
    changeQuat.z() = static_cast<float>(m_rotate_normal.z() * sinHalfAngle);

    // m_gizmoRotation = changeQuat * m_rotateOrg;
    m_manipulate_rotation = changeQuat * m_rotateOrg;
}

void HandleGizmo::_scale(const nous::vec2& pickPixel)
{
    nous::vec3 o;
    nous::vec3 d;
    nous::vec3 p0;
    nous::vec3 movePos;
    nous::vec3 scalePickVec = m_scalePickPos - m_gizmo_position;
    this->_getCameraPickRay(pickPixel, o, d);
    nous::algo::line_intersect_3d(o, o + d, m_gizmo_position, m_gizmo_position + scalePickVec, p0, movePos, false);

    nous::vec3 scaleVec = movePos - m_gizmo_position;
    m_manipulate_scale = m_scaleBase + m_scale_dir * (scaleVec.norm() / scalePickVec.norm() - 1.f);
}
}// namespace sketch
