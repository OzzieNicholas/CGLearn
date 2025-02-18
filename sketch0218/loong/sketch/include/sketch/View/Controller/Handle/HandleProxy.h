#pragma once

#include "Handle.h"
#include "sketch/View/ViewLayout/CELL_BATCH_BLOCK.h"

namespace sketch
{
class ViewCamera;
class HandleProxy : public Handle
{
public:
    explicit HandleProxy(std::unique_ptr<HandleBase>& handle);
    ~HandleProxy() override;
    HandleProxy(const HandleProxy&) = delete;
    HandleProxy& operator=(const HandleProxy&) = delete;

    [[nodiscard]] const HandleBase* set_handle() const { return m_handle.get(); }

    // override
    void reset(bool t, bool r, bool s) override;
    [[nodiscard]] const nous::mat4 get_transform_matrix() const override;
    int get_id() override;

    void set_enabled(bool enable) override;
    bool is_enabled() const override;
    void set_size(float size) override;
    float get_size() const override;
    void set_visible(bool visible) override { /*do nothing*/ }
    bool is_visible() const override { return false; }

    void set_gizmo_position(const nous::vec3& position) override;
    const nous::vec3& get_gizmo_position() override;
    void set_gizmo_rotation(const nous::quat& rotation) override;
    const nous::quat& get_gizmo_rotation() override;
    void set_gizmo_scale(const nous::vec3& scale) override;
    const nous::vec3& get_gizmo_scale() override;

    void onMouseDown(const nous::vec2& pick_pixel) override;
    void onMouseMove(const nous::vec2& pick_pixel) override;
    void onMouseUp(const nous::vec2& pick_pixel) override;

    void update() override;
    void set_manipulate_position(const nous::vec3& position) override;
    void set_manipulate_rotation(const nous::quat& rotation) override;
    void set_manipulate_scale(const nous::vec3& scale) override;
    [[nodiscard]] bool is_hovered() const override;
    [[nodiscard]] bool is_picked() const override;

    void get_render_mesh(std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorTris,
                         std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>>& colorLines) override;

    static Handle* current_handle() { return m_current_handle; }

    void set_gizmo_drag_type(unsigned int gizmoType) override;

private:
    std::unique_ptr<HandleBase> m_handle;
    static Handle* m_current_handle;
};
}// namespace sketch
