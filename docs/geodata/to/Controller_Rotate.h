#pragma once
#include "../Controller_Edit.h"

namespace sketch
{
struct InferResult;
class View;
class InferenceHelper;
class Controller_Rotate : public Controller_Edit
{
public:
    Controller_Rotate(const std::wstring& name);
    virtual ~Controller_Rotate();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& pProps);
    virtual void deactive();

    // void onSnapPoint(const SnapPoint& pt) override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void key_up(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view) override;

protected:
    virtual void _update_helper_mesh(InferenceHelper* infer_helper);
    void _update_meshes(float radian);
    void _gen_meshes(View* view);
    void _gen_infer_operator(const InferResult& infer);
    bool _is_select_invalid_element(ElementSelector* selector);

    void _rollback_to_pick_element();
    void _clear_all_tmp_cache();

protected:
    enum RotateStep
    {
        PICK_ELEMENT = 0,
        ROTATE_PLANE_FIXING = 1,
        ROTATE_BEGIN_FIXING = 2,
        ROTATE_FIXING = 3
    };
    RotateStep m_rotate_step;

    float m_radian;
    nous::vec3 m_rotate_center;
    nous::vec3 m_rotate_normal;
    nous::vec3 m_begin_pos;
    nous::vec3 m_hover_pos;
    nous::vec3 m_ctrl_pos;

    // operator plane
    nous::vec3 m_infer_operator_plane_color;
    std::vector<nous::vec3> m_infer_operator_plane;
    std::vector<std::pair<nous::vec3, nous::vec3>> m_infer_operator_plane_scale_lines;

    // operator line
    std::pair<nous::vec3, nous::vec3> m_infer_operator_proj_to_plane_line;
    std::pair<nous::vec3, nous::vec3> m_infer_operator_rotate_begin_line;
    std::pair<nous::vec3, nous::vec3> m_infer_operator_rotate_dir_line;
};
}// namespace sketch
