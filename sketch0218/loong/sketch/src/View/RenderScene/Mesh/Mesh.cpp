#include "sketch/View/RenderScene/Mesh/Mesh.h"

#include "Engine/Render/Device.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "Engine/Math/GeoUtil.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/View/RenderRes/NurbsData.h"

using namespace Engine;

namespace sketch
{
Mesh::Mesh(RenderScene* pScene, Define::ObjectType type) : RenderObject(pScene)
{
    m_type = type;

    m_ccw = true;
    m_render_dirty = true;
    m_matrixDirty = true;
    m_geoDirty = true;
}

Mesh::~Mesh()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    m_drawBatches.clear();
    m_drawEdgeBatches.clear();
}

void Mesh::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker)
{
    switch(method)
    {
    case Define::MSG_OBJ_UPDATE: {
        m_matrixDirty = true;
        m_geoDirty = true;
        m_render_dirty = true;
        m_pScene->updateObjInViews(this);
    }
    break;
    }
}

std::pair<nous::vec3, nous::vec3> Mesh::get_line_world_position(SketchLinePtr line)
{
    auto& mat = m_pDoc->world_matrix();
    auto renderable = m_pDoc->geometry();
    auto geo = std::dynamic_pointer_cast<GeoData>(renderable);
    return { NousConvert::product_v3(mat, geo->get_point_position(line->p0)), NousConvert::product_v3(mat, geo->get_point_position(line->p1)) };
}

void Mesh::get_polygon_topo_world_position(SketchPolygonPtr polygon, std::vector<std::vector<nous::vec3>>& topo_pos)
{
    auto& mat = m_pDoc->world_matrix();
    auto renderable = m_pDoc->geometry();
    auto geo = std::dynamic_pointer_cast<GeoData>(renderable);
    auto get_ring_position = [&geo, &mat](SketchRingPtr ring, std::vector<std::vector<nous::vec3>>& topo) {
        std::vector<SketchPointPtr> prim_topo;
        geo->get_element_points(ring, prim_topo);

        std::vector<nous::vec3>& posv = topo.emplace_back();
        for(SketchPointPtr point: prim_topo)
        {
            posv.emplace_back(std::move(NousConvert::product_v3(mat, geo->get_point_position(point))));
        }
    };

    get_ring_position(polygon->ring, topo_pos);
    for(auto inner_ring: polygon->inner_rings)
    {
        get_ring_position(inner_ring, topo_pos);
    }
}

void Mesh::get_ring_world_position(SketchRingPtr ring, std::vector<nous::vec3>& topo_pos)
{
    auto& mat = m_pDoc->world_matrix();
    auto renderable = m_pDoc->geometry();
    auto geo = std::dynamic_pointer_cast<GeoData>(renderable);
    std::vector<SketchPointPtr> prim_topo;
    geo->get_element_points(ring, prim_topo);
    for(SketchPointPtr point: prim_topo)
    {
        topo_pos.emplace_back(std::move(NousConvert::product_v3(mat, geo->get_point_position(point))));
    }
}

nous::vec3 Mesh::get_world_position_closest_with_ring_edge(SketchRingPtr ring, nous::vec3 pos)
{
    nous::vec3 closest_pos;
    auto& mat = m_pDoc->world_matrix();
    auto renderable = m_pDoc->geometry();
    auto geo = std::dynamic_pointer_cast<GeoData>(renderable);
    std::vector<SketchPointPtr> prim_topo;
    geo->get_element_points(ring, prim_topo);
    std::vector<nous::vec3> topo_pos;
    for(SketchPointPtr point: prim_topo)
    {
        topo_pos.emplace_back(std::move(NousConvert::product_v3(mat, geo->get_point_position(point))));
    }

    double min_dist = DBL_MAX;
    for(size_t i = 0; i < topo_pos.size(); ++i)
    {
        size_t j = (i + 1) % topo_pos.size();
        nous::vec3 v0 = topo_pos[i];
        nous::vec3 v1 = topo_pos[j];

        nous::vec3 vv = v1 - v0;
        nous::vec3 vp = pos - v0;

        float t = vp.dot(vv) / vv.dot(vv);

        nous::vec3 cp;
        if(t < 0.0f)
        {
            cp = v0;
        }
        else if(t > 1.0f)
        {
            cp = v1;
        }
        else
        {
            cp = v0 + vv * t;
        }

        double dist = (cp - pos).norm();
        if(dist < min_dist)
        {
            min_dist = dist;
            closest_pos = cp;
        }
    }

    return closest_pos;
}

