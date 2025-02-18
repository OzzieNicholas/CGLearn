#include "sketch/View/Controller/Main/Controller_Common_Operate.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/STCore.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout\ViewCamera.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/SmoothGroup.h"

namespace sketch
{
Controller_Common_Operate::Controller_Common_Operate(const std::wstring& name)
    : Controller_Main(name), m_operate_state(OperatorState::NONE), m_clipbord_obj_center(nous::vec3::Zero())
{
}

Controller_Common_Operate::~Controller_Common_Operate() {}

void Controller_Common_Operate::active(const SketchParamMap& props)
{
    Controller_Main::active(props);

    this->set_cursor(Define::CURSOR_ARROW);
}

void Controller_Common_Operate::deactive() { Controller_Main::deactive(); }

void Controller_Common_Operate::escape(const SketchParamMap& props)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    this->set_cursor(Define::CURSOR_ARROW);
    _clear_clipbord(view);
    if(m_operate_state == OperatorState::PASTING)
    {
        m_operate_state = OperatorState::NONE;
    }
}

void Controller_Common_Operate::enter(const SketchParamMap& props) {}

void Controller_Common_Operate::key_down(int key, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(ctrl)
    {
        if(key == 79)
        {// ctrl + o
            view->doc()->modify({ { "proj", Define::VIEWPROJ_ORTHO } });
        }
        else if(key == 80)
        {// ctrl + p
            view->doc()->modify({ { "proj", Define::VIEWPROJ_PERSPECTIVE } });
        }
        else if(key == 67)
        {
            // ctrl+c
            _clear_clipbord(view);
            auto& selectInfo = view->selector()->get_select_info();
            if(selectInfo.size())
            {
                _create_select_obj_to_clipbord(view);
                Engine::OBB bbox;
                for(auto obj: m_clipbord_objs)
                {
                    view->inferHelper()->add_ignore(obj.first->renderObject());
                    bbox.add(obj.first->bbox());
                }
                m_clipbord_obj_center = NousConvert::convert(bbox.center());
                m_operate_state = OperatorState::PASTING;
            }
            view->dirtyRT();
        }
        else if(key == 86)
        {// ctrl+v
            if(m_operate_state == OperatorState::PASTING)
            {
                this->set_cursor(Define::CURSOR_CROSS);
                _paste_obj(view);
            }

            view->dirtyRT();
        }
    }
}

void Controller_Common_Operate::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_operate_state == OperatorState::PASTING)
    {
        this->set_cursor(Define::CURSOR_CROSS);
        _paste_obj(view);
        view->dirtyRT();
    }
}

void Controller_Common_Operate::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_operate_state == OperatorState::PASTING)
    {
        InferenceHelper* infer_helper = view->inferHelper();
        infer_helper->set_infer_filter_type(InferType::ALL_TYPE);
        infer_helper->update_inference(view, x, y);
        auto& inferInfo = infer_helper->get_infer_result();
        if(inferInfo.infer_type != InferType::NONE)
        {
            m_hover_pos = inferInfo.position;
        }
        _show_clipbord_obj(view);
    }
    view->dirtyRT();
}

void Controller_Common_Operate::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Common_Operate::prepare_render(View* view)
{
    if(m_operate_state == OperatorState::PASTING)
    {
        view->batchRenderCtrlPoints(0, false, m_is_snap_mesh, NousConvert::convert(m_hover_pos));
    }
}

