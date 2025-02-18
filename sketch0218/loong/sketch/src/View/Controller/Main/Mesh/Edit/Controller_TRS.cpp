#include "sketch/View/Controller/Main/Mesh/Edit/Controller_TRS.h"
#include "sketch/View/Controller/Handle/HandleGizmo.h"

#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/NousEngine/SketchMath.h"

namespace sketch
{
Controller_TRS::Controller_TRS(const std::wstring& name) : Controller_Edit(name) { m_keep_connection = true; }

Controller_TRS::~Controller_TRS() {}

inline SketchParamMap Controller_TRS::info() const { return SketchParamMap(); }

void Controller_TRS::active(const SketchParamMap& props)
{
    Controller_Edit::active(props);
    _clear_affected_meshes();

    this->set_tip(L"TRS");
    this->set_cursor(Define::CURSOR_CROSS);

    _deactive_handle();

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->selector()->select_filter_view_group_area(view);
    if(!_is_select_invalid_element(view->selector()))
    {
        RvtBlock blk(view->doc()->scene()->rvtId());
        _prepar_affect_meshes_elements(view->selector()->get_select_info());
        _active_handle();
    }
}

void Controller_TRS::deactive() { Controller_Edit::deactive(); }

void Controller_TRS::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_handle_gizmo)
    {
        _finish_TRS_operation();
    }
    else
    {
        this->deactive();
    }
}

void Controller_TRS::enter(const SketchParamMap& props) { Controller_Main::enter(props); }

void Controller_TRS::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_TRS::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_handle_gizmo == nullptr || !m_handle_gizmo->is_enabled())
    {
        view->selector()->select_filter_view_group_area(view);
        if(!_is_select_invalid_element(view->selector()))
        {
            RvtBlock blk(view->doc()->scene()->rvtId());
            _prepar_affect_meshes_elements(view->selector()->get_select_info());
            _active_handle();
        }
    }
    else
    {
        m_handle_gizmo->onMouseDown(nous::vec2((float)x, (float)y));
    }
}

void Controller_TRS::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    if(m_handle_gizmo == nullptr || !m_handle_gizmo->is_enabled())
    {
        InferenceHelper* infer_helper = view->inferHelper();
        infer_helper->update_inference(view, x, y);
        auto& infer_info = infer_helper->get_infer_result();

        view->selector()->clear();
        if(infer_info.infer_type & InferType::ELEMENT_TYPE)
        {
            if(infer_info.mesh)
            {
                TFObjectDoc* group_doc = view->get_obj_highest_parent_in_working_group_area(infer_info.mesh->doc());
                if(group_doc && group_doc->_group())
                {
                    view->selector()->add_select_element(group_doc->renderObject(), std::unordered_set<SketchElementPtr>());
                }
                else
                {
                    std::unordered_set<SketchElementPtr> elements;
                    if(infer_info.infer_type & InferType::POINT || infer_info.infer_type & InferType::LINE ||
                       infer_info.infer_type & InferType::POLYGON || infer_info.infer_type & InferType::CURVE ||
                       infer_info.infer_type & InferType::SURFACE)
                    {
                        SketchElementPtr valid_element = infer_info.element->group();
                        elements.insert(valid_element ? valid_element : infer_info.element);
                    }
                    view->selector()->add_select_element(infer_info.mesh, elements);
                }
            }
        }
        // pCaster->setHightLightSnap(true);
    }
    else
    {
        if(m_handle_gizmo->is_enabled())
        {
            m_handle_gizmo->onMouseMove(nous::vec2((float)x, (float)y));
        }
        // Engine::Vec3 transOffset = GeoData::conversNousVec3(m_handles[0]->getPosition() - m_handleBasePoints[0]);
        // auto trans = m_handle_gizmo->get_change_matrix();
        // auto posture = m_handle_gizmo->get_last_posture_matrix();

        for(auto& bundle: m_affected_bundles)
        {
            nous::mat4 trans_mat = m_handle_gizmo->get_transform_matrix();
            const Engine::Mat43 engine_trans_mat = NousConvert::convert(trans_mat);
            bundle.first->set_matrix_from_world(engine_trans_mat * NousConvert::convert(bundle.second));
        }

        for(auto& nurbs_change_info: m_affected_nurbs_points)
        {
            NurbsDoc* nurbs_doc = nurbs_change_info.first;
            Engine::Mat43 mat = nurbs_doc->world_matrix();
            Engine::Mat43 inv_mat = nurbs_doc->world_matrix().inverse();
            for(auto& ctp_ptr: nurbs_change_info.second)
            {
                const nous::pos_t ori_pos = ctp_ptr.second;
                nous::vec4 trans_pos = m_handle_gizmo->get_transform_matrix() * nous::vec4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0f);
                const nous::pos_t new_pos = NousConvert::convert(inv_mat * NousConvert::convert(nous::vec3(trans_pos.head<3>() / trans_pos.w())));
                ctp_ptr.first->set_position(new_pos);
            }
            nurbs_doc->dirty();
        }

        for(auto& mesh_change_info: m_affected_mesh_local_position)
        {
            Engine::Mat43 inv_mat = mesh_change_info.first->world_matrix_inv();
            auto geo = mesh_change_info.first->geodata();

            std::vector<std::pair<SketchPointPtr, nous::vec3>> new_pos;
            for(auto& point: mesh_change_info.second)
            {
                nous::vec4 trans_pos =
                    m_handle_gizmo->get_transform_matrix() * nous::vec4(point.second.x(), point.second.y(), point.second.z(), 1.0f);
                Engine::Vec3 pos = inv_mat * NousConvert::convert(nous::vec3(trans_pos.head<3>() / trans_pos.w()));
                new_pos.emplace_back(point.first, NousConvert::convert(pos));
            }
            GeoDataTopoOperations::get().change_point_positon(geo.get(), new_pos);
            // mesh->dirty();
        }
        for(auto& affected_mesh_polygon: m_affected_mesh_polygons)
        {
            for(auto& polygons: affected_mesh_polygon.second)
            {
                polygons->m_changed = true;
            }
        }
    }
    view->dirtyRT();
}

