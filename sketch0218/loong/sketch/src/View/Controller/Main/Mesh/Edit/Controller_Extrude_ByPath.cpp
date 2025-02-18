#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_ByPath.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

namespace sketch
{
Controller_Extrude_ByPath::Controller_Extrude_ByPath(const std::wstring& name) : Controller_Main(name) {}

Controller_Extrude_ByPath::~Controller_Extrude_ByPath() {}

SketchParamMap Controller_Extrude_ByPath::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "distance");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Extrude_ByPath::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ElementSelector* selector = view->selector();
    m_extrude_path_step = EExtrudePathStep::PickRouteElement;

    auto select_infos = selector->get_select_info();
    if(select_infos.size())
    {
        if(process_path_element(select_infos))
        {
            m_extrude_path_step = EExtrudePathStep::PickShapeElement;
        }
    }

    set_cursor(Define::MouseCursor::CURSOR_CROSS);
}

void Controller_Extrude_ByPath::deactive()
{
    Controller_Main::deactive();
    rollback_to_pick_shape_element();
    rollback_to_pick_route_element();
}

void Controller_Extrude_ByPath::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);
    if(m_extrude_path_step == EExtrudePathStep::PickShapeElement)
    {
        rollback_to_pick_route_element();
    }
    else if(m_extrude_path_step == EExtrudePathStep::CreateSurface)
    {
        rollback_to_pick_shape_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Extrude_ByPath::enter(const SketchParamMap& props) { Controller_Main::enter(props); }

void Controller_Extrude_ByPath::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_ByPath::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_down(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ElementSelector* selector = view->selector();
    if(button == 0)
    {
        selector->update(view, x, y);
        selector->select_filter_view_group_area(view);
        auto select_infos = selector->get_select_info();
        if(m_extrude_path_step == EExtrudePathStep::PickRouteElement)
        {
            if(process_path_element(select_infos))
            {
                m_extrude_path_step = EExtrudePathStep::PickShapeElement;
            }
        }
        else if(m_extrude_path_step == EExtrudePathStep::PickShapeElement)
        {
            if(process_select_elements(select_infos))
            {
                View* view = STCore::inst->viewMgr()->layout()->focusedView();
                SceneDoc* scene = view->doc()->scene();
                RvtBlock rvt_block(scene->rvtId());
                generate_extrude_mesh_or_surface();
                m_extrude_path_step = EExtrudePathStep::CreateSurface;
            }
        }
        else
        {
            // 退化逻辑，在选中的基础面是 Line LineGroup Polygon 的情况下，且挤出路径是 Line LineGroup 的情况下，直接退化为 GeoData
            const nous::pos_t end_position = m_curve_path->get_position(m_last_u);
            const nous::pos_t start_position = m_curve_path->get_position(0.0);
            const nous::pos_t offset = end_position - start_position;

            MeshDoc* meshdoc = nullptr;

            if(m_extrude_path_type == EExtrudePathType::Curve)
            {
                std::vector<NurbsAlgebraSurfacePtr> all_sub_surfaces;
                if(m_extrude_path_shape_type == EExtrudePathShapeType::Polygon)
                {
                    NurbsDataPtr nurbs_data = m_picked_nurbs_doc->nurbsdata();
                    for(auto pair: m_polygon_boundary_side_surfaces)
                    {
                        std::vector<std::pair<size_t, size_t>>& weld_info = m_selected_polygon_boundary_weld_info[pair.first];
                        NurbsAlgebraSurfacePtr algebra_surface = nurbs_data->get_item(pair.second)->to_nurbs_algebra_surface();
                        auto&& sub_surfaces = nurbs::split_surface_on_u_with_weld_info(algebra_surface, weld_info);
                        all_sub_surfaces.insert(all_sub_surfaces.end(), sub_surfaces.begin(), sub_surfaces.end());
                    }
                    nurbs_data->clear();
                    for(auto& sub_surface: all_sub_surfaces)
                    {
                        // 升级为 complex surface
                        NurbsTrimmedSurfacePtr complex_surface = std::make_shared<NurbsTrimmedSurface>();
                        if(sub_surface->type() == EElementType::AlgebraSurface)
                        {
                            sub_surface->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::Solid);
                        }
                        complex_surface->set_exterior_ring(sub_surface);
                        nurbs_data->add_item(complex_surface);
                    }
                    nurbs_data->dirty();
                    m_picked_nurbs_doc->dirty();
                    m_cached_tmp_nurbs_items.clear();
                }
            }

            if(m_extrude_path_type == EExtrudePathType::Polyline)
            {
                if(m_extrude_path_shape_type == EExtrudePathShapeType::LineOrWeldLine || m_extrude_path_shape_type == EExtrudePathShapeType::Polygon)
                {
                    if(!meshdoc)
                    {
                        meshdoc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                    }
                    std::shared_ptr<GeoData> geodata = meshdoc->geodata();

                    // 退化成 MeshDoc 然后和其余的 MeshDoc merge 掉
                    NurbsDataPtr nurbs_data = m_picked_nurbs_doc->nurbsdata();
                    const nous::mat4& nurbs_trans = NousConvert::convert(m_picked_nurbs_doc->world_matrix());

                    if(m_extrude_path_shape_type == EExtrudePathShapeType::LineOrWeldLine)
                    {
                        for(auto data_pair: nurbs_data->get_all_nurbs())
                        {
                            if(m_cached_tmp_nurbs_items.find(data_pair.first) != m_cached_tmp_nurbs_items.end())
                            {
                                nurbs::transfer_1d_surface_into_geodata(meshdoc, data_pair.second->to_nurbs_algebra_surface(), nurbs_trans);
                                nurbs_data->remove_item(data_pair.first);
                            }
                        }
                    }
                    else
                    {
                        for(auto pair: m_polygon_boundary_side_surfaces)
                        {
                            std::vector<std::pair<size_t, size_t>>& weld_info = m_selected_polygon_boundary_weld_info[pair.first];
                            NurbsAlgebraSurfacePtr algebra_surface = nurbs_data->get_item(pair.second)->to_nurbs_algebra_surface();
                            if(m_cached_tmp_nurbs_items.find(algebra_surface->id()) != m_cached_tmp_nurbs_items.end())
                            {
                                nurbs::transfer_1d_surface_into_geodata_with_weld_info(meshdoc, algebra_surface, view, nurbs_trans, weld_info);
                                nurbs_data->remove_item(algebra_surface->id());
                            }
                        }
                    }

                    geodata->dirty();
                    meshdoc->dirty();
                    view->delete_obj_and_try_delete_relation(m_picked_nurbs_doc);
                    m_picked_nurbs_doc = nullptr;
                }
            }

            if(m_extrude_path_shape_type == EExtrudePathShapeType::Polygon)
            {
                // 补一个顶面 polygon 给 geodata
                meshdoc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                std::shared_ptr<GeoData> geodata = meshdoc->geodata();
                for(int i = 0; i < m_selected_polygon_positions.size(); ++i)
                {
                    for(int j = 0; j < m_selected_polygon_positions[i].size(); ++j)
                    {
                        m_selected_polygon_positions[i][j] += offset;
                    }
                }
                geodata->add_polygon(m_selected_polygon_positions);
                geodata->dirty();
                meshdoc->dirty();
            }

            // 将剩余的单曲面升级为 complex surface
            if(m_picked_nurbs_doc && m_cached_tmp_nurbs_items.size())
            {
                for(auto& item_id: m_cached_tmp_nurbs_items)
                {
                    auto& tmp_nurbs_item = m_picked_nurbs_doc->nurbsdata()->get_item(item_id);
                    if(tmp_nurbs_item->type() == EElementType::AlgebraSurface || tmp_nurbs_item->type() == EElementType::ClosedSurface)
                    {
                        if(tmp_nurbs_item->type() == EElementType::AlgebraSurface)
                        {
                            tmp_nurbs_item->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::Solid);
                        }
                        m_picked_nurbs_doc->nurbsdata()->remove_item(item_id);
                        NurbsTrimmedSurfacePtr complex_surface = std::make_shared<NurbsTrimmedSurface>();
                        complex_surface->set_exterior_ring(tmp_nurbs_item->to_nurbs_surface_base());
                        m_picked_nurbs_doc->nurbsdata()->add_item(complex_surface);
                    }
                }
            }

            if(meshdoc)
            {
                _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ meshdoc }, std::unordered_set<MeshDoc*>{}, view->doc()->scene());
            }

            if(m_picked_nurbs_doc)
            {
                for(auto& processed_curve: m_selected_curves)
                {
                    if(m_picked_nurbs_doc->nurbsdata()->item_exist(processed_curve->id()))
                    {
                        // 单曲线沿线推拉，删除该曲线，这个曲线已经成为了曲面的一个组件
                        // 相对而言，由于曲面边线被选中也可以参与运算，但是显而易见的是 id 查不到，所以刚好不会被移除
                        m_picked_nurbs_doc->nurbsdata()->remove_item(processed_curve->id());
                    }
                }

                for(auto& nurbs_item: m_picked_nurbs_doc->nurbsdata()->get_all_nurbs())
                {
                    auto surface_item = nurbs_item.second->to_nurbs_algebra_surface();
                    if(surface_item)
                    {
                        surface_item->set_render_level(ENurbsSurfaceRenderLevel::Solid);
                    }
                }
                m_picked_nurbs_doc->dirty();
            }

            m_curve_path = nullptr;
            m_prepared_extrude_curve.clear();
            m_path_inference_line.clear();
            m_selected_curves.clear();
            m_selected_polygon_positions.clear();
            m_selected_polygon_boundary_weld_info.clear();
            m_selected_lines.clear();
            m_extrude_path_step = EExtrudePathStep::PickRouteElement;
        }
    }
}

