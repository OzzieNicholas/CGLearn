#pragma once
#include "sketch/Object/SketchParamMap.h"

namespace sketch
{
class View;
class Controller;
class ControllerManager
{
public:
    static ControllerManager* create();
    virtual ~ControllerManager(void);

    virtual bool init() = 0;

    virtual void escape(const SketchParamMap& props) = 0;
    virtual void enter(const SketchParamMap& props) = 0;
    virtual void update(unsigned long long tm) = 0;
    virtual void key_down(int key, bool ctrl, bool alt, bool shift) = 0;
    virtual void key_up(int key, bool ctrl, bool alt, bool shift) = 0;
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) = 0;
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) = 0;
    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) = 0;
    virtual void mouse_double_click(int button, int x, int y, bool ctrl, bool alt, bool shift) = 0;
    virtual void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) = 0;
    virtual void event_filter(const std::string& event, const SketchParamMap& props) {}

    virtual void prepare_render(View* view) = 0;

    virtual Controller* get_controller(const std::wstring& name) = 0;
    virtual Controller* get_active_controller() = 0;
    virtual bool active_controller_is_lock() = 0;

    virtual void active_controller(const std::wstring& name, const SketchParamMap& props) = 0;
    virtual void deactive_current_controller() = 0;
};

}// namespace sketch
