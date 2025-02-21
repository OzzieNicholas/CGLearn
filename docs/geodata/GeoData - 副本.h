#pragma once

#include <string>

#include "./Element.h"
#include "./IRenderable.h"
#include "./SmoothGroup.h"

#include "../../Object/SketchObject.h"
#include "nous_mesh/algorithm/query.h"
#include "nous_mesh/algorithm/triangulate.h"

namespace sketch
{
//
struct RingPolygonRelation
{
    // 定义一个指向当前多边形的指针，类型为 SketchPolygonPtr，初始值为 nullptr
    SketchPolygonPtr polygon = nullptr;
    
    // 定义一个指向父多边形的指针，类型为 SketchPolygonPtr，初始值为 nullptr
    SketchPolygonPtr parent_polygon = nullptr;
};
//
class GeoData : public SketchObject, public IRenderable
{
public:
    friend class GeoDataUtils;
    friend class GeoDataTopoOperations;

public:
    GeoData();

    // 定义拷贝构造函数，用于创建一个现有对象的副本
    GeoData(const GeoData& other);

    // 定义虚析构函数，确保派生类对象正确释放资源
    virtual ~GeoData();

    // 定义虚函数 dispose，用于释放资源
    virtual void dispose();

    // 定义内联函数 dirty，设置脏标志位
    inline void dirty()
    {
        // 设置第一个脏标志位
        m_dirty_flags |= 0x1;
        // 设置第二个脏标志位
        m_dirty_flags |= 0x2;
    }

    // 定义虚函数 save，用于保存数据到序列化器
    virtual void save(SketchSerializer& serializer) override;

    // 定义虚函数 load，用于从序列化器加载数据
    virtual void load(SketchSerializer& serializer) override;

    // 定义内联函数 update_render_mesh，用于更新渲染网格
    inline bool update_render_mesh()
    {
        // 检查第一个脏标志位是否被设置
        if(m_dirty_flags & 0x1)
        {
            // 更新渲染资源
            this->_update_render_resource();
            // 清除第一个脏标志位
            m_dirty_flags &= ~0x1;
            return true;
        }
        return false;
    }

    void clear()
    {
        // 调用基类的 clear 方法
        IRenderable::clear();
        // 设置脏标志位
        this->dirty();
        // 清除 m_nous_mesh 对象
        m_nous_mesh->clear();
        // 清除 m_key_lines 容器
        m_key_lines.clear();
        // 清除 m_key_rings 容器
        m_key_rings.clear();
        // 清除 m_ring_adj_polygons 容器
        m_ring_adj_polygons.clear();
        // 同步 m_nous 对象
        this->_sync_nous();
    }

    void mark_all_sketch_polygon_dirty()
    {
        // 获取所有多边形的集合
        std::unordered_set<SketchPolygonPtr>&& all_polygons = get_all_polygons();

        // 遍历所有多边形，并将它们的 m_changed 标志设置为 true
        for(auto& polygon: all_polygons)
        {
            polygon->m_changed = true;
        }
    }

    inline void mark_ring_polygon_changed(SketchRingPtr ring)
    {
        // 检查 ring 是否为空
        if(ring)
        {
            // 检查 ring 对应的父多边形是否存在
            if(m_ring_adj_polygons[ring].parent_polygon)
            {
                // 将父多边形的 m_changed 标志设置为 true
                m_ring_adj_polygons[ring].parent_polygon->m_changed = true;
            }
        }
    }

    // 内联函数，设置渲染网格的脏标志位
    inline void mark_render_mesh_as_dirty() { m_dirty_flags |= 0x1; }

    // 内联函数，设置网格的脏标志位
    inline void mark_mesh_as_dirty() { m_dirty_flags |= 0x2; }

    // 内联函数，返回指向 m_nous_mesh 对象的指针
    inline nous::NMesh* get_mesh() { return m_nous_mesh.get(); }

    // 内联函数，返回 m_nous_position_handle 的引用
    inline const nous::AttributeRWHandle<nous::vec3>& get_point_position_handle() { return m_nous_position_handle; }

    // 内联函数，返回 m_key_lines 容器中线条的数量
    inline size_t get_line_number() { return m_key_lines.size(); }

    // 内联函数，返回 m_key_lines 容器的引用
    inline const std::unordered_map<size_t, SketchLinePtr>& get_all_lines() { return m_key_lines; }

