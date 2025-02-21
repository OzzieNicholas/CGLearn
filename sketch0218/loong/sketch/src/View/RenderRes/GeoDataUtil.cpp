#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "CDT.h"
#include "VerifyTopology.h"
#include "geos/operation/buffer/BufferOp.h"
#include "geos/operation/polygonize/Polygonizer.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/SmoothGroup.h"
#include <execution>
#include <sketch/NousEngine/NousConvert.h>

#include "sketch/NousEngine/SketchMath.h"

#include <sketch/NousEngine/earcut.hpp>

namespace sketch
{

GeoDataUtils& GeoDataUtils::get()
{
    static GeoDataUtils _instance;
    return _instance;
}

nous::AABB3 GeoDataUtils::get_aabb3(GeoData* geodata, const std::vector<nous::vec3>& points) { return sketch_math::aabb3_from_points(points, 0.1f); }

nous::AABB3 GeoDataUtils::get_aabb3(GeoData* geodata, const std::vector<SketchPointPtr>& points)
{
    std::vector<nous::vec3> points_pos;
    for(auto p: points)
    {
        nous::vec3 point_origin = geodata->get_point_position(p);
        points_pos.emplace_back(point_origin);
    }
    return sketch_math::aabb3_from_points(points_pos, 0.1f);
}

nous::AABB3 GeoDataUtils::get_element_aabb3(GeoData* geodata, SketchElementPtr element)
{
    if(element->type() == EElementType::Line)
    {
        // 强制更新position handle
        geodata->m_dirty_flags |= 0x2;
        geodata->_sync_nous();

        SketchLinePtr line = element->to_line();
        nous::vec3 pos0 = geodata->get_point_position(line->p0->point);
        nous::vec3 pos1 = geodata->get_point_position(line->p1->point);

        return sketch_math::aabb3_from_points({ pos0, pos1 }, 0.1f);
    }
    else if(element->type() == EElementType::Ring)
    {
        // 强制更新position handle
        geodata->m_dirty_flags |= 0x2;
        geodata->_sync_nous();

        SketchRingPtr ring = element->to_ring();
        std::vector<nous::vec3> points_pos = get_ring_positions(geodata, ring);

        return sketch_math::aabb3_from_points(points_pos, 0.1f);
    }
    return nous::AABB3();
}

void GeoDataUtils::get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<SketchPointPtr>>& out_polygon)
{
    std::vector<SketchPointPtr>& exring_points_arr = out_polygon.emplace_back();
    geodata->get_element_points(polygon->ring, exring_points_arr);

    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr>& inring_points_arr = out_polygon.emplace_back();
        geodata->get_element_points(inner_ring, inring_points_arr);
    }
}

std::vector<std::vector<SketchPointPtr>> GeoDataUtils::get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::vector<std::vector<SketchPointPtr>> result;
    get_polygon_topo(geodata, polygon, result);
    return result;
}

void GeoDataUtils::get_polygon_topo_and_reorder_by_weld_info(GeoData* geodata, SketchPolygonPtr polygon,
                                                             std::vector<std::vector<SketchPointPtr>>& out_polygon)
{
    get_polygon_topo(geodata, polygon, out_polygon);

    for(int i = 0; i < out_polygon.size(); ++i)
    {
        auto& out_ring = out_polygon[i];
        // 保证第一段线一定是焊接线的起始，或者不是焊接线
        int offset_number = 0;
        SketchPointPtr first_point = out_ring[0];
        SketchPointPtr second_point = out_ring[1];

        size_t first_line_key = GeoData::line_key(first_point, second_point);
        SketchLinePtr first_line = geodata->get_line(first_line_key);

        size_t curr_group_idx = -1;
        if(first_line->group() && first_line->group()->type() == EElementType::NoBranchLineGroup)
        {
            curr_group_idx = first_line->group()->id();
        }

        for(int j = 1; j < out_ring.size() - 1; ++j)
        {
            SketchPointPtr prev_point = out_ring[j];
            SketchPointPtr next_point = out_ring[j + 1];

            size_t behind_line_key = GeoData::line_key(prev_point, next_point);
            SketchLinePtr behind_line = geodata->get_line(behind_line_key);
            size_t next_group_idx = -1;
            if(behind_line->group() && behind_line->group()->type() == EElementType::NoBranchLineGroup)
            {
                next_group_idx = behind_line->group()->id();
            }

            if(next_group_idx != curr_group_idx)
            {
                offset_number = j;
                break;
            }
        }

        if(offset_number != 0)
        {
            // 偏移
            std::vector<SketchPointPtr> offseted_points_arr(out_ring.size(), nullptr);
            for(int j = 0; j < out_ring.size(); ++j)
            {
                offseted_points_arr[j] = out_ring[(j + offset_number) % out_ring.size()];
            }
            out_polygon[i] = offseted_points_arr;
        }
    }
}

void GeoDataUtils::get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<nous::pos_t>>& out_pos_arr)
{
    std::vector<SketchPointPtr> exring_points_arr;
    std::vector<nous::pos_t>& exring_pos_arr = out_pos_arr.emplace_back();
    geodata->get_element_points(polygon->ring, exring_points_arr);
    for(auto one_point: exring_points_arr)
    {
        // if(!one_point->vertex_ptr())
        //{
        //     continue;
        // }
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        exring_pos_arr.emplace_back(point_origin);
    }

    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr> inring_points_arr;
        geodata->get_element_points(inner_ring, inring_points_arr);
        std::vector<nous::pos_t>& inring_pos_arr = out_pos_arr.emplace_back();
        for(auto one_point: inring_points_arr)
        {
            // if(!one_point->vertex_ptr())
            //{
            //     continue;
            // }
            nous::vec3 point_origin = geodata->get_point_position(one_point);
            inring_pos_arr.emplace_back(point_origin);
        }
    }
}

std::vector<std::vector<nous::pos_t>> GeoDataUtils::get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::vector<std::vector<nous::pos_t>> result;
    get_polygon_positions(geodata, polygon, result);
    return result;
}

void GeoDataUtils::get_polygon_positions(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& polygon,
                                         std::vector<std::vector<nous::pos_t>>& out_pos_arr)
{
    for(int i = 0; i < polygon.size(); ++i)
    {
        std::vector<nous::pos_t>& ring_pos_arr = out_pos_arr.emplace_back();
        for(auto one_point: polygon[i])
        {
            if(!one_point->point->vertex_ptr())
            {
                continue;
            }
            nous::vec3 point_origin = geodata->get_point_position(one_point);
            ring_pos_arr.emplace_back(point_origin);
        }
    }
}

std::vector<SketchPointPtr> GeoDataUtils::get_ring_topo(GeoData* geodata, SketchRingPtr ring)
{
    std::vector<SketchPointPtr> result;
    geodata->get_element_points(ring, result);
    return result;
}

void GeoDataUtils::get_ring_topo(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& out_topo)
{
    geodata->get_element_points(ring, out_topo);
}

std::vector<nous::pos_t> GeoDataUtils::get_ring_positions(GeoData* geodata, SketchRingPtr ring)
{
    std::vector<nous::pos_t> result;
    get_ring_positions(geodata, ring, result);
    return result;
}

void GeoDataUtils::get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring, std::vector<nous::pos_t>& out_pos_arr)
{
    for(auto one_point: ring)
    {
        if(!one_point->point->vertex_ptr())
        {
            continue;
        }
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        out_pos_arr.emplace_back(point_origin);
    }
}

std::vector<nous::pos_t> GeoDataUtils::get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring)
{
    std::vector<nous::pos_t> result;
    get_ring_positions(geodata, ring, result);
    return result;
}

void GeoDataUtils::get_ring_positions(GeoData* geodata, const SketchRingPtr ring, std::vector<nous::pos_t>& out_pos_arr)
{
    std::vector<SketchPointPtr> ring_pts;
    geodata->get_element_points(ring, ring_pts);
    for(auto one_point: ring_pts)
    {
        if(!one_point->point->vertex_ptr())
        {
            continue;
        }
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        out_pos_arr.emplace_back(point_origin);
    }
}

bool GeoDataUtils::is_two_ring_coplane(GeoData* geodata, const SketchRingPtr ring0, const SketchRingPtr ring1)
{
    const nous::vec3& ring0_normal = geodata->get_mesh()->prim_normal(ring0->ring_prim->idx());
    const nous::vec3& ring1_normal = geodata->get_mesh()->prim_normal(ring1->ring_prim->idx());
    const nous::pos_t& ring0_pos = geodata->get_point_position(geodata->m_nousptr_points.at(ring0->ring_prim->vertex_ptr()->point_ptr()));
    const nous::pos_t& ring1_pos = geodata->get_point_position(geodata->m_nousptr_points.at(ring1->ring_prim->vertex_ptr()->point_ptr()));

    return sketch_math::plane_coplane_plane(ring0_normal, ring0_pos, ring1_normal, ring1_pos);
}

bool GeoDataUtils::is_line_on_ring(GeoData* geodata, SketchLinePtr line, const std::vector<SketchPointPtr>& ring)
{
    for(size_t i = 0; i < ring.size(); i++)
    {
        SketchPointPtr a = ring[i];
        SketchPointPtr b = ring[(i + 1) % ring.size()];

        if(line->key == geodata->line_key(a, b))
        {
            return true;
        }
    }

    return false;
}

bool GeoDataUtils::is_point_on_ring(GeoData* geodata, SketchPointPtr, const std::vector<SketchPointPtr>& ring)
{
    for(auto point: ring)
    {
        if(point == point)
        {
            return true;
        }
    }
    return false;
}

bool GeoDataUtils::is_ring_in_target_path(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                          const std::vector<SketchPointPtr>& target_ring, std::vector<std::vector<SketchPointPtr>> result)
{
    bool onPath = false;
    std::vector<SketchPointPtr> splitPath;
    SketchPointPtr firstPathBegin = nullptr;

    size_t index = 0;
    while(true)
    {
        auto cp = ring[index % ring.size()];
        auto np = ring[(index + 1) % ring.size()];
        ++index;
        if(cp == firstPathBegin)
        {
            break;
        }

        nous::vec3 cpp = geodata->get_point_position(cp);
        nous::vec3 npp = geodata->get_point_position(np);
        std::vector<nous::pos_t> target_ring_positions;
        get_ring_positions(geodata, target_ring, target_ring_positions);
        topo::ESketchTopoRelation cps = topo::calc_point_and_ring_relation(normal, cpp, target_ring_positions);
        topo::ESketchTopoRelation nps = topo::calc_point_and_ring_relation(normal, npp, target_ring_positions);
        if(onPath)
        {
            splitPath.push_back(cp);
            if(cps == topo::ESketchTopoRelation::OnBoundary)
            {
                onPath = false;
                result.push_back(splitPath);
            }
        }
        if(cps == topo::ESketchTopoRelation::OnBoundary && nps == topo::ESketchTopoRelation::Inside)
        {
            onPath = true;
            splitPath.clear();
            splitPath.push_back(cp);
            if(!firstPathBegin)
                firstPathBegin = cp;
        }
        if(!firstPathBegin && index == ring.size())
        {
            break;
        }
    }
    return result.size();
}

bool GeoDataUtils::is_ring_contain_target(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                          const std::vector<SketchPointPtr>& target_ring)
{
    for(int i = 0; i < target_ring.size(); i++)
    {
        auto cp = target_ring[i];
        // if(is_point_on_ring(geodata, cp, ring))
        //{
        //     continue;
        // }
        auto np = target_ring[(i + 1) % target_ring.size()];
        if(is_line_on_ring(geodata, geodata->m_key_lines.at(GeoData::line_key(cp, np)), ring))
        {
            continue;
        }

        nous::vec3 cpp = geodata->get_point_position(cp);
        nous::vec3 npp = geodata->get_point_position(np);

        std::vector<nous::pos_t> ring_positions = get_ring_positions(geodata, ring);
        if(topo::calc_point_and_ring_relation(normal, cpp + (npp - cpp) * 0.5, ring_positions) == topo::ESketchTopoRelation::Outside)
        {
            return false;
        }
    }
    return true;
}