void Controller_Common_Operate::_create_select_obj_to_clipbord(View* view)
{
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    SceneDoc* scene = view->doc()->scene();
    cmd_mgr->freezeRvt(scene->rvtId());

    auto& select_info = view->selector()->get_select_info();
    for(auto& obj: select_info)
    {
        RenderObject* cobj = obj.first;
        if(cobj->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* new_mesh = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
            auto new_geo = new_mesh->geodata();

            MeshDoc* org_mesh = static_cast<MeshDoc*>(cobj->doc());
            auto org_geo = org_mesh->geodata();
            Engine::Mat43 tmat = org_mesh->world_matrix();

            std::unordered_map<SketchPointPtr, SketchPointPtr> points_cache;
            for(auto& ele: obj.second.elements)
            {
                if(ele->type() == EElementType::Polygon)
                {
                    std::vector<std::vector<SketchPointPtr>> polygon_points = GeoDataUtils::get().get_polygon_topo(org_geo.get(), ele->to_polygon());

                    std::vector<std::vector<SketchPointPtr>> new_polygon;
                    for(auto ring: polygon_points)
                    {
                        std::vector<SketchPointPtr>& new_ring = new_polygon.emplace_back();
                        for(auto point: ring)
                        {
                            auto& pos = points_cache.find(point);
                            if(pos == points_cache.end())
                            {
                                Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(point));
                                SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                                points_cache[point] = nnp;
                            }
                            new_ring.emplace_back(points_cache[point]);
                        }
                    }
                    new_geo->add_polygon_without_check(new_polygon, true);
                }
                else if(ele->type() == EElementType::Line)
                {
                    SketchLinePtr line = ele->to_line();
                    auto& pos0 = points_cache.find(line->p0);
                    if(pos0 == points_cache.end())
                    {
                        Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line->p0));
                        SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                        points_cache[line->p0] = nnp;
                    }
                    SketchPointPtr p0 = points_cache[line->p0];

                    auto& pos1 = points_cache.find(line->p1);
                    if(pos1 == points_cache.end())
                    {
                        Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line->p1));
                        SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                        points_cache[line->p1] = nnp;
                    }
                    SketchPointPtr p1 = points_cache[line->p1];

                    new_geo->add_line_without_check(p0, p1);
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    ElementGroupPtr line_group = ele->to_group();
                    NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
                    for(auto& line: line_group->elements())
                    {
                        SketchLinePtr line_ptr = line->to_line();
                        auto& pos0 = points_cache.find(line_ptr->p0);
                        if(pos0 == points_cache.end())
                        {
                            Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line_ptr->p0));
                            SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                            points_cache[line_ptr->p0] = nnp;
                        }
                        SketchPointPtr p0 = points_cache[line_ptr->p0];

                        auto& pos1 = points_cache.find(line_ptr->p1);
                        if(pos1 == points_cache.end())
                        {
                            Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line_ptr->p1));
                            SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                            points_cache[line_ptr->p1] = nnp;
                        }
                        SketchPointPtr p1 = points_cache[line_ptr->p1];

                        SketchLinePtr new_line = new_geo->add_line_without_check(p0, p1);
                        new_group->add_element(new_line);
                    }
                    new_geo->add_element_group(new_group);
                }
                else if(ele->type() == EElementType::SmoothGroup)
                {
                    SmoothGroupPtr new_group = std::make_shared<SmoothGroup>();
                    SmoothGroupPtr smooth_group = std::dynamic_pointer_cast<SmoothGroup>(ele->to_group());
                    // line
                    for(auto& line: smooth_group->soft_edge())
                    {
                        SketchLinePtr line_ptr = line->to_line();
                        auto& pos0 = points_cache.find(line_ptr->p0);
                        if(pos0 == points_cache.end())
                        {
                            Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line_ptr->p0));
                            SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                            points_cache[line_ptr->p0] = nnp;
                        }
                        SketchPointPtr p0 = points_cache[line_ptr->p0];

                        auto& pos1 = points_cache.find(line_ptr->p1);
                        if(pos1 == points_cache.end())
                        {
                            Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(line_ptr->p1));
                            SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                            points_cache[line_ptr->p1] = nnp;
                        }
                        SketchPointPtr p1 = points_cache[line_ptr->p1];

                        SketchLinePtr new_line = new_geo->add_line_without_check(p0, p1);
                        new_group->add_element(new_line);
                    }
                    // group
                    for(auto& polygon: smooth_group->soft_polygon())
                    {
                        std::vector<std::vector<SketchPointPtr>> polygon_points =
                            GeoDataUtils::get().get_polygon_topo(org_geo.get(), polygon->to_polygon());

                        std::vector<std::vector<SketchPointPtr>> new_polygon_topo;
                        for(auto ring: polygon_points)
                        {
                            std::vector<SketchPointPtr>& new_ring = new_polygon_topo.emplace_back();
                            for(auto point: ring)
                            {
                                auto& pos = points_cache.find(point);
                                if(pos == points_cache.end())
                                {
                                    Engine::Vec3 global_pos = tmat * NousConvert::convert(org_geo->get_point_position(point));
                                    SketchPointPtr nnp = new_geo->add_point_without_check(nous::vec3(global_pos.x, global_pos.y, global_pos.z));
                                    points_cache[point] = nnp;
                                }
                                new_ring.emplace_back(points_cache[point]);
                            }
                        }
                        SketchPolygonPtr new_polygon = new_geo->add_polygon_without_check(new_polygon_topo, true);

                        new_group->add_element(new_polygon);
                    }
                    new_geo->add_element_group(new_group);
                }
            }

            new_mesh->dirty();
            m_clipbord_objs[new_mesh] = new_mesh->world_matrix();
        }
        else if(cobj->type() == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* new_nurbs = (NurbsDoc*)view->create_obj_in_working_group(Define::OBJ_D5_NURBS);
            auto new_data = new_nurbs->nurbsdata();

            NurbsDoc* org_nurbs = static_cast<NurbsDoc*>(cobj->doc());
            auto org_data = org_nurbs->nurbsdata();

            std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
            new_nurbs->set_matrix_from_world(org_nurbs->world_matrix());
            // nurbs 的 guid 都存在 line_keys 里
            for(auto& ele: obj.second.elements)
            {
                std::shared_ptr<INurbsBase> nurbs_item = org_data->get_item(ele->id());
                if(nurbs_item)
                {
                    new_data->add_item(nurbs_item->clone(remaped_map));
                }
            }
            new_nurbs->dirty();
            m_clipbord_objs[new_nurbs] = new_nurbs->world_matrix();
        }
        else if(cobj->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* new_group = (GroupDoc*)view->create_obj_in_working_group(Define::OBJ_D5_GROUP);
            view->copy_group(new_group, cobj->doc()->_group());
            new_group->dirty();
            m_clipbord_objs[new_group] = new_group->world_matrix();
        }
    }

    cmd_mgr->unfreezeRvt(scene->rvtId());
}

