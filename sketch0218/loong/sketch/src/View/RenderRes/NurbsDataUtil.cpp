#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/SmoothGroup.h"
#include <iostream>
#include <sketch/View/RenderView/View.h>

namespace sketch
{
namespace nurbs
{

unsigned int get_curve_shared_point_number(const NurbsCurvePtr curve)
{
    const ENurbsCloseType close_type = curve->get_curve_close_type();
    if(close_type == ENurbsCloseType::SmoothClose)
    {
        return curve->get_curve_degree();
    }

    if(close_type == ENurbsCloseType::SharpClose)
    {
        return 1;
    }

    return 0;
}

NurbsAlgebraSurfacePtr extrude_curve_to_surface(const NurbsCurvePtr curve, const nous::vec3& vector)
{
    NurbsAlgebraSurfacePtr new_surface = std::make_shared<NurbsAlgebraSurface>();

    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();

    surface_core->degree_v = 1;
    surface_core->degree_u = curve->get_curve_degree();

    const auto& curve_knots = curve->get_curve_knots();
    surface_core->knots_u.insert(surface_core->knots_u.end(), curve_knots.begin(), curve_knots.end());
    surface_core->knots_v.emplace_back(0.0);
    surface_core->knots_v.emplace_back(0.0);
    surface_core->knots_v.emplace_back(1.0);
    surface_core->knots_v.emplace_back(1.0);

    const auto& curve_control_points = curve->get_curve_control_points();
    surface_core->control_points.resize(curve_control_points.size(), 2);

    const unsigned int shared_point_number = get_curve_shared_point_number(curve);
    // 注意闭合曲线的共享控制点指针，不能弄多了！
    for(int i = 0; i < curve_control_points.size() - shared_point_number; ++i)
    {
        surface_core->control_points(i, 0) = curve_control_points[i];
        const nous::pos_t offseted_pos = surface_core->control_points(i, 0)->m_point + vector;
        surface_core->control_points(i, 1) = std::make_shared<SketchNurbsPoint>(offseted_pos);
    }
    for(int i = 0; i < shared_point_number; ++i)
    {
        surface_core->control_points(curve_control_points.size() - shared_point_number + i, 0) = surface_core->control_points(i, 0);
        surface_core->control_points(curve_control_points.size() - shared_point_number + i, 1) = surface_core->control_points(i, 1);
    }

    const auto& curve_weights = curve->get_curve_weights();
    surface_core->weights.resize(curve_weights.size(), 2);
    for(int i = 0; i < curve_weights.size(); ++i)
    {
        surface_core->weights(i, 0) = curve_weights[i];
        surface_core->weights(i, 1) = curve_weights[i];
    }

    new_surface->update_surface_data(surface_core);

    return new_surface;
}

std::vector<NurbsAlgebraSurfacePtr> extrude_curves_to_surface_with_shared_edge(const std::vector<NurbsCurvePtr>& curves, const nous::vec3& vector)
{
    std::vector<NurbsAlgebraSurfacePtr> result;

    bool curve_arr_closed = sketch_math::point_equal(curves.front()->get_position(0.0), curves.back()->get_position(1.0));

    for(int i = 0; i < curves.size(); ++i)
    {
        NurbsAlgebraSurfacePtr surface = extrude_curve_to_surface(curves[i], vector);

        if(i > 0)
        {
            NurbsAlgebraSurfacePtr prev_surface = result.back();
            auto&& prev_surface_ctp_arr = prev_surface->get_surface_control_points();
            surface->replace_surface_control_point(0, 0, prev_surface_ctp_arr[prev_surface_ctp_arr.size() - 1][0]);
            surface->replace_surface_control_point(0, 1, prev_surface_ctp_arr[prev_surface_ctp_arr.size() - 1][1]);
        }

        if(i == curves.size() - 1 && curve_arr_closed)
        {
            NurbsAlgebraSurfacePtr next_surface = result.front();
            auto&& curr_surface_ctp_arr = surface->get_surface_control_points();
            auto&& next_surface_ctp_arr = next_surface->get_surface_control_points();
            surface->replace_surface_control_point(curr_surface_ctp_arr.size() - 1, 0, next_surface_ctp_arr[0][0]);
            surface->replace_surface_control_point(curr_surface_ctp_arr.size() - 1, 1, next_surface_ctp_arr[0][1]);
        }

        result.emplace_back(surface);
    }

    return result;
}

std::vector<PointFrames> calc_segment_point_frames(const std::vector<nous::pos_t>& segment)
{
    if(segment.size() != 2)
    {
        return std::vector<PointFrames>();
    }

    const nous::pos_t& prev_pos = segment.front();
    const nous::pos_t& next_pos = segment.back();

    const nous::vec3 tangent = (next_pos - prev_pos).normalized();
    const nous::vec3 up = nous::vec3::Identity();
    nous::vec3 normal;
    if(tangent.dot(up) >= 1 - 1e-3)
    {
        normal = nous::vec3(1, 0, 0);
    }
    else
    {
        normal = tangent.cross(up).normalized();
    }
    const nous::vec3& bitangent = normal.cross(tangent);

    PointFrames spf;
    spf.pos = prev_pos;
    spf.normal = normal;
    spf.up = bitangent;
    spf.tangent = tangent;

    PointFrames epf;
    spf.pos = next_pos;
    spf.normal = normal;
    spf.up = bitangent;
    spf.tangent = tangent;

    std::vector<PointFrames> result;
    result.emplace_back(std::move(spf));
    result.emplace_back(std::move(epf));

    return result;
}

std::vector<PointFrames> calc_polyline_point_frames(const std::vector<nous::pos_t>& polyline, bool is_closed)
{
    if(polyline.size() == 2)
    {
        // 退化为线段
        return calc_segment_point_frames(polyline);
    }

    std::vector<PointFrames> result;
    for(int i = 0; i < polyline.size(); ++i)
    {
        PointFrames point_frame;
        point_frame.pos = polyline[i];

        const nous::pos_t& prev_pos = polyline[(i - 1 + polyline.size()) % polyline.size()];
        const nous::pos_t& curr_pos = polyline[i];
        const nous::pos_t& next_pos = polyline[(i + 1) % polyline.size()];

        if(!is_closed)
        {
            if(i == 0)
            {
                const nous::pos_t next_next_pos = polyline[(i + 2) % polyline.size()];
                point_frame.tangent = (next_pos - curr_pos).normalized();
                const nous::vec3 up = (next_next_pos - next_pos).normalized().cross(-point_frame.tangent).normalized();
                const nous::vec3 normal = up.cross(point_frame.tangent).normalized();
                point_frame.up = up;
                point_frame.normal = normal;
                continue;
            }

            if(i == polyline.size() - 1)
            {
                const nous::pos_t prev_prev_pos = polyline[(i - 2 + polyline.size()) % polyline.size()];
                point_frame.tangent = (curr_pos - prev_pos).normalized();
                const nous::vec3 up = (prev_prev_pos - prev_pos).normalized().cross(point_frame.tangent).normalized();
                const nous::vec3 normal = up.cross(point_frame.tangent).normalized();
                point_frame.up = up;
                point_frame.normal = normal;
                continue;
            }
        }

        point_frame.tangent = ((next_pos - curr_pos).normalized() + (curr_pos - prev_pos).normalized()).normalized();
        point_frame.up = (next_pos - curr_pos).normalized().cross((prev_pos - curr_pos).normalized()).normalized();
        point_frame.normal = point_frame.up.cross(point_frame.tangent).normalized();
        result.emplace_back(std::move(point_frame));
    }

    return result;
}

NurbsAlgebraSurfacePtr extrude_curve_to_surface(const NurbsCurvePtr shape_curve, const NurbsCurvePtr backbone_curve)
{
    // 使用 pointframes 的信息将 shape_curve 中的所有点旋转过去
    NurbsAlgebraSurfacePtr nurbs_surface = std::make_shared<NurbsAlgebraSurface>();
    auto& shape_curve_range = shape_curve->get_curve_range();
    nurbs_surface->set_surface_u_range(std::get<0>(shape_curve_range), std::get<1>(shape_curve_range));
    auto& backbone_curve_range = backbone_curve->get_curve_range();
    nurbs_surface->set_surface_v_range(std::get<0>(backbone_curve_range), std::get<1>(backbone_curve_range));

    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    surface_core->degree_u = shape_curve->get_curve_degree();
    surface_core->degree_v = backbone_curve->get_curve_degree();
    const std::vector<nous::scalar> shape_curve_knots = shape_curve->get_curve_knots();
    surface_core->knots_u.insert(surface_core->knots_u.end(), shape_curve_knots.begin(), shape_curve_knots.end());
    const std::vector<nous::scalar> backbone_curve_knots = backbone_curve->get_curve_knots();
    surface_core->knots_v.insert(surface_core->knots_v.end(), backbone_curve_knots.begin(), backbone_curve_knots.end());

    const std::vector<nous::scalar> shape_curve_weights = shape_curve->get_curve_weights();
    const std::vector<nous::scalar> backbone_curve_weights = backbone_curve->get_curve_weights();
    surface_core->weights.resize(shape_curve_weights.size(), backbone_curve_weights.size());
    for(int i = 0; i < shape_curve_weights.size(); ++i)
    {
        for(int j = 0; j < backbone_curve_weights.size(); ++j)
        {
            surface_core->weights(i, j) = shape_curve_weights[i] * backbone_curve_weights[j];
        }
    }

    const std::vector<SketchNurbsPointPtr> backbone_curve_control_points = backbone_curve->get_curve_control_points();
    nous::pos_t center_pos = backbone_curve_control_points[0]->m_point;

    // 要考虑到闭合曲线的点对象共享逻辑
    const unsigned int shape_curve_shared_point_number = get_curve_shared_point_number(shape_curve);
    const unsigned int backbone_curve_shared_point_number = get_curve_shared_point_number(backbone_curve);

    const std::vector<SketchNurbsPointPtr> shape_curve_control_points = shape_curve->get_curve_control_points();
    surface_core->control_points.resize(shape_curve_weights.size(), backbone_curve_weights.size());
    for(int i = 0; i < shape_curve_control_points.size() - shape_curve_shared_point_number; ++i)
    {
        for(int j = 0; j < backbone_curve_control_points.size() - backbone_curve_shared_point_number; ++j)
        {
            if(j == 0)
            {
                // 共享 shape_curve 的 ctp 作为底边
                surface_core->control_points(i, j) = shape_curve_control_points[i];
            }
            else
            {
                const nous::pos_t position = shape_curve_control_points[i]->m_point - center_pos + backbone_curve_control_points[j]->m_point;
                surface_core->control_points(i, j) = std::make_shared<SketchNurbsPoint>(position);
            }
        }
    }

    for(int i = 0; i < shape_curve_shared_point_number; ++i)
    {
        for(int j = 0; j < backbone_curve_control_points.size() - backbone_curve_shared_point_number; ++j)
        {
            surface_core->control_points(shape_curve_control_points.size() - shape_curve_shared_point_number + i, j) =
                surface_core->control_points(i, j);
        }
    }

    for(int j = 0; j < backbone_curve_shared_point_number; ++j)
    {
        for(int i = 0; i < shape_curve_shared_point_number; ++i)
        {
            surface_core->control_points(i, backbone_curve_control_points.size() - backbone_curve_shared_point_number + j) =
                surface_core->control_points(i, j);
        }
    }

    nurbs_surface->update_surface_data(surface_core);

    return nurbs_surface;
}

NurbsCurvePtr convert_polyline_to_curve(const std::vector<nous::pos_t>& polyline, bool closed)
{
    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> result = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();
    result->degree = 1;
    for(int i = 0; i < polyline.size(); ++i)
    {
        SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(polyline[i]);
        result->control_points.emplace_back(nurbs_point);
    }
    if(closed)
    {
        result->control_points.emplace_back(result->control_points.front());
    }
    result->weights.resize(result->control_points.size(), 1.0);
    result->knots.emplace_back(0.0);
    result->knots.emplace_back(0.0);
    for(int i = 0; i < result->control_points.size() - 1; ++i)
    {
        result->knots.emplace_back(result->knots.back() + (result->control_points[i + 1]->m_point - result->control_points[i]->m_point).norm());
    }
    const double max_knot = result->knots.back();
    result->knots.emplace_back(max_knot);
    for(auto& knot: result->knots)
    {
        knot /= max_knot;
    }

    NurbsCurvePtr curve = std::make_shared<NurbsCurve>();
    curve->update_curve_data(result);
    return curve;
}

void dp_simplify_polyline_core(const std::vector<nous::pos_t>& positions, const int left, const int right, const double ratio,
                               std::set<size_t, std::greater<size_t>>& delete_idx_set)
{
    if(left + 1 >= right)
    {
        return;
    }

    const nous::pos_t left_pos = positions[left];
    const nous::pos_t right_pos = positions[right];
    double left_to_right_distance = (left_pos - right_pos).norm();
    if(left_to_right_distance < 0.01)
    {
        left_to_right_distance = 0.01;
    }
    double max_distance = 0;
    int max_distance_idx = -1;
    for(int i = left + 1; i < right; ++i)
    {
        double distance = sketch_math::point_to_line_distance(positions[i], left_pos, right_pos, false);
        if(distance > max_distance)
        {
            max_distance = distance;
            max_distance_idx = i;
        }
    }

    if(max_distance_idx == -1)
    {
        return;
    }

    const double curr_ratio = max_distance / left_to_right_distance;
    if(curr_ratio <= ratio)
    {
        // left 到 right 之间的全删了
        for(int i = left + 1; i < right; ++i)
        {
            delete_idx_set.insert(i);
        }
    }
    else
    {
        dp_simplify_polyline_core(positions, left, max_distance_idx, ratio, delete_idx_set);
        dp_simplify_polyline_core(positions, max_distance_idx, right, ratio, delete_idx_set);
    }

    return;
}

/*
 * @brief dp 简化线算法
 * @param positions 输入的折线坐标
 * @param ratio 横纵向比值
 */
std::vector<nous::pos_t> dp_simplify_polyline(std::vector<nous::pos_t>& positions, const double ratio = 0.005)
{
    std::set<size_t, std::greater<size_t>> delete_idx_set;
    dp_simplify_polyline_core(positions, 0, positions.size() - 1, ratio, delete_idx_set);
    std::vector<nous::pos_t> simplified_positions;
    for(int i = 0; i < positions.size(); ++i)
    {
        if(delete_idx_set.find(i) == delete_idx_set.end())
        {
            simplified_positions.emplace_back(positions[i]);
        }
    }
    return simplified_positions;
}

std::vector<nous::pos_t> convert_curve_to_polyline(NurbsCurvePtr curve)
{
    std::vector<nous::pos_t> positions = curve->get_sampled_point_arr(10000);
    return dp_simplify_polyline(positions);
}

std::vector<nous::pos_t> convert_curve_to_polyline_with_limit(NurbsCurvePtr curve, int sample_count)
{
    std::vector<nous::pos_t> positions = curve->get_sampled_point_arr(10000);
    double ratio = 0.01;
    while(positions.size() > sample_count)
    {
        positions = dp_simplify_polyline(positions, ratio);
        ratio += 0.01;
    }
    return positions;
}

bool is_curve_coplane(NurbsCurvePtr curve)
{
    bool is_coplane = true;
    std::vector<SketchNurbsPointPtr> curve_control_points = curve->get_curve_control_points();
    const ENurbsCloseType close_type = curve->get_curve_close_type();
    if(close_type == ENurbsCloseType::SharpClose)
    {
        curve_control_points.pop_back();
    }

    nous::vec3 basic_normal;
    for(int i = 0; i < curve_control_points.size(); ++i)
    {
        const nous::pos_t& prev_pos = curve_control_points[(i - 1 + curve_control_points.size()) % curve_control_points.size()]->m_point;
        const nous::pos_t& curr_pos = curve_control_points[i]->m_point;
        const nous::pos_t& next_pos = curve_control_points[(i + 1) % curve_control_points.size()]->m_point;
        const nous::vec3& to_prev_dir = (prev_pos - curr_pos).normalized();
        const nous::vec3& to_next_dir = (next_pos - curr_pos).normalized();
        if(i == 0)
        {
            basic_normal = to_prev_dir.cross(to_next_dir).normalized();
        }
        else
        {
            const nous::vec3& other_normal = to_prev_dir.cross(to_next_dir);
            if(!sketch_math::direction_parallel(basic_normal, other_normal))
            {
                is_coplane = false;
                break;
            }
        }
    }
    return is_coplane;
}

std::vector<nous::scalar> calculate_ub(const std::vector<nous::pos_t>& fit_points)
{
    double d = 0.0;
    for(int i = 0; i < fit_points.size() - 1; ++i)
    {
        d += (fit_points[i + 1] - fit_points[i]).norm();
    }

    std::vector<nous::scalar> ub(fit_points.size());
    for(int i = 0; i < fit_points.size() - 1; ++i)
    {
        ub[i + 1] = ub[i] + (fit_points[i + 1] - fit_points[i]).norm() / d;
    }

    ub.back() = 1.0;

    return ub;
}

/*
 * @param ub 前置计算的参数
 * @param m + 1 为拟合点的数量
 * @param n + 1 为控制点的数量
 * @param p 为曲线的阶数
 */
std::vector<nous::scalar> calculate_knots(const std::vector<nous::scalar>& ub, const int m, const int n, const int p)
{
    const double d = static_cast<float>(m + 1) / static_cast<double>(n - p + 1);
    std::vector<nous::scalar> knots(n + p + 2, 0.0);

    for(size_t j = 1; j <= n - p; ++j)
    {
        nous::scalar& knot = knots[p + j];
        const int i = std::floor(j * d) + 1;
        const nous::scalar a = j * d + 1 - i;
        knot = (1 - a) * ub[i - 1] + a * ub[i];
    }

    for(int i = 0; i < p + 1; ++i)
    {
        knots[knots.size() - 1 - i] = 1.0;
    }

    return knots;
}

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> least_square_approximation(const size_t degree,
                                                                                   const std::vector<nous::pos_t>& through_points,
                                                                                   const int control_points_count)
{
    const int n = control_points_count;
    const int m = through_points.size();
    std::vector<nous::scalar> uk = tinynurbs::util::get_chord_parameterization(through_points);
    std::vector<nous::scalar> knot_vector;

    for(int i = 0; i < degree; ++i)
    {
        knot_vector.emplace_back(0.0);
    }
    knot_vector.insert(knot_vector.end(), uk.begin(), uk.end());
    for(int i = 0; i < degree; ++i)
    {
        knot_vector.emplace_back(1.0);
    }

    std::vector<nous::pos_t> control_points(n);
    std::vector<nous::scalar> weights(n, 1.0f);
    Eigen::Matrix<nous::scalar, -1, -1> matrix_r = Eigen::Matrix<nous::scalar, -1, -1>::Zero(3, n);
    Eigen::Matrix<nous::scalar, -1, -1> rk = Eigen::Matrix<nous::scalar, -1, -1>::Zero(3, m);
    const std::vector<nous::scalar> funs(degree + 1);
    Eigen::Matrix<nous::scalar, -1, -1> matrix_n = Eigen::Matrix<nous::scalar, -1, -1>::Zero(m, n);

    for(int i = 0; i < 3; ++i)
    {
        matrix_r(i, 0) = through_points[0].coeff(i);
        matrix_r(i, n - 1) = through_points[m - 1].coeff(i);
    }

    matrix_n(0, 0) = 1.0;
    matrix_n(m - 1, n - 1) = 1.0;

    for(int i = 0; i < m; ++i)
    {
        const int span_index = tinynurbs::findSpan(degree, knot_vector, uk[i]);
        std::vector<nous::scalar> basis = tinynurbs::bsplineBasis(degree, span_index, knot_vector, uk[i]);
        for(int j = 0; j < degree + 1; ++j)
        {
            matrix_n(i, span_index - degree + j) = basis[j];
        }

        const nous::pos_t tmp_pos = through_points[i] - matrix_n(i, 0) * through_points[0] - matrix_n(i, n - 1) * through_points[m - 1];
        for(int j = 0; j < 3; ++j)
        {
            rk(j, i) = tmp_pos.coeff(j);
        }
    }

    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < m; ++j)
        {
            // matrix_r(0, i) += matrix_n(j, i) * rk(0, j);
            matrix_r.col(i) += matrix_n(j, i) * rk.col(j);
        }