void Controller_TRS::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(m_handle_gizmo && m_handle_gizmo->is_enabled())
    {
        m_handle_gizmo->onMouseUp(nous::vec2((float)x, (float)y));
    }
}

void Controller_TRS::prepare_render(View* view)
{
    if(m_handle_gizmo && m_handle_gizmo->is_enabled() && m_handle_gizmo->is_visible())
    {
        std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>> colorTris;
        std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>> colorLines;
        m_handle_gizmo->get_render_mesh(colorTris, colorLines);
        for(auto tri: colorTris)
        {
            view->batchRenderTri(NousConvert::convert(nous::vec3(tri.first.head<3>())), NousConvert::convert(tri.second[0]),
                                 NousConvert::convert(tri.second[1]), NousConvert::convert(tri.second[2]), false, true, false, 1.0f);
        }
        for(auto line: colorLines)
        {
            view->batchRenderLine(1.f, 1.f, NousConvert::convert(nous::vec3(line.first.head<3>())), NousConvert::convert(line.second[0]),
                                  NousConvert::convert(line.second[1]), 0xffffu, 1, false, false);
        }
    }
}

void Controller_TRS::_active_handle()
{
    _calculateBox();

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ViewCamera* cam = view->camera();
    m_handle_gizmo = Handle::create("gizmo", cam);
    m_handle_gizmo->set_enabled(true);

    HandleGizmo* gizmo_handle = static_cast<HandleGizmo*>(m_handle_gizmo.get());
    gizmo_handle->set_gizmo_position(NousConvert::convert(m_pick_mesh_box.center()));
}

void Controller_TRS::_deactive_handle()
{
    if(m_handle_gizmo != nullptr)
    {
        m_handle_gizmo->set_enabled(false);
        m_handle_gizmo.reset();
    }
}

void Controller_TRS::_calculateBox()
{
    m_pick_mesh_box.m_max = Engine::Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    m_pick_mesh_box.m_min = Engine::Vec3(FLT_MAX, FLT_MAX, FLT_MAX);

    for(auto& bundle: m_affected_bundles)
    {
        Engine::OBB bundle_box = bundle.first->bbox();
        m_pick_mesh_box.add(bundle_box);
    }

    for(auto& nurbs_change_info: m_affected_nurbs_points)
    {
        NurbsDoc* nurbs_doc = nurbs_change_info.first;
        Engine::Mat43 mat = nurbs_doc->world_matrix();
        for(auto& ctp_ptr: nurbs_change_info.second)
        {
            const nous::pos_t ori_pos = ctp_ptr.first->get_position();
            Engine::Vec3 cp = mat * NousConvert::convert(ori_pos);
            m_pick_mesh_box.add(cp);
        }
        nurbs_doc->dirty();
    }

    for(auto& mesh_positon: m_affected_mesh_local_position)
    {
        Engine::Mat43 mat = mesh_positon.first->world_matrix();
        for(auto& pos: mesh_positon.second)
        {
            Engine::Vec3 cp = mat * NousConvert::convert(pos.second);
            m_pick_mesh_box.add(cp);
        }
    }
}