bool Mesh::rectangle_intersect_view_ele(const nous::vec2& rect_min, const nous::vec2& rect_max, bool contain_only,
                                        std::unordered_set<SketchElementPtr>& result_elements)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return false;

    ViewCamera* pCam = view->camera();
    auto& matDVP = pCam->dvpMatrix();
    auto& matW = m_pDoc->world_matrix();
    Mat4f matWVP = matDVP * matW;

    bool isIntersect = false;

    auto&& is_boundary_contained_by_rectangle = [&](const std::vector<nous::pos_t>& bounding_line) -> bool {
        bool edge_all_contain = true;
        for(int i = 0; i < bounding_line.size() - 1; ++i)
        {
            Engine::Vec4f v0 = matWVP * NousConvert::convert(bounding_line[i]);
            Engine::Vec4f v1 = matWVP * NousConvert::convert(bounding_line[(i + 1) % bounding_line.size()]);

            if(v0.w <= 0.0f && v1.w <= 0.0f)
                continue;

            pCam->clipProjLine(v0, v1);
            if(!lineIntersectRect(v0.x, v0.y, v1.x, v1.y, rect_min.x(), rect_min.y(), rect_max.x(), rect_max.y()))
            {
                edge_all_contain = false;
                break;
            }
        }
        return edge_all_contain;
    };

    auto&& is_nurbs_surface_contained_by_rectangle = [&](std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> elements) -> bool {
        bool surface_all_contain = true;

        for(const auto& bounding_line: elements)
        {
            bool edge_all_contain = is_boundary_contained_by_rectangle(bounding_line.second);

            if(!edge_all_contain)
            {
                surface_all_contain = false;
            }
            else
            {
                result_elements.insert(bounding_line.first);
            }
        }

        return surface_all_contain;
    };

    auto&& is_nurbs_surface_touch_rectangle = [&](std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> elements) {
        bool surface_contain = false;
        for(const auto& bounding_line: elements)
        {
            bool edge_contian = false;
            for(int i = 0; i < bounding_line.second.size(); ++i)
            {
                Engine::Vec4f v0 = matWVP * NousConvert::convert(bounding_line.second[i]);
                Engine::Vec4f v1 = matWVP * NousConvert::convert(bounding_line.second[(i + 1) % bounding_line.second.size()]);

                if(v0.w <= 0.0f && v1.w <= 0.0f)
                    continue;

                pCam->clipProjLine(v0, v1);
                if(lineIntersectRect(v0.x, v0.y, v1.x, v1.y, rect_min.x(), rect_min.y(), rect_max.x(), rect_max.y()))
                {
                    edge_contian = true;
                    result_elements.insert(bounding_line.first);
                    isIntersect = true;
                    break;
                }
            }

            if(edge_contian)
            {
                surface_contain = true;
            }
        }

        return surface_contain;
    };

    Define::ObjectType obj_type = m_pDoc->type();
    if(obj_type == Define::ObjectType::OBJ_D5_MESH)
    {
        MeshDoc* obj_doc = static_cast<MeshDoc*>(m_pDoc);

        if(contain_only)
        {
            std::unordered_set<SketchLinePtr> intersect_lines;
            auto geo_data = obj_doc->geodata();
            for(auto& line: geo_data->get_all_lines())
            {
                nous::vec4 v0 = NousConvert::product_v4(matWVP, geo_data->get_point_position(line.second->p0));
                nous::vec4 v1 = NousConvert::product_v4(matWVP, geo_data->get_point_position(line.second->p1));

                if(v0.w() <= 0.0f && v1.w() <= 0.0f)
                    continue;

                if(v0.x() < rect_min.x() * v0.w() || v0.x() > rect_max.x() * v0.w() || v0.y() < rect_min.y() * v0.w() ||
                   v0.y() > rect_max.y() * v0.w())
                {
                    continue;
                }
                if(v1.x() < rect_min.x() * v1.w() || v1.x() > rect_max.x() * v1.w() || v1.y() < rect_min.y() * v1.w() ||
                   v1.y() > rect_max.y() * v1.w())
                {
                    continue;
                }

                result_elements.insert(line.second);
                intersect_lines.insert(line.second);
                isIntersect = true;
            }
            std::unordered_set<SketchPolygonPtr> result_polygons;
            GeoDataUtils::get().get_polygons_contained_by_all_lines(geo_data.get(), intersect_lines, result_polygons);
            result_elements.insert(result_polygons.begin(), result_polygons.end());
        }
        else
        {
            auto renderable = m_pDoc->geometry();
            auto geo_data = std::dynamic_pointer_cast<GeoData>(renderable);
            for(auto& line: geo_data->get_all_lines())
            {

                Engine::Vec4f v0 = matWVP * NousConvert::convert(geo_data->get_point_position(line.second->p0));
                Engine::Vec4f v1 = matWVP * NousConvert::convert(geo_data->get_point_position(line.second->p1));

                if(v0.w <= 0.0f && v1.w <= 0.0f)
                    continue;

                pCam->clipProjLine(v0, v1);
                if(lineIntersectRect(v0.x, v0.y, v1.x, v1.y, rect_min.x(), rect_min.y(), rect_max.x(), rect_max.y()))
                {
                    result_elements.insert(line.second);
                    auto adj_rings = geo_data->get_line_adj_rings(line.second);
                    for(auto ring: adj_rings)
                    {
                        SketchPolygonPtr skp = geo_data->get_polygon(ring);
                        if(skp)
                        {
                            result_elements.insert(skp);
                        }
                    }
                    isIntersect = true;
                }
            }
        }
    }
    else if(obj_type == Define::ObjectType::OBJ_D5_NURBS)
    {
        NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(m_pDoc);
        auto nurbs_data = nurbs_doc->nurbsdata();
        if(contain_only)
        {
            for(auto& data_pair: nurbs_data->get_all_nurbs())
            {
                const auto type = data_pair.second->type();
                if(type == EElementType::Curve)
                {
                    std::shared_ptr<NurbsCurve> curve = std::static_pointer_cast<NurbsCurve>(data_pair.second);
                    const auto&& sample_curve_points = curve->get_sampled_point_arr();

                    for(int i = 0; i < sample_curve_points.size() - 1; ++i)
                    {
                        nous::vec4 v0 = NousConvert::product_v4(matWVP, sample_curve_points[i]);
                        nous::vec4 v1 = NousConvert::product_v4(matWVP, sample_curve_points[i + 1]);

                        if(v0.w() <= 0.0f && v1.w() <= 0.0f)
                            continue;

                        if(v0.x() < rect_min.x() * v0.w() || v0.x() > rect_max.x() * v0.w() || v0.y() < rect_min.y() * v0.w() ||
                           v0.y() > rect_max.y() * v0.w())
                        {
                            continue;
                        }
                        if(v1.x() < rect_min.x() * v1.w() || v1.x() > rect_max.x() * v1.w() || v1.y() < rect_min.y() * v1.w() ||
                           v1.y() > rect_max.y() * v1.w())
                        {
                            continue;
                        }

                        result_elements.insert(data_pair.second);
                        isIntersect = true;
                        break;
                    }
                }
                else
                {
                    std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> boundary_elements;
                    if(type == EElementType::AlgebraSurface)
                    {
                        std::shared_ptr<NurbsAlgebraSurface> surface = std::static_pointer_cast<NurbsAlgebraSurface>(data_pair.second);
                        boundary_elements = surface->get_surface_elements();
                    }
                    else if(type == EElementType::ClosedSurface)
                    {
                        std::shared_ptr<NurbsClosedSurface> surface = std::static_pointer_cast<NurbsClosedSurface>(data_pair.second);
                        boundary_elements = surface->get_surface_elements();
                    }
                    else if(type == EElementType::TrimmedSurface)
                    {
                        std::shared_ptr<NurbsTrimmedSurface> surface = std::static_pointer_cast<NurbsTrimmedSurface>(data_pair.second);
                        boundary_elements = surface->get_complex_surface_elements();
                    }

                    const bool surface_contained = is_nurbs_surface_contained_by_rectangle(boundary_elements);
                    if(surface_contained)
                    {
                        result_elements.insert(data_pair.second);
                        isIntersect = true;
                    }
                }
            }
        }
        else
        {
            for(auto& data_pair: nurbs_data->get_all_nurbs())
            {
                const auto type = data_pair.second->type();
                if(type == EElementType::Curve)
                {
                    std::shared_ptr<NurbsCurve> curve = std::static_pointer_cast<NurbsCurve>(data_pair.second);
                    const auto&& sample_curve_points = curve->get_sampled_point_arr();

                    for(int i = 0; i < sample_curve_points.size() - 1; ++i)
                    {
                        Engine::Vec4f v0 = matWVP * NousConvert::convert(sample_curve_points[i]);
                        Engine::Vec4f v1 = matWVP * NousConvert::convert(sample_curve_points[i + 1]);

                        if(v0.w <= 0.0f && v1.w <= 0.0f)
                            continue;

                        pCam->clipProjLine(v0, v1);
                        if(lineIntersectRect(v0.x, v0.y, v1.x, v1.y, rect_min.x(), rect_min.y(), rect_max.x(), rect_max.y()))
                        {
                            result_elements.insert(data_pair.second);
                            isIntersect = true;
                            break;
                        }
                    }
                }
                else
                {
                    std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> boundary_elements;
                    if(type == EElementType::ClosedSurface)
                    {
                        std::shared_ptr<NurbsAlgebraSurface> surface = std::static_pointer_cast<NurbsAlgebraSurface>(data_pair.second);
                        boundary_elements = surface->get_surface_elements();
                    }
                    else if(type == EElementType::ClosedSurface)
                    {
                        std::shared_ptr<NurbsClosedSurface> surface = std::static_pointer_cast<NurbsClosedSurface>(data_pair.second);
                        boundary_elements = surface->get_surface_elements();
                    }
                    else if(type == EElementType::TrimmedSurface)
                    {
                        std::shared_ptr<NurbsTrimmedSurface> surface = std::static_pointer_cast<NurbsTrimmedSurface>(data_pair.second);
                        boundary_elements = surface->get_complex_surface_elements();
                    }

                    const bool surface_touched = is_nurbs_surface_touch_rectangle(boundary_elements);
                    if(surface_touched)
                    {
                        result_elements.insert(data_pair.second);
                        isIntersect = true;
                    }
                }
            }
        }
    }
    return isIntersect;
}

