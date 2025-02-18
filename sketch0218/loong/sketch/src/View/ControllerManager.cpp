#include "sketch/View/ControllerManager.h"

#include "sketch/View/Controller/Controller.h"
#include "sketch/View/Controller/Main/Camera/Controller_Camera_TRS.h"
#include "sketch/View/Controller/Main/Controller_Common_Operate.h"
#include "sketch/View/Controller/Main/Controller_Select.h"
#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_BSpline.h"
#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Circle.h"
#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Polygon.h"
#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Rectangle.h"
#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_StraightLine.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Buffer.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Edit_Nurbs.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_ByPath.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_Freedom.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_Polygon.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Rotate.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Scale.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_TRS.h"
#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Translate.h"

#include "sketch/Common/EventPool.h"

#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Arc.h"
#include <functional>

namespace sketch
{
class ControllerManagerImpl : public ControllerManager
{
public:
    ControllerManagerImpl(void) = default;

    virtual ~ControllerManagerImpl(void) = default;

    virtual bool init() override
    {
        m_controllers[L"Camera_TRS"] = std::make_shared<Controller_Camera_TRS>(L"Camera_TRS");
        m_controllers[L"Common_Operate"] = std::make_shared<Controller_Common_Operate>(L"Common_Operate");
        m_controllers[L"Common_Select"] = std::make_shared<Controller_Select>(L"Common_Select");
        m_controllers[L"Create_Spline"] = std::make_shared<Controller_Create_BSpline>(L"Create_Spline");
        m_controllers[L"Create_Circle"] = std::make_shared<Controller_Create_Circle>(L"Create_Circle");
        m_controllers[L"Create_Rectangle"] = std::make_shared<Controller_Create_Rectangle>(L"Create_Rectangle");
        m_controllers[L"Create_StraightLine"] = std::make_shared<Controller_Create_StraightLine>(L"Create_StraightLine");
        m_controllers[L"Common_Buffer"] = std::make_shared<Controller_Buffer>(L"Common_Buffer");
        m_controllers[L"Edit_Nurbs"] = std::make_shared<Controller_Edit_Nurbs>(L"Edit_Nurbs");
        m_controllers[L"Extrude_Freedom"] = std::make_shared<Controller_Extrude_Freedom>(L"Extrude_Freedom");
        m_controllers[L"Extrude_ByPath"] = std::make_shared<Controller_Extrude_ByPath>(L"Extrude_ByPath");
        m_controllers[L"Extrude_ByNormal"] = std::make_shared<Controller_Extrude_ByNormal>(L"Extrude_ByNormal");
        m_controllers[L"Common_Rotate"] = std::make_shared<Controller_Rotate>(L"Common_Rotate");
        m_controllers[L"Common_Scale"] = std::make_shared<Controller_Scale>(L"Common_Scale");
        m_controllers[L"Common_Translate"] = std::make_shared<Controller_Translate>(L"Common_Translate");
        m_controllers[L"Common_TRS"] = std::make_shared<Controller_TRS>(L"Common_TRS");
        m_controllers[L"Create_Arc"] = std::make_shared<Controller_Create_Arc>(L"Create_Arc");
        m_controllers[L"Create_Polygon"] = std::make_shared<Controller_Create_Polygon>(L"Create_Polygon");

        return true;
    }

    virtual void escape(const SketchParamMap& props) override
    {
        m_controllers[L"Camera_TRS"]->escape(props);
        m_controllers[L"Common_Operate"]->escape(props);
        m_controllers[L"Common_Select"]->escape(props);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->escape(props);
            if(!m_active_controller->is_active())
            {
                // m_active_controller->deactive();
                m_active_controller = nullptr;
            }
        }
    }

