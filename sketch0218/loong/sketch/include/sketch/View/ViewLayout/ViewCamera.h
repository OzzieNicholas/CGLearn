#pragma once

#include <string>
#include <vector>

#include "../../Core.h"
#include "../../Document/DocListener.h"
#include "Engine/Math/Plane.h"
#include "Engine/Scene/Camera.h"

namespace sketch
{
class Document;

class ViewCamera : public Engine::Camera
{
public:
    ViewCamera() = default;
    virtual ~ViewCamera() = default;

    void setDirByEuler(float anglH, float anglV);

    inline Engine::Vec2i scr2Viewport(int x, int y) const { return Engine::Vec2i(x - (int)m_viewport.x, y - (int)m_viewport.y); }

    inline Engine::Vec2f getProjByPixel(int x, int y) const
    {
        return Engine::Vec2f((x - m_viewport.x) / m_viewport.z * 2.0f - 1.0f, (y - m_viewport.y) / m_viewport.w * 2.0f - 1.0f);
    }

    void getDocPickRay(Document* pDoc, float x, float y, Engine::Vec3* pOrg, Engine::Vec3* pDir);
    Engine::Vec3 getDocPickNearPoint(Document* pDoc, float x, float y);
    Engine::Vec3 getDocPickFarPoint(Document* pDoc, float x, float y);

protected:
};
}// namespace sketch
