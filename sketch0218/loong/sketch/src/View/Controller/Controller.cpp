#include "sketch/View/Controller/Controller.h"

using namespace Engine;

namespace sketch
{
Controller::Controller(const std::wstring& name) : m_lock(false), m_active(true) { m_name = name; }

Controller::~Controller() {}

void Controller::active(const SketchParamMap& props) { m_active = true; }

void Controller::deactive() { m_active = false; }

void Controller::set_tip(const std::wstring& tip)
{
    m_tip = tip;
    if(UIFuncs::setTip)
    {
        UIFuncs::setTip(m_tip);
    }
}

void Controller::set_cursor(Define::MouseCursor cursor)
{
    m_cursor = cursor;
    if(UIFuncs::setCursor)
    {
        UIFuncs::setCursor(0, m_cursor);
    }
}

}// namespace sketch