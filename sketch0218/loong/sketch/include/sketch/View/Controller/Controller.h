#pragma once

#include "../../UIFuncs.h"
#include "sketch/Object/SketchParamMap.h"

namespace sketch
{
class LayoutCell;
class ConfigureManager;
class ViewCamera;
class View;

class Controller
{
public:
    virtual ~Controller();
    Controller(const std::wstring& name);

    inline const std::wstring& name() const { return m_name; }
    virtual SketchParamMap info() const { return SketchParamMap(); }

    inline bool is_lock() const { return m_lock; }
    inline bool is_active() const { return m_active; }
    inline bool is_busy() const { return false; }
    inline bool accept_activate() { return true; }
    virtual void active(const SketchParamMap& props);
    virtual void deactive();

    virtual void escape(const SketchParamMap& props) {}
    virtual void enter(const SketchParamMap& props) {}
    virtual void key_down(int key, bool ctrl, bool alt, bool shift) {}
    virtual void key_up(int key, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_double_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) {}
    virtual void mouse_out(int button, int x, int y, bool ctrl, bool alt, bool shift) {}
    virtual bool update(long long elapsed_time) { return false; }
    virtual void render(LayoutCell* cell) {}
    virtual void post_render(ViewCamera* cam) {}
    virtual void post_process() {}
    virtual void event_filter(const std::string& event, const SketchParamMap& props) {}
    virtual void set_tip(const std::wstring& tip);
    virtual void set_cursor(Define::MouseCursor cursor);

    virtual void prepare_render(View* view) {};

protected:
protected:
    std::string m_current_value;
    std::wstring m_name;
    bool m_lock;
    bool m_active;
    std::wstring m_tip;
    Define::MouseCursor m_cursor;
};
}// namespace sketch