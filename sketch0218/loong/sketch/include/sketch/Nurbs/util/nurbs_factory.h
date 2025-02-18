#pragma once

#include "../core/curve.h"
#include "../core/surface.h"
#include <memory>
#include <nous_base/types.h>
#include <nous_mesh/types.h>

namespace sketch
{
namespace nurbs
{
std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> make_nurbs_circle(const nous::pos_t& O, const nous::vec3& X, const nous::vec3& Y,
                                                                          const double r, const double start_angle, double end_angle);

std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> make_nurbs_rectangle(const nous::pos_t& p00, const nous::pos_t& p01, const nous::pos_t& p10,
                                                                               const nous::pos_t& p11);

std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> make_nurbs_rectangle(const nous::pos_t& p00, const nous::pos_t& p01, const nous::pos_t& p10,
                                                                               const nous::pos_t& p11, const size_t degree_u, const size_t degree_v,
                                                                               const size_t rows, const size_t cols);

};// namespace nurbs
};// namespace sketch