#pragma once

#include "Engine/Math/GeoMesh/GeoMesh.h"
#include "Engine/Math/Vec4i.h"
#include "geos/geom.h"
#include "nous_mesh/mesh.h"

namespace sketch
{
class NousConvert
{
protected:
    NousConvert() {}
    ~NousConvert() {}

public:
    inline static nous::vec4 product_v4(const Engine::Mat43& m, const nous::vec3& v) { return NousConvert::product_v4(NousConvert::convert(m), v); }
    inline static nous::vec4 product_v4(const Engine::Mat4f& m, const nous::vec3& v) { return NousConvert::product_v4(NousConvert::convert(m), v); }
    inline static nous::vec4 product_v4(const nous::mat4& m, const nous::vec3& v) { return m * v.homogeneous(); }
    inline static nous::vec3 product_v3(const Engine::Mat43& m, const nous::vec3& v) { return NousConvert::product_v3(NousConvert::convert(m), v); }
    inline static nous::vec3 product_v3(const Engine::Mat4f& m, const nous::vec3& v) { return NousConvert::product_v3(NousConvert::convert(m), v); }
    inline static nous::vec3 product_v3(const nous::mat4& m, const nous::vec3& v)
    {
        nous::vec4 v4 = m * v.homogeneous();
        return v4.head<3>() / v4.w();
    }

    // common
    inline static Engine::Vec4f convert(const nous::vec4& v) { return Engine::Vec4f(v.x(), v.y(), v.z(), v.w()); }
    inline static nous::vec4 convert(const Engine::Vec4i& v) { return nous::vec4(v.x, v.y, v.z, v.w); }
    inline static Engine::Vec3 convert(const nous::vec3& v) { return Engine::Vec3(v.x(), v.y(), v.z()); }
    inline static nous::vec4 convert(const Engine::Vec4f& v) { return nous::vec4(v.x, v.y, v.z, v.w); }
    inline static nous::vec3 convert(const Engine::Vec3& v) { return nous::vec3(v.x, v.y, v.z); }
    inline static nous::vec2 convert(const Engine::Vec2f& v) { return nous::vec2(v.x, v.y); }
    inline static Engine::Vec2 convert(const nous::vec2& v) { return Engine::Vec2(v.x(), v.y()); }
    inline static nous::mat4 convert(const Engine::Mat4f& m)
    {
        nous::mat4 mat;
        mat.col(0) = nous::vec4(m.col[0].x, m.col[0].y, m.col[0].z, m.col[0].w);
        mat.col(1) = nous::vec4(m.col[1].x, m.col[1].y, m.col[1].z, m.col[1].w);
        mat.col(2) = nous::vec4(m.col[2].x, m.col[2].y, m.col[2].z, m.col[2].w);
        mat.col(3) = nous::vec4(m.col[3].x, m.col[3].y, m.col[3].z, m.col[3].w);
        return mat;
    }
    inline static nous::mat4 convert(const Engine::Mat43& m)
    {
        nous::mat4 mat;
        mat.col(0) = nous::vec4(m.col[0].x, m.col[0].y, m.col[0].z, 0.0f);
        mat.col(1) = nous::vec4(m.col[1].x, m.col[1].y, m.col[1].z, 0.0f);
        mat.col(2) = nous::vec4(m.col[2].x, m.col[2].y, m.col[2].z, 0.0f);
        mat.col(3) = nous::vec4(m.col[3].x, m.col[3].y, m.col[3].z, 1.0f);
        return mat;
    }
    inline static Engine::Mat43 convert(const nous::mat4& v)
    {
        Engine::Mat43 mat;
        mat._00 = v.col(0).coeff(0);
        mat._10 = v.col(0).coeff(1);
        mat._20 = v.col(0).coeff(2);
        mat._01 = v.col(1).coeff(0);
        mat._11 = v.col(1).coeff(1);
        mat._21 = v.col(1).coeff(2);
        mat._02 = v.col(2).coeff(0);
        mat._12 = v.col(2).coeff(1);
        mat._22 = v.col(2).coeff(2);
        mat._03 = v.col(3).coeff(0);
        mat._13 = v.col(3).coeff(1);
        mat._23 = v.col(3).coeff(2);
        return mat;
    }

private:
};

class NousGeometryConverter
{
public:
    static geos::geom::Geometry* position_convert_to_geos_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_positions);

    static geos::geom::Geometry* position_convert_to_geos_polygon(const std::vector<nous::pos_t>& polygon_positions);

    static geos::geom::Geometry* position_convert_to_geos_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_positions,
                                                                  const nous::vec3& normal);

    static geos::geom::Geometry* position_convert_to_geos_polygon(const std::vector<nous::pos_t>& polygon_positions, const nous::vec3& normal);

    static std::vector<std::vector<nous::pos_t>> geos_polygon_convert_to_position(geos::geom::Polygon* geos_polygon);

    static std::vector<std::vector<nous::pos_t>> geos_polygon_convert_to_position(geos::geom::Polygon* geos_polygon, const nous::vec3& normal);
};
}// namespace sketch
