#pragma once
#include "./NurbsData.h"
#include <map>

namespace sketch
{
class GeoData;
class INurbsBase;
class MeshDoc;
class NurbsDoc;
class View;

namespace nurbs
{

unsigned int get_curve_shared_point_number(const NurbsCurvePtr curve);

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> least_square_approximation(const size_t degree,
                                                                                   const std::vector<nous::pos_t>& through_points,
                                                                                   const int control_points_count);

/*
 * @brief The Nurbs Book 2nd 第290页提供的算法，A9.6伪代码，用于实现带导矢约束的最小二乘拟合
 * @param Q 目标拟合点数组
 * @param r Q的最大下标，等于 Q.size() - 1;
 * @param Wq 拟合点的权重, Wq[i]>0 说明 Qi 是非约束点，且 Wq[i] 对应的权值 Wqi，Wq[i]<0 说明是约束点
 * @param D 用来存储导矢的数组
 * @param s D的最大下标，等于 D.size() - 1; s == -1 说明没有指定导矢
 * @param I 整形数组，I[j] 是导矢 D[j] 对应的数据点在 Q 中的下标
 * @param Wd Wd[j]>0 说明 D[j] 是非约束的，否则是约束的
 * @param n 用含有 n+1 个控制点的曲线进行拟合
 * @param p 拟合曲线的阶数
 * @return 返回的 Nurbs 曲线对象
 */
std::shared_ptr<tinynurbs::Curve<nous::scalar>> wc_least_square_curve(const std::vector<nous::pos_t>& Q, const int r,
                                                                      const std::vector<nous::scalar>& Wq, const std::vector<nous::vec3>& D,
                                                                      const int s, const std::vector<int>& I, const std::vector<nous::scalar>& Wd,
                                                                      const int n, const int p);

/*
 * @brief Source: Piegl & Tiller: "The NURBS Book" - chapter 9.3.4，translate from ezdxf python
 */
std::shared_ptr<tinynurbs::Curve<nous::scalar>> local_cubic_bspline_interpolation_from_tangent(const std::vector<nous::pos_t>& fit_points,
                                                                                               const std::vector<nous::vec3>& tangents);
/*
 * @brief Source: Piegl & Tiller: "The NURBS Book" - chapter 9.2.1，A9.3 伪代码，曲面 UV 方向节向量参数计算方法
 */
void surface_mesh_params(const unsigned int n, const unsigned int m, const std::vector<std::vector<nous::pos_t>>& Q, std::vector<nous::scalar>& uk,
                         std::vector<nous::scalar>& vl);

/*
 * @brief Source: Piegl & Tiller: "The NURBS Book" - chapter 9.2.1，A9.1 伪代码，全局曲线插值方法
 */
std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> global_curve_interpolation(const unsigned int n, const std::vector<nous::pos_t>& Q,
                                                                                   const unsigned int p);

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> global_curve_interpolation(const unsigned int n, const std::vector<nous::pos_t>& Q,
                                                                                   const unsigned int p, const std::vector<nous::scalar>& uk);

/*
 * @brief "The NURBS Book" - chapter 9.2.3 三次样条曲线插值，A9.2 伪代码，通过求解三对角方程组计算 C^2 连续的三次样条曲线，
 * 默认生成 fit_points.size() + 2 个控制点的 3 阶 Nurbs曲线
 * @param Q 拟合点数组
 * @param auto_head_tangent 是否自动计算首点的切矢方向，默认为 true，若设定为 false 会使用 head_tangent 的值
 * @param head_tangent 首点的切矢方向，默认为 fit_point 第一段的方向
 * @param auto_tail_tangent 是否自动计算尾点的切矢方向，默认为 true，若设定为 false 会使用 tail_tangent 的值
 * @param tail_tangent 尾点的切矢方向，默认为 fit_point 最后一段的方向
 * @param type 生成曲线的闭合类型，Open SharpClose SmoothClose
 * @return 返回的 Nurbs 曲线对象
 */
std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> solve_tridiagonal(
    const std::vector<nous::pos_t>& fit_points, const bool auto_head_tangent = true, const nous::vec3 head_tangent = nous::vec3::Zero(),
    const bool auto_tail_tangent = true, const nous::vec3 tail_tangent = nous::vec3::Zero(), const ENurbsCloseType type = ENurbsCloseType::Open);

struct PointFrames
{
    nous::vec3 up;
    nous::vec3 pos;
    nous::vec3 tangent;
    nous::vec3 normal;
};

NurbsAlgebraSurfacePtr extrude_curve_to_surface(const NurbsCurvePtr curve, const nous::vec3& vector);

std::vector<NurbsAlgebraSurfacePtr> extrude_curves_to_surface_with_shared_edge(const std::vector<NurbsCurvePtr>& curves, const nous::vec3& vector);

// NurbsSurfacePtr sweep_curve_to_surface(const NurbsCurvePtr shape_curve, const NurbsCurvePtr backbone_curve);

NurbsAlgebraSurfacePtr extrude_curve_to_surface(const NurbsCurvePtr shape_curve, const NurbsCurvePtr backbone_curve);

/*
 * @brief 将折线转换为一阶 Nurbs 曲线
 */
NurbsCurvePtr convert_polyline_to_curve(const std::vector<nous::pos_t>& polyline, bool closed = false);

/*
 * @brief 将 Nurbs 曲线转换为折线
 */
std::vector<nous::pos_t> convert_curve_to_polyline(NurbsCurvePtr curve);

std::vector<nous::pos_t> convert_curve_to_polyline_with_limit(NurbsCurvePtr curve, int sample_count = 100);

bool is_curve_coplane(NurbsCurvePtr curve);

nous::scalar get_u_on_curve(NurbsCurvePtr curve, const nous::pos_t& point);

nous::scalar get_u_on_curve(NurbsCurvePtr curve, const nous::pos_t& ori, const nous::vec3& dir);

NurbsCurvePtr capture_curve(NurbsCurvePtr curve, const nous::scalar min_u, const nous::scalar max_u);

NurbsCurvePtr reverse_curve(NurbsCurvePtr curve, bool new_ctp = true);

void transfer_1d_surface_into_geodata(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, const nous::mat4& nurbs_trans);

void transfer_1d_surface_into_geodata_with_weld_info(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, View* view, const nous::mat4& nurbs_trans,
                                                     const std::vector<std::pair<size_t, size_t>> weld_info);

std::vector<NurbsAlgebraSurfacePtr> split_surface_on_u(NurbsAlgebraSurfacePtr surface);

std::vector<NurbsAlgebraSurfacePtr> split_surface_on_u_with_weld_info(NurbsAlgebraSurfacePtr surface,
                                                                      const std::vector<std::pair<size_t, size_t>> weld_info);

std::map<EAlgebraSurfaceBoundaryType, NurbsCurvePtr> extract_surface_boundary_curve(NurbsAlgebraSurfacePtr surface,
                                                                                    const bool ignore_closed_boundary = true);

// 返回 N 个面 ...... 五个或三个，闭合因为存在缝合边所以是三个，开放的是五个
std::vector<NurbsAlgebraSurfacePtr> extrude_surface_by_normal(NurbsAlgebraSurfacePtr surface, const nous::scalar value);

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal(NurbsClosedSurfacePtr curve_brep, const nous::scalar value,
                                                                   const nous::vec3 positive_dir);

std::vector<std::vector<nous::pos_t>> generate_surface_sample_points_with_1d_limits(NurbsAlgebraSurfacePtr surface, const unsigned int ref_rows,
                                                                                    const unsigned int ref_cols);

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal_keep_distance(NurbsAlgebraSurfacePtr surface, const nous::scalar value);

std::vector<std::shared_ptr<INurbsBase>> extrude_surface_by_normal_keep_distance(NurbsTrimmedSurfacePtr surface, const nous::scalar value);

bool is_curve_smooth_close(const unsigned int degree, const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr,
                           const std::vector<nous::scalar>& weight_arr);

bool is_curve_sharp_close(const unsigned int degree, const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr);

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_with_transform(NurbsAlgebraSurfacePtr surface, const nous::transform3 transform);

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_with_transform(NurbsClosedSurfacePtr surface, const nous::transform3 transform);

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_with_transform(NurbsTrimmedSurfacePtr surface, const nous::transform3 transform);

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_by_path(NurbsCurvePtr curve, const std::vector<NurbsCurvePtr>& path);

std::vector<NurbsAlgebraSurfacePtr> offset_surface_to_solid_by_path(NurbsAlgebraSurfacePtr surface, const std::vector<NurbsCurvePtr>& path,
                                                                    bool with_top = true);

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_by_path(NurbsClosedSurfacePtr curve_brep, const std::vector<NurbsCurvePtr>& path,
                                                                         bool with_top = true);

std::vector<std::shared_ptr<INurbsBase>> offset_surface_to_solid_by_path(NurbsTrimmedSurfacePtr trimmed_surface,
                                                                         const std::vector<NurbsCurvePtr>& path);

bool is_same_curve_object(NurbsCurvePtr first, NurbsCurvePtr second);

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsClosedSurfaceEdgeElementPtr edge_curve);

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsCurvePtr curve);

std::unordered_set<SketchElementPtr> find_same_edge_elements(NurbsAlgebraSurfaceEdgeElementPtr edge_curve);

std::unordered_set<SketchElementPtr> find_same_edge_elements(SketchElementPtr edge_curve);

std::vector<nous::pos_t> convert_surface_boundary_to_polyline(INurbsSurfaceBasePtr surface);

void convert_nurbs_surface_to_mesh(MeshDoc* mesh_doc, NurbsAlgebraSurfacePtr surface, const nous::mat4& nurbs_trans);

void convert_nurbs_surface_to_mesh(MeshDoc* mesh_doc, NurbsDoc* nurbs_doc, std::shared_ptr<INurbsBase> nurbs_item);

};// namespace nurbs
};// namespace sketch