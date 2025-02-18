#pragma once

#include "Controller_Main.h"
#include "Engine/Math/Vec2f.h"
#include "Engine/Math/Vec3f.h"
#include "sketch/Core.h"

namespace sketch
{
class TFObjectDoc;
class HandleBase;
class Controller_Select : public Controller_Main
{
public:
    Controller_Select(const std::wstring& name);
    virtual ~Controller_Select();

public:
    virtual SketchParamMap info() const { return SketchParamMap(); }

    virtual void active(const SketchParamMap& props) override;
    virtual void deactive() override;

    virtual void escape(const SketchParamMap& props) override;
    virtual void enter(const SketchParamMap& props) override;
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_double_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

protected:
    void prepare_render(View* view) override;

    void _left_pick(View* view, int x, int y, bool ctrl, bool alt, bool shift);
    void _right_pick(View* view, int x, int y, bool ctrl, bool alt, bool shift);

protected:
    static Controller_Select* m_pInstance;

    bool m_mouse_down;
    bool m_rect_select_mode;
    std::unordered_map<int, Engine::Vec2> m_mouse_down_pos;
    std::unordered_map<int, Engine::Vec2> m_mouseLastPos;
};
}// namespace sketch