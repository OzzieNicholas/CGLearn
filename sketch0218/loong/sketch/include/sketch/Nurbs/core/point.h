#pragma once
#ifndef TINYNURBS_POINT_H
#define TINYNURBS_POINT_H

#include <Eigen/Dense>

namespace tinynurbs
{

enum ENurbsPointType : uint8_t
{
    // 普通点
    Common = 0x00,
    // 共享点
    SharedPoint = 0x01,
    // 在 Surface 面上的点
    PointOnSurface = 0x02,
};

template<typename T> class NurbsPoint
{
public:
    NurbsPoint(ENurbsPointType type = ENurbsPointType::Common)
    {
        m_point = Eigen::Vector<T, 3>::Zero();
        m_type = type;
    }

    NurbsPoint(T x, T y, T z, ENurbsPointType type = ENurbsPointType::Common)
    {
        m_point = Eigen::Vector<T, 3>(x, y, z);
        m_type = type;
    }

    NurbsPoint(const Eigen::Vector<T, 3>& point, ENurbsPointType type = ENurbsPointType::Common)
    {
        m_point = Eigen::Vector<T, 3>(point.x(), point.y(), point.z());
        m_type = type;
    }

    NurbsPoint(const NurbsPoint& other)
    {
        m_point = Eigen::Vector<T, 3>(other.m_point.x(), other.m_point.y(), other.m_point.z());
        m_type = other.m_type;
    }

    NurbsPoint(NurbsPoint&& other)
    {
        m_point = Eigen::Vector<T, 3>(other.m_point.x(), other.m_point.y(), other.m_point.z());
        m_type = other.m_type;
    }

    virtual Eigen::Vector<T, 3> get_position() { return m_point; }

    virtual void set_position(const Eigen::Vector<T, 3>& pos) { m_point = pos; }

public:
    Eigen::Vector<T, 3> m_point;

    ENurbsPointType m_type = ENurbsPointType::Common;
};

template<typename T> using NurbsPointPtr = std::shared_ptr<NurbsPoint<T>>;

}// namespace tinynurbs

#endif