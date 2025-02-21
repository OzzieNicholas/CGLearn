#pragma once

#include "../../Controller_Main.h"
#include "sketch/View/Select/InferenceHelper.h"

namespace sketch
{
class Controller_Create_StraightLine : public Controller_Main
{
public:
    Controller_Create_StraightLine(const std::wstring& name);
    ~Controller_Create_StraightLine() override;

    virtual SketchParamMap info() const;

    void active(const SketchParamMap& props) override;
    void deactive() override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void key_up(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

protected:
    bool _gen_straight_line(View* view);
    void _add_hovered_reference(InferenceHelper* infer_helper, const InferResult& infer);
    void prepare_render(View* view) override;

    void _roll_back_to_pending();

private:
    nous::vec3 m_begin_pos;
    nous::vec3 m_hover_pos;

    enum StraightLineStep
    {
        PENDING = 0,
        FIXING = 1
    };
    StraightLineStep m_straight_line_step;
    bool m_need_update_reference;
};
}// namespace sketch