    // 内联函数，根据键返回对应的 SketchLinePtr
    inline SketchLinePtr get_line(size_t key) { return m_key_lines.at(key); }

    // 内联函数，返回 m_nous_mesh 中点的数量
    inline size_t get_point_number() { return m_nous_mesh->num_points(); }

    // 内联函数，根据索引返回对应的 SketchPointPtr
    inline SketchPointPtr get_point(nous::index_t index) { return m_nousptr_points.at(m_nous_mesh->point(index)); }

    // 内联函数，根据环返回对应的 SketchPolygonPtr
    inline const SketchPolygonPtr get_polygon(SketchRingPtr ring) { return m_ring_adj_polygons.at(ring).polygon; }

    // 函数声明，返回所有多边形的集合
    std::unordered_set<SketchPolygonPtr> get_all_polygons();

    // 函数，返回指定元素的所有点的指针
    std::vector<SketchPointPtr> get_element_points(SketchElementPtr ele)
    {
        std::vector<SketchPointPtr> points;
        this->get_element_points(ele, points);
        return points;
    }

    // 函数，将指定元素的所有点的指针添加到给定的向量中
    void get_element_points(SketchElementPtr ele, std::vector<SketchPointPtr>& points)
    {
        // 同步 m_nous 对象
        this->_sync_nous();
        // 检查元素类型是否为 Ring
        if(ele->type() == sketch::EElementType::Ring)
        {
            // 将元素转换为 Ring 类型
            auto ring = ele->to_ring();
            // 遍历环的顶点，并将对应的点的指针添加到 points 向量中
            for(auto prim_vert: m_nous_mesh->prim_vertices_range(ring->ring_prim->idx()))
            {
                points.push_back(m_nousptr_points.at(m_nous_mesh->vertex(prim_vert)->point_ptr()));
            }
        }
    }

    // 内联函数，检查指定元素是否有效
    inline bool is_valid(SketchElementPtr ele)
    {
        if(ele->type() == EElementType::Ring)
        {
            // 检查环是否存在于 m_ring_adj_polygons 中
            return m_ring_adj_polygons.find(ele->to_ring()) != m_ring_adj_polygons.end();
        }
        else if(ele->type() == EElementType::Point)
        {
            // 检查点的索引是否在有效范围内
            return ele->to_point()->point->idx() >= 0 && ele->to_point()->point->idx() < m_nous_mesh->num_points();
        }
        else if(ele->type() == EElementType::Line)
        {
            // 检查线是否存在于 m_key_lines 中
            return m_key_lines.find(ele->to_line()->key) != m_key_lines.end();
        }
        else if(ele->type() == EElementType::Polygon) // 不稳定
        {
            // 检查多边形的环是否存在于 m_ring_adj_polygons 中
            return m_ring_adj_polygons.find(ele->to_polygon()->ring) != m_ring_adj_polygons.end();
        }
        else if(ele->type() == EElementType::NoBranchLineGroup || ele->type() == EElementType::SmoothGroup)
        {
            // 检查组是否存在于 m_ele_groups 中
            return m_ele_groups.find(ele->to_group()) != m_ele_groups.end();
        }
        return false;
    }

    // 函数声明，检查指定的 prim 是否为线
    bool is_line(const nous::Prim* prim) const;

    // 内联函数，设置指定环的父多边形
    inline void set_ring_parent_polygon(SketchRingPtr ring, SketchPolygonPtr polygon) { m_ring_adj_polygons[ring].parent_polygon = polygon; }

    // 内联函数，设置指定环的多边形
    inline void set_ring_polygon(SketchRingPtr ring, SketchPolygonPtr polygon) { m_ring_adj_polygons[ring].polygon = polygon; }

