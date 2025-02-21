#pragma once

#include "../../NousEngine/NousTopoUtil.h"
#include "./GeoData.h"
#include "geos/geom.h"

namespace sketch
{

class GeoDataUtils
{
public:
    static GeoDataUtils& get();
    nous::AABB3 get_aabb3(GeoData* geodata, const std::vector<nous::vec3>& points);
    nous::AABB3 get_aabb3(GeoData* geodata, const std::vector<SketchPointPtr>& points);
    nous::AABB3 get_element_aabb3(GeoData* geodata, SketchElementPtr element);

    void get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<SketchPointPtr>>& out_polygon);

    std::vector<std::vector<SketchPointPtr>> get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon);

    void get_polygon_topo_and_reorder_by_weld_info(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<SketchPointPtr>>& out_polygon);

    void get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<nous::pos_t>>& out_pos_arr);

    std::vector<std::vector<nous::pos_t>> get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon);

    void get_polygon_positions(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& polygon,
                               std::vector<std::vector<nous::pos_t>>& out_pos_arr);

    std::vector<SketchPointPtr> get_ring_topo(GeoData* geodata, SketchRingPtr ring);
    void get_ring_topo(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& out_topo);
    std::vector<nous::pos_t> get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring);
    void get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring, std::vector<nous::pos_t>& out_pos_arr);
    std::vector<nous::pos_t> get_ring_positions(GeoData* geodata, SketchRingPtr ring);
    void get_ring_positions(GeoData* geodata, const SketchRingPtr ring, std::vector<nous::pos_t>& out_pos_arr);

    bool is_two_ring_coplane(GeoData* geodata, const SketchRingPtr ring0, const SketchRingPtr ring1);

    bool is_ring_in_target_path(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                const std::vector<SketchPointPtr>& target_ring, std::vector<std::vector<SketchPointPtr>> result);

    bool is_ring_contain_target(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                const std::vector<SketchPointPtr>& target_ring);

    bool is_line_on_ring(GeoData* geodata, SketchLinePtr line, const std::vector<SketchPointPtr>& ring);

    bool is_point_on_ring(GeoData* geodata, SketchPointPtr, const std::vector<SketchPointPtr>& ring);

    void get_ring_relation_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, SketchRingPtr& contained,
                                 std::unordered_set<SketchRingPtr>& contains);
    void get_ring_relation_rings(GeoData* geodata, SketchRingPtr ring, SketchRingPtr contained, std::unordered_set<SketchRingPtr>& contains);

    /*
     * @brief 获取包含了全部 Line 的已闭合 Ring 对象
     */
    void get_prims_contains_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines, std::unordered_set<SketchRingPtr>& prim_set);
    void get_polygons_contained_by_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines,
                                             std::unordered_set<SketchPolygonPtr>& result);

    std::unordered_set<SketchLinePtr> get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon);
    void get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon, std::unordered_set<SketchLinePtr>& result);
    std::unordered_set<SketchPolygonPtr> get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring);
    void get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring, std::unordered_set<SketchPolygonPtr>& result);
    std::unordered_set<SketchPolygonPtr> get_line_adj_polygons(GeoData* geodata, SketchLinePtr line_key);
    void get_line_adj_polygons(GeoData* geodata, SketchLinePtr line, std::unordered_set<SketchPolygonPtr>& result);
    std::unordered_set<SketchPolygonPtr> get_point_adj_polygons(GeoData* geodata, SketchPointPtr point);
    void get_point_adj_polygons(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPolygonPtr>& result);
    std::unordered_set<SketchLinePtr> get_point_adj_lines(GeoData* geodata, SketchPointPtr point);
    void get_point_adj_lines(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchLinePtr>& result);

    std::vector<std::vector<size_t>> convert_polygon_to_rel_point_idx(GeoData* geodata, SketchPolygonPtr polygon,
                                                                      std::vector<SketchPointPtr>& id_point_map);

    // add
    // 检测目标多边形是否完全被另一个多边形包含
    bool is_polygon_contained(GeoData* geodata, const std::vector<std::vector<nous::vec3>>& target_polygon, SketchPolygonPtr container_polygon);
    // add