        if(matrix_r.col(i).isApprox(nous::pos_t::Zero()))
        {
            return nullptr;
        }
    }

    if(n - 2 > 0)
    {
        Eigen::Matrix<nous::scalar, -1, -1> matrix_b = Eigen::Matrix<nous::scalar, -1, -1>::Zero(n - 2, 3);
        Eigen::Matrix<nous::scalar, -1, -1> matrix_ns = Eigen::Matrix<nous::scalar, -1, -1>::Zero(m - 2, n - 2);
        for(int i = 0; i < n - 2; ++i)
        {
            matrix_b.row(i) = matrix_r.col(i + 1);
        }

        for(int i = 1; i < m - 1; ++i)
        {
            for(int j = 1; j < n - 1; ++j)
            {
                matrix_ns(i - 1, j - 1) = matrix_n(i, j);
            }
        }

        auto matrix_nst = matrix_ns.transpose();
        auto& matrix_nstns = matrix_nst * matrix_ns;
        Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> matrix_x = matrix_nstns.lu().solve(matrix_b);
        for(int i = 0; i < n - 2; ++i)
        {
            control_points[i + 1] = matrix_x.row(i);
        }
    }
    control_points[0] = through_points[0];
    control_points[n - 1] = through_points[m - 1];

    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> result = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();

    result->degree = degree;
    result->knots.swap(knot_vector);
    for(int i = 0; i < control_points.size(); ++i)
    {
        SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(control_points[i]);
        result->control_points.emplace_back(nurbs_point);
    }
    result->weights.swap(weights);
    return result;
}

std::shared_ptr<tinynurbs::Curve<nous::scalar>> wc_least_square_curve(const std::vector<nous::pos_t>& Q, const int r,
                                                                      const std::vector<nous::scalar>& Wq, const std::vector<nous::vec3>& D,
                                                                      const int s, const std::vector<int>& I, const std::vector<nous::scalar>& Wd,
                                                                      const int n, const int p)
{
    int ru = -1;
    int rc = -1;
    for(int i = 0; i <= r; ++i)
    {
        Wq[i] > 0.0 ? ru++ : rc++;
    }
    int su = -1;
    int sc = -1;
    for(int j = 0; j <= s; ++j)
    {
        Wd[j] > 0.0 ? su++ : sc++;
    }

    int mu = ru + su + 1;
    int mc = rc + sc + 1;

    if(mc >= n || mc + n >= mu + 1)
    {
        return nullptr;
    }

    /*用 (9.5) 式计算 Uk 并存入 ub[] 中*/
    const std::vector<nous::scalar>&& ub = calculate_ub(Q);

    /*用 (9.68) 式和 (9.69) 式计算节点并存入 U 中*/
    const std::vector<nous::scalar> U = calculate_knots(ub, r, n, p);

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> S = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>(mu + 1, 3);
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> T = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>(mc + 1, 3);
    Eigen::Vector<nous::scalar, Eigen::Dynamic> W = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(mu + 1);
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> A = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(mu + 1);
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> N = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(mu + 1, n + 1);
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> M = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(mc + 1, n + 1);

    int j = 0;
    int mu2 = 0;
    int mc2 = 0;
    for(int i = 0; i <= r; ++i)
    {
        const int span = tinynurbs::findSpan(p, U, ub[i]);
        int dflag = 0;
        if(j <= s && i == I[j])
        {
            dflag = 1;
        }

        // 对应伪代码的 funs[2][p+1]
        std::vector<float> funs_row0;
        std::vector<float> funs_row1;
        if(dflag == 0)
        {
            funs_row0 = tinynurbs::bsplineBasis(p, span, U, ub[i]);
        }
        else
        {
            auto& tmp = tinynurbs::bsplineDerBasis(p, span, U, ub[i], 1);
            funs_row0.clear();
            funs_row1.clear();
            for(int m = 0; m < p + 1; m++)
            {
                funs_row0.emplace_back(tmp[m]);
            }
            for(int m = 0; m < p + 1; m++)
            {
                funs_row1.emplace_back(tmp[p + 1 + m]);
            }
        }

        if(Wq[i] > 0.0)
        {
            // 非约束点
            W.data()[mu2] = Wq[i];
            // 将 funs[0] 存入 N[mu2]
            Eigen::Vector<nous::scalar, Eigen::Dynamic> new_row = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(n + 1);
            for(int m = 0; m < p + 1; ++m)
            {
                new_row.data()[span - p + m] = funs_row0[m];
            }
            N.row(mu2) = new_row;
            S.row(mu2) = W.data()[mu2] * Q[i];
            mu2++;
        }
        else
        {
            // 约束点
            // 将 funs[0] 存入 N[mu2]
            Eigen::Vector<nous::scalar, Eigen::Dynamic> new_row = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(n + 1);
            for(int m = 0; m < p + 1; ++m)
            {
                new_row.data()[span - p + m] = funs_row0[m];
            }
            M.row(mc2) = new_row;
            T.row(mc2) = Q[i];
            mc2++;
        }

        if(dflag == 1)
        {
            if(Wd[j] > 0.0)
            {
                // 非约束导矢
                W.data()[mu2] = Wd[j];
                Eigen::Vector<nous::scalar, Eigen::Dynamic> new_row = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(n + 1);
                for(int m = 0; m < p + 1; ++m)
                {
                    new_row.data()[span - p + m] = funs_row1[m];
                }
                N.row(mu2) = new_row;
                S.row(mu2) = W.data()[mu2] * D[j];
                mu2++;
            }
            else
            {
                Eigen::Vector<nous::scalar, Eigen::Dynamic> new_row = Eigen::Vector<nous::scalar, Eigen::Dynamic>::Zero(n + 1);
                for(int m = 0; m < p + 1; ++m)
                {
                    new_row.data()[span - p + m] = funs_row1[m];
                }
                M.row(mc2) = new_row;
                T.row(mc2) = D[j];
                mc2++;
            }
        }

        j++;
    }

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> W_Mat =
        Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(mu + 1, mu + 1);
    for(int i = 0; i < W.size(); ++i)
    {
        W_Mat.data()[i * (mu + 1) + i] = W[i];
    }
    // 计算矩阵 N^T * W * N 和 N^T * W * S
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> NTWN = N.transpose() * W_Mat * N;
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> NTWS = N.transpose() * W_Mat * S;

    std::shared_ptr<tinynurbs::Curve<nous::scalar>> result = std::make_shared<tinynurbs::Curve<nous::scalar>>();
    result->degree = p;
    result->knots.insert(result->knots.end(), U.begin(), U.end());

    if(mc < 0)
    {
        // 无约束
        // 此时 (9.71) 式简化为 (NTWN)P = NTWS
        Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> P = NTWN.lu().solve(NTWS);

        for(int i = 0; i < P.rows(); ++i)
        {
            SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(P.row(i));
            result->control_points.emplace_back(nurbs_point);
        }

        return result;
    }

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> NTWN_INV = NTWN.inverse();
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> M_NTWN_INV_MT = M * NTWN_INV * M.transpose();

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> M_NTWN_INV_NTWS_MINUS_T = M * NTWN_INV * NTWS - T;
    A = M_NTWN_INV_MT.inverse() * M_NTWN_INV_NTWS_MINUS_T;

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> P = NTWN_INV * (NTWS - M.transpose() * A);

    for(int i = 0; i < P.rows(); ++i)
    {
        SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(P.row(i));
        result->control_points.emplace_back(nurbs_point);
    }
    return result;
}

std::shared_ptr<tinynurbs::Curve<nous::scalar>> local_cubic_bspline_interpolation_from_tangent(const std::vector<nous::pos_t>& fit_points,
                                                                                               const std::vector<nous::vec3>& tangents)
{
    const unsigned int degree = 3;
    const unsigned int order = degree + 1;
    std::vector<nous::pos_t> control_points;
    control_points.emplace_back(fit_points.front());
    float u = 0.0;
    std::vector<nous::scalar> params;

    for(int i = 0; i < fit_points.size() - 1; ++i)
    {
        const nous::pos_t& p0 = fit_points[i];
        const nous::pos_t& p3 = fit_points[i + 1];
        const nous::vec3& t0 = tangents[i];
        const nous::vec3& t3 = tangents[i + 1];

        const double a = 16.0 - (t0 + t3).squaredNorm();
        const double b = 12.0 * (p3 - p0).dot(t0 + t3);
        const double c = -36.0 * (p3 - p0).squaredNorm();

        const double b2m4ac = pow(b, 2) - 4 * a * c;
        if(b2m4ac < 0)
        {
            return nullptr;
        }
        const double sqrt_b2m4ac = sqrt(b2m4ac);
        const double alpha_plus = (-b + sqrt_b2m4ac) / (2 * a);
        const double alpha_minus = (-b - sqrt_b2m4ac) / (2 * a);

        const nous::pos_t p1 = p0 + alpha_plus * t0 / 3.0;
        const nous::pos_t p2 = p3 - alpha_plus * t3 / 3.0;

        control_points.emplace_back(p1);
        control_points.emplace_back(p2);
        u += 3.0 * (p1 - p0).norm();
        params.emplace_back(u);
    }
    control_points.emplace_back(fit_points.back());

    bool curve_closed = sketch_math::point_equal(fit_points.front(), fit_points.back());
    std::shared_ptr<tinynurbs::Curve<nous::scalar>> result = std::make_shared<tinynurbs::Curve<nous::scalar>>();
    for(int i = 0; i < control_points.size(); ++i)
    {
        if(i == control_points.size() - 1 && curve_closed)
        {
            result->control_points.emplace_back(result->control_points.front());
        }
        else
        {
            SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(control_points[i]);
            result->control_points.emplace_back(nurbs_point);
        }
    }
    result->degree = 3;
    result->knots.resize(order, 0.0);
    for(int i = 0; i < params.size() - 1; ++i)
    {
        result->knots.emplace_back(params[i] / u);
        result->knots.emplace_back(params[i] / u);
    }
    for(int i = 0; i < order; ++i)
    {
        result->knots.emplace_back(1.0);
    }

    return result;
}

void curve_polyline_params(const unsigned int n, const std::vector<nous::pos_t>& Q, std::vector<nous::scalar>& uk)
{
    uk.clear();
    uk.resize(n + 1, 0.0);

    for(int i = 1; i <= n; ++i)
    {
        uk[i] = uk[i - 1] + sketch_math::point_to_point_distance(Q[i], Q[i - 1]);
    }

    const nous::scalar uk_max = uk.back();
    for(int i = 1; i <= n; ++i)
    {
        uk[i] /= uk_max;
    }

    return;
}

void surface_mesh_params(const unsigned int n, const unsigned int m, const std::vector<std::vector<nous::pos_t>>& Q, std::vector<nous::scalar>& uk,
                         std::vector<nous::scalar>& vl)
{
    uk.clear();
    vl.clear();

    unsigned int num_m = m + 1;
    unsigned int num_n = n + 1;
    uk.resize(num_n, 0.0);
    uk[n] = 1.0;
    vl.resize(num_m, 0.0);
    vl[m] = 1.0;

    for(int l = 0; l <= m; ++l)
    {
        double total = 0.0;
        std::vector<double> cds;
        cds.emplace_back(0.0);
        for(int k = 1; k <= n; ++k)
        {
            cds.emplace_back(sketch_math::point_to_point_distance(Q[k][l], Q[k - 1][l]));
            total += cds[k];
        }
        if(total == 0.0)
        {
            --num_m;
        }
        else
        {
            double d = 0.0;
            for(int k = 1; k < n; ++k)
            {
                d = d + cds[k];
                uk[k] = uk[k] + d / total;
            }
        }
    }

    if(num_m == 0)
    {
        throw -1;
    }

    for(int k = 1; k < n; ++k)
    {
        uk[k] /= num_m;
    }

    for(int l = 0; l <= n; ++l)
    {
        double total = 0.0;
        std::vector<double> cds;
        cds.emplace_back(0.0);
        for(int k = 1; k <= m; ++k)
        {
            cds.emplace_back(sketch_math::point_to_point_distance(Q[l][k], Q[l][k - 1]));
            total += cds[k];
        }
        if(total == 0.0)
        {
            --num_n;
        }
        else
        {
            double d = 0.0;
            for(int k = 1; k < m; ++k)
            {
                d = d + cds[k];
                vl[k] = vl[k] + d / total;
            }
        }
    }

    if(num_m == 0)
    {
        throw -1;
    }

    for(int k = 1; k < m; ++k)
    {
        vl[k] /= num_n;
    }
}

std::vector<nous::scalar> generate_knots_by_nb98_func(const std::vector<nous::scalar>& knot_d, const unsigned int degree)
{
    std::vector<nous::scalar> knots;

    for(int i = 0; i < degree + 1; ++i)
    {
        knots.emplace_back(nous::scalar(0.0));
    }
    for(int j = 1; j <= knot_d.size() - degree - 1; ++j)
    {
        double sum_knot = 0.0;
        for(int i = j; i < j + degree; ++i)
        {
            sum_knot += knot_d[i];
        }
        const nous::scalar avg_knot = sum_knot / degree;
        knots.emplace_back(avg_knot);
    }
    for(int i = 0; i < degree + 1; ++i)
    {
        knots.emplace_back(nous::scalar(1.0));
    }

    return knots;
}

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> global_curve_interpolation(const unsigned int n, const std::vector<nous::pos_t>& Q,
                                                                                   const unsigned int p)
{
    std::vector<nous::scalar> uk;
    curve_polyline_params(n, Q, uk);
    return global_curve_interpolation(n, Q, p, uk);
}

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> global_curve_interpolation(const unsigned int n, const std::vector<nous::pos_t>& Q,
                                                                                   const unsigned int p, const std::vector<nous::scalar>& uk)
{
    const unsigned int knot_number = n + p + 1;
    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> A = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(n + 1, n + 1);
    const auto&& U = generate_knots_by_nb98_func(uk, p);
    for(int i = 0; i <= n; ++i)
    {
        unsigned int span = tinynurbs::findSpan(p, U, uk[i]);
        const auto&& basis = tinynurbs::bsplineBasis(p, span, U, uk[i]);
        for(int j = span - p; j <= span; ++j)
        {
            A(i, j) = basis[j - span + p];
        }
    }

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> QM = Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(n + 1, 3);
    for(int i = 0; i <= n; ++i)
    {
        QM.row(i) = Q[i];
    }

    Eigen::Matrix<nous::scalar, Eigen::Dynamic, Eigen::Dynamic> result = A.lu().solve(QM);

    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> curve = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();
    for(int i = 0; i < result.rows(); ++i)
    {
        SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(result.row(i));
        curve->control_points.emplace_back(nurbs_point);
        curve->weights.emplace_back(1.0);
    }
    curve->degree = p;
    curve->knots.insert(curve->knots.end(), U.begin(), U.end());

    // result 里存的控制点
    return curve;
}

std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> global_surface_interpolation(const unsigned int n, const unsigned int m,
                                                                                       const std::vector<std::vector<nous::pos_t>>& Q,
                                                                                       const unsigned int p, const unsigned int q)
{
    // n = rows - 1;
    // m = cols - 1;
    // uk.size() == rows;
    // vl.size() == cols;
    std::vector<nous::scalar> uk;// row
    std::vector<nous::scalar> vl;// col
    surface_mesh_params(n, m, Q, uk, vl);
    const std::vector<nous::scalar>&& knots_u = generate_knots_by_nb98_func(uk, p);
    const std::vector<nous::scalar>&& knots_v = generate_knots_by_nb98_func(vl, q);

    std::vector<std::vector<nous::pos_t>> R((n + 1), std::vector<nous::pos_t>((m + 1), nous::pos_t::Zero()));
    std::vector<std::vector<nous::pos_t>> P((n + 1), std::vector<nous::pos_t>((m + 1), nous::pos_t::Zero()));

    for(int l = 0; l <= m; ++l)
    {
        std::vector<nous::pos_t> col_Q;
        for(int i = 0; i <= n; ++i)
        {
            col_Q.emplace_back(Q[i][l]);
        }
        auto& curve = global_curve_interpolation(n, col_Q, p, uk);
        for(int i = 0; i <= n; ++i)
        {
            R[i][l] = curve->control_points[i]->m_point;
        }
    }

    for(int i = 0; i <= n; ++i)
    {
        auto& curve = global_curve_interpolation(m, Q[i], q, vl);
        for(int l = 0; l <= m; ++l)
        {
            R[i][l] = curve->control_points[l]->m_point;
        }
    }

    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> surface = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    surface->degree_u = p;
    surface->degree_v = q;
    surface->knots_u.insert(surface->knots_u.end(), knots_u.begin(), knots_u.end());
    surface->knots_v.insert(surface->knots_v.end(), knots_v.begin(), knots_v.end());
    surface->control_points.resize(n + 1, m + 1, nullptr);
    surface->weights.resize(n + 1, m + 1, nous::scalar(1.0));

    for(int i = 0; i <= n; ++i)
    {
        for(int j = 0; j <= m; ++j)
        {
            if(j == m && sketch_math::point_equal(R[i][j], R[i][0]))
            {
                surface->control_points(i, j) = surface->control_points(i, 0);
            }
            else if(i == n && sketch_math::point_equal(R[i][j], R[0][j]))
            {
                surface->control_points(i, j) = surface->control_points(0, j);
            }
            else
            {
                SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(R[i][j]);
                surface->control_points(i, j) = nurbs_point;
            }
        }
    }

    return surface;
}

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> solve_tridiagonal(const std::vector<nous::pos_t>& fit_points, const bool auto_head_tangent,
                                                                          const nous::vec3 head_tangent, const bool auto_tail_tangent,
                                                                          const nous::vec3 tail_tangent, const ENurbsCloseType type)
{
    unsigned int degree = 3;
    unsigned int control_point_number = fit_points.size() + 2;
    unsigned int knot_number = control_point_number + degree + 1;
    unsigned int n = fit_points.size() - 1;

    std::vector<nous::scalar> knots_arr(knot_number, 0.0);
    for(int i = 0; i < fit_points.size() - 1; ++i)
    {
        double distance = (fit_points[i + 1] - fit_points[i]).norm();
        knots_arr[degree + 1 + i] = knots_arr[degree + i] + distance;
    }

    for(int i = 0; i < degree; ++i)
    {
        knots_arr[degree + fit_points.size() + i] = knots_arr[degree + fit_points.size() + i - 1];
    }

    // how to determine D_0 and D_n, when no user input.
    nous::vec3 D_0 = head_tangent.normalized();
    if(!auto_head_tangent)
    {
        D_0 = (fit_points[1] - fit_points[0]).normalized() * knots_arr.back();
        if(fit_points.size() > 2)
        {
            const nous::vec3 next_tangent = (fit_points[2] - fit_points[1]).normalized();
            const nous::vec3 mixed_tangent = (next_tangent + D_0.normalized() * 4).normalized();
            nous::transform3 transform = nous::transform3::Identity();
            transform.rotate(Eigen::AngleAxisf(3.141592653, D_0.normalized()));
            D_0 = transform * mixed_tangent * knots_arr.back();
        }
    }

    nous::vec3 D_n = tail_tangent.normalized();
    if(auto_tail_tangent)
    {
        D_n = (fit_points[fit_points.size() - 1] - fit_points[fit_points.size() - 2]).normalized() * knots_arr.back();
        if(fit_points.size() > 2)
        {
            const nous::vec3 prev_tangent = (fit_points[fit_points.size() - 2] - fit_points[fit_points.size() - 1]).normalized();
            D_n = (prev_tangent + D_n.normalized() * 4).normalized();
        }
    }

    bool curve_closed = (fit_points.front() - fit_points.back()).norm() < 0.01;
    if(curve_closed && type == ENurbsCloseType::SmoothClose)
    {
        D_0 = D_n =
            ((fit_points[1] - fit_points[0]).normalized() + (fit_points[fit_points.size() - 1] - fit_points[fit_points.size() - 2]).normalized())
                .normalized() *
            knots_arr.back();
    }

    for(int i = 0; i < knots_arr.size(); ++i)
    {
        knots_arr[i] /= knots_arr.back();
    }

    std::vector<nous::pos_t> control_points(control_point_number, nous::pos_t::Zero());
    control_points[0] = fit_points[0];
    control_points[1] = knots_arr[4] / 3.0 * D_0 + fit_points[0];
    control_points[control_point_number - 1] = fit_points[fit_points.size() - 1];
    control_points[control_point_number - 2] = fit_points[fit_points.size() - 1] - (knots_arr.back() - knots_arr[n + 2]) / 3.0 * D_n;

    if(control_points.size() == 5)
    {
        const auto&& abc = tinynurbs::bsplineBasis(3, 4, knots_arr, knots_arr[4]);
        control_points[2] = (fit_points[1] - abc[0] * control_points[1] - abc[2] * control_points[3] - abc[3] * control_points[4]) / abc[1];
    }
    else if(control_points.size() > 5)
    {
        std::vector<nous::pos_t> R(n + 1);
        std::vector<nous::scalar> dd(n + 1, 0.0);
        for(int i = 3; i < n; ++i)
        {
            R[i] = fit_points[i - 1];
        }

        auto&& abc = tinynurbs::bsplineBasis(3, 4, knots_arr, knots_arr[4]);
        float den = abc[1];
        control_points[2] = (fit_points[1] - abc[0] * control_points[1]) / den;
        for(int i = 3; i < n; ++i)
        {
            dd[i] = abc[2] / den;
            abc = tinynurbs::bsplineBasis(3, i + 2, knots_arr, knots_arr[i + 2]);
            den = abc[1] - abc[0] * dd[i];
            control_points[i] = (R[i] - abc[0] * control_points[i - 1]) / den;
        }
        dd[n] = abc[2] / den;
        abc = tinynurbs::bsplineBasis(3, n + 2, knots_arr, knots_arr[n + 2]);
        den = abc[1] - abc[0] * dd[n];
        control_points[n] = (fit_points[n - 1] - abc[2] * control_points[n + 1] - abc[0] * control_points[n - 1]) / den;
        for(int i = n - 1; i >= 2; --i)
        {
            control_points[i] = control_points[i] - dd[i + 1] * control_points[i + 1];
        }
    }

    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> result = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();
    result->degree = degree;
    for(int i = 0; i < control_points.size() - curve_closed; ++i)
    {
        SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(control_points[i]);
        result->control_points.emplace_back(nurbs_point);
    }
    if(curve_closed)
    {
        result->control_points.emplace_back(result->control_points.front());
    }
    result->knots.insert(result->knots.end(), knots_arr.begin(), knots_arr.end());
    result->weights.resize(result->control_points.size(), 1.0);
    return result;
}

