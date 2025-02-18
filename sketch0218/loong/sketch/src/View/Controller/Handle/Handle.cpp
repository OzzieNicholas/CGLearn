#include "sketch/View/Controller/Handle/Handle.h"
#include "sketch/View/Controller/Handle/HandleGizmo.h"
#include "sketch/View/Controller/Handle/HandleProxy.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/STCore.h"
#include "sketch/View/ViewLayout/ViewCamera.h"

namespace sketch
{
std::unique_ptr<Handle> Handle::create(std::string_view name, ViewCamera* cam, Handle::GizmoMode mode)
{
    std::unique_ptr<HandleBase> handle;
    if(name == "gizmo")
    {
        handle = std::make_unique<HandleGizmo>(cam, mode);
    }

    if(!STCore::inst->is_use_internal_gizmo())
    {
        return std::make_unique<HandleProxy>(handle);
    }
    return handle;
}

int HandleBase::s_id_counter = 0;
void HandleBase::_synCameraInfo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    const Engine::Vec3f& cp = pDoc->v2d(m_camera->pos());
    m_camera_pos = nous::vec3(cp.x, cp.y, cp.z);

    const Engine::Vec3f& cd = m_camera->forwardDir();
    m_camera_dir = nous::vec3(cd.x, cd.y, cd.z);

    const Engine::Mat4f& cm = m_camera->dvpMatrix();
    m_camera_matrix.col(0) = nous::vec4(cm.col[0].x, cm.col[0].y, cm.col[0].z, cm.col[0].w);
    m_camera_matrix.col(1) = nous::vec4(cm.col[1].x, cm.col[1].y, cm.col[1].z, cm.col[1].w);
    m_camera_matrix.col(2) = nous::vec4(cm.col[2].x, cm.col[2].y, cm.col[2].z, cm.col[2].w);
    m_camera_matrix.col(3) = nous::vec4(cm.col[3].x, cm.col[3].y, cm.col[3].z, cm.col[3].w);

    const Engine::Vec4f& cv = m_camera->viewport();
    m_camera_view_port = nous::vec4(cv.x, cv.y, cv.z, cv.w);

    const Engine::Vec4f& ov = m_camera->orthoView();
    m_camera_ortho_view = nous::vec4(ov.x, ov.y, ov.z, ov.w);

    m_camera_project_type = m_camera->projType();
}

void HandleBase::_getCameraPickRay(const nous::vec2& pickPixel, nous::vec3& o, nous::vec3& d)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto& vp = m_camera->viewport();
    Engine::Vec3 vOrigin, vDir;
    m_camera->getDocPickRay(pDoc, pickPixel.x() + vp.x, pickPixel.y() + vp.y, &vOrigin, &vDir);

    o.x() = vOrigin.x;
    o.y() = vOrigin.y;
    o.z() = vOrigin.z;
    d.x() = vDir.x;
    d.y() = vDir.y;
    d.z() = vDir.z;
}
}// namespace sketch