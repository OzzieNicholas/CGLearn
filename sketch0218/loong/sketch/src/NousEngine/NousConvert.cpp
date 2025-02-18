#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"

namespace sketch
{

geos::geom::Geometry* NousGeometryConverter::position_convert_to_geos_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_positions)
{
    if(polygon_positions.empty() || polygon_positions.front().size() < 3)
    {
        return nullptr;
    }

    geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    geos::geom::CoordinateSequence exring_coordinates;
    for(auto point_origin: polygon_positions.front())
    {
        geos::geom::Coordinate cd = geos::geom::Coordinate(point_origin.x(), point_origin.y(), 0.0);
        exring_coordinates.add(cd);
    }
    exring_coordinates.add(exring_coordinates.front());
    std::unique_ptr<geos::geom::LinearRing> exring = geos_factory->createLinearRing(exring_coordinates);

    std::vector<geos::geom::LinearRing*> polygon_geom_inrings;
    for(int i = 1; i < polygon_positions.size(); ++i)
    {
        geos::geom::CoordinateSequence inring_coordinates;
        for(auto point_origin: polygon_positions[i])
        {
            geos::geom::Coordinate cd = geos::geom::Coordinate(point_origin.x(), point_origin.y(), 0.0);
            inring_coordinates.add(cd);
        }
        inring_coordinates.add(inring_coordinates.front());
        geos::geom::LinearRing* inring = geos_factory->createLinearRing(inring_coordinates).release();
        polygon_geom_inrings.emplace_back(inring);
    }

    geos::geom::Polygon* geom_polygon = geos_factory->createPolygon(*exring.get(), polygon_geom_inrings);

    if(!geom_polygon)
    {
        return nullptr;
    }

    if(!geom_polygon->isValid())
    {
        return nullptr;
    }

    return geom_polygon;
}

std::vector<std::vector<nous::pos_t>> NousGeometryConverter::geos_polygon_convert_to_position(geos::geom::Polygon* geos_polygon)
{
    std::vector<std::vector<nous::pos_t>> polygon_positions;
    if(!geos_polygon)
    {
        return polygon_positions;
    }
    if(!geos_polygon->isValid())
    {
        return polygon_positions;
    }

    const geos::geom::LinearRing* child_exring = geos_polygon->getExteriorRing();
    int exring_pt_number = child_exring->getNumPoints();
    if(exring_pt_number < 4)// 为什么valid的polygon会有无效的外环？
    {
        return polygon_positions;
    }
    auto& exring_pt_arr = polygon_positions.emplace_back();
    for(int i = 0; i < exring_pt_number - 1; ++i)
    {
        geos::geom::Coordinate coord = child_exring->getCoordinateN(i);
        exring_pt_arr.emplace_back(coord.x, coord.y, coord.z);
    }

    for(int i = 0; i < geos_polygon->getNumInteriorRing(); ++i)
    {
        const geos::geom::LinearRing* child_inring = geos_polygon->getInteriorRingN(i);
        auto& inring_pt_arr = polygon_positions.emplace_back();
        for(int j = 0; j < child_inring->getNumPoints() - 1; ++j)
        {
            geos::geom::Coordinate coord = child_inring->getCoordinateN(j);
            inring_pt_arr.emplace_back(coord.x, coord.y, coord.z);
        }
    }

    return polygon_positions;
}

geos::geom::Geometry* NousGeometryConverter::position_convert_to_geos_polygon(const std::vector<nous::pos_t>& polygon_positions)
{
    if(polygon_positions.size() < 3)
    {
        return nullptr;
    }
    geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    geos::geom::CoordinateSequence exring_coordinates;
    for(auto point_origin: polygon_positions)
    {
        geos::geom::Coordinate cd = geos::geom::Coordinate(point_origin.x(), point_origin.y(), 0.0);
        exring_coordinates.add(cd);
    }
    exring_coordinates.add(exring_coordinates.front());
    std::unique_ptr<geos::geom::LinearRing> exring = geos_factory->createLinearRing(exring_coordinates);

    geos::geom::Polygon* geom_polygon = geos_factory->createPolygon(std::move(exring)).release();

    if(!geom_polygon)
    {
        return nullptr;
    }

    if(!geom_polygon->isValid())
    {
        return nullptr;
    }

    return geom_polygon;
}

geos::geom::Geometry* NousGeometryConverter::position_convert_to_geos_polygon(const std::vector<std::vector<nous::pos_t>>& polygon_positions,
                                                                              const nous::vec3& normal)
{
    std::vector<std::vector<nous::pos_t>> projected_polygon_positions;

    nous::vec3 act_normal = normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    for(int i = 0; i < polygon_positions.size(); ++i)
    {
        std::vector<nous::pos_t>& projected_ring_positions = projected_polygon_positions.emplace_back();
        for(int j = 0; j < polygon_positions[i].size(); ++j)
        {
            nous::pos_t projected_position = rotate_to_xy_plane * polygon_positions[i][j];
            projected_ring_positions.emplace_back(projected_position);
        }
    }

    return NousGeometryConverter::position_convert_to_geos_polygon(projected_polygon_positions);
}

geos::geom::Geometry* NousGeometryConverter::position_convert_to_geos_polygon(const std::vector<nous::pos_t>& polygon_positions,
                                                                              const nous::vec3& normal)
{
    std::vector<nous::pos_t> projected_polygon_positions;

    nous::vec3 act_normal = normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    for(int i = 0; i < polygon_positions.size(); ++i)
    {
        nous::pos_t projected_position = rotate_to_xy_plane * polygon_positions[i];
        projected_polygon_positions.emplace_back(projected_position);
    }

    return NousGeometryConverter::position_convert_to_geos_polygon(projected_polygon_positions);
}

std::vector<std::vector<nous::pos_t>> NousGeometryConvertergeos_polygon_convert_to_position(geos::geom::Polygon* geos_polygon,
                                                                                            const nous::vec3& normal)
{
    std::vector<std::vector<nous::pos_t>> polygon_positions;
    if(!geos_polygon)
    {
        return polygon_positions;
    }

    nous::vec3 act_normal = normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1);
    }
    nous::quat rotate_from_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1)).conjugate();

    const geos::geom::LinearRing* child_exring = geos_polygon->getExteriorRing();
    auto& exring_pt_arr = polygon_positions.emplace_back();
    int exring_pt_number = child_exring->getNumPoints();
    for(int i = 0; i < exring_pt_number - 1; ++i)
    {
        geos::geom::Coordinate coord = child_exring->getCoordinateN(i);
        exring_pt_arr.emplace_back(rotate_from_xy_plane * nous::pos_t(coord.x, coord.y, coord.z));
    }

    for(int i = 0; i < geos_polygon->getNumInteriorRing(); ++i)
    {
        const geos::geom::LinearRing* child_inring = geos_polygon->getInteriorRingN(i);
        auto& inring_pt_arr = polygon_positions.emplace_back();
        for(int j = 0; j < child_inring->getNumPoints() - 1; ++j)
        {
            geos::geom::Coordinate coord = child_inring->getCoordinateN(j);
            inring_pt_arr.emplace_back(rotate_from_xy_plane * nous::pos_t(coord.x, coord.y, coord.z));
        }
    }

    return polygon_positions;
}

}// namespace sketch