void Controller_TRS::_gen_meshes(View* view)
{
    for(auto mesh_polygon: m_affected_mesh_polygons)
    {
        auto pick_geo = mesh_polygon.first->geodata();
        for(auto polygon: mesh_polygon.second)
        {
            pick_geo->delete_polygon(polygon);
        }
    }
    _update_new_meshes();
    // recreate lines & recreate no branch line groups relation
    for(auto mesh_line: m_affected_mesh_local_line)
    {
        auto geo = mesh_line.first->geodata();
        std::unordered_map<ElementGroupPtr, std::vector<std::pair<nous::vec3, nous::vec3>>> new_lines;
        for(auto& line: mesh_line.second)
        {
            nous::vec3 line_pos_0 = geo->get_point_position(line->p0);
            nous::vec3 line_pos_1 = geo->get_point_position(line->p1);
            ElementGroupPtr line_group = line->group();
            new_lines[line_group].emplace_back(std::move(line_pos_0), std::move(line_pos_1));
        }
        geo->delete_lines(mesh_line.second, false, false);
        for(auto& line_pair: new_lines)
        {
            ElementGroupPtr lines_group = line_pair.first;
            if(lines_group)
            {
                if(lines_group->type() == EElementType::NoBranchLineGroup)
                {
                    GeoDataTopoOperations::get().add_lines_to_no_branch_line_group(geo.get(), line_pair.second, lines_group);
                }
                else if(lines_group->type() == EElementType::SmoothGroup)
                {
                    GeoDataTopoOperations::get().add_lines_to_smooth_group(geo.get(), line_pair.second, lines_group);
                }
            }
            else
            {
                for(auto& line: line_pair.second)
                {
                    geo->add_line(line.first, line.second, false);
                }
            }
        }
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(geo.get());
        // GeoDataTopoOperations::get().check_and_recreate_smooth_group(geo.get());
        mesh_line.first->dirty();
    }
    // recreate polygon
    for(auto& mesh_new_polygon: m_mesh_new_polygons)
    {
        auto geo = mesh_new_polygon.first->geodata();
        for(auto& polygons_map: mesh_new_polygon.second)
        {
            for(auto& polygon: polygons_map.second)
            {
                geo->add_polygon(polygon);
            }
        }
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(geo.get());
        GeoDataTopoOperations::get().check_and_recreate_smooth_group(geo.get());
        mesh_new_polygon.first->dirty();
    }

    // merge
    std::unordered_set<MeshDoc*> change_meshes;
    for(auto& change_mesh_info: m_affected_mesh_local_position)
    {
        change_meshes.insert(change_mesh_info.first);
    }
    auto ignore_meshes = std::unordered_set<MeshDoc*>{};
    _check_intersection_and_merge_mesh(change_meshes, ignore_meshes, view->doc()->scene());
}

void Controller_TRS::_finish_TRS_operation()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_handle_gizmo && m_handle_gizmo->get_transform_matrix() == nous::mat4::Identity())
    {
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
    }
    else
    {
        RvtBlock blk(view->doc()->scene()->rvtId(), true);
        _gen_meshes(view);
    }
    _deactive_handle();
    _clear_affected_meshes();

    m_handle_gizmo = nullptr;
}

bool Controller_TRS::_is_select_invalid_element(ElementSelector* selector)
{
    int valid_element_count = 0;
    auto& select_info = selector->get_select_info();
    for(auto& select: select_info)
    {
        if(select.first->type() == Define::OBJ_D5_GROUP)
        {
            valid_element_count++;
        }
        else
        {
            for(auto& element: select.second.elements)
            {
                valid_element_count++;
                EElementType type = element->type();
                // if(type == EElementType::Point)
                //{
                //     return true;
                // }
            }
        }
    }
    return valid_element_count == 0;
}
}// namespace sketch