    virtual void enter(const SketchParamMap& props) override
    {
        m_controllers[L"Camera_TRS"]->enter(props);
        m_controllers[L"Common_Operate"]->enter(props);
        m_controllers[L"Common_Select"]->enter(props);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->enter(props);
        }
    }

    virtual void update(unsigned long long tm) override
    {
        m_controllers[L"Camera_TRS"]->update(tm);
        m_controllers[L"Common_Operate"]->update(tm);
        m_controllers[L"Common_Select"]->update(tm);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->update(tm);
        }
    }

    virtual void key_down(int key, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->key_down(key, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->key_down(key, ctrl, alt, shift);
        m_controllers[L"Common_Select"]->key_down(key, ctrl, alt, shift);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->key_down(key, ctrl, alt, shift);
        }
    }

    virtual void key_up(int key, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->key_up(key, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->key_up(key, ctrl, alt, shift);
        m_controllers[L"Common_Select"]->key_up(key, ctrl, alt, shift);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->key_up(key, ctrl, alt, shift);
        }
    }

    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->mouse_down(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->mouse_down(button, x, y, ctrl, alt, shift);
        if(button != 0 || !(m_active_controller && m_active_controller->is_active()))
        {
            m_controllers[L"Common_Select"]->mouse_down(button, x, y, ctrl, alt, shift);
        }

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->mouse_down(button, x, y, ctrl, alt, shift);
        }
    }

    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->mouse_move(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->mouse_move(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Select"]->mouse_move(button, x, y, ctrl, alt, shift);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->mouse_move(button, x, y, ctrl, alt, shift);

            SketchParamMap para = m_active_controller->info();
            // para.set_value("value", m_current_value);
            // para.set_value("name", m_name);
            EventPool::instance()->push_event("controller_update", para);
        }
    }

    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->mouse_up(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->mouse_up(button, x, y, ctrl, alt, shift);
        if(button != 0 || !(m_active_controller && m_active_controller->is_active()))
        {
            m_controllers[L"Common_Select"]->mouse_up(button, x, y, ctrl, alt, shift);
        }

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->mouse_up(button, x, y, ctrl, alt, shift);
        }
    }

    virtual void mouse_double_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->mouse_double_click(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->mouse_double_click(button, x, y, ctrl, alt, shift);
        m_controllers[L"Common_Select"]->mouse_double_click(button, x, y, ctrl, alt, shift);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->mouse_double_click(button, x, y, ctrl, alt, shift);
        }
    }

    virtual void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) override
    {
        m_controllers[L"Camera_TRS"]->mouse_wheel(button, x, y, delta, ctrl, alt, shift);
        m_controllers[L"Common_Operate"]->mouse_wheel(button, x, y, delta, ctrl, alt, shift);
        m_controllers[L"Common_Select"]->mouse_wheel(button, x, y, delta, ctrl, alt, shift);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->mouse_wheel(button, x, y, delta, ctrl, alt, shift);
        }
    }

    virtual void event_filter(const std::string& event, const SketchParamMap& props) override
    {
        m_controllers[L"Camera_TRS"]->event_filter(event, props);
        m_controllers[L"Common_Operate"]->event_filter(event, props);
        m_controllers[L"Common_Select"]->event_filter(event, props);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->event_filter(event, props);
        }
    }

    virtual void prepare_render(View* view) override
    {
        m_controllers[L"Camera_TRS"]->prepare_render(view);
        m_controllers[L"Common_Operate"]->prepare_render(view);
        m_controllers[L"Common_Select"]->prepare_render(view);

        if(m_active_controller && m_active_controller->is_active())
        {
            m_active_controller->prepare_render(view);
        }
    }

    virtual Controller* get_controller(const std::wstring& name) override
    {
        auto it = m_controllers.find(name);
        if(it != m_controllers.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    virtual Controller* get_active_controller() override
    {
        if(m_active_controller)
        {
            return m_active_controller.get();
        }
        return nullptr;
    }

    virtual bool active_controller_is_lock() override
    {
        if(m_active_controller)
        {
            return m_active_controller->is_lock();
        }
        return false;
    }

    virtual void active_controller(const std::wstring& name, const SketchParamMap& props) override
    {
        if(m_active_controller)
        {
            m_active_controller->deactive();
            m_active_controller = nullptr;
        }

        auto it = m_controllers.find(name);
        if(it != m_controllers.end())
        {
            if(it->second->accept_activate())
            {
                it->second->active(props);
                m_active_controller = it->second;
                EventPool::instance()->push_event("controller_active", SketchParamMap::alloc({ { "name", name } }));
            }
        }
    }

    virtual void deactive_current_controller() override
    {
        if(m_active_controller)
        {
            EventPool::instance()->push_event("controller_deactive", SketchParamMap::alloc({ { "name", m_active_controller->name() } }));
            m_active_controller->deactive();
            m_active_controller = nullptr;
        }
    }

private:
    std::unordered_map<std::wstring, std::shared_ptr<Controller>> m_controllers;

    std::shared_ptr<Controller> m_active_controller;
};

ControllerManager* ControllerManager::create() { return new ControllerManagerImpl(); }
ControllerManager::~ControllerManager(void) = default;
}// namespace sketch
