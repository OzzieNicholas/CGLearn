#include "sketch/View/Controller/Handle/HandleDirection.h"

namespace sketch
{
HandleDirection::HandleDirection(ViewCamera* cam)
    : HandleBase(cam), m_direction(nous::vec3(1.0f, 0.0f, 0.0f)), m_pixelRange(5 * 5), m_isPicked(false), m_isHovered(false)
{
    m_box.max() = nous::vec3(1.0f, 1.0f, 1.0f);
    m_box.min() = nous::vec3(-1.0f, -1.0f, -1.0f);
}

void HandleDirection::onMouseDown(const nous::vec2& pickPixel)
{
    m_isPicked = false;
    if(m_isHovered)
    {
        m_isPicked = true;
    }
}

void HandleDirection::onMouseMove(const nous::vec2& pickPixel)
{
    m_isHovered = false;
    if(m_isPicked)
    {
        nous::vec3 o;
        nous::vec3 d;
        nous::vec3 p0;
        nous::vec3 p1;
        this->_getCameraPickRay(pickPixel, o, d);
        nous::algo::line_intersect_3d(o, o + d, m_gizmo_position, m_gizmo_position + m_direction, p0, p1, false);
        m_gizmo_position = p1;
    }
    else
    {
        float minPixelDist = FLT_MAX;
        for(auto& p: m_points)
        {
            nous::vec4 viewPos = m_camera_matrix * nous::vec4(p.x(), p.y(), p.z(), 1.0f);
            if(viewPos.w() < 0.0)
                continue;
            nous::vec3 nvp = viewPos.head<3>() / viewPos.w();
            nous::vec2 vpn = (nvp.head<2>() + nous::vec2(1.0f, 1.0f)) * 0.5f;
            vpn.x() *= m_camera_view_port.z();
            vpn.y() *= m_camera_view_port.w();
            float pixelDist = (vpn - pickPixel).squaredNorm();

            if(pixelDist < m_pixelRange && pixelDist < minPixelDist)
            {
                minPixelDist = pixelDist;
                m_isHovered = true;
                break;
            }
        }
    }
}

void HandleDirection::onMouseUp(const nous::vec2& pickPixel) { m_isPicked = false; }

inline void HandleDirection::update() { _updateMesh(); }

void HandleDirection::getMesh(std::vector<std::pair<nous::vec3, std::vector<nous::vec3>>>& colorTris,
                              std::vector<std::pair<nous::vec3, std::vector<nous::vec3>>>& colorLines)
{
    _updateMesh();
    nous::vec3 color = nous::vec3(0.0f, 0.5f, 0.5f);
    if(m_isHovered)
        color = nous::vec3(0.0f, 1.0f, 1.0f);
    for(auto& tri: m_triangles)
    {
        colorTris.emplace_back(color, std::move(tri));
    }
}

void HandleDirection::_updateMesh()
{
    this->_synCameraInfo();

    float scaleFactor = 1.0f;
    if(m_camera_project_type == 0)
        scaleFactor = (m_camera_pos - m_gizmo_position).norm() * 0.075f * (700.0f / m_camera_view_port.z());
    else
        scaleFactor = 130.0f * (m_camera_ortho_view.y() - m_camera_ortho_view.x()) / m_camera_view_port.w();

    nous::transform3 transform = nous::transform3::Identity();
    transform.translate(m_gizmo_position);
    transform.rotate(m_gizmo_rotation);
    transform.scale(m_gizmo_scale * scaleFactor);
    nous::mat4 mat = transform.matrix();

    m_points.clear();
    nous::vec4 p0 = mat * nous::vec4(m_box.min().x(), m_box.min().y(), m_box.min().z(), 1.0);
    m_points.emplace_back(p0.head<3>() /= p0.w());
    nous::vec4 p1 = mat * nous::vec4(m_box.max().x(), m_box.min().y(), m_box.min().z(), 1.0);
    m_points.emplace_back(p1.head<3>() /= p1.w());
    nous::vec4 p2 = mat * nous::vec4(m_box.max().x(), m_box.max().y(), m_box.min().z(), 1.0);
    m_points.emplace_back(p2.head<3>() /= p2.w());
    nous::vec4 p3 = mat * nous::vec4(m_box.min().x(), m_box.max().y(), m_box.min().z(), 1.0);
    m_points.emplace_back(p3.head<3>() /= p3.w());
    nous::vec4 p4 = mat * nous::vec4(m_box.min().x(), m_box.min().y(), m_box.max().z(), 1.0);
    m_points.emplace_back(p4.head<3>() /= p4.w());
    nous::vec4 p5 = mat * nous::vec4(m_box.max().x(), m_box.min().y(), m_box.max().z(), 1.0);
    m_points.emplace_back(p5.head<3>() /= p5.w());
    nous::vec4 p6 = mat * nous::vec4(m_box.max().x(), m_box.max().y(), m_box.max().z(), 1.0);
    m_points.emplace_back(p6.head<3>() /= p6.w());
    nous::vec4 p7 = mat * nous::vec4(m_box.min().x(), m_box.max().y(), m_box.max().z(), 1.0);
    m_points.emplace_back(p7.head<3>() /= p7.w());

    m_triangles.clear();
    // Front face (min.z face)
    auto& tri0 = m_triangles.emplace_back();
    tri0.emplace_back(m_points[0]);
    tri0.emplace_back(m_points[1]);
    tri0.emplace_back(m_points[2]);
    auto& tri1 = m_triangles.emplace_back();
    tri1.emplace_back(m_points[0]);
    tri1.emplace_back(m_points[2]);
    tri1.emplace_back(m_points[3]);
    // Back face (max.z face)
    auto& tri2 = m_triangles.emplace_back();
    tri2.emplace_back(m_points[4]);
    tri2.emplace_back(m_points[5]);
    tri2.emplace_back(m_points[6]);
    auto& tri3 = m_triangles.emplace_back();
    tri3.emplace_back(m_points[4]);
    tri3.emplace_back(m_points[7]);
    tri3.emplace_back(m_points[6]);
    // Left face (min.x face)
    auto& tri4 = m_triangles.emplace_back();
    tri4.emplace_back(m_points[0]);
    tri4.emplace_back(m_points[3]);
    tri4.emplace_back(m_points[7]);
    auto& tri5 = m_triangles.emplace_back();
    tri5.emplace_back(m_points[0]);
    tri5.emplace_back(m_points[7]);
    tri5.emplace_back(m_points[4]);
    // Right face (max.x face)
    auto& tri6 = m_triangles.emplace_back();
    tri6.emplace_back(m_points[1]);
    tri6.emplace_back(m_points[5]);
    tri6.emplace_back(m_points[6]);
    auto& tri7 = m_triangles.emplace_back();
    tri7.emplace_back(m_points[1]);
    tri7.emplace_back(m_points[6]);
    tri7.emplace_back(m_points[2]);
    // Top face (max.y face)
    auto& tri8 = m_triangles.emplace_back();
    tri8.emplace_back(m_points[3]);
    tri8.emplace_back(m_points[2]);
    tri8.emplace_back(m_points[6]);
    auto& tri9 = m_triangles.emplace_back();
    tri9.emplace_back(m_points[3]);
    tri9.emplace_back(m_points[6]);
    tri9.emplace_back(m_points[7]);
    // Bottom face (min.y face)
    auto& tri10 = m_triangles.emplace_back();
    tri10.emplace_back(m_points[0]);
    tri10.emplace_back(m_points[4]);
    tri10.emplace_back(m_points[5]);
    auto& tri11 = m_triangles.emplace_back();
    tri11.emplace_back(m_points[0]);
    tri11.emplace_back(m_points[5]);
    tri11.emplace_back(m_points[1]);
}
}// namespace sketch