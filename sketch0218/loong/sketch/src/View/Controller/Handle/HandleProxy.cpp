#include "sketch/View/Controller/Handle/HandleProxy.h"
#include "Sketch/Common/EventPool.h"

namespace sketch
{
Handle* HandleProxy::m_current_handle = nullptr;

HandleProxy::HandleProxy(std::unique_ptr<HandleBase>& handle) { m_handle = std::move(handle); }

HandleProxy::~HandleProxy()
{
    if(m_current_handle == this)
    {
        m_current_handle = nullptr;
    }
}

void HandleProxy::reset(bool t, bool r, bool s)
{
    if(m_handle)
    {
        m_handle->reset(t, r, s);
    }
}
const nous::mat4 HandleProxy::get_transform_matrix() const
{
    if(m_handle)
    {
        return m_handle->get_transform_matrix();
    }

    return nous::mat4();
}
int HandleProxy::get_id()
{
    if(m_handle)
    {
        return m_handle->get_id();
    }
    return -1;
}
void HandleProxy::set_enabled(bool enable)
{
    if(m_handle)
    {
        m_handle->set_enabled(enable);
        enable ? m_current_handle = this : m_current_handle = nullptr;
    }

    enable ? EventPool::instance()->push_event("gizmo_enable", {}) : EventPool::instance()->push_event("gizmo_disable", {});
}
bool HandleProxy::is_enabled() const
{
    if(m_handle)
    {
        return m_handle->is_enabled();
    }
    return false;
}
void HandleProxy::set_size(float size)
{
    if(m_handle)
    {
        m_handle->set_size(size);
    }
}
float HandleProxy::get_size() const
{
    if(m_handle)
    {
        return m_handle->get_size();
    }
    return 0.0f;
}
void HandleProxy::set_gizmo_position(const nous::vec3& position)
{
    if(m_handle)
    {
        m_handle->set_gizmo_position(position);
    }
}

const nous::vec3& HandleProxy::get_gizmo_position()
{
    if(m_handle)
    {
        return m_handle->get_gizmo_position();
    }

    return nous::vec3();
}
void HandleProxy::set_gizmo_rotation(const nous::quat& rotation)
{
    if(m_handle)
    {
        m_handle->set_gizmo_rotation(rotation);
    }
}
const nous::quat& HandleProxy::get_gizmo_rotation()
{
    if(m_handle)
    {
        return m_handle->get_gizmo_rotation();
    }
    return nous::quat();
}
void HandleProxy::set_gizmo_scale(const nous::vec3& scale)
{
    if(m_handle)
    {
        m_handle->set_gizmo_scale(scale);
    }
}
const nous::vec3& HandleProxy::get_gizmo_scale()
{
    if(m_handle)
    {
        return m_handle->get_gizmo_scale();
    }
    return nous::vec3();
}
void HandleProxy::onMouseDown(const nous::vec2& pick_pixel)
{
    // do nothing
}
void HandleProxy::onMouseMove(const nous::vec2& pick_pixel)
{
    // do nothing
}
void HandleProxy::onMouseUp(const nous::vec2& pick_pixel)
{
    // do nothing
}
void HandleProxy::update()
{
    if(m_handle)
    {
        m_handle->update();
    }
}
void HandleProxy::get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                                  std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines)
{
    if(m_handle)
    {
        m_handle->get_render_mesh(colorTris, colorLines);
    }
}

void HandleProxy::set_manipulate_position(const nous::vec3& position)
{
    if(m_handle)
    {
        m_handle->set_manipulate_position(position);
    }
}

void HandleProxy::set_manipulate_rotation(const nous::quat& rotation)
{
    if(m_handle)
    {
        m_handle->set_manipulate_rotation(rotation);
    }
}

void HandleProxy::set_manipulate_scale(const nous::vec3& scale)
{
    if(m_handle)
    {
        m_handle->set_manipulate_scale(scale);
    }
}

bool HandleProxy::is_hovered() const
{
    if(m_handle)
    {
        return m_handle->is_hovered();
    }
    return false;
}
bool HandleProxy::is_picked() const
{
    if(m_handle)
    {
        return m_handle->is_picked();
    }
    return false;
}

void HandleProxy::set_gizmo_drag_type(unsigned int gizmoType)
{
    if(m_handle)
    {
        m_handle->set_gizmo_drag_type(gizmoType);
    }
}

}// namespace sketch