void GeoDataUtils::get_ring_relation_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, SketchRingPtr& contained,
                                           std::unordered_set<SketchRingPtr>& contains)
{
    geodata->_sync_nous();

    std::vector<nous::vec3> vp;
    for(auto np: ring)
    {
        vp.emplace_back(std::move(geodata->get_point_position(np)));
    }
    nous::vec3 c_normal = sketch_math::ring_normal(vp);

    size_t ring_key = GeoData::ring_key(ring);
    SketchRingPtr superior_ring = nullptr;
    std::vector<SketchPointPtr> superior_ring_topo;

    for(auto& ring_polygon_pair: geodata->get_ring_relation_polygons())
    {
        SketchRingPtr target_ring = ring_polygon_pair.first;
        std::vector<SketchPointPtr> target_ring_topo = geodata->get_element_points(target_ring);
        size_t target_ring_key = GeoData::ring_key(target_ring_topo);
        if(target_ring_key == ring_key)
        {
            continue;
        }

        nous::vec3 t_normal = target_ring->normal;
        bool coplane = sketch_math::plane_coplane_plane(c_normal, geodata->get_point_position(ring[0]), t_normal,
                                                        geodata->get_point_position(target_ring_topo[0]));
        if(coplane)
        {
            // contain&contained
            if(is_ring_contain_target(geodata, t_normal, ring, target_ring_topo))
            {
                // the rings contained by input ring.
                contains.insert(target_ring);
            }
            else if(is_ring_contain_target(geodata, t_normal, target_ring_topo, ring))
            {
                // the only ring smaller contain the input ring.
                if(superior_ring)
                {
                    if(is_ring_contain_target(geodata, t_normal, superior_ring_topo, target_ring_topo))
                    {
                        superior_ring = target_ring;
                        superior_ring_topo = target_ring_topo;
                    }
                }
                else
                {
                    superior_ring = target_ring;
                    superior_ring_topo = target_ring_topo;
                }
            }
        }
    }

    contained = superior_ring;
}

void GeoDataUtils::get_ring_relation_rings(GeoData* geodata, SketchRingPtr ring, SketchRingPtr contained, std::unordered_set<SketchRingPtr>& contains)
{
    std::vector<SketchPointPtr> ring_points = get_ring_topo(geodata, ring);
    get_ring_relation_rings(geodata, ring_points, contained, contains);
}

void GeoDataUtils::get_prims_contains_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines,
                                                std::unordered_set<SketchRingPtr>& prim_set)
{
    for(const auto& pm_idx: geodata->get_mesh()->prim_index_range())
    {
        SketchRingPtr curr_ring = geodata->m_nousptr_rings.at(geodata->get_mesh()->prim(pm_idx));
        if(curr_ring)
        {
            const std::unordered_set<SketchLinePtr> ring_adj_lines = geodata->get_ring_adj_lines(curr_ring);
            if(!ring_adj_lines.empty())
            {
                bool all_contain = true;
                for(auto line: lines)
                {
                    if(!ring_adj_lines.count(line))
                    {
                        all_contain = false;
                        break;
                    }
                }

                if(all_contain)
                {
                    prim_set.insert(curr_ring);
                }
            }
        }
    }
}

void GeoDataUtils::get_polygons_contained_by_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines,
                                                       std::unordered_set<SketchPolygonPtr>& result)
{
    for(const auto& pm_idx: geodata->get_mesh()->prim_index_range())
    {
        nous::Prim* ele_prim = geodata->get_mesh()->prim(pm_idx);
        if(ele_prim && !geodata->is_line(ele_prim))
        {
            SketchRingPtr curr_prim = geodata->m_nousptr_rings.at(ele_prim);
            const std::unordered_set<SketchLinePtr> ring_adj_lines = geodata->get_ring_adj_lines(curr_prim);
            if(!ring_adj_lines.empty())
            {
                bool all_contain = true;
                for(auto line: ring_adj_lines)
                {
                    if(lines.find(line) == lines.end())
                    {
                        all_contain = false;
                        break;
                    }
                }

                if(all_contain)
                {
                    SketchPolygonPtr skp = geodata->get_polygon(curr_prim);
                    if(skp)
                    {
                        result.insert(skp);
                    }
                }
            }
        }
    }
}

std::unordered_set<SketchLinePtr> GeoDataUtils::get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::unordered_set<SketchLinePtr> result;
    get_polygon_adj_lines(geodata, polygon, result);
    return result;
}

void GeoDataUtils::get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon, std::unordered_set<SketchLinePtr>& result)
{
    std::vector<SketchPointPtr> ring_points;
    geodata->get_element_points(polygon->ring, ring_points);
    for(int i = 0; i < ring_points.size(); ++i)
    {
        SketchPointPtr a = ring_points[i];
        SketchPointPtr b = ring_points[(i + 1) % ring_points.size()];
        size_t line_key = geodata->line_key(a, b);
        result.insert(geodata->m_key_lines.at(line_key));
    }

    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr> inner_ring_points;
        geodata->get_element_points(inner_ring, inner_ring_points);
        for(int i = 0; i < inner_ring_points.size(); ++i)
        {
            SketchPointPtr a = inner_ring_points[i];
            SketchPointPtr b = inner_ring_points[(i + 1) % inner_ring_points.size()];
            size_t line_key = geodata->line_key(a, b);
            result.insert(geodata->m_key_lines.at(line_key));
        }
    }
}

std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring)
{
    std::unordered_set<SketchPolygonPtr> result;
    get_ring_adj_polygons(geodata, ring, result);
    return result;
}

void GeoDataUtils::get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring, std::unordered_set<SketchPolygonPtr>& result)
{
    SketchPolygonPtr skp = geodata->get_polygon(ring);
    if(skp)
    {
        result.insert(skp);
    }
    SketchPolygonPtr skp_p = geodata->get_parent_polygon(ring);
    if(skp_p)
    {
        result.insert(skp_p);
    }
}

std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_line_adj_polygons(GeoData* geodata, SketchLinePtr line)
{
    std::unordered_set<SketchPolygonPtr> result;
    get_line_adj_polygons(geodata, line, result);
    return result;
}

void GeoDataUtils::get_line_adj_polygons(GeoData* geodata, SketchLinePtr line, std::unordered_set<SketchPolygonPtr>& result)
{
    std::unordered_set<SketchRingPtr> rings = geodata->get_line_adj_rings(line);
    for(auto ring: rings)
    {
        std::unordered_set<SketchPolygonPtr> ring_adj_polygons = get_ring_adj_polygons(geodata, ring);
        result.insert(ring_adj_polygons.begin(), ring_adj_polygons.end());
    }
}

std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_point_adj_polygons(GeoData* geodata, SketchPointPtr point)
{
    std::unordered_set<SketchPolygonPtr> result;
    get_point_adj_polygons(geodata, point, result);
    return result;
}

void GeoDataUtils::get_point_adj_polygons(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPolygonPtr>& result)
{
    std::unordered_set<SketchRingPtr> rings = geodata->get_point_adj_rings(point);
    for(auto ring: rings)
    {
        std::unordered_set<SketchPolygonPtr> ring_adj_polygons = get_ring_adj_polygons(geodata, ring);
        result.insert(ring_adj_polygons.begin(), ring_adj_polygons.end());
    }
}

std::unordered_set<SketchLinePtr> GeoDataUtils::get_point_adj_lines(GeoData* geodata, SketchPointPtr point)
{
    std::unordered_set<SketchLinePtr> adj_lines;
    get_point_adj_lines(geodata, point, adj_lines);
    return adj_lines;
}

void GeoDataUtils::get_point_adj_lines(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchLinePtr>& result)
{
    geodata->get_point_adj_lines(point, result);
}

std::vector<std::vector<size_t>> GeoDataUtils::convert_polygon_to_rel_point_idx(GeoData* geodata, SketchPolygonPtr polygon,
                                                                                std::vector<SketchPointPtr>& id_point_map)
{
    std::vector<std::vector<size_t>> result;
    std::vector<SketchPointPtr> ring_pt_arr;
    geodata->get_element_points(polygon->ring, ring_pt_arr);
    std::map<SketchPointPtr, size_t> point_id_map;
    std::vector<size_t>& exring_rel_idx_arr = result.emplace_back();
    for(int i = 0; i < ring_pt_arr.size(); ++i)
    {
        if(point_id_map.find(ring_pt_arr[i]) == point_id_map.end())
        {
            point_id_map.insert({ ring_pt_arr[i], point_id_map.size() });
        }
        exring_rel_idx_arr.emplace_back(point_id_map[ring_pt_arr[i]]);
    }

    for(auto& inring: polygon->inner_rings)
    {
        ring_pt_arr.clear();
        geodata->get_element_points(inring, ring_pt_arr);
        std::vector<size_t>& inring_rel_idx_arr = result.emplace_back();
        for(int i = 0; i < ring_pt_arr.size(); ++i)
        {
            if(point_id_map.find(ring_pt_arr[i]) == point_id_map.end())
            {
                point_id_map.insert({ ring_pt_arr[i], point_id_map.size() });
            }
            inring_rel_idx_arr.emplace_back(point_id_map[ring_pt_arr[i]]);
        }
    }

    id_point_map.resize(point_id_map.size());
    std::for_each(std::execution::par, point_id_map.begin(), point_id_map.end(), [&](auto& pair) { id_point_map[pair.second] = pair.first; });

    return result;
}

// add
// 检测目标多边形是否完全被另一个多边形包含
bool GeoDataUtils::is_polygon_contained(GeoData* geodata, const std::vector<std::vector<nous::vec3>>& target_polygon,
                                        SketchPolygonPtr container_polygon)
{
    std::vector<std::vector<nous::vec3>> container_positions;
    get_polygon_positions(geodata, container_polygon, container_positions);
    // 顶点包含检测（需要完善代码）
    for (auto& pt_ring : target_polygon)
    {
        for (auto& pt : pt_ring)
        {
            if (!is_point_inside_polygon(pt, container_positions))
            {
                return false;
            }
        }
    }
    // 边界交叉检测（需要完善代码）
    if (!do_polygons_intersect(target_polygon, container_positions))
    {
        return false;
    }
    return true;
}
// add

GeoDataTopoOperations& GeoDataTopoOperations::get()
{
    static GeoDataTopoOperations _instance;
    return _instance;
}

void GeoDataTopoOperations::merge_polygon_triangle(GeoData* geodata, std::vector<SketchPolygonTriangle>& vp,
                                                   std::vector<std::vector<std::vector<nous::vec3>>>& topos, bool update_sketch_trangle)
{
    std::unordered_map<nous::vec3, std::vector<std::vector<nous::vec3>>> plane_coplane_plane;

    for(auto& tri: vp)
    {
        if(update_sketch_trangle)
        {
            if(tri[0].first)
            {
                tri[0].second = geodata->get_point_position(tri[0].first);
            }
            if(tri[1].first)
            {
                tri[1].second = geodata->get_point_position(tri[1].first);
            }
            if(tri[2].first)
            {
                tri[2].second = geodata->get_point_position(tri[2].first);
            }
        }
        std::vector<nous::vec3> triData;
        triData.push_back(tri[0].second);
        triData.push_back(tri[1].second);
        triData.push_back(tri[2].second);
        nous::vec3 normal = sketch_math::ring_normal(triData);
        bool find = false;
        for(auto& plane: plane_coplane_plane)
        {
            if(sketch_math::plane_coplane_plane(plane.first, plane.second[0][0], normal, triData[0]))
            {
                plane.second.emplace_back(std::move(triData));
                find = true;
                break;
            }
        }
        if(!find)
        {
            plane_coplane_plane[normal].emplace_back(std::move(triData));
        }
    }

    for(auto& plane: plane_coplane_plane)
    {
        geos_util::union_all_rings(plane.second, plane.first, topos);
    }
}

SketchRingPtr GeoDataTopoOperations::merge_ring(GeoData* geodata, SketchRingPtr ring0, SketchRingPtr ring1, bool delete_org_rings)
{
    if(geodata->get_parent_polygon(ring0) != geodata->get_parent_polygon(ring1))
    {
        return nullptr;
    }
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(ring0);

    std::vector<SketchPointPtr> ring0_pos;
    geodata->get_element_points(ring0, ring0_pos);
    std::vector<SketchPointPtr> ring1_pos;
    geodata->get_element_points(ring1, ring1_pos);

    std::vector<std::vector<std::vector<SketchPointPtr>>> ring_union_result;
    geodata_topo::ring_union_other_rings(geodata, ring0_pos, std::vector<std::vector<SketchPointPtr>>{ ring1_pos }, ring0->normal, ring_union_result);

    // 约定情况下必然只有一个环生成
    std::vector<std::vector<SketchPointPtr>> polygon_edge;
    polygon_edge.emplace_back(std::move(ring_union_result[0][0]));
    SketchRingPtr new_ring = geodata->_add_nous_ring(polygon_edge[0]);

    geodata->set_ring_parent_polygon(new_ring, parent_skp);
    geodata->set_ring_parent_polygon(ring0, nullptr);
    geodata->set_ring_parent_polygon(ring1, nullptr);

    if(parent_skp)
    {
        parent_skp->inner_rings.insert(new_ring);
        parent_skp->inner_rings.erase(ring0);
        parent_skp->inner_rings.erase(ring1);
        parent_skp->m_changed = true;
    }
    if(delete_org_rings)
    {
        geodata->_delete_nous_ring(ring0);
        geodata->_delete_nous_ring(ring1);
    }

    return new_ring;
}

