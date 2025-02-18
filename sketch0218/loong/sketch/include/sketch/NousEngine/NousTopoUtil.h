#pragma once

#include "nous_mesh/algorithm/intersect.h"
#include "nous_mesh/mesh.h"
#include <optional>
#include <vector>

namespace sketch
{

namespace util
{
/*
 * @brief 计算输入的环的第一个转角点的 index，如下图所示，在 p1 处，p1 p2 p5 共线，则该函数返回 p2 的位置
 * p5 ----- p1 ----- p2
 * |                  |
 * |                  |
 * p4----------------p3
 */
int calc_ring_first_corner_index(const std::vector<nous::pos_t>& ring);
};// namespace util

namespace segment
{
/*
 * @brief 计算两条线段是否近似完全一致
 * @param l1_sp 第一条线的起点坐标
 * @param l1_ep 第一条线的终点坐标
 * @param l2_sp 第二条线的起点坐标
 * @param l2_ep 第二条线的终点坐标
 * @param tolerance 判定点坐标相等的容差
 * @return 是否完全一致
 */
bool is_two_segment_nearly_equal(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                                 const double tolerance = 1e-5);

/*
 * @brief 计算两条线段是否平行且存在叠置
 * @param l1_sp 第一条线的起点坐标
 * @param l1_ep 第一条线的终点坐标
 * @param l2_sp 第二条线的起点坐标
 * @param l2_ep 第二条线的终点坐标
 * @return 是否存在重叠部分
 */
bool is_two_segments_overlap(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep);

/*
 * @brief 计算一条线段是否完全包含另一条线段
 * @param l1_sp 第一条线的起点坐标
 * @param l1_ep 第一条线的终点坐标
 * @param l2_sp 第二条线的起点坐标
 * @param l2_ep 第二条线的终点坐标
 * @return 0:不存在包含关系; 1:第一条线段包含第二条线段; 2:第二条线段包含第一条线段; 3: 两条线完全相等
 */
unsigned int is_two_segments_contained(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep);

/*
 * @brief 计算两条线段是否首尾相接
 * @param l1_sp 第一条线的起点坐标
 * @param l1_ep 第一条线的终点坐标
 * @param l2_sp 第二条线的起点坐标
 * @param l2_ep 第二条线的终点坐标
 * @param tolerance 判定点坐标相等的容差
 * @return 是否首尾相接
 */
bool is_two_segment_linked(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                           const double tolerance = 1e-5);

void two_segment_point_relative_percent(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                                        double& l1_sp_p, double& l1_ep_p, double& l2_sp_p, double& l2_ep_p);

};// namespace segment

namespace topo
{
/*
 * @brief 用于计算线沿目标方向的可移动限制距离区间
 */
struct SketchMoveLimitAloneDirection
{
    // 可移动方向
    nous::vec3 m_direction;
    // 方向逆向是否存在限制数值
    bool m_has_negative_limit = false;
    // 方向正向是否存在限制数值
    bool m_has_positive_limit = false;
    // 逆向限制数值
    double m_negative_distance_limit = std::numeric_limits<double>::max();
    // 正向限制数值
    double m_positive_distance_limit = std::numeric_limits<double>::max();
};

/*
 * @brief 计算一条指定边在一个多边形内延特定方向的可移动距离限制区间，若边在
 * @param polygon 多边形内外环点，第一个是外环，后面的都是内环
 * @param edge_spt_pos 传入的线段的起点坐标
 * @param edge_ept_pos 传入的线段的终点坐标
 * @param direction 线段的移动方向，线的移动方向一定与 polygon 的法线方向垂直
 * @return 若 polygon 非法或 direction 和 polygon 法线不垂直，无法获取计算结果，返回 nullopt，否则正常计算，返回 SketchMoveLimitAloneDirection 对象
 */
std::optional<SketchMoveLimitAloneDirection> calc_edge_move_distance_limit_alone_direction_in_polygon(
    const std::vector<std::vector<nous::pos_t>>& polygon, const nous::pos_t& edge_spt_pos, const nous::pos_t& edge_ept_pos,
    const nous::vec3& direction);

enum class ESketchTopoRelation : char
{
    Outside = -1,
    OnBoundary = 0,
    Inside = 1
};

/*
 * @brief 计算点和环的空间拓扑关系
 */
ESketchTopoRelation calc_point_and_ring_relation(const nous::vec3& normal, const nous::vec3& postion, const std::vector<nous::pos_t>& ring);

/*
 * @brief 将一个可能存在自相交的环在相交点处执行切割，在环内加入交点
 * @param ring 环坐标
 * @param 已知环内的一条线，在 move 模式下只会有一条线可移动与其余边产生重合，相交等情况
 * @param id_pos_map 坐标 id 与实际位置的映射表
 * @return 使用 id 表示的环，相同 id 表示相同的点
 */
std::vector<size_t> split_ring_at_cross_point_by_edge(const std::vector<nous::pos_t>& ring, const int edge_spt_idx,
                                                      std::vector<nous::pos_t>& id_pos_map);

std::vector<std::vector<nous::pos_t>> split_self_cross_ring(const std::vector<nous::pos_t>& ring, const int edge_spt_idx);

std::vector<size_t> convert_ring_position_to_relative_idx(const std::vector<nous::pos_t>& ring, std::vector<nous::pos_t>& id_pos_map);

std::vector<std::vector<size_t>> convert_polygon_position_to_relative_idx(const std::vector<std::vector<nous::pos_t>>& polygon,
                                                                          std::vector<nous::pos_t>& id_pos_map);

std::vector<std::vector<std::vector<nous::pos_t>>> rebuild_invalid_polygon_topo(std::vector<std::vector<nous::pos_t>>& polygon);

void fix_one_point_repeated_polygon(std::vector<std::vector<nous::pos_t>>& polygon);

std::vector<std::vector<nous::pos_t>> generate_offseted_position_by_normal(const std::vector<std::vector<nous::pos_t>>& positions,
                                                                           const nous::scalar value);

enum class EPointSetTopo : uint8_t
{
    Default = 0,
    Coline = 1,
    Coplane = 2
};

EPointSetTopo calc_point_set_topo(const std::vector<nous::pos_t>& point_set, nous::vec3& coplane_normal);

};// namespace topo

};// namespace sketch