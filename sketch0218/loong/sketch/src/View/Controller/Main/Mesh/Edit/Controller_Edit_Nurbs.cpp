#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Edit_Nurbs.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/Controller/Handle/Handle.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

namespace sketch
{
Controller_Edit_Nurbs::Controller_Edit_Nurbs(const std::wstring& name) : Controller_Edit(name) {}

Controller_Edit_Nurbs::~Controller_Edit_Nurbs() {}

SketchParamMap Controller_Edit_Nurbs::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "distance");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Edit_Nurbs::active(const SketchParamMap& pProps)
{
    Controller_Edit::active(pProps);

    this->set_tip(L"select one nurbs to edit.");
    this->set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* inferHelper = view->inferHelper();
    inferHelper->set_infer_filter_type(InferType::ALL_TYPE);

    m_edit_step = EEditNurbStep::PickElement;
    view->selector()->clear();
}

void Controller_Edit_Nurbs::deactive()
{
    Controller_Edit::deactive();
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* inferHelper = view->inferHelper();
    inferHelper->set_infer_filter_type(InferType::ALL_TYPE);

    _step_back_to_pick_control_point();

    _step_back_to_pick_element();
}

void Controller_Edit_Nurbs::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);
    this->deactive();
}

void Controller_Edit_Nurbs::enter(const SketchParamMap& props) { Controller_Main::enter(props); }

void Controller_Edit_Nurbs::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Edit_Nurbs::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button == 0)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        view->inferHelper()->update_inference(view, x, y);
        view->selector()->update(view, x, y);
        view->selector()->select_filter_view_group_area(view);
        if(m_edit_step == EEditNurbStep::PickElement)
        {
            std::unordered_map<RenderObject*, SelectInfo> select_info = view->selector()->get_select_info();
            m_picked_doc = nullptr;

            for(auto& select_data_pair: select_info)
            {
                if(select_data_pair.first->doc()->type() == Define::ObjectType::OBJ_D5_NURBS)
                {
                    auto tmp_doc = dynamic_cast<NurbsDoc*>(select_data_pair.first->doc());
                    if(select_data_pair.second.elements.size() == 1)
                    {
                        m_picked_doc = tmp_doc;
                        m_picked_nurbs = m_picked_doc->nurbsdata(true);
                        m_picked_element = *(select_data_pair.second.elements.begin());
                        break;
                    }
                }
            }

            if(!m_picked_doc)
            {
                return;
            }

            if(_preprocess_nurbs_control_points())
            {
                m_edit_step = EEditNurbStep::PickControlPoint;
            }
        }
        else if(m_edit_step == EEditNurbStep::PickControlPoint)
        {
            if(m_hovered_control_point_idx != -1)
            {
                m_picked_control_point_idx = m_hovered_control_point_idx;
                m_edit_step = EEditNurbStep::FixingControlPoint;

                m_handle_gizmo = Handle::create("gizmo", view->camera(), Handle::GizmoMode::Translate);
                m_handle_gizmo->set_enabled(true);
                m_handle_gizmo->set_gizmo_position(m_prepared_select_points[m_picked_control_point_idx]);
            }
            else
            {
                _try_change_pick_nurbs_item(view);
            }
        }
        else if(m_edit_step == EEditNurbStep::FixingControlPoint)
        {
            m_handle_gizmo->onMouseDown(nous::vec2(x, y));
            if(m_handle_gizmo->is_hovered())
            {
                m_begin_drag_gizmo = true;
                SceneDoc* scene = STCore::inst->viewMgr()->layout()->focusedView()->doc()->scene();
                RvtBlock blk(scene->rvtId());
                m_picked_doc->record();
            }
            else if(m_hovered_control_point_idx != m_picked_control_point_idx && m_hovered_control_point_idx != -1)
            {
                _try_change_pick_control_point();
            }
            else
            {
                _try_change_pick_nurbs_item(view);
            }
        }
    }
    else if(button == 2)
    {
        // 暂时使用右键退出
        if(m_edit_step == EEditNurbStep::PickControlPoint)
        {
            // PickControlPoint 阶段退回 PickElement 阶段
            _step_back_to_pick_element();
        }
        else if(m_edit_step == EEditNurbStep::FixingControlPoint)
        {
            // FixingControlPoint 阶段退回 PickControlPoint 阶段
            _step_back_to_pick_control_point();
        }
    }
}