void convert_nurbs_surface_to_mesh(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, const nous::mat4& nurbs_trans)
{
    if(surface)
    {
        nous::mat4 mesh_trans = NousConvert::convert(mesh_doc->world_matrix().inverse());

        const unsigned int surface_degree_u = surface->get_surface_degree_u();
        const unsigned int surface_degree_v = surface->get_surface_degree_v();

        const std::vector<nous::scalar> surface_knots_u = surface->get_surface_knots_u();
        const std::vector<nous::scalar> surface_knots_v = surface->get_surface_knots_v();
        auto& range_u = surface->get_surface_u_range_with_limit();
        auto& range_v = surface->get_surface_v_range_with_limit();
        const nous::scalar min_u = std::get<0>(range_u);
        const nous::scalar max_u = std::get<1>(range_u);
        const nous::scalar min_v = std::get<0>(range_v);
        const nous::scalar max_v = std::get<1>(range_v);

        std::vector<nous::scalar> sample_u_arr;
        std::vector<nous::scalar> sample_v_arr;
        if(surface_degree_u == 1)
        {
            sample_u_arr.insert(sample_u_arr.end(), surface_knots_u.begin() + 1, surface_knots_u.end() - 1);
        }
        else
        {
            for(int i = 0; i <= 9; ++i)
            {
                const nous::scalar u = min_u + (max_u - min_u) / 9.0 * i;
                sample_u_arr.emplace_back(u);
            }
        }

        if(surface_degree_v == 1)
        {
            sample_v_arr.insert(sample_v_arr.end(), surface_knots_v.begin() + 1, surface_knots_v.end() - 1);
        }
        else
        {
            for(int i = 0; i <= 9; ++i)
            {
                const nous::scalar v = min_v + (max_v - min_v) / 9.0 * i;
                sample_v_arr.emplace_back(v);
            }
        }

        unsigned int rows = sample_u_arr.size();
        unsigned int cols = sample_v_arr.size();
        std::vector<std::vector<nous::pos_t>> sample_positions;
        for(int i = 0; i < sample_u_arr.size(); ++i)
        {
            auto& row_positions = sample_positions.emplace_back();
            for(int j = 0; j < sample_v_arr.size(); ++j)
            {
                const nous::scalar u = sample_u_arr[i];
                const nous::scalar v = sample_v_arr[j];
                const nous::pos_t pos = surface->get_position(u, v);
                nous::pos_t trans_pos = NousConvert::product_v3(nurbs_trans, pos);
                trans_pos = NousConvert::product_v3(mesh_trans, trans_pos);
                row_positions.emplace_back(trans_pos);
            }
        }

        for(int i = 0; i < sample_u_arr.size() - 1; ++i)
        {
            for(int j = 0; j < sample_v_arr.size() - 1; ++j)
            {
                const size_t p0 = i * cols + j;
                const size_t p1 = i * cols + (j + 1);
                const size_t p2 = (i + 1) * cols + j;
                const size_t p3 = (i + 1) * cols + (j + 1);

                mesh_doc->geodata()->add_polygon({ { sample_positions[i][j], sample_positions[i][j + 1], sample_positions[i + 1][j] } });
                mesh_doc->geodata()->add_polygon({ { sample_positions[i][j + 1], sample_positions[i + 1][j + 1], sample_positions[i + 1][j] } });
            }
        }

        std::unordered_set<SketchElementPtr> inside_line_set;
        for(auto& line: mesh_doc->geodata()->get_all_lines())
        {
            if(mesh_doc->geodata()->get_line_adj_rings(line.second).size() == 2)
            {
                inside_line_set.insert(line.second);
            }
        }
        GeoDataTopoOperations::get().try_merge_smooth_group(mesh_doc->geodata().get(), inside_line_set, true);

        // 依次处理四条线呐
        {
            NoBranchLineGroupPtr weld_line_u0 = std::make_shared<NoBranchLineGroup>();
            for(int i = 0; i < rows - 1; ++i)
            {
                auto&& lines = mesh_doc->geodata()->add_line(sample_positions[i][0], sample_positions[i + 1][0]);
                for(auto& line: lines)
                {
                    weld_line_u0->add_element(line);
                }
            }
            mesh_doc->geodata()->add_element_group(weld_line_u0);
            mesh_doc->dirty();
        }
        {
            NoBranchLineGroupPtr weld_line_u1 = std::make_shared<NoBranchLineGroup>();
            for(int i = 0; i < rows - 1; ++i)
            {
                auto&& lines = mesh_doc->geodata()->add_line(sample_positions[i][cols - 1], sample_positions[i + 1][cols - 1]);
                for(auto& line: lines)
                {
                    weld_line_u1->add_element(line);
                }
            }
            mesh_doc->geodata()->add_element_group(weld_line_u1);
            mesh_doc->dirty();
        }
        {
            NoBranchLineGroupPtr weld_line_v0 = std::make_shared<NoBranchLineGroup>();
            for(int i = 0; i < cols - 1; ++i)
            {
                auto&& lines = mesh_doc->geodata()->add_line(sample_positions[0][i], sample_positions[0][i + 1]);
                for(auto& line: lines)
                {
                    weld_line_v0->add_element(line);
                }
            }
            mesh_doc->geodata()->add_element_group(weld_line_v0);
            mesh_doc->dirty();
        }
        {
            NoBranchLineGroupPtr weld_line_v1 = std::make_shared<NoBranchLineGroup>();
            for(int i = 0; i < cols - 1; ++i)
            {
                auto&& lines = mesh_doc->geodata()->add_line(sample_positions[rows - 1][i], sample_positions[rows - 1][i + 1]);
                for(auto& line: lines)
                {
                    weld_line_v1->add_element(line);
                }
            }
            mesh_doc->geodata()->add_element_group(weld_line_v1);
            mesh_doc->dirty();
        }
    }
}

void convert_nurbs_surface_to_mesh(MeshDoc* mesh_doc, NurbsDoc* nurbs_doc, std::shared_ptr<INurbsBase> nurbs_item)
{
    if(nurbs_item->type() != EElementType::TrimmedSurface)
    {
        return;
    }

    NurbsTrimmedSurfacePtr trimmed_surface = nurbs_item->to_nurbs_trimmed_surface();

    nous::vec3 coplane_normal;
    const bool is_surface_coplane = trimmed_surface->is_surface_coplane(coplane_normal);
    EElementType cs_ex_type = trimmed_surface->get_surface_exring_type();

    nous::mat4 nurbs_doc_world_mat = NousConvert::convert(nurbs_doc->world_matrix());
    nous::mat4 mesh_doc_inv_world_mat = NousConvert::convert(mesh_doc->world_matrix().inverse());

    // coplane 只需要拿边界信息，内部全部不管，因此简单一些
    if(is_surface_coplane && cs_ex_type == EElementType::ClosedSurface)
    {
        std::vector<std::vector<nous::pos_t>> polygon_data;
        NurbsClosedSurfacePtr closed_surface_exring = trimmed_surface->get_exterior_ring()->to_nurbs_closed_surface();
        auto&& exring_boundary_positions = convert_surface_boundary_to_polyline(closed_surface_exring);
        polygon_data.emplace_back(exring_boundary_positions);

        for(int i = 0; i < trimmed_surface->get_inring_number(); ++i)
        {
            INurbsSurfaceBasePtr inring_surface_base = trimmed_surface->get_interior_ring_at(i);
            auto&& inring_boundary_positions = convert_surface_boundary_to_polyline(inring_surface_base);
            polygon_data.emplace_back(inring_boundary_positions);
        }

        // 转坐标空间
        for(auto& ring_pos_arr: polygon_data)
        {
            for(int i = 0; i < ring_pos_arr.size(); ++i)
            {
                const nous::pos_t position = ring_pos_arr[i];
                const nous::vec4 position4 = nous::vec4(position.x(), position.y(), position.z(), 1.0);
                const nous::vec4 pos_trans1 = nurbs_doc_world_mat * position4;
                const nous::vec4 pos_trans2 = mesh_doc_inv_world_mat * pos_trans1;
                ring_pos_arr[i] = pos_trans2.head<3>() / pos_trans2.w();
            }
        }

        // 一条条线加吧，稳一点
        for(int j = 0; j < polygon_data.size(); ++j)
        {
            std::shared_ptr<NoBranchLineGroup> weld_line_group = std::make_shared<NoBranchLineGroup>();
            const auto& ring_pos_arr = polygon_data[j];
            for(int i = 0; i < ring_pos_arr.size(); ++i)
            {
                const nous::pos_t& pt_pos0 = ring_pos_arr[i];
                const nous::pos_t& pt_pos1 = ring_pos_arr[(i + 1) % ring_pos_arr.size()];
                std::vector<SketchLinePtr> added_skls = mesh_doc->geodata()->add_line(pt_pos0, pt_pos1, false);
                for(auto& line: added_skls)
                {
                    weld_line_group->add_element(line);
                }
            }
            mesh_doc->geodata()->add_element_group(weld_line_group);
            mesh_doc->geodata()->dirty();
            mesh_doc->dirty();
        }

        // 最后加入多边形对象即可
        auto&& added_polygon_points = mesh_doc->geodata()->add_polygon(polygon_data);
        mesh_doc->dirty();
    }
    else if(cs_ex_type == EElementType::AlgebraSurface)
    {
        NurbsAlgebraSurfacePtr algebra_surface = trimmed_surface->get_exterior_ring()->to_nurbs_algebra_surface();
        if(algebra_surface)
        {
            const unsigned int surface_degree_u = algebra_surface->get_surface_degree_u();
            const unsigned int surface_degree_v = algebra_surface->get_surface_degree_v();

            const std::vector<nous::scalar> surface_knots_u = algebra_surface->get_surface_knots_u();
            const std::vector<nous::scalar> surface_knots_v = algebra_surface->get_surface_knots_v();
            auto& range_u = algebra_surface->get_surface_u_range_with_limit();
            auto& range_v = algebra_surface->get_surface_v_range_with_limit();
            const nous::scalar min_u = std::get<0>(range_u);
            const nous::scalar max_u = std::get<1>(range_u);
            const nous::scalar min_v = std::get<0>(range_v);
            const nous::scalar max_v = std::get<1>(range_v);

            std::vector<nous::scalar> sample_u_arr;
            std::vector<nous::scalar> sample_v_arr;
            if(surface_degree_u == 1)
            {
                sample_u_arr.insert(sample_u_arr.end(), surface_knots_u.begin() + 1, surface_knots_u.end() - 1);
            }
            else
            {
                for(int i = 0; i <= 9; ++i)
                {
                    const nous::scalar u = min_u + (max_u - min_u) / 9.0 * i;
                    sample_u_arr.emplace_back(u);
                }
            }

            if(surface_degree_v == 1)
            {
                sample_v_arr.insert(sample_v_arr.end(), surface_knots_v.begin() + 1, surface_knots_v.end() - 1);
            }
            else
            {
                for(int i = 0; i <= 9; ++i)
                {
                    const nous::scalar v = min_v + (max_v - min_v) / 9.0 * i;
                    sample_v_arr.emplace_back(v);
                }
            }

            unsigned int rows = sample_u_arr.size();
            unsigned int cols = sample_v_arr.size();
            std::vector<std::vector<nous::pos_t>> sample_positions;
            for(int i = 0; i < sample_u_arr.size(); ++i)
            {
                auto& row_positions = sample_positions.emplace_back();
                for(int j = 0; j < sample_v_arr.size(); ++j)
                {
                    const nous::scalar u = sample_u_arr[i];
                    const nous::scalar v = sample_v_arr[j];
                    const nous::pos_t pos = algebra_surface->get_position(u, v);
                    row_positions.emplace_back(pos);
                }
            }

            for(int i = 0; i < sample_u_arr.size() - 1; ++i)
            {
                for(int j = 0; j < sample_v_arr.size() - 1; ++j)
                {
                    const size_t p0 = i * cols + j;
                    const size_t p1 = i * cols + (j + 1);
                    const size_t p2 = (i + 1) * cols + j;
                    const size_t p3 = (i + 1) * cols + (j + 1);

                    mesh_doc->geodata()->add_polygon({ { sample_positions[i][j], sample_positions[i][j + 1], sample_positions[i + 1][j] } });
                    mesh_doc->geodata()->add_polygon({ { sample_positions[i][j + 1], sample_positions[i + 1][j + 1], sample_positions[i + 1][j] } });
                }
            }

            std::unordered_set<SketchElementPtr> inside_line_set;
            for(auto& line: mesh_doc->geodata()->get_all_lines())
            {
                if(mesh_doc->geodata()->get_line_adj_rings(line.second).size() == 2)
                {
                    inside_line_set.insert(line.second);
                }
            }
            GeoDataTopoOperations::get().try_merge_smooth_group(mesh_doc->geodata().get(), inside_line_set, true);

            // 依次处理四条线呐
            {
                NoBranchLineGroupPtr weld_line_u0 = std::make_shared<NoBranchLineGroup>();
                for(int i = 0; i < rows - 1; ++i)
                {
                    auto&& lines = mesh_doc->geodata()->add_line(sample_positions[i][0], sample_positions[i + 1][0]);
                    for(auto& line: lines)
                    {
                        weld_line_u0->add_element(line);
                    }
                }
                mesh_doc->geodata()->add_element_group(weld_line_u0);
                mesh_doc->dirty();
            }
            {
                NoBranchLineGroupPtr weld_line_u1 = std::make_shared<NoBranchLineGroup>();
                for(int i = 0; i < rows - 1; ++i)
                {
                    auto&& lines = mesh_doc->geodata()->add_line(sample_positions[i][cols - 1], sample_positions[i + 1][cols - 1]);
                    for(auto& line: lines)
                    {
                        weld_line_u1->add_element(line);
                    }
                }
                mesh_doc->geodata()->add_element_group(weld_line_u1);
                mesh_doc->dirty();
            }
            {
                NoBranchLineGroupPtr weld_line_v0 = std::make_shared<NoBranchLineGroup>();
                for(int i = 0; i < cols - 1; ++i)
                {
                    auto&& lines = mesh_doc->geodata()->add_line(sample_positions[0][i], sample_positions[0][i + 1]);
                    for(auto& line: lines)
                    {
                        weld_line_v0->add_element(line);
                    }
                }
                mesh_doc->geodata()->add_element_group(weld_line_v0);
                mesh_doc->dirty();
            }
            {
                NoBranchLineGroupPtr weld_line_v1 = std::make_shared<NoBranchLineGroup>();
                for(int i = 0; i < cols - 1; ++i)
                {
                    auto&& lines = mesh_doc->geodata()->add_line(sample_positions[rows - 1][i], sample_positions[rows - 1][i + 1]);
                    for(auto& line: lines)
                    {
                        weld_line_v1->add_element(line);
                    }
                }
                mesh_doc->geodata()->add_element_group(weld_line_v1);
                mesh_doc->dirty();
            }
        }

        //// 显然现在的系统里参数面不会带洞，因此拿出来单独做简化处理
        // std::vector<nous::pos_t> sampled_positions;
        // std::vector<nous::vec3i> sampled_triangles;
        // const unsigned int rows = 10;
        // const unsigned int cols = 10;
        // trimmed_surface->get_sampled_triangles_low_resolution_when_exring_algebra_surface(rows, cols, sampled_positions, sampled_triangles);
        // std::vector<SketchPointPtr> sampled_points;
        // for(int i = 0; i < sampled_positions.size(); ++i)
        //{
        //     const nous::pos_t position = sampled_positions[i];
        //     const nous::vec4 position4 = nous::vec4(position.x(), position.y(), position.z(), 1.0);
        //     const nous::vec4 pos_trans1 = nurbs_doc_world_mat * position4;
        //     const nous::vec4 pos_trans2 = mesh_doc_inv_world_mat * pos_trans1;
        //     const nous::pos_t final_pos = pos_trans2.head<3>() / pos_trans2.w();
        //     sampled_points.emplace_back(mesh_doc->geodata()->add_point_without_check(final_pos));
        // }

        // SmoothGroupPtr smooth_group = std::make_shared<SmoothGroup>();
        // bool has_valid_triangle = false;
        // for(int i = 0; i < sampled_triangles.size(); ++i)
        //{
        //     const nous::vec3i one_triangle = sampled_triangles[i];
        //     const SketchPointPtr p0 = sampled_points[one_triangle.x()];
        //     const SketchPointPtr p1 = sampled_points[one_triangle.y()];
        //     const SketchPointPtr p2 = sampled_points[one_triangle.z()];
        //     auto& skp = mesh_doc->geodata()->add_polygon_without_check({ { p0, p1, p2 } }, true);
        //     smooth_group->add_element(skp);
        //     has_valid_triangle = true;
        // }

        // if(has_valid_triangle)
        //{
        //     mesh_doc->geodata()->add_element_group(smooth_group);
        //     mesh_doc->dirty();
        // }

        //// 焊接边线，判别边线是否存在
        // const bool surface_u_closed = trimmed_surface->get_exterior_ring()->to_nurbs_algebra_surface()->is_surface_u_closed();
        // const bool surface_v_closed = trimmed_surface->get_exterior_ring()->to_nurbs_algebra_surface()->is_surface_v_closed();
    }
    else
    {
        // 这里先走通流程，用三角面拼接起来
        std::vector<nous::pos_t> sampled_positions;
        std::vector<nous::vec3i> sampled_triangles;
        trimmed_surface->get_surface_sampled_triangles_without_uv_split(sampled_positions, sampled_triangles);

        std::vector<SketchPointPtr> sampled_points;
        // 转坐标空间
        for(int i = 0; i < sampled_positions.size(); ++i)
        {
            const nous::pos_t position = sampled_positions[i];
            const nous::vec4 position4 = nous::vec4(position.x(), position.y(), position.z(), 1.0);
            const nous::vec4 pos_trans1 = nurbs_doc_world_mat * position4;
            const nous::vec4 pos_trans2 = mesh_doc_inv_world_mat * pos_trans1;
            const nous::pos_t final_pos = pos_trans2.head<3>() / pos_trans2.w();
            sampled_points.emplace_back(mesh_doc->geodata()->add_point_without_check(final_pos));
        }

        std::unordered_set<SketchElementPtr> smooth_group_elements;
        bool has_valid_triangle = false;
        for(int i = 0; i < sampled_triangles.size(); ++i)
        {
            const nous::vec3i one_triangle = sampled_triangles[i];
            const SketchPointPtr p0 = sampled_points[one_triangle.x()];
            const SketchPointPtr p1 = sampled_points[one_triangle.y()];
            const SketchPointPtr p2 = sampled_points[one_triangle.z()];
            auto& skp = mesh_doc->geodata()->add_polygon_without_check({ { p0, p1, p2 } }, true);
            smooth_group_elements.insert(mesh_doc->geodata()->get_line(GeoData::line_key(p0, p1)));
            smooth_group_elements.insert(mesh_doc->geodata()->get_line(GeoData::line_key(p1, p2)));
            smooth_group_elements.insert(mesh_doc->geodata()->get_line(GeoData::line_key(p0, p2)));
            has_valid_triangle = true;
        }

        if(has_valid_triangle)
        {
            GeoDataTopoOperations::get().try_merge_smooth_group(mesh_doc->geodata().get(), smooth_group_elements, true);

            std::unordered_set<SketchElementPtr> weld_line_elements;
            for(auto& line: mesh_doc->geodata()->get_all_lines())
            {
                if(mesh_doc->geodata()->get_line_adj_rings(line.second).size() < 2)
                {
                    weld_line_elements.insert(line.second);
                }
            }

            GeoDataTopoOperations::get().try_merge_no_branch_line_group(mesh_doc->geodata().get(), weld_line_elements, {});
            mesh_doc->dirty();
        }
    }

    nurbs_doc->nurbsdata()->remove_item(nurbs_item->id());
    nurbs_doc->dirty();
}