    /*
     * @brief 获取包含这条 Line 的所有已闭合 Ring 对象
     */
    std::unordered_set<SketchRingPtr> get_line_adj_rings(SketchLinePtr line);
    /*
     * @brief 获取 Ring 对象持有的全部 Line 对象
     */
    std::unordered_set<SketchLinePtr> get_ring_adj_lines(SketchRingPtr ring);
    /*
     * @brief 获取 Point 的全部邻接 Point
     */
    std::unordered_set<SketchPointPtr> get_point_adj_points(SketchPointPtr point);
    void get_point_adj_rings(SketchPointPtr point, std::unordered_set<SketchRingPtr>& adj_rings)
    {
        for(auto i: m_nous_mesh->point_prims_range(point->point->idx()))
        {
            if(!is_line(m_nous_mesh->prim(i)))
            {
                adj_rings.insert(m_nousptr_rings.at(m_nous_mesh->prim(i)));
            }
        }
    }
    std::unordered_set<SketchRingPtr> get_point_adj_rings(SketchPointPtr point)
    {
        std::unordered_set<SketchRingPtr> rings;
        this->get_point_adj_rings(point, rings);
        return rings;
    }
    void get_point_adj_lines(SketchPointPtr point, std::unordered_set<SketchLinePtr>& adj_lines)
    {
        for(auto np: get_point_adj_points(point))
        {
            adj_lines.insert(m_key_lines.at(GeoData::line_key(point, np)));
        }
    }
    std::unordered_set<SketchLinePtr> get_point_adj_lines(SketchPointPtr point)
    {
        std::unordered_set<SketchLinePtr> lines;
        this->get_point_adj_lines(point, lines);
        return lines;
    }

    inline SketchPolygonPtr get_parent_polygon(SketchRingPtr ring)
    {
        if(ring && m_ring_adj_polygons.count(ring))
        {
            return m_ring_adj_polygons.at(ring).parent_polygon;
        }
        return nullptr;
    }
    inline std::unordered_map<SketchRingPtr, RingPolygonRelation> get_ring_relation_polygons() { return m_ring_adj_polygons; }
    inline nous::AABB3 get_nous_aabb() { return m_nous_mesh->bbox(); }

    SketchPointPtr add_point(const nous::vec3& pos);
    SketchPointPtr add_point_without_check(const nous::vec3& pos);
    std::vector<SketchLinePtr> add_line(const nous::vec3& p0, const nous::vec3& p1, bool genPolygon = true);
    SketchLinePtr add_line_without_check(SketchPointPtr p0, SketchPointPtr p1);
    void delete_line(SketchLinePtr line, bool merge_adj_line = true, bool merge_adj_polygon = true,
                     std::unordered_set<SketchPointPtr>* force_keeped_point_set = nullptr);
    void delete_lines(const std::unordered_set<SketchLinePtr>& lines, bool mergeAdjLine = true, bool mergeAdjRing = true);

    /*
     * @brief 将一个用内外环坐标数组表示的 polygon 尝试加入 GeoData 对象
     * @param polygon_pos 多边形的内外环坐标数组，第一个表示外环，其余表示内环
     * @return polygon 的坐标加入 GeoData 后在其中对应的 Point*
     */
    std::vector<std::vector<SketchPointPtr>> add_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_pos);
    void add_polygon(const std::vector<std::vector<SketchPointPtr>>& polygon_topo, nous::vec3 normal);
    std::vector<std::vector<SketchPointPtr>> add_polygon_without_check(const std::vector<std::vector<nous::pos_t>>& polygon_pos,
                                                                       bool add_line = false);
    SketchPolygonPtr add_polygon_without_check(const std::vector<std::vector<SketchPointPtr>>& polygon, bool add_line = false);
    void delete_polygon(SketchPolygonPtr polygon, bool need_delete_line = false,
                        std::unordered_set<SketchPointPtr>* force_keeped_point_set = nullptr);

    void add_element_group(ElementGroupPtr group);
    void delete_element_group(ElementGroupPtr group);
    inline std::unordered_set<ElementGroupPtr> get_element_groups() { return m_ele_groups; }

    inline nous::vec3 get_point_position(SketchPointPtr p)
    {
        this->_sync_nous();
        return m_nous_position_handle.get(p->point->idx());
    }
    inline nous::vec3 get_point_position(nous::Point* p)
    {
        this->_sync_nous();
        return m_nous_position_handle.get(p->idx());
    }

    inline void set_point_position(SketchPointPtr point, const nous::vec3& pos)
    {
        this->_sync_nous();
        m_nous_position_handle.set(point->point->idx(), pos);
        m_dirty_flags |= 0x1;
    }

    template<typename T> inline void set_prim_attribute(nous::Prim* prim, std::string name, T val)
    {
        auto attr = m_nous_mesh->get_or_create_prim_attrib<T>(name.c_str(), T());
        auto attr_handle = attr->write();
        attr_handle.set(prim->idx(), val);
    }

