#pragma once
#include "nous_base/nous_base.h"

namespace sketch
{
class ViewCamera;

// the handle interface
class Handle
{
public:
    enum GizmoMode : uint8_t
    {
        None = 0,
        Translate = 1,
        Rotate = 2,
        Scale = 4,
        TranslateAndRotate = Translate | Rotate,
        TranslateAndScale = Translate | Scale,
        RotateAndScale = Rotate | Scale,
        All = Translate | Rotate | Scale
    };

    virtual ~Handle() = default;

    virtual int get_id() = 0;

    virtual void set_enabled(bool enable) = 0;
    virtual bool is_enabled() const = 0;

    virtual void set_size(float size) = 0;
    virtual float get_size() const = 0;

    virtual void set_visible(bool visible) = 0;
    virtual bool is_visible() const = 0;

    virtual void reset(bool t, bool r, bool s) = 0;
    virtual const nous::mat4 get_transform_matrix() const = 0;
    virtual void set_gizmo_position(const nous::vec3& position) = 0;
    virtual void set_gizmo_rotation(const nous::quat& rotation) = 0;
    virtual void set_gizmo_scale(const nous::vec3& scale) = 0;
    virtual const nous::vec3& get_gizmo_position() = 0;
    virtual const nous::quat& get_gizmo_rotation() = 0;
    virtual const nous::vec3& get_gizmo_scale() = 0;

    virtual void onMouseDown(const nous::vec2& pick_pixel) = 0;
    virtual void onMouseMove(const nous::vec2& pick_pixel) = 0;
    virtual void onMouseUp(const nous::vec2& pick_pixel) = 0;
    virtual void update() = 0;

    virtual void set_manipulate_position(const nous::vec3& position) = 0;
    virtual void set_manipulate_rotation(const nous::quat& rotation) = 0;
    virtual void set_manipulate_scale(const nous::vec3& scale) = 0;
    virtual bool is_hovered() const = 0;
    virtual bool is_picked() const = 0;

    virtual void get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                                 std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines) = 0;

    static std::unique_ptr<Handle> create(std::string_view name, ViewCamera* cam, GizmoMode mode = All);

    virtual void set_gizmo_drag_type(unsigned int gizmoType) = 0;
};

class HandleBase : public Handle
{
public:
    HandleBase(ViewCamera* cam)
        : m_enabled(false), m_visible(true), m_size(1.0f), m_gizmo_scale(nous::vec3(1.f, 1.f, 1.f)), m_gizmo_rotation(nous::quat::Identity()),
          m_manipulate_scale(nous::vec3(1.f, 1.f, 1.f)), m_manipulate_rotation(nous::quat::Identity()),
          m_manipulate_position(nous::vec3(0.f, 0.f, 0.f))
    {
        m_id = s_id_counter++;
        m_camera = cam;
        _synCameraInfo();
    }
    ~HandleBase() override = default;

    void reset(bool t = true, bool r = true, bool s = true) override
    {
        if(t)
        {
            m_manipulate_position = nous::vec3(0.f, 0.f, 0.f);
        }
        if(r)
        {
            m_manipulate_rotation = nous::quat::Identity();
        }
        if(s)
        {
            m_manipulate_scale = nous::vec3(1.f, 1.f, 1.f);
        }
    }

    const nous::mat4 get_transform_matrix() const override
    {
        nous::transform3 transform = nous::transform3::Identity();
        transform.translate(m_gizmo_position);
        transform.scale(m_manipulate_scale);
        transform.rotate(m_manipulate_rotation);
        transform.translate(m_manipulate_position - m_gizmo_position);
        return transform.matrix();
    }
    int get_id() override { return m_id; }

    void set_enabled(bool b) override { m_enabled = b; }
    bool is_enabled() const override { return m_enabled; }

    void set_size(float f) override { m_size = f; }
    float get_size() const override { return m_size; }

    void set_visible(bool v) override { m_visible = v; }
    bool is_visible() const override { return m_visible; }

    void set_gizmo_position(const nous::vec3& position) override { m_gizmo_position = position; }
    const nous::vec3& get_gizmo_position() override { return m_gizmo_position; }

    void set_gizmo_rotation(const nous::quat& rotation) override { m_gizmo_rotation = rotation; }
    const nous::quat& get_gizmo_rotation() override { return m_gizmo_rotation; }

    void set_gizmo_scale(const nous::vec3& scale) override { m_gizmo_scale = scale; }

    const nous::vec3& get_gizmo_scale() override { return m_gizmo_scale; }

    void onMouseDown(const nous::vec2& pickPixel) override {}
    void onMouseMove(const nous::vec2& pickPixel) override {}
    void onMouseUp(const nous::vec2& pickPixel) override {}
    void update() override {}

    void get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                         std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines) override
    {
    }

    void set_manipulate_position(const nous::vec3& position) override { m_manipulate_position = position; }
    void set_manipulate_rotation(const nous::quat& rotation) override { m_manipulate_rotation = rotation; }
    void set_manipulate_scale(const nous::vec3& scale) override { m_manipulate_scale = scale; }

    bool is_hovered() const override { return false; }
    bool is_picked() const override { return false; }

    void set_gizmo_drag_type(unsigned int gizmoType) override {}

protected:
    void _synCameraInfo();
    void _getCameraPickRay(const nous::vec2& pickPixel, nous::vec3& o, nous::vec3& d);

    static int s_id_counter;

    int m_id;
    bool m_enabled;
    float m_size;
    bool m_visible;

    nous::vec3 m_gizmo_position;
    nous::vec3 m_gizmo_scale;
    nous::quat m_gizmo_rotation;

    nous::vec3 m_manipulate_position;
    nous::vec3 m_manipulate_scale;
    nous::quat m_manipulate_rotation;

    ViewCamera* m_camera;
    nous::vec3 m_camera_pos;
    nous::vec3 m_camera_dir;
    nous::mat4 m_camera_matrix;
    nous::vec4 m_camera_view_port;
    nous::vec4 m_camera_ortho_view;
    int m_camera_project_type;
};
}// namespace sketch
