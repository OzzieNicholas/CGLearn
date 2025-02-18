#include "sketch/NousEngine/SketchMath.h"
#include "nous_mesh/algorithm/intersect.h"

namespace sketch_math
{
nous::AABB3 aabb3_from_points(const std::vector<nous::pos_t>& points, float expand)
{
    nous::AABB3 aabb3;
    float min_size = sketch_math::sketch_epsilon;
    for(const auto& pos: points)
    {
        aabb3.extend(pos);
    }

    // 扩展 AABB
    for(int i = 0; i < 3; ++i)
    {
        aabb3.min()[i] -= expand;
        aabb3.max()[i] += expand;

        // 如果某个维度扩展后尺寸为零，设置最小值，确保有体积
        if(aabb3.max()[i] - aabb3.min()[i] < min_size)
        {
            float center = (aabb3.max()[i] + aabb3.min()[i]) * 0.5f;
            aabb3.min()[i] = center - min_size * 0.5;
            aabb3.max()[i] = center + min_size * 0.5;
        }
    }
    return aabb3;
}

bool plane_coplane_plane(const nous::vec3& plane0_normal, const nous::pos_t& plane0_pos, const nous::vec3& plane1_normal,
                         const nous::pos_t& plane1_pos)
{
    if(!direction_parallel(plane0_normal, plane1_normal))
    {
        return false;
    }
    //
    // if(point_on_plane(plane0_pos, plane1_pos, plane1_normal))
    //{
    //     return true;
    // }

    nous::vec3 dir = plane0_pos - plane1_pos;
    if(!direction_vertical(dir, plane0_normal))
    {
        return false;
    }

    return true;
}

nous::vec3d point_to_plane_closest(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon)
{
    nous::vec3d normalized_n = plane_n.normalized();
    nous::vec3d point_to_origin = point - plane_o;
    double distance = point_to_origin.dot(normalized_n);
    nous::vec3d closest_point = point - normalized_n * distance;
    return closest_point;
}

nous::vec3 point_to_plane_closest(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon)
{
    nous::vec3d o = plane_o.cast<double>();
    nous::vec3d n = plane_n.cast<double>();
    nous::vec3d pointd = point.cast<double>();

    nous::vec3d result = point_to_plane_closest(pointd, o, n, epsilon);
    return result.cast<float>();
}

double point_to_plane_distance(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon)
{
    nous::vec3d closest_point = point_to_plane_closest(point, plane_o, plane_n, epsilon);
    nous::vec3d point_to_closest = point - closest_point;
    return point_to_closest.norm();
}

float point_to_plane_distance(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon)
{
    nous::vec3d o = plane_o.cast<double>();
    nous::vec3d n = plane_n.cast<double>();
    nous::vec3d pointd = point.cast<double>();
    return point_to_line_distance(pointd, o, o + n, false, epsilon);
}

bool point_on_plane(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon)
{
    nous::vec3d o = plane_o.cast<double>();
    nous::vec3d n = plane_n.cast<double>();
    nous::vec3d pointd = point.cast<double>();
    return point_on_plane(pointd, o, n);
}

bool point_on_plane(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon)
{
    nous::vec3d closest_point = point_to_plane_closest(point, plane_o, plane_n, epsilon);
    nous::vec3d point_to_closest = point - closest_point;
    return point_to_closest.squaredNorm() < epsilon * epsilon;
}

nous::vec3d point_to_line_closest(const nous::vec3d& point, const nous::vec3d& p0, const nous::vec3d& p1, bool seg_line, float epsilon)
{
    nous::vec3d line_vec = p1 - p0;
    nous::vec3d point_vec = point - p0;

    double line_len_squared = line_vec.dot(line_vec);
    if(line_len_squared < epsilon)
    {
        return p0;
    }

    double proj = point_vec.dot(line_vec) / line_len_squared;
    if(seg_line)
    {
        if(proj < epsilon)
            return p0;
        else if(proj > 1.0 + epsilon)
            return p1;
        else
        {
            return p0 + line_vec * proj;
        }
    }
    else
    {
        return p0 + line_vec * proj;
    }
}

nous::vec3 point_to_line_closest(const nous::vec3& point, const nous::vec3& p0, const nous::vec3& p1, bool seg_line, float epsilon)
{
    nous::vec3d p0d = p0.cast<double>();
    nous::vec3d p1d = p1.cast<double>();
    nous::vec3d pointd = point.cast<double>();
    nous::vec3d result = point_to_line_closest(pointd, p0d, p1d, seg_line, epsilon);
    return result.cast<float>();
}

double point_to_point_distance(const nous::pos_t& p0, const nous::pos_t& p1) { return (p0 - p1).norm(); }

double point_to_line_distance(const nous::vec3d& point, const nous::vec3d& p0, const nous::vec3d& p1, bool seg_line, float epsilon)
{
    nous::vec3d closest_point = point_to_line_closest(point, p0, p1, seg_line, epsilon);
    return (point - closest_point).norm();
}

float point_to_line_distance(const nous::vec3& point, const nous::vec3& p0, const nous::vec3& p1, bool seg_line, float epsilon)
{
    nous::vec3d p0d = p0.cast<double>();
    nous::vec3d p1d = p1.cast<double>();
    nous::vec3d pointd = point.cast<double>();
    return point_to_line_distance(pointd, p0d, p1d, seg_line, epsilon);
}

nous::vec3 line_intersect_plane(const nous::vec3& line_pos, const nous::vec3& line_dir, const nous::vec3& plane_pos, const nous::vec3& plane_normal)
{
    const double D = -(plane_pos.x() * plane_normal.x() + plane_pos.y() * plane_normal.y() + plane_pos.z() * plane_normal.z());
    if(fabs(line_dir.dot(plane_normal)) < 1e-5)
    {
        return nous::vec3::Zero();
    }
    const double t = -((plane_normal.dot(line_pos) + D) / line_dir.dot(plane_normal));
    return line_pos + t * line_dir;
}

bool direction_parallel(const nous::vec3d& dir1, const nous::vec3d& dir2, float epsilon)
{
    nous::vec3d cross_product = dir1.normalized().cross(dir2.normalized());
    float cross_norm_squared = cross_product.squaredNorm();
    // float max_norm_squared = std::max(dir1.squaredNorm(), dir2.squaredNorm());

    return cross_norm_squared < (epsilon * epsilon /* max_norm_squared*/);
}

bool direction_parallel(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon)
{
    nous::vec3d d1 = dir1.cast<double>();
    nous::vec3d d2 = dir2.cast<double>();
    return direction_parallel(d1, d2, epsilon);
}

bool direction_parallel_sketch_precision(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon)
{
    nous::vec3d d1 = dir1.cast<double>();
    nous::vec3d d2 = dir2.cast<double>();

    d1 = d1.normalized();
    d2 = d2.normalized();

    double length1_squal = d1.squaredNorm();
    double length2_squal = d2.squaredNorm();

    // 延长线段到最大长度
    nous::vec3d extended_p0 = d1 * sketch_max_length;
    nous::vec3d extended_p3 = d2 * sketch_max_length;

    // 计算延长后端点到对方线段的距离
    float dist1 = point_to_line_distance(extended_p0, nous::vec3d::Zero(), extended_p3, false);
    float dist2 = point_to_line_distance(extended_p3, nous::vec3d::Zero(), extended_p0, false);

    // 判断距离是否小于误差阈值
    return dist1 < epsilon * 2.f && dist2 < epsilon * 2.f;
}

bool direction_vertical(const nous::vec3d& dir1, const nous::vec3d& dir2, float epsilon)
{
    //// 计算两个方向向量的模长
    // float length1 = dir1.norm();
    // float length2 = dir2.norm();
    //
    //// 线段退化成点
    // if(length1 < epsilon || length2 < epsilon)
    //     return false;

    double dot_product = dir1.normalized().dot(dir2.normalized());
    return std::fabs(dot_product) < epsilon;
}

bool direction_vertical(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon)
{
    nous::vec3d d1 = dir1.cast<double>();
    nous::vec3d d2 = dir2.cast<double>();
    return direction_vertical(d1, d2, epsilon);
}

nous::vec3 ring_normal(const std::vector<nous::vec3>& ring)
{
    nous::vec3 normal = nous::vec3::Zero();
    for(int i = 0; i < ring.size(); ++i)
    {
        const nous::pos_t curr_pos = ring[i];
        const nous::pos_t next_pos = ring[(i + 1) % ring.size()];

        normal.x() -= (curr_pos.y() - next_pos.y()) * (curr_pos.z() + next_pos.z());
        normal.y() -= (curr_pos.z() - next_pos.z()) * (curr_pos.x() + next_pos.x());
        normal.z() -= (curr_pos.x() - next_pos.x()) * (curr_pos.y() + next_pos.y());
    }

    return normal.normalized();
}

float direction_angle(const nous::vec3& a, const nous::vec3& b, const nous::vec3& normal, float epsilon)
{
    const nous::vec3d da = a.cast<double>();
    const nous::vec3d db = b.cast<double>();
    const nous::vec3d dn = normal.cast<double>();

    double dot_product = da.dot(db);
    if(std::abs(dot_product) < epsilon)
    {
        dot_product = 0.0f;
    }
    else if(std::abs(dot_product + 1.0f) < epsilon)
    {
        dot_product = -1.0f;
    }
    else if(std::abs(dot_product - 1.0f) < epsilon)
    {
        dot_product = 1.0f;
    }
    dot_product = std::clamp(dot_product, -1.0, 1.0);
    double angle = std::acos(-dot_product) * (180.0 / 3.14159265);
    nous::vec3d cross_product = da.cross(db);
    if(cross_product.dot(dn) < 0.0)
    {
        angle = 360.0f - angle;
    }
    return angle;
}

bool point_equal(const nous::pos_t& pos1, const nous::pos_t& pos2, float epsilon)
{
    const nous::vec3d dpos1 = pos1.cast<double>();
    const nous::vec3d dpos2 = pos2.cast<double>();
    const double distance = (dpos1 - dpos2).squaredNorm();
    const double epsilon2 = epsilon * epsilon;

    if(distance < epsilon2)
    {
        return true;
    }

    return false;
}

bool line_intersect(const nous::pos_t& line0_p0, const nous::pos_t& line0_p1, const nous::pos_t& line1_p0, const nous::pos_t& line1_p1,
                    float* line0_p0_distance, float* line1_p0_distance, bool segment)
{
    const nous::vec3d p0 = line0_p0.cast<double>();
    const nous::vec3d p1 = line0_p1.cast<double>();
    const nous::vec3d p2 = line1_p0.cast<double>();
    const nous::vec3d p3 = line1_p1.cast<double>();

    nous::vec3d d1 = p1 - p0;
    nous::vec3d d2 = p3 - p2;

    nous::vec3d cross_d1_d2 = d1.cross(d2);
    double cross_norm_pow2 = cross_d1_d2.squaredNorm();

    if(cross_norm_pow2 < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    nous::vec3d r = p2 - p0;
    double t0 = r.cross(d2).dot(cross_d1_d2) / cross_norm_pow2;
    double t1 = r.cross(d1).dot(cross_d1_d2) / cross_norm_pow2;

    if(line0_p0_distance)
    {
        *line0_p0_distance = static_cast<float>(t0);
    }
    if(line1_p0_distance)
    {
        *line1_p0_distance = static_cast<float>(t1);
    }

    if(segment)
    {
        if(t0 < -sketch_epsilon || t0 > 1.0 + sketch_epsilon || t1 < -sketch_epsilon || t1 > 1.0 + sketch_epsilon)
        {
            return false;
        }
    }

    return true;
}

bool line_intersect(const nous::pos_t& line0_p0, const nous::pos_t& line0_p1, const nous::pos_t& line1_p0, const nous::pos_t& line1_p1,
                    nous::pos_t& line0_intersect, nous::pos_t& line1_intersect, bool segment)
{
    float line0_p0_distance = 0.0f;
    float line1_p0_distance = 0.0f;
    bool is_intersect = line_intersect(line0_p0, line0_p1, line1_p0, line1_p1, &line0_p0_distance, &line1_p0_distance, segment);
    line0_intersect = line0_p0 + (line0_p1 - line0_p0) * line0_p0_distance;
    line1_intersect = line1_p0 + (line1_p1 - line1_p0) * line1_p0_distance;
    return is_intersect;
}

bool is_line_intersect_3d(const nous::pos_t& pos1, const nous::pos_t& pos2, const nous::pos_t& pos3, const nous::pos_t& pos4, nous::pos_t& cross_a,
                          nous::pos_t& cross_b, const bool is_segment)
{
    const bool is_intersect = nous::algo::line_intersect_3d(pos1, pos2, pos3, pos4, cross_a, cross_b, is_segment);
    if(is_intersect && (cross_a - cross_b).norm() < sketch_epsilon)
    {
        return true;
    }
    return false;
}

bool is_segment_intersect_3d(const nous::pos_t& pos1, const nous::pos_t& pos2, const nous::pos_t& pos3, const nous::pos_t& pos4, nous::pos_t& cross_a,
                             nous::pos_t& cross_b, bool with_head)
{
    const bool is_intersect = nous::algo::line_intersect_3d(pos1, pos2, pos3, pos4, cross_a, cross_b, false);
    if(is_intersect && (cross_a - cross_b).norm() < sketch_epsilon)
    {
        // 判断交点是否在线内
        const double segment1_dist = (pos2 - pos1).norm();
        const double segment2_dist = (pos4 - pos3).norm();
        const double crossa_in_seg1_percent = (cross_a - pos1).norm() * (cross_a - pos1).normalized().dot((pos2 - pos1).normalized());
        const double crossb_in_seg2_percent = (cross_b - pos3).norm() * (cross_b - pos3).normalized().dot((pos4 - pos3).normalized());

        if(with_head)
        {
            if(crossa_in_seg1_percent >= -sketch_epsilon && crossa_in_seg1_percent <= segment1_dist + sketch_epsilon &&
               crossb_in_seg2_percent >= -sketch_epsilon && crossb_in_seg2_percent <= segment2_dist + sketch_epsilon)
            {
                return true;
            }
        }
        else
        {
            if(crossa_in_seg1_percent > sketch_epsilon && crossa_in_seg1_percent < segment1_dist - sketch_epsilon &&
               crossb_in_seg2_percent > sketch_epsilon && crossb_in_seg2_percent < segment2_dist - sketch_epsilon)
            {
                return true;
            }
        }
    }
    return false;
}

}// namespace sketch_math