void GeoDataTopoOperations::merge_polygon(GeoData* geodata, SketchPolygonPtr polygon0, SketchPolygonPtr polygon1, bool delete_org_poly)
{
    // 拥有相同上一级可合并
    if(geodata->get_parent_polygon(polygon0->ring) != geodata->get_parent_polygon(polygon1->ring))
    {
        return;
    }

    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon0->ring);

    std::vector<std::vector<SketchPointPtr>> polygon0_topo = GeoDataUtils::get().get_polygon_topo(geodata, polygon0);
    std::vector<std::vector<SketchPointPtr>> polygon1_topo = GeoDataUtils::get().get_polygon_topo(geodata, polygon1);

    std::vector<std::vector<std::vector<SketchPointPtr>>> result;
    geodata_topo::polygon_union(geodata, polygon0_topo, polygon1_topo, polygon0->ring->normal, result);

    // merge smooth group
    if(polygon0->group() && polygon1->group())
    {
        if(polygon0->group() != polygon1->group())
        {
            polygon0->group()->merge(polygon1->group());
        }
    }

    if(delete_org_poly)
    {
        geodata->_delete_polygon(polygon0);
        if(geodata->get_parent_polygon(polygon0->ring))
        {
            // 带有父节点的环需要手动删除
            geodata->_delete_nous_ring(polygon0->ring);
        }
        geodata->_delete_polygon(polygon1);
        if(geodata->get_parent_polygon(polygon1->ring))
        {
            // 带有父节点的环需要手动删除
            geodata->_delete_nous_ring(polygon1->ring);
        }
    }

    for(auto& polygon_topo: result)
    {
        SketchPolygonPtr new_polygon = geodata->add_polygon_without_check(polygon_topo);
        if(parent_skp)
        {
            parent_skp->inner_rings.insert(new_polygon->ring);
            geodata->set_ring_parent_polygon(new_polygon->ring, parent_skp);
        }
    }
}

void GeoDataTopoOperations::verify_sketch_polygon(GeoData* geodata, SketchPolygonPtr polygon, const bool destory_topo)
{
    if(!polygon)
    {
        return;
    }

    polygon->m_changed = true;
    SketchRingPtr ring = polygon->ring;

    std::vector<SketchPointPtr> current_ring_topo;
    geodata->get_element_points(ring, current_ring_topo);

    std::unordered_map<size_t, SketchRingPtr> inner_ring_keys;
    std::vector<std::vector<SketchPointPtr>> inner_ring_topos;
    for(auto inner: polygon->inner_rings)
    {
        std::vector<SketchPointPtr>& inner_ring_topo = inner_ring_topos.emplace_back();
        geodata->get_element_points(inner, inner_ring_topo);
        inner_ring_keys[GeoData::ring_key(inner_ring_topo)] = inner;
    }

    if(!inner_ring_topos.empty())
    {
        std::vector<std::vector<std::vector<SketchPointPtr>>> valid_polygon_topos;

        // 外环与全部内环进行 difference2D 计算，得到修复后拓扑
        // valid_polygon_topos 用了一个 Multipolygon 结构，依次处理里面的每个 Polygon
        geodata_topo::ring_difference_other_rings(geodata, current_ring_topo, inner_ring_topos, polygon->ring->normal, valid_polygon_topos);

        // 如果 Difference 的计算没有结果，说明有内环完全包含了这个 polygon 的外环，所以删除这个 polygon
        if(valid_polygon_topos.empty())
        {
            geodata->_delete_polygon(polygon);
            if(geodata->get_parent_polygon(ring))
            {
                geodata->_delete_nous_ring(ring);
            }
        }
        else
        {
            // 如果 difference 的边界一致，说明无需额外处理
            if(GeoData::ring_key(valid_polygon_topos[0][0]) == polygon->ring->key)
            {
                return;
            }

            // 否则需要将环进行替换
            std::vector<std::vector<SketchPointPtr>> all_outer_ring;
            for(auto& polygon_topo: valid_polygon_topos)
            {
                all_outer_ring.emplace_back(std::move(polygon_topo[0]));
            }

            std::vector<SketchRingPtr> resultRings;
            replace_polygon_equal_or_smaller(geodata, geodata->get_polygon(ring), all_outer_ring, resultRings);
        }
    }
}

SketchPolygonPtr GeoDataTopoOperations::rebuild_sketch_polygon(GeoData* geodata, SketchRingPtr ring)
{
    SketchPolygonPtr new_skp = std::make_shared<SketchPolygon>();
    new_skp->ring = ring;
    new_skp->normal = ring->normal;

    SketchRingPtr contained = nullptr;
    std::unordered_set<SketchRingPtr> contains;
    GeoDataUtils::get().get_ring_relation_rings(geodata, ring, contained, contains);

    geodata->set_ring_polygon(ring, new_skp);
    for(auto& inner_ring: contains)
    {
        SketchRingPtr inner_ring_contained = nullptr;
        std::unordered_set<SketchRingPtr> inner_ring_contains;
        GeoDataUtils::get().get_ring_relation_rings(geodata, inner_ring, inner_ring_contained, inner_ring_contains);

        // inner_ring_contained 不等于当前 ring 说明这个 inner_ring 被其余 inner_ring 包含，跳过
        if(inner_ring_contained != new_skp->ring)
        {
            continue;
        }

        SketchPolygonPtr parent_polygon = geodata->get_polygon(contained);
        if(contained && parent_polygon)
        {
            parent_polygon->inner_rings.erase(inner_ring);
        }

        geodata->set_ring_parent_polygon(inner_ring, new_skp);
        new_skp->inner_rings.insert(inner_ring);
    }

    return new_skp;
}

void GeoDataTopoOperations::replace_polygon_equal_or_smaller(GeoData* geodata, SketchPolygonPtr polygon,
                                                             const std::vector<std::vector<SketchPointPtr>>& replace_topos,
                                                             std::vector<SketchRingPtr>& resultRings)
{
    if(!replace_topos.size() || !polygon)
    {
        return;
    }

    // 由于这个 case 是将一个 sketch polygon 拆分成多个 sketch polygon
    // 所以涉及到环与环间的相互关系
    // 基本逻辑是，使用 difference 计算完毕后的几个 ring 作为外环，依次查询原 polygon 中的内环是否被其包含
    // 若包含，则将这些内环归属于 ring
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon->ring);
    for(auto& ex_ring: replace_topos)
    {
        SketchPolygonPtr new_skp = geodata->_create_polygon(std::vector<std::vector<SketchPointPtr>>{ ex_ring });

        // 记录新生成的 ring 对象，返回进行 verify
        resultRings.emplace_back(new_skp->ring);

        new_skp->ring->normal = polygon->ring->normal;
        std::unordered_set<SketchRingPtr> processed_ring;

        for(auto inner: polygon->inner_rings)
        {
            std::vector<SketchPointPtr> inner_ring_pts;
            geodata->get_element_points(inner, inner_ring_pts);
            if(GeoDataUtils::get().is_ring_contain_target(geodata, polygon->ring->normal, ex_ring, inner_ring_pts))
            {
                new_skp->inner_rings.insert(inner);
                processed_ring.insert(inner);
                geodata->set_ring_parent_polygon(inner, new_skp);
            }
        }

        // 将被获取的环从原 polygon 中移除
        for(const auto pr: processed_ring)
        {
            polygon->inner_rings.erase(pr);
        }

        // 更改外挂的 ring polygon 拓扑关系
        // 若当前 polygon 不是最外层，即 polygon 的 ring 是别的 polygon 的内环
        // 则需要更新，包含 polygon 的外层 polygon 需要包含新的 polygon
        geodata->set_ring_polygon(new_skp->ring, new_skp);
        geodata->set_ring_parent_polygon(new_skp->ring, parent_skp);
        if(parent_skp)
        {
            parent_skp->inner_rings.insert(new_skp->ring);
            parent_skp->m_changed = true;
        }
    }

    // 残留子环
    for(auto inner_ring: polygon->inner_rings)
    {
        geodata->set_ring_parent_polygon(inner_ring, parent_skp);
        if(parent_skp)
        {
            parent_skp->inner_rings.insert(inner_ring);
        }
    }

    // 置空父关系
    if(parent_skp)
    {
        parent_skp->inner_rings.erase(polygon->ring);
        geodata->set_ring_parent_polygon(polygon->ring, nullptr);
    }

    // 删除原 polygon
    geodata->_delete_polygon(polygon);
}

void GeoDataTopoOperations::replace_polygon(GeoData* geodata, SketchPolygonPtr polygon,
                                            const std::vector<std::vector<std::vector<SketchPointPtr>>>& replace_topos)
{
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon->ring);
    geodata->_delete_polygon(polygon);
    if(parent_skp)
    {
        // 带有父节点的环需要手动删除
        geodata->_delete_nous_ring(polygon->ring);
    }

    std::unordered_set<size_t> inner_rings_key;
    std::unordered_map<size_t, SketchPolygonPtr> new_polygons;
    for(auto& polygon_topo: replace_topos)
    {
        if(polygon_topo.size() > 1)
        {
            for(int i = 1; i < polygon_topo.size(); ++i)
            {
                inner_rings_key.insert(GeoData::ring_key(polygon_topo[i]));
            }
        }

        SketchPolygonPtr new_polygon = geodata->_create_polygon(polygon_topo);
        new_polygon->ring->normal = polygon->ring->normal;
        new_polygons[GeoData::ring_key(polygon_topo[0])] = new_polygon;
    }

    // 处理最高层级多边形的父节点
    for(auto pos: new_polygons)
    {
        if(inner_rings_key.find(pos.first) == inner_rings_key.end())
        {
            geodata->set_ring_parent_polygon(pos.second->ring, parent_skp);
            if(parent_skp)
            {
                parent_skp->inner_rings.insert(pos.second->ring);
            }
        }
    }
}

bool GeoDataTopoOperations::check_path_and_split_sketch_polygon(GeoData* geodata, SketchLinePtr line, const bool gen_polygon)
{
    // geodata->_sync_nous();
    std::unordered_map<SketchRingPtr, std::pair<nous::vec3, std::vector<std::vector<SketchPointPtr>>>> splitOuterRing;
    nous::vec3 p0pos = geodata->get_point_position(line->p0);
    nous::vec3 p1pos = geodata->get_point_position(line->p1);
    for(auto& polygonPair: geodata->m_ring_adj_polygons)
    {
        SketchRingPtr target_ring = polygonPair.first;

        if(geodata->get_ring_adj_lines(target_ring).count(line))
        {
            continue;
        }

        nous::vec3 tNormal = target_ring->normal;
        nous::vec3 primPointPos = geodata->get_point_position(geodata->m_nousptr_points.at(target_ring->ring_prim->vertex_ptr()->point_ptr()));
        nous::vec3 uDir = (p1pos - p0pos).normalized();

        float dp = tNormal.dot(uDir);
        if(std::abs(dp) < sketch_math::sketch_epsilon)
        {
            std::vector<nous::pos_t> target_ring_pos_arr;
            GeoDataUtils::get().get_ring_positions(geodata, target_ring, target_ring_pos_arr);
            if(topo::calc_point_and_ring_relation(tNormal, p0pos + (p1pos - p0pos) * 0.5, target_ring_pos_arr) == topo::ESketchTopoRelation::Outside)
            {
                continue;
            }
            std::unordered_set<SketchPointPtr> visited;
            std::vector<SketchPointPtr> path0;
            std::vector<SketchPointPtr> path1;
            std::vector<SketchPointPtr> result0;
            std::vector<SketchPointPtr> result1;

            visited.insert(line->p1);
            geodata->_find_split_ring_path(line->p0, target_ring, visited, tNormal, path0, result0);
            visited.insert(line->p0);
            visited.erase(line->p1);
            geodata->_find_split_ring_path(line->p1, target_ring, visited, tNormal, path1, result1);

            if(!result0.empty() && !result1.empty())
            {
                std::vector<SketchPointPtr> splitPath;
                if(result0.back() == result1.back())
                {
                    continue;
                }
                splitPath.insert(splitPath.end(), result1.rbegin(), result1.rend());
                splitPath.insert(splitPath.end(), result0.begin(), result0.end());
                path0.clear();
                path1.clear();

                nous::Vertex* beginVertex = target_ring->ring_prim->vertex_ptr();
                nous::Vertex* endVertex = beginVertex;
                bool switchPath = true;
                bool throughPath = false;
                while(true)
                {
                    SketchPointPtr pp = geodata->m_nousptr_points.at(beginVertex->pre_vertex_ptr()->point_ptr());
                    SketchPointPtr cp = geodata->m_nousptr_points.at(beginVertex->point_ptr());
                    SketchPointPtr np = geodata->m_nousptr_points.at(beginVertex->next_vertex_ptr()->point_ptr());

                    if(cp == splitPath.front())
                    {
                        if(!throughPath)
                        {
                            path0.insert(path0.end(), splitPath.begin(), splitPath.end());
                            path1.insert(path1.end(), splitPath.rbegin(), splitPath.rend());
                            throughPath = true;
                        }
                        switchPath = !switchPath;
                    }
                    else if(cp == splitPath.back())
                    {
                        if(!throughPath)
                        {
                            path0.insert(path0.end(), splitPath.rbegin(), splitPath.rend());
                            path1.insert(path1.end(), splitPath.begin(), splitPath.end());
                            throughPath = true;
                        }
                        switchPath = !switchPath;
                    }
                    else
                    {
                        if(switchPath)
                        {
                            path0.push_back(cp);
                        }
                        else
                        {
                            path1.push_back(cp);
                        }
                    }

                    beginVertex = beginVertex->next_vertex_ptr();
                    if(beginVertex == endVertex)
                    {
                        break;
                    }
                }

                if(path0.size() > 2 && path1.size() > 2)
                {
                    std::vector<std::vector<SketchPointPtr>> splitRing;
                    splitRing.emplace_back(std::move(path0));
                    splitRing.emplace_back(std::move(path1));
                    splitOuterRing[target_ring] = { tNormal, splitRing };
                }
            }
        }
    }

    for(auto pair: splitOuterRing)
    {
        auto& v = pair.second.second;
        SketchPolygonPtr delete_polygon = geodata->get_polygon(pair.first);

        if(!delete_polygon)
        {
            delete_polygon = rebuild_sketch_polygon(geodata, pair.first);
        }

        std::vector<SketchRingPtr> resultRings;
        replace_polygon_equal_or_smaller(geodata, delete_polygon, v, resultRings);
        for(auto ring: resultRings)
        {
            verify_sketch_polygon(geodata, geodata->get_polygon(ring));
        }
    }

    geodata->mark_render_mesh_as_dirty();
    geodata->mark_mesh_as_dirty();
    return splitOuterRing.size();
}

