#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_Freedom.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/LayoutCell.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
#include <Engine/Math/GeoUtil.h>

namespace sketch
{
Controller_Extrude_Freedom::Controller_Extrude_Freedom(const std::wstring& name) : Controller_Main(name) {}

Controller_Extrude_Freedom::~Controller_Extrude_Freedom() {}

SketchParamMap Controller_Extrude_Freedom::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "angle");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Extrude_Freedom::active(const SketchParamMap& props)
{
    Controller_Main::active(props);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    set_cursor(Define::MouseCursor::CURSOR_CROSS);

    _prepare_fixing_extrude();
}

void Controller_Extrude_Freedom::deactive()
{
    Controller_Main::deactive();

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    if(m_extrude_step == ESketchExtrudeStep::FixingSurface)
    {
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
    }

    clear_all_cache();
    m_extrude_step = ESketchExtrudeStep::PickElement;
}

void Controller_Extrude_Freedom::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);
    if(m_extrude_step == ESketchExtrudeStep::FixingSurface)
    {
        _rollback_to_pick_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Extrude_Freedom::enter(const SketchParamMap& props) { Controller_Main::enter(props); }

void Controller_Extrude_Freedom::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_Freedom::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_Freedom::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_down(button, x, y, ctrl, alt, shift);

    if(button != 0)
    {
        return;
    }

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ElementSelector* selector = view->selector();
    selector->update(view, x, y);
    selector->select_filter_view_group_area(view);
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->update_inference(view, x, y);

    if(m_extrude_step == ESketchExtrudeStep::PickElement)
    {
        _prepare_fixing_extrude();
    }
    else
    {
        if(m_new_polygon_doc)
        {
            _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ m_new_polygon_doc }, std::unordered_set<MeshDoc*>{},
                                               view->doc()->scene());
        }
        else
        {
            // 将 nurbsdoc 中的临时非 complex surface 的对象升级为 complex surface
            for(auto& select_info: m_selected_curves_info)
            {
                for(auto& curve_info: select_info.second)
                {
                    if(curve_info->type() == EElementType::Curve)
                    {
                        select_info.first->nurbsdata()->remove_item(curve_info->id());
                    }
                }
            }

            for(auto& tmp_items_info: m_cached_tmp_nurbs_items)
            {
                for(auto& item_id: tmp_items_info.second)
                {
                    auto& tmp_nurbs_item = tmp_items_info.first->nurbsdata()->get_item(item_id);
                    if(tmp_nurbs_item->type() == EElementType::AlgebraSurface || tmp_nurbs_item->type() == EElementType::ClosedSurface)
                    {
                        tmp_items_info.first->nurbsdata()->remove_item(item_id);
                        NurbsTrimmedSurfacePtr complex_surface = std::make_shared<NurbsTrimmedSurface>();
                        complex_surface->set_exterior_ring(tmp_nurbs_item->to_nurbs_surface_base());
                        tmp_items_info.first->nurbsdata()->add_item(complex_surface);
                    }
                }
            }
        }
        infer_helper->clear_ignore();
        m_extrude_step = ESketchExtrudeStep::PickElement;
        clear_all_cache();
    }
}