nous::scalar get_u_on_curve(NurbsCurvePtr curve, const nous::pos_t& point)
{
    const size_t degree = curve->get_curve_degree();
    const auto&& control_points = curve->get_curve_control_points();

    std::vector<std::tuple<nous::size_t, nous::size_t>> bounding_ranges;
    for(int i = 0; i < control_points.size() - degree; ++i)
    {
        nous::AABB3 part_bounding_box;
        for(int j = 0; j <= degree; ++j)
        {
            part_bounding_box.extend(control_points[i + j]->m_point);
        }

        // point 由于 Nurbs 曲线的强凸包性，被这一段的控制点围成的 box 包含
        // 因此 point 可能位于这一段的 knots 之间
        if(part_bounding_box.contains(point))
        {
            bounding_ranges.emplace_back(i, i + degree + 1);
            i = i + degree + 1;
        }
    }

    // 找到一定数量的区间，依次处理
    double gl_min_distance = std::numeric_limits<double>::max();
    nous::scalar gl_u = -1;
    for(auto& range: bounding_ranges)
    {
        const size_t begin_ctp_idx = std::get<0>(range);
        const size_t end_ctp_idx = std::get<1>(range);
        const double min_u = curve->get_curve_knots()[begin_ctp_idx + degree];
        const double max_u = curve->get_curve_knots()[end_ctp_idx + degree];
        const size_t sample_count = 100;
        std::vector<nous::pos_t> sampled_positions = curve->get_sampled_point_arr(min_u, max_u, sample_count);
        double tmp_min_distance = std::numeric_limits<double>::max();
        size_t record_idx = -1;
        for(int i = 0; i < sampled_positions.size(); ++i)
        {
            const double pt_to_pt_dis = (sampled_positions[i] - point).norm();
            if(pt_to_pt_dis < tmp_min_distance)
            {
                tmp_min_distance = pt_to_pt_dis;
                record_idx = i;
            }
        }

        if(record_idx != -1)
        {
            const double u0 = min_u + (max_u - min_u) / sample_count * record_idx;
            // 牛顿迭代法
            double u1 = u0;
            while(true)
            {
                const nous::vec3 first_der = curve->get_derivs(u1, 1);
                const nous::vec3 second_der = curve->get_derivs(u1, 2);
                const nous::pos_t sample_pos = curve->get_position(u1);

                const nous::scalar judge1 = (sample_pos - point).norm();
                const nous::scalar judge2 = (first_der.dot(sample_pos - point)) / (first_der.norm() * (sample_pos - point).norm());

                if(judge1 <= 1e-2 && judge2 <= 1e-2)
                {
                    break;
                }

                const nous::scalar v1 = first_der.dot(sample_pos - point);
                const nous::scalar v2 = second_der.dot(sample_pos - point) + first_der.squaredNorm();
                nous::scalar u2 = (u1 - v1 / v2);

                if(curve->is_curve_closed())
                {
                    if(u2 < min_u)
                    {
                        u2 = max_u - (min_u - u2);
                    }

                    if(u2 > max_u)
                    {
                        u2 = min_u + (u2 - max_u);
                    }
                }
                else
                {
                    if(u2 < min_u)
                    {
                        u2 = min_u;
                    }

                    if(u2 > max_u)
                    {
                        u2 = max_u;
                    }
                }

                const nous::scalar judge3 = ((u2 - u1) * first_der).norm();
                if(judge3 <= 1e-2)
                {
                    break;
                }

                u1 = u2;
            }

            // 此时 u1 是牛顿迭代法在这一段的结果
            const nous::pos_t region_result_pos = curve->get_position(u1);
            const double result_dist = (region_result_pos - point).norm();
            if(result_dist < gl_min_distance)
            {
                gl_min_distance = result_dist;
                gl_u = u1;
            }
        }
    }

    return gl_u;
}

nous::scalar get_u_on_curve(NurbsCurvePtr curve, const nous::pos_t& ori, const nous::vec3& dir)
{
    const size_t degree = curve->get_curve_degree();
    const auto&& control_points = curve->get_curve_control_points();

    // 找到一定数量的区间，依次处理
    const double min_u = 0.0;
    const double max_u = 1.0;
    std::vector<nous::pos_t> sampled_positions = curve->get_sampled_point_arr();
    double tmp_min_distance = std::numeric_limits<double>::max();
    size_t record_idx = -1;
    for(int i = 0; i < sampled_positions.size(); ++i)
    {
        const double pt_to_pt_dis = sketch_math::point_to_line_distance(sampled_positions[i], ori, ori + dir, false);
        if(pt_to_pt_dis < tmp_min_distance)
        {
            tmp_min_distance = pt_to_pt_dis;
            record_idx = i;
        }
    }

    nous::scalar gl_u = -1;
    if(record_idx != -1)
    {
        const double u0 = min_u + (max_u - min_u) / sampled_positions.size() * record_idx;
        // 牛顿迭代法
        double u1 = u0;
        int count = 0;
        while(count < 0)
        {
            ++count;
            const nous::vec3 first_der = curve->get_derivs(u1, 1);
            const nous::vec3 second_der = curve->get_derivs(u1, 2);
            const nous::pos_t sample_pos = curve->get_position(u1);

            const nous::pos_t point = sketch_math::point_to_line_closest(sample_pos, ori, ori + dir, false);

            const nous::scalar judge1 = (sample_pos - point).norm();
            const nous::scalar judge2 = (first_der.dot(sample_pos - point)) / (first_der.norm() * (sample_pos - point).norm());

            if(judge1 <= 1e-2 && judge2 <= 1e-2)
            {
                break;
            }

            const nous::scalar v1 = first_der.dot(sample_pos - point);
            const nous::scalar v2 = second_der.dot(sample_pos - point) + first_der.squaredNorm();
            nous::scalar u2 = (u1 - v1 / v2);

            if(curve->is_curve_closed())
            {
                if(u2 < min_u)
                {
                    u2 = max_u - (min_u - u2);
                }

                if(u2 > max_u)
                {
                    u2 = min_u + (u2 - max_u);
                }
            }
            else
            {
                if(u2 < min_u)
                {
                    u2 = min_u;
                }

                if(u2 > max_u)
                {
                    u2 = max_u;
                }
            }

            const nous::scalar judge3 = ((u2 - u1) * first_der).norm();
            if(judge3 <= 1e-2)
            {
                break;
            }

            u1 = u2;
        }

        gl_u = u1;
    }

    return gl_u;
}

NurbsCurvePtr capture_curve(NurbsCurvePtr curve, const nous::scalar min_u, const nous::scalar max_u)
{
    // 在 min_u 右侧 max_u 左侧的曲线部分
    return curve->split_curve_at_u(min_u).second->split_curve_at_u(max_u).first;
}

NurbsCurvePtr reverse_curve(NurbsCurvePtr curve, bool new_ctp)
{
    const auto&& curve_knots = curve->get_curve_knots();
    std::vector<nous::scalar> reversed_knots;
    reversed_knots.emplace_back(0.0);
    for(int i = 0; i < curve_knots.size() - 1; ++i)
    {
        reversed_knots.emplace_back(reversed_knots[i] + curve_knots[curve_knots.size() - 1 - i] - curve_knots[curve_knots.size() - 2 - i]);
    }

    const auto& curve_u_range = curve->get_curve_range_with_limit();
    const nous::scalar curve_min_u = curve_u_range.first;
    const nous::scalar curve_max_u = curve_u_range.second;

    const nous::scalar reversed_max_u = 1.0 - curve_min_u;
    const nous::scalar reversed_min_u = 1.0 - curve_max_u;

    NurbsCurvePtr reversed_curve = std::make_shared<NurbsCurve>();
    std::vector<nous::scalar> curve_w_arr = curve->get_curve_weights();
    std::reverse(curve_w_arr.begin(), curve_w_arr.end());
    std::vector<SketchNurbsPointPtr> curve_ctp_arr = curve->get_curve_control_points();
    if(new_ctp)
    {
        std::vector<nous::pos_t> curve_ctp_pos_arr;
        for(int i = 0; i < curve_ctp_arr.size(); ++i)
        {
            curve_ctp_pos_arr.emplace_back(curve_ctp_arr[i]->m_point);
        }
        std::reverse(curve_ctp_pos_arr.begin(), curve_ctp_pos_arr.end());
        reversed_curve->update_curve_data(curve->get_curve_degree(), std::vector<nous::pos_t>(), curve_ctp_pos_arr, reversed_knots, curve_w_arr);
    }
    else
    {
        std::reverse(curve_ctp_arr.begin(), curve_ctp_arr.end());
        reversed_curve->update_curve_data(curve->get_curve_degree(), curve_ctp_arr, reversed_knots, curve_w_arr);
    }

    reversed_curve->set_curve_range(reversed_min_u, reversed_max_u);

    return reversed_curve;
}

void transfer_1d_surface_into_geodata(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, const nous::mat4& nurbs_trans)
{
    if(surface->get_surface_degree_u() != surface->get_surface_degree_v() || surface->get_surface_degree_u() != 1)
    {
        return;
    }

    convert_nurbs_surface_to_mesh(mesh_doc, surface, nurbs_trans);
}

void transfer_1d_surface_into_geodata_with_weld_info(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, View* view, const nous::mat4& nurbs_trans,
                                                     const std::vector<std::pair<size_t, size_t>> weld_info)
{
    if(surface->get_surface_degree_u() != surface->get_surface_degree_v() || surface->get_surface_degree_u() != 1)
    {
        return;
    }

    std::vector<NurbsAlgebraSurfacePtr> sub_algebra_surfaces = split_surface_on_u_with_weld_info(surface, weld_info);

    for(int i = 0; i < sub_algebra_surfaces.size(); ++i)
    {
        MeshDoc* submesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
        convert_nurbs_surface_to_mesh(submesh_doc, sub_algebra_surfaces[i], nurbs_trans);
        mesh_doc->merge(submesh_doc);
        view->delete_obj_and_try_delete_relation(submesh_doc);
    }
}

std::vector<NurbsAlgebraSurfacePtr> split_surface_on_u(NurbsAlgebraSurfacePtr surface)
{
    std::vector<NurbsAlgebraSurfacePtr> result;

    if(surface->get_surface_degree_u() != 1)
    {
        return result;
    }

    const auto&& surface_control_points = surface->get_surface_control_points();
    const auto&& surface_weights = surface->get_surface_weights();
    const auto&& surface_knots_v = surface->get_surface_knots_v();
    const unsigned int cols = surface_control_points.front().size();
    for(int i = 0; i < surface_control_points.size() - 1; ++i)
    {
        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> sub_surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        sub_surface_core->degree_u = 1;
        sub_surface_core->degree_v = surface->get_surface_degree_v();
        sub_surface_core->knots_u.emplace_back(0.0);
        sub_surface_core->knots_u.emplace_back(0.0);
        sub_surface_core->knots_u.emplace_back(1.0);
        sub_surface_core->knots_u.emplace_back(1.0);
        sub_surface_core->knots_v.insert(sub_surface_core->knots_v.end(), surface_knots_v.begin(), surface_knots_v.end());
        sub_surface_core->control_points.resize(2, cols);
        sub_surface_core->weights.resize(2, cols);

        for(int j = 0; j < cols; ++j)
        {
            sub_surface_core->control_points(0, j) = surface_control_points[i][j];
            sub_surface_core->control_points(1, j) = surface_control_points[i + 1][j];
            sub_surface_core->weights(0, j) = surface_weights[i][j];
            sub_surface_core->weights(1, j) = surface_weights[i + 1][j];
        }

        NurbsAlgebraSurfacePtr sub_surface = std::make_shared<NurbsAlgebraSurface>();
        sub_surface->update_surface_data(sub_surface_core);
        result.emplace_back(sub_surface);
    }

    return result;
}

std::vector<NurbsAlgebraSurfacePtr> split_surface_on_u_with_weld_info(NurbsAlgebraSurfacePtr surface,
                                                                      const std::vector<std::pair<size_t, size_t>> weld_info)
{
    std::vector<NurbsAlgebraSurfacePtr> result;
    if(surface->get_surface_degree_u() != 1)
    {
        return result;
    }

    const auto&& surface_control_points = surface->get_surface_control_points();
    const auto&& surface_weights = surface->get_surface_weights();
    const auto&& surface_knots_v = surface->get_surface_knots_v();
    const auto&& surface_knots_u = surface->get_surface_knots_u();
    const unsigned int cols = surface_control_points.front().size();

    for(int i = 0; i < weld_info.size(); ++i)
    {
        const size_t prev_idx = std::get<0>(weld_info[i]);
        const size_t next_idx = std::get<1>(weld_info[i]);
        const size_t rows = next_idx - prev_idx + 1;

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> sub_surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        sub_surface_core->degree_u = 1;
        sub_surface_core->degree_v = surface->get_surface_degree_v();
        sub_surface_core->knots_v.insert(sub_surface_core->knots_v.end(), surface_knots_v.begin(), surface_knots_v.end());
        sub_surface_core->control_points.resize(rows, cols);
        sub_surface_core->weights.resize(rows, cols);

        sub_surface_core->knots_u.emplace_back(0.0);
        for(int m = 0; m < rows; ++m)
        {
            for(int n = 0; n < cols; ++n)
            {
                sub_surface_core->control_points(m, n) = surface_control_points[prev_idx + m][n];
                sub_surface_core->weights(m, n) = surface_weights[prev_idx + m][n];
            }
            sub_surface_core->knots_u.emplace_back(surface_knots_u[prev_idx + m + 1]);
        }
        sub_surface_core->knots_u.emplace_back(1.0);

        const nous::scalar min_u = sub_surface_core->knots_u[1];
        const nous::scalar max_u = sub_surface_core->knots_u[sub_surface_core->knots_u.size() - 2];
        for(int j = 0; j < sub_surface_core->knots_u.size() - 2; ++j)
        {
            sub_surface_core->knots_u[j + 1] = (sub_surface_core->knots_u[j + 1] - min_u) / (max_u - min_u);
        }

        NurbsAlgebraSurfacePtr sub_surface = std::make_shared<NurbsAlgebraSurface>();
        sub_surface->update_surface_data(sub_surface_core);
        result.emplace_back(sub_surface);
    }

    return result;
}

std::map<EAlgebraSurfaceBoundaryType, NurbsCurvePtr> extract_surface_boundary_curve(NurbsAlgebraSurfacePtr surface, const bool ignore_closed_boundary)
{
    const auto&& surface_control_points = surface->get_surface_control_points();
    const auto&& surface_weights = surface->get_surface_weights();

    const auto&& surface_knots_u = surface->get_surface_knots_u();
    const auto&& surface_knots_v = surface->get_surface_knots_v();

    std::vector<SketchNurbsPointPtr> v0_curve_control_points;
    std::vector<nous::scalar> v0_curve_weights;
    v0_curve_control_points.insert(v0_curve_control_points.end(), surface_control_points.front().begin(), surface_control_points.front().end());
    v0_curve_weights.insert(v0_curve_weights.end(), surface_weights.front().begin(), surface_weights.front().end());

    std::vector<SketchNurbsPointPtr> v1_curve_control_points;
    std::vector<nous::scalar> v1_curve_weights;
    v1_curve_control_points.insert(v1_curve_control_points.end(), surface_control_points.back().begin(), surface_control_points.back().end());
    v1_curve_weights.insert(v1_curve_weights.end(), surface_weights.back().begin(), surface_weights.back().end());

    std::vector<SketchNurbsPointPtr> u0_curve_control_points;
    std::vector<SketchNurbsPointPtr> u1_curve_control_points;
    for(int i = 0; i < surface_control_points.size(); ++i)
    {
        u0_curve_control_points.emplace_back(surface_control_points[i].front());
        u1_curve_control_points.emplace_back(surface_control_points[i].back());
    }
    std::vector<nous::scalar> u0_curve_weights;
    std::vector<nous::scalar> u1_curve_weights;
    for(int i = 0; i < surface_weights.size(); ++i)
    {
        u0_curve_weights.emplace_back(surface_weights[i].front());
        u1_curve_weights.emplace_back(surface_weights[i].back());
    }

    NurbsCurvePtr u0_curve = std::make_shared<NurbsCurve>();
    NurbsCurvePtr u1_curve = std::make_shared<NurbsCurve>();
    NurbsCurvePtr v0_curve = std::make_shared<NurbsCurve>();
    NurbsCurvePtr v1_curve = std::make_shared<NurbsCurve>();

    u0_curve->update_curve_data(surface->get_surface_degree_u(), u0_curve_control_points, surface_knots_u, u0_curve_weights);
    u1_curve->update_curve_data(surface->get_surface_degree_u(), u1_curve_control_points, surface_knots_u, u1_curve_weights);
    v0_curve->update_curve_data(surface->get_surface_degree_v(), v0_curve_control_points, surface_knots_v, v0_curve_weights);
    v1_curve->update_curve_data(surface->get_surface_degree_v(), v1_curve_control_points, surface_knots_v, v1_curve_weights);

    std::map<EAlgebraSurfaceBoundaryType, NurbsCurvePtr> result;

    if(ignore_closed_boundary)
    {
        if(!surface->is_surface_u_closed())
        {
            auto& surface_v_range = surface->get_surface_v_range();
            v0_curve->set_curve_range(std::get<0>(surface_v_range), std::get<1>(surface_v_range));
            v1_curve->set_curve_range(std::get<0>(surface_v_range), std::get<1>(surface_v_range));
            result.insert({ EAlgebraSurfaceBoundaryType::V0, v0_curve });
            result.insert({ EAlgebraSurfaceBoundaryType::V1, v1_curve });
        }

        if(!surface->is_surface_v_closed())
        {
            auto& surface_u_range = surface->get_surface_u_range();
            u0_curve->set_curve_range(std::get<0>(surface_u_range), std::get<1>(surface_u_range));
            u1_curve->set_curve_range(std::get<0>(surface_u_range), std::get<1>(surface_u_range));
            result.insert({ EAlgebraSurfaceBoundaryType::U0, u0_curve });
            result.insert({ EAlgebraSurfaceBoundaryType::U1, u1_curve });
        }
    }
    else
    {
        result.insert({ EAlgebraSurfaceBoundaryType::V0, v0_curve });
        result.insert({ EAlgebraSurfaceBoundaryType::V1, v1_curve });
        result.insert({ EAlgebraSurfaceBoundaryType::U0, u0_curve });
        result.insert({ EAlgebraSurfaceBoundaryType::U1, u1_curve });
    }

    return result;
}

