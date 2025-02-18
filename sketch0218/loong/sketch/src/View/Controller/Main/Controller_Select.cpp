#include "sketch/View/Controller/Main/Controller_Select.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Common/EventPool.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/Controller/Handle/Handle.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
Controller_Select::Controller_Select(const std::wstring& name) : Controller_Main(name), m_mouse_down(false), m_rect_select_mode(false) {}

Controller_Select::~Controller_Select() {}

void Controller_Select::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    this->set_tip(L"Select");
    this->set_cursor(Define::CURSOR_ARROW);
}

void Controller_Select::deactive() { Controller_Main::deactive(); }

void Controller_Select::escape(const SketchParamMap& props)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->selector()->clear();
    TFObjectDoc* current_group = view->working_group_area();
    if(current_group)
    {
        view->set_working_group_area(current_group->parent());
    }
}

void Controller_Select::enter(const SketchParamMap& props) {}

void Controller_Select::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    m_mouse_down = true;
    m_mouse_down_pos[button] = Engine::Vec2((float)x, (float)y);

    m_rect_select_mode = false;
}

void Controller_Select::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(m_mouse_down)
    {
        m_mouseLastPos[button] = Engine::Vec2((float)x, (float)y);
        if(button == 0 &&
           (fabs(m_mouseLastPos[button].x - m_mouse_down_pos[button].x) > 6.0 || fabs(m_mouseLastPos[button].y - m_mouse_down_pos[button].y) > 6.0))
        {
            View* view = STCore::inst->viewMgr()->layout()->focusedView();
            if(view == nullptr)
                return;
            view->dirtyRT();
            m_rect_select_mode = true;
        }
        else
        {
            m_rect_select_mode = false;
        }
    }
}

void Controller_Select::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(!m_mouse_down)
        return;

    m_mouse_down = false;
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(button == 0)
    {
        _left_pick(view, x, y, ctrl, alt, shift);
    }
    else if(button == 2)
    {
        _right_pick(view, x, y, ctrl, alt, shift);
    }
}

void Controller_Select::mouse_double_click(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    ElementSelector* selector = view->selector();
    selector->clear();

    auto pick_info = selector->get_select_info(view, x, y);
    if(!pick_info.mesh)
    {
        return;
    }

    TFObjectDoc* pick_obj = view->get_obj_highest_parent_in_working_group_area(pick_info.mesh->doc());
    if(pick_obj)
    {
        if(pick_obj->type() == Define::ObjectType::OBJ_D5_GROUP)
        {
            view->selector()->clear();
            view->set_working_group_area(pick_obj);
        }
        else if(pick_obj->type() == Define::ObjectType::OBJ_D5_MESH)
        {
            selector->add_select_element(pick_info.mesh, pick_info.elements);
        }
    }
}

void Controller_Select::prepare_render(View* view)
{
    if(m_rect_select_mode)
    {
        const Engine::Vec4f& viewport = view->camera()->viewport();
        Vec2f vp0 = (m_mouse_down_pos[0] - viewport.xy) / viewport.zw * 2.0f - 1.0f;
        Vec2f vp1 = (m_mouseLastPos[0] - viewport.xy) / viewport.zw * 2.0f - 1.0f;

        uint16_t patten = m_mouse_down_pos[0].x <= m_mouseLastPos[0].x ? 0xffff : 0x00ff;

        View* pView = STCore::inst->viewMgr()->layout()->focusedView();
        if(auto render = pView->render())
        {
            render->append_render_screen_line(1, 1.0f, { 1.0f, 1.0f, 1.0f }, { vp0.x, vp0.y, 0.0f }, { vp1.x, vp0.y, 0.0f }, patten, 1, false);
            render->append_render_screen_line(1, 1.0f, { 1.0f, 1.0f, 1.0f }, { vp1.x, vp0.y, 0.0f }, { vp1.x, vp1.y, 0.0f }, patten, 1, false);
            render->append_render_screen_line(1, 1.0f, { 1.0f, 1.0f, 1.0f }, { vp1.x, vp1.y, 0.0f }, { vp0.x, vp1.y, 0.0f }, patten, 1, false);
            render->append_render_screen_line(1, 1.0f, { 1.0f, 1.0f, 1.0f }, { vp0.x, vp1.y, 0.0f }, { vp0.x, vp0.y, 0.0f }, patten, 1, false);
        }
        // STE::renderer()->renderRectLine2D(Mat4f::IDENTITY, vp0.x, vp0.y, vp1.x, vp1.y, Vec4f::WHITE, 1.0f, 1, patten);
    }
}