void Controller_Extrude_ByPath::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->update_inference(view, x, y);

    if(m_extrude_path_step == EExtrudePathStep::CreateSurface)
    {
        ViewCamera* curr_camera = view->camera();
        Document* global_doc = STCore::inst->docMgr()->document();
        auto& view_port = curr_camera->viewport();
        Engine::Vec3 view_ori, view_dir;
        curr_camera->getDocPickRay(global_doc, (float)x + view_port.x, (float)y + view_port.y, &view_ori, &view_dir);

        if(infer_helper->get_infer_result().infer_type != InferType::NONE)
        {
            m_hover_pos = infer_helper->get_infer_result().position;
        }
        else
        {
            m_hover_pos = NousConvert::convert(view_ori);
        }

        const nous::scalar u = nurbs::get_u_on_curve(m_curve_path, NousConvert::convert(view_ori), NousConvert::convert(view_dir));
        m_cross_pos = m_curve_path->get_position(u);
        m_last_u = u;
        for(auto& nurbs_item_guid: m_cached_tmp_nurbs_items)
        {
            m_picked_nurbs_doc->nurbsdata()->remove_item(nurbs_item_guid);
        }
        m_cached_tmp_nurbs_items.clear();
        m_polygon_boundary_side_surfaces.clear();
        if(u > 0)
        {
            NurbsCurvePtr captured_path = m_curve_path_in_doc_transform->split_curve_at_u(u).first;
            if(!m_prepared_extrude_curve.empty())
            {
                for(int i = 0; i < m_prepared_extrude_curve.size(); ++i)
                {
                    NurbsAlgebraSurfacePtr ex_surface = nurbs::extrude_curve_to_surface(m_prepared_extrude_curve[i], captured_path);
                    ex_surface->set_render_level(ENurbsSurfaceRenderLevel::WireFrame, 1, 4);
                    ex_surface->set_render_wireframe_type(ENurbsSurfaceWireFrameShowType::UV_Boundary_With_U_Inside);
                    m_picked_nurbs_doc->nurbsdata()->add_item(ex_surface);
                    m_cached_tmp_nurbs_items.insert(ex_surface->id());
                    m_polygon_boundary_side_surfaces.insert({ i, ex_surface->id() });
                }
            }

            if(m_selected_algebra_surface)
            {
                auto&& new_surfaces = nurbs::offset_surface_to_solid_by_path(m_selected_algebra_surface, std::vector<NurbsCurvePtr>{ captured_path });
                for(int i = 0; i < new_surfaces.size(); ++i)
                {
                    NurbsAlgebraSurfacePtr ex_surface = new_surfaces[i];
                    ex_surface->set_render_level(ENurbsSurfaceRenderLevel::WireFrame, 1, 4);
                    ex_surface->set_render_wireframe_type(ENurbsSurfaceWireFrameShowType::UV_Boundary_With_U_Inside);
                    m_picked_nurbs_doc->nurbsdata()->add_item(ex_surface);
                    m_cached_tmp_nurbs_items.insert(ex_surface->id());
                }
            }

            if(m_selected_closed_surface)
            {
                auto&& new_surfaces = nurbs::offset_surface_to_solid_by_path(m_selected_closed_surface, std::vector<NurbsCurvePtr>{ captured_path });
                for(int i = 0; i < new_surfaces.size(); ++i)
                {
                    auto& ex_surface = new_surfaces[i];
                    if(i < new_surfaces.size() - 1)
                    {
                        ex_surface->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame, 1, 4);
                        ex_surface->to_nurbs_algebra_surface()->set_render_wireframe_type(ENurbsSurfaceWireFrameShowType::UV_Boundary_With_U_Inside);
                    }
                    m_picked_nurbs_doc->nurbsdata()->add_item(ex_surface);
                    m_cached_tmp_nurbs_items.insert(ex_surface->id());
                }
            }

            if(m_selected_trimmed_surface)
            {
                auto&& new_surfaces = nurbs::offset_surface_to_solid_by_path(m_selected_trimmed_surface, std::vector<NurbsCurvePtr>{ captured_path });
                for(int i = 0; i < new_surfaces.size(); ++i)
                {
                    auto& ex_surface = new_surfaces[i];
                    if(i < new_surfaces.size() - 1)
                    {
                        ex_surface->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame, 1, 4);
                        ex_surface->to_nurbs_algebra_surface()->set_render_wireframe_type(ENurbsSurfaceWireFrameShowType::UV_Boundary_With_U_Inside);
                    }
                    m_picked_nurbs_doc->nurbsdata()->add_item(ex_surface);
                    m_cached_tmp_nurbs_items.insert(ex_surface->id());
                }
            }
        }
        m_picked_nurbs_doc->dirty();
    }

    view->dirtyRT();
}

