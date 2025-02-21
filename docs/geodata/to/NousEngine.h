#pragma once

#include "geos/geom/GeometryFactory.h"
#include "nous/graph/graph.h"
#include "nous_mesh/mesh.h"

namespace nous
{
class Graph;
class Point;
}// namespace nous
namespace geos
{
namespace geom
{
class Geometry;
class PrecisionModel;
}// namespace geom
}// namespace geos
namespace sketch
{
class GeoData;
static geos::geom::PrecisionModel* sketch_geos_precision_model;

namespace geos_util
{
std::vector<geos::geom::Geometry*> extract_valid_polygons(geos::geom::Geometry* geom);

std::vector<geos::geom::Geometry*> difference_polygon(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1);

std::vector<geos::geom::Geometry*> intersect_polygon(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1);

bool is_contain(geos::geom::Geometry* geom0, geos::geom::Geometry* geom1);

bool buffer_ring(const std::vector<nous::vec3>& ring, const nous::vec3& normal, float offset,
                 std::vector<std::vector<std::vector<nous::vec3>>>& result);

bool union_all_rings(const std::vector<std::vector<nous::vec3>>& rings, const nous::vec3& normal,
                     std::vector<std::vector<std::vector<nous::vec3>>>& result);

bool ring_difference_other_ring(const std::vector<nous::vec3>& ring, const std::vector<std::vector<nous::vec3>>& others, const nous::vec3& normal,
                                std::vector<std::vector<std::vector<nous::vec3>>>& result);

bool is_polygon_valid(const std::vector<nous::pos_t>& polygon);

bool is_polygon_valid(const std::vector<std::vector<nous::pos_t>>& polygon);

}// namespace geos_util

class NousEngine
{
protected:
    NousEngine();
    ~NousEngine();

public:
    enum PointRelativeRing
    {
        PR_ON,
        PR_IN,
        PR_OUT
    };
    static NousEngine* instance();

    void init();
    void release();

    static bool is_inited();

    nous::Graph* getDefaultGraph();
    nous::Node* getNormalNode();
    nous::Node* getCacheNode();

private:
    geos::geom::GeometryFactory::Ptr m_geosFactory;

    nous::unique_ptr<nous::Graph> m_graph;
    nous::Node* m_normalNode;
    nous::Node* m_cacheNode;

    static bool m_inited;
};
}// namespace sketch