inline bool Mesh::ray_intersect_face(const nous::vec3& o, const nous::vec3& d, nous::vec3& intersect_pos, nous::vec3& intersect_normal,
                                     SketchElementPtr* closest)
{
    double min_distance = DBL_MAX;
    bool is_pick_face = false;

    Define::ObjectType obj_type = m_pDoc->type();

    if(m_pDoc->type() == Define::OBJ_D5_MESH)
    {
        auto renderable = m_pDoc->geometry();
        auto geo = std::dynamic_pointer_cast<GeoData>(renderable);

        Vec3 tp;
        size_t primId;

        for(auto& ring_relation: geo->get_ring_relation_polygons())
        {
            SketchPolygonPtr polygon = ring_relation.second.polygon;
            if(polygon)
            {
                for(auto& tri: polygon->triangles)
                {
                    double distance = 0.0;
                    Engine::Vec3 pos = Engine::Vec3();
                    if(rayIntersectTri(NousConvert::convert(o), NousConvert::convert(d),
                                       Engine::Vec3(tri[0].second.x(), tri[0].second.y(), tri[0].second.z()),
                                       Engine::Vec3(tri[1].second.x(), tri[1].second.y(), tri[1].second.z()),
                                       Engine::Vec3(tri[2].second.x(), tri[2].second.y(), tri[2].second.z()), &distance, &pos))
                    {
                        if(distance < min_distance)
                        {
                            min_distance = distance;
                            intersect_pos = NousConvert::convert(pos);
                            intersect_normal = polygon->normal;
                            *closest = polygon;
                        }
                        break;
                    }
                }
            }
        }
        if(min_distance < DBL_MAX)
        {
            is_pick_face = true;
        }
    }
    else if(m_pDoc->type() == Define::OBJ_D5_NURBS)
    {
        auto renderable = m_pDoc->geometry();
        auto nurbs_data = std::dynamic_pointer_cast<NurbsData>(renderable);

        for(const auto& data_pair: nurbs_data->get_all_nurbs())
        {
            if(data_pair.second->type() == EElementType::AlgebraSurface)
            {
                std::shared_ptr<NurbsAlgebraSurface> surface = std::dynamic_pointer_cast<NurbsAlgebraSurface>(data_pair.second);
                std::vector<nous::pos_t> positions;
                std::vector<nous::vec3i> triangles;
                surface->get_sampled_triangles_arr(positions, triangles);

                for(int i = 0; i < triangles.size(); ++i)
                {
                    const nous::pos_t tp0 = positions[triangles[i].x()];
                    const nous::pos_t tp1 = positions[triangles[i].y()];
                    const nous::pos_t tp2 = positions[triangles[i].z()];
                    double distance = 0.0;
                    Engine::Vec3 pos = Engine::Vec3();
                    if(rayIntersectTri(NousConvert::convert(o), NousConvert::convert(d), NousConvert::convert(tp0), NousConvert::convert(tp1),
                                       NousConvert::convert(tp2), &distance, &pos))
                    {
                        if(distance < min_distance)
                        {
                            min_distance = distance;
                            intersect_pos = NousConvert::convert(pos);
                            intersect_normal = sketch_math::ring_normal({ tp0, tp1, tp2 });
                            *closest = data_pair.second;
                        }
                        break;
                    }
                }
            }

            if(data_pair.second->type() == EElementType::ClosedSurface)
            {
                NurbsClosedSurfacePtr closed_surface = std::dynamic_pointer_cast<NurbsClosedSurface>(data_pair.second);
                const auto& triangles = closed_surface->get_sampled_triangles();
                for(int i = 0; i < triangles.size(); ++i)
                {
                    const nous::pos_t tp0 = triangles[i][0];
                    const nous::pos_t tp1 = triangles[i][1];
                    const nous::pos_t tp2 = triangles[i][2];
                    double distance = 0.0;
                    Engine::Vec3 pos = Engine::Vec3();
                    if(rayIntersectTri(NousConvert::convert(o), NousConvert::convert(d), NousConvert::convert(tp0), NousConvert::convert(tp1),
                                       NousConvert::convert(tp2), &distance, &pos))
                    {
                        if(distance < min_distance)
                        {
                            min_distance = distance;
                            intersect_pos = NousConvert::convert(pos);
                            intersect_normal = -sketch_math::ring_normal({ tp0, tp1, tp2 });
                            *closest = data_pair.second;
                        }
                        break;
                    }
                }
            }

            if(data_pair.second->type() == EElementType::TrimmedSurface)
            {
                NurbsTrimmedSurfacePtr trimmed_surface = std::dynamic_pointer_cast<NurbsTrimmedSurface>(data_pair.second);
                std::vector<nous::pos_t> sampled_points_arr;
                std::vector<nous::vec3i> sampled_triangles_arr;
                trimmed_surface->get_surface_sampled_triangles(sampled_points_arr, sampled_triangles_arr);

                for(int i = 0; i < sampled_triangles_arr.size(); ++i)
                {
                    const nous::pos_t tp0 = sampled_points_arr[sampled_triangles_arr[i].x()];
                    const nous::pos_t tp1 = sampled_points_arr[sampled_triangles_arr[i].y()];
                    const nous::pos_t tp2 = sampled_points_arr[sampled_triangles_arr[i].z()];
                    double distance = 0.0;
                    Engine::Vec3 pos = Engine::Vec3();
                    if(rayIntersectTri(NousConvert::convert(o), NousConvert::convert(d), NousConvert::convert(tp0), NousConvert::convert(tp1),
                                       NousConvert::convert(tp2), &distance, &pos))
                    {
                        if(distance < min_distance)
                        {
                            min_distance = distance;
                            intersect_pos = NousConvert::convert(pos);
                            intersect_normal = sketch_math::ring_normal({ tp0, tp1, tp2 });
                            *closest = data_pair.second;
                        }
                        break;
                    }
                }
            }
        }

        if(min_distance < DBL_MAX)
        {
            is_pick_face = true;
        }
    }

    return is_pick_face;
}