void Controller_Extrude_ByPath::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_ByPath::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);
    if(m_extrude_path_step == EExtrudePathStep::CreateSurface)
    {
        view->batchRenderLine(1.0, 0.8, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_cross_pos), NousConvert::convert(m_hover_pos), 0x0ff0, 1,
                              true, false);
        for(int i = 0; i < m_path_inference_line.size() - 1; ++i)
        {
            view->batchRenderLine(2.0, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_path_inference_line[i]),
                                  NousConvert::convert(m_path_inference_line[i + 1]), 0xffff, 1, true, false);
        }
    }
}

bool Controller_Extrude_ByPath::process_select_elements(std::unordered_map<RenderObject*, SelectInfo>& select_infos)
{
    int valid_line_count = 0;
    int valid_polygon_count = 0;
    for(auto& pair: select_infos)
    {
        auto doc_type = pair.first->doc()->type();
        const Engine::Mat43 doc_world_trans = pair.first->doc()->world_matrix();
        const nous::mat4 world_trans = NousConvert::convert(doc_world_trans);

        if(doc_type == Define::ObjectType::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(pair.first->doc());
            std::shared_ptr<GeoData> geodata = mesh_doc->geodata();
            for(auto& ele: pair.second.elements)
            {
                if(ele->type() == EElementType::Polygon)
                {
                    SketchPolygonPtr polygon = ele->to_polygon();
                    std::vector<std::vector<SketchPointPtr>> polygon_pt_arr;
                    GeoDataUtils::get().get_polygon_topo_and_reorder_by_weld_info(geodata.get(), polygon, polygon_pt_arr);
                    m_selected_polygon_positions.clear();
                    m_selected_polygon_boundary_weld_info.clear();
                    const auto exring_point_arr = polygon_pt_arr.front();
                    auto& exring_pos_arr = m_selected_polygon_positions.emplace_back();

                    auto& exring_weld_info_arr = m_selected_polygon_boundary_weld_info.emplace_back();
                    long old_group_idx = -2;
                    long inited_pos = -1;
                    for(int i = 0; i < exring_point_arr.size(); ++i)
                    {
                        SketchPointPtr prev_pt = exring_point_arr[i];
                        SketchPointPtr next_pt = exring_point_arr[(i + 1) % exring_point_arr.size()];
                        nous::pos_t prev_pt_pos = geodata->get_point_position(prev_pt);
                        nous::pos_t next_pt_pos = geodata->get_point_position(next_pt);
                        nous::pos_t trans_prev_pt_pos = NousConvert::product_v3(world_trans, prev_pt_pos);
                        nous::pos_t trans_next_pt_pos = NousConvert::product_v3(world_trans, next_pt_pos);
                        exring_pos_arr.emplace_back(trans_prev_pt_pos);

                        SketchLinePtr line = geodata->get_line(GeoData::line_key(prev_pt, next_pt));
                        size_t curr_group_idx = -1;
                        if(line->group() && line->group()->type() == EElementType::NoBranchLineGroup)
                        {
                            curr_group_idx = line->group()->id();
                        }

                        if(inited_pos == -1)
                        {
                            old_group_idx = curr_group_idx;
                            inited_pos = i;
                        }
                        else
                        {
                            if(old_group_idx != curr_group_idx || (old_group_idx == -1 && curr_group_idx == -1))
                            {
                                exring_weld_info_arr.emplace_back(inited_pos, i);
                                inited_pos = i;
                                old_group_idx = curr_group_idx;
                            }
                        }
                    }

                    if(inited_pos < exring_point_arr.size())
                    {
                        exring_weld_info_arr.emplace_back(inited_pos, exring_point_arr.size());
                    }

                    for(int i = 1; i < polygon_pt_arr.size(); ++i)
                    {
                        const auto& inring_point_arr = polygon_pt_arr[i];
                        auto& inring_pos_arr = m_selected_polygon_positions.emplace_back();

                        auto& inring_weld_info_arr = m_selected_polygon_boundary_weld_info.emplace_back();
                        old_group_idx = -2;
                        inited_pos = -1;
                        for(int j = 0; j < inring_point_arr.size(); ++j)
                        {
                            SketchPointPtr prev_pt = inring_point_arr[j];
                            SketchPointPtr next_pt = inring_point_arr[(j + 1) % inring_point_arr.size()];
                            nous::pos_t prev_pt_pos = geodata->get_point_position(prev_pt);
                            nous::pos_t next_pt_pos = geodata->get_point_position(next_pt);
                            nous::pos_t trans_prev_pt_pos = NousConvert::product_v3(world_trans, prev_pt_pos);
                            nous::pos_t trans_next_pt_pos = NousConvert::product_v3(world_trans, next_pt_pos);
                            inring_pos_arr.emplace_back(trans_prev_pt_pos);

                            SketchLinePtr line = geodata->get_line(GeoData::line_key(prev_pt, next_pt));
                            size_t curr_group_idx = -1;
                            if(line->group() && line->group()->type() == EElementType::NoBranchLineGroup)
                            {
                                curr_group_idx = line->group()->id();
                            }

                            if(inited_pos == -1)
                            {
                                old_group_idx = curr_group_idx;
                                inited_pos = j;
                            }
                            else
                            {
                                if(old_group_idx != curr_group_idx || (old_group_idx == -1 && curr_group_idx == -1))
                                {
                                    inring_weld_info_arr.emplace_back(inited_pos, j);
                                    inited_pos = j;
                                    old_group_idx = curr_group_idx;
                                }
                            }
                        }

                        if(inited_pos < inring_point_arr.size())
                        {
                            inring_weld_info_arr.emplace_back(inited_pos, inring_point_arr.size());
                        }
                    }

                    valid_polygon_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::Polygon;
                }
                else if(ele->type() == EElementType::Line)
                {
                    SketchLinePtr line = ele->to_line();
                    m_selected_lines.clear();
                    nous::pos_t prev_pt_pos = geodata->get_point_position(line->p0);
                    nous::pos_t next_pt_pos = geodata->get_point_position(line->p1);
                    nous::pos_t trans_prev_pt_pos = NousConvert::product_v3(world_trans, prev_pt_pos);
                    nous::pos_t trans_next_pt_pos = NousConvert::product_v3(world_trans, next_pt_pos);
                    m_selected_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                    valid_line_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::LineOrWeldLine;
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    NoBranchLineGroupPtr weld_line = ele->to_no_branch_line_group();
                    auto&& sub_elements = weld_line->elements();
                    auto&& line_positions = GeoDataTopoOperations::get().try_get_weld_line_position_array(geodata, sub_elements);
                    m_selected_lines.clear();
                    for(int i = 0; i < line_positions.size() - 1; ++i)
                    {
                        nous::pos_t prev_pt_pos = line_positions[i];
                        nous::pos_t next_pt_pos = line_positions[i + 1];
                        nous::pos_t trans_prev_pt_pos = NousConvert::product_v3(world_trans, prev_pt_pos);
                        nous::pos_t trans_next_pt_pos = NousConvert::product_v3(world_trans, next_pt_pos);
                        m_selected_lines.emplace_back(trans_prev_pt_pos, trans_next_pt_pos);
                        m_extrude_path_shape_type = EExtrudePathShapeType::LineOrWeldLine;
                    }
                    valid_line_count++;
                }
            }

            m_picked_nurbs_doc = nullptr;
        }

        if(doc_type == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(pair.first->doc());
            std::shared_ptr<NurbsData> nurbsdata = nurbs_doc->nurbsdata();

            m_selected_curves.clear();
            m_selected_closed_surface = nullptr;
            m_selected_algebra_surface = nullptr;
            m_selected_trimmed_surface = nullptr;

            for(auto& ele: pair.second.elements)
            {
                if(ele->type() == EElementType::Curve)
                {
                    m_selected_curves.emplace_back(ele->to_nurbs_curve());
                    valid_line_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::CurveOrCurveGroup;
                    m_picked_nurbs_doc = nurbs_doc;
                }
                else if(ele->type() == EElementType::AlgebraSurfaceEdge)
                {
                    m_selected_curves.emplace_back(ele->to_nurbs_algebra_surface_element()->m_edge_curve);
                    valid_line_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::CurveOrCurveGroup;
                    m_picked_nurbs_doc = nurbs_doc;
                }
                else if(ele->type() == EElementType::ClosedSurfaceEdge)
                {
                    m_selected_curves.emplace_back(ele->to_nurbs_closed_surface_element()->m_edge_curve);
                    valid_line_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::CurveOrCurveGroup;
                    m_picked_nurbs_doc = nurbs_doc;
                }
                else if(ele->type() == EElementType::AlgebraSurface)
                {
                    m_selected_algebra_surface = ele->to_nurbs_algebra_surface();
                    valid_polygon_count++;
                    m_extrude_path_shape_type = EExtrudePathShapeType::Surface;
                    m_picked_nurbs_doc = nurbs_doc;
                }
                else if(ele->type() == EElementType::ClosedSurface)
                {
                    valid_polygon_count++;
                    m_selected_closed_surface = ele->to_nurbs_closed_surface();
                    m_extrude_path_shape_type = EExtrudePathShapeType::Surface;
                    m_picked_nurbs_doc = nurbs_doc;
                }
                else if(ele->type() == EElementType::TrimmedSurface)
                {
                    valid_polygon_count++;
                    m_selected_trimmed_surface = ele->to_nurbs_trimmed_surface();
                    m_extrude_path_shape_type = EExtrudePathShapeType::Surface;
                    m_picked_nurbs_doc = nurbs_doc;
                }
            }
        }

        // GroupDoc...
    }

    if(valid_line_count + valid_polygon_count != 1)
    {
        m_selected_curves.clear();
        m_selected_lines.clear();
        valid_polygon_count = 0;
        valid_line_count = 0;
        m_picked_nurbs_doc = nullptr;
        return false;
    }

    m_is_polygon = valid_polygon_count > 0;

    return true;
}

