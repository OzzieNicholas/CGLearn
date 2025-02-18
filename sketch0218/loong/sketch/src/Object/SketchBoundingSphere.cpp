#include "sketch/NousEngine/SketchBoundingSphere.h"

namespace sketch
{

SketchBoundingSphere::SketchBoundingSphere() : m_center(nous::pos_t::Zero()), m_radius(0.0) {}

SketchBoundingSphere::SketchBoundingSphere(const nous::pos_t& center, const nous::scalar radius) : m_center(center), m_radius(radius) {}

bool SketchBoundingSphere::is_intersects(const SketchBoundingSphere& other)
{
    double center_distance = (other.get_center() - get_center()).norm();
    double radius_distance = other.get_radius() + get_radius() + 0.1f;
    return center_distance <= radius_distance;
}

nous::pos_t SketchBoundingSphere::get_center() const { return m_center; }

nous::scalar SketchBoundingSphere::get_radius() const { return m_radius; }

void SketchBoundingSphere::update(const nous::pos_t& center, const nous::scalar radius)
{
    m_center = center;
    m_radius = radius;
}

};// namespace sketch