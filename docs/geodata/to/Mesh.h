#pragma once

#include "../RenderObject.h"
#include "nous_base/nous_base.h"

namespace Engine
{
class DrawBatch;
};// namespace Engine

namespace sketch
{
class ViewCamera;
class GeoData;
class SketchElement;
class SketchPoint;
class SketchLine;
class SketchRing;
class SketchPolygon;
class Mesh : public RenderObject
{
public:
    Mesh(RenderScene* pScene, Define::ObjectType type);
    virtual ~Mesh();

    static RenderObject* newInstance(RenderScene* pScene, Define::ObjectType type) { return new Mesh(pScene, type); }

    virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override;

    virtual std::pair<nous::vec3, nous::vec3> get_line_world_position(SketchLinePtr line);
    virtual void get_polygon_topo_world_position(SketchPolygonPtr polygon, std::vector<std::vector<nous::vec3>>& topo_pos);
    virtual void get_ring_world_position(SketchRingPtr ring, std::vector<nous::vec3>& topo_pos);
    virtual nous::vec3 get_world_position_closest_with_ring_edge(SketchRingPtr ring, nous::vec3 pos);

    virtual bool rectangle_intersect_view_ele(const nous::vec2& rect_min, const nous::vec2& rect_max, bool contain_only,
                                              std::unordered_set<SketchElementPtr>& result_elements);

    virtual bool ray_intersect_face(const nous::vec3& o, const nous::vec3& d, nous::vec3& intersect_pos, nous::vec3& intersect_normal,
                                    SketchElementPtr* closest);

    virtual float ray_intersect_view_line(const nous::vec3& o, const nous::vec3& d, const nous::vec2& view_pick, const nous::vec2& view_size,
                                          const nous::mat4& vp_mat, nous::vec3& intersect_pos, nous::vec3& intersect_tangent,
                                          SketchElementPtr* closest);

    virtual float pixel_distance_to_view_point(const nous::vec2& view_pick, const nous::vec2& view_size, const nous::mat4& vp_mat,
                                               nous::vec3& intersect_pos, SketchPointPtr* closest);

    virtual float position_distance_to_view_line_center(const nous::vec3& pos, SketchLinePtr line, const nous::vec2& view_size,
                                                        const nous::mat4& vp_mat, nous::vec3& intersect_pos);

    virtual void prepareDrawBatch(std::vector<Engine::DrawBatch*>& opaqueBatches, std::vector<Engine::DrawBatch*>& transBatches,
                                  View* pView) override;

    void prepare_render(View* view) override;

    double get_picked_pos_distance_from_view_line(const nous::vec3& o, const nous::vec3& d, const nous::vec2& view_pick, const nous::vec2& view_size,
                                                  nous::mat4& mat, nous::vec3 v0, nous::vec3 v1, nous::vec3& pos);

protected:
    virtual void _onUpdateGeometry() override;
    virtual void _onUpdateTransform() override;

protected:
    bool m_ccw;
    bool m_render_dirty;

    std::unordered_set<std::shared_ptr<Engine::DrawBatchBlinnPhong>> m_drawBatches;
    std::unordered_set<std::shared_ptr<Engine::DrawBatchBlinnPhong>> m_drawEdgeBatches;
};
}// namespace sketch