    template<typename T> inline T get_prim_attribute(nous::Prim* prim, std::string name)
    {
        auto attr = m_nous_mesh->prim_attrib<T>(name.c_str());
        if(attr)
        {
            auto attr_handle = attr->read();
            return attr_handle.get(prim->idx());
        }
        return T();
    }

protected:
    inline void _sync_nous()
    {
        if(m_dirty_flags & 0x2)
        {
            m_nous_position_handle = nous::AttributeRWHandle<nous::vec3>(m_nous_mesh->point_attrib<nous::pos_t>(nous::ATTRIB_NAME_POS));
            m_dirty_flags &= ~0x2;
        }
    }

    /*
     * @param lp0 线起点坐标
     * @param lp1 线终点坐标
     * @param recreatLine 与这条线相交的其余线的分割结果
     * @param newLine 这条线的分割结果
     */
    void _add_line(const nous::vec3& lp0, const nous::vec3& lp1, std::map<SketchLinePtr, std::vector<SketchPointPtr>>& recreat_lines,
                   std::vector<SketchPointPtr>& new_line_points);
    SketchLinePtr _add_nous_line(SketchPointPtr p0, SketchPointPtr p1);
    void _delete_nous_line(SketchLinePtr line);

    SketchRingPtr _add_nous_ring(const std::vector<SketchPointPtr>& topo);
    /*
     * delete ring会删除引用该ring的polygon
     * 保持父子结构正确，将子环继承给parent polygon
     * 递归删除该polygon的子环
     */
    void _delete_nous_ring(SketchRingPtr ring);
    void _find_coplain_rings(SketchPointPtr current, SketchPointPtr end, std::vector<SketchPointPtr>& path,
                             std::unordered_set<SketchPointPtr>& visited, std::vector<nous::vec3>& normal, std::vector<nous::vec3>& dir,
                             uint8_t pathMode, std::vector<std::pair<nous::vec3, std::vector<std::vector<SketchPointPtr>>>>& result);
    void _find_split_ring_path(SketchPointPtr cpos, SketchRingPtr ring, std::unordered_set<SketchPointPtr>& visited, const nous::vec3& normal,
                               std::vector<SketchPointPtr>& path, std::vector<SketchPointPtr>& result);
    SketchRingPtr _check_ring_affilia_and_create_polygon(const std::vector<SketchPointPtr>& ring, const bool gen_polygon = true,
                                                         const bool verify_parent = false);

    SketchPolygonPtr _create_polygon(const std::vector<std::vector<SketchPointPtr>>& ring);
    SketchPolygonPtr _create_polygon(SketchRingPtr ring, const std::unordered_set<SketchRingPtr>& inner_ring);
    /*
     * delete polygon释放polygon对内外环的引用，然后尝试删除无其他引用的内环，再删除外环
     */
    void _delete_polygon(SketchPolygonPtr polygon);

    void _trangulate_sketch_polygon(SketchPolygonPtr vp);
    bool _update_render_resource();

public:
    inline static size_t line_key(SketchPointPtr p0, SketchPointPtr p1)
    {
        std::hash<nous::Point*> hasher;
        size_t hash0 = hasher(p0->point);
        size_t hash1 = hasher(p1->point);
        if(hash0 > hash1)
        {
            std::swap(hash0, hash1);
        }
        return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
    }

    inline static size_t ring_key(const std::vector<SketchPointPtr>& ring)
    {
        std::hash<nous::Point*> hasher;
        std::vector<size_t> hashes;
        for(auto p: ring)
        {
            hashes.push_back(hasher(p->point));
        }
        std::sort(hashes.begin(), hashes.end());
        size_t combinedHash = 0;
        for(size_t hash: hashes)
        {
            combinedHash ^= hash + 0x9e3779b9 + (combinedHash << 6) + (combinedHash >> 2);
        }
        return combinedHash;
    }

protected:
    uint32_t m_dirty_flags;
    // nous
    nous::NMeshPtr m_nous_mesh;
    nous::AttributeRWHandle<nous::vec3> m_nous_position_handle;

    std::unordered_map<size_t, SketchLinePtr> m_key_lines;
    std::unordered_map<size_t, SketchRingPtr> m_key_rings;

    std::unordered_map<SketchRingPtr, RingPolygonRelation> m_ring_adj_polygons;

    // nous ptr to sketch ptr
    std::unordered_map<nous::Point*, SketchPointPtr> m_nousptr_points;
    std::unordered_map<nous::Prim*, SketchLinePtr> m_nousptr_lines;
    std::unordered_map<nous::Prim*, SketchRingPtr> m_nousptr_rings;

    std::unordered_set<ElementGroupPtr> m_ele_groups;
};
}// namespace sketch