private:
    GeoDataUtils() = default;

    GeoDataUtils(GeoData&& data) = delete;

    GeoDataUtils(const GeoData& data) = delete;
};

class GeoDataTopoOperations
{
public:
    static GeoDataTopoOperations& get();

    void merge_polygon_triangle(GeoData* geodata, std::vector<SketchPolygonTriangle>& vp, std::vector<std::vector<std::vector<nous::vec3>>>& topos,
                                bool update_sketch_trangle = true);
    /*
     * @brief merge相邻且共面的环，需满足GeoData规范
     */
    SketchRingPtr merge_ring(GeoData* geodata, SketchRingPtr ring0, SketchRingPtr ring1, bool delete_org_rings = false);
    /*
     * @brief merge相邻且共面的多边形，需满足GeoData规范
     */
    void merge_polygon(GeoData* geodata, SketchPolygonPtr polygon0, SketchPolygonPtr polygon1, bool delete_org_poly = false);

    void verify_sketch_polygon(GeoData* geodata, SketchPolygonPtr polygon, const bool destory_topo = true);
    SketchPolygonPtr rebuild_sketch_polygon(GeoData* geodata, SketchRingPtr ring);
    void replace_polygon_equal_or_smaller(GeoData* geodata, SketchPolygonPtr polygon, const std::vector<std::vector<SketchPointPtr>>& replace_topos,
                                          std::vector<SketchRingPtr>& resultRings);
    void replace_polygon(GeoData* geodata, SketchPolygonPtr polygon, const std::vector<std::vector<std::vector<SketchPointPtr>>>& replace_topos);
    bool check_path_and_split_sketch_polygon(GeoData* geodata, SketchLinePtr line, const bool gen_polygon = true);
    void check_point_and_merge_line(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPointPtr>* limit_point_set = nullptr);
    void merge_line(GeoData* geodata, SketchPointPtr pre, SketchPointPtr center, SketchPointPtr next);
    void merge_coplane_ring_and_polygon(GeoData* geodata, std::vector<std::pair<SketchRingPtr, SketchRingPtr>>& coplane_rings,
                                        bool merge_adj_polygon);
    void retopo_ring(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& newTopo);
    void replace_line(GeoData* geodata, SketchLinePtr line, std::vector<SketchPointPtr>& new_line_points);
    void change_point_positon(GeoData* geodata, const std::vector<std::pair<SketchPointPtr, nous::vec3>>& new_pos);

    void copy_element_without_check(GeoData* geodata, GeoData* src_geodata, const std::unordered_set<SketchElementPtr>& elements);

    /*
     * @brief: 非必要不使用此方法，该方法添加的 polygon 不会互相影响 geodata 内部其余 polygon 的拓扑
     */
    std::vector<std::vector<SketchPointPtr>> add_polygon_without_split_line(GeoData* geodata, std::vector<std::vector<nous::pos_t>>& polygon_pos,
                                                                            bool* is_valid = nullptr);

