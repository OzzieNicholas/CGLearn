#include "sketch/View/RenderRes/GeoData.h"

#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetMesh.h"
#include "Engine/Math/GeoMesh/GeoMesh.h"
#include "Engine/Math/GeoUtil.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/STCore.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/ViewManager.h"

#include "sketch/NousEngine/SketchMath.h"
#include "sketch/NousEngine/SketchRTree.h"
#include <sketch/UV/AutoMappingUV.h>

using namespace Engine;

namespace sketch
{
GeoData::GeoData()
    : SketchObject(ESketchObjectType::GeoData), IRenderable(), m_dirty_flags(0xffffffff), m_nous_mesh(nous::NMesh::create()),
      m_nous_position_handle(m_nous_mesh->point_attrib<nous::pos_t>(nous::ATTRIB_NAME_POS))
{
    m_lines_tree = std::make_shared<SketchRTree<SketchLinePtr>>();
    m_ring_tree = std::make_shared<SketchRTree<SketchRingPtr>>();
}

GeoData::GeoData(const GeoData& other)
    : SketchObject(ESketchObjectType::GeoData), m_nous_mesh(nous::NMesh::create()),
      m_nous_position_handle(m_nous_mesh->point_attrib<nous::pos_t>(nous::ATTRIB_NAME_POS))
{
    m_nous_mesh->copy(other.m_nous_mesh.get());
    m_lines_tree = std::make_shared<SketchRTree<SketchLinePtr>>();
    m_ring_tree = std::make_shared<SketchRTree<SketchRingPtr>>();

    // points
    int num_points = m_nous_mesh->num_points();
    for(int i = 0; i < num_points; ++i)
    {
        nous::Point* point = m_nous_mesh->point(i);
        m_nousptr_points[point] = std::make_shared<SketchPoint>(point);
    }

    // lines
    for(const auto& pair: other.m_key_lines)
    {
        nous::Prim* line_prim = m_nous_mesh->prim(pair.second->line_prim->idx());
        size_t point0_idx = pair.second->p0->point->idx();
        size_t point1_idx = pair.second->p1->point->idx();
        nous::Point* point0 = m_nous_mesh->point(point0_idx);
        nous::Point* point1 = m_nous_mesh->point(point1_idx);

        size_t lk = line_key(m_nousptr_points.at(point0), m_nousptr_points.at(point1));
        m_key_lines[lk] = std::make_shared<SketchLine>(line_prim, lk);
        m_key_lines[lk]->p0 = m_nousptr_points.at(point0);
        m_key_lines[lk]->p1 = m_nousptr_points.at(point1);
        m_nousptr_lines[line_prim] = m_key_lines[lk];

        // rtree
        nous::AABB3 line_box = GeoDataUtils::get().get_element_aabb3(this, m_key_lines[lk]);
        m_lines_tree->insert_object(m_key_lines[lk], line_box);
    }

    // rings
    for(const auto& pair: other.m_key_rings)
    {
        nous::Prim* ring_prim = m_nous_mesh->prim(pair.second->ring_prim->idx());
        // key
        std::vector<SketchPointPtr> ring_points;
        for(auto vtx_idx: m_nous_mesh->prim_vertices_range(ring_prim->idx()))
        {
            ring_points.push_back(m_nousptr_points.at(m_nous_mesh->vertex(vtx_idx)->point_ptr()));
        }
        size_t ring_key = GeoData::ring_key(ring_points);
        SketchRingPtr ring = std::make_shared<SketchRing>(ring_prim, ring_key);
        ring->normal = pair.second->normal;
        m_key_rings[ring_key] = ring;
        m_nousptr_rings[ring_prim] = ring;

        // rtree
        nous::AABB3 ring_box = GeoDataUtils::get().get_element_aabb3(this, ring);
        m_ring_tree->insert_object(ring, ring_box);
    }

    // polygons
    for(auto& ring_relation: other.m_ring_adj_polygons)
    {
        nous::Prim* ring_prim = m_nous_mesh->prim(ring_relation.first->ring_prim->idx());
        SketchRingPtr ring = m_nousptr_rings.at(ring_prim);

        if(ring_relation.second.polygon)
        {
            auto polygon_pos = m_ring_adj_polygons.find(ring);
            if(polygon_pos == m_ring_adj_polygons.end())
            {
                SketchPolygonPtr current_polygon = std::make_shared<SketchPolygon>(ring);
                current_polygon->normal = ring_relation.second.polygon->normal;
                m_ring_adj_polygons[ring].polygon = current_polygon;
            }
            else if(!polygon_pos->second.polygon)
            {
                SketchPolygonPtr current_polygon = std::make_shared<SketchPolygon>(ring);
                current_polygon->normal = ring_relation.second.polygon->normal;
                polygon_pos->second.polygon = current_polygon;
            }
            for(auto& inner_ring: ring_relation.second.polygon->inner_rings)
            {
                nous::Prim* inner_ring_prim = m_nous_mesh->prim(inner_ring->ring_prim->idx());
                SketchRingPtr inner_ring = m_nousptr_rings.at(inner_ring_prim);
                m_ring_adj_polygons[ring].polygon->inner_rings.insert(inner_ring);
                m_ring_adj_polygons[inner_ring].parent_polygon = m_ring_adj_polygons[ring].polygon;
            }
        }
        if(ring_relation.second.parent_polygon)
        {
            nous::Prim* parent_ring_prim = m_nous_mesh->prim(ring_relation.second.parent_polygon->ring->ring_prim->idx());
            SketchRingPtr parent_ring = m_nousptr_rings.at(parent_ring_prim);
            // parent
            auto parent_pos = m_ring_adj_polygons.find(parent_ring);
            if(parent_pos == m_ring_adj_polygons.end())
            {
                SketchPolygonPtr parent_skp = std::make_shared<SketchPolygon>(parent_ring);
                parent_skp->normal = ring_relation.second.parent_polygon->normal;
                m_ring_adj_polygons[parent_ring].polygon = parent_skp;
            }
            m_ring_adj_polygons[ring].parent_polygon = m_ring_adj_polygons[parent_ring].polygon;
        }
    }

    // group
    for(auto& group: other.m_ele_groups)
    {
        ElementGroupPtr new_group = nullptr;
        if(group->type() == EElementType::NoBranchLineGroup)
        {
            new_group = std::make_shared<NoBranchLineGroup>();
            for(const auto& ele: group->to_group()->elements())
            {
                size_t index = ele->to_line()->line_prim->idx();
                SketchLinePtr line = m_nousptr_lines.at(m_nous_mesh->prim(index));
                new_group->add_element(line);
            }
            m_ele_groups.insert(new_group);
        }
        else if(group->type() == EElementType::SmoothGroup)
        {
            new_group = std::make_shared<SmoothGroup>();
            for(const auto& ele: group->to_group()->elements())
            {
                if(ele->type() == EElementType::Polygon)
                {
                    size_t index = ele->to_polygon()->ring->ring_prim->idx();
                    SketchRingPtr ring = m_nousptr_rings.at(m_nous_mesh->prim(index));

                    SketchPolygonPtr polygon = m_ring_adj_polygons.at(ring).polygon;
                    new_group->add_element(polygon);
                }
                else if(ele->type() == EElementType::Line)
                {
                    size_t index = ele->to_line()->line_prim->idx();
                    SketchLinePtr line = m_nousptr_lines.at(m_nous_mesh->prim(index));
                    new_group->add_element(line);
                }
            }
            m_ele_groups.insert(new_group);
        }
    }

    m_dirty_flags = 0xffffffff;
}

GeoData::~GeoData() {}

void GeoData::dispose()
{
    m_dirty_flags = 0xffffffff;

    this->clear();
}

std::unordered_set<SketchPolygonPtr> GeoData::get_all_polygons()
{
    std::unordered_set<SketchPolygonPtr> result;

    for(auto& ring_pg_pair: m_ring_adj_polygons)
    {
        if(ring_pg_pair.second.polygon)
        {
            result.insert(ring_pg_pair.second.polygon);
        }

        if(ring_pg_pair.second.parent_polygon)
        {
            result.insert(ring_pg_pair.second.parent_polygon);
        }
    }

    return result;
}

bool GeoData::is_line(const nous::Prim* prim) const
{
    if(prim && prim->type() == nous::PrimType::Polyline && prim->num_vertices() == 2 && prim->is_valid())
    {
        return true;
    }
    return false;
}

std::unordered_set<SketchRingPtr> GeoData::get_line_adj_rings(SketchLinePtr line)
{
    std::unordered_set<SketchRingPtr> result;

    for(const nous::index_t& vtx_idx: m_nous_mesh->point_vertices_range(line->p0->point->idx()))
    {
        nous::Vertex* point_vertex = m_nous_mesh->vertex(vtx_idx);
        if(point_vertex->next_vertex_ptr() && point_vertex->next_vertex_ptr()->point() == line->p1->point->idx())
        {
            if(!is_line(point_vertex->prim_ptr()))
            {
                result.insert(m_nousptr_rings.at(point_vertex->prim_ptr()));
            }
        }

        if(point_vertex->pre_vertex_ptr() && point_vertex->pre_vertex_ptr()->point() == line->p1->point->idx())
        {
            if(!is_line(point_vertex->prim_ptr()))
            {
                result.insert(m_nousptr_rings.at(point_vertex->prim_ptr()));
            }
        }
    }

    for(const nous::index_t& vtx_idx: m_nous_mesh->point_vertices_range(line->p1->point->idx()))
    {
        nous::Vertex* point_vertex = m_nous_mesh->vertex(vtx_idx);
        if(point_vertex->next_vertex_ptr() && point_vertex->next_vertex_ptr()->point() == line->p0->point->idx())
        {
            if(!is_line(point_vertex->prim_ptr()))
            {
                result.insert(m_nousptr_rings.at(point_vertex->prim_ptr()));
            }
        }

        if(point_vertex->pre_vertex_ptr() && point_vertex->pre_vertex_ptr()->point() == line->p0->point->idx())
        {
            if(!is_line(point_vertex->prim_ptr()))
            {
                result.insert(m_nousptr_rings.at(point_vertex->prim_ptr()));
            }
        }
    }
    return result;
}

std::unordered_set<SketchLinePtr> GeoData::get_ring_adj_lines(SketchRingPtr ring_prim)
{
    std::unordered_set<SketchLinePtr> result;

    std::vector<SketchPointPtr> topo = this->get_element_points(ring_prim);
    for(int i = 0; i < topo.size(); ++i)
    {
        size_t lk = line_key(topo[i], topo[(i + 1) % topo.size()]);
        if(m_key_lines.find(lk) != m_key_lines.end())
        {
            result.insert(m_key_lines[lk]);
        }
    }

    return result;
}

std::unordered_set<SketchPointPtr> GeoData::get_point_adj_points(SketchPointPtr point)
{
    std::unordered_set<SketchPointPtr> result;
    if(!point->point->vertex_ptr())
    {
        return result;
    }
    for(const nous::index_t& pt_idx: m_nous_mesh->point_neighbours_range(point->point->idx()))
    {
        SketchPointPtr adj_pt = m_nousptr_points.at(m_nous_mesh->point(pt_idx));
        if(adj_pt)
        {
            result.insert(adj_pt);
        }
    }

    return result;
}

SketchPointPtr GeoData::add_point(const nous::vec3& pos)
{
    for(const auto& pt_idx: m_nous_mesh->point_index_range())
    {
        const nous::pos_t exist_pt_pos = m_nous_position_handle.get(pt_idx);
        if(sketch_math::point_equal(pos, exist_pt_pos))
        {
            return m_nousptr_points.at(m_nous_mesh->point(pt_idx));
        }
    }

    return add_point_without_check(pos);
}

SketchPointPtr GeoData::add_point_without_check(const nous::vec3& pos)
{
    nous::Point* np = m_nous_mesh->point(m_nous_mesh->add_point(pos));
    SketchPointPtr point = std::make_shared<SketchPoint>(np);
    m_nousptr_points[np] = point;
    return point;
}

std::vector<SketchLinePtr> GeoData::add_line(const nous::vec3& p0, const nous::vec3& p1, bool genPolygon)
{
    std::vector<SketchLinePtr> new_lines;
    std::vector<SketchPointPtr> new_line_points;
    std::map<SketchLinePtr, std::vector<SketchPointPtr>> recreat_line;
    this->_add_line(p0, p1, recreat_line, new_line_points);

    // 旧line与其焊接组标记
    for(auto& rl: recreat_line)
    {
        GeoDataTopoOperations::get().replace_line(this, rl.first, rl.second);
    }

    // 查找标记被影响的组
    // 焊接组需要强制分割点的原因：部分与端点相交的焊接组，虽然拓扑不变，但依然需要被分割。
    for(auto point: new_line_points)
    {
        for(auto adj_line: this->get_point_adj_lines(point))
        {
            ElementGroupPtr group = adj_line->group();
            if(group)
            {
                if(group->type() == EElementType::NoBranchLineGroup)
                {
                    NoBranchLineGroupPtr no_branch_group = std::dynamic_pointer_cast<NoBranchLineGroup>(group);
                    // 强制断开点
                    no_branch_group->add_force_separate_point(point);
                }
                else if(group->type() == EElementType::SmoothGroup)
                {
                }
                group->set_need_check(true);
            }
        }
    }

    // 新line
    for(int i = 1; i < new_line_points.size(); ++i)
    {
        SketchLinePtr cl = _add_nous_line(new_line_points[i], new_line_points[i - 1]);
        new_lines.push_back(cl);
        GeoDataTopoOperations::get().check_path_and_split_sketch_polygon(this, cl);
        if(genPolygon)
        {
            auto ring_contain_target_part = [this](const nous::vec3& normal, const std::vector<SketchPointPtr>& target,
                                                   const std::vector<SketchPointPtr>& ring) {
                for(int i = 0; i < target.size(); ++i)
                {
                    SketchPointPtr p0 = target[i];
                    SketchPointPtr p1 = target[(i + 1) % target.size()];
                    nous::vec3 p0_pos = m_nous_position_handle.get(p0->point->idx());
                    nous::vec3 p1_pos = m_nous_position_handle.get(p1->point->idx());
                    nous::vec3 target_ring_pos = p0_pos + (p1_pos - p0_pos) * 0.5f;
                    std::vector<nous::pos_t> ring_positions;
                    GeoDataUtils::get().get_ring_positions(this, ring, ring_positions);
                    if(topo::calc_point_and_ring_relation(normal, target_ring_pos, ring_positions) == topo::ESketchTopoRelation::Inside)
                    {
                        return true;
                    }
                }
                return false;
            };

            this->_sync_nous();
            std::map<size_t, std::vector<SketchPointPtr>> allRings;

            nous::vec3 pos0 = m_nous_position_handle.get(cl->p0->point->idx());
            nous::vec3 pos1 = m_nous_position_handle.get(cl->p1->point->idx());
            nous::vec3 base_dir = (pos1 - pos0).normalized();

            std::vector<SketchPointPtr> path;
            std::vector<nous::vec3> normal;
            std::vector<nous::vec3> dir;
            std::unordered_set<SketchPointPtr> visited;

            path.push_back(cl->p1);
            normal.emplace_back(0.f, 0.f, 0.f);
            dir.emplace_back(base_dir);
            visited.insert(cl->p1);
            uint8_t mode = 0x3;
            std::vector<std::pair<nous::vec3, std::vector<std::vector<SketchPointPtr>>>> coplane_rings;
            _find_coplain_rings(cl->p1, cl->p0, path, visited, normal, dir, mode, coplane_rings);
            // 共面环，检测是否重叠
            for(auto& rings: coplane_rings)
            {
                if(rings.second.size() == 2)
                {
                    if(ring_contain_target_part(rings.first, rings.second[0], rings.second[1]))
                    {
                        rings.second.pop_back();
                    }
                    else if(ring_contain_target_part(rings.first, rings.second[1], rings.second[0]))
                    {
                        rings.second[0] = std::move(rings.second[1]);
                        rings.second.pop_back();
                    }
                }
            }
            for(auto& coplane_rings: coplane_rings)
            {
                for(auto& ring: coplane_rings.second)
                {
                    auto ring_pos = m_key_rings.find(GeoData::ring_key(ring));
                    if(ring_pos == m_key_rings.end())
                    {
                        // 如果环是新环，创建多边形
                        std::vector<std::vector<SketchPointPtr>> new_polygon_topo;
                        new_polygon_topo.emplace_back(std::move(ring));
                        this->add_polygon(new_polygon_topo, coplane_rings.first);
                    }
                    else
                    {
                        // 如果环已存在，检查是否需要创建多边形
                        if(!m_ring_adj_polygons[ring_pos->second].polygon)
                        {
                            std::vector<std::vector<SketchPointPtr>> new_polygon_topo;
                            new_polygon_topo.emplace_back(std::move(ring));
                            this->add_polygon(new_polygon_topo, coplane_rings.first);
                        }
                    }
                }
            }
        }
    }

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;

    return new_lines;
}

SketchLinePtr GeoData::add_line_without_check(SketchPointPtr p0, SketchPointPtr p1) { return this->_add_nous_line(p0, p1); }

void GeoData::delete_line(SketchLinePtr line, bool merge_adj_line, bool merge_adj_polygon, std::unordered_set<SketchPointPtr>* force_keeped_point_set)
{
    // ring
    std::vector<std::pair<SketchRingPtr, SketchRingPtr>> coplane_ring;
    std::unordered_set<SketchRingPtr> used_prim_set;
    std::unordered_set<SketchRingPtr> line_adj_rings = get_line_adj_rings(line);
    for(auto ring1: line_adj_rings)
    {
        if(used_prim_set.find(ring1) != used_prim_set.end())
        {
            continue;
        }

        used_prim_set.insert(ring1);
        bool find_coplane = false;

        const nous::vec3 ring1_normal = m_nous_mesh->prim_normal(ring1->ring_prim->idx());
        for(auto ring2: line_adj_rings)
        {
            if(ring1 == ring2)
            {
                continue;
            }

            const nous::vec3 ring2_normal = m_nous_mesh->prim_normal(ring2->ring_prim->idx());
            if(sketch_math::direction_parallel(ring1_normal, ring2_normal))
            {
                find_coplane = true;
                used_prim_set.insert(ring2);
                auto& pair_data = coplane_ring.emplace_back();
                pair_data.first = ring1;
                pair_data.second = ring2;
                break;
            }
        }

        if(!find_coplane)
        {
            auto& pair_data = coplane_ring.emplace_back();
            pair_data.first = ring1;
            pair_data.second = nullptr;
        }
    }
    used_prim_set.clear();

    GeoDataTopoOperations::get().merge_coplane_ring_and_polygon(this, coplane_ring, merge_adj_polygon);

    // line
    SketchPointPtr p0 = line->p0;
    SketchPointPtr p1 = line->p1;
    this->_delete_nous_line(line);

    if(merge_adj_line)
    {
        if(!force_keeped_point_set || (force_keeped_point_set && force_keeped_point_set->find(p0) == force_keeped_point_set->end()))
        {
            GeoDataTopoOperations::get().check_point_and_merge_line(this, p0, force_keeped_point_set);
        }

        if(!force_keeped_point_set || (force_keeped_point_set && force_keeped_point_set->find(p1) == force_keeped_point_set->end()))
        {
            GeoDataTopoOperations::get().check_point_and_merge_line(this, p1, force_keeped_point_set);
        }
    }

    if(!p0->point->vertex_ptr())
    {
        m_nousptr_points.erase(p0->point);
        m_nous_mesh->delete_point(p0->point->idx());
    }

    if(!p1->point->vertex_ptr())
    {
        m_nousptr_points.erase(p1->point);
        m_nous_mesh->delete_point(p1->point->idx());
    }

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
}

void GeoData::delete_lines(const std::unordered_set<SketchLinePtr>& lines, bool mergeAdjLine, bool mergeAdjRing)
{
    std::unordered_set<SketchPointPtr> points;
    for(auto line: lines)
    {
        points.insert(line->p0);
        points.insert(line->p1);
        this->delete_line(line, false, mergeAdjRing);
    }

    if(mergeAdjLine)
    {
        for(auto p: points)
        {
            GeoDataTopoOperations::get().check_point_and_merge_line(this, p);
        }
    }
}

std::vector<std::vector<SketchPointPtr>> GeoData::add_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_pos)
{
    // polygon split line，保证point/line/ring的拓扑正确，返回新的polygon拓扑。
    std::unordered_set<SketchPointPtr> recreat_line_points;
    std::vector<std::vector<SketchPointPtr>> polygon_pt_arr;
    for(const auto& ring: polygon_pos)
    {
        std::vector<SketchPointPtr>& ring_pt_arr = polygon_pt_arr.emplace_back();
        for(int i = 0; i < ring.size(); ++i)
        {
            nous::pos_t p0 = ring[i];
            nous::pos_t p1 = ring[(i + 1) % ring.size()];

            std::vector<SketchPointPtr> new_line_points;
            std::map<SketchLinePtr, std::vector<SketchPointPtr>> recreat_line;
            _add_line(p0, p1, recreat_line, new_line_points);

            // 旧line
            for(auto& rl: recreat_line)
            {
                GeoDataTopoOperations::get().replace_line(this, rl.first, rl.second);
            }
            // 查找标记被影响的组
            // 焊接组需要强制分割点的原因：部分与端点相交的焊接组，虽然拓扑不变，但依然需要被分割。
            // for(auto point: new_line_points)
            //{
            //     for(auto adj_line: this->get_point_adj_lines(point))
            //     {
            //         ElementGroupPtr group = adj_line->group();
            //         if(group && group->type() == EElementType::NoBranchLineGroup)
            //         {
            //             NoBranchLineGroupPtr no_branch_group = std::dynamic_pointer_cast<NoBranchLineGroup>(group);
            //             // 强制断开点
            //             no_branch_group->add_force_separate_point(point);
            //             no_branch_group->set_need_check(true);
            //         }
            //     }
            // }
            //  新line
            if(new_line_points.size())
            {
                for(int i = 0; i < new_line_points.size() - 1; ++i)
                {
                    recreat_line_points.insert(new_line_points[i]);
                    ring_pt_arr.push_back(new_line_points[i]);
                    auto& cl = _add_nous_line(new_line_points[i], new_line_points[i + 1]);
                }
                recreat_line_points.insert(new_line_points.back());
            }
        }
    }
    nous::vec3 normal = sketch_math::ring_normal(polygon_pos[0]);

    this->add_polygon(polygon_pt_arr, normal);

    return polygon_pt_arr;
}

