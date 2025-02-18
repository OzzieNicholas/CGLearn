#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/earcut.hpp"

#include "nous/graph/graph_factory.h"
#include "nous/initialize.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/View/RenderRes/GeoData.h"

#include "geos/geom/Geometry.h"
#include "geos/operation/buffer/BufferOp.h"
#include "geos/operation/polygonize/Polygonizer.h"

namespace sketch
{
bool NousEngine::m_inited = false;

NousEngine::NousEngine() : m_graph(nullptr) { m_geosFactory = geos::geom::GeometryFactory::create(sketch_geos_precision_model); }

NousEngine::~NousEngine() {}

NousEngine* NousEngine::instance()
{
    static NousEngine instance;
    return &instance;
}

void NousEngine::init()
{
    if(!m_inited)
    {
        sketch_geos_precision_model = new PrecisionModel(100);
        nous::initialize();
        m_inited = true;
    }
}

void NousEngine::release() { nous::finalize(); }

bool NousEngine::is_inited() { return NousEngine::m_inited; }

namespace geos_util
{

std::vector<geos::geom::Geometry*> extract_valid_polygons(geos::geom::Geometry* geom)
{
    std::vector<geos::geom::Geometry*> result_geometries;

    if(!geom)
    {
        return result_geometries;
    }

    const geos::geom::GeometryTypeId geom_type_id = geom->getGeometryTypeId();

    if(geom_type_id == geos::geom::GeometryTypeId::GEOS_POLYGON)
    {
        result_geometries.emplace_back(geom);
    }
    else if(geom_type_id == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION || geom_type_id == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON)
    {
        for(int i = 0; i < geom->getNumGeometries(); ++i)
        {
            const geos::geom::Geometry* child_geom = geom->getGeometryN(i);

            const geos::geom::GeometryTypeId child_geom_type_id = child_geom->getGeometryTypeId();

            if(child_geom_type_id == geos::geom::GeometryTypeId::GEOS_POLYGON)
            {
                result_geometries.emplace_back(const_cast<geos::geom::Geometry*>(child_geom));
            }
        }
    }

    return result_geometries;
}

bool is_contain(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1) { return geom0->contains(geom1); }

std::vector<geos::geom::Geometry*> difference_polygon(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1)
{
    geos::geom::Geometry* diff_result = geom0->difference(geom1).release();
    return geos_util::extract_valid_polygons(diff_result);
}

std::vector<geos::geom::Geometry*> intersect_polygon(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1)
{
    geos::geom::Geometry* intersect_result = geom0->intersection(geom1).release();
    return geos_util::extract_valid_polygons(intersect_result);
}

std::vector<geos::geom::Geometry*> union_polygon(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1)
{
    return std::vector<geos::geom::Geometry*>();
}

bool buffer_ring(const std::vector<nous::pos_t>& ring, const nous::vec3& normal, float offset,
                 std::vector<std::vector<std::vector<nous::vec3>>>& result)
{
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float org_z = 0.f;

    std::vector<std::vector<nous::pos_t>> ring_as_polygon_pos_arr;
    std::vector<nous::pos_t>& ring_xoy_positions = ring_as_polygon_pos_arr.emplace_back();
    for(const nous::pos_t& pt_pos: ring)
    {
        nous::pos_t pt_pos_rotate = rotate_to_xy_plane * pt_pos;
        org_z = pt_pos_rotate.z();
        ring_xoy_positions.emplace_back(pt_pos_rotate);
    }

    geos::geom::Geometry* ring_geometry = NousGeometryConverter::position_convert_to_geos_polygon(ring_as_polygon_pos_arr);
    if(!ring_geometry)
    {
        return false;
    }

    geos::operation::buffer::BufferParameters buffer_params;
    buffer_params.setJoinStyle(geos::operation::buffer::BufferParameters::JOIN_MITRE);
    geos::operation::buffer::BufferOp buffer_op(ring_geometry, buffer_params);
    geos::geom::Geometry* buffer_geometry = buffer_op.getResultGeometry(offset).release();

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(buffer_geometry);
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto result_part = NousGeometryConverter::geos_polygon_convert_to_position(static_cast<geos::geom::Polygon*>(polygon));
        if(result_part.size())
        {
            for(auto& ring: result_part)
            {
                for(auto& pt_pos: ring)
                {
                    pt_pos = rotate_to_xy_plane.conjugate() * nous::pos_t(pt_pos.x(), pt_pos.y(), org_z);
                }
            }
            result.emplace_back(std::move(result_part));
        }
    }