void Controller_Extrude_Freedom::_process_select_info(std::unordered_map<RenderObject*, SelectInfo>& select_info)
{
    clear_all_cache();

    for(auto& obj_info: select_info)
    {
        auto doc_type = obj_info.first->doc()->type();
        nous::mat4 trans_mat = NousConvert::convert(obj_info.first->doc()->world_matrix());
        if(doc_type == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* selected_nurbs_doc = static_cast<NurbsDoc*>(obj_info.first->doc());
            std::vector<SketchElementPtr> m_inside_data_vec;
            for(auto ele: obj_info.second.elements)
            {
                NurbsDataPtr nurbs_data = selected_nurbs_doc->nurbsdata();
                if(nurbs_data->item_exist(ele->id()))
                {
                    // 是实际的Nurbs实体
                    m_inside_data_vec.emplace_back(ele);
                }
                else
                {
                    // 是Nurbs实体的边界Curve
                    if(ele->type() == EElementType::AlgebraSurfaceEdge || ele->type() == EElementType::ClosedSurfaceEdge)
                    {
                        m_inside_data_vec.emplace_back(ele);
                    }
                }
            }
            if(!m_inside_data_vec.empty())
            {
                m_selected_curves_info.insert({ selected_nurbs_doc, m_inside_data_vec });
            }
        }

        if(doc_type == Define::ObjectType::OBJ_D5_MESH)
        {
            std::unordered_set<SketchLinePtr> polygon_edges;
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(obj_info.first->doc());
            std::shared_ptr<GeoData> geodata = mesh_doc->geodata();
            for(auto& ele: obj_info.second.elements)
            {
                if(ele->type() == EElementType::Polygon)
                {
                    SketchPolygonPtr polygon = ele->to_polygon();
                    auto& polygon_pos_arr = m_selected_polygons.emplace_back();
                    const auto&& exring_point_arr = geodata->get_element_points(polygon->ring);
                    auto& exring_pos_arr = polygon_pos_arr.emplace_back();
                    for(int i = 0; i < exring_point_arr.size(); ++i)
                    {
                        SketchPointPtr prev_pt = exring_point_arr[i];
                        SketchPointPtr next_pt = exring_point_arr[(i + 1) % exring_point_arr.size()];
                        nous::pos_t prev_pt_pos = geodata->get_point_position(prev_pt);
                        nous::pos_t next_pt_pos = geodata->get_point_position(next_pt);
                        nous::vec4 prev_pt_pos4(prev_pt_pos.x(), prev_pt_pos.y(), prev_pt_pos.z(), 1.0);
                        nous::vec4 trans_prev_pt_pos4 = trans_mat * prev_pt_pos4;
                        nous::vec4 next_pt_pos4(next_pt_pos.x(), next_pt_pos.y(), next_pt_pos.z(), 1.0);
                        nous::vec4 trans_next_pt_pos4 = trans_mat * next_pt_pos4;
                        nous::pos_t trans_prev_pt_pos = trans_prev_pt_pos4.head<3>() / trans_prev_pt_pos4.w();
                        nous::pos_t trans_next_pt_pos = trans_next_pt_pos4.head<3>() / trans_next_pt_pos4.w();
                        SketchLinePtr line = geodata->get_line(GeoData::line_key(prev_pt, next_pt));
                        polygon_edges.insert(line);
                        exring_pos_arr.emplace_back(trans_prev_pt_pos);
                        m_selected_or_transfered_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                        if(line->group() && line->group()->type() == EElementType::NoBranchLineGroup)
                        {
                            // Todo record some information.
                            if(m_weld_obj_rel_info.find(line->group()->id()) == m_weld_obj_rel_info.end())
                            {
                                m_weld_obj_rel_info.insert({ line->group()->id(), std::unordered_set<int>() });
                            }
                            m_weld_obj_rel_info[line->group()->id()].insert(m_selected_or_transfered_lines.size() - 1);
                        }
                        else
                        {
                            m_unweld_line_idx.insert(m_selected_or_transfered_lines.size() - 1);
                        }
                    }
                    for(auto inner_ring: polygon->inner_rings)
                    {
                        const auto&& inring_point_arr = geodata->get_element_points(inner_ring);
                        auto& inring_pos_arr = polygon_pos_arr.emplace_back();
                        for(int i = 0; i < inring_point_arr.size(); ++i)
                        {
                            SketchPointPtr prev_pt = inring_point_arr[i];
                            SketchPointPtr next_pt = inring_point_arr[(i + 1) % inring_point_arr.size()];
                            nous::pos_t prev_pt_pos = geodata->get_point_position(prev_pt);
                            nous::pos_t next_pt_pos = geodata->get_point_position(next_pt);
                            nous::vec4 prev_pt_pos4(prev_pt_pos.x(), prev_pt_pos.y(), prev_pt_pos.z(), 1.0);
                            nous::vec4 trans_prev_pt_pos4 = trans_mat * prev_pt_pos4;
                            nous::vec4 next_pt_pos4(next_pt_pos.x(), next_pt_pos.y(), next_pt_pos.z(), 1.0);
                            nous::vec4 trans_next_pt_pos4 = trans_mat * next_pt_pos4;
                            nous::pos_t trans_prev_pt_pos = trans_prev_pt_pos4.head<3>() / trans_prev_pt_pos4.w();
                            nous::pos_t trans_next_pt_pos = trans_next_pt_pos4.head<3>() / trans_next_pt_pos4.w();
                            SketchLinePtr line = geodata->get_line(GeoData::line_key(prev_pt, next_pt));
                            polygon_edges.insert(line);
                            inring_pos_arr.emplace_back(trans_prev_pt_pos);
                            m_selected_or_transfered_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                            if(line->group() && line->group()->type() == EElementType::NoBranchLineGroup)
                            {
                                // Todo record some information.
                                if(m_weld_obj_rel_info.find(line->group()->id()) == m_weld_obj_rel_info.end())
                                {
                                    m_weld_obj_rel_info.insert({ line->group()->id(), std::unordered_set<int>() });
                                }
                                m_weld_obj_rel_info[line->group()->id()].insert(m_selected_or_transfered_lines.size() - 1);
                            }
                            else
                            {
                                m_unweld_line_idx.insert(m_selected_or_transfered_lines.size() - 1);
                            }
                        }
                    }
                }
                else if(ele->type() == EElementType::Line)
                {
                    SketchLinePtr line = ele->to_line();
                    if(polygon_edges.find(line) == polygon_edges.end())
                    {
                        nous::pos_t prev_pt_pos = geodata->get_point_position(line->p0);
                        nous::pos_t next_pt_pos = geodata->get_point_position(line->p1);
                        nous::vec4 prev_pt_pos4(prev_pt_pos.x(), prev_pt_pos.y(), prev_pt_pos.z(), 1.0);
                        nous::vec4 trans_prev_pt_pos4 = trans_mat * prev_pt_pos4;
                        nous::vec4 next_pt_pos4(next_pt_pos.x(), next_pt_pos.y(), next_pt_pos.z(), 1.0);
                        nous::vec4 trans_next_pt_pos4 = trans_mat * next_pt_pos4;
                        nous::pos_t trans_prev_pt_pos = trans_prev_pt_pos4.head<3>() / trans_prev_pt_pos4.w();
                        nous::pos_t trans_next_pt_pos = trans_next_pt_pos4.head<3>() / trans_next_pt_pos4.w();
                        m_selected_or_transfered_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                        m_unweld_line_idx.insert(m_selected_or_transfered_lines.size() - 1);
                    }
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    NoBranchLineGroupPtr weld_line = ele->to_no_branch_line_group();
                    std::unordered_set<SketchElementPtr> weld_line_sub_elements = weld_line->elements();
                    auto&& line_positions = GeoDataTopoOperations::get().try_get_weld_line_position_array(geodata, weld_line_sub_elements);
                    for(int i = 0; i < line_positions.size() - 1; ++i)
                    {
                        nous::pos_t prev_pt_pos = line_positions[i];
                        nous::pos_t next_pt_pos = line_positions[(i + 1) % line_positions.size()];
                        nous::vec4 prev_pt_pos4(prev_pt_pos.x(), prev_pt_pos.y(), prev_pt_pos.z(), 1.0);
                        nous::vec4 trans_prev_pt_pos4 = trans_mat * prev_pt_pos4;
                        nous::vec4 next_pt_pos4(next_pt_pos.x(), next_pt_pos.y(), next_pt_pos.z(), 1.0);
                        nous::vec4 trans_next_pt_pos4 = trans_mat * next_pt_pos4;
                        nous::pos_t trans_prev_pt_pos = trans_prev_pt_pos4.head<3>() / trans_prev_pt_pos4.w();
                        nous::pos_t trans_next_pt_pos = trans_next_pt_pos4.head<3>() / trans_next_pt_pos4.w();
                        m_selected_or_transfered_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                        m_weld_obj_rel_info[weld_line->id()].insert(m_selected_or_transfered_lines.size() - 1);
                    }
                }
            }
        }
    }
}