void GeoDataTopoOperations::check_point_and_merge_line(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPointPtr>* limit_point_set)
{
    auto&& adjPoints = geodata->get_point_adj_points(point);
    if(adjPoints.size() == 2)
    {
        auto it = adjPoints.begin();
        SketchPointPtr pp = *it;
        ++it;
        SketchPointPtr pn = *it;

        if(limit_point_set)
        {
            const bool in_set1 = limit_point_set->find(pp) != limit_point_set->end();
            const bool in_set2 = limit_point_set->find(point) != limit_point_set->end();
            const bool in_set3 = limit_point_set->find(pn) != limit_point_set->end();

            if(in_set1 && in_set2 && in_set3)
            {
                return;
            }
        }
        merge_line(geodata, pp, point, pn);
    }
}

void GeoDataTopoOperations::merge_line(GeoData* geodata, SketchPointPtr pre, SketchPointPtr center, SketchPointPtr next)
{
    auto calculateScalePositon = [](const nous::vec3& baseDir, const nous::vec3& basePos, const nous::vec3& cpos) {
        float scale = 0.01f;
        nous::vec3 bc = cpos - basePos;
        float k = -bc.dot(baseDir) / baseDir.dot(baseDir);
        return cpos + baseDir * k * (1.0f - scale);
    };
    // geodata->_sync_nous();

    nous::vec3 prePos = geodata->get_point_position(pre);
    nous::vec3 currentPos = geodata->get_point_position(center);
    nous::vec3 cDir = (currentPos - prePos).normalized();
    nous::vec3 nextPos = geodata->get_point_position(next);
    nextPos = calculateScalePositon(cDir, currentPos, nextPos);
    nous::vec3 nDir = (nextPos - currentPos).normalized();

    float dp = cDir.dot(nDir);
    if(std::abs(dp - 1.0f) < sketch_math::sketch_epsilon)
    {
        SketchLinePtr line0 = geodata->m_key_lines.at(GeoData::line_key(pre, center));
        SketchLinePtr line1 = geodata->m_key_lines.at(GeoData::line_key(center, next));
        if(line0->group() != line1->group())
        {
            return;
        }
        auto& adjRings = geodata->get_line_adj_rings(line0);
        std::map<SketchRingPtr, std::vector<SketchPointPtr>> newTopo;
        for(auto ring: adjRings)
        {
            for(auto prim_vert: geodata->get_mesh()->prim_vertices_range(ring->ring_prim->idx()))
            {
                SketchPointPtr cp = geodata->m_nousptr_points.at(geodata->get_mesh()->vertex(prim_vert)->point_ptr());
                if(cp != center)
                {
                    newTopo[ring].push_back(cp);
                }
            }
        }

        for(auto newRing: newTopo)
        {
            retopo_ring(geodata, newRing.first, newRing.second);
        }

        SketchLinePtr new_line = geodata->_add_nous_line(pre, next);
        if(line0->group())
        {
            ElementGroupPtr group = line0->group();
            group->erase_element(line0);
            group->erase_element(line1);
            group->add_element(new_line);
        }
        geodata->_delete_nous_line(line0);
        geodata->_delete_nous_line(line1);
    }
}

void GeoDataTopoOperations::merge_coplane_ring_and_polygon(GeoData* geodata, std::vector<std::pair<SketchRingPtr, SketchRingPtr>>& coplane_rings,
                                                           bool merge_adj_polygon)
{
    bool ring_deleted = false;
    for(auto coplane_rings: coplane_rings)
    {
        SketchRingPtr ring0 = coplane_rings.first;
        SketchRingPtr ring1 = coplane_rings.second;
        if(!ring0)
        {
            geodata->_delete_nous_ring(ring1);
        }
        else if(!ring1)
        {
            geodata->_delete_nous_ring(ring0);
        }
        else
        {
            SketchPolygonPtr polygon0 = geodata->get_polygon(ring0);
            SketchPolygonPtr polygon1 = geodata->get_polygon(ring1);
            if(polygon0 && polygon1)
            {
                if(merge_adj_polygon)
                {
                    merge_polygon(geodata, polygon0, polygon1, true);
                }
                else
                {
                    geodata->_delete_polygon(polygon0);
                    geodata->_delete_polygon(polygon1);
                }
            }
            else
            {
                SketchPolygonPtr ring0_parent_skp = geodata->get_parent_polygon(ring0);
                SketchPolygonPtr ring1_parent_skp = geodata->get_parent_polygon(ring1);
                if(ring0_parent_skp && ring1_parent_skp && ring0_parent_skp == ring1_parent_skp)
                {
                    merge_ring(geodata, ring0, ring1, true);
                }
                // if(polygon0)
                //{
                //     geodata->_delete_nous_ring(ring1);
                //     geodata->_delete_polygon(polygon0.get());
                // }
                // if(polygon1)
                //{
                //     geodata->_delete_nous_ring(ring0);
                //     geodata->_delete_polygon(polygon1.get());
                // }
            }
        }
    }
}

void GeoDataTopoOperations::retopo_ring(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& newTopo)
{
    SketchPolygonPtr polygon = geodata->get_polygon(ring);
    SketchPolygonPtr parent_polygon = geodata->get_parent_polygon(ring);

    SketchRingPtr new_ring = geodata->_add_nous_ring(newTopo);
    geodata->set_ring_polygon(new_ring, polygon);
    geodata->set_ring_parent_polygon(new_ring, parent_polygon);

    if(polygon)
    {
        polygon->ring = new_ring;
        polygon->m_changed = true;
    }

    if(parent_polygon)
    {
        parent_polygon->inner_rings.erase(ring);
        parent_polygon->inner_rings.insert(new_ring);
    }

    geodata->set_ring_polygon(ring, nullptr);
    geodata->set_ring_parent_polygon(ring, nullptr);
    geodata->_delete_nous_ring(ring);
}

void GeoDataTopoOperations::replace_line(GeoData* geodata, SketchLinePtr line, std::vector<SketchPointPtr>& new_line_points)
{
    auto line_adj_rings = geodata->get_line_adj_rings(line);
    if(!line_adj_rings.empty())
    {
        std::unordered_map<SketchRingPtr, std::vector<SketchPointPtr>> replace_topo;
        for(auto& ring: line_adj_rings)
        {
            for(auto prim_vert: geodata->get_mesh()->prim_vertices_range(ring->ring_prim->idx()))
            {
                nous::Vertex* ver = geodata->get_mesh()->vertex(prim_vert);
                nous::Vertex* next_ver = ver->next_vertex_ptr();
                SketchPointPtr current_p = geodata->m_nousptr_points.at(ver->point_ptr());
                SketchPointPtr next_p = geodata->m_nousptr_points.at(next_ver->point_ptr());
                if(current_p == new_line_points.back() && next_p == new_line_points.front())
                {
                    std::reverse(new_line_points.begin(), new_line_points.end());
                }
                if(current_p == new_line_points.front() && next_p == new_line_points.back())
                {
                    replace_topo[ring].insert(replace_topo[ring].end(), new_line_points.begin(), new_line_points.end() - 1);
                }
                else
                {
                    replace_topo[ring].push_back(current_p);
                }
            }
        }

        // geodata->_sync_nous();
        for(auto& ring_pair: replace_topo)
        {
            retopo_ring(geodata, ring_pair.first, ring_pair.second);
        }
    }

    // group
    ElementGroupPtr group = line->group();
    if(group)
    {
        group->erase_element(line);
    }
    for(int i = 1; i < new_line_points.size(); ++i)
    {
        SketchLinePtr new_line = geodata->_add_nous_line(new_line_points[i - 1], new_line_points[i]);
        if(group)
        {
            group->add_element(new_line);
        }
    }
    geodata->_delete_nous_line(line);
}

void GeoDataTopoOperations::change_point_positon(GeoData* geodata, const std::vector<std::pair<SketchPointPtr, nous::vec3>>& new_pos)
{
    // std::unordered_set<size_t> new_lines;
    // std::unordered_set<SketchPolygonPtr> points_adj_polygons;

    for(auto& pos: new_pos)
    {
        geodata->set_point_position(pos.first, pos.second);
        // GeoDataUtils::get().get_point_adj_polygons(geodata, pos.first, points_adj_polygons);
    }

    ////spilt un plane_coplane_plane polygon
    // for(auto polygon: points_adj_polygons)
    //{
    //     std::unordered_map<nous::vec3, std::vector<std::vector<nous::vec3>>> plane_coplane_plane;
    //     for(auto& tri: polygon->triangles)
    //     {
    //         std::vector<nous::vec3> tri_data;
    //         tri_data.push_back(tri[0].second);
    //         tri_data.push_back(tri[1].second);
    //         tri_data.push_back(tri[2].second);
    //         bool find = false;
    //         nous::vec3 plane_dir = (tri_data[0] - tri_data[1]).normalized();
    //         for(auto& plane: plane_coplane_plane)
    //         {
    //             if(sketch_math::direction_vertical(plane_dir, plane.first))
    //             {
    //                 plane.second.emplace_back(std::move(tri_data));
    //                 find = true;
    //             }
    //         }
    //         if(!find)
    //         {
    //             nous::vec3 normal = sketch_math::ring_normal(tri_data);
    //             plane_coplane_plane[normal].emplace_back(std::move(tri_data));
    //         }
    //     }
    //     if(plane_coplane_plane.size() > 1)//此面破裂需要重建
    //     {
    //         std::vector<std::vector<std::vector<nous::vec3>>> split_polygons;
    //         for(auto& plane: plane_coplane_plane)
    //         {
    //             geos_util::union_all_rings(plane.second, plane.first, split_polygons);
    //         }
    //         geodata->delete_polygon(polygon.get());
    //         for(auto polygon_pos: split_polygons)
    //         {
    //             geodata->add_polygon_without_check(polygon_pos);
    //         }
    //     }
    // }

    ////split line & polygon
    // if(retopo_geo)
    //{
    // }

    // return new_lines;
}

void GeoDataTopoOperations::copy_element_without_check(GeoData* geodata, GeoData* src_geodata, const std::unordered_set<SketchElementPtr>& elements)
{
    std::unordered_map<SketchPointPtr, SketchPointPtr> point_cache;
    for(auto ele: elements)
    {
        if(ele->type() == EElementType::Polygon)
        {
            std::vector<std::vector<SketchPointPtr>> new_polygon_topo;
            std::vector<std::vector<SketchPointPtr>> polygon_topo = GeoDataUtils::get().get_polygon_topo(src_geodata, ele->to_polygon());
            for(auto& ring: polygon_topo)
            {
                std::vector<SketchPointPtr>& new_ring = new_polygon_topo.emplace_back();
                for(auto& point: ring)
                {
                    auto& pos = point_cache.find(point);
                    if(pos == point_cache.end())
                    {
                        point_cache[point] = geodata->add_point_without_check(src_geodata->get_point_position(point));
                    }
                    new_ring.push_back(point_cache[point]);
                }
            }
            geodata->add_polygon_without_check(new_polygon_topo, true);
        }
        else if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            SketchPointPtr p0 = line->p0;
            SketchPointPtr p1 = line->p1;
            auto& pos0 = point_cache.find(p0);
            if(pos0 == point_cache.end())
            {
                point_cache[p0] = geodata->add_point_without_check(src_geodata->get_point_position(p0));
            }
            auto& pos1 = point_cache.find(p1);
            if(pos1 == point_cache.end())
            {
                point_cache[p1] = geodata->add_point_without_check(src_geodata->get_point_position(p1));
            }
            geodata->add_line_without_check(point_cache[p0], point_cache[p1]);
        }
        else if(ele->type() == EElementType::NoBranchLineGroup)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            ElementGroupPtr line_group = ele->to_group();
            for(auto& ele: line_group->elements())
            {
                SketchLinePtr line = ele->to_line();
                SketchPointPtr p0 = line->p0;
                SketchPointPtr p1 = line->p1;
                auto& pos0 = point_cache.find(p0);
                if(pos0 == point_cache.end())
                {
                    point_cache[p0] = geodata->add_point_without_check(src_geodata->get_point_position(p0));
                }
                auto& pos1 = point_cache.find(p1);
                if(pos1 == point_cache.end())
                {
                    point_cache[p1] = geodata->add_point_without_check(src_geodata->get_point_position(p1));
                }
                SketchLinePtr new_line = geodata->add_line_without_check(point_cache[p0], point_cache[p1]);
                new_group->add_element(new_line);
            }
            geodata->add_element_group(new_group);
        }
    }
}

