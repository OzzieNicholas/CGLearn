#include "sketch/View/Controller/Main/Mesh/Controller_Edit.h"

#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/SmoothGroup.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
Controller_Edit::Controller_Edit(const std::wstring& name) : Controller_Main(name) {}

Controller_Edit::~Controller_Edit() {}

void Controller_Edit::active(const SketchParamMap& props) { Controller_Main::active(props); }

void Controller_Edit::deactive() { Controller_Main::deactive(); }

void Controller_Edit::_clear_affected_meshes()
{
    m_affected_bundles.clear();
    m_affected_mesh_local_position.clear();
    m_affected_nurbs_points.clear();
    m_affected_mesh_local_line.clear();
    m_affected_mesh_polygons.clear();
    m_affected_polygon_topo.clear();
    m_affected_polygon_triangles.clear();
    m_selected_polygons.clear();
    m_mesh_new_polygons.clear();
}

void Controller_Edit::_prepar_affect_meshes_elements(const std::unordered_map<RenderObject*, SelectInfo>& select_info)
{
    for(auto& info: select_info)
    {
        if(info.first->doc()->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = dynamic_cast<MeshDoc*>(info.first->doc());
            auto pick_geo = mesh_doc->geodata(true);

            std::unordered_map<SketchPointPtr, nous::vec3>& affected_points = m_affected_mesh_local_position[mesh_doc];
            std::unordered_set<SketchLinePtr>& affected_lines = m_affected_mesh_local_line[mesh_doc];
            std::unordered_set<SketchPolygonPtr>& affected_polygons = m_affected_mesh_polygons[mesh_doc];
            for(auto ele: info.second.elements)
            {
                if(ele->type() == EElementType::Point)
                {
                    auto point = ele->to_point();
                    if(affected_points.find(point) == affected_points.end())
                    {
                        affected_points[point] = pick_geo->get_point_position(point);
                        GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), point, affected_polygons);
                        GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), point, affected_lines);
                    }
                }
                else if(ele->type() == EElementType::Line)
                {
                    SketchLinePtr line = ele->to_line();
                    if(affected_points.find(line->p0) == affected_points.end())
                    {
                        affected_points[line->p0] = pick_geo->get_point_position(line->p0);
                        GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), line->p0, affected_polygons);
                        GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), line->p0, affected_lines);
                    }
                    if(affected_points.find(line->p1) == affected_points.end())
                    {
                        affected_points[line->p1] = pick_geo->get_point_position(line->p1);
                        GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), line->p1, affected_polygons);
                        GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), line->p1, affected_lines);
                    }
                }
                else if(ele->type() == EElementType::Polygon)
                {
                    std::vector<std::vector<SketchPointPtr>> polygon_points;
                    GeoDataUtils::get().get_polygon_topo(pick_geo.get(), ele->to_polygon(), polygon_points);
                    for(auto ring: polygon_points)
                    {
                        for(auto point: ring)
                        {
                            if(affected_points.find(point) == affected_points.end())
                            {
                                affected_points[point] = pick_geo->get_point_position(point);
                                GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), point, affected_polygons);
                                GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), point, affected_lines);
                            }
                        }
                    }
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    // group中的线段同时选中
                    for(auto group_child: ele->to_group()->elements())
                    {
                        SketchLinePtr child_line = group_child->to_line();
                        if(affected_points.find(child_line->p0) == affected_points.end())
                        {
                            affected_points[child_line->p0] = pick_geo->get_point_position(child_line->p0);
                            GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), child_line->p0, affected_polygons);
                            GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), child_line->p0, affected_lines);
                        }
                        if(affected_points.find(child_line->p1) == affected_points.end())
                        {
                            affected_points[child_line->p1] = pick_geo->get_point_position(child_line->p1);
                            GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), child_line->p1, affected_polygons);
                            GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), child_line->p1, affected_lines);
                        }
                    }
                }
                else if(ele->type() == EElementType::SmoothGroup)
                {
                    // group中的线段和面同时选中
                    SmoothGroupPtr smooth_group = std::dynamic_pointer_cast<SmoothGroup>(ele->to_group());
                    for(auto line: smooth_group->soft_edge())
                    {
                        SketchLinePtr child_line = line->to_line();
                        if(affected_points.find(child_line->p0) == affected_points.end())
                        {
                            affected_points[child_line->p0] = pick_geo->get_point_position(child_line->p0);
                            GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), child_line->p0, affected_polygons);
                            GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), child_line->p0, affected_lines);
                        }
                        if(affected_points.find(child_line->p1) == affected_points.end())
                        {
                            affected_points[child_line->p1] = pick_geo->get_point_position(child_line->p1);
                            GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), child_line->p1, affected_polygons);
                            GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), child_line->p1, affected_lines);
                        }
                    }
                    for(auto polygon: smooth_group->soft_polygon())
                    {
                        std::vector<std::vector<SketchPointPtr>> polygon_points;
                        GeoDataUtils::get().get_polygon_topo(pick_geo.get(), polygon->to_polygon(), polygon_points);
                        for(auto ring: polygon_points)
                        {
                            for(auto point: ring)
                            {
                                if(affected_points.find(point) == affected_points.end())
                                {
                                    affected_points[point] = pick_geo->get_point_position(point);
                                    GeoDataUtils::get().get_point_adj_polygons(pick_geo.get(), point, affected_polygons);
                                    GeoDataUtils::get().get_point_adj_lines(pick_geo.get(), point, affected_lines);
                                }
                            }
                        }
                    }
                }
            }
            for(auto polygon: affected_polygons)
            {
                if(info.second.elements.find(polygon) != info.second.elements.end())
                {
                    m_selected_polygons.insert(polygon);
                }
                auto& polygon_topo = m_affected_polygon_topo[polygon];
                GeoDataUtils::get().get_polygon_topo(pick_geo.get(), polygon, polygon_topo);
                m_affected_polygon_triangles[polygon] = std::move(polygon->triangles);
                polygon->m_changed = true;
            }
            mesh_doc->dirty();
        }
        else if(info.first->doc()->type() == Define::OBJ_D5_NURBS)
        {
            NurbsDoc* pick_nurbs_doc = dynamic_cast<NurbsDoc*>(info.first->doc());
            const Engine::Mat43 world_matrix = pick_nurbs_doc->world_matrix();
            const nous::mat4 world_trans = NousConvert::convert(world_matrix);
            auto pick_nurbs_data = pick_nurbs_doc->nurbsdata(true);
            std::unordered_set<SketchNurbsPointPtr> affected_point_set;
            for(auto ele: info.second.elements)
            {
                if(ele->to_nurbs_curve())
                {
                    auto&& curve_ctp_arr = ele->to_nurbs_curve()->get_curve_control_points();
                    affected_point_set.insert(curve_ctp_arr.begin(), curve_ctp_arr.end());
                }
                else if(ele->to_nurbs_algebra_surface_element())
                {
                    auto&& curve_ctp_arr = ele->to_nurbs_algebra_surface_element()->m_edge_curve->get_curve_control_points();
                    affected_point_set.insert(curve_ctp_arr.begin(), curve_ctp_arr.end());
                }
                else if(ele->to_nurbs_closed_surface_element())
                {
                    auto&& curve_ctp_arr = ele->to_nurbs_closed_surface_element()->m_edge_curve.lock()->get_curve_control_points();
                    affected_point_set.insert(curve_ctp_arr.begin(), curve_ctp_arr.end());
                }
                else if(ele->to_nurbs_algebra_surface() || ele->to_nurbs_closed_surface() || ele->to_nurbs_trimmed_surface())
                {
                    auto&& surface_ctp_arr = ele->to_nurbs_surface_base()->get_surface_control_points_1D();
                    affected_point_set.insert(surface_ctp_arr.begin(), surface_ctp_arr.end());
                }
            }
            std::vector<std::pair<SketchNurbsPointPtr, nous::pos_t>> ctp_with_origin_pos;
            for(auto& ctp_ptr: affected_point_set)
            {
                ctp_with_origin_pos.emplace_back(std::make_pair(ctp_ptr, ctp_ptr->get_position()));
            }
            m_affected_nurbs_points[pick_nurbs_doc] = ctp_with_origin_pos;
        }
        else if(info.first->doc()->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* group_doc = info.first->doc()->_group();
            group_doc->record();
            m_affected_bundles[group_doc] = NousConvert::convert(group_doc->world_matrix());
        }
    }
}