void Controller_Select::_left_pick(View* view, int x, int y, bool ctrl, bool alt, bool shift)
{
    ElementSelector* selector = view->selector();
    auto cam = view->camera();
    const Vec4f& vp = cam->viewport();

    // select objects
    uint8_t smode = ElementSelector::SelectMode::NORMAL;
    if(ctrl && shift)
    {
        smode = ElementSelector::SelectMode::SUB;
    }
    else if(ctrl)
    {
        smode = ElementSelector::SelectMode::ADD;
    }
    else if(shift)
    {
        smode = ElementSelector::SelectMode::ADD | ElementSelector::SelectMode::SUB;
    }
    else
    {
        smode = ElementSelector::SelectMode::NORMAL;
    }

    selector->setSelectMode(ElementSelector::SelectMode(smode));

    if(m_rect_select_mode)
    {// quad pick
        m_rect_select_mode = false;
        int x0 = (int)STMIN(m_mouse_down_pos[0].x, m_mouseLastPos[0].x);
        int y0 = (int)STMIN(m_mouse_down_pos[0].y, m_mouseLastPos[0].y);
        int x1 = (int)STMAX(m_mouse_down_pos[0].x, m_mouseLastPos[0].x);
        int y1 = (int)STMAX(m_mouse_down_pos[0].y, m_mouseLastPos[0].y);

        bool bm = m_mouse_down_pos[0].x <= m_mouseLastPos[0].x;
        selector->update(view, x0, y0, x1, y1, bm);
    }
    else if(RANGE_IN(m_mouse_down_pos[0].x, 0.0, vp.z) && RANGE_IN(m_mouse_down_pos[0].y, 0.0, vp.w))
    {// single pick
        selector->update(view, x, y);
    }

    // 判断是否需要切换工作组
    selector->select_filter_view_group_area(view);
    auto& select_info = selector->get_select_info();
    if(!select_info.size())
    {
        TFObjectDoc* current_group = view->working_group_area();
        if(current_group)
        {
            Engine::OBB working_obb = current_group->bbox();
            working_obb.set(working_obb.m_min - Engine::Vec3(20.0), working_obb.m_max + Engine::Vec3(20.0), working_obb.m_matrix);

            Engine::Vec3 vOrigin, vDir;
            Document* doc = STCore::inst->docMgr()->document();
            cam->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &vOrigin, &vDir);
            if(!working_obb.intersectRay(vOrigin, vDir))
            {
                view->selector()->clear();
                view->set_working_group_area(current_group->parent());
            }
        }
    }
}