std::vector<std::vector<SketchPointPtr>> GeoDataTopoOperations::add_polygon_without_split_line(GeoData* geodata,
                                                                                               std::vector<std::vector<nous::pos_t>>& polygon_pos,
                                                                                               bool* is_valid)
{
    nous::vec3 polygon_normal = sketch_math::ring_normal(polygon_pos.front());
    std::vector<std::vector<SketchPointPtr>> polygon_pt_arr;
    for(const auto& ring: polygon_pos)
    {
        std::vector<SketchPointPtr>& ring_pt_arr = polygon_pt_arr.emplace_back();
        for(int i = 0; i < ring.size(); ++i)
        {
            nous::pos_t p0 = ring[i];
            nous::pos_t p1 = ring[(i + 1) % ring.size()];

            std::vector<SketchPointPtr> newLines;
            std::map<SketchLinePtr, std::vector<SketchPointPtr>> recreatLine;
            geodata->_add_line(p0, p1, recreatLine, newLines);

            if(!newLines.empty())
            {
                // 直接只加第一个点，如果后面的点没有被使用，全删了
                std::map<size_t, SketchPointPtr, std::greater<size_t>> unused_pt_idx_set;
                for(int i = 1; i < newLines.size(); ++i)
                {
                    SketchPointPtr pt = newLines[i];
                    if(!pt->point->vertex_ptr())
                    {
                        unused_pt_idx_set.insert({ pt->point->idx(), pt });
                    }
                }

                for(auto& pair: unused_pt_idx_set)
                {
                    geodata->m_nousptr_points.erase(pair.second->point);
                    geodata->m_nous_mesh->delete_point(pair.first);
                }

                ring_pt_arr.emplace_back(newLines.front());
            }
        }

        for(int i = 0; i < ring_pt_arr.size(); ++i)
        {
            SketchPointPtr prev_point = ring_pt_arr[i];
            SketchPointPtr next_point = ring_pt_arr[(i + 1) % ring_pt_arr.size()];

            const size_t ring_line_key = GeoData::line_key(prev_point, next_point);

            if(geodata->m_key_lines.find(ring_line_key) == geodata->m_key_lines.end())
            {
                bool is_exist = false;
                geodata->_add_nous_line(prev_point, next_point);
            }
        }
    }

    // new_polygons 中包含的是新的 polygon 中的剩余部分
    for(int i = 0; i < polygon_pt_arr.size(); ++i)
    {
        // 只有外环 gen_polygon，内环不能生成 polygon
        geodata->_check_ring_affilia_and_create_polygon(polygon_pt_arr[i], i == 0, true);
    }

    return polygon_pt_arr;
}

void GeoDataTopoOperations::try_merge_smooth_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements, bool smooth_edge)
{
    std::unordered_map<int, std::pair<std::unordered_set<SketchElementPtr>, std::unordered_set<SketchElementPtr>>> smooth_groups;
    int smooth_groups_counter = 0;

    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchPolygonPtr>& adj_polygons) {
        int affiliation_group_id_0 = -1;
        int affiliation_group_id_1 = -1;
        auto it = adj_polygons.begin();
        SketchPolygonPtr first_polygon = *it;
        SketchPolygonPtr second_polygon = *(++it);

        // 判断相邻面是否已经为平滑组
        if(first_polygon->group() && first_polygon->group()->type() == EElementType::SmoothGroup && second_polygon->group() &&
           second_polygon->group()->type() == EElementType::SmoothGroup)
        {
            first_polygon->group()->add_element(line);
            if(first_polygon->group() != second_polygon->group())
            {
                // 合并组
                first_polygon->group()->merge(second_polygon->group());
                // geodata->delete_element_group(second_polygon->group());
            }
        }
        else if(first_polygon->group() && first_polygon->group()->type() == EElementType::SmoothGroup)
        {
            first_polygon->group()->add_element(line);
            first_polygon->group()->add_element(second_polygon);
        }
        else if(second_polygon->group() && second_polygon->group()->type() == EElementType::SmoothGroup)
        {
            second_polygon->group()->add_element(line);
            second_polygon->group()->add_element(first_polygon);
        }
        else
        {
            // 待生成的平滑组
            for(auto group: smooth_groups)
            {
                if(group.second.first.find(first_polygon) != group.second.first.end())
                {
                    affiliation_group_id_0 = group.first;
                }
                if(group.second.first.find(second_polygon) != group.second.first.end())
                {
                    affiliation_group_id_1 = group.first;
                }
                if(affiliation_group_id_0 >= 0 && affiliation_group_id_1 >= 0)
                {
                    break;
                }
            }
            // 合并连接在一起的平滑组
            if(affiliation_group_id_0 >= 0 && affiliation_group_id_1 >= 0)
            {
                if(affiliation_group_id_0 != affiliation_group_id_1)
                {
                    // 合并两个组为一个组
                    smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(smooth_groups[affiliation_group_id_1].first.begin(),
                                                                       smooth_groups[affiliation_group_id_1].first.end());

                    smooth_groups[affiliation_group_id_0].second.insert(line);
                    smooth_groups[affiliation_group_id_0].second.insert(smooth_groups[affiliation_group_id_1].second.begin(),
                                                                        smooth_groups[affiliation_group_id_1].second.end());

                    smooth_groups.erase(affiliation_group_id_1);
                }
                else
                {
                    // 在同一组内
                    smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                    smooth_groups[affiliation_group_id_0].second.insert(line);
                }
            }
            else if(affiliation_group_id_0 >= 0)
            {
                smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                smooth_groups[affiliation_group_id_0].second.insert(line);
            }
            else if(affiliation_group_id_1 >= 0)
            {
                smooth_groups[affiliation_group_id_1].first.insert(first_polygon);
                smooth_groups[affiliation_group_id_1].first.insert(second_polygon);
                smooth_groups[affiliation_group_id_1].second.insert(line);
            }
            else
            {
                int new_group_id = smooth_groups_counter++;
                smooth_groups[new_group_id].first.insert(first_polygon);
                smooth_groups[new_group_id].first.insert(second_polygon);
                smooth_groups[new_group_id].second.insert(line);
            }
        }
    };

    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            std::unordered_set<SketchPolygonPtr> adj_polygons = GeoDataUtils::get().get_line_adj_polygons(geodata, line);
            if(adj_polygons.size() == 2)
            {
                find_valid_group(line, adj_polygons);
            }
        }
        else if(ele->type() == EElementType::NoBranchLineGroup)
        {
            ElementGroupPtr weld_line_group = ele->to_group();
            std::unordered_set<SketchElementPtr> remain_lines;
            for(auto line: weld_line_group->elements())
            {
                SketchLinePtr line_ptr = line->to_line();
                std::unordered_set<SketchPolygonPtr> adj_polygons = GeoDataUtils::get().get_line_adj_polygons(geodata, line_ptr);
                if(adj_polygons.size() == 2)
                {
                    find_valid_group(line_ptr, adj_polygons);
                }
                else
                {
                    remain_lines.insert(line);
                }
            }
            geodata->delete_element_group(weld_line_group);
            try_merge_no_branch_line_group(geodata, remain_lines, {});
        }
    }

    for(auto smooth_group: smooth_groups)
    {
        std::unordered_set<SketchElementPtr>& polygons = smooth_group.second.first;
        std::unordered_set<SketchElementPtr>& lines = smooth_group.second.second;

        SmoothGroupPtr new_group = std::make_shared<SmoothGroup>();
        for(auto polygon: polygons)
        {
            new_group->add_element(polygon);
        }
        for(auto line: lines)
        {
            new_group->add_element(line);
        }
        new_group->set_need_check(false);
        geodata->add_element_group(new_group);
    }
}

void GeoDataTopoOperations::try_merge_no_branch_line_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements,
                                                           const std::unordered_set<SketchElementPtr>& force_separate_points)
{
    std::vector<std::unordered_set<SketchLinePtr>> valid_line_groups;
    std::unordered_set<SketchLinePtr> visited_lines;

    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchLinePtr>& group) {
        group.insert(line);

        std::stack<SketchLinePtr> line_stack;
        line_stack.push(line);
        while(line_stack.size())
        {
            SketchLinePtr current_line = line_stack.top();
            line_stack.pop();
            visited_lines.insert(current_line);

            SketchPointPtr p0 = current_line->p0;
            SketchPointPtr p1 = current_line->p1;

            if(force_separate_points.find(p0) == force_separate_points.end())
            {
                std::unordered_set<SketchLinePtr> p0_adj_lines = geodata->get_point_adj_lines(p0);
                p0_adj_lines.erase(current_line);
                std::unordered_set<SketchLinePtr> p0_valid_adj_lines;
                for(auto adj_line: p0_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end())
                    {
                        p0_valid_adj_lines.insert(adj_line);
                    }
                }
                if(p0_valid_adj_lines.size() == 1)
                {
                    SketchLinePtr valid_line = *p0_valid_adj_lines.begin();
                    if(elements.find(valid_line) != elements.end() && visited_lines.find(valid_line) == visited_lines.end())
                    {
                        group.insert(valid_line);
                        line_stack.push(valid_line);
                    }
                }
            }
            if(force_separate_points.find(p1) == force_separate_points.end())
            {
                std::unordered_set<SketchLinePtr> p1_adj_lines = geodata->get_point_adj_lines(p1);
                p1_adj_lines.erase(current_line);
                std::unordered_set<SketchLinePtr> p1_valid_adj_lines;
                for(auto adj_line: p1_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end())
                    {
                        p1_valid_adj_lines.insert(adj_line);
                    }
                }
                if(p1_valid_adj_lines.size() == 1)
                {
                    SketchLinePtr valid_line = *p1_valid_adj_lines.begin();
                    if(elements.find(valid_line) != elements.end() && visited_lines.find(valid_line) == visited_lines.end())
                    {
                        group.insert(valid_line);
                        line_stack.push(valid_line);
                    }
                }
            }
        }
    };
    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            if(visited_lines.find(line) == visited_lines.end())
            {
                std::unordered_set<SketchLinePtr> valid_group;
                find_valid_group(line, valid_group);
                valid_line_groups.push_back(std::move(valid_group));
            }
        }
    }
    for(auto& group: valid_line_groups)
    {
        if(group.size() > 1)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto line: group)
            {
                new_group->add_element(line);
            }
            new_group->set_need_check(false);
            geodata->add_element_group(new_group);
        }
    }
}

void GeoDataTopoOperations::try_merge_no_branch_line_group_2(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements)
{
    std::vector<std::unordered_set<SketchLinePtr>> valid_line_groups;
    std::unordered_set<SketchLinePtr> visited_lines;

    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchLinePtr>& group) {
        group.insert(line);

        std::stack<SketchLinePtr> line_stack;
        line_stack.push(line);
        while(line_stack.size())
        {
            SketchLinePtr current_line = line_stack.top();
            line_stack.pop();
            visited_lines.insert(current_line);

            SketchPointPtr p0 = current_line->p0;
            SketchPointPtr p1 = current_line->p1;

            std::unordered_set<SketchLinePtr> p0_adj_lines = geodata->get_point_adj_lines(p0);
            if(p0_adj_lines.size() == 2)
            {
                for(auto adj_line: p0_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end() && visited_lines.find(adj_line) == visited_lines.end())
                    {
                        group.insert(adj_line);
                        line_stack.push(adj_line);
                    }
                }
            }

            std::unordered_set<SketchLinePtr> p1_adj_lines = geodata->get_point_adj_lines(p1);
            if(p1_adj_lines.size() == 2)
            {
                for(auto adj_line: p1_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end() && visited_lines.find(adj_line) == visited_lines.end())
                    {
                        group.insert(adj_line);
                        line_stack.push(adj_line);
                    }
                }
            }
        }
    };
    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            if(visited_lines.find(line) == visited_lines.end())
            {
                std::unordered_set<SketchLinePtr> valid_group;
                find_valid_group(line, valid_group);
                valid_line_groups.push_back(std::move(valid_group));
            }
        }
    }
    for(auto& group: valid_line_groups)
    {
        if(group.size() > 1)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto line: group)
            {
                new_group->add_element(line);
            }
            new_group->set_need_check(false);
            geodata->add_element_group(new_group);
        }
    }
}