void GeoData::add_polygon(const std::vector<std::vector<SketchPointPtr>>& polygon, nous::vec3 normal)
{
    std::unordered_set<SketchPolygonPtr> coplane_polygons;
    std::unordered_set<SketchRingPtr> coplane_rings;
    nous::vec3 pos = this->get_point_position(polygon[0][0]);

    nous::AABB3 current_polygon_aabb3 = GeoDataUtils::get().get_aabb3(this, polygon[0]);
    std::unordered_set<SketchRingPtr> intersect_rings = m_ring_tree->search(current_polygon_aabb3);
    for(const auto& ring: intersect_rings)
    {
        SketchPolygonPtr polygon = m_ring_adj_polygons.at(ring).polygon;
        if(polygon)
        {
            const nous::vec3 ring_normal = polygon->normal;
            const nous::pos_t ring_pos = get_point_position(m_nousptr_points.at(ring->ring_prim->vertex_ptr()->point_ptr()));
            // 如果环与输入的 polygon 不共面，可以跳过运算
            if(!sketch_math::plane_coplane_plane(ring_normal, ring_pos, normal, pos))
            {
                continue;
            }

            coplane_polygons.insert(polygon);
            coplane_rings.insert(polygon->ring);
            coplane_rings.insert(polygon->inner_rings.begin(), polygon->inner_rings.end());
        }
    }

    std::map<SketchPolygonPtr, std::vector<std::vector<std::vector<SketchPointPtr>>>> recreate_polygons;
    std::vector<std::vector<std::vector<SketchPointPtr>>> new_polygons;
    geodata_topo::split_intersect_polygons(this, polygon, normal, coplane_polygons, recreate_polygons, new_polygons);

    // geos生成的polygon内环为联合体，可能由多个现存的环组成，需要拆分
    // 缓存相关联的rings topo
    std::unordered_map<size_t, std::vector<SketchPointPtr>> coplane_rings_topo;
    for(auto& ring: coplane_rings)
    {
        std::vector<SketchPointPtr> ring_topo = GeoDataUtils::get().get_ring_topo(this, ring);
        coplane_rings_topo[GeoData::ring_key(ring_topo)] = std::move(ring_topo);
    }
    auto split_polygon_inner_ring = [&](std::vector<std::vector<SketchPointPtr>>& polygon, std::vector<std::vector<SketchPointPtr>>& result_polygon) {
        result_polygon.emplace_back(std::move(polygon[0]));
        if(polygon.size() > 1)
        {
            // 判断内环是否可分裂
            for(int i = 1; i < polygon.size(); ++i)
            {
                auto& current_inner_ring = polygon[i];
                size_t crk = GeoData::ring_key(current_inner_ring);
                if(coplane_rings_topo.find(crk) == coplane_rings_topo.end())
                {
                    // 该内环不存在，则该内环可能由多个现存的环组成的新环，寻找子环
                    std::unordered_set<size_t> sub_ring_keys;
                    std::vector<std::vector<SketchPointPtr>> sub_ring_topos;
                    for(auto& ring: coplane_rings_topo)
                    {
                        if(GeoDataUtils::get().is_ring_contain_target(this, normal, current_inner_ring, ring.second))
                        {
                            sub_ring_keys.insert(ring.first);
                            sub_ring_topos.emplace_back(std::move(ring.second));
                        }
                    }
                    // 清空被包含的环
                    for(auto& sub_ring_key: sub_ring_keys)
                    {
                        coplane_rings_topo.erase(sub_ring_key);
                    }
                    // 计算新环
                    std::vector<std::vector<std::vector<SketchPointPtr>>> new_rings;
                    geodata_topo::ring_difference_other_rings(this, current_inner_ring, sub_ring_topos, normal, new_rings);
                    for(auto& polygon: new_rings)
                    {
                        // 只取结果的外环
                        result_polygon.emplace_back(std::move(polygon[0]));
                    }
                    // 添加被包含的环
                    for(auto& ring: sub_ring_topos)
                    {
                        result_polygon.emplace_back(std::move(ring));
                    }
                }
                else// 该内环存在，为最小环
                {
                    coplane_rings_topo.erase(crk);// 同时该环也不可能为其他子环的子环
                    result_polygon.emplace_back(std::move(current_inner_ring));
                }
            }
        }
    };

    // recreate_polygons 中包含了那些需要拆分的 polygon
    for(auto& polygon: recreate_polygons)
    {
        std::vector<std::vector<std::vector<SketchPointPtr>>> replace_polygons;
        for(auto& new_polygon: polygon.second)
        {
            std::vector<std::vector<SketchPointPtr>>& polygon_rings = replace_polygons.emplace_back();
            split_polygon_inner_ring(new_polygon, polygon_rings);
        }
        GeoDataTopoOperations::get().replace_polygon(this, polygon.first, replace_polygons);
    }

    // new_polygons 中包含的是新的 polygon 中的剩余部分
    for(auto& new_polygon: new_polygons)
    {
        std::vector<std::vector<SketchPointPtr>> polygon;
        split_polygon_inner_ring(new_polygon, polygon);
        this->add_polygon_without_check(polygon);
    }

    // std::unordered_set<size_t> recreat_lines;
    // for(auto point: recreat_line_points)
    //{
    //     this->get_point_adj_lines(point, recreat_lines);
    // }
    // for(auto lk: recreat_lines)
    //{
    //     auto line_pos = m_key_lines.find(lk);
    //     if(line_pos != m_key_lines.end())
    //     {
    //         GeoDataTopoOperations::get().check_path_and_split_sketch_polygon(this, m_key_lines.at(lk));
    //     }
    // }
}

