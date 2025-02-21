#pragma once
#include "../Controller_Edit.h"

namespace sketch
{
class View;
class InferenceHelper;
class Controller_Translate : public Controller_Edit
{
public:
    Controller_Translate(const std::wstring& name);
    virtual ~Controller_Translate();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& pProps);
    virtual void deactive();

    // void onSnapPoint(const SnapPoint& pt) override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view) override;

protected:
    virtual void _update_helper_mesh(InferenceHelper* infer_helper);
    void _update_meshes();
    void _gen_meshes(View* view);
    bool _is_select_invalid_element(ElementSelector* selector);

    void _roll_back_to_pick_element();
    void _clear_all_tmp_cache();

protected:
    enum TranslateStep
    {
        PICK_ELEMENT = 0,
        TRANS_BEGIN_FIXING = 1,
        TRANS_FIXING = 2
    };
    TranslateStep m_trans_step;

    float m_trans;
    nous::vec3 m_begin_pos;
    nous::vec3 m_hover_pos;
};
}// namespace sketch
