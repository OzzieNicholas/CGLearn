#include "sketch/Nurbs/util/nurbs_factory.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/View/RenderRes/NurbsData.h"

namespace sketch::nurbs
{

std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> make_nurbs_circle(const nous::pos_t& O, const nous::vec3& X, const nous::vec3& Y,
                                                                          const double r, const double start_angle, double end_angle)
{
    if(end_angle < start_angle)
    {
        end_angle += 360.0;
    }

    const double angle_theta = end_angle - start_angle;
    unsigned int arc_number = 0;
    if(angle_theta <= 90.0)
    {
        arc_number = 1;
    }
    else if(angle_theta <= 180.0)
    {
        arc_number = 2;
    }
    else if(angle_theta <= 270.0)
    {
        arc_number = 3;
    }
    else
    {
        arc_number = 4;
    }

    const double delta_angle = angle_theta / arc_number;
    const unsigned int n = 2 * arc_number;
    const double w1 = cos(delta_angle / 2.0 / 180.0 * 3.141592653);
    // double w1 = 1.0;
    nous::pos_t P0 = O + r * cos(start_angle / 180.0 * 3.141592653) * X + r * sin(start_angle / 180.0 * 3.141592653) * Y;
    nous::pos_t T0 = -sin(start_angle / 180.0 * 3.141592653) * X + cos(start_angle / 180.0 * 3.141592653) * Y;

    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> circle = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();
    circle->control_points.emplace_back(std::make_shared<SketchNurbsPoint>(P0));
    circle->weights.emplace_back(1.0);
    circle->degree = 2;

    double angle = start_angle;

    for(int i = 1; i <= arc_number; ++i)
    {
        angle += delta_angle;
        const nous::pos_t P2 = O + r * cos(angle / 180.0 * 3.141592653) * X + r * sin(angle / 180.0 * 3.141592653) * Y;
        const nous::pos_t T2 = -sin(angle / 180.0 * 3.141592653) * X + cos(angle / 180.0 * 3.141592653) * Y;
        nous::pos_t cross1, cross2;
        const bool is_intersect = sketch_math::is_line_intersect_3d(P0, P0 + T0, P2, P2 + T2, cross1, cross2, false);
        const nous::pos_t P1 = cross1;
        circle->control_points.emplace_back(std::make_shared<SketchNurbsPoint>(P1));
        circle->weights.emplace_back(w1);
        circle->control_points.emplace_back(std::make_shared<SketchNurbsPoint>(P2));
        circle->weights.emplace_back(1.0);
        if(i < arc_number)
        {
            P0 = P2;
            T0 = T2;
        }
    }

    for(int i = 0; i < 3; i++)
    {
        circle->knots.emplace_back(0.0);
    }
    switch(arc_number)
    {
    case 1: break;
    case 2:
        circle->knots.emplace_back(0.5);
        circle->knots.emplace_back(0.5);
        break;
    case 3:
        circle->knots.emplace_back(1.0 / 3.0);
        circle->knots.emplace_back(1.0 / 3.0);
        circle->knots.emplace_back(2.0 / 3.0);
        circle->knots.emplace_back(2.0 / 3.0);
        break;
    case 4:
        circle->knots.emplace_back(0.25);
        circle->knots.emplace_back(0.25);
        circle->knots.emplace_back(0.5);
        circle->knots.emplace_back(0.5);
        circle->knots.emplace_back(0.75);
        circle->knots.emplace_back(0.75);
        break;
    default: break;
    }
    for(int i = 0; i < 3; i++)
    {
        circle->knots.emplace_back(1.0);
    }

    return circle;
}

std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> make_nurbs_rectangle(const nous::pos_t& p00, const nous::pos_t& p01, const nous::pos_t& p10,
                                                                               const nous::pos_t& p11)
{
    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> rectangle = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    rectangle->knots_u.emplace_back(0);
    rectangle->knots_u.emplace_back(0);
    rectangle->knots_u.emplace_back(1);
    rectangle->knots_u.emplace_back(1);
    rectangle->knots_v.emplace_back(0);
    rectangle->knots_v.emplace_back(0);
    rectangle->knots_v.emplace_back(1);
    rectangle->knots_v.emplace_back(1);
    rectangle->degree_u = 1;
    rectangle->degree_v = 1;
    rectangle->control_points.resize(2, 2, nullptr);
    rectangle->control_points[0] = std::make_shared<SketchNurbsPoint>(p00);
    rectangle->control_points[1] = std::make_shared<SketchNurbsPoint>(p01);
    rectangle->control_points[2] = std::make_shared<SketchNurbsPoint>(p10);
    rectangle->control_points[3] = std::make_shared<SketchNurbsPoint>(p11);
    rectangle->weights.resize(2, 2, 1.0);

    return rectangle;
}

std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> make_nurbs_rectangle(const nous::pos_t& p00, const nous::pos_t& p01, const nous::pos_t& p10,
                                                                               const nous::pos_t& p11, const size_t degree_u, const size_t degree_v,
                                                                               const size_t rows, const size_t cols)
{
    // 横向多少 control points
    const size_t fixed_rows = std::max(degree_u + 1, rows);
    // 纵向多少 control points
    const size_t fixed_cols = std::max(degree_v + 1, cols);

    const size_t u_knot_number = fixed_rows + degree_u + 1;
    const size_t v_knot_number = fixed_cols + degree_v + 1;

    std::vector<nous::scalar> knots_u(degree_u + 1, 0.0);
    std::vector<nous::scalar> knots_v(degree_v + 1, 0.0);

    const size_t u_knot_middle_number = u_knot_number - (degree_u + 1) * 2;
    const size_t v_knot_middle_number = v_knot_number - (degree_v + 1) * 2;

    for(int i = 0; i < u_knot_middle_number; ++i)
    {
        knots_u.emplace_back(1.0 / (u_knot_middle_number + 1) * (i + 1));
    }
    for(int i = 0; i < v_knot_middle_number; ++i)
    {
        knots_v.emplace_back(1.0 / (v_knot_middle_number + 1) * (i + 1));
    }

    for(int i = 0; i < degree_u + 1; ++i)
    {
        knots_u.emplace_back(1.0);
    }
    for(int i = 0; i < degree_v + 1; ++i)
    {
        knots_v.emplace_back(1.0);
    }

    const nous::vec3 delta_row = (p10 - p00) / (fixed_rows - 1);
    const nous::vec3 delta_col = (p01 - p00) / (fixed_cols - 1);
    // 将控制点打成格网
    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> rectangle = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    rectangle->control_points.resize(fixed_rows, fixed_cols, nullptr);
    for(int i = 0; i < fixed_rows; ++i)
    {
        for(int j = 0; j < fixed_cols; ++j)
        {
            rectangle->control_points[i * fixed_cols + j] = std::make_shared<SketchNurbsPoint>(p00 + delta_row * i + delta_col * j);
        }
    }
    rectangle->weights.resize(fixed_rows, fixed_cols, 1.0);
    rectangle->degree_u = degree_u;
    rectangle->degree_v = degree_v;
    rectangle->knots_u.insert(rectangle->knots_u.end(), knots_u.begin(), knots_u.end());
    rectangle->knots_v.insert(rectangle->knots_v.end(), knots_v.begin(), knots_v.end());

    return rectangle;
}

};// namespace sketch::nurbs