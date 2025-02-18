#pragma once
#include "../Controller_Edit.h"

namespace sketch
{
class View;
class InferenceHelper;
class Controller_Scale : public Controller_Edit
{
public:
    Controller_Scale(const std::wstring& name);
    virtual ~Controller_Scale();

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

    void prepare_render(View* view) override;

protected:
    virtual void _update_helper_mesh(InferenceHelper* infer_helper);
    void _update_meshes(float scale);
    void _gen_meshes(View* view);
    bool _is_select_invalid_element(ElementSelector* selector);

    void _roll_back_to_pick_element();
    void _clear_all_tmp_cache();

protected:
    enum ScaleStep
    {
        PICK_ELEMENT = 0,
        SCALE_BEGIN_FIXING = 1,
        SCALE_PICK_BEGIN_POS = 2,
        SCALE_PICK_END_POS = 3,
        SCALE_FIXING = 4
    };
    ScaleStep m_scale_step;

    float m_scale;
    nous::vec3 m_end_pos;
    nous::vec3 m_begin_pos;

    nous::vec3 m_ctrl_pos;
    nous::vec3 m_hover_pos;
    bool m_center_scale_mode;
};
}// namespace sketch