void Controller_Edit_Nurbs::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(m_begin_drag_gizmo)
    {
        m_handle_gizmo->onMouseUp(nous::vec2(x, y));
        m_handle_gizmo->reset(true, true, true);
        m_begin_drag_gizmo = false;
        _refresh_control_points();
    }
}

void Controller_Edit_Nurbs::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->inferHelper()->update_inference(view, x, y);
    nous::vec3 new_pos = view->inferHelper()->get_infer_result().position;

    if(m_edit_step >= EEditNurbStep::PickControlPoint)
    {
        ViewCamera* curr_camera = view->camera();
        Document* global_doc = STCore::inst->docMgr()->document();
        auto& view_port = curr_camera->viewport();

        const nous::mat4 matVP = NousConvert::convert(curr_camera->dvpMatrix());

        m_hovered_control_point_idx = -1;
        double min_dist = 10.0;
        for(int i = 0; i < m_prepared_select_points.size(); ++i)
        {
            const nous::pos_t control_pos = m_prepared_select_points[i];
            nous::vec4 vpos = NousConvert::product_v4(matVP, control_pos);
            if(vpos.w() <= 0.0f)// invisible
                continue;
            vpos.head<3>() /= vpos.w();
            nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
            npos.x() *= view_port.z;
            npos.y() *= view_port.w;
            float distance2 = (npos - nous::vec2(x, y)).squaredNorm();
            if(distance2 < min_dist)
            {
                min_dist = distance2;
                m_hovered_control_point_idx = i;
            }
        }
    }

    if(m_edit_step == EEditNurbStep::FixingControlPoint)
    {
        m_handle_gizmo->onMouseMove(nous::vec2((float)x, (float)y));
        if(m_begin_drag_gizmo)
        {
            auto trans = m_handle_gizmo->get_transform_matrix();
            const nous::pos_t position = m_prepared_select_points[m_picked_control_point_idx];
            new_pos = (trans * nous::vec4(position.x(), position.y(), position.z(), 1.0)).head<3>();

            Engine::Mat43 tmat_inv = m_picked_doc->world_matrix_inv();
            nous::mat4 trans_mat = NousConvert::convert(tmat_inv);

            nous::vec4 new_pos4(new_pos.x(), new_pos.y(), new_pos.z(), 1.0);
            nous::vec4 cm_pos = trans_mat * new_pos4;
            const nous::pos_t ci_pos = cm_pos.head<3>() / cm_pos.w();

            if(m_picked_curve)
            {
                if(m_edit_mode == EEditNurbMode::ControlPoint)
                {
                    m_prepared_select_ctp_points[m_picked_control_point_idx]->set_position(ci_pos);
                }
                else
                {
                    const bool is_closed_line = m_picked_curve->is_curve_closed();
                    std::vector<nous::pos_t> tmp_pos_arr;
                    for(int i = 0; i < m_prepared_select_points.size(); ++i)
                    {
                        nous::vec4 fitted_pos4(m_prepared_select_points[i].x(), m_prepared_select_points[i].y(), m_prepared_select_points[i].z(),
                                               1.0);
                        nous::vec4 tmp_pos4 = trans_mat * fitted_pos4;
                        const nous::pos_t tmp_pos = tmp_pos4.head<3>() / tmp_pos4.w();
                        tmp_pos_arr.emplace_back(tmp_pos);
                    }
                    if(is_closed_line && (m_picked_control_point_idx == 0 || m_picked_control_point_idx == m_prepared_select_points.size() - 1))
                    {
                        tmp_pos_arr[0] = ci_pos;
                        tmp_pos_arr[m_prepared_select_points.size() - 1] = ci_pos;
                    }
                    tmp_pos_arr[m_picked_control_point_idx] = ci_pos;
                    auto new_curve = nurbs::solve_tridiagonal(tmp_pos_arr, true, nous::vec3::Zero(), true, nous::vec3::Zero(),
                                                              is_closed_line ? ENurbsCloseType::SmoothClose : ENurbsCloseType::Open);
                    m_picked_curve->update_curve_data(new_curve);
                }
            }

            if(m_picked_algebra_surface || m_picked_closed_surface || m_picked_trimmed_surface)
            {
                m_prepared_select_ctp_points[m_picked_control_point_idx]->set_position(ci_pos);
            }

            m_picked_nurbs->dirty();
            m_picked_doc->dirty();
        }
    }

    m_hover_pos = new_pos;

    view->dirtyRT();
}