ElementGroupPtr GeoDataTopoOperations::add_lines_to_no_branch_line_group(GeoData* geodata,
                                                                         const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                                                         ElementGroupPtr group)
{
    NoBranchLineGroupPtr new_group = group ? std::dynamic_pointer_cast<NoBranchLineGroup>(group) : std::make_shared<NoBranchLineGroup>();

    for(auto& line_pos: lines)
    {
        std::vector<SketchLinePtr> current_new_lines = geodata->add_line(line_pos.first, line_pos.second, false);
        for(auto line: current_new_lines)
        {
            new_group->add_element(line);
        }
    }

    geodata->add_element_group(new_group);
    new_group->clear_force_separate_points();

    return new_group;
}

ElementGroupPtr GeoDataTopoOperations::add_lines_to_smooth_group(GeoData* geodata, const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                                                 ElementGroupPtr group)
{
    SmoothGroupPtr new_group = group ? std::dynamic_pointer_cast<SmoothGroup>(group) : std::make_shared<SmoothGroup>();

    for(auto& line_pos: lines)
    {
        std::vector<SketchLinePtr> current_new_lines = geodata->add_line(line_pos.first, line_pos.second, false);
        for(auto line: current_new_lines)
        {
            new_group->add_element(line);
        }
    }
    geodata->add_element_group(new_group);

    return new_group;
}

void GeoDataTopoOperations::check_and_recreate_no_branch_line_group(GeoData* geodata)
{
    std::vector<ElementGroupPtr> old_groups;
    for(auto group: geodata->get_element_groups())
    {
        if(group->type() == EElementType::NoBranchLineGroup && group->need_check())
        {
            NoBranchLineGroupPtr no_branch_group = std::dynamic_pointer_cast<NoBranchLineGroup>(group);
            std::unordered_set<SketchElementPtr> lines = group->elements();
            std::unordered_set<SketchElementPtr> force_separate_points = no_branch_group->force_separate_points();
            if(lines.size() > 1)
            {
                // group在此会被切断
                this->try_merge_no_branch_line_group(geodata, lines, force_separate_points);
            }
            old_groups.push_back(group);
        }
    }
    // 清理重新生成过的组
    for(auto group: old_groups)
    {
        // if(group->empty())
        {
            geodata->delete_element_group(group);
        }
    }
}

void GeoDataTopoOperations::check_and_recreate_smooth_group(GeoData* geodata)
{
    for(auto group: geodata->get_element_groups())
    {
        if(group->type() == EElementType::SmoothGroup)
        {
            SmoothGroupPtr smooth_group = std::dynamic_pointer_cast<SmoothGroup>(group);

            if(smooth_group->soft_edge().empty())
            {
                geodata->delete_element_group(group);
            }
            else if(group->need_check())
            {
                //
                std::unordered_set<SketchElementPtr> lines = smooth_group->soft_edge();
                geodata->delete_element_group(group);
                this->try_merge_smooth_group(geodata, lines, false);
            }
        }
    }
}

std::vector<std::pair<SketchLinePtr, bool>> GeoDataTopoOperations::try_order_weld_line_elements(
    const std::unordered_set<SketchElementPtr>& line_elements)
{
    std::vector<std::pair<SketchLinePtr, bool>> result;

    std::unordered_set<SketchElementPtr> remained_elements(line_elements.begin(), line_elements.end());
    SketchLinePtr first_line = (*remained_elements.begin())->to_line();
    remained_elements.erase(first_line);
    result.emplace_back(first_line, false);

    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    while(!remained_elements.empty())
    {
        SketchLinePtr picked_line = nullptr;
        for(auto& element: remained_elements)
        {
            SketchLinePtr line = element->to_line();
            if(line)
            {
                if(line->p0 == left_pt_marker)
                {
                    result.insert(result.begin(), std::make_pair(line, true));
                    left_pt_marker = line->p1;
                    picked_line = line;
                }
                else if(line->p1 == left_pt_marker)
                {
                    result.insert(result.begin(), std::make_pair(line, false));
                    left_pt_marker = line->p0;
                    picked_line = line;
                }
                else if(line->p0 == right_pt_marker)
                {
                    result.emplace_back(line, false);
                    right_pt_marker = line->p1;
                    picked_line = line;
                }
                else if(line->p1 == right_pt_marker)
                {
                    result.emplace_back(line, true);
                    right_pt_marker = line->p0;
                    picked_line = line;
                }
            }
        }

        // 防止这个集合内部有一些不是 line 的 element 引发循环无法结束
        if(!picked_line)
        {
            break;
        }

        remained_elements.erase(picked_line);
    }

    return result;
}

std::vector<std::pair<SketchLinePtr, bool>> GeoDataTopoOperations::try_order_weld_line_elements(
    const std::shared_ptr<GeoData> geodata, const std::unordered_set<SketchElementPtr>& line_elements)
{
    std::vector<std::pair<SketchLinePtr, bool>> result;

    std::unordered_set<SketchLinePtr> visited_lines;
    SketchLinePtr first_line = (*line_elements.begin())->to_line();
    result.emplace_back(first_line, false);
    visited_lines.insert(first_line);

    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    auto&& adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    while(adj_lines.size() > 1)
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines)
        {
            if(visited_lines.find(adj_line) != visited_lines.end())
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end())
            {
                next_line = adj_line;
                break;
            }
        }

        if(!next_line)
        {
            break;
        }

        if(next_line->p0 == left_pt_marker)
        {
            result.insert(result.begin(), std::make_pair(next_line, true));
            left_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == left_pt_marker)
        {
            result.insert(result.begin(), std::make_pair(next_line, false));
            left_pt_marker = next_line->p0;
        }

        adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    }

    adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    while(adj_lines.size() > 1)
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines)
        {
            if(visited_lines.find(adj_line) != visited_lines.end())
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end())
            {
                next_line = adj_line;
                break;
            }
        }

        if(!next_line)
        {
            break;
        }

        if(next_line->p0 == right_pt_marker)
        {
            result.emplace_back(next_line, false);
            right_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == right_pt_marker)
        {
            result.emplace_back(next_line, true);
            right_pt_marker = next_line->p0;
        }

        visited_lines.insert(next_line);
        adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    }

    return result;
}

std::vector<nous::pos_t> GeoDataTopoOperations::try_get_weld_line_position_array(const std::shared_ptr<GeoData> geodata,
                                                                                 const std::unordered_set<SketchElementPtr>& line_elements)
{
    std::vector<nous::pos_t> result;

    std::unordered_set<SketchLinePtr> visited_lines;
    SketchLinePtr first_line = (*line_elements.begin())->to_line();
    visited_lines.insert(first_line);

    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    nous::pos_t left_pt_pos = geodata->get_point_position(left_pt_marker);
    nous::pos_t right_pt_pos = geodata->get_point_position(right_pt_marker);
    result.emplace_back(left_pt_pos);
    result.emplace_back(right_pt_pos);

    auto&& adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    while(adj_lines.size() > 1)
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines)
        {
            if(visited_lines.find(adj_line) != visited_lines.end())
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end())
            {
                next_line = adj_line;
                break;
            }
        }

        if(!next_line)
        {
            break;
        }

        if(next_line->p0 == left_pt_marker)
        {
            left_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == left_pt_marker)
        {
            left_pt_marker = next_line->p0;
        }
        else
        {
            break;
        }
        visited_lines.insert(next_line);
        result.insert(result.begin(), geodata->get_point_position(left_pt_marker));
        adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    }

    adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    while(adj_lines.size() > 1)
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines)
        {
            if(visited_lines.find(adj_line) != visited_lines.end())
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end())
            {
                next_line = adj_line;
                break;
            }
        }

        if(!next_line)
        {
            break;
        }

        if(next_line->p0 == right_pt_marker)
        {
            right_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == right_pt_marker)
        {
            right_pt_marker = next_line->p0;
        }
        else
        {
            break;
        }
        visited_lines.insert(next_line);
        result.emplace_back(geodata->get_point_position(right_pt_marker));
        adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    }

    return result;
}

namespace convert
{

geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    if(!geodata || polygon.empty() || polygon.front().size() < 3)
    {
        return nullptr;
    }

    nous::vec3 act_normal = ref_normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    geos::geom::CoordinateSequence exring_coordinates;
    for(auto one_point: polygon.front())
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        nous::vec3 p = rotate_to_xy_plane * point_origin;
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z());
        exring_coordinates.add(cd);
        revert_map.insert({ cd, one_point });
    }
    exring_coordinates.add(exring_coordinates.front());
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates);

    std::vector<geos::geom::LinearRing*> polygon_geom_inrings;
    for(int i = 1; i < polygon.size(); ++i)
    {
        geos::geom::CoordinateSequence inring_coordinates;
        for(auto one_point: polygon[i])
        {
            nous::vec3 point_origin = geodata->get_point_position(one_point);
            nous::vec3 p = rotate_to_xy_plane * point_origin;
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z());
            inring_coordinates.add(cd);
            revert_map.insert({ cd, one_point });
        }
        inring_coordinates.add(inring_coordinates.front());
        geos::geom::LinearRing* inring = geom_factory->createLinearRing(inring_coordinates).release();
        polygon_geom_inrings.emplace_back(inring);
    }

    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(*exring.get(), polygon_geom_inrings);

    if(!geom_polygon->isValid())
    {
        // Try Use Difference
        geos::geom::Geometry* exring_polygon = geom_factory->createPolygon(*exring.get(), std::vector<geos::geom::LinearRing*>());
        for(int i = 0; i < polygon_geom_inrings.size(); ++i)
        {
            geos::geom::Polygon* inring_polygon = geom_factory->createPolygon(*polygon_geom_inrings[i], std::vector<geos::geom::LinearRing*>());
            exring_polygon = exring_polygon->difference(inring_polygon).release();
        }

        return exring_polygon;
    }

    return geom_polygon;
}

geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<SketchPointPtr> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    if(!geodata || polygon.empty() || polygon.size() < 3)
    {
        return nullptr;
    }

    nous::vec3 act_normal = ref_normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    geos::geom::CoordinateSequence exring_coordinates;
    for(auto one_point: polygon)
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        nous::vec3 p = rotate_to_xy_plane * point_origin;
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z());
        exring_coordinates.add(cd);
        revert_map.insert({ cd, one_point });
    }
    exring_coordinates.add(exring_coordinates.front());
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates);

    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(std::move(exring)).release();

    if(!geom_polygon->isValid())
    {
        return nullptr;
    }

    return geom_polygon;
}

geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, SketchPolygonPtr polygon,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map,
                                                                  const nous::vec3* ref_normal)
{
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    if(!polygon || !polygon->ring || !geodata)
    {
        return nullptr;
    }

    nous::vec3 act_normal = polygon->ring->normal;
    if(ref_normal)
    {
        act_normal = *ref_normal;
    }
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    geos::geom::CoordinateSequence exring_coordinates;
    std::vector<SketchPointPtr> exring_points_arr;
    geodata->get_element_points(polygon->ring, exring_points_arr);
    for(auto one_point: exring_points_arr)
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        nous::vec3 p = rotate_to_xy_plane * point_origin;
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z());
        exring_coordinates.add(cd);
        revert_map.insert({ cd, one_point });
    }
    exring_coordinates.add(exring_coordinates.front());
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates);

    std::vector<geos::geom::LinearRing*> polygon_geom_inrings;
    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr> inring_points_arr;
        geodata->get_element_points(inner_ring, inring_points_arr);
        geos::geom::CoordinateSequence inring_coordinates;
        for(auto one_point: inring_points_arr)
        {
            nous::vec3 point_origin = geodata->get_point_position(one_point);
            nous::vec3 p = rotate_to_xy_plane * point_origin;
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z());
            inring_coordinates.add(cd);
            revert_map.insert({ cd, one_point });
        }
        inring_coordinates.add(inring_coordinates.front());
        geos::geom::LinearRing* inring = geom_factory->createLinearRing(inring_coordinates).release();
        polygon_geom_inrings.emplace_back(inring);
    }
    // geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(*exring.get(), polygon_geom_inrings);

    // 确保内环合并
    std::vector<std::unique_ptr<geos::geom::Geometry>> union_polygons;
    for(auto ring: polygon_geom_inrings)
    {
        auto polygon = geom_factory->createPolygon(std::unique_ptr<geos::geom::LinearRing>(ring));
        union_polygons.emplace_back(std::move(polygon));
    }
    auto result = geom_factory->createGeometryCollection(std::move(union_polygons))->Union();
    geos::geom::GeometryTypeId type_id = result->getGeometryTypeId();

    std::vector<std::unique_ptr<geos::geom::Geometry>> result_geoms;
    if(const auto* result_collection = dynamic_cast<const geos::geom::GeometryCollection*>(result.get()))
    {
        for(std::size_t i = 0; i < result_collection->getNumGeometries(); ++i)
        {
            result_geoms.emplace_back(std::unique_ptr<geos::geom::Geometry>(result_collection->getGeometryN(i)->clone()));
        }
    }
    else
    {
        result_geoms.emplace_back(std::move(result));
    }
    //
    std::vector<geos::geom::LinearRing*> holes;
    for(const auto& geom: result_geoms)
    {
        if(!geom)
            continue;

        // 如果是Polygon，提取外环和孔洞
        if(const auto* polygon = dynamic_cast<const geos::geom::Polygon*>(geom.get()))
        {
            // 提取外环
            const geos::geom::LinearRing* exterior = dynamic_cast<const geos::geom::LinearRing*>(polygon->getExteriorRing());
            if(exterior)
            {
                holes.push_back(const_cast<geos::geom::LinearRing*>(exterior));
            }

            // 提取孔洞
            for(std::size_t i = 0; i < polygon->getNumInteriorRing(); ++i)
            {
                const geos::geom::LinearRing* interior = dynamic_cast<const geos::geom::LinearRing*>(polygon->getInteriorRingN(i));
                if(interior)
                {
                    holes.push_back(const_cast<geos::geom::LinearRing*>(interior));
                }
            }
        }
        // 如果是LinearRing，直接添加
        else if(const auto* ring = dynamic_cast<const geos::geom::LinearRing*>(geom.get()))
        {
            holes.push_back(const_cast<geos::geom::LinearRing*>(ring));
        }
    }
    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(*exring.get(), holes);

    return geom_polygon;
}

std::vector<std::vector<SketchPointPtr>> convert_geometry_to_polygon_with_revert_map(geos::geom::Polygon* polygon,
                                                                                     std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    std::vector<std::vector<SketchPointPtr>> child_skpg_rings;
    if(!polygon)
    {
        return child_skpg_rings;
    }

    const geos::geom::LinearRing* child_exring = polygon->getExteriorRing();
    int exring_pt_number = child_exring->getNumPoints();
    if(exring_pt_number < 4)
    {
        return child_skpg_rings;
    }
    auto& exring_pt_arr = child_skpg_rings.emplace_back();
    for(int i = 0; i < exring_pt_number - 1; ++i)
    {
        geos::geom::Coordinate coord = child_exring->getCoordinateN(i);
        if(revert_map.find(coord) != revert_map.end())
        {
            exring_pt_arr.emplace_back(revert_map.at(coord));
        }
        else
        {
            for(const auto& revert_pair: revert_map)
            {
                if(coord.distance(revert_pair.first) < 0.0001)
                {
                    exring_pt_arr.emplace_back(revert_pair.second);
                    break;
                }
            }
        }
    }

    for(int i = 0; i < polygon->getNumInteriorRing(); ++i)
    {
        const geos::geom::LinearRing* child_inring = polygon->getInteriorRingN(i);
        auto& inring_pt_arr = child_skpg_rings.emplace_back();
        for(int j = 0; j < child_inring->getNumPoints() - 1; ++j)
        {
            geos::geom::Coordinate coord = child_inring->getCoordinateN(j);
            if(revert_map.find(coord) != revert_map.end())
            {
                inring_pt_arr.emplace_back(revert_map.at(coord));
            }
        }
    }

    return child_skpg_rings;
}
};// namespace convert

namespace geodata_topo
{
bool polygon_union(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon0, const std::vector<std::vector<SketchPointPtr>> polygon1,
                   const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map;
    geos::geom::Geometry* ring0_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon0, normal, revert_map);
    geos::geom::Geometry* ring1_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon1, normal, revert_map);

    auto result_geo = ring0_geometry->Union(ring1_geometry);
    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(result_geo.get());
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map);
        if(result_part.size())
            result.emplace_back(std::move(result_part));
    }

    return true;
}

bool is_polygon_valid(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::vector<std::vector<nous::pos_t>> polygon_pos_arr;
    GeoDataUtils::get().get_polygon_positions(geodata, polygon, polygon_pos_arr);
    return geos_util::is_polygon_valid(polygon_pos_arr);
}

/*
 * @brief 计算一个 ring 与其余多个 ring 的不重合部分
 */
bool ring_difference_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                                 const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    if(!geodata)
    {
        return false;
    }

    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map;
    std::vector<std::vector<SketchPointPtr>> ring_as_polygon;
    ring_as_polygon.emplace_back(ring);
    geos::geom::Geometry* ring_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, ring_as_polygon, normal, revert_map);

    if(!ring_geometry)
    {
        return false;
    }

    geos::geom::Geometry* difference_geometry = ring_geometry;
    for(int i = 0; i < others.size(); ++i)
    {
        std::vector<std::vector<SketchPointPtr>> other_ring_as_polygon;
        other_ring_as_polygon.emplace_back(others[i]);
        geos::geom::Geometry* other_ring_geometry =
            convert::convert_polygon_to_geometry_with_revert_map(geodata, other_ring_as_polygon, normal, revert_map);

        if(other_ring_geometry)
        {
            if(other_ring_geometry->isValid())
            {
                difference_geometry = difference_geometry->difference(other_ring_geometry).release();
            }
            else
            {
                result.clear();
                return false;
            }
        }
    }

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(difference_geometry);
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map);
        if(result_part.size())
            result.emplace_back(std::move(result_part));
    }

    return true;
}

/*
 * @brief 计算一个 ring 与其余多个 ring 的合并部分
 */
bool ring_union_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                            const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    if(!geodata)
    {
        return false;
    }

    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map;
    std::vector<std::vector<SketchPointPtr>> ring_as_polygon;
    ring_as_polygon.emplace_back(ring);
    geos::geom::Geometry* ring_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, ring_as_polygon, normal, revert_map);

    if(!ring_geometry)
    {
        return false;
    }

    geos::geom::Geometry* union_geometry = ring_geometry;
    for(int i = 0; i < others.size(); ++i)
    {
        std::vector<std::vector<SketchPointPtr>> other_ring_as_polygon;
        other_ring_as_polygon.emplace_back(others[i]);
        geos::geom::Geometry* other_ring_geometry =
            convert::convert_polygon_to_geometry_with_revert_map(geodata, other_ring_as_polygon, normal, revert_map);

        if(other_ring_geometry)
        {
            union_geometry = union_geometry->Union(other_ring_geometry).release();
        }
    }

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(union_geometry);
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map);
        if(result_part.size())
            result.emplace_back(std::move(result_part));
    }

    return true;
}

topo::ESketchTopoRelation point_ring_relation(GeoData* geodata, const nous::pos_t& point, const std::vector<SketchPointPtr>& ring,
                                              const nous::vec3& normal)
{
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0, 0, 1));
    std::vector<geos::geom::LinearRing*> holes_0;

    geos::geom::CoordinateSequence ring_coord_seq;
    for(auto np: ring)
    {
        nous::vec3 point_origin = geodata->get_point_position(np);
        nous::vec3 p = rotate_to_xy_plane * point_origin;
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y());
        ring_coord_seq.add(cd);
    }
    ring_coord_seq.add(ring_coord_seq.front());

    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);
    std::unique_ptr<geos::geom::LinearRing> orgRing = geom_factory->createLinearRing(std::move(ring_coord_seq));
    geos::geom::Geometry* orgPolygon = geom_factory->createPolygon(*orgRing.get(), holes_0);

    nous::vec3 planePos = rotate_to_xy_plane * point;
    geos::geom::Coordinate npcd = geos::geom::Coordinate(planePos.x(), planePos.y());
    std::unique_ptr<geos::geom::Point> testPoint = geom_factory->createPoint(npcd);

    if(orgPolygon->contains(testPoint.get()))
    {
        if(orgPolygon->touches(testPoint.get()))
        {
            return topo::ESketchTopoRelation::OnBoundary;
        }

        return topo::ESketchTopoRelation::Inside;
    }

    return topo::ESketchTopoRelation::Outside;
}

bool split_intersect_polygons(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& input_polygon, const nous::vec3 input_pg_normal,
                              const std::unordered_set<SketchPolygonPtr>& coplane_polygons,
                              std::map<SketchPolygonPtr, std::vector<std::vector<std::vector<SketchPointPtr>>>>& split_pieces,
                              std::vector<std::vector<std::vector<SketchPointPtr>>>& remaining_pieces)
{
    // 存储临时生成用于拓扑运算的 Geometry 和原始 ring 对应的 skecthpolygon 之间的关系
    std::unordered_map<geos::geom::Geometry*, SketchPolygonPtr> geometry_to_prim_map;
    std::map<geos::geom::Coordinate, SketchPointPtr> ctpmap;

    // 这段代码将准备好输入的 skpolygon 和与其共面的所有 skpolygon 的 geometry
    geos::geom::Geometry* input_geom_polygon = convert::convert_polygon_to_geometry_with_revert_map(geodata, input_polygon, input_pg_normal, ctpmap);
    if(!input_geom_polygon || !input_geom_polygon->isValid())
    {
        return false;
    }

    for(const auto& polygon: coplane_polygons)
    {
        geos::geom::Geometry* geom_polygon = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon, ctpmap, &input_pg_normal);
        if(geom_polygon->isValid())
        {
            geometry_to_prim_map.insert({ geom_polygon, polygon });
        }
        else
        {
        }
    }

    // 将 input_polygon 的 geometry 对象和与其共面的所有 geometry 对象执行拓扑计算
    for(auto geom_prim_pair: geometry_to_prim_map)
    {
        geos::geom::Geometry* other_polygon = geom_prim_pair.first;

        //// 如果这个 polygon 被某个其余 polygon 完全包含，无需处理，在 addring 时会找到对应的 parent
        // if(geos_util::is_contain(other_polygon, input_geom_polygon))
        //{
        //     continue;
        // }
        //
        //// 如果这个 polygon 完全包含了其余的某个 polygon，无需处理，在其剩余部分 addring 时会找到对应的 child
        // if(geos_util::is_contain(input_geom_polygon, other_polygon))
        //{
        //     continue;
        // }

        // 若两个多边形完全不存在包含和被包含关系，则先简单判断两个多边形是否存在相交关系，若不存在相交关系，则无需进一步处理
        if(!input_geom_polygon->intersects(other_polygon))
        {
            continue;
        }

        // 这个函数会返回所有的 intersection 计算出的 polygon 的集合，将这些 polygon 转换成环，然后记录在 other_polygon 对应的外环中
        // 说明这个外环对应的 polygon 已经被切分出的一定的部分，后续需要 difference 运算进行拆分操作
        std::vector<geos::geom::Geometry*> intersection_result = geos_util::intersect_polygon(input_geom_polygon, other_polygon);

        // 为安全起见，intersects 的判断很可能包含一些【线重合】和【点重合】的情况，在 intersect_polygon 方法中会丢弃掉这些线和点
        // 所以 intersection_result 可能为空，为避免无效操作，此处需要多个判断
        if(!intersection_result.empty())
        {
            split_pieces.insert({ geom_prim_pair.second, std::vector<std::vector<std::vector<SketchPointPtr>>>() });

            // 对于每个被切分出的 polygon，将其转为 Point* 组成的 ring 对象
            // 与此同时，将这个部分与 input_geom_polygon 和 other_polygon 分别计算 difference
            for(auto child_geometry: intersection_result)
            {
                // 会内存泄露
                other_polygon = other_polygon->difference(child_geometry).release();
                input_geom_polygon = input_geom_polygon->difference(child_geometry).release();

                // 从 childpolygon 中的 Coordinate 反查 Point*
                geos::geom::Polygon* child_polygon = static_cast<geos::geom::Polygon*>(child_geometry);

                auto child_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(child_polygon, ctpmap);
                if(child_pg_pt_arr.size())
                    split_pieces[geom_prim_pair.second].emplace_back(std::move(child_pg_pt_arr));
            }

            // 全部裁切完毕后，other_polygon 的剩余部分也需要重新划分成不同的 polygon 加入返回的字典中
            std::vector<geos::geom::Geometry*> splited_polygon_parts = geos_util::extract_valid_polygons(other_polygon);
            for(auto splited_geometry: splited_polygon_parts)
            {
                geos::geom::Polygon* splited_polygon = static_cast<geos::geom::Polygon*>(splited_geometry);

                auto splited_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(splited_polygon, ctpmap);
                if(splited_pg_pt_arr.size())
                    split_pieces[geom_prim_pair.second].emplace_back(std::move(splited_pg_pt_arr));
            }
        }
    }

    // 此时 inputpolygon 已经和与其共面的 polygon 全部 intersection 解算完毕，剩余的部分作为单独的环加入其中
    std::vector<geos::geom::Geometry*> remained_geometries = geos_util::extract_valid_polygons(input_geom_polygon);
    for(auto remain_geometry: remained_geometries)
    {
        geos::geom::Polygon* remain_polygon = static_cast<geos::geom::Polygon*>(remain_geometry);

        auto remain_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(remain_polygon, ctpmap);
        if(remain_pg_pt_arr.size())
            remaining_pieces.emplace_back(std::move(remain_pg_pt_arr));
    }

    return true;
}

