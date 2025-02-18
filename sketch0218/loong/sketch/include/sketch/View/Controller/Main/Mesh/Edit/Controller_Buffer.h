#pragma once
#include "../../Controller_Main.h"

namespace sketch
{
class View;
class Controller_Buffer : public Controller_Main
{
public:
    Controller_Buffer(const std::wstring& name);
    virtual ~Controller_Buffer();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& props);
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
    void _gen_offset_mesh(View* view);
    void _gen_offset(float offset);

    void _roll_back_to_pick_element();

protected:
    enum BufferStep
    {
        PICK_ELEMENT = 0,
        BUFFER_BEGIN_FIXING = 1,
        BUFFER_FIXING = 2
    };
    BufferStep m_buffer_step;

    float m_buffer_offset;
    nous::vec3 m_begin_pos;
    nous::vec3 m_end_pos;
    nous::vec3 m_buffer_normal;
    std::vector<nous::vec3> m_org_ring;
    std::vector<nous::vec3> m_buffer_ring;

    nous::vec3 m_ctrl_pos;
    nous::vec3 m_hover_pos;
};
}// namespace sketch