void Controller_Extrude_Freedom::_prepare_fixing_extrude()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    InferResult infer_result = infer_helper->get_infer_result();
    ElementSelector* selector = view->selector();

    auto select_info = selector->get_select_info();
    if(select_info.size())
    {
        _process_select_info(select_info);
        if(m_selected_curves_info.size() + m_selected_or_transfered_lines.size())
        {
            m_direction_origin = infer_result.position;
            RvtBlock blk(view->doc()->scene()->rvtId());
            if(!m_selected_curves_info.empty())
            {
                for(auto& info_pair: m_selected_curves_info)
                {
                    info_pair.first->record();
                }
            }

            if(m_new_polygon_doc)
            {
                m_new_polygon_data = nullptr;
                view->delete_obj_and_try_delete_relation(m_new_polygon_doc);
                m_new_polygon_doc = nullptr;
            }
            if(!m_selected_or_transfered_lines.empty())
            {
                m_new_polygon_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                m_new_polygon_data = m_new_polygon_doc->geodata(true);
                infer_helper->add_ignore(m_new_polygon_doc->renderObject());
            }
            m_extrude_step = ESketchExtrudeStep::FixingSurface;
        }
    }
}

void Controller_Extrude_Freedom::_rollback_to_pick_element()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
    clear_all_cache();
    m_extrude_step = ESketchExtrudeStep::PickElement;
}