std::vector<std::vector<SketchPointPtr>> GeoData::add_polygon_without_check(const std::vector<std::vector<nous::pos_t>>& polygon_pos,
                                                                            const bool add_line)
{
    std::vector<std::vector<SketchPointPtr>> polygon_pt_arr;
    for(auto& ring: polygon_pos)
    {
        auto& ring_pt_arr = polygon_pt_arr.emplace_back();
        for(auto& pos: ring)
        {
            SketchPointPtr new_point = add_point_without_check(pos);
            ring_pt_arr.emplace_back(new_point);
        }
    }

    this->add_polygon_without_check(polygon_pt_arr, add_line);
    return polygon_pt_arr;
}

SketchPolygonPtr GeoData::add_polygon_without_check(const std::vector<std::vector<SketchPointPtr>>& polygon, bool add_line)
{
    if(add_line)
    {
        for(const auto ring_pts: polygon)
        {
            for(int i = 0; i < ring_pts.size(); ++i)
            {
                SketchPointPtr prev_pt = ring_pts[i];
                SketchPointPtr next_pt = ring_pts[(i + 1) % ring_pts.size()];
                add_line_without_check(prev_pt, next_pt);
            }
        }
    }
    return _create_polygon(polygon);
}

void GeoData::delete_polygon(SketchPolygonPtr polygon, bool need_delete_line, bool need_merge_line,
                             std::unordered_set<SketchPointPtr>* force_keeped_point_set)
{
    std::unordered_set<SketchLinePtr> line_set;

    if(need_delete_line)
    {
        std::vector<SketchPointPtr> exring_pt_arr;
        get_element_points(polygon->ring, exring_pt_arr);
        for(int i = 0; i < exring_pt_arr.size(); ++i)
        {
            SketchPointPtr prev_pt = exring_pt_arr[i];
            SketchPointPtr next_pt = exring_pt_arr[(i + 1) % exring_pt_arr.size()];

            const size_t line_key = GeoData::line_key(prev_pt, next_pt);
            line_set.insert(m_key_lines.at(line_key));
        }

        for(auto& inner_ring: polygon->inner_rings)
        {
            std::vector<SketchPointPtr> inring_pt_arr;
            get_element_points(inner_ring, inring_pt_arr);

            for(int i = 0; i < inring_pt_arr.size(); ++i)
            {
                SketchPointPtr prev_pt = inring_pt_arr[i];
                SketchPointPtr next_pt = inring_pt_arr[(i + 1) % inring_pt_arr.size()];

                const size_t line_key = GeoData::line_key(prev_pt, next_pt);
                line_set.insert(m_key_lines.at(line_key));
            }
        }
    }

    _delete_polygon(polygon);

    if(need_delete_line)
    {
        // 检查所有的 line 是否还被 ring 使用，不被使用的话移除这些 line
        for(auto line: line_set)
        {
            if(get_line_adj_rings(line).empty())
            {
                delete_line(line, need_merge_line, true, force_keeped_point_set);
            }
            else
            {
                SketchPointPtr p0 = line->p0;
                SketchPointPtr p1 = line->p1;
                GeoDataTopoOperations::get().check_point_and_merge_line(this, p0, force_keeped_point_set);
                GeoDataTopoOperations::get().check_point_and_merge_line(this, p1, force_keeped_point_set);
            }
        }
    }

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
}

