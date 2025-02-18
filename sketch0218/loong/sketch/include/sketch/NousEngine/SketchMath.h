#pragma once

#include "nous_mesh/mesh.h"

namespace sketch_math
{
static constexpr double sketch_max_length = 99999.0f;
static constexpr float sketch_epsilon = 1e-2f;
static constexpr float sketch_pi = 3.1415926f;

nous::AABB3 aabb3_from_points(const std::vector<nous::pos_t>& points, float expand = 0.0f);

bool plane_coplane_plane(const nous::vec3& plane0_normal, const nous::pos_t& plane0_pos, const nous::vec3& plane1_normal,
                         const nous::pos_t& plane1_pos);

nous::vec3d point_to_plane_closest(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon = sketch_epsilon);
nous::vec3 point_to_plane_closest(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon = sketch_epsilon);

double point_to_plane_distance(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon = sketch_epsilon);
float point_to_plane_distance(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon = sketch_epsilon);

bool point_on_plane(const nous::vec3& point, const nous::vec3& plane_o, const nous::vec3& plane_n, float epsilon = sketch_epsilon);
bool point_on_plane(const nous::vec3d& point, const nous::vec3d& plane_o, const nous::vec3d& plane_n, float epsilon = sketch_epsilon);

nous::vec3d point_to_line_closest(const nous::vec3d& point, const nous::vec3d& p0, const nous::vec3d& p1, bool seg_line = true,
                                  float epsilon = sketch_epsilon);
nous::vec3 point_to_line_closest(const nous::vec3& point, const nous::vec3& p0, const nous::vec3& p1, bool seg_line = true,
                                 float epsilon = sketch_epsilon);

double point_to_point_distance(const nous::pos_t& p0, const nous::pos_t& p1);

double point_to_line_distance(const nous::vec3d& point, const nous::vec3d& p0, const nous::vec3d& p1, bool seg_line = true,
                              float epsilon = sketch_epsilon);
float point_to_line_distance(const nous::vec3& point, const nous::vec3& p0, const nous::vec3& p1, bool seg_line = true,
                             float epsilon = sketch_epsilon);

nous::vec3 line_intersect_plane(const nous::vec3& line_pos, const nous::vec3& line_dir, const nous::vec3& plane_pos, const nous::vec3& plane_normal);

// 因为parallel数学精度问题。
// 尽量使用vertical来取代parallel。
bool direction_parallel(const nous::vec3d& dir1, const nous::vec3d& dir2, float epsilon = sketch_epsilon * sketch_epsilon);
bool direction_parallel(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon = sketch_epsilon * sketch_epsilon);
bool direction_parallel_sketch_precision(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon = sketch_epsilon);

bool direction_vertical(const nous::vec3d& dir1, const nous::vec3d& dir2, float epsilon = sketch_epsilon);
bool direction_vertical(const nous::vec3& dir1, const nous::vec3& dir2, float epsilon = sketch_epsilon);

nous::vec3 ring_normal(const std::vector<nous::vec3>& ring);

float direction_angle(const nous::vec3& a, const nous::vec3& b, const nous::vec3& normal, float epsilon = sketch_epsilon);

bool point_equal(const nous::pos_t& pos1, const nous::pos_t& pos2, float epsilon = sketch_epsilon);

bool line_intersect(const nous::pos_t& line0_p0, const nous::pos_t& line0_p1, const nous::pos_t& line1_p0, const nous::pos_t& line1_p1,
                    float* line0_p0_distance, float* line1_p0_distance, bool segment = false);

bool line_intersect(const nous::pos_t& line0_p0, const nous::pos_t& line0_p1, const nous::pos_t& line1_p0, const nous::pos_t& line1_p1,
                    nous::pos_t& line0_intersect, nous::pos_t& line1_intersect, bool segment = false);

bool is_line_intersect_3d(const nous::pos_t& pos1, const nous::pos_t& pos2, const nous::pos_t& pos3, const nous::pos_t& pos4, nous::pos_t& cross_a,
                          nous::pos_t& cross_b, const bool is_segment);

bool is_segment_intersect_3d(const nous::pos_t& pos1, const nous::pos_t& pos2, const nous::pos_t& pos3, const nous::pos_t& pos4, nous::pos_t& cross_a,
                             nous::pos_t& cross_b, bool with_head = true);

}// namespace sketch_math