void Controller_Extrude_Freedom::clear_all_cache()
{
    m_cached_tmp_nurbs_items.clear();
    m_selected_curves_info.clear();
    m_selected_polygons.clear();
    m_selected_or_transfered_lines.clear();
    m_weld_obj_rel_info.clear();
    m_unweld_line_idx.clear();
    m_new_polygon_data = nullptr;
    m_new_polygon_doc = nullptr;
}

void Controller_Extrude_Freedom::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->update_inference(view, x, y);
    InferResult infer_result = infer_helper->get_infer_result();
    const nous::pos_t& infer_pos = infer_result.position;

    if(m_extrude_step == ESketchExtrudeStep::FixingSurface)
    {
        m_direction = (infer_pos - m_direction_origin).normalized();
        const nous::vec3 offset_vector = infer_pos - m_direction_origin;

        for(auto& cache_info_pair: m_cached_tmp_nurbs_items)
        {
            for(auto& item_guid: cache_info_pair.second)
            {
                cache_info_pair.first->nurbsdata()->remove_item(item_guid);
            }
        }
        m_cached_tmp_nurbs_items.clear();

        for(auto& nurbs_info: m_selected_curves_info)
        {
            NurbsDoc* nurbs_doc = nurbs_info.first;
            NurbsDataPtr nurbs_data = nurbs_doc->nurbsdata();
            std::unordered_set<uint32_t> tmp_nurbs_item_guid_arr;

            nous::mat4 inv_mat = NousConvert::convert(nurbs_doc->world_matrix().inverse());
            nous::vec4 offset_v4(offset_vector.x(), offset_vector.y(), offset_vector.z(), 1.0);
            nous::vec4 inv_offset_v4 = inv_mat * offset_v4;
            nous::vec3 inv_offset = inv_offset_v4.head<3>() / inv_offset_v4.w();
            nous::transform3 offset_mat = nous::transform3::Identity();
            offset_mat.translate(inv_offset);

            for(int i = 0; i < nurbs_info.second.size(); ++i)
            {
                auto& nurbs_item = nurbs_info.second[i];
                if(nurbs_item->type() == EElementType::Curve)
                {
                    auto& new_surface = nurbs::extrude_curve_to_surface(nurbs_item->to_nurbs_curve(), inv_offset);
                    tmp_nurbs_item_guid_arr.insert(new_surface->id());
                    nurbs_data->add_item(new_surface);
                }
                else if(nurbs_item->type() == EElementType::AlgebraSurfaceEdge)
                {
                    auto& new_surface = nurbs::extrude_curve_to_surface(nurbs_item->to_nurbs_algebra_surface_element()->m_edge_curve, inv_offset);
                    tmp_nurbs_item_guid_arr.insert(new_surface->id());
                    nurbs_data->add_item(new_surface);
                }
                else if(nurbs_item->type() == EElementType::ClosedSurfaceEdge)
                {
                    auto& new_surface =
                        nurbs::extrude_curve_to_surface(nurbs_item->to_nurbs_closed_surface_element()->m_edge_curve.lock(), inv_offset);
                    tmp_nurbs_item_guid_arr.insert(new_surface->id());
                    nurbs_data->add_item(new_surface);
                }
                else if(nurbs_item->type() == EElementType::AlgebraSurface)
                {
                    auto&& new_surfaces = nurbs::offset_surface_to_solid_with_transform(nurbs_item->to_nurbs_algebra_surface(), offset_mat);
                    for(auto& new_surface: new_surfaces)
                    {
                        tmp_nurbs_item_guid_arr.insert(new_surface->id());
                        nurbs_data->add_item(new_surface);
                    }
                }
                else if(nurbs_item->type() == EElementType::ClosedSurface)
                {
                    auto&& new_surfaces = nurbs::offset_surface_to_solid_with_transform(nurbs_item->to_nurbs_closed_surface(), offset_mat);
                    for(auto& new_surface: new_surfaces)
                    {
                        tmp_nurbs_item_guid_arr.insert(new_surface->id());
                        nurbs_data->add_item(new_surface);
                    }
                }
                else if(nurbs_item->type() == EElementType::TrimmedSurface)
                {
                    auto&& new_surfaces = nurbs::offset_surface_to_solid_with_transform(nurbs_item->to_nurbs_trimmed_surface(), offset_mat);
                    for(auto& new_surface: new_surfaces)
                    {
                        tmp_nurbs_item_guid_arr.insert(new_surface->id());
                        nurbs_data->add_item(new_surface);
                    }
                }
            }
            m_cached_tmp_nurbs_items.insert({ nurbs_doc, tmp_nurbs_item_guid_arr });
            nurbs_doc->dirty();
        }

        if(m_new_polygon_data)
        {
            m_new_polygon_data->clear();

            for(auto& line_idx: m_unweld_line_idx)
            {
                const nous::pos_t p00 = std::get<0>(m_selected_or_transfered_lines[line_idx]);
                const nous::pos_t p01 = std::get<1>(m_selected_or_transfered_lines[line_idx]);
                const nous::pos_t p10 = std::get<1>(m_selected_or_transfered_lines[line_idx]) + offset_vector;
                const nous::pos_t p11 = std::get<0>(m_selected_or_transfered_lines[line_idx]) + offset_vector;
                m_new_polygon_data->add_polygon({ { p00, p01, p10, p11 } });
                m_new_polygon_data->dirty();
                m_new_polygon_doc->dirty();
            }

            for(auto& weld_lines_info: m_weld_obj_rel_info)
            {
                std::unordered_set<SketchElementPtr> boundary_line_set;

                MeshDoc* tmp_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH, nullptr);
                GeoData* tmp_geodata = tmp_mesh_doc->geodata().get();
                for(auto& line_idx: weld_lines_info.second)
                {
                    const nous::pos_t p00 = std::get<0>(m_selected_or_transfered_lines[line_idx]);
                    const nous::pos_t p01 = std::get<1>(m_selected_or_transfered_lines[line_idx]);
                    const nous::pos_t p10 = std::get<1>(m_selected_or_transfered_lines[line_idx]) + offset_vector;
                    const nous::pos_t p11 = std::get<0>(m_selected_or_transfered_lines[line_idx]) + offset_vector;
                    tmp_geodata->add_polygon({ { p00, p01, p10, p11 } });
                    auto&& top_lines = tmp_geodata->add_line(p10, p11, false);
                    boundary_line_set.insert(top_lines.begin(), top_lines.end());
                    auto&& bottom_lines = tmp_geodata->add_line(p00, p01, false);
                    boundary_line_set.insert(bottom_lines.begin(), bottom_lines.end());
                }

                // 此时内部所有的面需要做成柔化组
                std::unordered_set<SketchElementPtr> not_boundary_line_set;

                for(auto& line_pair: tmp_geodata->get_all_lines())
                {
                    SketchLinePtr line = line_pair.second;
                    if(tmp_geodata->get_line_adj_rings(line).size() == 2)
                    {
                        not_boundary_line_set.insert(line);
                    }
                }

                GeoDataTopoOperations::get().try_merge_smooth_group(tmp_geodata, not_boundary_line_set, true);
                for(auto& not_boundary_line: not_boundary_line_set)
                {
                    if(boundary_line_set.find(not_boundary_line) != boundary_line_set.end())
                    {
                        boundary_line_set.erase(not_boundary_line);
                    }
                }
                GeoDataTopoOperations::get().try_merge_no_branch_line_group(tmp_geodata, boundary_line_set, {});

                m_new_polygon_doc->merge(tmp_mesh_doc);
                view->delete_obj_and_try_delete_relation(tmp_mesh_doc);
            }

            for(int i = 0; i < m_selected_polygons.size(); ++i)
            {
                std::vector<std::vector<nous::pos_t>> offseted_polygon;
                for(int m = 0; m < m_selected_polygons[i].size(); ++m)
                {
                    std::vector<nous::pos_t>& ring_pos = offseted_polygon.emplace_back();
                    for(int n = 0; n < m_selected_polygons[i][m].size(); ++n)
                    {
                        ring_pos.emplace_back(m_selected_polygons[i][m][n] + offset_vector);
                    }
                }
                m_new_polygon_data->add_polygon(offseted_polygon);
                m_new_polygon_data->dirty();
                m_new_polygon_doc->dirty();
            }
        }
    }

    m_hover_pos = infer_pos;
    view->dirtyRT();
}