void GeoData::add_element_group(ElementGroupPtr group) { m_ele_groups.insert(group); }

void GeoData::delete_element_group(ElementGroupPtr group)
{
    group->clear();
    m_ele_groups.erase(group);
}

void GeoData::_add_line(const nous::vec3& lp0, const nous::vec3& lp1, std::map<SketchLinePtr, std::vector<SketchPointPtr>>& recreat_lines,
                        std::vector<SketchPointPtr>& new_line_points)
{
    this->_sync_nous();

    auto lessThan = [](const nous::vec3& v1, const nous::vec3& v2) {
        float xoff = v1.x() - v2.x();
        float yoff = v1.y() - v2.y();
        float zoff = v1.z() - v2.z();

        float epsilon = 1e-2f;
        if(xoff < -epsilon)
        {
            return true;
        }
        else if(std::abs(xoff) <= epsilon && yoff < -epsilon)
        {
            return true;
        }
        else if(std::abs(xoff) <= epsilon && std::abs(yoff) <= epsilon && zoff < -epsilon)
        {
            return true;
        }
        return false;
    };
    auto swapV3 = [](nous::vec3& v1, nous::vec3& v2) {
        nous::vec3 t;
        t = v1;
        v1 = v2;
        v2 = t;
    };
    auto equalV3 = [](const nous::vec3& v1, const nous::vec3& v2) {
        float epsilon = 1e-2f;
        return (v1 - v2).squaredNorm() < epsilon * epsilon;
    };

    std::map<float, std::pair<nous::vec3, SketchPointPtr>> _new_line_point;
    std::map<SketchLinePtr, std::vector<std::pair<float, SketchPointPtr>>> _recreat_line_point;

    if(equalV3(lp0, lp1))
        return;
    bool isSwap = false;
    nous::vec3 p0 = lp0;
    nous::vec3 p1 = lp1;
    if(lessThan(p1, p0))
    {
        isSwap = true;
        swapV3(p0, p1);
    }
    nous::vec3 u = p1 - p0;
    float line_min_len_pow2 = 0.0f;
    float line_max_len_pow2 = u.squaredNorm();
    _new_line_point[line_min_len_pow2] = { p0, nullptr };
    _new_line_point[line_max_len_pow2] = { p1, nullptr };

    nous::AABB3 current_line_aabb3 = sketch_math::aabb3_from_points({ p0, p1 }, 0.1f);
    std::unordered_set<SketchLinePtr> intersect_lines = m_lines_tree->search(current_line_aabb3);
    // nous::parallel_for_light_items(nous::BlockedRange(m_key_lines.size()), [&](const auto& r) {
    // for (auto li : r.index_range()) {
    for(auto& line: intersect_lines)
    {
        // box intersect  precessor?
        SketchPointPtr np2 = line->p0;
        SketchPointPtr np3 = line->p1;
        nous::vec3 p2 = m_nous_position_handle.get(np2->point->idx());
        nous::vec3 p3 = m_nous_position_handle.get(np3->point->idx());

        if(lessThan(p3, p2))
        {
            swapV3(p2, p3);
            SketchPointPtr t = np2;
            np2 = np3;
            np3 = t;
        }
        if(equalV3(p0, p2) && equalV3(p1, p3))
        {
            _new_line_point[line_min_len_pow2] = { p2, np2 };
            _new_line_point[line_max_len_pow2] = { p3, np3 };
            // return;
            break;
        }
        nous::vec3 v = p3 - p2;
        nous::vec3 w = p0 - p2;
        // nous::vec3 u_cross_v = u.cross(v);
        // float n = u_cross_v.squaredNorm();
        if(sketch_math::direction_parallel(u, v))
        {
            // nous::vec3 u_cross_v2 = u.cross(w);
            // float n2 = u_cross_v2.squaredNorm();
            if(!sketch_math::direction_parallel(u, w))
            {
                continue;
            }
            if(lessThan(p1, p2) || lessThan(p3, p0))
            {
                continue;
            }
            else if(equalV3(p1, p2))
            {
                _new_line_point[line_max_len_pow2] = { p2, np2 };
            }
            else if(equalV3(p1, p3))
            {
                _new_line_point[line_max_len_pow2] = { p3, np3 };
                if(lessThan(p2, p0))
                {
                    _recreat_line_point[line].push_back({ 0.0f, np2 });
                    _recreat_line_point[line].push_back({ line_min_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ 0.0f, np3 });
                }
                else
                {
                    _new_line_point[(p2 - p0).squaredNorm()] = { p2, np2 };
                }
            }
            else if(equalV3(p0, p3))
            {
                _new_line_point[line_min_len_pow2] = { p3, np3 };
            }
            else if(equalV3(p0, p2))
            {
                _new_line_point[line_min_len_pow2] = { p2, np2 };
                if(lessThan(p1, p3))
                {
                    _recreat_line_point[line].push_back({ 0.0f, np2 });
                    _recreat_line_point[line].push_back({ line_max_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ 0.0f, np3 });
                }
                else
                {
                    _new_line_point[(p3 - p0).squaredNorm()] = { p3, np3 };
                }
            }
            else
            {
                bool a = lessThan(p2, p0);
                bool b = lessThan(p1, p3);
                if(a && b)
                {
                    _new_line_point[line_min_len_pow2] = { p0, nullptr };
                    _new_line_point[line_max_len_pow2] = { p1, nullptr };
                    _recreat_line_point[line].push_back({ 0.0f, np2 });
                    _recreat_line_point[line].push_back({ line_min_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ line_max_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ 0.0f, np3 });
                }
                else if(a)
                {
                    _new_line_point[line_min_len_pow2] = { p0, nullptr };
                    _new_line_point[(p3 - p0).squaredNorm()] = { p3, np3 };
                    _recreat_line_point[line].push_back({ 0.0f, np2 });
                    _recreat_line_point[line].push_back({ line_min_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ 0.0f, np3 });
                }
                else if(b)
                {
                    _new_line_point[line_max_len_pow2] = { p1, nullptr };
                    _new_line_point[(p2 - p0).squaredNorm()] = { p2, np2 };
                    _recreat_line_point[line].push_back({ 0.0f, np2 });
                    _recreat_line_point[line].push_back({ line_max_len_pow2, nullptr });
                    _recreat_line_point[line].push_back({ 0.0f, np3 });
                }
                else
                {
                    _new_line_point[(p2 - p0).squaredNorm()] = { p2, np2 };
                    _new_line_point[(p3 - p0).squaredNorm()] = { p3, np3 };
                }
            }
        }
        else
        {
            if(equalV3(p0, p2))
            {
                _new_line_point[line_min_len_pow2] = { p2, np2 };
            }
            else if(equalV3(p0, p3))
            {
                _new_line_point[line_min_len_pow2] = { p3, np3 };
            }
            else if(equalV3(p1, p2))
            {
                _new_line_point[line_max_len_pow2] = { p2, np2 };
            }
            else if(equalV3(p1, p3))
            {
                _new_line_point[line_max_len_pow2] = { p3, np3 };
            }
            else
            {
                nous::vec3 u_cross_v = u.cross(v);
                float n = u_cross_v.squaredNorm();

                float s = v.cross(w).dot(u_cross_v) / n;
                float t = u.cross(w).dot(u_cross_v) / n;

                s = std::min(std::max(0.0f, s), 1.0f);
                t = std::min(std::max(0.0f, t), 1.0f);
                if(s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f)
                {
                    nous::vec3 p0x = p0 + u * s;
                    nous::vec3 p2x = p2 + v * t;
                    if(equalV3(p0x, p2x))
                    {
                        float d = (p0x - p0).squaredNorm();
                        if(equalV3(p0x, p2))
                        {
                            _new_line_point[d] = { p2, np2 };
                        }
                        else if(equalV3(p0x, p3))
                        {
                            _new_line_point[d] = { p3, np3 };
                        }
                        else
                        {
                            _new_line_point[d] = { p0x, nullptr };
                            _recreat_line_point[line].push_back({ 0.0f, np2 });
                            _recreat_line_point[line].push_back({ d, nullptr });
                            _recreat_line_point[line].push_back({ 0.0f, np3 });
                        }
                    }
                }
            }
        }
    }
    //});

    auto prePos = _new_line_point.begin();
    for(auto it = _new_line_point.begin(); it != _new_line_point.end(); ++it)
    {
        if(it != prePos)
        {
            if((it->second.second && (it->second.second == prePos->second.second)) || equalV3(it->second.first, prePos->second.first))
            {
                it->second.first = prePos->second.first;
                it->second.second = it->second.second ? it->second.second : prePos->second.second;
                ++prePos;
                continue;
            }
            ++prePos;
        }
        if(!it->second.second)
        {
            nous::Point* new_point = m_nous_mesh->point(m_nous_mesh->add_point(it->second.first));
            SketchPointPtr sp = std::make_shared<SketchPoint>(new_point);
            m_nousptr_points[new_point] = sp;
            it->second.second = sp;
        }
        new_line_points.push_back(it->second.second);
    }

    if(isSwap)
    {
        std::reverse(new_line_points.begin(), new_line_points.end());
    }

    for(auto& l: _recreat_line_point)
    {
        for(int i = 0; i < l.second.size(); ++i)
        {
            SketchPointPtr np = l.second[i].second;
            if(np == nullptr)
            {
                auto p0 = _new_line_point.find(l.second[i].first);
                np = p0->second.second;
            }
            recreat_lines[l.first].push_back(np);
        }
    }
}