std::vector<NurbsAlgebraSurfacePtr> extrude_surface_by_normal(NurbsAlgebraSurfacePtr surface, const nous::scalar value)
{
    // 由于控制点是一个阵列，且不可能 u/v 方向上只有一个控制点，那么每个控制点的法向能不能就变成这个阵列上各个面的法向的平均，然后向这个法向移动？
    // 试试看先
    std::vector<std::vector<SketchNurbsPointPtr>> surface_control_point_mat = surface->get_surface_control_points();
    const size_t rows = surface_control_point_mat.size();
    const size_t cols = surface_control_point_mat.front().size();

    std::vector<std::vector<nous::vec3>> control_point_normal_mat(rows, std::vector<nous::pos_t>(cols));

    // 四个角只需要一个三角形确定法线
    control_point_normal_mat[0][0] = sketch_math::ring_normal(
        { surface_control_point_mat[0][0]->m_point, surface_control_point_mat[0][1]->m_point, surface_control_point_mat[1][0]->m_point });

    control_point_normal_mat[rows - 1][0] =
        sketch_math::ring_normal({ surface_control_point_mat[rows - 1][0]->m_point, surface_control_point_mat[rows - 2][0]->m_point,
                                   surface_control_point_mat[rows - 1][1]->m_point });

    control_point_normal_mat[0][cols - 1] =
        sketch_math::ring_normal({ surface_control_point_mat[0][cols - 1]->m_point, surface_control_point_mat[1][cols - 1]->m_point,
                                   surface_control_point_mat[0][cols - 2]->m_point });

    control_point_normal_mat[rows - 1][cols - 1] =
        sketch_math::ring_normal({ surface_control_point_mat[rows - 1][cols - 1]->m_point, surface_control_point_mat[rows - 1][cols - 2]->m_point,
                                   surface_control_point_mat[rows - 2][cols - 1]->m_point });

    // 边缘用两个三角形确定法线
    for(int i = 1; i < rows - 1; ++i)
    {
        const nous::vec3 normal1 = sketch_math::ring_normal(
            { surface_control_point_mat[i][0]->m_point, surface_control_point_mat[i - 1][0]->m_point, surface_control_point_mat[i][1]->m_point });
        const nous::vec3 normal2 = sketch_math::ring_normal(
            { surface_control_point_mat[i][0]->m_point, surface_control_point_mat[i][1]->m_point, surface_control_point_mat[i + 1][0]->m_point });
        control_point_normal_mat[i][0] = (normal1 + normal2).normalized();

        const nous::vec3 normal3 =
            sketch_math::ring_normal({ surface_control_point_mat[i][cols - 1]->m_point, surface_control_point_mat[i][cols - 2]->m_point,
                                       surface_control_point_mat[i - 1][cols - 1]->m_point });
        const nous::vec3 normal4 =
            sketch_math::ring_normal({ surface_control_point_mat[i][cols - 1]->m_point, surface_control_point_mat[i + 1][cols - 1]->m_point,
                                       surface_control_point_mat[i][cols - 2]->m_point });
        control_point_normal_mat[i][cols - 1] = (normal3 + normal4).normalized();
    }

    for(int i = 1; i < cols - 1; ++i)
    {
        const nous::vec3 normal1 = sketch_math::ring_normal(
            { surface_control_point_mat[0][i]->m_point, surface_control_point_mat[1][i]->m_point, surface_control_point_mat[0][i - 1]->m_point });
        const nous::vec3 normal2 = sketch_math::ring_normal(
            { surface_control_point_mat[0][i]->m_point, surface_control_point_mat[0][i + 1]->m_point, surface_control_point_mat[1][i]->m_point });
        control_point_normal_mat[0][i] = (normal1 + normal2).normalized();

        const nous::vec3 normal3 =
            sketch_math::ring_normal({ surface_control_point_mat[rows - 1][i]->m_point, surface_control_point_mat[rows - 1][i - 1]->m_point,
                                       surface_control_point_mat[rows - 2][i]->m_point });
        const nous::vec3 normal4 =
            sketch_math::ring_normal({ surface_control_point_mat[rows - 1][i]->m_point, surface_control_point_mat[rows - 2][i]->m_point,
                                       surface_control_point_mat[rows - 1][i + 1]->m_point });
        control_point_normal_mat[rows - 1][i] = (normal3 + normal4).normalized();
    }

    // 中间用四个三角形确定法线
    for(int i = 1; i < rows - 1; ++i)
    {
        for(int j = 1; j < cols - 1; ++j)
        {
            const nous::vec3 normal1 =
                sketch_math::ring_normal({ surface_control_point_mat[i][j]->m_point, surface_control_point_mat[i][j - 1]->m_point,
                                           surface_control_point_mat[i - 1][j]->m_point });
            const nous::vec3 normal2 =
                sketch_math::ring_normal({ surface_control_point_mat[i][j]->m_point, surface_control_point_mat[i - 1][j]->m_point,
                                           surface_control_point_mat[i][j + 1]->m_point });
            const nous::vec3 normal3 =
                sketch_math::ring_normal({ surface_control_point_mat[i][j]->m_point, surface_control_point_mat[i][j + 1]->m_point,
                                           surface_control_point_mat[i + 1][j]->m_point });
            const nous::vec3 normal4 =
                sketch_math::ring_normal({ surface_control_point_mat[i][j]->m_point, surface_control_point_mat[i + 1][j]->m_point,
                                           surface_control_point_mat[i][j - 1]->m_point });
            control_point_normal_mat[i][j] = (normal1 + normal2 + normal3 + normal4).normalized();
        }
    }

    auto&& surface_knots_u = surface->get_surface_knots_u();
    auto&& surface_knots_v = surface->get_surface_knots_v();
    auto&& surface_weights = surface->get_surface_weights();

    if(surface->get_surface_u_close_type() == ENurbsCloseType::SmoothClose)
    {
        // normal 首尾 normal 要改
        const unsigned int act_rows = rows - surface->get_surface_degree_u();
        for(int i = 0; i < cols; ++i)
        {
            control_point_normal_mat[0][i] = control_point_normal_mat[act_rows][i];
            control_point_normal_mat[rows - 1][i] = control_point_normal_mat[rows - act_rows - 1][i];
        }
    }

    if(surface->get_surface_v_close_type() == ENurbsCloseType::SmoothClose)
    {
        const unsigned int act_cols = cols - surface->get_surface_degree_v();
        for(int i = 0; i < rows; ++i)
        {
            control_point_normal_mat[i][0] = control_point_normal_mat[i][act_cols];
            control_point_normal_mat[i][cols - 1] = control_point_normal_mat[i][cols - act_cols - 1];
        }
    }

    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> new_offset_top_surface_core =
        std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    new_offset_top_surface_core->control_points.resize(rows, cols);
    new_offset_top_surface_core->weights.resize(rows, cols);
    new_offset_top_surface_core->knots_u.insert(new_offset_top_surface_core->knots_u.end(), surface_knots_u.begin(), surface_knots_u.end());
    new_offset_top_surface_core->knots_v.insert(new_offset_top_surface_core->knots_v.end(), surface_knots_v.begin(), surface_knots_v.end());
    new_offset_top_surface_core->degree_u = surface->get_surface_degree_u();
    new_offset_top_surface_core->degree_v = surface->get_surface_degree_v();

    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            const nous::pos_t offset_pos = surface_control_point_mat[i][j]->m_point + value * control_point_normal_mat[i][j];
            new_offset_top_surface_core->control_points(i, j) = std::make_shared<SketchNurbsPoint>(offset_pos);
            new_offset_top_surface_core->weights(i, j) = surface_weights[i][j];
        }
    }

    NurbsAlgebraSurfacePtr new_offset_top_surface = std::make_shared<NurbsAlgebraSurface>();
    new_offset_top_surface->update_surface_data(new_offset_top_surface_core);

    std::vector<NurbsAlgebraSurfacePtr> result;
    result.emplace_back(new_offset_top_surface);

    std::vector<std::vector<SketchNurbsPointPtr>> offset_control_point_mat = new_offset_top_surface->get_surface_control_points();

    // 构造侧面
    if(!surface->is_surface_u_closed())
    {
        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_v0_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_v0_core->control_points.resize(cols, 2);
        side_surface_for_v0_core->weights.resize(cols, 2);
        side_surface_for_v0_core->degree_u = surface->get_surface_degree_v();
        side_surface_for_v0_core->knots_u.insert(side_surface_for_v0_core->knots_u.end(), surface_knots_v.begin(), surface_knots_v.end());
        side_surface_for_v0_core->degree_v = 1;
        side_surface_for_v0_core->knots_v.emplace_back(0.0);
        side_surface_for_v0_core->knots_v.emplace_back(0.0);
        side_surface_for_v0_core->knots_v.emplace_back(1.0);
        side_surface_for_v0_core->knots_v.emplace_back(1.0);

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_v1_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_v1_core->control_points.resize(cols, 2);
        side_surface_for_v1_core->weights.resize(cols, 2);
        side_surface_for_v1_core->degree_u = surface->get_surface_degree_v();
        side_surface_for_v1_core->knots_u.insert(side_surface_for_v1_core->knots_u.end(), surface_knots_v.begin(), surface_knots_v.end());
        side_surface_for_v1_core->degree_v = 1;
        side_surface_for_v1_core->knots_v.emplace_back(0.0);
        side_surface_for_v1_core->knots_v.emplace_back(0.0);
        side_surface_for_v1_core->knots_v.emplace_back(1.0);
        side_surface_for_v1_core->knots_v.emplace_back(1.0);

        for(int i = 0; i < cols; ++i)
        {
            side_surface_for_v0_core->control_points(i, 0) = surface_control_point_mat[0][i];
            side_surface_for_v0_core->control_points(i, 1) = offset_control_point_mat[0][i];
            side_surface_for_v0_core->weights(i, 0) = surface_weights[0][i];
            side_surface_for_v0_core->weights(i, 1) = surface_weights[0][i];

            side_surface_for_v1_core->control_points(i, 0) = surface_control_point_mat[rows - 1][i];
            side_surface_for_v1_core->control_points(i, 1) = offset_control_point_mat[rows - 1][i];
            side_surface_for_v1_core->weights(i, 0) = surface_weights[rows - 1][i];
            side_surface_for_v1_core->weights(i, 1) = surface_weights[rows - 1][i];
        }

        NurbsAlgebraSurfacePtr side_surface_for_v0 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_v0->update_surface_data(side_surface_for_v0_core);
        NurbsAlgebraSurfacePtr side_surface_for_v1 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_v1->update_surface_data(side_surface_for_v1_core);

        result.emplace_back(side_surface_for_v0);
        result.emplace_back(side_surface_for_v1);
    }

    if(!surface->is_surface_v_closed())
    {
        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_u0_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_u0_core->control_points.resize(rows, 2);
        side_surface_for_u0_core->weights.resize(rows, 2);
        side_surface_for_u0_core->degree_u = surface->get_surface_degree_u();
        side_surface_for_u0_core->knots_u.insert(side_surface_for_u0_core->knots_u.end(), surface_knots_u.begin(), surface_knots_u.end());
        side_surface_for_u0_core->degree_v = 1;
        side_surface_for_u0_core->knots_v.emplace_back(0.0);
        side_surface_for_u0_core->knots_v.emplace_back(0.0);
        side_surface_for_u0_core->knots_v.emplace_back(1.0);
        side_surface_for_u0_core->knots_v.emplace_back(1.0);

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_u1_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_u1_core->control_points.resize(rows, 2);
        side_surface_for_u1_core->weights.resize(rows, 2);
        side_surface_for_u1_core->degree_u = surface->get_surface_degree_u();
        side_surface_for_u1_core->knots_u.insert(side_surface_for_u1_core->knots_u.end(), surface_knots_u.begin(), surface_knots_u.end());
        side_surface_for_u1_core->degree_v = 1;
        side_surface_for_u1_core->knots_v.emplace_back(0.0);
        side_surface_for_u1_core->knots_v.emplace_back(0.0);
        side_surface_for_u1_core->knots_v.emplace_back(1.0);
        side_surface_for_u1_core->knots_v.emplace_back(1.0);

        for(int i = 0; i < rows; ++i)
        {
            side_surface_for_u0_core->control_points(i, 0) = surface_control_point_mat[i][0];
            side_surface_for_u0_core->control_points(i, 1) = offset_control_point_mat[i][0];
            side_surface_for_u0_core->weights(i, 0) = surface_weights[i][0];
            side_surface_for_u0_core->weights(i, 1) = surface_weights[i][0];

            side_surface_for_u1_core->control_points(i, 0) = surface_control_point_mat[i][cols - 1];
            side_surface_for_u1_core->control_points(i, 1) = offset_control_point_mat[i][cols - 1];
            side_surface_for_u1_core->weights(i, 0) = surface_weights[i][cols - 1];
            side_surface_for_u1_core->weights(i, 1) = surface_weights[i][cols - 1];
        }

        NurbsAlgebraSurfacePtr side_surface_for_u0 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_u0->update_surface_data(side_surface_for_u0_core);
        NurbsAlgebraSurfacePtr side_surface_for_u1 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_u1->update_surface_data(side_surface_for_u1_core);

        result.emplace_back(side_surface_for_u0);
        result.emplace_back(side_surface_for_u1);
    }

    return result;
}

std::vector<NurbsCurvePtr> extract_u_curves_on_surface(NurbsAlgebraSurfacePtr surface, bool shared_point)
{
    auto&& surface_control_points = surface->get_surface_control_points();
    auto&& surface_weights = surface->get_surface_weights();
    const unsigned int rows = surface_control_points.size();
    const unsigned int cols = surface_control_points.front().size();

    const size_t degree_u = surface->get_surface_degree_u();
    auto&& knots_u = surface->get_surface_knots_u();
    std::vector<NurbsCurvePtr> curves;
    for(int i = 0; i < cols; ++i)
    {
        NurbsCurvePtr sub_curve = std::make_shared<NurbsCurve>();
        std::vector<nous::scalar> curve_weights(rows, nous::scalar(0.0));

        if(shared_point)
        {
            std::vector<SketchNurbsPointPtr> curve_control_points(rows, nullptr);
            for(int j = 0; j < rows; ++j)
            {
                curve_control_points[i] = surface_control_points[i][j];
                curve_weights[i] = surface_weights[i][j];
            }
            sub_curve->update_curve_data(degree_u, curve_control_points, knots_u, curve_weights);
        }
        else
        {
            std::vector<nous::pos_t> curve_control_points(rows, nous::pos_t::Zero());
            for(int j = 0; j < rows; ++j)
            {
                curve_control_points[i] = surface_control_points[i][j]->m_point;
                curve_weights[i] = surface_weights[i][j];
            }
            sub_curve->update_curve_data(degree_u, std::vector<nous::pos_t>(), curve_control_points, knots_u, curve_weights);
        }

        curves.emplace_back(sub_curve);
    }
    return curves;
}