double Mesh::get_picked_pos_distance_from_view_line(const nous::vec3& o, const nous::vec3& d, const nous::vec2& view_pick,
                                                    const nous::vec2& view_size, nous::mat4& mat, nous::vec3 v0, nous::vec3 v1, nous::vec3& pos)
{
    float min_dist = FLT_MAX;
    double t0, t1;
    if(!rayIntersect(NousConvert::convert(o), NousConvert::convert(d), NousConvert::convert(v0), NousConvert::convert(nous::vec3(v1 - v0)), t0, t1) ||
       t0 < 0.0 || t1 < 0.0 || t1 > 1.0)
        return min_dist;

    pos = v0 + (v1 - v0) * t1;
    nous::vec4 vpos = NousConvert::product_v4(mat, pos);
    if(vpos.w() <= 0.0f)// invisible
        return min_dist;

    vpos.head<3>() /= vpos.w();
    nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
    npos.x() *= view_size.x();
    npos.y() *= view_size.y();
    return (npos - view_pick).squaredNorm();
}

inline float Mesh::ray_intersect_view_line(const nous::vec3& o, const nous::vec3& d, const nous::vec2& view_pick, const nous::vec2& view_size,
                                           const nous::mat4& vp_mat, nous::vec3& intersect_pos, nous::vec3& intersect_tangent,
                                           SketchElementPtr* closest)
{
    Define::ObjectType obj_type = m_pDoc->type();
    float min_dist = FLT_MAX;

    if(obj_type == Define::ObjectType::OBJ_D5_MESH)
    {
        auto renderable = m_pDoc->geometry();
        auto pGeo = std::dynamic_pointer_cast<GeoData>(renderable);
        nous::mat4 mat_wvp = vp_mat * NousConvert::convert(m_pDoc->world_matrix());

        for(auto& line: pGeo->get_all_lines())
        {
            nous::vec3 v0 = pGeo->get_point_position(line.second->p0);
            nous::vec3 v1 = pGeo->get_point_position(line.second->p1);

            nous::vec3 pos;
            float distance2 = get_picked_pos_distance_from_view_line(o, d, view_pick, view_size, mat_wvp, v0, v1, pos);

            if(distance2 < min_dist)
            {
                min_dist = distance2;
                intersect_pos = pos;
                intersect_tangent = (v1 - v0).normalized();
                *closest = line.second;
            }
        }
    }
    else if(obj_type == Define::ObjectType::OBJ_D5_NURBS)
    {
        auto renderable = m_pDoc->geometry();
        auto nurbs_data = std::dynamic_pointer_cast<NurbsData>(renderable);

        nous::mat4 mat_wvp = vp_mat * NousConvert::convert(m_pDoc->world_matrix());

        const auto judge_func = [&](const std::vector<nous::pos_t>& sample_curve_points, SketchElementPtr nurbs_item) -> void {
            for(int i = 0; i < sample_curve_points.size() - 1; ++i)
            {
                nous::vec3 v0 = sample_curve_points[i];
                nous::vec3 v1 = sample_curve_points[i + 1];

                nous::vec3 pos;
                float distance2 = get_picked_pos_distance_from_view_line(o, d, view_pick, view_size, mat_wvp, v0, v1, pos);

                if(distance2 < min_dist)
                {
                    min_dist = distance2;
                    intersect_pos = pos;
                    intersect_tangent = (v1 - v0).normalized();
                    *closest = nurbs_item;
                }
            }
        };

        for(auto& data_pair: nurbs_data->get_all_nurbs())
        {
            if(data_pair.second->type() == EElementType::Curve)
            {
                std::shared_ptr<NurbsCurve> curve = data_pair.second->to_nurbs_curve();
                const auto sample_curve_points = curve->get_sampled_point_arr();
                judge_func(sample_curve_points, data_pair.second);
            }
            else
            {
                std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> surface_boundary_elements;
                if(data_pair.second->type() == EElementType::AlgebraSurface)
                {
                    surface_boundary_elements = data_pair.second->to_nurbs_algebra_surface()->get_surface_elements();
                }
                else if(data_pair.second->type() == EElementType::ClosedSurface)
                {
                    surface_boundary_elements = data_pair.second->to_nurbs_closed_surface()->get_surface_elements();
                }
                else if(data_pair.second->type() == EElementType::TrimmedSurface)
                {
                    surface_boundary_elements = data_pair.second->to_nurbs_trimmed_surface()->get_complex_surface_elements();
                }

                bool picked_surface_boundary_element = false;
                if(!surface_boundary_elements.empty())
                {
                    for(auto& boundary_element: surface_boundary_elements)
                    {
                        judge_func(boundary_element.second, boundary_element.first);
                    }
                }
            }
        }
    }
    return min_dist;
}