bool Controller_Extrude_ByPath::process_path_element(std::unordered_map<RenderObject*, SelectInfo>& select_infos)
{
    bool has_valid_item = false;
    for(auto& pair: select_infos)
    {
        auto doc_type = pair.first->doc()->type();
        const Engine::Mat43 doc_world_trans = pair.first->doc()->world_matrix();
        const nous::mat4 world_trans = NousConvert::convert(doc_world_trans);

        if(doc_type == Define::ObjectType::OBJ_D5_MESH)
        {
            if(pair.second.elements.size() == 1)
            {
                MeshDoc* mesh_doc = static_cast<MeshDoc*>(pair.first->doc());
                std::shared_ptr<GeoData> geodata = mesh_doc->geodata();
                for(auto& ele: pair.second.elements)
                {
                    if(ele->type() == EElementType::Line)
                    {
                        SketchLinePtr sketch_line = ele->to_line();
                        nous::pos_t prev_pt_pos = geodata->get_point_position(sketch_line->p0);
                        nous::pos_t next_pt_pos = geodata->get_point_position(sketch_line->p1);
                        nous::vec4 prev_pt_pos4(prev_pt_pos.x(), prev_pt_pos.y(), prev_pt_pos.z(), 1.0);
                        nous::vec4 trans_prev_pt_pos4 = world_trans * prev_pt_pos4;
                        nous::vec4 next_pt_pos4(next_pt_pos.x(), next_pt_pos.y(), next_pt_pos.z(), 1.0);
                        nous::vec4 trans_next_pt_pos4 = world_trans * next_pt_pos4;
                        nous::pos_t trans_prev_pt_pos = trans_prev_pt_pos4.head<3>() / trans_prev_pt_pos4.w();
                        nous::pos_t trans_next_pt_pos = trans_next_pt_pos4.head<3>() / trans_next_pt_pos4.w();
                        m_curve_path = nurbs::convert_polyline_to_curve({ trans_prev_pt_pos, trans_next_pt_pos });
                        m_extrude_path_type = EExtrudePathType::Polyline;
                        has_valid_item = !has_valid_item;
                    }
                    else if(ele->type() == EElementType::NoBranchLineGroup)
                    {
                        NoBranchLineGroupPtr weld_line = ele->to_no_branch_line_group();
                        auto&& line_positions = GeoDataTopoOperations::get().try_get_weld_line_position_array(geodata, weld_line->elements());
                        if(sketch_math::point_equal(line_positions.front(), line_positions.back()))
                        {
                            m_curve_path = nullptr;
                            return false;
                        }

                        std::vector<nous::pos_t> path_positions;
                        for(int i = 0; i < line_positions.size(); ++i)
                        {
                            nous::pos_t pt_pos = line_positions[i];
                            nous::vec4 pt_pos4(pt_pos.x(), pt_pos.y(), pt_pos.z(), 1.0);
                            nous::vec4 trans_pt_pos4 = world_trans * pt_pos4;
                            nous::pos_t trans_pt_pos = trans_pt_pos4.head<3>() / trans_pt_pos4.w();
                            path_positions.emplace_back(trans_pt_pos);
                        }
                        m_curve_path = nurbs::convert_polyline_to_curve(path_positions);
                        m_extrude_path_type = EExtrudePathType::Polyline;
                        has_valid_item = !has_valid_item;
                    }
                }
            }
        }

        if(doc_type == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* mesh_doc = static_cast<NurbsDoc*>(pair.first->doc());
            std::shared_ptr<NurbsData> nurbsdata = mesh_doc->nurbsdata();

            if(pair.second.elements.size() == 1)
            {
                for(auto& ele: pair.second.elements)
                {
                    if(ele->type() == EElementType::Curve)
                    {
                        if(!ele->to_nurbs_curve()->is_curve_closed())
                        {
                            std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
                            m_curve_path = ele->to_nurbs_curve()->clone(remaped_map, world_trans)->to_nurbs_curve();
                            m_curve_path->dirty();
                            m_curve_path->get_sampled_point_arr();
                            m_extrude_path_type = EExtrudePathType::Curve;
                            has_valid_item = !has_valid_item;
                        }
                    }
                    else if(ele->type() == EElementType::AlgebraSurfaceEdge)
                    {
                        if(!ele->to_nurbs_algebra_surface_element()->m_edge_curve->is_curve_closed())
                        {
                            std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
                            m_curve_path = ele->to_nurbs_algebra_surface_element()->m_edge_curve->clone(remaped_map, world_trans)->to_nurbs_curve();
                            m_curve_path->dirty();
                            m_curve_path->get_sampled_point_arr();
                            m_extrude_path_type = EExtrudePathType::Curve;
                            has_valid_item = !has_valid_item;
                        }
                    }
                    else if(ele->type() == EElementType::ClosedSurfaceEdge)
                    {
                        NurbsClosedSurfaceEdgeElementPtr closed_surface_edge = ele->to_nurbs_closed_surface_element();
                        if(!closed_surface_edge->m_edge_curve.expired() && closed_surface_edge->m_edge_curve.lock()->is_curve_closed())
                        {
                            std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
                            m_curve_path = closed_surface_edge->m_edge_curve.lock()->clone(remaped_map, world_trans)->to_nurbs_curve();
                            m_curve_path->dirty();
                            m_curve_path->get_sampled_point_arr();
                            m_extrude_path_type = EExtrudePathType::Curve;
                            has_valid_item = !has_valid_item;
                        }
                    }
                }
            }
        }

        // GroupDoc...
    }

    // 若两个都有就两个都不要
    if(!has_valid_item)
    {
        m_curve_path = nullptr;
    }

    return has_valid_item;
}

