#pragma once
#include "Handle.h"
#include "nous_mesh/algorithm/intersect.h"

namespace sketch
{
class HandleDirection : public HandleBase
{
public:
    HandleDirection(ViewCamera* cam);
    ~HandleDirection() noexcept override = default;

    inline void setPixelRange(int size) { m_pixelRange = size; }
    inline void setDir(const nous::vec3& dir)
    {
        m_direction = dir.normalized();
        m_gizmo_rotation = nous::quat::FromTwoVectors(nous::vec3(0.f, 0.f, 1.f), m_direction);
    }
    inline const nous::vec3& getDir() { return m_direction; }

    void onMouseDown(const nous::vec2& pickPixel) override;
    void onMouseMove(const nous::vec2& pickPixel) override;
    void onMouseUp(const nous::vec2& pickPixel) override;
    void update() override;

    virtual void getMesh(std::vector<std::pair<nous::vec3, std::vector<nous::vec3>>>& colorTris,
                         std::vector<std::pair<nous::vec3, std::vector<nous::vec3>>>& colorLines);

protected:
    void _updateMesh();

protected:
    nous::vec3 m_direction;
    int m_pixelRange;
    bool m_isPicked;
    bool m_isHovered;
    nous::AABB3 m_box;
    std::vector<nous::vec3> m_points;
    std::vector<std::vector<nous::vec3>> m_triangles;
};
}// namespace sketch