void Controller_Extrude_Freedom::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_Freedom::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_extrude_step == ESketchExtrudeStep::FixingSurface)
    {
        view->batchRenderLine(1, 1.0, Engine::Vec3f(1.0, 0.0, 0.0), NousConvert::convert(m_hover_pos), NousConvert::convert(m_direction_origin),
                              0x00ff, 1, true, false);
        view->batchRenderCtrlPoints(2, false, false, NousConvert::convert(m_direction_origin));
    }

    // for(auto& vis_obj: view->visObjects())
    //{
    //     if(vis_obj->doc()->type() == Define::OBJ_D5_MESH)
    //     {
    //         MeshDoc* mesh_doc = static_cast<MeshDoc*>(vis_obj->doc());
    //         for(auto& line: mesh_doc->geodata()->get_all_lines())
    //         {
    //             if(mesh_doc->geodata()->get_line_adj_rings(line.second).size() < 2)
    //             {
    //                 const nous::pos_t p0_pos = mesh_doc->geodata()->get_point_position(line.second->p0);
    //                 const nous::pos_t p1_pos = mesh_doc->geodata()->get_point_position(line.second->p1);
    //                 view->batchRenderLine(2, 1.0f, NousConvert::convert(nous::vec3f(1.0, 0.0, 0.0)), NousConvert::convert(p0_pos),
    //                                       NousConvert::convert(p1_pos), 0xffffu, 1, false, false);
    //             }

    //            if(mesh_doc->geodata()->get_line_adj_rings(line.second).size() > 2)
    //            {
    //                const nous::pos_t p0_pos = mesh_doc->geodata()->get_point_position(line.second->p0);
    //                const nous::pos_t p1_pos = mesh_doc->geodata()->get_point_position(line.second->p1);
    //                view->batchRenderLine(2, 1.0f, NousConvert::convert(nous::vec3f(0.0, 1.0, 0.0)), NousConvert::convert(p0_pos),
    //                                      NousConvert::convert(p1_pos), 0xffffu, 1, false, false);
    //            }
    //        }
    //    }
    //}
}

};// namespace sketch