void Controller_Edit::_update_new_meshes()
{
    m_mesh_new_polygons.clear();
    for(auto& affected_mesh_polygon: m_affected_mesh_polygons)
    {
        auto geo = affected_mesh_polygon.first->geodata();
        std::unordered_map<std::shared_ptr<SketchPolygon>, std::vector<std::vector<std::vector<nous::vec3>>>>& new_polygons =
            m_mesh_new_polygons[affected_mesh_polygon.first];
        for(auto& polygon_ptr: affected_mesh_polygon.second)
        {
            if(m_keep_connection)
            {
                GeoDataTopoOperations::get().merge_polygon_triangle(geo.get(), polygon_ptr->triangles, new_polygons[polygon_ptr], true);
            }
            else
            {
                // 通过环生成新的多边形
                auto& polygon_topo = m_affected_polygon_topo[polygon_ptr];
                std::vector<nous::vec3> polygon_ring;
                std::vector<std::vector<nous::vec3>> polygon_inner_ring;
                for(int i = 0; i < polygon_topo.size(); ++i)
                {
                    std::vector<nous::vec3>& change_ring = i == 0 ? polygon_ring : polygon_inner_ring.emplace_back();
                    for(auto& point: polygon_topo[i])
                    {
                        change_ring.emplace_back(std::move(geo->get_point_position(point)));
                    }
                }
                nous::vec3 normal = sketch_math::ring_normal(polygon_ring);

                std::vector<std::vector<nous::vec3>> polygon_coplane_inner_ring;
                for(auto& inner_ring: polygon_inner_ring)
                {
                    nous::vec3 inner_normal = sketch_math::ring_normal(inner_ring);
                    if(sketch_math::plane_coplane_plane(inner_normal, inner_ring.front(), normal, polygon_ring.front()))
                    {
                        polygon_coplane_inner_ring.emplace_back(std::move(inner_ring));
                    }
                }
                geos_util::ring_difference_other_ring(polygon_ring, polygon_coplane_inner_ring, normal, new_polygons[polygon_ptr]);
            }
        }

        affected_mesh_polygon.first->dirty();
    }
}