    return true;
}

bool union_all_rings(const std::vector<std::vector<nous::vec3>>& rings, const nous::vec3& normal,
                     std::vector<std::vector<std::vector<nous::vec3>>>& result)
{
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float org_z = 0.f;

    std::vector<geos::geom::Geometry*> ring_as_polygon_arr;
    for(const auto& ring: rings)
    {
        std::vector<std::vector<nous::pos_t>> ring_as_polygon_pos_arr;
        std::vector<nous::pos_t>& ring_xoy_positions = ring_as_polygon_pos_arr.emplace_back();
        for(const nous::pos_t& pt_pos: ring)
        {
            nous::pos_t pt_pos_rotate = rotate_to_xy_plane * pt_pos;
            org_z = pt_pos_rotate.z();
            ring_xoy_positions.emplace_back(std::move(pt_pos_rotate));
        }
        geos::geom::Geometry* ring_geometry = NousGeometryConverter::position_convert_to_geos_polygon(ring_as_polygon_pos_arr);
        if(ring_geometry)
        {
            ring_as_polygon_arr.emplace_back(ring_geometry);
        }
    }

    const geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create();
    geos::geom::Geometry* union_polygon = geos_factory->createPolygon().release();
    for(auto geo: ring_as_polygon_arr)
    {
        union_polygon = union_polygon->Union(geo).release();
    }

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(union_polygon);
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto result_part = NousGeometryConverter::geos_polygon_convert_to_position(static_cast<geos::geom::Polygon*>(polygon));
        if(result_part.size())
        {
            for(auto& ring: result_part)
            {
                for(auto& pt_pos: ring)
                {
                    nous::pos_t org_rotate_pos = nous::pos_t(pt_pos.x(), pt_pos.y(), org_z);
                    pt_pos = rotate_to_xy_plane.conjugate() * org_rotate_pos;
                }
            }
            result.emplace_back(std::move(result_part));
        }
    }

    return true;
}

bool ring_difference_other_ring(const std::vector<nous::vec3>& main_ring, const std::vector<std::vector<nous::vec3>>& others,
                                const nous::vec3& normal, std::vector<std::vector<std::vector<nous::vec3>>>& result)
{
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float org_z = 0.f;

    // other geometry
    std::vector<geos::geom::Geometry*> other_ring_polygon_arr;
    for(const auto& ring: others)
    {
        std::vector<std::vector<nous::pos_t>> ring_as_polygon_pos_arr;
        std::vector<nous::pos_t>& ring_xoy_positions = ring_as_polygon_pos_arr.emplace_back();
        for(const nous::pos_t& pt_pos: ring)
        {
            nous::pos_t pt_pos_rotate = rotate_to_xy_plane * pt_pos;
            org_z = pt_pos_rotate.z();
            ring_xoy_positions.emplace_back(rotate_to_xy_plane * pt_pos_rotate);
        }
        geos::geom::Geometry* other_ring_geometry = NousGeometryConverter::position_convert_to_geos_polygon(ring_as_polygon_pos_arr);
        if(other_ring_geometry)
        {
            other_ring_polygon_arr.emplace_back(other_ring_geometry);
        }
    }
    const geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create();
    geos::geom::Geometry* other_geometry = geos_factory->createPolygon().release();
    for(auto other_geo: other_ring_polygon_arr)
    {
        other_geometry = other_geometry->Union(other_geo).release();
    }

    // ring
    std::vector<nous::pos_t> ring_as_polygon;
    for(const nous::pos_t& pt_pos: main_ring)
    {
        nous::pos_t pt_pos_rotate = rotate_to_xy_plane * pt_pos;
        org_z = pt_pos_rotate.z();
        ring_as_polygon.emplace_back(pt_pos_rotate);
    }
    geos::geom::Geometry* main_ring_geometry = NousGeometryConverter::position_convert_to_geos_polygon(ring_as_polygon);
    if(!main_ring_geometry)
    {
        return false;
    }

    // sub
    std::vector<geos::geom::Geometry*> result_geo = difference_polygon(main_ring_geometry, other_geometry);

    // result
    for(auto geo: result_geo)
    {
        std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(geo);
        for(geos::geom::Geometry* polygon: valid_polygons)
        {
            auto result_part = NousGeometryConverter::geos_polygon_convert_to_position(static_cast<geos::geom::Polygon*>(polygon));
            if(result_part.size())
            {
                for(auto& ring: result_part)
                {
                    for(auto& pt_pos: ring)
                    {
                        nous::pos_t org_rotate_pos = nous::pos_t(pt_pos.x(), pt_pos.y(), org_z);
                        pt_pos = rotate_to_xy_plane.conjugate() * org_rotate_pos;
                    }
                }
                result.emplace_back(std::move(result_part));
            }
        }
    }
    return true;
}

bool is_polygon_valid(const std::vector<std::vector<nous::pos_t>>& polygon)
{
    if(polygon.empty() || polygon.front().size() < 3)
    {
        return false;
    }

    geos::geom::Geometry* geometry = NousGeometryConverter::position_convert_to_geos_polygon(polygon, sketch_math::ring_normal(polygon.front()));

    if(!geometry)
    {
        return false;
    }

    return geometry->isValid();
}

bool is_polygon_valid(const std::vector<nous::pos_t>& polygon)
{
    if(polygon.size() < 3)
    {
        return false;
    }

    geos::geom::Geometry* geometry = NousGeometryConverter::position_convert_to_geos_polygon(polygon, sketch_math::ring_normal(polygon));

    if(!geometry)
    {
        return false;
    }

    return geometry->isValid();
}

}// namespace geos_util

}// namespace sketch