bool Controller_Extrude_ByPath::generate_extrude_mesh_or_surface()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();

    if(!m_picked_nurbs_doc)
    {
        // 这玩意一定要存在一个来存储临时构建的曲面，最后 onmousedown 的时候会处理这玩意
        m_picked_nurbs_doc = (NurbsDoc*)view->doc()->scene()->createObject(Define::ObjectType::OBJ_D5_NURBS);
    }

    // 折线沿折线是 mesh，其余的都是 nurbs
    if(m_extrude_path_shape_type == EExtrudePathShapeType::LineOrWeldLine)
    {
        if(m_selected_lines.size() == 1)
        {
            // Line
            NurbsCurvePtr trans_curve = nurbs::convert_polyline_to_curve({ m_selected_lines.front().first, m_selected_lines.front().second });
            m_prepared_extrude_curve.emplace_back(trans_curve);
        }
        else
        {
            // Weld Line
            std::vector<nous::pos_t> line_positions;
            for(int i = 0; i < m_selected_lines.size(); ++i)
            {
                if(i == 0)
                {
                    line_positions.emplace_back(m_selected_lines[i].first);
                }
                line_positions.emplace_back(m_selected_lines[i].second);
            }
            NurbsCurvePtr trans_curve = nurbs::convert_polyline_to_curve(line_positions);
            m_prepared_extrude_curve.emplace_back(trans_curve);
        }
    }
    else if(m_extrude_path_shape_type == EExtrudePathShapeType::Polygon)
    {
        for(int i = 0; i < m_selected_polygon_positions.size(); ++i)
        {
            NurbsCurvePtr trans_curve = nurbs::convert_polyline_to_curve(m_selected_polygon_positions[i], true);
            m_prepared_extrude_curve.emplace_back(trans_curve);
        }
    }
    else
    {
        m_prepared_extrude_curve.insert(m_prepared_extrude_curve.end(), m_selected_curves.begin(), m_selected_curves.end());
    }

    // 判断鼠标位置与 path 的关系，从距离近的那头往远的方向推拉，涉及路径曲线的 reverse 操作
    const nous::pos_t mouse_position = view->inferHelper()->get_infer_result().position;
    const nous::pos_t& start_pos = m_curve_path->get_position(0.0);
    const nous::pos_t& end_pos = m_curve_path->get_position(1.0);

    const double dis_to_start = (mouse_position - start_pos).norm();
    const double dis_to_end = (mouse_position - end_pos).norm();

    if(dis_to_end < dis_to_start)
    {
        m_curve_path = nurbs::reverse_curve(m_curve_path);
    }
    const nous::pos_t new_start_pos = m_curve_path->get_position(0.0);
    const nous::vec3 offset = mouse_position - new_start_pos;
    nous::transform3 rel_trans = nous::transform3::Identity();
    rel_trans.translate(offset);
    std::unordered_set<SketchNurbsPointPtr> remaped_set;
    m_curve_path->transform(remaped_set, rel_trans.matrix());

    // 仅限初始选择了 NurbsDoc 时需要处理 inv_mat
    const Engine::Mat43 world_mat_inv = m_picked_nurbs_doc->world_matrix().inverse();
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
    m_curve_path_in_doc_transform = m_curve_path->clone(remaped_map, NousConvert::convert(world_mat_inv))->to_nurbs_curve();

    m_path_inference_line = m_curve_path->get_sampled_point_arr(100);

    m_picked_nurbs_doc->record();

    return true;
}

void Controller_Extrude_ByPath::rollback_to_pick_route_element()
{
    if(m_extrude_path_step == EExtrudePathStep::PickShapeElement)
    {
        m_curve_path = nullptr;
        m_prepared_extrude_curve.clear();
        m_extrude_path_step = EExtrudePathStep::PickRouteElement;
    }
}

void Controller_Extrude_ByPath::rollback_to_pick_shape_element()
{
    if(m_extrude_path_step == EExtrudePathStep::CreateSurface)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();

        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
        if(m_picked_nurbs_doc)
        {
            m_picked_nurbs_doc = nullptr;
        }
        m_curve_path_in_doc_transform = nullptr;

        m_path_inference_line.clear();
        m_selected_closed_surface = nullptr;
        m_selected_algebra_surface = nullptr;
        m_selected_curves.clear();
        m_selected_polygon_positions.clear();
        m_selected_polygon_boundary_weld_info.clear();
        m_selected_lines.clear();
        m_extrude_path_step = EExtrudePathStep::PickShapeElement;
    }
}

};// namespace sketch