SketchLinePtr GeoData::_add_nous_line(SketchPointPtr p0, SketchPointPtr p1)
{
    size_t key = GeoData::line_key(p0, p1);
    auto& pos = m_key_lines.find(key);
    if(pos == m_key_lines.end())
    {
        nous::Prim* line_prim = m_nous_mesh->prim(m_nous_mesh->add_prim(nous::Array<nous::Point*>{ p0->point, p1->point }, nous::PrimType::Polyline));

        SketchLinePtr line = std::make_shared<SketchLine>(line_prim, key);
        line->p0 = p0;
        line->p1 = p1;

        // add rtree
        nous::AABB3 line_box = GeoDataUtils::get().get_element_aabb3(this, line);
        m_lines_tree->insert_object(line, line_box);
        m_key_lines[key] = line;
        m_nousptr_lines[line_prim] = line;
    }

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
    return m_key_lines[key];
}

void GeoData::_delete_nous_line(SketchLinePtr line)
{
    if(line->group())
    {
        line->group()->erase_element(line);
    }

    // clear rtree
    nous::AABB3 line_box = GeoDataUtils::get().get_element_aabb3(this, line);
    m_lines_tree->delete_object(line, line_box);

    m_nousptr_lines.erase(line->line_prim);
    m_nous_mesh->delete_prim(line->line_prim->idx(), false);
    m_key_lines.erase(line->key);

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
}

SketchRingPtr GeoData::_add_nous_ring(const std::vector<SketchPointPtr>& topo)
{
    size_t ring_key = GeoData::ring_key(topo);
    auto find_pos = m_key_rings.find(ring_key);
    if(find_pos == m_key_rings.end())
    {
        nous::Array<nous::Point*> n_topo;
        for(auto& p: topo)
        {
            n_topo.push_back(p->point);
        }
        nous::Prim* new_prim = m_nous_mesh->prim(m_nous_mesh->add_prim(n_topo, nous::PrimType::Polygon));

        SketchRingPtr new_ring = std::make_shared<SketchRing>(new_prim, ring_key);
        std::vector<nous::vec3> vp = GeoDataUtils::get().get_ring_positions(this, new_ring);
        new_ring->normal = sketch_math::ring_normal(vp);

        // rtree
        nous::AABB3 ring_box = GeoDataUtils::get().get_aabb3(this, vp);
        m_ring_tree->insert_object(new_ring, ring_box);

        m_key_rings[ring_key] = new_ring;
        m_nousptr_rings[new_prim] = new_ring;
        m_dirty_flags |= 0x1;
        m_dirty_flags |= 0x2;
        return new_ring;
    }
    return find_pos->second;
}

void GeoData::_delete_nous_ring(SketchRingPtr ring)
{
    // if(m_ring_adj_polygons.find(ring) == m_ring_adj_polygons.end())
    //{
    //     return;
    // }

    SketchPolygonPtr parent_polygon = m_ring_adj_polygons[ring].parent_polygon;
    SketchPolygonPtr polygon = m_ring_adj_polygons[ring].polygon;
    if(parent_polygon)
    {
        parent_polygon->inner_rings.erase(ring);
        parent_polygon->m_changed = true;
    }
    if(polygon)
    {
        // smooth group delete
        if(polygon->group() && polygon->group()->type() == EElementType::SmoothGroup)
        {
            polygon->group()->erase_element(polygon);
        }
        //
        std::unordered_set<SketchRingPtr> need_delete;
        for(auto inner_ring: polygon->inner_rings)
        {
            m_ring_adj_polygons[inner_ring].parent_polygon = parent_polygon;
            if(parent_polygon)
            {
                parent_polygon->inner_rings.insert(inner_ring);
            }
            else if(!m_ring_adj_polygons[inner_ring].polygon)
            {
                need_delete.insert(inner_ring);
            }
        }
        for(auto inner_ring: need_delete)
        {
            _delete_nous_ring(inner_ring);
        }
    }

    // this->_sync_nous();
    // rtree
    nous::AABB3 ring_box = GeoDataUtils::get().get_element_aabb3(this, ring);
    m_ring_tree->delete_object(ring, ring_box);

    std::vector<SketchPointPtr> topo = get_element_points(ring);
    m_key_rings.erase(ring->key);
    m_nousptr_rings.erase(ring->ring_prim);
    m_ring_adj_polygons.erase(ring);

    m_nous_mesh->delete_prim(ring->ring_prim->idx(), false);

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
}

