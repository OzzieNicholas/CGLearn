/**
 * Helper functions
 *
 * Use of this source code is governed by a BSD-style license that can be found in
 * the LICENSE file.
 */

#ifndef TINYNURBS_UTIL
#define TINYNURBS_UTIL

#include "../core/curve.h"
#include "array2.h"
#include <Eigen/Dense>
#include <vector>

namespace tinynurbs
{
namespace util
{

/**
 * Convert an nd point in homogenous coordinates to an (n-1)d point in cartesian
 * coordinates by perspective division
 * @param[in] pt Point in homogenous coordinates
 * @return Point in cartesian coordinates
 */
template<int nd, typename T> inline Eigen::Vector<T, nd - 1> homogenousToCartesian(const Eigen::Vector<T, nd>& pt)
{
    T w = pt[pt.SizeAtCompileTime - 1];
    Eigen::Vector<T, nd - 1> result(pt.x() / w, pt.y() / w, pt.z() / w);
    return result;
}

/**
 * Convert a list of nd points in homogenous coordinates to a list of (n-1)d points in cartesian
 * coordinates by perspective division
 * @param[in] ptsws Points in homogenous coordinates
 * @param[out] pts Points in cartesian coordinates
 * @param[out] ws Homogenous weights
 */
template<int nd, typename T>
inline void homogenousToCartesian(const std::vector<Eigen::Vector<T, nd>>& ptsws, std::vector<Eigen::Vector<T, nd - 1>>& pts, std::vector<T>& ws)
{
    pts.clear();
    ws.clear();
    pts.reserve(ptsws.size());
    ws.reserve(ptsws.size());
    for(int i = 0; i < ptsws.size(); ++i)
    {
        const Eigen::Vector<nd, T>& ptw_i = ptsws[i];
        pts.push_back(Eigen::Vector<T, nd - 1>(ptw_i / ptw_i[ptw_i.length() - 1]));
        ws.push_back(ptw_i[ptw_i.length() - 1]);
    }
}

/**
 * Convert a 2D list of nd points in homogenous coordinates to cartesian
 * coordinates by perspective division
 * @param[in] ptsws Points in homogenous coordinates
 * @param[out] pts Points in cartesian coordinates
 * @param[out] ws Homogenous weights
 */
template<int nd, typename T>
inline void homogenousToCartesian(const array2<Eigen::Vector<T, nd>>& ptsws, array2<Eigen::Vector<T, nd - 1>>& pts, array2<T>& ws)
{
    pts.resize(ptsws.rows(), ptsws.cols());
    ws.resize(ptsws.rows(), ptsws.cols());
    for(int i = 0; i < ptsws.rows(); ++i)
    {
        for(int j = 0; j < ptsws.cols(); ++j)
        {
            const Eigen::Vector<T, nd>& ptw_ij = ptsws(i, j);
            T w_ij = ptw_ij[nd - 1];
            pts(i, j) = Eigen::Vector<T, nd - 1>(ptw_ij / w_ij);
            ws(i, j) = w_ij;
        }
    }
}

template<typename T> inline void homogenousToCartesian(const array2<Eigen::Vector<T, 4>>& ptsws, array2<NurbsPointPtr<T>>& pts, array2<T>& ws)
{
    pts.resize(ptsws.rows(), ptsws.cols());
    ws.resize(ptsws.rows(), ptsws.cols());
    for(int i = 0; i < ptsws.rows(); ++i)
    {
        for(int j = 0; j < ptsws.cols(); ++j)
        {
            const Eigen::Vector<T, 4>& ptw_ij = ptsws(i, j);
            T w_ij = ptw_ij[3];
            const Eigen::Vector<T, 3> pt_ij(ptw_ij.x() / w_ij, ptw_ij.y() / w_ij, ptw_ij.z() / w_ij);
            pts(i, j) = std::make_shared<NurbsPoint<T>>(pt_ij);
            ws(i, j) = w_ij;
        }
    }
}

/**
 * Convert an nd point in cartesian coordinates to an (n+1)d point in homogenous
 * coordinates
 * @param[in] pt Point in cartesian coordinates
 * @param[in] w Weight
 * @return Input point in homogenous coordinates
 */
template<int nd, typename T> inline Eigen::Vector<T, nd + 1> cartesianToHomogenous(const Eigen::Vector<T, nd>& pt, T w)
{
    Eigen::Vector<T, nd> tmp = pt * w;
    return Eigen::Vector<T, nd + 1>(tmp.x(), tmp.y(), tmp.z(), w);
}

/**
 * Convert list of points in cartesian coordinates to homogenous coordinates
 * @param[in] pts Points in cartesian coordinates
 * @param[in] ws Weights
 * @return Points in homogenous coordinates
 */
template<int nd, typename T>
inline std::vector<Eigen::Vector<T, nd + 1>> cartesianToHomogenous(const std::vector<Eigen::Vector<T, nd>>& pts, const std::vector<T>& ws)
{
    std::vector<Eigen::Vector<T, nd + 1>> Cw;
    Cw.reserve(pts.size());
    for(int i = 0; i < pts.size(); ++i)
    {
        Cw.push_back(cartesianToHomogenous(pts[i], ws[i]));
    }
    return Cw;
}

/**
 * Convert 2D list of points in cartesian coordinates to homogenous coordinates
 * @param[in] pts Points in cartesian coordinates
 * @param[in] ws Weights
 * @return Points in homogenous coordinates
 */
template<int nd, typename T>
inline array2<Eigen::Vector<T, nd + 1>> cartesianToHomogenous(const array2<Eigen::Vector<T, nd>>& pts, const array2<T>& ws)
{
    array2<Eigen::Vector<T, nd + 1>> Cw(pts.rows(), pts.cols());
    for(int i = 0; i < pts.rows(); ++i)
    {
        for(int j = 0; j < pts.cols(); ++j)
        {
            Cw(i, j) = util::cartesianToHomogenous(pts(i, j), ws(i, j));
        }
    }
    return Cw;
}

template<typename T> inline std::vector<Eigen::Vector<T, 4>> cartesianToHomogenous(const array2<NurbsPointPtr<T>>& pts, const array2<T>& ws)
{
    array2<Eigen::Vector<T, 4>> Cw(pts.rows(), pts.cols());
    for(int i = 0; i < pts.rows(); ++i)
    {
        for(int j = 0; j < pts.cols(); ++j)
        {
            Cw(i, j) = util::cartesianToHomogenous(pts(i, j)->m_point, ws(i, j));
        }
    }
    return Cw;
}

/**
 * Convert an (n+1)d point to an nd point without perspective division
 * by truncating the last dimension
 * @param[in] pt Point in homogenous coordinates
 * @return Input point in cartesian coordinates
 */
template<int nd, typename T> inline Eigen::Vector<T, nd - 1> truncateHomogenous(const Eigen::Vector<T, nd>& pt)
{
    return pt.head<nd - 1>();
    // return Eigen::Vector<T, nd - 1>(pt);
}

/**
 * Compute the binomial coefficient (nCk) using the formula
 * \product_{i=0}^k (n + 1 - i) / i
 */
inline unsigned int binomial(unsigned int n, unsigned int k)
{
    unsigned int result = 1;
    if(k > n)
    {
        return 0;
    }
    for(unsigned int i = 1; i <= k; ++i)
    {
        result *= (n + 1 - i);
        result /= i;
    }
    return result;
}

/**
 * Check if two numbers are close enough within eps
 * @param[in] a First number
 * @param[in] b Second number
 * @param[in] eps Tolerance for checking closeness
 * @return Whether the numbers are close w.r.t. the tolerance
 */
template<typename T> inline bool close(T a, T b, double eps = std::numeric_limits<T>::epsilon()) { return (std::abs(a - b) < eps) ? true : false; }

/**
 * Map numbers from one interval to another
 * @param[in] val Number to map to another range
 * @param[in] old_min Minimum value of original range
 * @param[in] old_max Maximum value of original range
 * @param[in] new_min Minimum value of new range
 * @param[in] new_max Maximum value of new range
 * @return Number mapped to new range
 */
template<typename T> inline T mapToRange(T val, T old_min, T old_max, T new_min, T new_max)
{
    T old_range = old_max - old_min;
    T new_range = new_max - new_min;
    return (((val - old_min) * new_range) / old_range) + new_min;
}

template<typename T> T get_total_chord_length(const std::vector<Eigen::Vector<T, 3>>& through_points)
{
    const int n = through_points.size();
    T length = 0.0;
    for(int i = 1; i < n; ++i)
    {
        length += (through_points[i] - through_points[i - 1]).norm();
    }
    return length;
}

template<typename T> std::vector<T> get_chord_parameterization(const std::vector<Eigen::Vector<T, 3>>& through_points)
{
    if(through_points.empty())
    {
        return std::vector<T>();
    }
    const int size = through_points.size();
    const int n = size - 1;

    std::vector<T> uk(size, 0.0);
    uk[n] = 1.0;

    const T d = get_total_chord_length(through_points);
    for(int i = 1; i < n; ++i)
    {
        uk[i] = uk[i - 1] + (through_points[i] - through_points[i - 1]).norm() / d;
    }
    return uk;
}

template<typename T> void normalize_curve_knots(RationalCurve<T>& curve)
{
    const T min_u = curve.knots.front();
    const T max_u = curve.knots.back();

    for(int i = 0; i < curve.knots.size(); ++i)
    {
        curve.knots[i] = (curve.knots[i] - min_u) / (max_u - min_u);
    }
}

}// namespace util
}// namespace tinynurbs

#endif// TINYNURBS_UTIL