void Controller_Select::_right_pick(View* view, int x, int y, bool ctrl, bool alt, bool shift)
{
    SketchParamMap para;
    para.set_value("close_group", false);
    para.set_value("create_group", false);
    para.set_value("disperse_group", false);
    para.set_value("convert_to_geodata", false);
    para.set_value("name", m_name);
    if(view->working_group_area())
    {
        para.set_value("close_group", true);
    }

    ElementSelector* selector = view->selector();
    auto& select_info = selector->get_select_info();
    auto pick_info = selector->get_select_info(view, x, y);
    if(!pick_info.mesh)
    {
        EventPool::instance()->push_event("show_right_menu", para);
        return;
    }
    TFObjectDoc* pick_obj = view->get_obj_highest_parent_in_working_group_area(pick_info.mesh->doc());
    if(!pick_obj)
    {
        // 不在当前工作区域内
        EventPool::instance()->push_event("show_right_menu", para);
        return;
    }
    bool pick_on_select = false;
    int select_num = 0;
    int select_sketch_line_num = 0;
    bool pick_mesh_group = false;
    bool pick_weld_line = false;
    bool pick_smooth_group = false;
    bool pick_nurbs_items = false;
    for(auto& mesh_select: select_info)
    {
        GroupDoc* group = mesh_select.first->doc()->_group();
        if(group)
        {
            pick_mesh_group = true;
            if(group == pick_obj->_group())
            {
                pick_on_select = true;
            }
            select_num += 1;
        }
        for(auto& ele: mesh_select.second.elements)
        {
            if(pick_info.mesh == mesh_select.first && pick_info.elements.find(ele) != pick_info.elements.end())
            {
                pick_on_select = true;
            }
            if(ele->type() == EElementType::Line)
            {
                select_sketch_line_num += 1;
            }
            else if(ele->type() == EElementType::NoBranchLineGroup)
            {
                select_sketch_line_num += 1;
                for(auto& pick_ele: pick_info.elements)
                {
                    if(pick_ele->group() == ele)
                    {
                        pick_on_select = true;
                    }
                }
                pick_weld_line = true;
            }
            else if(ele->type() == EElementType::SmoothGroup)
            {
                pick_smooth_group = true;
                for(auto& pick_ele: pick_info.elements)
                {
                    if(pick_ele->group() == ele)
                    {
                        pick_on_select = true;
                    }
                }
            }
            else if(ele->type() == EElementType::Curve || ele->type() == EElementType::TrimmedSurface)
            {
                pick_nurbs_items = true;
                for(auto& pick_ele: pick_info.elements)
                {
                    if(pick_ele->group() == ele)
                    {
                        pick_on_select = true;
                    }
                }
            }
            select_num += 1;
        }
    }
    if(pick_on_select)
    {
        // 创建组
        if(select_num >= 2)
        {
            para.set_value("create_group", true);
        }
        else
        {
            para.set_value("create_group", false);
        }
        // 分散组
        if(pick_mesh_group)
        {
            para.set_value("disperse_group", true);
        }
        else
        {
            para.set_value("disperse_group", false);
        }
        // 离散化曲线曲面
        if(pick_nurbs_items)
        {
            para.set_value("convert_to_geodata", true);
        }
        else
        {
            para.set_value("convert_to_geodata", false);
        }
        // 焊接线
        if(select_sketch_line_num >= 2)
        {
            para.set_value("weld_line", true);
        }
        else
        {
            para.set_value("weld_line", false);
        }
        // 分解焊接线
        if(pick_weld_line)
        {
            para.set_value("disperse_weld_line", true);
        }
        else
        {
            para.set_value("disperse_weld_line", false);
        }
        // 柔化线
        if(select_sketch_line_num >= 1)
        {
            para.set_value("soft_line", true);
        }
        else
        {
            para.set_value("soft_line", false);
        }
        // 取消柔滑
        if(pick_smooth_group)
        {
            para.set_value("disperse_smooth_group", true);
        }
        else
        {
            para.set_value("disperse_smooth_group", false);
        }
    }
    else
    {
        selector->clear();
        if(pick_obj->type() == Define::ObjectType::OBJ_D5_GROUP)
        {
            para.set_value("create_group", false);
            para.set_value("disperse_group", true);
            selector->add_select_element(pick_obj->renderObject(), std::unordered_set<SketchElementPtr>());
        }
        else if(pick_obj->type() == Define::ObjectType::OBJ_D5_MESH)
        {
            selector->add_select_element(pick_info.mesh, pick_info.elements);
        }
    }
    EventPool::instance()->push_event("show_right_menu", para);
}

}// namespace sketch