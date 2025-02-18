#pragma once

#include "nous_mesh/mesh.h"

namespace sketch
{

class SketchBoundingSphere
{
public:
    SketchBoundingSphere();

    SketchBoundingSphere(const nous::pos_t& center, const nous::scalar radius);

    bool is_intersects(const SketchBoundingSphere& other);

    nous::pos_t get_center() const;

    nous::scalar get_radius() const;

    void update(const nous::pos_t& center, const nous::scalar radius);

private:
    nous::pos_t m_center = nous::pos_t::Zero();
    nous::scalar m_radius = 0.0f;
};

};// namespace sketch