void GeoData::_find_coplain_rings(SketchPointPtr current, SketchPointPtr end, std::vector<SketchPointPtr>& path,
                                  std::unordered_set<SketchPointPtr>& visited, std::vector<nous::vec3>& normal, std::vector<nous::vec3>& dir,
                                  uint8_t pathMode, std::vector<std::pair<nous::vec3, std::vector<std::vector<SketchPointPtr>>>>& result)
{
    float dot_epsilon = 1e-3f;
    this->_sync_nous();
    if(current == end)
    {
        if(path.size() > 2)
        {
            bool findCoplan = false;
            for(auto& plan: result)
            {
                float dp = normal.back().normalized().dot(plan.first.normalized());
                if(std::abs(dp - 1.0f) < dot_epsilon)
                {
                    plan.second.push_back(path);
                    findCoplan = true;
                }
            }
            if(!findCoplan)
            {
                std::vector<std::vector<SketchPointPtr>> coplanTopo;
                coplanTopo.push_back(path);
                result.push_back({ normal.back(), coplanTopo });
            }
        }
        return;
    }

    std::multimap<float, SketchPointPtr> validNeighbor;
    for(auto& neighbor: get_point_adj_points(current))
    {
        if(visited.find(neighbor) == visited.end() || neighbor == end)
        {
            nous::vec3 currentPos = m_nous_position_handle.get(current->point->idx());
            nous::vec3 neighborPos = m_nous_position_handle.get(neighbor->point->idx());
            nous::vec3 ab = dir.back();
            nous::vec3 bc = neighborPos - currentPos;
            float k = -bc.dot(ab) / ab.dot(ab);
            validNeighbor.insert(std::make_pair(k, neighbor));
        }
    }
    // 狭长多边形压缩长�?
    float scale = 0.01f;
    if(normal.back().squaredNorm() <= dot_epsilon)
    {
        std::vector<std::pair<nous::vec3, std::map<float, std::pair<SketchPointPtr, nous::vec3>>>> validPlan;
        std::pair<SketchPointPtr, nous::vec3> paraPath = { nullptr, nous::vec3() };
        for(auto& pair: validNeighbor)
        {
            SketchPointPtr neighbor = pair.second;
            nous::vec3 currentPos = m_nous_position_handle.get(current->point->idx());
            nous::vec3 neighborPos = m_nous_position_handle.get(neighbor->point->idx());
            neighborPos = neighborPos + dir.back() * pair.first * (1.0f - scale);
            nous::vec3 cdir = (neighborPos - currentPos).normalized();
            nous::vec3 planNormal = dir.back().cross(cdir).normalized();
            float angle = sketch_math::direction_angle(dir.back(), cdir, planNormal, dot_epsilon);
            if(angle == 360.0f || angle == 0.0f)
            {
                continue;
            }
            else if(angle == 180.0f)
            {
                paraPath = std::make_pair(neighbor, cdir);
            }
            else
            {
                bool findCoplan = false;
                for(auto& plan: validPlan)
                {
                    float dp = cdir.dot(plan.first.normalized());
                    if(plan.first.squaredNorm() > dot_epsilon && std::abs(dp) < dot_epsilon && planNormal.squaredNorm() > dot_epsilon)
                    {
                        findCoplan = true;
                        angle = sketch_math::direction_angle(dir.back(), cdir, plan.first, dot_epsilon);
                        plan.second[angle] = std::make_pair(neighbor, cdir);
                        break;
                    }
                }
                if(!findCoplan)
                {
                    std::map<float, std::pair<SketchPointPtr, nous::vec3>> newPlan;
                    newPlan[angle] = std::make_pair(neighbor, cdir);
                    validPlan.emplace_back(planNormal, newPlan);
                }
            }
        }

        if(!validPlan.size() && paraPath.first)
        {
            std::map<float, std::pair<SketchPointPtr, nous::vec3>> noPlan;
            noPlan[180.0f] = paraPath;
            validPlan.emplace_back(nous::vec3(.0f, .0f, .0f), noPlan);
        }

        for(auto& plan: validPlan)
        {
            if(paraPath.first)
                plan.second[180.0f] = paraPath;
            std::map<uint8_t, std::pair<SketchPointPtr, nous::vec3>> validPoint;
            if(plan.second.size())
            {
                auto path1 = plan.second.begin();
                auto path2 = std::prev(plan.second.end());
                if(path1 != path2)
                {
                    if(pathMode & 0x1)
                    {
                        validPoint[0x1] = path1->second;
                    }
                    if(pathMode & 0x2)
                    {
                        validPoint[0x2] = path2->second;
                    }
                }
                else
                {
                    validPoint[pathMode] = path1->second;
                }
            }

            for(auto& pos: validPoint)
            {
                SketchPointPtr neighbor = pos.second.first;
                normal.push_back(plan.first);
                dir.push_back(pos.second.second);
                path.push_back(neighbor);
                visited.insert(neighbor);
                this->_find_coplain_rings(neighbor, end, path, visited, normal, dir, pos.first, result);
                normal.pop_back();
                dir.pop_back();
                visited.erase(neighbor);
                path.pop_back();
            }
        }
    }
    else
    {
        std::map<float, std::pair<SketchPointPtr, nous::vec3>> anglePoint;
        for(auto& pair: validNeighbor)
        {
            SketchPointPtr neighbor = pair.second;
            nous::vec3 currentPos = m_nous_position_handle.get(current->point->idx());
            nous::vec3 neighborPos = m_nous_position_handle.get(neighbor->point->idx());
            neighborPos = neighborPos + dir.back() * pair.first * (1.0 - scale);
            nous::vec3 cdir = (neighborPos - currentPos).normalized();
            float dp = normal.back().normalized().dot(cdir);
            if(std::abs(dp) < dot_epsilon)
            {
                float angle = sketch_math::direction_angle(dir.back(), cdir, normal.back(), dot_epsilon);
                if(angle == 360.0f || angle == 0.0f)
                {
                    continue;
                }
                anglePoint[angle] = { neighbor, cdir };
            }
        }

        std::map<uint8_t, std::pair<SketchPointPtr, nous::vec3>> validPoint;
        if(anglePoint.size())
        {
            auto path1 = anglePoint.begin();
            auto path2 = std::prev(anglePoint.end());
            if(path1 != path2)
            {
                if(pathMode & 0x1)
                {
                    validPoint[0x1] = path1->second;
                }
                if(pathMode & 0x2)
                {
                    validPoint[0x2] = path2->second;
                }
            }
            else
            {
                validPoint[pathMode] = path1->second;
            }
        }
        for(auto& pos: validPoint)
        {
            SketchPointPtr neighbor = pos.second.first;
            normal.push_back(normal.back());
            dir.push_back(pos.second.second);
            path.push_back(neighbor);
            visited.insert(neighbor);
            this->_find_coplain_rings(neighbor, end, path, visited, normal, dir, pos.first, result);
            normal.pop_back();
            dir.pop_back();
            visited.erase(neighbor);
            path.pop_back();
        }
    }
}

void GeoData::_find_split_ring_path(SketchPointPtr cpos, SketchRingPtr ring, std::unordered_set<SketchPointPtr>& visited, const nous::vec3& normal,
                                    std::vector<SketchPointPtr>& path, std::vector<SketchPointPtr>& result)
{
    float dot_epsilon = 1e-3f;
    this->_sync_nous();
    if(visited.find(cpos) == visited.end())
    {
        path.push_back(cpos);

        for(auto i: m_nous_mesh->point_prims_range(cpos->point->idx()))
        {
            if(ring->ring_prim->idx() == i)
            {
                result = path;
                return;
            }
        }

        visited.insert(cpos);

        for(auto& neighbor: get_point_adj_points(cpos))
        {
            if(visited.find(neighbor) != visited.end())
            {
                continue;
            }

            nous::vec3 p0 = m_nous_position_handle.get(cpos->point->idx());
            nous::vec3 p1 = m_nous_position_handle.get(neighbor->point->idx());
            nous::vec3 uDir = (p1 - p0).normalized();
            float dp = uDir.dot(normal);

            if(std::abs(dp) < dot_epsilon)
            {
                _find_split_ring_path(neighbor, ring, visited, normal, path, result);
                if(!result.empty())
                {
                    return;
                }
            }
        }

        path.pop_back();
    }
}

SketchRingPtr GeoData::_check_ring_affilia_and_create_polygon(const std::vector<SketchPointPtr>& ring, const bool gen_polygon,
                                                              const bool verify_parent)
{
    SketchRingPtr contained = nullptr;
    std::unordered_set<SketchRingPtr> contains;

    // 计算 Ring 是否已经存在，若不存在则返回被它包含的 Rings 以及包含它的最近的一个 Ring
    if(m_key_rings.find(GeoData::ring_key(ring)) != m_key_rings.end())
    {
        return m_key_rings[GeoData::ring_key(ring)];
    }
    GeoDataUtils::get().get_ring_relation_rings(this, ring, contained, contains);

    if(!contains.empty() || contained)
    {
        std::cout << "Debug Here!" << std::endl;
    }

    // 创建这个 SketchPolygon 并且挂接空间拓扑关系
    SketchRingPtr new_ring = _add_nous_ring(ring);
    SketchPolygonPtr new_skp = nullptr;
    if(gen_polygon)
    {
        new_skp = _create_polygon(new_ring, std::unordered_set<SketchRingPtr>{});
    }

    if(contained && m_ring_adj_polygons[contained].polygon)
    {
        m_ring_adj_polygons[contained].polygon->inner_rings.insert(new_ring);
        m_ring_adj_polygons[contained].polygon->m_changed = true;
        m_ring_adj_polygons[new_ring].parent_polygon = m_ring_adj_polygons[contained].polygon;
    }

    // std::unordered_set<nous::Prim*> visitedPath;
    if(new_skp)
    {
        for(auto inner_ring: contains)
        {
            SketchRingPtr inner_ring_contained = nullptr;
            std::unordered_set<SketchRingPtr> inner_ring_contains;
            GeoDataUtils::get().get_ring_relation_rings(this, inner_ring, inner_ring_contained, inner_ring_contains);

            // inner_ring_contained 不等于当前 ring 说明这个 inner_ring 被其余 inner_ring 包含，跳过
            if(inner_ring_contained != new_skp->ring)
            {
                continue;
            }

            if(contained && m_ring_adj_polygons[contained].polygon)
            {
                m_ring_adj_polygons[contained].polygon->inner_rings.erase(inner_ring);
            }

            m_ring_adj_polygons[inner_ring].parent_polygon = new_skp;
            new_skp->inner_rings.insert(inner_ring);
        }

        // 验证 Polygon 的正确性
        GeoDataTopoOperations::get().verify_sketch_polygon(this, new_skp, false);
    }

    if(verify_parent && contained && m_ring_adj_polygons[contained].polygon)
    {
        GeoDataTopoOperations::get().verify_sketch_polygon(this, m_ring_adj_polygons[contained].polygon, false);
    }

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;

    return new_ring;
}

SketchPolygonPtr GeoData::_create_polygon(const std::vector<std::vector<SketchPointPtr>>& ring)
{
    SketchRingPtr main_ring = this->_add_nous_ring(ring[0]);
    std::unordered_set<SketchRingPtr> inner_ring;
    for(int i = 1; i < ring.size(); ++i)
    {
        inner_ring.insert(this->_add_nous_ring(ring[i]));
    }

    return _create_polygon(main_ring, inner_ring);
}