std::vector<std::vector<nous::vec3>> triangulate_polygon_with_intersect(std::vector<std::vector<nous::vec3>>& polygon)
{
    std::vector<std::vector<nous::vec3>> result;
    nous::vec3 normal = sketch_math::ring_normal(polygon.front());
    geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create();

    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float plane_z = 0.0f;

    std::map<geos::geom::Coordinate, nous::vec3> geopos_pos_map;
    std::vector<geos::geom::LinearRing*> holes_0;
    geos::geom::CoordinateSequence out_rs;
    for(auto pos: polygon.front())
    {
        nous::vec3 p = rotate_to_xy_plane * pos;
        plane_z = p.z();
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y());
        out_rs.add(cd);
        geopos_pos_map[cd] = pos;
    }
    out_rs.add(out_rs.front());
    std::unique_ptr<geos::geom::LinearRing> outerRing = geos_factory->createLinearRing(std::move(out_rs));
    geos::geom::Geometry* geosOuterPolygon = geos_factory->createPolygon(*outerRing.get(), holes_0);

    // std::vector<std::unique_ptr<geos::geom::LinearRing>> innerRings;
    // std::unique_ptr<geos::geom::Geometry> unioned_ring(m_geosFactory->createLinearRing());
    geos::geom::Geometry* inner_mergePolygon = geos_factory->createPolygon().release();
    for(int i = 1; i < polygon.size(); ++i)
    {
        geos::geom::CoordinateSequence in_rs;
        for(const auto& pos: polygon[i])
        {
            nous::vec3 p = rotate_to_xy_plane * pos;
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y());
            in_rs.add(cd);
            geopos_pos_map[cd] = pos;
        }
        in_rs.add(in_rs.front());

        std::unique_ptr<geos::geom::LinearRing> exRing = geos_factory->createLinearRing(std::move(in_rs));
        // innerRings.push_back(std::move(exRing));
        geos::geom::Geometry* geosInnerPolygon = geos_factory->createPolygon(*exRing.get(), holes_0);
        inner_mergePolygon = inner_mergePolygon->Union(geosInnerPolygon).release();
    }

    geos::operation::polygonize::Polygonizer polygonizerOut;
    polygonizerOut.add(geosOuterPolygon);
    std::unique_ptr<geos::geom::GeometryCollection> outPolygons = geos_factory->createGeometryCollection(polygonizerOut.getPolygons());
    std::unique_ptr<geos::geom::Geometry> mergedOutPolys = outPolygons->Union();

    geos::operation::polygonize::Polygonizer polygonizerIn;
    polygonizerIn.add(inner_mergePolygon);
    std::unique_ptr<geos::geom::GeometryCollection> inPolygons = geos_factory->createGeometryCollection(polygonizerIn.getPolygons());
    std::unique_ptr<geos::geom::Geometry> mergedInPolys = inPolygons->Union();

    const geos::geom::Geometry* geosResult = mergedOutPolys->difference(mergedInPolys.get()).release();

    geos::geom::GeometryTypeId type_id = geosResult->getGeometryTypeId();
    std::vector<const geos::geom::Geometry*> geometrys;
    if(type_id == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION || type_id == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON)
    {
        int gnum = geosResult->getNumGeometries();
        for(int i = 0; i < gnum; ++i)
        {
            auto child_geom = geosResult->getGeometryN(i);
            if(child_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_POLYGON)
            {
                geometrys.push_back(child_geom);
            }
        }
    }
    else if(type_id == geos::geom::GeometryTypeId::GEOS_POLYGON)
    {
        geometrys.push_back(geosResult);
    }
    else
    {
    }
    for(auto geo: geometrys)
    {
        std::vector<nous::vec3> nparray;
        nous::Array<nous::Array<nous::vec2>> allPolygon;

        const geos::geom::Polygon* valid_polygon = static_cast<const geos::geom::Polygon*>(geo);

        const geos::geom::LinearRing* outerRing = valid_polygon->getExteriorRing();
        auto outsq = outerRing->getCoordinates();
        size_t outerPointNum = outsq->getSize();// outer_ring->getNumPoints();
        if(outerPointNum > 0)
        {
            nous::Array<nous::vec2>& outer = allPolygon.emplace_back();
            for(int i = 0; i < outerPointNum - 1; ++i)
            {
                // std::unique_ptr<geos::geom::Point> pt = outer_ring->getPointN(i);
                // outerPolygon.emplace_back(pt->getX(), pt->getY());
                geos::geom::Coordinate p;
                outsq->getAt(i, p);
                auto pos = geopos_pos_map.find(p);
                if(pos == geopos_pos_map.end())
                {
                    nparray.emplace_back(rotate_to_xy_plane.conjugate() * nous::vec3(p.x, p.y, plane_z));
                }
                else
                {
                    nparray.emplace_back(pos->second);
                }
                outer.emplace_back(p.x, p.y);
            }
            if(outer.size() < 3)
            {
                allPolygon.pop_back();
            }
        }

        size_t innerRingNum = valid_polygon->getNumInteriorRing();
        if(innerRingNum > 0)
        {
            for(int i = 0; i < innerRingNum; ++i)
            {
                const geos::geom::LinearRing* interRing = valid_polygon->getInteriorRingN(i);
                auto insq = interRing->getCoordinates();
                size_t interRingPointNum = insq->getSize();// interRing->getNumPoints();
                nous::Array<nous::vec2>& innerPolygon = allPolygon.emplace_back();
                for(int j = 0; j < interRingPointNum - 1; ++j)
                {
                    // std::unique_ptr<geos::geom::Point> pt = interRing->getPointN(j);
                    // innerPolygon.emplace_back(pt->getX(), pt->getY());
                    geos::geom::Coordinate p;
                    insq->getAt(j, p);
                    auto pos = geopos_pos_map.find(p);
                    if(pos == geopos_pos_map.end())
                    {
                        nparray.emplace_back(rotate_to_xy_plane.conjugate() * nous::vec3(p.x, p.y, plane_z));
                    }
                    else
                    {
                        nparray.emplace_back(pos->second);
                    }
                    innerPolygon.emplace_back(p.x, p.y);
                }
                if(innerPolygon.size() < 3)
                {
                    allPolygon.pop_back();
                }
            }
        }

        auto tris = mapbox::earcut<size_t>(allPolygon);
        for(auto tri: tris)
        {
            std::vector<nous::vec3>& nt = result.emplace_back();
            nt.emplace_back(nparray[tri.x()]);
            nt.emplace_back(nparray[tri.y()]);
            nt.emplace_back(nparray[tri.z()]);
        }
    }

    return result;
}

std::vector<std::vector<nous::vec3>> triangulate_polygon(std::vector<std::vector<nous::vec3>>& polygon, bool reverse_triangle_order)
{
    std::vector<nous::vec3> id_pos_map;
    const auto&& polygon_pt_rel_idx = topo::convert_polygon_position_to_relative_idx(polygon, id_pos_map);
    std::vector<CDT::V2d<float>> cdt_positions;

    nous::vec3 normal = sketch_math::ring_normal(polygon.front());
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float plane_z = 0.0f;

    for(int i = 0; i < id_pos_map.size(); ++i)
    {
        const nous::pos_t xoy_pos = rotate_to_xy_plane * id_pos_map[i];
        plane_z = xoy_pos.z();
        CDT::V2d<float> cdt_pos;
        cdt_pos.x = xoy_pos.x();
        cdt_pos.y = xoy_pos.y();
        cdt_positions.emplace_back(cdt_pos);
    }

    CDT::EdgeVec constrained_edges;
    for(int i = 0; i < polygon_pt_rel_idx.size(); ++i)
    {
        for(int j = 0; j < polygon_pt_rel_idx[i].size(); ++j)
        {
            const size_t prev_pt_idx = polygon_pt_rel_idx[i][j];
            const size_t next_pt_idx = polygon_pt_rel_idx[i][(j + 1) % polygon_pt_rel_idx[i].size()];
            constrained_edges.push_back(CDT::Edge(prev_pt_idx, next_pt_idx));
        }
    }

    std::vector<std::vector<nous::vec3>> result;
    try
    {
        CDT::Triangulation<float> cdt;
        cdt.insertVertices(cdt_positions);
        cdt.insertEdges(constrained_edges);

        const bool is_valid = CDT::verifyTopology(cdt);
        if(!is_valid)
        {
            // 退化给 geos 和 earcut 三角化
            return triangulate_polygon_with_intersect(polygon);
        }

        cdt.eraseOuterTrianglesAndHoles();

        for(const auto& tri: cdt.triangles)
        {
            auto& triangle = result.emplace_back();
            if(reverse_triangle_order)
            {
                triangle.emplace_back(id_pos_map[tri.vertices[0]]);
                triangle.emplace_back(id_pos_map[tri.vertices[1]]);
                triangle.emplace_back(id_pos_map[tri.vertices[2]]);
            }
            else
            {
                triangle.emplace_back(id_pos_map[tri.vertices[0]]);
                triangle.emplace_back(id_pos_map[tri.vertices[2]]);
                triangle.emplace_back(id_pos_map[tri.vertices[1]]);
            }
        }
    }
    catch(CDT::IntersectingConstraintsError ice)
    {
        // 退化给自交检测
        std::vector<std::vector<std::vector<nous::pos_t>>> splited_valid_polygon = topo::rebuild_invalid_polygon_topo(polygon);
        for(int i = 0; i < splited_valid_polygon.size(); ++i)
        {
            std::vector<std::vector<nous::vec3>> sub_polygon_triangles = triangulate_polygon(splited_valid_polygon[i]);
            result.insert(result.end(), sub_polygon_triangles.begin(), sub_polygon_triangles.end());
        }
        return result;
    }
    catch(CDT::DuplicateVertexError dve)
    {
        return triangulate_polygon_with_intersect(polygon);
    }

    return result;
}

void triangulate_polygon_cdt(GeoData* geodata, SketchPolygonPtr polygon)
{
    polygon->triangles.clear();

    if(!is_polygon_valid(geodata, polygon))
    {
        return;
    }

    std::vector<SketchPointPtr> id_point_map;
    const auto&& polygon_pt_rel_idx = GeoDataUtils::get().convert_polygon_to_rel_point_idx(geodata, polygon, id_point_map);
    std::vector<CDT::V2d<float>> cdt_positions;

    double record_z = 0.0;
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(polygon->ring->normal, nous::vec3(0.f, 0.f, 1.f));
    for(int i = 0; i < id_point_map.size(); ++i)
    {
        const nous::pos_t pt_pos = geodata->get_point_position(id_point_map[i]);
        const nous::pos_t xoy_pos = rotate_to_xy_plane * pt_pos;
        record_z = xoy_pos.z();
        CDT::V2d<float> cdt_pos;
        cdt_pos.x = xoy_pos.x();
        cdt_pos.y = xoy_pos.y();
        cdt_positions.emplace_back(cdt_pos);
    }

    CDT::EdgeVec constrained_edges;
    for(int i = 0; i < polygon_pt_rel_idx.size(); ++i)
    {
        for(int j = 0; j < polygon_pt_rel_idx[i].size(); ++j)
        {
            const size_t prev_pt_idx = polygon_pt_rel_idx[i][j];
            const size_t next_pt_idx = polygon_pt_rel_idx[i][(j + 1) % polygon_pt_rel_idx[i].size()];
            constrained_edges.push_back(CDT::Edge(prev_pt_idx, next_pt_idx));
        }
    }

    try
    {
        CDT::Triangulation<float> cdt;
        cdt.insertVertices(cdt_positions);
        cdt.insertEdges(constrained_edges);
        cdt.eraseOuterTrianglesAndHoles();

        for(const auto& tri: cdt.triangles)
        {
            auto& triangle = polygon->triangles.emplace_back();
            triangle.emplace_back(id_point_map[tri.vertices[0]], nous::vec3::Zero());
            triangle.emplace_back(id_point_map[tri.vertices[1]], nous::vec3::Zero());
            triangle.emplace_back(id_point_map[tri.vertices[2]], nous::vec3::Zero());
        }
    }
    catch(CDT::IntersectingConstraintsError ice)
    {
        // 退化给自交检测
        polygon->triangles.clear();
        return;
    }
    catch(CDT::DuplicateVertexError dve)
    {
        polygon->triangles.clear();
        return;
    }
}

};// namespace geodata_topo
};// namespace sketch