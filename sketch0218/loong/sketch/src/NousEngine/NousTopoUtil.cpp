#include "include/sketch/NousEngine/NousTopoUtil.h"

#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/SketchMath.h"
#include <set>
#include <unordered_set>

namespace sketch
{
namespace segment
{
bool is_two_segment_nearly_equal(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                                 const double tolerance)
{
    if((l1_sp - l2_sp).norm() < tolerance && (l1_ep - l2_ep).norm() < tolerance)
    {
        return true;
    }

    if((l1_sp - l2_ep).norm() < tolerance && (l1_ep - l2_sp).norm() < tolerance)
    {
        return true;
    }

    return false;
}

bool is_two_segments_overlap(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep)
{
    if(is_two_segment_nearly_equal(l1_sp, l1_ep, l2_sp, l2_ep, 1e-3))
    {
        return true;
    }

    const nous::vec3 l1_dir = (l1_ep - l1_sp).normalized();
    const nous::vec3 l2_dir = (l2_ep - l2_sp).normalized();
    if(!sketch_math::direction_parallel(l1_dir, l2_dir))
    {
        return false;
    }

    nous::vec3 link_dir = (l2_sp - l1_sp).normalized();
    if(link_dir.norm() < 1e-5)
    {
        // 防止共点，计算出 link_dir = (0, 0, 0)
        link_dir = (l2_ep - l1_sp).normalized();
    }

    if(!sketch_math::direction_parallel(l1_dir, link_dir))
    {
        return false;
    }

    const double l1_sp_t = 0.0;
    const double l1_ep_t = (l1_ep - l1_sp).norm() * (l1_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    double l2_sp_t = (l2_sp - l1_sp).norm() * (l2_sp - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    double l2_ep_t = (l2_ep - l1_sp).norm() * (l2_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());

    if(l2_sp_t > l2_ep_t)
    {
        const double tmp_v = l2_sp_t;
        l2_sp_t = l2_ep_t;
        l2_ep_t = tmp_v;
    }

    if(l2_sp_t > l1_sp_t + 1e-3 && l2_sp_t < l1_ep_t - 1e-3)
    {
        return true;
    }

    if(l2_ep_t > l1_sp_t + 1e-3 && l2_ep_t < l1_ep_t - 1e-3)
    {
        return true;
    }

    if(l1_sp_t > l2_sp_t + 1e-3 && l1_sp_t < l2_ep_t - 1e-3)
    {
        return true;
    }

    if(l1_ep_t > l2_sp_t + 1e-3 && l1_ep_t < l2_ep_t - 1e-3)
    {
        return true;
    }

    return false;
}

unsigned int is_two_segments_contained(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep)
{
    if(is_two_segment_nearly_equal(l1_sp, l1_ep, l2_sp, l2_ep))
    {
        return 3;
    }

    const nous::vec3 l1_dir = (l1_ep - l1_sp).normalized();
    const nous::vec3 l2_dir = (l2_ep - l2_sp).normalized();
    if(!sketch_math::direction_parallel(l1_dir, l2_dir))
    {
        return false;
    }

    const nous::vec3 link_dir = (l2_sp - l1_sp).normalized();
    if(!sketch_math::direction_parallel(l1_dir, link_dir))
    {
        return false;
    }

    const double l1_sp_t = 0.0;
    const double l1_ep_t = (l1_ep - l1_sp).norm() * (l1_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    double l2_sp_t = (l2_sp - l1_sp).norm() * (l2_sp - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    double l2_ep_t = (l2_ep - l1_sp).norm() * (l2_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());

    if(l2_sp_t > l2_ep_t)
    {
        const double tmp_v = l2_sp_t;
        l2_sp_t = l2_ep_t;
        l2_ep_t = tmp_v;
    }

    if((l2_sp_t <= l1_sp_t && l2_ep_t >= l1_ep_t) || (l2_ep_t <= l1_sp_t && l2_sp_t >= l1_ep_t))
    {
        return 2;
    }

    if((l1_sp_t <= l2_sp_t && l1_ep_t >= l2_ep_t) || (l1_ep_t <= l2_sp_t && l1_sp_t >= l2_ep_t))
    {
        return 1;
    }

    return 0;
}

bool is_two_segment_linked(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                           const double tolerance)
{
    if((l1_sp - l2_sp).norm() < 1e-5 || (l1_sp - l2_ep).norm() < 1e-5 || (l1_ep - l2_sp).norm() < 1e-5 || (l1_ep - l2_ep).norm() < 1e-5)
    {
        return true;
    }
    return false;
}

void two_segment_point_relative_percent(const nous::pos_t& l1_sp, const nous::pos_t& l1_ep, const nous::pos_t& l2_sp, const nous::pos_t& l2_ep,
                                        double& l1_sp_p, double& l1_ep_p, double& l2_sp_p, double& l2_ep_p)
{
    l1_sp_p = 0.0;
    l1_ep_p = (l1_ep - l1_sp).norm() * (l1_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    l2_sp_p = (l2_sp - l1_sp).norm() * (l2_sp - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
    l2_ep_p = (l2_ep - l1_sp).norm() * (l2_ep - l1_sp).normalized().dot((l1_ep - l1_sp).normalized());
}
};// namespace segment

namespace topo
{
std::optional<SketchMoveLimitAloneDirection> calc_edge_move_distance_limit_alone_direction_in_polygon(
    const std::vector<std::vector<nous::pos_t>>& polygon, const nous::pos_t& edge_spt_pos, const nous::pos_t& edge_ept_pos,
    const nous::vec3& direction)
{
    SketchMoveLimitAloneDirection limit_result;
    limit_result.m_direction = direction;

    // polygon 为空或外环点数不足，无法构成面
    if(polygon.empty() || polygon.front().size() < 3)
    {
        return std::nullopt;
    }

    // 不垂直则跳过，说明 edge 不在该面上移动
    const nous::vec3 polygon_normal = sketch_math::ring_normal(polygon.front()).normalized();
    const double dot_v = polygon_normal.dot(direction.normalized());
    if(dot_v < -1e-5 || dot_v > 1e-5)
    {
        return std::nullopt;
    }

    // 计算线与线的关系
    for(const auto& ring: polygon)
    {
        for(int i = 0; i < ring.size(); ++i)
        {
            const nous::pos_t ring_spt_pos = ring[i];
            const nous::pos_t ring_ept_pos = ring[(i + 1) % ring.size()];

            // 如果输入的线和 polygon 的环上的某线段平行且重合，跳过
            if(segment::is_two_segments_contained(edge_spt_pos, edge_ept_pos, ring_spt_pos, ring_ept_pos))
            {
                continue;
            }

            // 如果输入的线和 polygon 的环上的某线段有共点，跳过
            // 排除和这条线直接相连的那些线，这种情况需要用另外的逻辑判断
            if(segment::is_two_segment_linked(ring_spt_pos, ring_ept_pos, edge_spt_pos, edge_ept_pos))
            {
                continue;
            }

            const nous::pos_t ring_spt_offset = ring_spt_pos + direction;
            const nous::pos_t ring_ept_offset = ring_ept_pos + direction;
            const nous::pos_t edge_spt_offset = edge_spt_pos + direction;
            const nous::pos_t edge_ept_offset = edge_ept_pos + direction;

            // 要尝试相交四次
            {
                // 从 edge 的端点向 polygon 的边构造直线求解相交
                nous::vec3 cross_pt_a, cross_pt_b;
                const bool is_intersect1 =
                    nous::algo::line_intersect_3d(edge_spt_pos, edge_spt_offset, ring_spt_pos, ring_ept_pos, cross_pt_a, cross_pt_b, false);
                nous::vec3 cross_pt_c, cross_pt_d;
                const bool is_intersect2 =
                    nous::algo::line_intersect_3d(edge_ept_pos, edge_ept_offset, ring_spt_pos, ring_ept_pos, cross_pt_c, cross_pt_d, false);

                const bool is_nearly_equal = segment::is_two_segment_nearly_equal(ring_spt_pos, ring_ept_pos, cross_pt_a, cross_pt_c);
                const bool is_linked = segment::is_two_segment_linked(ring_spt_pos, ring_ept_pos, cross_pt_a, cross_pt_c, 1e-3);
                const bool is_overlap = segment::is_two_segments_overlap(ring_spt_pos, ring_ept_pos, cross_pt_a, cross_pt_c);
                if(is_nearly_equal || (is_linked && !is_overlap))
                {
                    continue;
                }

                const double valid_dist_limit = (ring_ept_pos - ring_spt_pos).norm();
                if(is_intersect1 && (cross_pt_a - cross_pt_b).norm() < 1e-3)
                {
                    const double cross_to_ori_dist = (cross_pt_a - ring_spt_pos).norm() *
                                                     ((cross_pt_a - ring_spt_pos).normalized().dot((ring_ept_pos - ring_spt_pos).normalized()));

                    // 从 edge 绘制的直线与 ring 的线段内部存在有效交点
                    if(cross_to_ori_dist <= valid_dist_limit && cross_to_ori_dist >= 0)
                    {
                        const double valid_move_dist =
                            (cross_pt_a - edge_spt_pos).norm() * (cross_pt_a - edge_spt_pos).normalized().dot(direction.normalized());
                        if(valid_move_dist < 0)
                        {
                            limit_result.m_has_negative_limit = true;
                            limit_result.m_negative_distance_limit = std::min(limit_result.m_negative_distance_limit, fabs(valid_move_dist));
                        }
                        else
                        {
                            limit_result.m_has_positive_limit = true;
                            limit_result.m_positive_distance_limit = std::min(limit_result.m_positive_distance_limit, fabs(valid_move_dist));
                        }
                    }
                }

                if(is_intersect2 && (cross_pt_c - cross_pt_d).norm() < 1e-3)
                {
                    const double cross_to_ori_dist = (cross_pt_c - ring_spt_pos).norm() *
                                                     ((cross_pt_c - ring_spt_pos).normalized().dot((ring_ept_pos - ring_spt_pos).normalized()));

                    // 从 edge 绘制的直线与 ring 的线段内部存在有效交点
                    if(cross_to_ori_dist <= valid_dist_limit && cross_to_ori_dist >= 0)
                    {
                        const double valid_move_dist =
                            (cross_pt_c - edge_ept_pos).norm() * (cross_pt_c - edge_ept_pos).normalized().dot(direction.normalized());
                        if(valid_move_dist < 0)
                        {
                            limit_result.m_has_negative_limit = true;
                            limit_result.m_negative_distance_limit = std::min(limit_result.m_negative_distance_limit, fabs(valid_move_dist));
                        }
                        else
                        {
                            limit_result.m_has_positive_limit = true;
                            limit_result.m_positive_distance_limit = std::min(limit_result.m_positive_distance_limit, fabs(valid_move_dist));
                        }
                    }
                }
            }

            {
                // 从 polygon 边的端点向 edge 构造直线求解相交
                nous::vec3 cross_pt_e, cross_pt_f;
                const bool is_intersect3 =
                    nous::algo::line_intersect_3d(ring_spt_pos, ring_spt_offset, edge_spt_pos, edge_ept_pos, cross_pt_e, cross_pt_f, false);
                nous::vec3 cross_pt_g, cross_pt_h;
                const bool is_intersect4 =
                    nous::algo::line_intersect_3d(ring_ept_pos, ring_ept_offset, edge_spt_pos, edge_ept_pos, cross_pt_g, cross_pt_h, false);

                const double valid_dist_limit = (edge_ept_pos - edge_spt_pos).norm();

                const bool is_nearly_equal = segment::is_two_segment_nearly_equal(edge_spt_pos, edge_ept_pos, cross_pt_e, cross_pt_g);
                const bool is_linked = segment::is_two_segment_linked(edge_spt_pos, edge_ept_pos, cross_pt_e, cross_pt_g, 1e-3);
                const bool is_overlap = segment::is_two_segments_overlap(edge_spt_pos, edge_ept_pos, cross_pt_e, cross_pt_g);
                if(is_nearly_equal || (is_linked && !is_overlap))
                {
                    continue;
                }

                if(is_intersect3 && (cross_pt_e - cross_pt_f).norm() < 1e-3)
                {
                    const double cross_to_ori_dist = (cross_pt_e - edge_spt_pos).norm() *
                                                     ((cross_pt_e - edge_spt_pos).normalized().dot((edge_ept_pos - edge_spt_pos).normalized()));

                    // 从 edge 绘制的直线与 ring 的线段内部存在有效交点
                    if(cross_to_ori_dist <= valid_dist_limit && cross_to_ori_dist >= 0)
                    {
                        const double valid_move_dist =
                            (ring_spt_pos - cross_pt_e).norm() * (ring_spt_pos - cross_pt_e).normalized().dot(direction.normalized());
                        if(valid_move_dist < 0)
                        {
                            limit_result.m_has_negative_limit = true;
                            limit_result.m_negative_distance_limit = std::min(limit_result.m_negative_distance_limit, fabs(valid_move_dist));
                        }
                        else
                        {
                            limit_result.m_has_positive_limit = true;
                            limit_result.m_positive_distance_limit = std::min(limit_result.m_positive_distance_limit, fabs(valid_move_dist));
                        }
                    }
                }

                if(is_intersect4 && (cross_pt_g - cross_pt_h).norm() < 1e-3)
                {
                    const double cross_to_ori_dist = (cross_pt_g - edge_spt_pos).norm() *
                                                     ((cross_pt_g - edge_spt_pos).normalized().dot((edge_ept_pos - edge_spt_pos).normalized()));

                    // 从 edge 绘制的直线与 ring 的线段内部存在有效交点
                    if(cross_to_ori_dist <= valid_dist_limit && cross_to_ori_dist >= 0)
                    {
                        const double valid_move_dist =
                            (ring_ept_pos - cross_pt_g).norm() * (ring_ept_pos - cross_pt_g).normalized().dot(direction.normalized());
                        if(valid_move_dist < 0)
                        {
                            limit_result.m_has_negative_limit = true;
                            limit_result.m_negative_distance_limit = std::min(limit_result.m_negative_distance_limit, fabs(valid_move_dist));
                        }
                        else
                        {
                            limit_result.m_has_positive_limit = true;
                            limit_result.m_positive_distance_limit = std::min(limit_result.m_positive_distance_limit, fabs(valid_move_dist));
                        }
                    }
                }
            }
        }
    }

    return limit_result;
}

ESketchTopoRelation calc_point_and_ring_relation(const nous::vec3& normal, const nous::vec3& postion, const std::vector<nous::pos_t>& ring)
{
    const float absNx = std::abs(normal.x());
    const float absNy = std::abs(normal.y());
    const float absNz = std::abs(normal.z());
    const int dominantAxis = (absNx > absNy) ? ((absNx > absNz) ? 0 : 2) : ((absNy > absNz) ? 1 : 2);

    int count = 0;
    for(size_t i = 0; i < ring.size(); i++)
    {
        nous::vec3 aPos = ring[i];
        nous::vec3 bPos = ring[(i + 1) % ring.size()];

        float aCoord1, aCoord2, bCoord1, bCoord2, pCoord1, pCoord2;
        if(dominantAxis == 0)
        {
            aCoord1 = aPos.y();
            aCoord2 = aPos.z();
            bCoord1 = bPos.y();
            bCoord2 = bPos.z();
            pCoord1 = postion.y();
            pCoord2 = postion.z();
        }
        else if(dominantAxis == 1)
        {
            aCoord1 = aPos.x();
            aCoord2 = aPos.z();
            bCoord1 = bPos.x();
            bCoord2 = bPos.z();
            pCoord1 = postion.x();
            pCoord2 = postion.z();
        }
        else
        {
            aCoord1 = aPos.x();
            aCoord2 = aPos.y();
            bCoord1 = bPos.x();
            bCoord2 = bPos.y();
            pCoord1 = postion.x();
            pCoord2 = postion.y();
        }

        if(pCoord2 == aCoord2 && pCoord1 == aCoord1)
        {
            return ESketchTopoRelation::OnBoundary;
        }
        if(pCoord2 == bCoord2 && pCoord1 == bCoord1)
        {
            return ESketchTopoRelation::OnBoundary;
        }
        if(aCoord2 == pCoord2 && bCoord2 == pCoord2)
        {
            float minCoord1 = std::min(aCoord1, bCoord1);
            float maxCoord1 = std::max(aCoord1, bCoord1);
            if(pCoord1 >= minCoord1 && pCoord1 <= maxCoord1)
            {
                return ESketchTopoRelation::OnBoundary;
            }
        }

        if(((aCoord2 > pCoord2) != (bCoord2 > pCoord2)) && bCoord2 != aCoord2)
        {
            float intersect = aCoord1 + (pCoord2 - aCoord2) * (bCoord1 - aCoord1) / (bCoord2 - aCoord2);
            if(abs(intersect - pCoord1) < 1e-2)
            {
                return ESketchTopoRelation::OnBoundary;
            }
            if(intersect > pCoord1)
            {
                count++;
            }
        }
    }
    return (count % 2 != 0) ? ESketchTopoRelation::Inside : ESketchTopoRelation::Outside;
}

std::vector<size_t> split_ring_at_cross_point_by_edge(const std::vector<nous::pos_t>& ring, const int edge_spt_idx,
                                                      std::vector<nous::pos_t>& id_pos_map)
{
    // 在限制下就两种情况
    // 1. 这条移动的线和某些线重合（overlap)
    // 2. 这条移动的线和某些线的端点处存在一个交点（intersect）
    std::vector<std::tuple<double, int>> edge_inside_cross_pt_info;

    const nous::pos_t selected_edge_spt_pos = ring[edge_spt_idx];
    const nous::pos_t selected_edge_ept_pos = ring[(edge_spt_idx + 1) % ring.size()];

    std::vector<nous::pos_t> temp_position_arr;
    int last_insert_ref_idx = -1;

    for(int i = 0; i < ring.size(); ++i)
    {
        if(i == edge_spt_idx)
        {
            last_insert_ref_idx = temp_position_arr.size();
            continue;
        }

        const nous::pos_t other_edge_spt_pos = ring[i];
        const nous::pos_t other_edge_ept_pos = ring[(i + 1) % ring.size()];

        // 都是环，首点先加入
        temp_position_arr.emplace_back(other_edge_spt_pos);

        // 两条线相接不算 overlap，算 touch 或者 linked，必须有一定的重叠部分才算
        const bool is_overlap =
            segment::is_two_segments_overlap(selected_edge_spt_pos, selected_edge_ept_pos, other_edge_spt_pos, other_edge_ept_pos);

        if(is_overlap)
        {
            double l1sp_p, l1ep_p, l2sp_p, l2ep_p;
            segment::two_segment_point_relative_percent(selected_edge_spt_pos, selected_edge_ept_pos, other_edge_spt_pos, other_edge_ept_pos, l1sp_p,
                                                        l1ep_p, l2sp_p, l2ep_p);

            const int contain_relation =
                segment::is_two_segments_contained(selected_edge_spt_pos, selected_edge_ept_pos, other_edge_spt_pos, other_edge_ept_pos);
            if(contain_relation == 1)
            {
                // 选中线包含其他线，在当前线中加入
                if(l2sp_p > sketch_math::sketch_epsilon && l2sp_p < l1ep_p - sketch_math::sketch_epsilon)
                {
                    edge_inside_cross_pt_info.emplace_back(l2sp_p, i);
                }
                if(l2ep_p > sketch_math::sketch_epsilon && l2ep_p < l1ep_p - sketch_math::sketch_epsilon)
                {
                    edge_inside_cross_pt_info.emplace_back(l2ep_p, i);
                }
            }
            else if(contain_relation == 2)
            {
                // 其他线包含选中线，需要在其他线位置加入两个交点，分别是选中线的首尾点
                if(l2sp_p < 0)
                {
                    // 两条线是同向的，先加入首点再加入尾点
                    temp_position_arr.emplace_back(selected_edge_spt_pos);
                    temp_position_arr.emplace_back(selected_edge_ept_pos);
                }

                if(l2sp_p > 0)
                {
                    // 两条线是异向的，先加入尾点再加入首点
                    temp_position_arr.emplace_back(selected_edge_ept_pos);
                    temp_position_arr.emplace_back(selected_edge_spt_pos);
                }
            }
            else if(contain_relation == 0)
            {
                // 两条线 overlap 且没有相互包含关系，说明是两边一边加一个点
                if(l2sp_p >= 0 && l2sp_p <= l1sp_p)
                {
                    // other 的首点在 select 内部，尾点一定在 select 外部
                    if(l2ep_p < 0)
                    {
                        // 尾点在 select 首点前面，是 select 的首点插入
                        temp_position_arr.emplace_back(selected_edge_spt_pos);
                    }
                    else
                    {
                        temp_position_arr.emplace_back(selected_edge_ept_pos);
                    }

                    edge_inside_cross_pt_info.emplace_back(l2sp_p, i);
                }
                else
                {
                    // other 的首点在 select 内部，尾点一定在 select 外部
                    if(l2sp_p < 0)
                    {
                        // 尾点在 select 首点前面，是 select 的首点插入
                        temp_position_arr.emplace_back(selected_edge_spt_pos);
                    }
                    else
                    {
                        temp_position_arr.emplace_back(selected_edge_ept_pos);
                    }

                    edge_inside_cross_pt_info.emplace_back(l2ep_p, i);
                }
            }

            // 两条线完全重合什么都不用做
        }
        else
        {
            // 计算相交点
            nous::pos_t cross_pt1, cross_pt2;
            const bool is_intersect = sketch_math::is_segment_intersect_3d(selected_edge_spt_pos, selected_edge_ept_pos, other_edge_spt_pos,
                                                                           other_edge_ept_pos, cross_pt1, cross_pt2, true);

            if(is_intersect)
            {
                // 且 cross_pt1 在 select 的范围内，否则无视
                const double selected_edge_dist = (selected_edge_ept_pos - selected_edge_spt_pos).norm();
                const double dist1 =
                    (cross_pt1 - selected_edge_spt_pos).norm() *
                    (cross_pt1 - selected_edge_spt_pos).normalized().dot((selected_edge_ept_pos - selected_edge_spt_pos).normalized());
                if(dist1 > 1e-3 && dist1 < selected_edge_dist)
                {
                    edge_inside_cross_pt_info.emplace_back(dist1, i);
                }

                const double other_edge_dist = (other_edge_ept_pos - other_edge_spt_pos).norm();
                const double dist2 = (cross_pt1 - other_edge_spt_pos).norm() *
                                     (cross_pt1 - other_edge_spt_pos).normalized().dot((other_edge_ept_pos - other_edge_spt_pos).normalized());
                if(dist2 > 1e-3 && dist2 < other_edge_dist)
                {
                    temp_position_arr.emplace_back(cross_pt1);
                }
            }
        }
    }

    std::sort(edge_inside_cross_pt_info.begin(), edge_inside_cross_pt_info.end());
    std::vector<nous::pos_t> splited_select_line_pos_arr = { selected_edge_spt_pos };
    const nous::vec3 selected_edge_dir = (selected_edge_ept_pos - selected_edge_spt_pos).normalized();
    for(int i = 0; i < edge_inside_cross_pt_info.size(); ++i)
    {
        if(i > 0 && (std::get<0>(edge_inside_cross_pt_info[i]) - std::get<0>(edge_inside_cross_pt_info[i - 1])) < sketch_math::sketch_epsilon)
        {
            continue;
        }
        splited_select_line_pos_arr.emplace_back(selected_edge_spt_pos + selected_edge_dir * std::get<0>(edge_inside_cross_pt_info[i]));
    }

    temp_position_arr.insert(temp_position_arr.begin() + last_insert_ref_idx, splited_select_line_pos_arr.begin(), splited_select_line_pos_arr.end());

    return convert_ring_position_to_relative_idx(temp_position_arr, id_pos_map);
}

std::vector<std::vector<nous::pos_t>> try_split_ring_rel_idx_arr(const std::vector<nous::pos_t>& ring, std::vector<size_t>& ring_rel_idx_arr,
                                                                 const std::vector<nous::pos_t>& id_pos_map)
{
    std::vector<std::vector<nous::pos_t>> result;

    const nous::vec3 basic_ring_normal = sketch_math::ring_normal(ring);

    // ToDo remove invalid hang lines in ring_pt_arr
    // 基于生成的 ring_pt_arr 序列，如果里面有重复的 Point* 则需要判断两个相同 Point* 间的玩意是否是一个有效的 Polygon，否则移除这些点
    for(int i = 0; i < ring_rel_idx_arr.size(); ++i)
    {
        const nous::size_t basic_pt_idx = ring_rel_idx_arr[i];
        int j = i + 1;
        while(j < (ring_rel_idx_arr.size() + i))
        {
            const size_t repeat_pt_idx = ring_rel_idx_arr[j % ring_rel_idx_arr.size()];
            if(basic_pt_idx == repeat_pt_idx)
            {
                break;
            }
            ++j;
        }

        // 未找到 basic_pt 的重复点
        if(j % ring_rel_idx_arr.size() == i)
        {
            continue;
        }

        bool ring_pt_idx_arr_modified = false;

        // 找到 i j 之后正方向一圈，逆方向一圈，如果有合法的多边形就提取出来
        std::vector<nous::pos_t> forward_polygon_positions;
        for(int m = i; m < j; ++m)
        {
            forward_polygon_positions.emplace_back(id_pos_map[ring_rel_idx_arr[m % ring_rel_idx_arr.size()]]);
        }

        std::vector<nous::pos_t> backward_polygon_positions;
        if(j % ring_rel_idx_arr.size() < i)
        {
            for(int m = j; m < i; ++m)
            {
                backward_polygon_positions.emplace_back(id_pos_map[ring_rel_idx_arr[m]]);
            }
        }
        else
        {
            for(int m = j; m < (i + ring_rel_idx_arr.size()); ++m)
            {
                backward_polygon_positions.emplace_back(id_pos_map[ring_rel_idx_arr[m % ring_rel_idx_arr.size()]]);
            }
        }

        std::set<size_t, std::greater<size_t>> need_delete_pt_idx_set;
        if(geos_util::is_polygon_valid(forward_polygon_positions))
        {
            const nous::vec3 forward_polygon_normal = sketch_math::ring_normal(forward_polygon_positions);
            if(basic_ring_normal.dot(forward_polygon_normal) >= 1 - 1e-5)
            {
                // 正向一圈是合法多边形，加入结果，将涉及到的 point 加入删除序列
                result.emplace_back(std::move(forward_polygon_positions));

                for(int m = i; m < j; ++m)
                {
                    need_delete_pt_idx_set.insert(m % ring_rel_idx_arr.size());
                }
                ring_pt_idx_arr_modified = true;
            }
        }

        if(geos_util::is_polygon_valid(backward_polygon_positions))
        {
            const nous::vec3 backward_polygon_normal = sketch_math::ring_normal(backward_polygon_positions);
            if(basic_ring_normal.dot(backward_polygon_normal) >= 1 - 1e-5)
            {
                // 反向同理于正向
                result.emplace_back(std::move(backward_polygon_positions));

                if(j % ring_rel_idx_arr.size() < i)
                {
                    for(int m = j; m < i; ++m)
                    {
                        need_delete_pt_idx_set.insert(ring_rel_idx_arr[m]);
                    }
                }
                else
                {
                    for(int m = j; m < (i + ring_rel_idx_arr.size()); ++m)
                    {
                        need_delete_pt_idx_set.insert(m % ring_rel_idx_arr.size());
                    }
                }
                ring_pt_idx_arr_modified = true;
            }
        }

        if(!need_delete_pt_idx_set.empty())
        {
            for(const size_t pt_idx: need_delete_pt_idx_set)
            {
                ring_rel_idx_arr.erase(ring_rel_idx_arr.begin() + pt_idx);
            }
        }

        if(ring_pt_idx_arr_modified)
        {
            // 从头找一遍
            i = 0;
        }
    }

    // 剩下的最后一圈也要 check 一遍
    std::vector<nous::pos_t> remain_polygon_positions;
    for(int i = 0; i < ring_rel_idx_arr.size(); ++i)
    {
        remain_polygon_positions.emplace_back(id_pos_map[ring_rel_idx_arr[i]]);
    }
    if(geos_util::is_polygon_valid(remain_polygon_positions))
    {
        result.emplace_back(std::move(remain_polygon_positions));
    }

    return result;
}

std::vector<std::vector<nous::pos_t>> split_self_cross_ring(const std::vector<nous::pos_t>& ring, const int edge_spt_idx)
{
    std::vector<std::vector<nous::pos_t>> result;

    std::vector<nous::pos_t> id_pos_map;
    std::vector<size_t> ring_pt_idx_arr = split_ring_at_cross_point_by_edge(ring, edge_spt_idx, id_pos_map);

    return try_split_ring_rel_idx_arr(ring, ring_pt_idx_arr, id_pos_map);
}

std::vector<size_t> convert_ring_position_to_relative_idx(const std::vector<nous::pos_t>& ring, std::vector<nous::pos_t>& id_pos_map)
{
    std::vector<size_t> result;
    // 从坐标序列转为编号序列
    for(int i = 0; i < ring.size(); ++i)
    {
        int exist_pt_idx = -1;

        for(int j = 0; j < id_pos_map.size(); ++j)
        {
            if((id_pos_map[j] - ring[i]).norm() < 1e-2)
            {
                exist_pt_idx = j;
                break;
            }
        }

        if(exist_pt_idx == -1)
        {
            exist_pt_idx = id_pos_map.size();
            id_pos_map.emplace_back(ring[i]);
        }

        result.emplace_back(exist_pt_idx);
    }

    for(int i = 0; i < result.size(); ++i)
    {
        if(result[i] == result[(i + 1) % result.size()])
        {
            result.erase(result.begin() + i);
            --i;
        }
    }

    return result;
}

std::vector<std::vector<size_t>> convert_polygon_position_to_relative_idx(const std::vector<std::vector<nous::pos_t>>& polygon,
                                                                          std::vector<nous::pos_t>& id_pos_map)
{
    std::vector<std::vector<size_t>> result;
    for(const auto ring: polygon)
    {
        auto&& ring_result = convert_ring_position_to_relative_idx(ring, id_pos_map);
        result.emplace_back(std::move(ring_result));
    }
    return result;
}

std::vector<std::vector<std::vector<nous::pos_t>>> rebuild_invalid_polygon_topo(std::vector<std::vector<nous::pos_t>>& polygon)
{
    std::vector<std::vector<std::vector<nous::pos_t>>> rebuild_result;

    std::vector<std::vector<nous::pos_t>> inner_ring_cache_arr;
    for(int i = 1; i < polygon.size(); ++i)
    {
        std::vector<nous::pos_t> inring_id_pos_map;
        std::vector<size_t> inring_pt_rel_idx_arr = convert_ring_position_to_relative_idx(polygon[i], inring_id_pos_map);
        auto&& inring_checked_result = try_split_ring_rel_idx_arr(polygon[i], inring_pt_rel_idx_arr, inring_id_pos_map);
        for(auto& inner_ring: inring_checked_result)
        {
            inner_ring_cache_arr.emplace_back(std::move(inner_ring));
        }
    }

    std::vector<nous::pos_t> exring_id_pos_map;
    std::vector<size_t> exring_pt_rel_idx_arr = convert_ring_position_to_relative_idx(polygon.front(), exring_id_pos_map);
    auto&& exring_checked_result = try_split_ring_rel_idx_arr(polygon.front(), exring_pt_rel_idx_arr, exring_id_pos_map);

    for(int i = 0; i < exring_checked_result.size(); ++i)
    {
        auto& child_polygon = rebuild_result.emplace_back();
        child_polygon.emplace_back(std::move(exring_checked_result[i]));
        geos::geom::Geometry* exring_polygon = NousGeometryConverter::position_convert_to_geos_polygon(child_polygon.front());
        for(int j = 0; j < inner_ring_cache_arr.size(); ++j)
        {
            geos::geom::Geometry* inring_polygon = NousGeometryConverter::position_convert_to_geos_polygon(inner_ring_cache_arr[j]);
            if(exring_polygon->contains(inring_polygon))
            {
                child_polygon.emplace_back(std::move(inner_ring_cache_arr[j]));
                inner_ring_cache_arr.erase(inner_ring_cache_arr.begin() + j);
                --j;
            }
        }
    }

    return rebuild_result;
}

void fix_one_point_repeated_polygon(std::vector<std::vector<nous::pos_t>>& polygon)
{
    auto& polygon_exring_pos = polygon.front();
    std::vector<nous::pos_t> id_pos_map;
    const auto&& ring_pt_rel_idx_arr = convert_ring_position_to_relative_idx(polygon_exring_pos, id_pos_map);
    const nous::vec3 polygon_normal = sketch_math::ring_normal(polygon_exring_pos);

    for(int i = 0; i < ring_pt_rel_idx_arr.size(); ++i)
    {
        if(ring_pt_rel_idx_arr[i] == i)
        {
            continue;
        }

        const int prev_repeat_idx = ring_pt_rel_idx_arr[i];

        // 在 prev_repeat_idx 和 i 之间 和 i 到 prev_repeat_idx 之间的范围，若有一个法线相反的面，这是内环要处理下
        std::vector<nous::pos_t> child_ring;
        for(int j = prev_repeat_idx; j < i; ++j)
        {
            child_ring.emplace_back(id_pos_map[ring_pt_rel_idx_arr[j]]);
        }
        nous::vec3 child_ring_normal = sketch_math::ring_normal(child_ring);
        double dot_v = polygon_normal.dot(child_ring_normal);
        if(dot_v < 0)
        {
            // 这一段变成内环
            std::vector<nous::pos_t> new_inner_ring;
            for(int j = prev_repeat_idx; j < i; ++j)
            {
                new_inner_ring.emplace_back(id_pos_map[ring_pt_rel_idx_arr[j]]);
                polygon_exring_pos.erase(polygon_exring_pos.begin() + prev_repeat_idx);
            }
            polygon.push_back(std::move(new_inner_ring));
        }
        else
        {
            // 留下这一段，剩余一段变成内环
            std::vector<nous::pos_t> new_inner_ring;
            const int backward_count = polygon_exring_pos.size() - i;
            const int ori_exring_pt_count = polygon_exring_pos.size();
            for(int j = i; j < (prev_repeat_idx + ori_exring_pt_count); ++j)
            {
                if(j - i < backward_count)
                {
                    new_inner_ring.emplace_back(id_pos_map[ring_pt_rel_idx_arr[j]]);
                    polygon_exring_pos.erase(polygon_exring_pos.begin() + i);
                }
                else
                {
                    new_inner_ring.emplace_back(id_pos_map[ring_pt_rel_idx_arr[j]]);
                    polygon_exring_pos.erase(polygon_exring_pos.begin());
                }
            }
            polygon.push_back(std::move(new_inner_ring));
        }

        break;
    }
}

std::vector<std::vector<nous::pos_t>> generate_offseted_position_by_normal(const std::vector<std::vector<nous::pos_t>>& positions,
                                                                           const nous::scalar value)
{
    const size_t rows = positions.size();
    const size_t cols = positions.front().size();

    std::vector<std::vector<nous::vec3>> control_point_normal_mat(rows, std::vector<nous::pos_t>(cols));

    // 四个角只需要一个三角形确定法线
    control_point_normal_mat[0][0] = sketch_math::ring_normal({ positions[0][0], positions[0][1], positions[1][0] });

    control_point_normal_mat[rows - 1][0] = sketch_math::ring_normal({ positions[rows - 1][0], positions[rows - 2][0], positions[rows - 1][1] });

    control_point_normal_mat[0][cols - 1] = sketch_math::ring_normal({ positions[0][cols - 1], positions[1][cols - 1], positions[0][cols - 2] });

    control_point_normal_mat[rows - 1][cols - 1] =
        sketch_math::ring_normal({ positions[rows - 1][cols - 1], positions[rows - 1][cols - 2], positions[rows - 2][cols - 1] });

    // 边缘用两个三角形确定法线
    for(int i = 1; i < rows - 1; ++i)
    {
        const nous::vec3 normal1 = sketch_math::ring_normal({ positions[i][0], positions[i - 1][0], positions[i][1] });
        const nous::vec3 normal2 = sketch_math::ring_normal({ positions[i][0], positions[i][1], positions[i + 1][0] });
        control_point_normal_mat[i][0] = (normal1 + normal2).normalized();

        const nous::vec3 normal3 = sketch_math::ring_normal({ positions[i][cols - 1], positions[i][cols - 2], positions[i - 1][cols - 1] });
        const nous::vec3 normal4 = sketch_math::ring_normal({ positions[i][cols - 1], positions[i + 1][cols - 1], positions[i][cols - 2] });
        control_point_normal_mat[i][cols - 1] = (normal3 + normal4).normalized();
    }

    for(int i = 1; i < cols - 1; ++i)
    {
        const nous::vec3 normal1 = sketch_math::ring_normal({ positions[0][i], positions[1][i], positions[0][i - 1] });
        const nous::vec3 normal2 = sketch_math::ring_normal({ positions[0][i], positions[0][i + 1], positions[1][i] });
        control_point_normal_mat[0][i] = (normal1 + normal2).normalized();

        const nous::vec3 normal3 = sketch_math::ring_normal({ positions[rows - 1][i], positions[rows - 1][i - 1], positions[rows - 2][i] });
        const nous::vec3 normal4 = sketch_math::ring_normal({ positions[rows - 1][i], positions[rows - 2][i], positions[rows - 1][i + 1] });
        control_point_normal_mat[rows - 1][i] = (normal3 + normal4).normalized();
    }

    // 中间用四个三角形确定法线
    for(int i = 1; i < rows - 1; ++i)
    {
        for(int j = 1; j < cols - 1; ++j)
        {
            const nous::vec3 normal1 = sketch_math::ring_normal({ positions[i][j], positions[i][j - 1], positions[i - 1][j] });
            const nous::vec3 normal2 = sketch_math::ring_normal({ positions[i][j], positions[i - 1][j], positions[i][j + 1] });
            const nous::vec3 normal3 = sketch_math::ring_normal({ positions[i][j], positions[i][j + 1], positions[i + 1][j] });
            const nous::vec3 normal4 = sketch_math::ring_normal({ positions[i][j], positions[i + 1][j], positions[i][j - 1] });
            control_point_normal_mat[i][j] = (normal1 + normal2 + normal3 + normal4).normalized();
        }
    }

    for(int i = 0; i < cols; ++i)
    {
        if(sketch_math::point_equal(positions[0][i], positions[rows - 1][i]))
        {
            const nous::vec3 shared_point_normal = (control_point_normal_mat[0][i] + control_point_normal_mat[rows - 1][i]).normalized();
            control_point_normal_mat[0][i] = shared_point_normal;
            control_point_normal_mat[rows - 1][i] = shared_point_normal;
        }
    }

    for(int i = 0; i < rows; ++i)
    {
        if(sketch_math::point_equal(positions[i][0], positions[i][cols - 1]))
        {
            const nous::vec3 shared_point_normal = (control_point_normal_mat[i][0] + control_point_normal_mat[i][cols - 1]).normalized();
            control_point_normal_mat[i][0] = shared_point_normal;
            control_point_normal_mat[i][cols - 1] = shared_point_normal;
        }
    }

    std::vector<std::vector<nous::pos_t>> result(rows, std::vector<nous::pos_t>(cols));
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            result[i][j] = positions[i][j] + control_point_normal_mat[i][j] * value;
        }
    }

    return result;
}

EPointSetTopo calc_point_set_topo(const std::vector<nous::pos_t>& point_set, nous::vec3& coplane_normal)
{
    if(point_set.size() < 3)
    {
        return EPointSetTopo::Coline;
    }

    if(point_set.size() < 4)
    {
        return EPointSetTopo::Coplane;
    }

    nous::vec3 guide_dir1;
    for(int i = 1; i < point_set.size(); ++i)
    {
        guide_dir1 = (point_set[i] - point_set[0]).normalized();
        if(guide_dir1.norm() > 1e-5)
        {
            break;
        }
    }

    bool coline = true;
    for(int i = 2; i < point_set.size(); ++i)
    {
        const nous::vec3 guide_dir2 = (point_set[i] - point_set[0]).normalized();
        if(guide_dir2.norm() < 1e-5)
        {
            continue;
        }

        // 判断 guide_dir1 与 guide_dir2 是否平行，若平行 cross 不出法线
        if(!sketch_math::direction_parallel(guide_dir1, guide_dir2))
        {
            coline = false;
            coplane_normal = guide_dir1.cross(guide_dir2);
            break;
        }
    }

    // 所有的点都在一条直线上
    if(coline)
    {
        return EPointSetTopo::Coline;
    }

    bool coplane = true;
    for(int i = 1; i < point_set.size(); ++i)
    {
        const nous::vec3 check_dir = (point_set[i] - point_set[0]).normalized();
        if(check_dir.norm() < 1e-5)
        {
            continue;
        }

        const bool is_dir_vertical = sketch_math::direction_vertical(check_dir, coplane_normal);
        if(!is_dir_vertical)
        {
            coplane = false;
            break;
        }
    }

    if(coplane)
    {
        return EPointSetTopo::Coplane;
    }

    return EPointSetTopo::Default;
}

};// namespace topo

namespace util
{

int calc_ring_first_corner_index(const std::vector<nous::pos_t>& ring)
{
    for(int j = 0; j < ring.size(); ++j)
    {
        const nous::pos_t prev_pt_pos = ring[(j - 1 + ring.size()) % ring.size()];
        const nous::pos_t curr_pt_pos = ring[j];
        const nous::pos_t next_pt_pos = ring[(j + 1) % ring.size()];

        const nous::vec3 prev_to_curr_dir = (curr_pt_pos - prev_pt_pos).normalized();
        const nous::vec3 curr_to_next_dir = (next_pt_pos - curr_pt_pos).normalized();

        if(sketch_math::direction_parallel(prev_to_curr_dir, curr_to_next_dir))
        {
            return j;
        }
    }

    return -1;
}

};// namespace util

};// namespace sketch