void Controller_Common_Operate::_show_clipbord_obj(View* view)
{
    Engine::Vec3 offset = NousConvert::convert(nous::vec3(m_hover_pos - m_clipbord_obj_center));
    Engine::Mat43 offsetMat;
    offsetMat.translate(offset);

    for(auto obj: m_clipbord_objs)
    {
        obj.first->set_matrix_from_world(offsetMat * obj.second);
        obj.first->dirty();
    }
}

void Controller_Common_Operate::_paste_obj(View* view)
{
    SceneDoc* scene = view->doc()->scene();
    RvtBlock blk(scene->rvtId());

    std::unordered_set<MeshDoc*> new_meshes;
    std::unordered_set<MeshDoc*> clipbord_meshes;
    for(auto obj: m_clipbord_objs)
    {
        TFObjectDoc* past_obj = view->create_obj_in_working_group(obj.first->type());
        past_obj->copy_from(obj.first);
        past_obj->dirty();

        if(obj.first->type() == Define::OBJ_D5_GROUP)
        {
            view->copy_group(past_obj->_group(), obj.first->_group());
            past_obj->dirty();
        }
        else if(obj.first->type() == Define::OBJ_D5_MESH)
        {
            new_meshes.insert(static_cast<MeshDoc*>(past_obj));
            clipbord_meshes.insert(static_cast<MeshDoc*>(obj.first));
        }
    }
    _check_intersection_and_merge_mesh(new_meshes, clipbord_meshes, scene);
}

void Controller_Common_Operate::_clear_clipbord(View* view)
{
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    SceneDoc* scene = view->doc()->scene();
    cmd_mgr->freezeRvt(scene->rvtId());

    for(auto obj: m_clipbord_objs)
    {
        view->delete_obj_and_try_delete_relation(obj.first);
    }
    m_clipbord_objs.clear();

    cmd_mgr->unfreezeRvt(scene->rvtId());
}

}// namespace sketch