std::vector<NurbsCurvePtr> extract_v_curves_on_surface(NurbsAlgebraSurfacePtr surface, bool shared_point)
{
    auto&& surface_control_points = surface->get_surface_control_points();
    auto&& surface_weights = surface->get_surface_weights();
    const unsigned int rows = surface_control_points.size();
    const unsigned int cols = surface_control_points.front().size();

    const size_t degree_v = surface->get_surface_degree_v();
    auto&& knots_v = surface->get_surface_knots_v();
    std::vector<NurbsCurvePtr> curves;
    for(int i = 0; i < rows; ++i)
    {
        NurbsCurvePtr sub_curve = std::make_shared<NurbsCurve>();
        if(shared_point)
        {
            sub_curve->update_curve_data(degree_v, surface_control_points[i], knots_v, surface_weights[i]);
        }
        else
        {
            std::vector<nous::pos_t> curve_control_points(rows, nous::pos_t::Zero());
            for(int j = 0; j < cols; ++j)
            {
                curve_control_points[i] = surface_control_points[i][j]->m_point;
            }
            sub_curve->update_curve_data(degree_v, std::vector<nous::pos_t>(), curve_control_points, knots_v, surface_weights[i]);
        }
        curves.emplace_back(sub_curve);
    }
    return curves;
}

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal(NurbsClosedSurfacePtr curve_brep, const nous::scalar value,
                                                                   const nous::vec3 positive_dir)
{
    // Tips: 为了保证连续性，我们要优先保证 NurbsCurveBRepObjectPtr 里的所有曲线对象是首尾相接且闭合的，且每条曲线的首尾点是共享的
    const size_t inner_curve_number = curve_brep->get_curve_number();
    std::vector<NurbsCurvePtr> captured_boundary_curves;
    for(int i = 0; i < inner_curve_number; ++i)
    {
        NurbsClosedSurfaceCurve curve_range = curve_brep->get_brep_curve_range(i);
        captured_boundary_curves.emplace_back(curve_range.m_inner_curve);
    }

    std::vector<std::shared_ptr<INurbsBase>> result;

    if(inner_curve_number == 1)
    {
        NurbsClosedSurfacePtr top_surface = std::make_shared<NurbsClosedSurface>();
        result.emplace_back(top_surface);
        // 只有一条闭合曲线围成的边界表示曲面
        NurbsCurvePtr only_curve = captured_boundary_curves[0];
        const auto&& curve_control_points = only_curve->get_curve_control_points();
        std::vector<nous::vec3> curve_ctps_normal(curve_control_points.size(), nous::vec3::Zero());

        nous::vec3 last_tangent = nous::vec3::Zero();
        for(int i = 1; i < curve_control_points.size() - 1; ++i)
        {
            const nous::pos_t& prev_pos = curve_control_points[i - 1]->m_point;
            const nous::pos_t& curr_pos = curve_control_points[i]->m_point;
            const nous::pos_t& next_pos = curve_control_points[i + 1]->m_point;

            const nous::vec3 tangent = ((next_pos - curr_pos).normalized() + (curr_pos - prev_pos).normalized()).normalized();

            if(i == 1)
            {
                curve_ctps_normal[i] = -((prev_pos - curr_pos).normalized().cross((next_pos - curr_pos).normalized()).normalized());
                if(curve_ctps_normal[i].dot(positive_dir) < 0)
                {
                    curve_ctps_normal[i] = -curve_ctps_normal[i];
                }
            }
            else
            {
                const nous::vec3 prev_normal = curve_ctps_normal[i - 1];
                Eigen::Quaternion<nous::scalar> rotate_quat = Eigen::Quaternion<nous::scalar>::FromTwoVectors(last_tangent, tangent);
                curve_ctps_normal[i] = rotate_quat.matrix() * prev_normal;
            }

            last_tangent = tangent;
        }

        if(only_curve->get_curve_close_type() == ENurbsCloseType::SmoothClose)
        {
            const unsigned int act_ctp_number = curve_control_points.size() - only_curve->get_curve_degree();
            curve_ctps_normal[0] = curve_ctps_normal[act_ctp_number];
            curve_ctps_normal[curve_control_points.size() - 1] = curve_ctps_normal[curve_control_points.size() - act_ctp_number - 1];
        }
        else if(only_curve->get_curve_close_type() == ENurbsCloseType::SharpClose)
        {
            const nous::pos_t& prev_pos = curve_control_points[curve_control_points.size() - 2]->m_point;
            const nous::pos_t& curr_pos = curve_control_points[0]->m_point;
            const nous::pos_t& next_pos = curve_control_points[1]->m_point;

            const nous::vec3 tangent = ((next_pos - curr_pos).normalized() + (curr_pos - prev_pos).normalized()).normalized();
            Eigen::Quaternion<nous::scalar> rotate_quat = Eigen::Quaternion<nous::scalar>::FromTwoVectors(last_tangent, tangent);
            curve_ctps_normal[0] = curve_ctps_normal[curve_control_points.size() - 1] =
                rotate_quat.matrix() * curve_ctps_normal[curve_control_points.size() - 2];
        }
        else
        {
            curve_ctps_normal[0] = curve_ctps_normal[1];
            curve_ctps_normal[curve_control_points.size() - 1] = curve_ctps_normal[curve_control_points.size() - 2];
        }

        std::vector<nous::pos_t> new_control_points(curve_control_points.size());
        auto&& curve_knots = only_curve->get_curve_knots();
        auto&& curve_weights = only_curve->get_curve_weights();

        std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
        NurbsCurvePtr top_curve = only_curve->clone(remaped_map)->to_nurbs_curve();
        for(int i = 0; i < curve_control_points.size(); ++i)
        {
            top_curve->update_curve_control_point(i, curve_control_points[i]->m_point + curve_ctps_normal[i] * value);
        }
        top_surface->add_sub_curve_without_check(top_curve);

        const auto&& offseted_control_points = top_curve->get_curve_control_points();

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_core->degree_u = only_curve->get_curve_degree();
        side_surface_core->degree_v = 1;
        side_surface_core->knots_u.insert(side_surface_core->knots_u.end(), curve_knots.begin(), curve_knots.end());
        side_surface_core->knots_v.emplace_back(0.0);
        side_surface_core->knots_v.emplace_back(0.0);
        side_surface_core->knots_v.emplace_back(1.0);
        side_surface_core->knots_v.emplace_back(1.0);
        side_surface_core->control_points.resize(new_control_points.size(), 2, nullptr);
        side_surface_core->weights.resize(new_control_points.size(), 2, 1.0);

        for(int i = 0; i < new_control_points.size(); ++i)
        {
            side_surface_core->control_points(i, 0) = curve_control_points[i];
            side_surface_core->control_points(i, 1) = offseted_control_points[i];
            side_surface_core->weights(i, 0) = curve_weights[i];
            side_surface_core->weights(i, 1) = curve_weights[i];
        }

        NurbsAlgebraSurfacePtr side_surface = std::make_shared<NurbsAlgebraSurface>();
        side_surface->update_surface_data(side_surface_core);
        result.emplace_back(side_surface);
    }
    else
    {
        // 换个思路，把边界线都提出来就完事儿了，这个 else 分支一定是
        std::vector<SketchNurbsPointPtr> curve_brep_ctp_arr;
        for(int i = 0; i < captured_boundary_curves.size(); ++i)
        {
            NurbsCurvePtr boundary_curve = captured_boundary_curves[i];
            auto&& boundary_curve_ctp_arr = boundary_curve->get_curve_control_points();
            curve_brep_ctp_arr.insert(curve_brep_ctp_arr.end(), boundary_curve_ctp_arr.begin(), boundary_curve_ctp_arr.end() - 1);
        }

        std::vector<nous::vec3> curve_brep_ctp_normal_arr(curve_brep_ctp_arr.size());
        nous::vec3 last_tangent = nous::vec3::Zero();
        for(int i = 0; i < curve_brep_ctp_arr.size(); ++i)
        {
            SketchNurbsPointPtr prev_ctp_ptr = curve_brep_ctp_arr[(i - 1 + curve_brep_ctp_arr.size()) % curve_brep_ctp_arr.size()];
            SketchNurbsPointPtr curr_ctp_ptr = curve_brep_ctp_arr[i];
            SketchNurbsPointPtr next_ctp_ptr = curve_brep_ctp_arr[(i + 1) % curve_brep_ctp_arr.size()];

            const nous::pos_t prev_ctp_pos = prev_ctp_ptr->m_point;
            const nous::pos_t curr_ctp_pos = curr_ctp_ptr->m_point;
            const nous::pos_t next_ctp_pos = next_ctp_ptr->m_point;

            const nous::vec3 curr_tangent = ((curr_ctp_pos - prev_ctp_pos).normalized() + (next_ctp_pos - curr_ctp_pos).normalized()).normalized();
            if(i == 0)
            {
                const nous::vec3 dir1 = (prev_ctp_pos - curr_ctp_pos).normalized();
                const nous::vec3 dir2 = (next_ctp_pos - curr_ctp_pos).normalized();
                curve_brep_ctp_normal_arr[i] = dir1.cross(dir2).normalized();
                if(curve_brep_ctp_normal_arr[i].dot(positive_dir) < 0)
                {
                    curve_brep_ctp_normal_arr[i] = -curve_brep_ctp_normal_arr[i];
                }
            }
            else
            {
                const nous::vec3 prev_normal = curve_brep_ctp_normal_arr[i - 1];
                Eigen::Quaternion<nous::scalar> rot_quat = Eigen::Quaternion<nous::scalar>::FromTwoVectors(last_tangent, curr_tangent);
                curve_brep_ctp_normal_arr[i] = rot_quat.matrix() * prev_normal;
            }

            last_tangent = curr_tangent;
        }

        std::vector<nous::pos_t> top_curve_brep_pos_arr(curve_brep_ctp_normal_arr.size());
        for(int i = 0; i < top_curve_brep_pos_arr.size(); ++i)
        {
            top_curve_brep_pos_arr[i] = curve_brep_ctp_arr[i]->m_point + curve_brep_ctp_normal_arr[i] * value;
        }

        std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
        NurbsClosedSurfacePtr top_curve_brep = curve_brep->clone(remaped_map)->to_nurbs_closed_surface();
        result.emplace_back(top_curve_brep);
        unsigned int acc_pt_count = 0;
        for(int i = 0; i < top_curve_brep->get_curve_number(); ++i)
        {
            NurbsCurvePtr top_sub_curve = top_curve_brep->get_brep_curve_range(i).m_inner_curve;
            NurbsCurvePtr sub_curve = curve_brep->get_brep_curve_range(i).m_inner_curve;

            auto&& sub_curve_ctp_arr = sub_curve->get_curve_control_points();
            auto&& top_sub_curve_ctp_arr = top_sub_curve->get_curve_control_points();
            auto&& sub_curve_w_arr = sub_curve->get_curve_weights();
            auto&& top_sub_curve_w_arr = top_sub_curve->get_curve_weights();

            const unsigned int top_sub_curve_ctp_number = top_sub_curve_ctp_arr.size();
            for(int j = 0; j < top_sub_curve_ctp_number - 1; ++j)
            {
                top_sub_curve->update_curve_control_point(j, top_curve_brep_pos_arr[acc_pt_count + j]);
            }
            acc_pt_count += (top_sub_curve_ctp_number - 1);

            auto&& sub_curve_knots = top_sub_curve->get_curve_knots();

            NurbsAlgebraSurfacePtr sub_surface = std::make_shared<NurbsAlgebraSurface>();
            std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> sub_surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
            sub_surface_core->degree_u = top_sub_curve->get_curve_degree();
            sub_surface_core->degree_v = 1;
            sub_surface_core->knots_u.insert(sub_surface_core->knots_u.end(), sub_curve_knots.begin(), sub_curve_knots.end());
            sub_surface_core->knots_v.emplace_back(0.0);
            sub_surface_core->knots_v.emplace_back(0.0);
            sub_surface_core->knots_v.emplace_back(1.0);
            sub_surface_core->knots_v.emplace_back(1.0);
            sub_surface_core->weights.resize(top_sub_curve_ctp_number, 2, 1.0);
            sub_surface_core->control_points.resize(top_sub_curve_ctp_number, 2, nullptr);

            for(int j = 0; j < top_sub_curve_ctp_number; ++j)
            {
                sub_surface_core->control_points(j, 0) = sub_curve_ctp_arr[j];
                sub_surface_core->control_points(j, 1) = top_sub_curve_ctp_arr[j];
                sub_surface_core->weights(j, 0) = sub_curve_w_arr[j];
                sub_surface_core->weights(j, 1) = top_sub_curve_w_arr[j];
            }

            sub_surface->update_surface_data(sub_surface_core);
            result.emplace_back(sub_surface);
        }
    }

    return result;
}