    /*
     * 平滑组
     * 柔化边线相邻面，接受SketchLine对象与NoBranchLineGroup对象
     * 有效生成柔化面的焊接线会被拆分
     */
    void try_merge_smooth_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements, bool smooth_edge);
    /*
     * 焊接线
     * 以无分叉的路径尝试焊接，只接受SketchLine对象
     * try_merge_no_branch_line_group保证group内部的路径是无分叉的
     * try_merge_no_branch_line_group_2保证焊接线在整个GeoData中的路径是无分叉的
     */
    void try_merge_no_branch_line_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements,
                                        const std::unordered_set<SketchElementPtr>& force_separate_points);
    void try_merge_no_branch_line_group_2(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements);
    /*
     * 不做合法性检查
     * 如果有重合线分别属于两个 group，重合线分到哪一组属于不确定事件
     */
    ElementGroupPtr add_lines_to_no_branch_line_group(GeoData* geodata, const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                                      ElementGroupPtr group = nullptr);
    ElementGroupPtr add_lines_to_smooth_group(GeoData* geodata, const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                              ElementGroupPtr group = nullptr);
    /*
     * 不合法的组会被删除重建
     * 可以考虑优化为新增组模式
     */
    void check_and_recreate_no_branch_line_group(GeoData* geodata);
    /*
     * 不合法的组会被删除重建
     */
    void check_and_recreate_smooth_group(GeoData* geodata);

    // 尝试将一堆 WeldLine 中的 Line 对象进行排序
    // 返回结果中的 bool 用户存储每条 Line 的线序，语义表示为是否逆序，即 false = 正向, true = 逆向
    std::vector<std::pair<SketchLinePtr, bool>> try_order_weld_line_elements(const std::unordered_set<SketchElementPtr>& line_elements);

    std::vector<nous::pos_t> try_get_weld_line_position_array(const std::shared_ptr<GeoData> geodata,
                                                              const std::unordered_set<SketchElementPtr>& line_elements);

    // 携带 GeoData 可以基于 Topo 结构进行快速查询
    std::vector<std::pair<SketchLinePtr, bool>> try_order_weld_line_elements(const std::shared_ptr<GeoData> geodata,
                                                                             const std::unordered_set<SketchElementPtr>& line_elements);

private:
    GeoDataTopoOperations() = default;

    GeoDataTopoOperations(GeoData&& data) = delete;

    GeoDataTopoOperations(const GeoData& data) = delete;
};

namespace convert
{
geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map);

std::vector<std::vector<SketchPointPtr>> convert_geometry_to_polygon_with_revert_map(geos::geom::Polygon* polygon,
                                                                                     std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map);

geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, SketchPolygonPtr polygon,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map,
                                                                  const nous::vec3* ref_normal = nullptr);

geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<SketchPointPtr> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map);
};// namespace convert

namespace geodata_topo
{
/*
 * @brief 计算一个 ring 与其余多个 ring 的合并部分
 */
bool polygon_union(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon0, const std::vector<std::vector<SketchPointPtr>> polygon1,
                   const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result);

bool is_polygon_valid(GeoData* geodata, SketchPolygonPtr polygon);

/*
 * @brief 计算一个 ring 与其余多个 ring 的不重合部分
 */
bool ring_difference_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                                 const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result);

/*
 * @brief 计算一个 ring 与其余多个 ring 的合并部分
 */
bool ring_union_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                            const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result);

topo::ESketchTopoRelation point_ring_relation(GeoData* geodata, const nous::pos_t& point, const std::vector<SketchPointPtr>& ring,
                                              const nous::vec3& normal);

/*
 * 基于输入的 inputpolygon，计算 geodata 中与其共面的 polygon 的拓扑关系
 * 得到新的 polygon 中与其余 polygon 重叠的部分，以及重叠计算完毕后剩余的部分
 */
bool split_intersect_polygons(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& input_polygon, const nous::vec3 input_pg_normal,
                              const std::unordered_set<SketchPolygonPtr>& coplane_polygons,
                              std::map<SketchPolygonPtr, std::vector<std::vector<std::vector<SketchPointPtr>>>>& split_pieces,
                              std::vector<std::vector<std::vector<SketchPointPtr>>>& remaining_pieces);

std::vector<std::vector<nous::vec3>> triangulate_polygon(std::vector<std::vector<nous::vec3>>& polygon, bool reverse_triangle_order = false);

void triangulate_polygon_cdt(GeoData* geodata, SketchPolygonPtr polygon);

};// namespace geodata_topo

};// namespace sketch