inline float Mesh::pixel_distance_to_view_point(const nous::vec2& view_pick, const nous::vec2& view_size, const nous::mat4& vp_mat,
                                                nous::vec3& intersect_pos, SketchPointPtr* closest)
{
    float min_dist = FLT_MAX;
    if(m_pDoc->type() == Define::OBJ_D5_MESH)
    {
        auto renderable = m_pDoc->geometry();
        auto geo = std::dynamic_pointer_cast<GeoData>(renderable);

        nous::mat4 matWVP = vp_mat * NousConvert::convert(m_pDoc->world_matrix());

        size_t pnum = geo->get_point_number();
        for(int i = 0; i < pnum; ++i)
        {
            SketchPointPtr np = geo->get_point(i);
            nous::vec3 pos = geo->get_point_position(np);
            nous::vec4 vpos = NousConvert::product_v4(matWVP, pos);
            if(vpos.w() < 0.0f)// invisible
                continue;

            vpos.head<3>() /= vpos.w();
            nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
            npos.x() *= view_size.x();
            npos.y() *= view_size.y();
            float distance2 = (npos - view_pick).squaredNorm();

            if(distance2 < min_dist)
            {
                min_dist = distance2;
                intersect_pos = pos;
                *closest = np;
            }
        }
    }
    return min_dist;
}