void Controller_Edit_Nurbs::_transfer_directly_to_gizmo()
{
    _refresh_control_points();
    if(!m_handle_gizmo)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        m_handle_gizmo = Handle::create("gizmo", view->camera(), Handle::GizmoMode::Translate);
    }
    m_handle_gizmo->set_enabled(true);
    m_handle_gizmo->set_gizmo_position(m_prepared_select_points[m_picked_control_point_idx]);
}

void Controller_Edit_Nurbs::_transfer_gizmo_to_directly() {}

void Controller_Edit_Nurbs::_refresh_control_points()
{
    m_prepared_select_ctp_points.clear();
    m_prepared_select_points.clear();

    std::unordered_set<SketchNurbsPointPtr> ctp_set;
    if(m_picked_curve)
    {
        if(m_picked_curve->get_curve_type() == ENurbsCurveType::Control)
        {
            const auto&& curve_ctp_arr = m_picked_curve->get_curve_control_points();
            ctp_set.insert(curve_ctp_arr.begin(), curve_ctp_arr.end());
            m_edit_mode = EEditNurbMode::ControlPoint;
        }
        else
        {
            m_prepared_select_points = m_picked_curve->get_fitting_points();
            m_edit_mode = EEditNurbMode::FitPoint;
        }
    }

    if(m_picked_algebra_surface)
    {
        auto& temp_control_points = m_picked_algebra_surface->get_surface_control_points();
        for(auto& row: temp_control_points)
        {
            ctp_set.insert(row.begin(), row.end());
        }
        m_edit_mode = EEditNurbMode::ControlPoint;
    }

    if(m_picked_closed_surface)
    {
        // 只能编辑控制点
        for(int i = 0; i < m_picked_closed_surface->get_curve_number(); ++i)
        {
            const auto& inside_curve = m_picked_closed_surface->get_brep_curve_range(i).m_inner_curve;
            const auto&& curve_ctp_arr = inside_curve->get_curve_control_points();
            ctp_set.insert(curve_ctp_arr.begin(), curve_ctp_arr.end());
        }
        m_edit_mode = EEditNurbMode::ControlPoint;
    }

    if(m_picked_trimmed_surface)
    {
        const auto&& complex_surface_ctp_arr = m_picked_trimmed_surface->get_surface_control_points_1D();
        ctp_set.insert(complex_surface_ctp_arr.begin(), complex_surface_ctp_arr.end());
        m_edit_mode = EEditNurbMode::ControlPoint;
    }

    Engine::Mat43 tmat = m_picked_doc->world_matrix();
    nous::mat4 trans_mat = NousConvert::convert(tmat);

    // fitpoint transform to world!
    if(m_edit_mode == EEditNurbMode::FitPoint)
    {
        for(int i = 0; i < m_prepared_select_points.size(); ++i)
        {
            nous::vec3 ori_pos = m_prepared_select_points[i];
            nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
            nous::vec4 cm_pos = trans_mat * ori_pos4;
            m_prepared_select_points[i] = cm_pos.head<3>() / cm_pos.w();
        }
    }
    else
    {
        m_prepared_select_ctp_points.insert(m_prepared_select_ctp_points.end(), ctp_set.begin(), ctp_set.end());
        m_prepared_select_points.clear();
        m_prepared_select_points.resize(m_prepared_select_ctp_points.size());
        for(int i = 0; i < m_prepared_select_ctp_points.size(); ++i)
        {
            nous::vec3 ori_pos = m_prepared_select_ctp_points[i]->m_point;
            nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
            nous::vec4 cm_pos = trans_mat * ori_pos4;
            m_prepared_select_points[i] = cm_pos.head<3>() / cm_pos.w();
        }
    }
}