SketchPolygonPtr GeoData::_create_polygon(SketchRingPtr ring, const std::unordered_set<SketchRingPtr>& inner_rings)
{
    SketchPolygonPtr new_skp = std::make_shared<SketchPolygon>(ring);
    new_skp->normal = ring->normal;
    new_skp->inner_rings = inner_rings;
    for(auto inner_ring: inner_rings)
    {
        m_ring_adj_polygons[inner_ring].parent_polygon = new_skp;
    }

    auto attr_handle = m_nous_mesh->get_or_create_prim_attrib<nous::vec3>(nous::ATTRIB_NAME_NORMAL, nous::vec3())->write();
    attr_handle.set(ring->ring_prim->idx(), ring->normal);

    auto pos = m_ring_adj_polygons.find(ring);
    if(pos == m_ring_adj_polygons.end())
    {
        m_ring_adj_polygons[ring] = { new_skp, nullptr };
    }
    m_ring_adj_polygons[ring].polygon = new_skp;
    m_ring_adj_polygons[ring].polygon->normal = ring->normal;

    m_dirty_flags |= 0x1;
    m_dirty_flags |= 0x2;
    return new_skp;
}

void GeoData::_delete_polygon(SketchPolygonPtr polygon)
{
    // smooth group delete
    if(polygon->group() && polygon->group()->type() == EElementType::SmoothGroup)
    {
        polygon->group()->erase_element(polygon);
    }
    //
    for(auto inner_ring: polygon->inner_rings)
    {
        m_ring_adj_polygons[inner_ring].parent_polygon = nullptr;
        if(!m_ring_adj_polygons[inner_ring].polygon)
        {
            _delete_nous_ring(inner_ring);
        }
    }
    polygon->inner_rings.clear();

    SketchRingPtr ring = polygon->ring;
    m_ring_adj_polygons[ring].polygon = nullptr;
    if(m_ring_adj_polygons[ring].parent_polygon)
    {
        return;
    }
    _delete_nous_ring(ring);
}

void GeoData::_trangulate_sketch_polygon(SketchPolygonPtr vp)
{
    vp->triangles.clear();
    if(vp->ring)
    {
        geodata_topo::triangulate_polygon_cdt(this, vp);
        if(vp->triangles.size() == 0)
        {
            std::vector<std::vector<nous::pos_t>> polygon_point_pos = GeoDataUtils::get().get_polygon_positions(this, vp);
            std::vector<std::vector<std::vector<nous::pos_t>>> splited_valid_polygon = topo::rebuild_invalid_polygon_topo(polygon_point_pos);
            // 产生了潜在的非法多边形，尝试解决
            for(int i = 0; i < splited_valid_polygon.size(); ++i)
            {
                const auto& triangles = geodata_topo::triangulate_polygon(splited_valid_polygon[i]);
                for(int j = 0; j < triangles.size(); ++j)
                {
                    SketchPolygonTriangle triangle;
                    triangle.emplace_back(nullptr, triangles[j][0]);
                    triangle.emplace_back(nullptr, triangles[j][1]);
                    triangle.emplace_back(nullptr, triangles[j][2]);
                    vp->triangles.push_back(triangle);
                }
            }
        }
    }
}

bool GeoData::_update_render_resource()
{
    this->_sync_nous();
    IRenderable::clear();

    std::map<SketchPolygonPtr, std::vector<unsigned int>> polygon_submesh;
    std::map<SketchLinePtr, std::vector<unsigned int>> line_submesh;

    std::vector<float>& position = this->get_render_position();
    std::vector<float>& normal = this->get_render_normal();
    std::vector<float>& color = this->get_render_color();
    std::vector<float>& uv = this->get_render_uv();
    std::vector<unsigned int>& lines_index = this->get_render_lines_index();
    std::vector<unsigned int>& triangles_index = this->get_render_triangles_index();
    std::vector<SubmeshData>& submeshes = this->get_render_submesh();
    std::unordered_map<SketchPointPtr, size_t> mesh_point_vertex_map;

    nous::vec3 standard_z(0.0f, 0.0f, 1.0f);
    nous::vec3 standard_x(1.0f, 0.0f, 0.0f);

    const size_t base_point_size = position.size() / 3;
    std::vector<nous::pos_t> added_positions;
    std::vector<nous::vec3i> added_triangles;
    for(auto& skp: get_all_polygons())
    {
        const nous::vec3 skp_normal = skp->normal;

        nous::quat rotate_to_xy_plane = nous::quat::Identity();
        nous::quat rotate_to_standard_x = nous::quat::Identity();
        // 沿任意轴将 normal 转到 (0, 0, 1) 后还需要将 tangent 转到 (1, 0, 0) 上，否则 uv 就歪七扭八的
        // Normal 与 Z 轴平行的面可以不处理，直接使用 Identity 的就可以
        if(skp_normal.dot(standard_z) < 1 - 1e-5 && skp_normal.dot(standard_z) > -1 + 1e-5)
        {
            rotate_to_xy_plane = nous::quat::FromTwoVectors(skp_normal, nous::vec3(0.f, 0.f, 1.f));
            const nous::vec3 skp_tangent = skp_normal.cross(standard_z);
            const nous::vec3 rotated_tangent = rotate_to_xy_plane * skp_tangent;
            rotate_to_standard_x = nous::quat::FromTwoVectors(rotated_tangent, standard_x);
        }

        if(!skp)
        {
            continue;
        }
        skp->ring->key = GeoData::ring_key(get_element_points(skp->ring));

        if(skp->m_changed)
        {
            skp->m_changed = false;
            skp->triangles.clear();
            _trangulate_sketch_polygon(skp);
        }

        std::unordered_map<SketchPointPtr, size_t> prim_point_vertex_map;
        for(auto& tri: skp->triangles)
        {
            auto fill_tri = [&position, &normal, &color, &uv, &prim_point_vertex_map, &mesh_point_vertex_map, &polygon_submesh, &skp,
                             &added_positions, &added_triangles, rotate_to_xy_plane,
                             rotate_to_standard_x](std::pair<SketchPointPtr, nous::vec3>& point, auto& handle) -> size_t {
                size_t pt_idx = 0;
                if(point.first)
                {
                    point.second = handle.get(point.first->point->idx());
                    if(prim_point_vertex_map.find(point.first) == prim_point_vertex_map.end())
                    {
                        const nous::pos_t xoy_pos = rotate_to_xy_plane * point.second;
                        const nous::pos_t standard_xoy_pos = rotate_to_standard_x * xoy_pos;
                        int index = position.size() / 3;
                        prim_point_vertex_map[point.first] = index;
                        mesh_point_vertex_map[point.first] = index;
                        position.push_back(point.second.x());
                        position.push_back(point.second.y());
                        position.push_back(point.second.z());
                        normal.push_back(skp->normal.x());
                        normal.push_back(skp->normal.y());
                        normal.push_back(skp->normal.z());
                        uv.push_back(standard_xoy_pos.x() / 100.0);
                        uv.push_back(standard_xoy_pos.y() / 100.0);
                        added_positions.emplace_back(point.second);
                    }
                    polygon_submesh[skp].push_back(prim_point_vertex_map[point.first]);
                    pt_idx = prim_point_vertex_map[point.first];
                }
                else
                {
                    int index = position.size() / 3;
                    const nous::pos_t xoy_pos = rotate_to_xy_plane * point.second;
                    const nous::pos_t standard_xoy_pos = rotate_to_standard_x * xoy_pos;
                    position.push_back(point.second.x());
                    position.push_back(point.second.y());
                    position.push_back(point.second.z());
                    normal.push_back(skp->normal.x());
                    normal.push_back(skp->normal.y());
                    normal.push_back(skp->normal.z());
                    uv.push_back(standard_xoy_pos.x() / 100.0);
                    uv.push_back(standard_xoy_pos.y() / 100.0);
                    polygon_submesh[skp].push_back(index);
                    added_positions.emplace_back(point.second);
                    pt_idx = index;
                }

                return pt_idx;
            };
            const size_t t1 = fill_tri(tri[0], m_nous_position_handle) - base_point_size;
            const size_t t2 = fill_tri(tri[1], m_nous_position_handle) - base_point_size;
            const size_t t3 = fill_tri(tri[2], m_nous_position_handle) - base_point_size;
            added_triangles.emplace_back(nous::vec3i(t1, t2, t3));
        }
    }

    // line vertex
    // nous::parallel_for_light_items(nous::BlockedRange(m_key_lines.size()), [&](const auto& r) {
    // for (auto li : r.index_range()) {

    for(auto& line_pair: m_key_lines)
    {
        SketchLinePtr line = line_pair.second;
        auto point_valid_vertex = [&position, &normal, &mesh_point_vertex_map, &color, &uv](SketchPointPtr np, auto& handle) {
            if(mesh_point_vertex_map.find(np) == mesh_point_vertex_map.end())
            {
                size_t valid_ver = position.size() / 3;
                mesh_point_vertex_map[np] = valid_ver;
                nous::vec3& p = handle.get(np->point->idx());
                position.push_back(p.x());
                position.push_back(p.y());
                position.push_back(p.z());
                normal.push_back(0.0);
                normal.push_back(0.0);
                normal.push_back(0.0);
                uv.push_back(rand(0.0, 1.0));
                uv.push_back(rand(0.0, 1.0));
            }
            return mesh_point_vertex_map[np];
        };

        line_submesh[line_pair.second].push_back(point_valid_vertex(line->p0, m_nous_position_handle));
        line_submesh[line_pair.second].push_back(point_valid_vertex(line->p1, m_nous_position_handle));
    }

    for(auto& ele: polygon_submesh)
    {
        SketchElementPtr group = ele.first->group();
        submeshes.emplace_back("Polygon", triangles_index.size(), ele.second.size(), SubmeshData::Topology::Triangles, group ? group : ele.first);
        triangles_index.insert(triangles_index.end(), ele.second.begin(), ele.second.end());
    }

    for(auto& ele: line_submesh)
    {
        SketchElementPtr group = ele.first->group();
        submeshes.emplace_back("Line", lines_index.size(), ele.second.size(), SubmeshData::Topology::Lines, group ? group : ele.first);
        lines_index.insert(lines_index.end(), ele.second.begin(), ele.second.end());
    }

    return true;
}