std::vector<std::vector<nous::pos_t>> generate_surface_sample_points_with_1d_limits(NurbsAlgebraSurfacePtr surface, const unsigned int ref_rows,
                                                                                    const unsigned int ref_cols)
{
    std::vector<std::vector<nous::pos_t>> result;

    std::vector<nous::scalar> sample_u_arr;
    std::vector<nous::scalar> sample_v_arr;

    auto& surface_u_range = surface->get_surface_u_range_with_limit();
    auto& surface_v_range = surface->get_surface_v_range_with_limit();

    const nous::scalar min_u = surface_u_range.first;
    const nous::scalar max_u = surface_u_range.second;
    const nous::scalar min_v = surface_v_range.first;
    const nous::scalar max_v = surface_v_range.second;

    if(surface->get_surface_degree_u() == 1)
    {
        sample_u_arr.emplace_back(min_u);
        auto&& knots_u = surface->get_surface_knots_u();
        for(int i = 1; i < knots_u.size() - 1; ++i)
        {
            if(knots_u[i] > sample_u_arr.back() && knots_u[i] < max_u)
            {
                sample_u_arr.emplace_back(knots_u[i]);
            }
        }
        sample_u_arr.emplace_back(max_u);
    }
    else
    {
        for(int i = 0; i < ref_rows; ++i)
        {
            sample_u_arr.emplace_back(min_u + ((1.0 / (ref_rows - 1)) * i * (max_u - min_u)));
        }
    }

    if(surface->get_surface_degree_v() == 1)
    {
        sample_v_arr.emplace_back(min_v);
        auto&& knots_v = surface->get_surface_knots_v();
        for(int i = 1; i < knots_v.size() - 1; ++i)
        {
            if(knots_v[i] > sample_v_arr.back() && knots_v[i] < max_v)
            {
                sample_v_arr.emplace_back(knots_v[i]);
            }
        }
        sample_v_arr.emplace_back(max_v);
    }
    else
    {
        for(int i = 0; i < ref_cols; ++i)
        {
            sample_v_arr.emplace_back(min_v + ((1.0 / (ref_cols - 1)) * i * (max_v - min_v)));
        }
    }

    result.resize(sample_u_arr.size(), std::vector<nous::pos_t>(sample_v_arr.size()));
    for(int i = 0; i < sample_u_arr.size(); ++i)
    {
        for(int j = 0; j < sample_v_arr.size(); ++j)
        {
            result[i][j] = surface->get_position(sample_u_arr[i], sample_v_arr[j]);
        }
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal_keep_distance(NurbsAlgebraSurfacePtr surface, const nous::scalar value)
{
    const unsigned int surface_u_degree = surface->get_surface_degree_u();
    const unsigned int surface_v_degree = surface->get_surface_degree_v();

    auto&& sampled_surface_points = generate_surface_sample_points_with_1d_limits(surface, 30, 30);

    auto&& offseted_surface_points = topo::generate_offseted_position_by_normal(sampled_surface_points, value);

    // 顶面要用 offseted_surface_points 重新拟合一个出来
    // 侧面直接用边缘怼，ugly

    const unsigned int rows = offseted_surface_points.size();
    const unsigned int cols = offseted_surface_points.front().size();
    auto surface_core = global_surface_interpolation(rows - 1, cols - 1, sampled_surface_points, surface_u_degree, surface_v_degree);
    auto offseted_surface_core = global_surface_interpolation(rows - 1, cols - 1, offseted_surface_points, surface_u_degree, surface_v_degree);

    const auto& basic_ctp_matrix = surface_core->control_points;
    const auto& offset_ctp_matrix = offseted_surface_core->control_points;

    std::vector<std::shared_ptr<INurbsBase>> result;

    NurbsAlgebraSurfacePtr top_surface = std::make_shared<NurbsAlgebraSurface>();
    top_surface->update_surface_data(offseted_surface_core);
    result.emplace_back(top_surface);

    const auto&& surface_knots_u = top_surface->get_surface_knots_u();
    const auto&& surface_knots_v = top_surface->get_surface_knots_v();

    if(!surface->is_surface_u_closed() && !surface->is_surface_v_closed())
    {
        auto&& surface_boundary_set = nurbs::extract_surface_boundary_curve(surface);
        auto&& top_surface_boundary_set = nurbs::extract_surface_boundary_curve(top_surface);

        {
            NurbsClosedSurfacePtr u0_side_core = std::make_shared<NurbsClosedSurface>();
            NurbsCurvePtr u0_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::U0];
            NurbsCurvePtr top_u0_curve = reverse_curve(top_surface_boundary_set[EAlgebraSurfaceBoundaryType::U0], false);
            u0_side_core->add_sub_curve_without_check(u0_curve);
            NurbsCurvePtr u0_side_v_cuvre0 = std::make_shared<NurbsCurve>();
            u0_side_v_cuvre0->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ u0_curve->get_curve_control_points().back(), top_u0_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            u0_side_core->add_sub_curve_without_check(u0_side_v_cuvre0);
            u0_side_core->add_sub_curve_without_check(top_u0_curve);
            NurbsCurvePtr u0_side_v_curve1 = std::make_shared<NurbsCurve>();
            u0_side_v_curve1->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ top_u0_curve->get_curve_control_points().back(), u0_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            u0_side_core->add_sub_curve_without_check(u0_side_v_curve1);
            result.emplace_back(u0_side_core);
        }

        {
            NurbsClosedSurfacePtr v1_side_core = std::make_shared<NurbsClosedSurface>();
            NurbsCurvePtr v1_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::V1];
            NurbsCurvePtr top_v1_curve = reverse_curve(top_surface_boundary_set[EAlgebraSurfaceBoundaryType::V1], false);
            v1_side_core->add_sub_curve_without_check(v1_curve);
            NurbsCurvePtr v1_side_v_cuvre0 = std::make_shared<NurbsCurve>();
            v1_side_v_cuvre0->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ v1_curve->get_curve_control_points().back(), top_v1_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            v1_side_core->add_sub_curve_without_check(v1_side_v_cuvre0);
            v1_side_core->add_sub_curve_without_check(top_v1_curve);
            NurbsCurvePtr v1_side_v_curve1 = std::make_shared<NurbsCurve>();
            v1_side_v_curve1->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ top_v1_curve->get_curve_control_points().back(), v1_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            v1_side_core->add_sub_curve_without_check(v1_side_v_curve1);
            result.emplace_back(v1_side_core);
        }

        {
            NurbsClosedSurfacePtr u1_side_core = std::make_shared<NurbsClosedSurface>();
            NurbsCurvePtr u1_curve = reverse_curve(surface_boundary_set[EAlgebraSurfaceBoundaryType::U1], false);
            NurbsCurvePtr top_u1_curve = top_surface_boundary_set[EAlgebraSurfaceBoundaryType::U1];
            u1_side_core->add_sub_curve_without_check(u1_curve);
            NurbsCurvePtr u1_side_v_curve0 = std::make_shared<NurbsCurve>();
            u1_side_v_curve0->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ u1_curve->get_curve_control_points().back(), top_u1_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            u1_side_core->add_sub_curve_without_check(u1_side_v_curve0);
            u1_side_core->add_sub_curve_without_check(top_u1_curve);
            NurbsCurvePtr u1_side_v_curve1 = std::make_shared<NurbsCurve>();
            u1_side_v_curve1->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ top_u1_curve->get_curve_control_points().back(), u1_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            u1_side_core->add_sub_curve_without_check(u1_side_v_curve1);
            result.emplace_back(u1_side_core);
        }

        {
            NurbsClosedSurfacePtr v0_side_core = std::make_shared<NurbsClosedSurface>();
            NurbsCurvePtr v0_curve = reverse_curve(surface_boundary_set[EAlgebraSurfaceBoundaryType::V0], false);
            NurbsCurvePtr top_v0_curve = top_surface_boundary_set[EAlgebraSurfaceBoundaryType::V0];
            v0_side_core->add_sub_curve_without_check(v0_curve);
            NurbsCurvePtr v0_side_v_curve0 = std::make_shared<NurbsCurve>();
            v0_side_v_curve0->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ v0_curve->get_curve_control_points().back(), top_v0_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            v0_side_core->add_sub_curve_without_check(v0_side_v_curve0);
            v0_side_core->add_sub_curve_without_check(top_v0_curve);
            NurbsCurvePtr v0_side_v_curve1 = std::make_shared<NurbsCurve>();
            v0_side_v_curve1->update_curve_data(
                1, std::vector<SketchNurbsPointPtr>{ top_v0_curve->get_curve_control_points().back(), v0_curve->get_curve_control_points().front() },
                std::vector<nous::scalar>{ 0.0, 0.0, 1.0, 1.0 }, std::vector<nous::scalar>{ 1.0, 1.0 });
            v0_side_core->add_sub_curve_without_check(v0_side_v_curve1);
            result.emplace_back(v0_side_core);
        }
    }
    else
    {
        auto&& surface_boundary_set = nurbs::extract_surface_boundary_curve(surface);
        auto&& offseted_surface_boundary_set = nurbs::extract_surface_boundary_curve(top_surface);

        if(!surface->is_surface_u_closed())
        {
            NurbsCurvePtr surface_v0_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::V0];
            NurbsCurvePtr surface_v1_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::V1];
            NurbsCurvePtr offseted_surface_v0_curve = offseted_surface_boundary_set[EAlgebraSurfaceBoundaryType::V0];
            NurbsCurvePtr offseted_surface_v1_curve = offseted_surface_boundary_set[EAlgebraSurfaceBoundaryType::V1];

            // 看看如何排布这个玩意
            // 挺麻烦还
            NurbsTrimmedSurfacePtr top_trimmed_surface = std::make_shared<NurbsTrimmedSurface>();
            NurbsTrimmedSurfacePtr bottom_trimmed_surface = std::make_shared<NurbsTrimmedSurface>();
            if(value > 0)
            {
                NurbsClosedSurfacePtr top_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_shell_closed_surface->add_sub_curve_without_check(offseted_surface_v0_curve);
                NurbsClosedSurfacePtr top_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_hole_closed_surface->add_sub_curve_without_check(surface_v0_curve);
                top_trimmed_surface->set_exterior_ring(top_shell_closed_surface);
                top_trimmed_surface->add_interior_ring(top_hole_closed_surface);
                result.emplace_back(top_trimmed_surface);

                NurbsClosedSurfacePtr bottom_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_shell_closed_surface->add_sub_curve_without_check(offseted_surface_v1_curve);
                NurbsClosedSurfacePtr bottom_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_hole_closed_surface->add_sub_curve_without_check(surface_v1_curve);
                bottom_trimmed_surface->set_exterior_ring(bottom_shell_closed_surface);
                bottom_trimmed_surface->add_interior_ring(bottom_hole_closed_surface);
                result.emplace_back(bottom_trimmed_surface);
            }
            else
            {
                NurbsClosedSurfacePtr top_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_shell_closed_surface->add_sub_curve_without_check(surface_v0_curve);
                NurbsClosedSurfacePtr top_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_hole_closed_surface->add_sub_curve_without_check(offseted_surface_v0_curve);
                top_trimmed_surface->set_exterior_ring(top_shell_closed_surface);
                top_trimmed_surface->add_interior_ring(top_hole_closed_surface);
                result.emplace_back(top_trimmed_surface);

                NurbsClosedSurfacePtr bottom_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_shell_closed_surface->add_sub_curve_without_check(surface_v1_curve);
                NurbsClosedSurfacePtr bottom_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_hole_closed_surface->add_sub_curve_without_check(offseted_surface_v1_curve);
                bottom_trimmed_surface->set_exterior_ring(bottom_shell_closed_surface);
                bottom_trimmed_surface->add_interior_ring(bottom_hole_closed_surface);
                result.emplace_back(bottom_trimmed_surface);
            }
        }

        if(!surface->is_surface_v_closed())
        {
            NurbsCurvePtr surface_u0_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::U0];
            NurbsCurvePtr surface_u1_curve = surface_boundary_set[EAlgebraSurfaceBoundaryType::U1];
            NurbsCurvePtr offseted_surface_u0_curve = offseted_surface_boundary_set[EAlgebraSurfaceBoundaryType::U0];
            NurbsCurvePtr offseted_surface_u1_curve = offseted_surface_boundary_set[EAlgebraSurfaceBoundaryType::U1];

            NurbsTrimmedSurfacePtr top_complex_surface = std::make_shared<NurbsTrimmedSurface>();
            NurbsTrimmedSurfacePtr bottom_complex_surface = std::make_shared<NurbsTrimmedSurface>();
            if(value < 0)
            {
                NurbsClosedSurfacePtr top_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_shell_closed_surface->add_sub_curve_without_check(offseted_surface_u0_curve);
                NurbsClosedSurfacePtr top_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_hole_closed_surface->add_sub_curve_without_check(surface_u0_curve);
                top_complex_surface->set_exterior_ring(top_shell_closed_surface);
                top_complex_surface->add_interior_ring(top_hole_closed_surface);
                result.emplace_back(top_complex_surface);

                NurbsClosedSurfacePtr bottom_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_shell_closed_surface->add_sub_curve_without_check(offseted_surface_u1_curve);
                NurbsClosedSurfacePtr bottom_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_hole_closed_surface->add_sub_curve_without_check(surface_u1_curve);
                bottom_complex_surface->set_exterior_ring(bottom_shell_closed_surface);
                bottom_complex_surface->add_interior_ring(bottom_hole_closed_surface);
                result.emplace_back(bottom_complex_surface);
            }
            else
            {
                NurbsClosedSurfacePtr top_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_shell_closed_surface->add_sub_curve_without_check(surface_u0_curve);
                NurbsClosedSurfacePtr top_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                top_hole_closed_surface->add_sub_curve_without_check(offseted_surface_u0_curve);
                top_complex_surface->set_exterior_ring(top_shell_closed_surface);
                top_complex_surface->add_interior_ring(top_hole_closed_surface);
                result.emplace_back(top_complex_surface);

                NurbsClosedSurfacePtr bottom_shell_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_shell_closed_surface->add_sub_curve_without_check(surface_u1_curve);
                NurbsClosedSurfacePtr bottom_hole_closed_surface = std::make_shared<NurbsClosedSurface>();
                bottom_hole_closed_surface->add_sub_curve_without_check(offseted_surface_u1_curve);
                bottom_complex_surface->set_exterior_ring(bottom_shell_closed_surface);
                bottom_complex_surface->add_interior_ring(bottom_hole_closed_surface);
                result.emplace_back(bottom_complex_surface);
            }
        }
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal_keep_distance(NurbsTrimmedSurfacePtr surface, const nous::scalar value)
{
    std::vector<std::shared_ptr<INurbsBase>> result;
    auto& exring_surface = surface->get_exterior_ring();

    NurbsTrimmedSurfacePtr top_trimmed_surface = std::make_shared<NurbsTrimmedSurface>();
    if(exring_surface->type() == EElementType::AlgebraSurface)
    {
        auto&& exring_side_shell_surfaces = extrude_surface_by_normal_keep_distance(exring_surface->to_nurbs_algebra_surface(), value);
        result.insert(result.end(), exring_side_shell_surfaces.begin() + 1, exring_side_shell_surfaces.end());
        top_trimmed_surface->set_exterior_ring(exring_side_shell_surfaces.front()->to_nurbs_surface_base());
    }
    else if(exring_surface->type() == EElementType::ClosedSurface)
    {
        const nous::vec3 positive_dir = sketch_math::ring_normal(exring_surface->to_nurbs_closed_surface()->get_sampled_boundary());
        auto&& exring_side_shell_surfaces = extrude_surface_by_normal(exring_surface->to_nurbs_closed_surface(), value, positive_dir);
        result.insert(result.end(), exring_side_shell_surfaces.begin() + 1, exring_side_shell_surfaces.end());
        top_trimmed_surface->set_exterior_ring(exring_side_shell_surfaces.front()->to_nurbs_surface_base());
    }

    for(int i = 0; i < surface->get_interior_rings().size(); ++i)
    {
        auto& inring_surface = surface->get_interior_ring_at(i);
        if(inring_surface->type() == EElementType::AlgebraSurface)
        {
            auto&& inring_side_shell_surfaces = extrude_surface_by_normal_keep_distance(inring_surface->to_nurbs_algebra_surface(), value);
            result.insert(result.end(), inring_side_shell_surfaces.begin() + 1, inring_side_shell_surfaces.end());
            top_trimmed_surface->add_interior_ring(inring_side_shell_surfaces.front()->to_nurbs_surface_base());
        }
        else if(exring_surface->type() == EElementType::ClosedSurface)
        {
            const nous::vec3 positive_dir = sketch_math::ring_normal(inring_surface->to_nurbs_closed_surface()->get_sampled_boundary());
            auto&& inring_side_shell_surfaces = extrude_surface_by_normal(inring_surface->to_nurbs_closed_surface(), value, positive_dir);
            result.insert(result.end(), inring_side_shell_surfaces.begin() + 1, inring_side_shell_surfaces.end());
            top_trimmed_surface->add_interior_ring(inring_side_shell_surfaces.front()->to_nurbs_surface_base());
        }
    }

    result.emplace_back(top_trimmed_surface);
    top_trimmed_surface->dirty();
    return result;
}

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_with_transform(NurbsAlgebraSurfacePtr surface, const nous::transform3 transform)
{
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
    NurbsAlgebraSurfacePtr offseted_surface = surface->clone(remaped_map, transform.matrix())->to_nurbs_algebra_surface();
    auto&& base_surface_ctp_mat = surface->get_surface_control_points();
    auto&& top_surface_ctp_mat = offseted_surface->get_surface_control_points();

    auto&& surface_knots_u = surface->get_surface_knots_u();
    auto&& surface_knots_v = surface->get_surface_knots_v();
    auto&& base_surface_w_mat = surface->get_surface_weights();
    auto&& top_surface_w_mat = offseted_surface->get_surface_weights();
    unsigned int rows = base_surface_ctp_mat.size();
    unsigned int cols = base_surface_ctp_mat.front().size();

    std::vector<NurbsAlgebraSurfacePtr> result;
    result.emplace_back(offseted_surface);

    if(!surface->is_surface_u_closed())
    {
        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_v0_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_v0_core->control_points.resize(cols, 2);
        side_surface_for_v0_core->weights.resize(cols, 2);
        side_surface_for_v0_core->degree_u = surface->get_surface_degree_v();
        side_surface_for_v0_core->knots_u.insert(side_surface_for_v0_core->knots_u.end(), surface_knots_v.begin(), surface_knots_v.end());
        side_surface_for_v0_core->degree_v = 1;
        side_surface_for_v0_core->knots_v.emplace_back(0.0);
        side_surface_for_v0_core->knots_v.emplace_back(0.0);
        side_surface_for_v0_core->knots_v.emplace_back(1.0);
        side_surface_for_v0_core->knots_v.emplace_back(1.0);

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_v1_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_v1_core->control_points.resize(cols, 2);
        side_surface_for_v1_core->weights.resize(cols, 2);
        side_surface_for_v1_core->degree_u = surface->get_surface_degree_v();
        side_surface_for_v1_core->knots_u.insert(side_surface_for_v1_core->knots_u.end(), surface_knots_v.begin(), surface_knots_v.end());
        side_surface_for_v1_core->degree_v = 1;
        side_surface_for_v1_core->knots_v.emplace_back(0.0);
        side_surface_for_v1_core->knots_v.emplace_back(0.0);
        side_surface_for_v1_core->knots_v.emplace_back(1.0);
        side_surface_for_v1_core->knots_v.emplace_back(1.0);

        for(int i = 0; i < cols; ++i)
        {
            side_surface_for_v0_core->control_points(i, 0) = base_surface_ctp_mat[0][i];
            side_surface_for_v0_core->control_points(i, 1) = top_surface_ctp_mat[0][i];
            side_surface_for_v0_core->weights(i, 0) = 1.0;
            side_surface_for_v0_core->weights(i, 1) = 1.0;

            side_surface_for_v1_core->control_points(i, 0) = base_surface_ctp_mat[rows - 1][i];
            side_surface_for_v1_core->control_points(i, 1) = top_surface_ctp_mat[rows - 1][i];
            side_surface_for_v1_core->weights(i, 0) = 1.0;
            side_surface_for_v1_core->weights(i, 1) = 1.0;
        }

        NurbsAlgebraSurfacePtr side_surface_for_v0 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_v0->update_surface_data(side_surface_for_v0_core);
        NurbsAlgebraSurfacePtr side_surface_for_v1 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_v1->update_surface_data(side_surface_for_v1_core);

        result.emplace_back(side_surface_for_v0);
        result.emplace_back(side_surface_for_v1);
    }

    if(!surface->is_surface_v_closed())
    {
        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_u0_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_u0_core->control_points.resize(rows, 2);
        side_surface_for_u0_core->weights.resize(rows, 2);
        side_surface_for_u0_core->degree_u = surface->get_surface_degree_u();
        side_surface_for_u0_core->knots_u.insert(side_surface_for_u0_core->knots_u.end(), surface_knots_u.begin(), surface_knots_u.end());
        side_surface_for_u0_core->degree_v = 1;
        side_surface_for_u0_core->knots_v.emplace_back(0.0);
        side_surface_for_u0_core->knots_v.emplace_back(0.0);
        side_surface_for_u0_core->knots_v.emplace_back(1.0);
        side_surface_for_u0_core->knots_v.emplace_back(1.0);

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_for_u1_core =
            std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_for_u1_core->control_points.resize(rows, 2);
        side_surface_for_u1_core->weights.resize(rows, 2);
        side_surface_for_u1_core->degree_u = surface->get_surface_degree_u();
        side_surface_for_u1_core->knots_u.insert(side_surface_for_u1_core->knots_u.end(), surface_knots_u.begin(), surface_knots_u.end());
        side_surface_for_u1_core->degree_v = 1;
        side_surface_for_u1_core->knots_v.emplace_back(0.0);
        side_surface_for_u1_core->knots_v.emplace_back(0.0);
        side_surface_for_u1_core->knots_v.emplace_back(1.0);
        side_surface_for_u1_core->knots_v.emplace_back(1.0);

        for(int i = 0; i < rows; ++i)
        {
            side_surface_for_u0_core->control_points(i, 0) = base_surface_ctp_mat[i][0];
            side_surface_for_u0_core->control_points(i, 1) = top_surface_ctp_mat[i][0];
            side_surface_for_u0_core->weights(i, 0) = 1.0;
            side_surface_for_u0_core->weights(i, 1) = 1.0;

            side_surface_for_u1_core->control_points(i, 0) = base_surface_ctp_mat[i][cols - 1];
            side_surface_for_u1_core->control_points(i, 1) = top_surface_ctp_mat[i][cols - 1];
            side_surface_for_u1_core->weights(i, 0) = 1.0;
            side_surface_for_u1_core->weights(i, 1) = 1.0;
        }

        NurbsAlgebraSurfacePtr side_surface_for_u0 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_u0->update_surface_data(side_surface_for_u0_core);
        NurbsAlgebraSurfacePtr side_surface_for_u1 = std::make_shared<NurbsAlgebraSurface>();
        side_surface_for_u1->update_surface_data(side_surface_for_u1_core);

        result.emplace_back(side_surface_for_u0);
        result.emplace_back(side_surface_for_u1);
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_with_transform(NurbsClosedSurfacePtr surface, const nous::transform3 transform)
{
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
    NurbsClosedSurfacePtr offseted_surface = surface->clone(remaped_map, transform.matrix())->to_nurbs_closed_surface();
    std::vector<std::shared_ptr<INurbsBase>> result;
    result.emplace_back(offseted_surface);

    for(int i = 0; i < surface->get_curve_number(); ++i)
    {
        NurbsCurvePtr base_inner_curve = surface->get_brep_curve_range(i).m_inner_curve;
        NurbsCurvePtr top_inner_curve = offseted_surface->get_brep_curve_range(i).m_inner_curve;
        auto&& curve_knots = base_inner_curve->get_curve_knots();
        auto&& base_curve_ctp_arr = base_inner_curve->get_curve_control_points();
        auto&& top_curve_ctp_arr = top_inner_curve->get_curve_control_points();
        auto&& base_curve_w_arr = base_inner_curve->get_curve_weights();
        auto&& top_curve_w_arr = top_inner_curve->get_curve_weights();

        std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> side_surface_core = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
        side_surface_core->control_points.resize(base_curve_ctp_arr.size(), 2);
        side_surface_core->weights.resize(base_curve_ctp_arr.size(), 2);
        side_surface_core->degree_u = base_inner_curve->get_curve_degree();
        side_surface_core->knots_u.insert(side_surface_core->knots_u.end(), curve_knots.begin(), curve_knots.end());
        side_surface_core->degree_v = 1;
        side_surface_core->knots_v.emplace_back(0.0);
        side_surface_core->knots_v.emplace_back(0.0);
        side_surface_core->knots_v.emplace_back(1.0);
        side_surface_core->knots_v.emplace_back(1.0);

        for(int i = 0; i < base_curve_ctp_arr.size(); ++i)
        {
            side_surface_core->control_points(i, 0) = base_curve_ctp_arr[i];
            side_surface_core->control_points(i, 1) = top_curve_ctp_arr[i];

            side_surface_core->weights(i, 0) = base_curve_w_arr[i];
            side_surface_core->weights(i, 1) = top_curve_w_arr[i];
        }

        NurbsAlgebraSurfacePtr side_surface = std::make_shared<NurbsAlgebraSurface>();
        side_surface->update_surface_data(side_surface_core);
        result.emplace_back(side_surface);
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_with_transform(NurbsTrimmedSurfacePtr surface, const nous::transform3 transform)
{
    std::vector<std::shared_ptr<INurbsBase>> result;
    auto& exring_surface = surface->get_exterior_ring();

    NurbsTrimmedSurfacePtr top_complex_surface = std::make_shared<NurbsTrimmedSurface>();
    if(exring_surface->type() == EElementType::AlgebraSurface)
    {
        auto&& exring_side_shell_surfaces = offset_surface_to_solid_with_transform(exring_surface->to_nurbs_algebra_surface(), transform);
        result.insert(result.end(), exring_side_shell_surfaces.begin() + 1, exring_side_shell_surfaces.end());
        top_complex_surface->set_exterior_ring(exring_side_shell_surfaces.front());
    }
    else if(exring_surface->type() == EElementType::ClosedSurface)
    {
        auto&& exring_side_shell_surfaces = offset_surface_to_solid_with_transform(exring_surface->to_nurbs_closed_surface(), transform);
        result.insert(result.end(), exring_side_shell_surfaces.begin() + 1, exring_side_shell_surfaces.end());
        top_complex_surface->set_exterior_ring(exring_side_shell_surfaces.front()->to_nurbs_surface_base());
    }

    for(int i = 0; i < surface->get_interior_rings().size(); ++i)
    {
        auto& inring_surface = surface->get_interior_ring_at(i);
        if(inring_surface->type() == EElementType::AlgebraSurface)
        {
            auto&& inring_side_shell_surfaces = offset_surface_to_solid_with_transform(inring_surface->to_nurbs_algebra_surface(), transform);
            result.insert(result.end(), inring_side_shell_surfaces.begin() + 1, inring_side_shell_surfaces.end());
            top_complex_surface->add_interior_ring(inring_side_shell_surfaces.front());
        }
        else if(exring_surface->type() == EElementType::ClosedSurface)
        {
            auto&& inring_side_shell_surfaces = offset_surface_to_solid_with_transform(inring_surface->to_nurbs_closed_surface(), transform);
            result.insert(result.end(), inring_side_shell_surfaces.begin() + 1, inring_side_shell_surfaces.end());
            top_complex_surface->add_interior_ring(inring_side_shell_surfaces.front()->to_nurbs_surface_base());
        }
    }

    result.emplace_back(top_complex_surface);
    return result;
}

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_by_path(NurbsCurvePtr curve, const std::vector<NurbsCurvePtr>& path)
{
    std::vector<NurbsAlgebraSurfacePtr> result;
    const auto&& curve_ctp_arr = curve->get_curve_control_points();
    const unsigned int curve_ctp_number = curve_ctp_arr.size();
    const nous::pos_t path_base_pos = path.front()->get_position(0.0);
    NurbsAlgebraSurfacePtr prev_surface = nullptr;
    for(int i = 0; i < path.size(); ++i)
    {
        // 这个方法会把 curve 写在 surface 的列上，所以要把第一列更换为上一个 surface 的最后一列，然后更新剩余 ctp 的位置
        NurbsAlgebraSurfacePtr sub_surface = extrude_curve_to_surface(curve, path[i]);
        const nous::pos_t sub_path_start_pos = path[i]->get_position(0.0);
        const nous::vec3 offset_delta = sub_path_start_pos - path_base_pos;

        const auto&& path_ctp_arr = curve->get_curve_control_points();
        const unsigned int path_ctp_number = path_ctp_arr.size();

        auto&& sub_surface_ctp_mat = sub_surface->get_surface_control_points();
        if(i > 0)
        {
            auto&& prev_surface_ctp_mat = prev_surface->get_surface_control_points();
            for(int j = 0; j < curve_ctp_number; ++j)
            {
                sub_surface->replace_surface_control_point(j, 0, prev_surface_ctp_mat[j][prev_surface_ctp_mat[j].size() - 1]);
            }
            for(int m = 0; m < curve_ctp_number; ++m)
            {
                for(int n = 1; n < path_ctp_number; ++n)
                {
                    const nous::pos_t ori_pos = sub_surface_ctp_mat[m][n]->get_position();
                    sub_surface_ctp_mat[m][n]->set_position(ori_pos + offset_delta);
                }
            }
        }

        prev_surface = sub_surface;
        result.emplace_back(sub_surface);
    }

    return result;
}

void link_two_surface_groups_gen_by_path(const std::vector<NurbsAlgebraSurfacePtr>& last_surfaces,
                                         const std::vector<NurbsAlgebraSurfacePtr> column_surfaces)
{
    for(int j = 0; j < column_surfaces.size(); ++j)
    {
        NurbsAlgebraSurfacePtr curr_surface = column_surfaces[j];
        NurbsAlgebraSurfacePtr prev_surface = last_surfaces[j];

        const unsigned int rows = prev_surface->get_rows();
        const unsigned int cols = curr_surface->get_cols();
        auto&& prev_surface_ctp_mat = prev_surface->get_surface_control_points();
        for(int m = 0; m < cols; ++m)
        {
            curr_surface->replace_surface_control_point(0, m, prev_surface_ctp_mat[rows - 1][m]);
        }
    }
}

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_by_path(NurbsAlgebraSurfacePtr surface, const std::vector<NurbsCurvePtr>& path,
                                                                    bool with_top)
{
    // 外面要注意一下，把 path 转到 curve 的 doc 空间里去
    std::vector<NurbsAlgebraSurfacePtr> result;

    const nous::pos_t path_base_pos = path.front()->get_position(0.0);
    const nous::pos_t path_end_pos = path.back()->get_position(1.0);
    const nous::vec3 offset = path_end_pos - path_base_pos;
    nous::transform3 offset_transform = nous::transform3::Identity();
    offset_transform.translate(offset);
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
    const NurbsAlgebraSurfacePtr top_surface = surface->clone(remaped_map, offset_transform.matrix())->to_nurbs_algebra_surface();
    const unsigned int top_surface_rows = top_surface->get_rows();
    const unsigned int top_surface_cols = top_surface->get_cols();

    std::vector<NurbsAlgebraSurfacePtr> last_surfaces;
    auto&& surface_boundary_lines = nurbs::extract_surface_boundary_curve(surface, false);
    for(int i = 0; i < 4; ++i)
    {
        EAlgebraSurfaceBoundaryType bdtp = static_cast<EAlgebraSurfaceBoundaryType>(i);
        NurbsCurvePtr curve = surface_boundary_lines[bdtp];
        if(i >= 2)
        {
            curve = reverse_curve(curve);
        }

        auto&& column_surfaces = offset_surface_to_solid_by_path(curve, path);
        if(i > 0)
        {
            link_two_surface_groups_gen_by_path(last_surfaces, column_surfaces);
            if(i == 3)
            {
                for(int j = 0; j < column_surfaces.size(); ++j)
                {
                    NurbsAlgebraSurfacePtr last_surface = column_surfaces[j];
                    NurbsAlgebraSurfacePtr first_surface = result[j]->to_nurbs_algebra_surface();

                    const unsigned int rows = last_surface->get_rows();
                    const unsigned int cols = last_surface->get_cols();

                    auto&& first_surface_ctp_mat = first_surface->get_surface_control_points();
                    for(int m = 0; m < cols; ++m)
                    {
                        last_surface->replace_surface_control_point(rows - 1, m, first_surface_ctp_mat[0][m]);
                    }
                }
            }
        }

        auto&& near_top_surface_ctp_mat = column_surfaces.back()->get_surface_control_points();
        const unsigned int nts_rows = column_surfaces.back()->get_rows();
        const unsigned int nts_cols = column_surfaces.back()->get_cols();
        if(bdtp == EAlgebraSurfaceBoundaryType::U0)
        {
            for(int i = 0; i < top_surface_rows; ++i)
            {
                top_surface->replace_surface_control_point(i, 0, near_top_surface_ctp_mat[i][nts_cols - 1]);
            }
        }

        if(bdtp == EAlgebraSurfaceBoundaryType::V1)
        {
            for(int i = 0; i < top_surface_cols; ++i)
            {
                top_surface->replace_surface_control_point(top_surface_rows - 1, i, near_top_surface_ctp_mat[i][nts_cols - 1]);
            }
        }

        if(bdtp == EAlgebraSurfaceBoundaryType::U1)
        {
            for(int i = 0; i < top_surface_rows; ++i)
            {
                top_surface->replace_surface_control_point(top_surface_rows - 1 - i, top_surface_cols - 1, near_top_surface_ctp_mat[i][nts_cols - 1]);
            }
        }

        if(bdtp == EAlgebraSurfaceBoundaryType::V0)
        {
            for(int i = 0; i < top_surface_cols; ++i)
            {
                top_surface->replace_surface_control_point(0, top_surface_cols - 1 - i, near_top_surface_ctp_mat[i][nts_cols - 1]);
            }
        }

        for(NurbsAlgebraSurfacePtr surface: column_surfaces)
        {
            result.emplace_back(surface);
        }
        last_surfaces = column_surfaces;
    }

    if(with_top)
    {
        result.emplace_back(top_surface);
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_by_path(NurbsClosedSurfacePtr curve_brep, const std::vector<NurbsCurvePtr>& path,
                                                                         bool with_top)
{
    std::vector<std::shared_ptr<INurbsBase>> result;

    const unsigned int inner_curve_count = curve_brep->get_curve_number();
    const nous::pos_t path_base_pos = path.front()->get_position(0.0);
    const nous::pos_t path_end_pos = path.back()->get_position(1.0);
    const nous::vec3 offset = path_end_pos - path_base_pos;
    nous::transform3 offset_transform = nous::transform3::Identity();
    offset_transform.translate(offset);
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
    const NurbsClosedSurfacePtr top_curve_brep = curve_brep->clone(remaped_map, offset_transform.matrix())->to_nurbs_closed_surface();

    std::vector<NurbsAlgebraSurfacePtr> last_surfaces;
    for(int i = 0; i < inner_curve_count; ++i)
    {
        NurbsCurvePtr inner_curve = curve_brep->get_brep_curve_range(i).m_inner_curve;
        const auto&& curve_ctp_arr = inner_curve->get_curve_control_points();
        const unsigned int curve_ctp_number = curve_ctp_arr.size();
        auto&& column_surfaces = nurbs::offset_surface_to_solid_by_path(inner_curve, path);

        if(i > 0)
        {
            link_two_surface_groups_gen_by_path(last_surfaces, column_surfaces);

            if(i == inner_curve_count - 1)
            {
                for(int j = 0; j < column_surfaces.size(); ++j)
                {
                    NurbsAlgebraSurfacePtr last_surface = column_surfaces[j];
                    NurbsAlgebraSurfacePtr first_surface = result[j]->to_nurbs_algebra_surface();

                    const unsigned int rows = last_surface->get_rows();
                    const unsigned int cols = last_surface->get_cols();

                    auto&& first_surface_ctp_mat = first_surface->get_surface_control_points();
                    for(int m = 0; m < cols; ++m)
                    {
                        last_surface->replace_surface_control_point(rows - 1, m, first_surface_ctp_mat[0][m]);
                    }
                }
            }
        }

        NurbsAlgebraSurfacePtr near_top_surface = column_surfaces.back();
        auto&& near_top_surface_ctp_mat = near_top_surface->get_surface_control_points();
        unsigned int near_top_surface_cols = near_top_surface->get_cols();
        for(int j = 0; j < near_top_surface->get_rows(); ++j)
        {
            top_curve_brep->get_brep_curve_range(i).m_inner_curve->replace_curve_control_point(
                j, near_top_surface_ctp_mat[j][near_top_surface_cols - 1]);
        }

        for(NurbsAlgebraSurfacePtr surface: column_surfaces)
        {
            result.emplace_back(surface);
        }
        last_surfaces = column_surfaces;
    }

    if(with_top)
    {
        result.emplace_back(top_curve_brep);
    }

    return result;
}

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_by_path(NurbsTrimmedSurfacePtr trimmed_surface,
                                                                         const std::vector<NurbsCurvePtr>& path)
{
    std::vector<std::shared_ptr<INurbsBase>> result;
    auto& exring_surface = trimmed_surface->get_exterior_ring();

    NurbsTrimmedSurfacePtr top_complex_surface = std::make_shared<NurbsTrimmedSurface>();
    if(exring_surface->type() == EElementType::AlgebraSurface)
    {
        auto&& exring_side_shell_surfaces = offset_surface_to_solid_by_path(exring_surface->to_nurbs_algebra_surface(), path);
        result.insert(result.end(), exring_side_shell_surfaces.begin(), exring_side_shell_surfaces.end() - 1);
        top_complex_surface->set_exterior_ring(exring_side_shell_surfaces.back());
    }
    else if(exring_surface->type() == EElementType::ClosedSurface)
    {
        auto&& exring_side_shell_surfaces = offset_surface_to_solid_by_path(exring_surface->to_nurbs_closed_surface(), path);
        result.insert(result.end(), exring_side_shell_surfaces.begin(), exring_side_shell_surfaces.end() - 1);
        top_complex_surface->set_exterior_ring(exring_side_shell_surfaces.back()->to_nurbs_surface_base());
    }

    for(int i = 0; i < trimmed_surface->get_interior_rings().size(); ++i)
    {
        auto& inring_surface = trimmed_surface->get_interior_ring_at(i);
        if(inring_surface->type() == EElementType::AlgebraSurface)
        {
            auto&& inring_side_shell_surfaces = offset_surface_to_solid_by_path(inring_surface->to_nurbs_algebra_surface(), path);
            result.insert(result.end(), inring_side_shell_surfaces.begin(), inring_side_shell_surfaces.end() - 1);
            top_complex_surface->add_interior_ring(inring_side_shell_surfaces.back());
        }
        else if(exring_surface->type() == EElementType::ClosedSurface)
        {
            auto&& inring_side_shell_surfaces = offset_surface_to_solid_by_path(inring_surface->to_nurbs_closed_surface(), path);
            result.insert(result.end(), inring_side_shell_surfaces.begin(), inring_side_shell_surfaces.end() - 1);
            top_complex_surface->add_interior_ring(inring_side_shell_surfaces.back()->to_nurbs_surface_base());
        }
    }

    result.emplace_back(top_complex_surface);

    return result;
}

bool is_same_curve_object(NurbsCurvePtr first, NurbsCurvePtr second)
{
    // 顺序和逆序判别
    auto&& first_curve_ctp_arr = first->get_curve_control_points();
    auto&& second_curve_ctp_arr = second->get_curve_control_points();
    auto&& first_curve_knots_arr = first->get_curve_knots();
    auto&& second_curve_knots_arr = second->get_curve_knots();
    auto&& first_curve_weights_arr = first->get_curve_weights();
    auto&& second_curve_weights_arr = second->get_curve_weights();

    bool result = first_curve_ctp_arr.size() == second_curve_ctp_arr.size() && first_curve_knots_arr.size() == second_curve_knots_arr.size() &&
                  first_curve_weights_arr.size() == second_curve_weights_arr.size() && first->get_curve_degree() == second->get_curve_degree();

    if(result)
    {
        bool is_forward_same = true;
        for(int i = 0; i < first_curve_ctp_arr.size(); ++i)
        {
            if(!sketch_math::point_equal(first_curve_ctp_arr[i]->m_point, second_curve_ctp_arr[i]->m_point) ||
               !((first_curve_weights_arr[i] - second_curve_weights_arr[i]) < 1e-5))
            {
                is_forward_same = false;
                break;
            }
        }

        if(is_forward_same)
        {
            for(int i = 0; i < first_curve_knots_arr.size(); ++i)
            {
                if(!((first_curve_knots_arr[i] - second_curve_knots_arr[i]) < 1e-5))
                {
                    is_forward_same = false;
                    break;
                }
            }
        }

        bool is_backward_same = true;
        for(int i = 0; i < first_curve_ctp_arr.size(); ++i)
        {
            if(!sketch_math::point_equal(first_curve_ctp_arr[i]->m_point, second_curve_ctp_arr[second_curve_ctp_arr.size() - 1 - i]->m_point) ||
               !((first_curve_weights_arr[i] - second_curve_weights_arr[second_curve_weights_arr.size() - 1 - i]) < 1e-5))
            {
                is_backward_same = false;
                break;
            }
        }

        if(is_backward_same)
        {
            for(int i = 0; i < first_curve_knots_arr.size() - 1; ++i)
            {
                double first_delta = first_curve_knots_arr[i + 1] - first_curve_knots_arr[i];
                double second_delta =
                    second_curve_knots_arr[second_curve_knots_arr.size() - 1 - i] - second_curve_knots_arr[second_curve_knots_arr.size() - 2 - i];

                if(!(first_delta - second_delta) < 1e-5)
                {
                    is_backward_same = false;
                    break;
                }
            }
        }

        result = is_forward_same || is_backward_same;
    }

    return result;
}

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsClosedSurfaceEdgeElementPtr edge_curve)
{
    auto&& result = find_same_edge_elements(edge_curve->m_edge_curve.lock());
    return result;
}

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsCurvePtr curve)
{
    std::unordered_set<SketchElementPtr> result;
    auto&& edge_ctp_arr = curve->get_curve_control_points();
    auto& first_ctp = edge_ctp_arr.front();
    auto&& valid_parents = first_ctp->get_valid_parents();
    for(auto& ctp_parent: valid_parents)
    {
        if(ctp_parent->type() == EElementType::Curve)
        {
            if(ctp_parent->to_nurbs_curve()->get_parent())
            {
                NurbsClosedSurfacePtr closed_surface = ctp_parent->to_nurbs_curve()->get_parent()->to_nurbs_closed_surface();
                auto&& closed_surface_elements = closed_surface->get_surface_elements();
                for(auto& surface_element: closed_surface_elements)
                {
                    if(is_same_curve_object(curve, surface_element.first->to_nurbs_closed_surface_element()->m_edge_curve.lock()))
                    {
                        result.insert(surface_element.first);
                    }
                }
            }
            else
            {
                if(is_same_curve_object(curve, ctp_parent->to_nurbs_curve()))
                {
                    result.insert(ctp_parent);
                }
            }
        }

        if(ctp_parent->type() == EElementType::AlgebraSurface)
        {
            auto&& algebra_surface_elements = ctp_parent->to_nurbs_algebra_surface()->get_surface_elements();
            for(auto& surface_element: algebra_surface_elements)
            {
                if(is_same_curve_object(curve, surface_element.first->to_nurbs_algebra_surface_element()->m_edge_curve))
                {
                    result.insert(surface_element.first);
                }
            }
        }
    }
    return result;
}

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsAlgebraSurfaceEdgeElementPtr edge_curve)
{
    auto&& result = find_same_edge_elements(edge_curve->m_edge_curve);
    return result;
}

std::unordered_set<SketchElementPtr> find_same_edge_elements(SketchElementPtr edge_curve)
{
    std::unordered_set<SketchElementPtr> result;

    if(edge_curve->type() == EElementType::AlgebraSurfaceEdge)
    {
        auto&& tmp = find_same_edge_elements(edge_curve->to_nurbs_algebra_surface_element());
        result.insert(tmp.begin(), tmp.end());
    }
    else if(edge_curve->type() == EElementType::ClosedSurfaceEdge)
    {
        auto&& tmp = find_same_edge_elements(edge_curve->to_nurbs_closed_surface_element());
        result.insert(tmp.begin(), tmp.end());
    }
    else if(edge_curve->type() == EElementType::Curve)
    {
        auto&& tmp = find_same_edge_elements(edge_curve->to_nurbs_curve());
        result.insert(tmp.begin(), tmp.end());
    }

    // 要把被 AlgebraSurfaceEdge 和 ClosedSurfaceEdge 包含的 Curve 对象移除，否则会引发重复
    std::unordered_set<SketchElementPtr> need_removed_curves;
    for(auto& ele: result)
    {
        if(ele->type() == EElementType::AlgebraSurfaceEdge)
        {
            need_removed_curves.insert(ele->to_nurbs_algebra_surface_element()->m_edge_curve);
        }
        else if(ele->type() == EElementType::ClosedSurfaceEdge)
        {
            need_removed_curves.insert(ele->to_nurbs_closed_surface_element()->m_edge_curve.lock());
        }
    }

    for(auto& ele: need_removed_curves)
    {
        result.erase(ele);
    }
    result.erase(edge_curve);

    return result;
}

std::vector<nous::pos_t> convert_surface_boundary_to_polyline(NurbsClosedSurfacePtr closed_surface)
{
    std::vector<nous::pos_t> result;

    for(int i = 0; i < closed_surface->get_curve_number(); ++i)
    {
        NurbsClosedSurfaceCurve curve_range = closed_surface->get_brep_curve_range(i);
        NurbsCurvePtr curve = curve_range.m_inner_curve;
        auto&& polyline = convert_curve_to_polyline(curve);
        result.insert(result.end(), polyline.begin(), polyline.end());
    }

    return result;
}

std::vector<nous::pos_t> convert_surface_boundary_to_polyline(NurbsAlgebraSurfacePtr algebra_surface)
{
    std::vector<nous::pos_t> result;
    auto&& boundary_curves = extract_surface_boundary_curve(algebra_surface, false);

    NurbsCurvePtr curve_u0 = boundary_curves[EAlgebraSurfaceBoundaryType::U0];
    NurbsCurvePtr curve_v1 = boundary_curves[EAlgebraSurfaceBoundaryType::V1];
    NurbsCurvePtr curve_u1 = reverse_curve(boundary_curves[EAlgebraSurfaceBoundaryType::U1]);
    NurbsCurvePtr curve_v0 = reverse_curve(boundary_curves[EAlgebraSurfaceBoundaryType::V0]);

    auto&& curve_u0_position_arr = convert_curve_to_polyline(curve_u0);
    auto&& curve_v1_position_arr = convert_curve_to_polyline(curve_v1);
    auto&& curve_u1_position_arr = convert_curve_to_polyline(curve_u1);
    auto&& curve_v0_position_arr = convert_curve_to_polyline(curve_v0);

    if(!curve_u0_position_arr.empty())
    {
        result.insert(result.end(), curve_u0_position_arr.begin(), curve_u0_position_arr.end() - 1);
    }

    if(!curve_v1_position_arr.empty())
    {
        result.insert(result.end(), curve_v1_position_arr.begin(), curve_v1_position_arr.end() - 1);
    }

    if(!curve_u1_position_arr.empty())
    {
        result.insert(result.end(), curve_u1_position_arr.begin(), curve_u1_position_arr.end() - 1);
    }

    if(!curve_v0_position_arr.empty())
    {
        result.insert(result.end(), curve_v0_position_arr.begin(), curve_v0_position_arr.end() - 1);
    }

    return result;
}

std::vector<nous::pos_t> convert_surface_boundary_to_polyline(INurbsSurfaceBasePtr surface)
{
    std::vector<nous::pos_t> result;

    if(surface->type() == EElementType::ClosedSurface)
    {
        return convert_surface_boundary_to_polyline(surface->to_nurbs_closed_surface());
    }

    if(surface->type() == EElementType::AlgebraSurface)
    {
        return convert_surface_boundary_to_polyline(surface->to_nurbs_algebra_surface());
    }

    return result;
}

bool is_curve_smooth_close(const unsigned int degree, const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr,
                           const std::vector<nous::scalar>& weight_arr)
{
    for(int i = 0; i < degree; ++i)
    {
        int j = control_point_arr.size() - degree + i;
        int k = weight_arr.size() - degree + i;

        if((control_point_arr[i] - control_point_arr[j]).norm() > 1e-2)
        {
            return false;
        }

        if((weight_arr[i] - weight_arr[k]) > 1e-5)
        {
            return false;
        }
    }

    for(int i = 0; i < degree - 1; ++i)
    {
        int j = knot_arr.size() - degree + i;
        if(std::abs((knot_arr[i + 1] - knot_arr[i]) - (knot_arr[j + 1] - knot_arr[j])) > 1e-5)
        {
            return false;
        }
    }

    return true;
}

bool is_curve_sharp_close(const unsigned int degree, const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr)
{
    if((control_point_arr.front() - control_point_arr.back()).norm() > 1e-2)
    {
        return false;
    }

    for(int i = 0; i < degree; ++i)
    {
        int j = knot_arr.size() - degree + i;
        if(knot_arr[i] != 0.0 || knot_arr[j] != 1.0)
        {
            return false;
        }
    }

    return true;
}

};// namespace nurbs
};// namespace sketch