void Controller_Edit_Nurbs::prepare_render(View* view)
{
    if(m_edit_step > EEditNurbStep::PickElement)
    {
        if(m_hovered_control_point_idx != -1)
        {
            view->batchRenderCtrlPoints(0, false, true, NousConvert::convert(m_prepared_select_points[m_hovered_control_point_idx]));
        }
    }

    if(m_edit_step == EEditNurbStep::PickControlPoint)
    {
        for(int i = 0; i < m_prepared_select_points.size(); ++i)
        {
            if(i == m_hovered_control_point_idx)
            {
                view->batchRenderCtrlPoints(0, false, true, NousConvert::convert(m_prepared_select_points[m_hovered_control_point_idx]));
            }
            else
            {
                view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(m_prepared_select_points[i]));
            }
        }
    }

    if(m_edit_step == EEditNurbStep::FixingControlPoint && m_picked_control_point_idx >= 0 &&
       m_picked_control_point_idx < m_prepared_select_points.size())
    {
        Engine::Mat43 tmat = m_picked_doc->world_matrix();
        nous::mat4 trans_mat = NousConvert::convert(tmat);
        std::unordered_set<unsigned int> act_selected_idx_set;
        if(m_picked_curve)
        {
            std::vector<nous::pos_t> modified_control_points;
            if(m_picked_curve->get_curve_type() == ENurbsCurveType::Control)
            {
                const auto& ctp_arr = m_picked_curve->get_curve_control_points();
                for(int i = 0; i < ctp_arr.size(); ++i)
                {
                    modified_control_points.emplace_back(ctp_arr[i]->m_point);
                    if(ctp_arr[i] == m_prepared_select_ctp_points[m_picked_control_point_idx])
                    {
                        act_selected_idx_set.insert(i);
                    }
                }
            }
            else
            {
                modified_control_points = m_picked_curve->get_fitting_points();
                act_selected_idx_set.insert(m_picked_control_point_idx);
            }

            for(int i = 0; i < modified_control_points.size(); ++i)
            {
                nous::vec3 ori_pos = modified_control_points[i];
                nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
                nous::vec4 cm_pos = trans_mat * ori_pos4;
                const nous::pos_t draw_pos = cm_pos.head<3>() / cm_pos.w();

                if(act_selected_idx_set.find(i) != act_selected_idx_set.end())
                {
                    view->batchRenderCtrlPoints(0, true, false, NousConvert::convert(draw_pos));
                }
                else
                {
                    view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(draw_pos));
                }
            }
        }
        if(m_picked_algebra_surface)
        {
            auto& modified_control_points = m_picked_algebra_surface->get_surface_control_points();
            for(int i = 0; i < modified_control_points.size(); ++i)
            {
                for(int j = 0; j < modified_control_points[i].size(); ++j)
                {
                    nous::vec3 ori_pos = modified_control_points[i][j]->m_point;
                    nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
                    nous::vec4 cm_pos = trans_mat * ori_pos4;
                    const nous::pos_t draw_pos = cm_pos.head<3>() / cm_pos.w();
                    if(modified_control_points[i][j] == m_prepared_select_ctp_points[m_picked_control_point_idx])
                    {
                        view->batchRenderCtrlPoints(0, true, false, NousConvert::convert(draw_pos));
                    }
                    else
                    {
                        view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(draw_pos));
                    }
                }
            }
        }

        if(m_picked_closed_surface)
        {
            std::vector<nous::pos_t> modified_control_points;
            for(int i = 0; i < m_picked_closed_surface->get_curve_number(); ++i)
            {
                const auto& inside_curve = m_picked_closed_surface->get_brep_curve_range(i).m_inner_curve;
                const auto&& curve_ctp_arr = inside_curve->get_curve_control_points();
                for(int i = 0; i < curve_ctp_arr.size(); ++i)
                {
                    modified_control_points.emplace_back(curve_ctp_arr[i]->m_point);
                    if(curve_ctp_arr[i] == m_prepared_select_ctp_points[m_picked_control_point_idx])
                    {
                        act_selected_idx_set.insert(i);
                    }
                }
            }

            for(int i = 0; i < modified_control_points.size(); ++i)
            {
                nous::vec3 ori_pos = modified_control_points[i];
                nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
                nous::vec4 cm_pos = trans_mat * ori_pos4;
                const nous::pos_t draw_pos = cm_pos.head<3>() / cm_pos.w();

                if(act_selected_idx_set.find(i) != act_selected_idx_set.end())
                {
                    view->batchRenderCtrlPoints(0, true, false, NousConvert::convert(draw_pos));
                }
                else
                {
                    view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(draw_pos));
                }
            }
        }

        if(m_picked_trimmed_surface)
        {
            auto& modified_control_points = m_picked_trimmed_surface->get_surface_control_points();
            for(int i = 0; i < modified_control_points.size(); ++i)
            {
                for(int j = 0; j < modified_control_points[i].size(); ++j)
                {
                    nous::vec3 ori_pos = modified_control_points[i][j]->m_point;
                    nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
                    nous::vec4 cm_pos = trans_mat * ori_pos4;
                    const nous::pos_t draw_pos = cm_pos.head<3>() / cm_pos.w();
                    if(modified_control_points[i][j] == m_prepared_select_ctp_points[m_picked_control_point_idx])
                    {
                        view->batchRenderCtrlPoints(0, true, false, NousConvert::convert(draw_pos));
                    }
                    else
                    {
                        view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(draw_pos));
                    }
                }
            }
        }

        if(m_handle_gizmo->is_visible())
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
}