float Mesh::position_distance_to_view_line_center(const nous::vec3& pos, SketchLinePtr line, const nous::vec2& view_size, const nous::mat4& vp_mat,
                                                  nous::vec3& intersect_pos)
{
    float distance = 0.0f;
    if(m_pDoc->type() == Define::OBJ_D5_MESH)
    {
        auto renderable = m_pDoc->geometry();
        auto pGeo = std::dynamic_pointer_cast<GeoData>(renderable);

        nous::mat4 matWVP = vp_mat * NousConvert::convert(m_pDoc->world_matrix());

        nous::vec3 v0 = pGeo->get_point_position(line->p0);
        nous::vec3 v1 = pGeo->get_point_position(line->p1);
        intersect_pos = (v0 + v1) * 0.5;

        nous::vec4 vpos = NousConvert::product_v4(vp_mat, pos);
        nous::vec4 vcpos = NousConvert::product_v4(matWVP, intersect_pos);
        if(vpos.w() < 0.0 || vcpos.w() < 0.0)// invisible
            return DBL_MAX;

        vpos.head<3>() /= vpos.w();
        nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
        npos.x() *= view_size.x();
        npos.y() *= view_size.y();

        vcpos.head<3>() /= vcpos.w();
        nous::vec2 ncpos = (vcpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
        ncpos.x() *= view_size.x();
        ncpos.y() *= view_size.y();

        distance = (npos - ncpos).squaredNorm();
    }
    return distance;
}

void Mesh::_onUpdateGeometry()
{
    // MeshDoc* pObjDoc = (MeshDoc*)m_pDoc;
    // auto geo = pObjDoc->geometry();
    // geo->update_render_mesh();
}

void Mesh::_onUpdateTransform()
{
    if(m_pDoc == nullptr)
        return;

    DocManager* pDocMgr = STCore::inst->docMgr();
    Document* pDoc = pDocMgr->document();

    auto& matW = m_pDoc->world_matrix();
    m_ccw = matW.isRHS();

    this->setMatrix(pDoc->d2vMatrix() * matW);
}

void Mesh::prepareDrawBatch(std::vector<DrawBatch*>& opaqueBatches, std::vector<DrawBatch*>& transBatches, View* pView)
{
    N_UNUSED(opaqueBatches);
    N_UNUSED(transBatches);

    prepare_render(pView);
}

void Mesh::prepare_render(View* view)
{
    RenderObject::prepare_render(view);

    auto renderable = m_pDoc->geometry();
    if(!renderable)
        return;
    bool mesh_change = renderable->update_render_mesh();
    if(renderable->get_render_position().size() == 0)
        return;

    auto mesh_data = renderable->render_mesh();
    auto& select_info = view->selector()->get_select_info();
    auto& find_pos = select_info.find(this);

    TFObjectDoc* working_group = view->working_group_area();
    bool in_woring_group = false;
    if(working_group)
    {
        in_woring_group = view->obj_in_working_group(m_pDoc);
    }
    // material
    bool material_change = false;
    for(auto& submesh: mesh_data->submeshes)
    {
        if(submesh.sketch_element.expired())
        {
            continue;
        }
        SketchElementPtr ele = submesh.sketch_element.lock();
        int last_material_id = submesh.material_id;
        submesh.material_id = 0;
        if(ele->type() == EElementType::SmoothGroup && submesh.topology == SubmeshData::Topology::Lines)
        {
            // 平滑组中的柔化线，置灰色
            submesh.material_id = 2;
        }
        else if(working_group && !in_woring_group)
        {
            // 在有效工作空间外的对象，置灰色
            submesh.material_id = 2;
        }
        else
        {
            if(find_pos != select_info.end())
            {
                // 选中
                if(find_pos->second.elements.find(ele) != find_pos->second.elements.end())
                {
                    submesh.material_id = 1;
                }
            }
        }
        material_change |= (last_material_id != submesh.material_id);
    }
    auto& mat = this->matrix();
    mesh_data->transform = {
        mat.col[0].x, mat.col[0].y, mat.col[0].z, 0.f, mat.col[1].x, mat.col[1].y, mat.col[1].z, 0.f,
        mat.col[2].x, mat.col[2].y, mat.col[2].z, 0.f, mat.col[3].x, mat.col[3].y, mat.col[3].z, 1.f,
    };

    RenderMesh render_mesh;
    render_mesh.id = this->id();
    if(mesh_change)
    {
        render_mesh.dirty_flag |= DirtyFlag::DirtyFlag_Mesh;
    }
    if(material_change)
    {
        render_mesh.dirty_flag |= DirtyFlag::DirtyFlag_Material;
    }
    render_mesh.data = mesh_data;
    view->prepare_mesh(render_mesh);
    m_render_dirty = false;
}

}// namespace sketch