void GeoData::save(SketchSerializer& serializer)
{
    nous::Array<nous::uint8> mesh_binary_buffer;
    m_nous_mesh->save_binary(mesh_binary_buffer);
    serializer.write_binary(mesh_binary_buffer, "nous_mesh");

    // polygon
    serializer.begin_write_array("polygons");
    for(auto& polygon: this->m_ring_adj_polygons)
    {
        if(polygon.second.polygon == nullptr)
        {
            continue;
        }
        serializer.begin_write_object();
        serializer.advance_write(polygon.first->normal, "normal");
        serializer.write(polygon.first->ring_prim->idx(), "ring_idx");
        serializer.begin_write_array("inner_ring_idxs");
        for(auto inner_ring: polygon.second.polygon->inner_rings)
        {
            serializer.write(inner_ring->ring_prim->idx(), "idx");
        }
        serializer.end_write_array();
        serializer.end_write_object();
    }
    serializer.end_write_array();

    // group
    serializer.begin_write_array("no_branch_line_group");
    for(auto group: m_ele_groups)
    {
        if(group->type() == EElementType::NoBranchLineGroup)
        {
            serializer.begin_write_array("elements");
            for(auto line: group->elements())
            {
                serializer.write(line->to_line()->line_prim->idx());
            }
            serializer.end_write_array();
        }
    }
    serializer.end_write_array();
    serializer.begin_write_array("smooth_group");
    for(auto group: m_ele_groups)
    {
        if(group->type() == EElementType::SmoothGroup)
        {
            serializer.begin_write_array("elements");
            for(auto ele: group->elements())
            {
                if(ele->type() == EElementType::Line)
                {
                    serializer.begin_write_object();
                    auto line = ele->to_line();
                    serializer.write(0, "type");
                    serializer.write(line->line_prim->idx(), "id");
                    serializer.end_write_object();
                }
                else if(ele->type() == EElementType::Polygon)
                {
                    serializer.begin_write_object();
                    auto ring = ele->to_polygon()->ring;
                    serializer.write(1, "type");
                    serializer.write(ring->ring_prim->idx(), "id");
                    serializer.end_write_object();
                }
            }
            serializer.end_write_array();
        }
    }
    serializer.end_write_array();
}

void GeoData::load(SketchSerializer& serializer)
{
    nous::Array<nous::uint8> mesh_binary_buffer;
    serializer.read_binary(mesh_binary_buffer, "nous_mesh");
    m_nous_mesh->load_binary(mesh_binary_buffer);

    // points
    int num_points = m_nous_mesh->num_points();
    for(int i = 0; i < num_points; ++i)
    {
        nous::Point* point = m_nous_mesh->point(i);
        m_nousptr_points[point] = std::make_shared<SketchPoint>(point);
    }
    // lines&rings
    int num_prims = m_nous_mesh->num_prims();
    for(int i = 0; i < num_prims; ++i)
    {
        nous::Prim* current_prim = m_nous_mesh->prim(i);
        size_t lk = 0;
        if(is_line(current_prim))
        {
            nous::Vertex* prim_vertex = current_prim->vertex_ptr();
            nous::Point* p0 = m_nous_mesh->point(prim_vertex->point());
            nous::Point* p1 = nullptr;
            if(prim_vertex->next_vertex_ptr())
            {
                p1 = m_nous_mesh->point(prim_vertex->next_vertex_ptr()->point());
            }
            else
            {
                p1 = m_nous_mesh->point(prim_vertex->pre_vertex_ptr()->point());
            }
            lk = line_key(m_nousptr_points.at(p0), m_nousptr_points.at(p1));
            m_key_lines[lk] = std::make_shared<SketchLine>(current_prim, lk);
            m_key_lines[lk]->p0 = m_nousptr_points.at(p0);
            m_key_lines[lk]->p1 = m_nousptr_points.at(p1);
            m_nousptr_lines[current_prim] = m_key_lines[lk];

            // rtree
            nous::AABB3 line_box = GeoDataUtils::get().get_element_aabb3(this, m_key_lines[lk]);
            m_lines_tree->insert_object(m_key_lines[lk], line_box);
        }
        else
        {
            // key
            std::vector<SketchPointPtr> ring_points;
            for(auto vtx_idx: m_nous_mesh->prim_vertices_range(current_prim->idx()))
            {
                ring_points.push_back(m_nousptr_points.at(m_nous_mesh->vertex(vtx_idx)->point_ptr()));
            }
            size_t ring_key = GeoData::ring_key(ring_points);
            SketchRingPtr ring = std::make_shared<SketchRing>(current_prim, ring_key);
            // ring->normal = pair.second->normal;
            m_key_rings[ring_key] = ring;
            m_nousptr_rings[current_prim] = ring;

            // rtree
            nous::AABB3 ring_box = GeoDataUtils::get().get_element_aabb3(this, ring);
            m_ring_tree->insert_object(ring, ring_box);
        }
    }

    // polygons
    size_t polygon_count;
    serializer.begin_read_array("polygons", polygon_count);
    for(int i = 0; i < polygon_count; ++i)
    {
        serializer.begin_read_object();
        nous::vec3 normal;
        serializer.advance_read(normal, "normal");
        // ring
        nous::index_t ring_idx;
        serializer.read(ring_idx, "ring_idx");
        nous::Prim* ring_prim = m_nous_mesh->prim(ring_idx);
        SketchRingPtr ring = m_nousptr_rings.at(ring_prim);
        ring->normal = normal;
        SketchPolygonPtr polygon = std::make_shared<SketchPolygon>(ring);
        polygon->normal = ring->normal;
        // polygon map
        m_ring_adj_polygons[ring].polygon = polygon;
        // inner ring
        size_t inner_ring_count = 0;
        serializer.begin_read_array("inner_ring_idxs", inner_ring_count);
        for(int j = 0; j < inner_ring_count; ++j)
        {
            nous::index_t inner_ring_idx;
            serializer.read(inner_ring_idx);
            nous::Prim* inner_ring_prim = m_nous_mesh->prim(inner_ring_idx);
            SketchRingPtr inner_ring = m_nousptr_rings.at(inner_ring_prim);
            polygon->inner_rings.insert(inner_ring);
            // polygon map
            m_ring_adj_polygons[inner_ring].parent_polygon = polygon;
        }
        serializer.end_read_array();
        serializer.end_read_object();
    }
    serializer.end_read_array();

    // weld line group
    size_t weld_line_count;
    serializer.begin_read_array("no_branch_line_group", weld_line_count);
    for(int i = 0; i < weld_line_count; ++i)
    {
        ElementGroupPtr group = std::make_shared<NoBranchLineGroup>();
        m_ele_groups.insert(group);

        size_t line_count;
        serializer.begin_read_array("elements", line_count);
        for(int j = 0; j < line_count; ++j)
        {
            nous::index_t line_idx;
            serializer.read(line_idx);
            nous::Prim* line_prim = m_nous_mesh->prim(line_idx);
            SketchLinePtr line = m_nousptr_lines.at(line_prim);
            group->add_element(line);
        }
        serializer.end_read_array();
    }
    serializer.end_read_array();

    // smooth group
    size_t smooth_group_count;
    serializer.begin_read_array("smooth_group", smooth_group_count);
    for(int i = 0; i < smooth_group_count; ++i)
    {
        SmoothGroupPtr group = std::make_shared<SmoothGroup>();
        m_ele_groups.insert(group);

        size_t line_count;
        serializer.begin_read_array("elements", line_count);
        for(int j = 0; j < line_count; ++j)
        {
            serializer.begin_read_object();
            int type;
            serializer.read(type, "type");
            if(type == 0)
            {
                nous::index_t line_idx;
                serializer.read(line_idx, "id");
                nous::Prim* line_prim = m_nous_mesh->prim(line_idx);
                SketchLinePtr line = m_nousptr_lines.at(line_prim);
                group->add_element(line);
            }
            else if(type == 1)
            {
                nous::index_t polygon_ring_idx;
                serializer.read(polygon_ring_idx, "id");
                nous::Prim* ring_prim = m_nous_mesh->prim(polygon_ring_idx);
                SketchRingPtr ring = m_nousptr_rings.at(ring_prim);
                SketchPolygonPtr polygon = m_ring_adj_polygons.at(ring).polygon;
                group->add_element(polygon);
            }
            serializer.end_read_object();
        }
        serializer.end_read_array();
    }
    serializer.end_read_array();
}
}// namespace sketch