bool Controller_Edit_Nurbs::_preprocess_nurbs_control_points()
{
    if(!m_picked_doc || !m_picked_nurbs || !m_picked_element)
    {
        return false;
    }

    auto&& process_as_curve = [&](SketchElementPtr nurbs_item) -> void {
        std::shared_ptr<NurbsCurve> nurbs_curve = std::static_pointer_cast<NurbsCurve>(nurbs_item);
        m_picked_curve = nurbs_curve;
        m_picked_algebra_surface = nullptr;
        m_picked_closed_surface = nullptr;
        m_picked_trimmed_surface = nullptr;
    };

    auto&& process_as_algebra_surface = [&](SketchElementPtr nurbs_item) -> void {
        std::shared_ptr<NurbsAlgebraSurface> nurbs_surface = std::static_pointer_cast<NurbsAlgebraSurface>(nurbs_item);
        m_picked_algebra_surface = nurbs_surface;
        m_picked_curve = nullptr;
        m_picked_closed_surface = nullptr;
        m_picked_trimmed_surface = nullptr;
    };

    auto&& process_as_closed_surface = [&](SketchElementPtr nurbs_item) -> void {
        std::shared_ptr<NurbsClosedSurface> nurbs_curve_brep = std::static_pointer_cast<NurbsClosedSurface>(nurbs_item);
        if(nurbs_curve_brep->get_curve_number() == 1)
        {
            process_as_curve(nurbs_curve_brep->get_brep_curve_range(0).m_inner_curve);
        }
        else
        {
            m_picked_curve = nullptr;
            m_picked_algebra_surface = nullptr;
            m_picked_closed_surface = nurbs_curve_brep;
            m_picked_trimmed_surface = nullptr;
        }
    };

    auto&& process_as_complex_surface = [&](SketchElementPtr nurbs_item) -> void {
        std::shared_ptr<NurbsTrimmedSurface> complex_surface = std::static_pointer_cast<NurbsTrimmedSurface>(nurbs_item);
        if(complex_surface->get_interior_rings().empty())
        {
            // 退化，没内环的话就等于上面两种面
            INurbsBasePtr exring_base = complex_surface->get_exterior_ring();
            if(exring_base->type() == EElementType::AlgebraSurface)
            {
                process_as_algebra_surface(exring_base);
            }
            if(exring_base->type() == EElementType::ClosedSurface)
            {
                process_as_closed_surface(exring_base);
            }
        }
        else
        {
            m_picked_curve = nullptr;
            m_picked_algebra_surface = nullptr;
            m_picked_closed_surface = nullptr;
            m_picked_trimmed_surface = complex_surface;
        }
    };

    if(m_picked_element->type() == EElementType::Curve)
    {
        process_as_curve(m_picked_element);
    }
    else if(m_picked_element->type() == EElementType::AlgebraSurfaceEdge)
    {
        std::shared_ptr<NurbsAlgebraSurfaceEdgeElement> algebra_surface_edge =
            std::static_pointer_cast<NurbsAlgebraSurfaceEdgeElement>(m_picked_element);
        process_as_curve(algebra_surface_edge->m_edge_curve);
    }
    else if(m_picked_element->type() == EElementType::ClosedSurfaceEdge)
    {
        std::shared_ptr<NurbsClosedSurfaceEdgeElement> closed_surface_edge =
            std::static_pointer_cast<NurbsClosedSurfaceEdgeElement>(m_picked_element);
        process_as_curve(closed_surface_edge->m_edge_curve.lock());
    }
    else if(m_picked_element->type() == EElementType::AlgebraSurface)
    {
        process_as_algebra_surface(m_picked_element);
    }
    else if(m_picked_element->type() == EElementType::ClosedSurface)
    {
        process_as_closed_surface(m_picked_element);
    }
    else if(m_picked_element->type() == EElementType::TrimmedSurface)
    {
        process_as_complex_surface(m_picked_element);
    }

    _refresh_control_points();

    return true;
}