void Controller_Edit::_render_new_meshes(View* view)
{
    for(auto& mesh_new_polygon: m_mesh_new_polygons)
    {
        for(auto& polygons: mesh_new_polygon.second)
        {
            bool is_selected = m_selected_polygons.find(polygons.first) != m_selected_polygons.end();
            Engine::Vec3f color = is_selected ? Engine::Vec3f(0.9f, 0.9, 0.9f) : Engine::Vec3f(0.4f, 0.4f, 0.4f);
            for(auto& polygon: polygons.second)
            {
                std::vector<std::vector<nous::vec3>> result = geodata_topo::triangulate_polygon(polygon);
                for(auto& tri: result)
                {
                    view->batchRenderTri(color, NousConvert::convert(tri[0]), NousConvert::convert(tri[1]), NousConvert::convert(tri[2]), true, true,
                                         true, 1.0f);
                    view->batchRenderLine(1, 0.2f, Engine::Vec3f::GREEN, NousConvert::convert(tri[0]), NousConvert::convert(tri[1]), 0x0f0fu, 1, true,
                                          true);
                    view->batchRenderLine(1, 0.2f, Engine::Vec3f::GREEN, NousConvert::convert(tri[1]), NousConvert::convert(tri[2]), 0x0f0fu, 1, true,
                                          true);
                    view->batchRenderLine(1, 0.2f, Engine::Vec3f::GREEN, NousConvert::convert(tri[2]), NousConvert::convert(tri[0]), 0x0f0fu, 1, true,
                                          true);
                }
            }
        }
    }
}

void Controller_Edit::_add_affected_element_to_infer_ignore()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();

    for(auto bundle: m_affected_bundles)
    {
        TFObjectDoc* obj = bundle.first;
        if(obj->_group())
        {
            std::stack<GroupDoc*> stack;
            stack.push(obj->_group());
            while(!stack.empty())
            {
                GroupDoc* group = stack.top();
                stack.pop();
                for(auto child: group->children())
                {
                    if(child->type() == Define::OBJ_D5_GROUP)
                    {
                        stack.push(child->_group());
                    }
                    else// if(child->type() == Define::OBJ_D5_MESH)
                    {
                        infer_helper->add_ignore(child->renderObject());
                    }
                }
            }
        }
    }

    for(auto mesh_affected_points: m_affected_mesh_local_position)
    {
        std::unordered_set<SketchElementPtr> elements;
        for(auto point: mesh_affected_points.second)
        {
            elements.insert(point.first);
        }
        infer_helper->add_ignore(mesh_affected_points.first->renderObject(), elements);
    }
    for(auto mesh_affected_lines: m_affected_mesh_local_line)
    {
        std::unordered_set<SketchElementPtr> elements;
        for(auto line_key: mesh_affected_lines.second)
        {
            elements.insert(line_key);
        }
        infer_helper->add_ignore(mesh_affected_lines.first->renderObject(), elements);
    }
    for(auto mesh_affected_polygons: m_affected_mesh_polygons)
    {
        std::unordered_set<SketchElementPtr> elements;
        for(auto polygon: mesh_affected_polygons.second)
        {
            elements.insert(polygon);
        }
        infer_helper->add_ignore(mesh_affected_polygons.first->renderObject(), elements);
    }
}
}// namespace sketch