void Controller_Edit_Nurbs::_step_back_to_pick_control_point()
{
    if(m_edit_step == EEditNurbStep::FixingControlPoint)
    {
        const nous::pos_t rollback_pos = m_prepared_select_points[m_picked_control_point_idx];
        Engine::Mat43 tmat_inv = m_picked_doc->world_matrix_inv();
        nous::mat4 trans_mat = NousConvert::convert(tmat_inv);

        nous::vec4 new_pos4(rollback_pos.x(), rollback_pos.y(), rollback_pos.z(), 1.0);
        nous::vec4 cm_pos = trans_mat * new_pos4;
        const nous::pos_t ci_pos = cm_pos.head<3>() / cm_pos.w();

        if(m_picked_curve)
        {
            if(m_edit_mode == EEditNurbMode::FitPoint)
            {
                const bool is_closed_line = m_picked_curve->is_curve_closed();
                std::vector<nous::pos_t> tmp_pos_arr;
                for(int i = 0; i < m_prepared_select_points.size(); ++i)
                {
                    nous::vec4 fitted_pos4(m_prepared_select_points[i].x(), m_prepared_select_points[i].y(), m_prepared_select_points[i].z(), 1.0);
                    nous::vec4 tmp_pos4 = trans_mat * fitted_pos4;
                    const nous::pos_t tmp_pos = tmp_pos4.head<3>() / tmp_pos4.w();
                    tmp_pos_arr.emplace_back(tmp_pos);
                }
                auto new_curve = nurbs::solve_tridiagonal(tmp_pos_arr, true, nous::vec3::Zero(), true, nous::vec3::Zero(),
                                                          is_closed_line ? ENurbsCloseType::SmoothClose : ENurbsCloseType::Open);
                m_picked_curve->update_curve_data(new_curve);
            }
            else
            {
                m_prepared_select_ctp_points[m_picked_control_point_idx]->set_position(ci_pos);
            }
        }

        if(m_picked_algebra_surface || m_picked_closed_surface || m_picked_trimmed_surface)
        {
            m_prepared_select_ctp_points[m_picked_control_point_idx]->set_position(ci_pos);
        }

        m_picked_nurbs->dirty();
        m_picked_doc->dirty();

        if(m_handle_gizmo)
        {
            m_handle_gizmo->set_enabled(false);
            m_handle_gizmo->reset(true, true, true);
            m_handle_gizmo = nullptr;
        }

        m_edit_step = EEditNurbStep::PickControlPoint;
    }
}

void Controller_Edit_Nurbs::_step_back_to_pick_element()
{
    if(m_edit_step == EEditNurbStep::PickControlPoint)
    {
        m_prepared_select_points.clear();
        m_picked_doc = nullptr;
        m_picked_nurbs = nullptr;
        m_picked_algebra_surface = nullptr;
        m_picked_control_point_idx = -1;
        m_hovered_control_point_idx = -1;
        m_edit_step = EEditNurbStep::PickElement;
    }
}

void Controller_Edit_Nurbs::_try_change_pick_control_point()
{
    m_picked_control_point_idx = m_hovered_control_point_idx;
    m_handle_gizmo->set_gizmo_position(m_prepared_select_points[m_picked_control_point_idx]);
}

void Controller_Edit_Nurbs::_try_change_pick_nurbs_item(View* view)
{
    std::unordered_map<RenderObject*, SelectInfo> select_info = view->selector()->get_select_info();

    for(auto& select_data_pair: select_info)
    {
        if(select_data_pair.first->doc()->type() == Define::ObjectType::OBJ_D5_NURBS)
        {
            auto tmp_doc = dynamic_cast<NurbsDoc*>(select_data_pair.first->doc());
            if(select_data_pair.second.elements.size() == 1)
            {
                this->_step_back_to_pick_control_point();
                this->_step_back_to_pick_element();
                m_picked_doc = tmp_doc;
                m_picked_nurbs = m_picked_doc->nurbsdata(true);
                m_picked_element = (*select_data_pair.second.elements.begin());
                break;
            }
        }
    }

    if(!m_picked_doc)
    {
        return;
    }

    _preprocess_nurbs_control_points();

    m_edit_step = EEditNurbStep::PickControlPoint;
}

};// namespace sketch