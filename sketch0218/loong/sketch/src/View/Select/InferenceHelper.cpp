#include "sketch/View/Select/InferenceHelper.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"

#include "Engine/Math/GeoUtil.h"
#include "nous_mesh/algorithm/intersect.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/SketchMath.h"

namespace sketch
{
bool InferResult::is_nurbs() const
{
    return (infer_type & InferType::ELEMENT_TYPE) && element && (uint32_t)element->type() >= 5 && (uint32_t)element->type() <= 8;
}
bool InferResult::is_point() const
{
    return (element && ((uint32_t)element->type()) == 1) || infer_type == InferType::LINE_CENTER || infer_type & InferType::POINT;
}

bool InferResult::is_world_axis() const
{
    float epsilon = sketch_math::sketch_epsilon;
    return (infer_type & InferType::AXIS_X && abs(position.y()) < epsilon && abs(position.z()) < epsilon) ||
           (infer_type & InferType::AXIS_Y && abs(position.x()) < epsilon && abs(position.z()) < epsilon) ||
           (infer_type & InferType::AXIS_Z && abs(position.x()) < epsilon && abs(position.y()) < epsilon);
}

InferenceHelper::InferenceHelper(ViewCamera* cam)
    : m_camera(cam), m_infer_pixel_range_pow2(25 * 25), m_infer_filter_type(InferType::ALL_TYPE), m_enalble_force_reference(false)
{
}

InferenceHelper::~InferenceHelper() {}

void InferenceHelper::update_inference(View* view, int x, int y, nous::vec3* extra_refer_pos, InferType extra_type)
{
    if(!view)
    {
        return;
    }
    _gen_reference_element(m_infer_result, false);

    Document* doc = STCore::inst->docMgr()->document();
    auto& vp = m_camera->viewport();
    nous::mat4 cam_vp_mat = NousConvert::convert(m_camera->dvpMatrix());
    Engine::Vec3 cam_pos, cam_dir;
    m_camera->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &cam_pos, &cam_dir);

    auto& objs = view->visObjects();
    std::vector<Mesh*> ray_cast_objs;
    std::vector<std::pair<nous::vec3, nous::vec3>> ray_cast_objs_local_ray;

    for(auto& obj: objs)
    {
        if(m_infer_ignore_obj_list.find(obj) != m_infer_ignore_obj_list.end() || obj->doc()->_group())
        {
            continue;
        }
        // check bbox first
        Engine::OBB obj_box = obj->doc()->bbox();
        obj_box.setMatrix(obj->doc()->world_matrix());
        Engine::Vec3 bbox_size = obj_box.sizeW();
        if(!obj_box.intersectRay(cam_pos, cam_dir, Engine::Vec3((bbox_size.x + bbox_size.y + bbox_size.z) * 0.2)))
            continue;
        auto& mat_inv = obj->doc()->world_matrix_inv();
        Engine::Vec3 p_local = mat_inv * cam_pos;
        Engine::Vec3 d_local = (mat_inv * cam_dir - mat_inv.pos).normalize();

        Mesh* mesh = static_cast<Mesh*>(obj);
        ray_cast_objs.push_back(mesh);
        ray_cast_objs_local_ray.emplace_back(NousConvert::convert(p_local), NousConvert::convert(d_local));
    }

    InferResult current_infer;

    bool select_world_center = false;
    Mesh* select_virtual_mesh = nullptr;
    SketchElementPtr select_virtual_ele = nullptr;
    nous::vec3 select_virtual_pos = nous::vec3::Zero();

    double closest_pixel_dist_pow2 = DBL_MAX;
    auto valid_infer = [&view](double closest_pixel_dist, Mesh* select_mesh, double select_distance) -> bool {
        bool replace = false;
        double d = closest_pixel_dist - select_distance;
        if(d > -sketch_math::sketch_epsilon)
        {
            if(select_mesh && abs(d) < sketch_math::sketch_epsilon)
            {
                // 共面/线/点的情况下，优先选择在工作区的对象
                if(view->get_obj_in_working_group_area_layer(select_mesh->doc()) == 0)
                {
                    replace = true;
                }
            }
            else
            {
                replace = true;
            }
        }
        return replace;
    };

    if(m_infer_filter_type & InferType::POINT)
    {
        // attach
        if(m_refer_triangle_points)
        {
            for(int i = 0; i < m_reference_triangle.size();)
            {
                nous::vec3 pos;
                pos.x() = m_reference_triangle[i];
                pos.y() = m_reference_triangle[i + 1];
                pos.z() = m_reference_triangle[i + 2];
                i += 3;
                nous::vec4 view_pos = NousConvert::product_v4(cam_vp_mat, pos);
                if(view_pos.w() < 0.0f)// invisible
                    continue;

                view_pos.head<3>() /= view_pos.w();
                nous::vec2 npos = (view_pos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
                npos.x() *= vp.z;
                npos.y() *= vp.w;
                float distance_pow2 = (npos - nous::vec2(x, y)).squaredNorm();

                if(distance_pow2 < m_infer_pixel_range_pow2 && distance_pow2 < closest_pixel_dist_pow2)
                {
                    closest_pixel_dist_pow2 = distance_pow2;
                    current_infer.position = select_virtual_pos = pos;
                    current_infer.infer_type = InferType::TRIANGLE_POINT;
                }
            }
        }
        // element
        for(int i = 0; i < ray_cast_objs.size(); ++i)
        {
            nous::vec3 intersect_pos;
            SketchPointPtr cp = nullptr;
            double pixel_distanc_pow2 = DBL_MAX;
            if((pixel_distanc_pow2 = ray_cast_objs[i]->pixel_distance_to_view_point(nous::vec2(x, y), nous::vec2(vp.z, vp.w), cam_vp_mat,
                                                                                    intersect_pos, &cp)) < m_infer_pixel_range_pow2)
            {
                if(valid_infer(closest_pixel_dist_pow2, ray_cast_objs[i], pixel_distanc_pow2))
                {
                    closest_pixel_dist_pow2 = pixel_distanc_pow2;
                    intersect_pos = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), intersect_pos);
                    select_virtual_mesh = ray_cast_objs[i];
                    current_infer.position = select_virtual_pos = intersect_pos;
                    current_infer.element = select_virtual_ele = cp;
                    current_infer.infer_type = InferType::POINT;
                }
            }
        }
        // world 000
        nous::vec4 world_center_pos = NousConvert::product_v4(cam_vp_mat, nous::vec3::Zero());
        if(world_center_pos.w() > 0.0f)
        {
            world_center_pos.head<3>() /= world_center_pos.w();
            nous::vec2 npos = (world_center_pos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
            npos.x() *= vp.z;
            npos.y() *= vp.w;
            float distance_pow2 = (npos - nous::vec2(x, y)).squaredNorm();

            if(distance_pow2 < m_infer_pixel_range_pow2 && distance_pow2 < closest_pixel_dist_pow2)
            {
                closest_pixel_dist_pow2 = distance_pow2;
                select_virtual_mesh = nullptr;
                select_world_center = true;
                current_infer.position = select_virtual_pos = nous::vec3::Zero();
                current_infer.infer_type = InferType::POINT;
            }
        }
    }

    if(m_infer_filter_type & InferType::LINE && current_infer.infer_type == InferType::NONE)
    {
        // attach
        if(m_refer_triangle_lines)
        {
            auto get_picked_pos_distance_from_view_line = [&](nous::mat4& cam_vp, nous::vec3 v0, nous::vec3 v1, nous::vec3& pos) {
                double t0, t1;
                if(!rayIntersect(cam_pos, cam_dir, NousConvert::convert(v0), NousConvert::convert(nous::vec3(v1 - v0)), t0, t1) || t0 < 0.0 ||
                   t1 < 0.0 || t1 > 1.0)
                    return FLT_MAX;

                pos = v0 + (v1 - v0) * t1;
                nous::vec4 vpos = NousConvert::product_v4(cam_vp, pos);
                if(vpos.w() < 0.0f)// invisible
                    return FLT_MAX;

                vpos.head<3>() /= vpos.w();
                nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
                npos.x() *= vp.z;
                npos.y() *= vp.w;
                return (npos - nous::vec2(x, y)).squaredNorm();
            };

            for(int i = 0; i < m_reference_triangle.size();)
            {
                nous::vec3 pos_0;
                pos_0.x() = m_reference_triangle[i];
                pos_0.y() = m_reference_triangle[i + 1];
                pos_0.z() = m_reference_triangle[i + 2];
                nous::vec3 pos_1;
                pos_1.x() = m_reference_triangle[i + 3];
                pos_1.y() = m_reference_triangle[i + 4];
                pos_1.z() = m_reference_triangle[i + 5];
                nous::vec3 pos_2;
                pos_2.x() = m_reference_triangle[i + 6];
                pos_2.y() = m_reference_triangle[i + 7];
                pos_2.z() = m_reference_triangle[i + 8];
                i += 9;

                std::vector<nous::vec3> view_pos;
                view_pos.emplace_back(pos_0);
                view_pos.emplace_back(pos_1);
                view_pos.emplace_back(pos_2);

                for(int i = 0; i < view_pos.size(); ++i)
                {
                    nous::vec3 pos;
                    float distance_pow2 = get_picked_pos_distance_from_view_line(cam_vp_mat, view_pos[i], view_pos[(i + 1) % view_pos.size()], pos);
                    if(distance_pow2 < m_infer_pixel_range_pow2 && distance_pow2 < closest_pixel_dist_pow2)
                    {
                        closest_pixel_dist_pow2 = distance_pow2;
                        select_virtual_mesh = nullptr;
                        current_infer.position = select_virtual_pos = pos;
                        current_infer.infer_type = InferType::TRIANGLE_LINE;
                    }
                }
            }
        }
        // element
        for(int i = 0; i < ray_cast_objs.size(); ++i)
        {
            double pixel_distanc_pow2 = DBL_MAX;
            nous::vec3 pos;
            nous::vec3 tangent;
            SketchElementPtr line = nullptr;

            if((pixel_distanc_pow2 =
                    ray_cast_objs[i]->ray_intersect_view_line(ray_cast_objs_local_ray[i].first, ray_cast_objs_local_ray[i].second, nous::vec2(x, y),
                                                              nous::vec2(vp.z, vp.w), cam_vp_mat, pos, tangent, &line)) < m_infer_pixel_range_pow2)
            {
                if(valid_infer(closest_pixel_dist_pow2, ray_cast_objs[i], pixel_distanc_pow2))
                {
                    closest_pixel_dist_pow2 = pixel_distanc_pow2;
                    select_virtual_pos = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), pos);
                    select_virtual_mesh = ray_cast_objs[i];
                    current_infer.position = select_virtual_pos;
                    current_infer.tangent = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), tangent).normalized();
                    current_infer.element = select_virtual_ele = line;
                    current_infer.infer_type = InferType::LINE;
                    if(select_virtual_ele->type() != EElementType::Line)
                    {
                        current_infer.infer_type = InferType::CURVE;
                    }
                }
            }
        }

        // check line center
        if(m_infer_filter_type & InferType::LINE_CENTER && current_infer.infer_type == InferType::LINE)
        {
            nous::vec3 intersect_pos;
            double pixel_dist_pow2 = select_virtual_mesh->position_distance_to_view_line_center(
                current_infer.position, current_infer.element->to_line(), nous::vec2(vp.z, vp.w), cam_vp_mat, intersect_pos);
            if(pixel_dist_pow2 < m_infer_pixel_range_pow2)
            {
                intersect_pos = NousConvert::product_v3(select_virtual_mesh->doc()->world_matrix(), intersect_pos);
                select_virtual_pos = intersect_pos;
                current_infer.position = intersect_pos;
                current_infer.infer_type = InferType::LINE_CENTER;
            }
        }
    }

    bool select_triangle = false;
    Mesh* select_face_mesh = nullptr;
    SketchElementPtr select_face_ele = nullptr;
    int select_face_vid = -1;
    double closest_dist = DBL_MAX;

    if(m_infer_filter_type & InferType::POLYGON)
    {
        // attach
        if(m_refer_triangle_faces)
        {
            for(int i = 0; i < m_reference_triangle.size();)
            {
                nous::vec3 pos_0;
                pos_0.x() = m_reference_triangle[i];
                pos_0.y() = m_reference_triangle[i + 1];
                pos_0.z() = m_reference_triangle[i + 2];
                nous::vec3 pos_1;
                pos_1.x() = m_reference_triangle[i + 3];
                pos_1.y() = m_reference_triangle[i + 4];
                pos_1.z() = m_reference_triangle[i + 5];
                nous::vec3 pos_2;
                pos_2.x() = m_reference_triangle[i + 6];
                pos_2.y() = m_reference_triangle[i + 7];
                pos_2.z() = m_reference_triangle[i + 8];
                i += 9;

                double distance = 0.0;
                Engine::Vec3 pos = Engine::Vec3();
                if(rayIntersectTri(cam_pos, cam_dir, Engine::Vec3(pos_0.x(), pos_0.y(), pos_0.z()), Engine::Vec3(pos_1.x(), pos_1.y(), pos_1.z()),
                                   Engine::Vec3(pos_2.x(), pos_2.y(), pos_2.z()), &distance, &pos))
                {
                    if(distance < closest_dist)
                    {
                        closest_dist = distance;
                        select_face_mesh = nullptr;
                        select_triangle = true;
                        current_infer.position = NousConvert::convert(pos);
                    }
                    break;
                }
            }
        }
        // element
        for(int i = 0; i < ray_cast_objs.size(); ++i)
        {
            double distance = DBL_MAX;
            nous::vec3 pos;
            nous::vec3 normal;

            SketchElementPtr cp = nullptr;
            if(ray_cast_objs[i]->ray_intersect_face(ray_cast_objs_local_ray[i].first, ray_cast_objs_local_ray[i].second, pos, normal, &cp))
            {
                pos = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), pos);
                float distance = (NousConvert::convert(cam_pos) - pos).norm();
                if(valid_infer(closest_dist, ray_cast_objs[i], distance))
                {
                    closest_dist = distance;
                    select_face_mesh = ray_cast_objs[i];
                    select_triangle = false;
                    select_face_vid = i;
                    current_infer.element = select_face_ele = cp;
                    current_infer.position = pos;
                    auto& mat_inv = ray_cast_objs[i]->doc()->world_matrix_inv();
                    current_infer.normal = (NousConvert::product_v3(mat_inv, normal) - NousConvert::convert(mat_inv.pos)).normalized();
                }
            }
        }
    }

    // choose the closest one
    if((select_world_center || select_virtual_mesh) && (select_face_mesh))
    {
        nous::vec3 vo = ray_cast_objs_local_ray[select_face_vid].first;
        nous::vec3 vd = (select_virtual_pos - NousConvert::convert(cam_pos)).normalized();
        auto& matWInv = select_face_mesh->doc()->world_matrix_inv();
        vd = (NousConvert::product_v3(matWInv, vd) - NousConvert::convert(matWInv.pos)).normalized();

        nous::vec3 pos;
        nous::vec3 normal;
        float el = FLT_MAX;
        SketchElementPtr ele = nullptr;
        if(select_face_mesh->ray_intersect_face(vo, vd, pos, normal, &ele))
        {
            pos = NousConvert::product_v3(select_face_mesh->doc()->world_matrix(), pos);
            el = (NousConvert::convert(cam_pos) - pos).norm();
        }

        float vl = (NousConvert::convert(cam_pos) - select_virtual_pos).norm();
        float dis = vl - el;
        if(dis > sketch_math::sketch_epsilon)
        {
            select_virtual_mesh = nullptr;
        }
        else
        {
            select_face_mesh = nullptr;
        }
    }

    if(select_face_mesh)
    {
        current_infer.mesh = select_face_mesh;
        current_infer.element = select_face_ele;
        if(current_infer.element->type() == EElementType::Polygon)
        {
            current_infer.infer_type = InferType::POLYGON;
        }
        else if(current_infer.element->type() == EElementType::AlgebraSurface || current_infer.element->type() == EElementType::ClosedSurface ||
                current_infer.element->type() == EElementType::TrimmedSurface)
        {
            current_infer.infer_type = InferType::SURFACE;
        }
    }
    else if(select_virtual_mesh)
    {
        current_infer.mesh = select_virtual_mesh;
        current_infer.element = select_virtual_ele;
        current_infer.position = select_virtual_pos;
    }
    else if(select_triangle)
    {
        current_infer.infer_type = InferType::POLYGON;
    }

    // ignore
    if(current_infer.mesh)
    {
        if(m_infer_ignore_obj_list.find(current_infer.mesh) != m_infer_ignore_obj_list.end())
        {
            current_infer = InferResult();
        }
        else
        {
            auto ignore_pos = m_infer_ignore_list.find(current_infer.mesh);
            if(ignore_pos != m_infer_ignore_list.end())
            {
                if(ignore_pos->second.elements.find(current_infer.element) != ignore_pos->second.elements.end())
                {
                    current_infer = InferResult();
                }
            }
        }
    }

    // check infer
    // if(m_infer_filter_type & InferType::INFER_OF_REFERENCE)
    {
        if(!current_infer.is_point() && !current_infer.is_nurbs() && !m_enalble_force_reference)
        {
            _inference_with_refer(current_infer, view, x, y, extra_refer_pos, extra_type);
        }
    }

    // check axis plane
    if(m_infer_filter_type & InferType::PLANE_XYZ_TYPE && current_infer.infer_type == InferType::NONE)
    {
        if(extra_refer_pos && extra_type == InferType::PLANE_XYZ_TYPE)
        {
            _intersect_with_xyz_plan(view, x, y, current_infer.position, current_infer.normal, *extra_refer_pos);
            current_infer.infer_type = InferType(InferType::PLANE_X | InferType::PLANE_Y | InferType::PLANE_Z);
        }
        else
        {
            _intersect_with_xyz_plan(view, x, y, current_infer.position, current_infer.normal);
            current_infer.infer_type = InferType(InferType::PLANE_X | InferType::PLANE_Y | InferType::PLANE_Z);
        }
    }

    if(current_infer.normal.squaredNorm() < std::powf(sketch_math::sketch_epsilon, 2.0f))
    {
        nous::vec3 o;
        _intersect_with_xyz_plan(view, x, y, o, current_infer.normal);
    }

    if(m_enalble_force_reference)
    {
        _inference_with_force_refer(current_infer, NousConvert::convert(cam_pos), NousConvert::convert(cam_dir));
    }
    else if(m_infer_filter_type & InferType::AXIS_TYPE)
    {
        // 生成自身推导线
        if(extra_refer_pos && extra_type == InferType::AXIS_TYPE)
        {
            bool is_exist = false;
            for(auto& infer: current_infer.inference_lines)
            {
                if(infer.first & InferType::CURRENT_LINE)
                {
                    is_exist = true;
                    break;
                }
            }
            if(!is_exist)
            {
                current_infer.inference_lines.emplace_back(InferType::CURRENT_LINE, std::pair{ *extra_refer_pos, current_infer.position });
            }
        }
    }

    // update snap
    if(current_infer.mesh != m_infer_result.mesh || current_infer.infer_type != m_infer_result.infer_type ||
       current_infer.position != m_infer_result.position || current_infer.normal != m_infer_result.normal)
    {
        m_infer_result = current_infer;
        m_infer_result.hover_time = Util::getCurrentTimeMS();
    }
}

void InferenceHelper::render_obj_changed(Define::Message method, RenderObject* obj)
{
    if(!obj)
        return;

    switch(method)
    {
    case Define::MSG_OBJ_DELETE: {
        // ignore
        m_infer_ignore_obj_list.erase(obj);
        m_infer_ignore_list.erase(obj);
        // infer
        if(m_infer_result.mesh == obj)
        {
            m_infer_result = InferResult();
        }
        // refer
        std::vector<ReferAttrib> infer_points_array;
        while(!m_reference_points_queue.empty())
        {
            if(m_reference_points_queue.front().mesh != obj)
            {
                infer_points_array.emplace_back(std::move(m_reference_points_queue.front()));
            }
            m_reference_points_queue.pop();
        }
        for(auto& infer: infer_points_array)
        {
            m_reference_points_queue.push(infer);
        }

        std::vector<ReferAttrib> infer_lines_array;
        while(!m_reference_lines_queue.empty())
        {
            if(m_reference_lines_queue.front().mesh != obj)
            {
                infer_lines_array.emplace_back(std::move(m_reference_lines_queue.front()));
            }
            m_reference_lines_queue.pop();
        }
        for(auto& infer: infer_lines_array)
        {
            m_reference_lines_queue.push(infer);
        }

        std::vector<ReferAttrib> infer_polygon_array;
        while(!m_reference_polygons_queue.empty())
        {
            if(m_reference_polygons_queue.front().mesh != obj)
            {
                infer_polygon_array.emplace_back(std::move(m_reference_polygons_queue.front()));
            }
            m_reference_polygons_queue.pop();
        }
        for(auto& infer: infer_polygon_array)
        {
            m_reference_polygons_queue.push(infer);
        }

        std::vector<ReferAttrib> infer_curve_array;
        while(!m_reference_curve_queue.empty())
        {
            if(m_reference_curve_queue.front().mesh != obj)
            {
                infer_curve_array.emplace_back(std::move(m_reference_curve_queue.front()));
            }
            m_reference_curve_queue.pop();
        }
        for(auto& infer: infer_curve_array)
        {
            m_reference_curve_queue.push(infer);
        }

        std::vector<ReferAttrib> infer_surface_array;
        while(!m_reference_surface_queue.empty())
        {
            if(m_reference_surface_queue.front().mesh != obj)
            {
                infer_surface_array.emplace_back(std::move(m_reference_surface_queue.front()));
            }
            m_reference_surface_queue.pop();
        }
        for(auto& infer: infer_curve_array)
        {
            m_reference_surface_queue.push(infer);
        }
    }
    break;
    case Define::MSG_OBJ_UPDATE: {
        // ignore
        auto& pos = m_infer_ignore_list.find(obj);
        if(pos != m_infer_ignore_list.end())
        {
            std::unordered_set<SketchElementPtr> invalid_ele;
            if(obj->type() == Define::ObjectType::OBJ_D5_MESH)
            {
                auto mesh_doc = dynamic_cast<MeshDoc*>(obj->doc());
                auto geo_data = mesh_doc->geodata();
                for(auto ele: pos->second.elements)
                {
                    if(!geo_data->is_valid(ele))
                    {
                        invalid_ele.insert(ele);
                    }
                }
            }
            else if(obj->type() == Define::ObjectType::OBJ_D5_NURBS)
            {
                auto nurbs_doc = dynamic_cast<NurbsDoc*>(obj->doc());
                auto nurbs_data = nurbs_doc->nurbsdata();
                for(auto ele: pos->second.elements)
                {
                    if(!nurbs_data->item_exist(ele->id()))
                    {
                        invalid_ele.insert(ele);
                    }
                }
            }

            for(auto lk: invalid_ele)
            {
                pos->second.elements.erase(lk);
            }
            if(!pos->second.elements.size())
            {
                m_infer_ignore_list.erase(pos);
            }
        }

        // infer
        if(m_infer_result.mesh == obj)
        {
            if(obj->type() == Define::ObjectType::OBJ_D5_MESH)
            {
                auto mesh_doc = dynamic_cast<MeshDoc*>(obj->doc());
                auto geo_data = mesh_doc->geodata();
                if(!geo_data->is_valid(m_infer_result.element))
                {
                    m_infer_result = InferResult();
                }
            }

            if(obj->type() == Define::ObjectType::OBJ_D5_NURBS)
            {
                auto nurbs_doc = dynamic_cast<NurbsDoc*>(obj->doc());
                auto nurbs_data = nurbs_doc->nurbsdata();
                if(!nurbs_data->item_exist(m_infer_result.element->id()))
                {
                    m_infer_result = InferResult();
                }
            }
        }

        // refer
        std::vector<ReferAttrib> inferPointsArray;
        while(!m_reference_points_queue.empty())
        {
            if(m_reference_points_queue.front().mesh != obj)
            {
                inferPointsArray.emplace_back(std::move(m_reference_points_queue.front()));
            }
            m_reference_points_queue.pop();
        }
        for(auto& infer: inferPointsArray)
        {
            m_reference_points_queue.push(infer);
        }

        std::vector<ReferAttrib> inferLinesArray;
        while(!m_reference_lines_queue.empty())
        {
            if(m_reference_lines_queue.front().mesh != obj)
            {
                inferLinesArray.emplace_back(std::move(m_reference_lines_queue.front()));
            }
            m_reference_lines_queue.pop();
        }
        for(auto& infer: inferLinesArray)
        {
            m_reference_lines_queue.push(infer);
        }

        std::vector<ReferAttrib> inferPolygonArray;
        while(!m_reference_polygons_queue.empty())
        {
            if(m_reference_polygons_queue.front().mesh != obj)
            {
                inferPolygonArray.emplace_back(std::move(m_reference_polygons_queue.front()));
            }
            m_reference_polygons_queue.pop();
        }
        for(auto& infer: inferPolygonArray)
        {
            m_reference_polygons_queue.push(infer);
        }

        std::vector<ReferAttrib> inferCurveArray;
        while(!m_reference_curve_queue.empty())
        {
            if(m_reference_curve_queue.front().mesh != obj)
            {
                inferCurveArray.emplace_back(std::move(m_reference_curve_queue.front()));
            }
            m_reference_curve_queue.pop();
        }
        for(auto& infer: inferCurveArray)
        {
            m_reference_curve_queue.push(infer);
        }

        std::vector<ReferAttrib> inferSurfaceArray;
        while(!m_reference_surface_queue.empty())
        {
            if(m_reference_surface_queue.front().mesh != obj)
            {
                inferSurfaceArray.emplace_back(std::move(m_reference_surface_queue.front()));
            }
            m_reference_surface_queue.pop();
        }
        for(auto& infer: inferCurveArray)
        {
            m_reference_surface_queue.push(infer);
        }
    }
    break;
    }
}

void InferenceHelper::_gen_reference_element(InferResult& infer, bool force)
{
    if(infer.infer_type & InferType::CURRENT_LINE)
    {
        return;
    }
    // 参考元素无去重逻辑
    if(Util::getCurrentTimeMS() - infer.hover_time > 800 || force)
    {
        infer.hover_time = Util::getCurrentTimeMS();
        if(infer.element && infer.element->type() == EElementType::Line)
        {
            std::pair<nous::vec3, nous::vec3> line_pos = static_cast<Mesh*>(infer.mesh)->get_line_world_position(infer.element->to_line());
            ReferAttrib refer = ReferAttrib(line_pos.first, line_pos.second, InferType::VPC_LINE_TYPE);
            refer.mesh = infer.mesh;
            m_reference_lines_queue.push(refer);
            if(m_reference_lines_queue.size() > 1)
            {
                m_reference_lines_queue.pop();
            }
        }
        if(infer.element && infer.element->type() == EElementType::Polygon)
        {
            ReferAttrib refer = ReferAttrib(infer.position, infer.position + infer.normal, InferType::VPC_PLANE_TYPE);
            refer.mesh = infer.mesh;
            static_cast<Mesh*>(infer.mesh)->get_polygon_topo_world_position(infer.element->to_polygon(), refer.polygon);
            m_reference_polygons_queue.push(refer);
            if(m_reference_polygons_queue.size() > 1)
            {
                m_reference_polygons_queue.pop();
            }
        }
        if(infer.element && infer.element->type() == EElementType::Curve)
        {
            ReferAttrib refer = ReferAttrib(infer.position, infer.normal, InferType::VPC_CURVE_TYPE);
            refer.mesh = infer.mesh;
            m_reference_curve_queue.push(refer);
            if(m_reference_curve_queue.size() > 1)
            {
                m_reference_curve_queue.pop();
            }
        }
        if(infer.element && infer.element->type() == EElementType::AlgebraSurface)
        {
            ReferAttrib refer = ReferAttrib(infer.position, infer.normal, InferType::VPC_SURFACE_TYPE);
            refer.mesh = infer.mesh;
            m_reference_surface_queue.push(refer);
            if(m_reference_surface_queue.size() > 1)
            {
                m_reference_surface_queue.pop();
            }
        }
        if(infer.is_point())
        {
            ReferAttrib refer = ReferAttrib(infer.position, nous::vec3::Zero(), InferType::AXIS_TYPE);
            refer.mesh = infer.mesh;
            m_reference_points_queue.push(refer);
            if(m_reference_points_queue.size() > 3)
            {
                m_reference_points_queue.pop();
            }
        }
    }
}

void InferenceHelper::_intersect_with_xyz_plan(View* view, int x, int y, nous::vec3& pos, nous::vec3& normal, const nous::vec3 axis_center)
{
    pos = axis_center;

    if(!view)
    {
        return;
    }

    auto calculate_angle = [](const nous::vec3& dir, const nous::vec3& normal) { return acos(dir.dot(normal) / (dir.norm() * normal.norm())); };

    Document* doc = STCore::inst->docMgr()->document();
    auto& vp = m_camera->viewport();
    Engine::Vec3 cDir = m_camera->forwardDir();
    Engine::Vec3 vOrigin, vDir;
    m_camera->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &vOrigin, &vDir);

    nous::vec3 normals[] = { nous::vec3(1.f, 0.f, 0.f), nous::vec3(0.f, 1.f, 0.f), nous::vec3(0.f, 0.f, 1.f) };
    float min_angle = -FLT_MAX;
    int best_plane = -1;

    float cam_dir_angle = abs(NousConvert::convert(cDir).dot(normals[2]));
    float angle_xy = calculate_angle(NousConvert::convert(vDir), normals[2]);
    if(abs(cam_dir_angle) > 0.25f && ((angle_xy > 1.6f && m_camera->pos().z > 0.f) || (angle_xy < 1.6f && m_camera->pos().z < 0.f)))
    {
        best_plane = 2;
    }
    else
    {
        // 查找夹角最大的XZ或YZ平面
        for(int i = 0; i < 2; ++i)// 只检查XZ和YZ
        {
            float angle = abs(NousConvert::convert(vDir).dot(normals[i]));
            if(angle > min_angle)
            {
                min_angle = angle;
                best_plane = i;
            }
        }
    }
    constexpr float epsilon = 1e-2f;
    switch(best_plane)
    {
    case 2:// XY平面，z = axis_center.z
        if(fabs(vDir.z) > epsilon)
        {
            float t = (axis_center.z() - vOrigin.z) / vDir.z;
            pos = NousConvert::convert(vOrigin + t * vDir);
            normal = nous::vec3(0.f, 0.f, 1.f);
        }
        break;
    case 1://  XZ平面，y = axis_center.y
        if(fabs(vDir.y) > epsilon)
        {
            float t = (axis_center.y() - vOrigin.y) / vDir.y;
            pos = NousConvert::convert(vOrigin + t * vDir);
            normal = nous::vec3(0.f, 1.f, 0.f);
        }
        break;
    case 0:// YZ平面，x = axis_center.x
        if(fabs(vDir.x) > epsilon)
        {
            float t = (axis_center.x() - vOrigin.x) / vDir.x;
            pos = NousConvert::convert(vOrigin + t * vDir);
            normal = nous::vec3(1.f, 0.f, 0.f);
        }
        break;
    default: break;
    }
}

void InferenceHelper::_inference_with_refer(InferResult& current_infer, View* view, int x, int y, nous::vec3* extra_refer_pos, InferType extra_type)
{
    float epsilon = 1e-2;

    auto copy_queue_data_to_vec = [](std::queue<ReferAttrib>& queue, std::vector<ReferAttrib>& vec) {
        std::vector<ReferAttrib> temp_vec;
        while(!queue.empty())
        {
            temp_vec.emplace_back(std::move(queue.front()));
            queue.pop();
        }
        for(auto& refer: temp_vec)
        {
            queue.push(refer);
        }
        vec.insert(vec.end(), temp_vec.begin(), temp_vec.end());
    };

    struct infer_attrib
    {
        infer_attrib() {}
        infer_attrib(const nous::vec3& center, const nous::vec3& dir, uint32_t type)
            : refer_line({ nous::vec3::Zero(), nous::vec3::Zero() }), center(center), dir(dir), refer_type((InferType)type)
        {
        }
        nous::vec3 refer_point;
        std::pair<nous::vec3, nous::vec3> refer_line;
        std::vector<std::vector<nous::vec3>> refer_polygon;

        nous::vec3 center;
        nous::vec3 dir;
        InferType refer_type;
    };

    std::vector<ReferAttrib> for_infer_line_refer_array;
    std::vector<ReferAttrib> for_infer_plane_refer_array;

    copy_queue_data_to_vec(m_reference_points_queue, for_infer_line_refer_array);
    copy_queue_data_to_vec(m_reference_points_queue, for_infer_plane_refer_array);

    copy_queue_data_to_vec(m_reference_lines_queue, for_infer_line_refer_array);
    copy_queue_data_to_vec(m_reference_curve_queue, for_infer_line_refer_array);

    copy_queue_data_to_vec(m_reference_polygons_queue, for_infer_line_refer_array);
    copy_queue_data_to_vec(m_reference_polygons_queue, for_infer_plane_refer_array);

    copy_queue_data_to_vec(m_reference_surface_queue, for_infer_line_refer_array);

    std::vector<infer_attrib> infer_line_array;
    if(extra_refer_pos && extra_type == InferType::AXIS_TYPE)
    {
        // 新增自身LINE的XYZ推导
        infer_line_array.emplace_back(*extra_refer_pos, nous::vec3(1.f, 0.f, 0.f), InferType::AXIS_X | InferType::CURRENT_LINE).refer_point =
            *extra_refer_pos;
        infer_line_array.emplace_back(*extra_refer_pos, nous::vec3(0.f, 1.f, 0.f), InferType::AXIS_Y | InferType::CURRENT_LINE).refer_point =
            *extra_refer_pos;
        infer_line_array.emplace_back(*extra_refer_pos, nous::vec3(0.f, 0.f, 1.f), InferType::AXIS_Z | InferType::CURRENT_LINE).refer_point =
            *extra_refer_pos;
    }
    for(auto& ref: for_infer_line_refer_array)
    {
        if(ref.refer_type == InferType::AXIS_TYPE)
        {
            infer_line_array.emplace_back(ref.para_0, nous::vec3(1.f, 0.f, 0.f), InferType::AXIS_X).refer_point = ref.para_0;
            infer_line_array.emplace_back(ref.para_0, nous::vec3(0.f, 1.f, 0.f), InferType::AXIS_Y).refer_point = ref.para_0;
            infer_line_array.emplace_back(ref.para_0, nous::vec3(0.f, 0.f, 1.f), InferType::AXIS_Z).refer_point = ref.para_0;
        }

        if(ref.refer_type == InferType::VPC_LINE_TYPE && extra_refer_pos)
        {
            nous::vec3 collinear_dir = (ref.para_1 - ref.para_0).normalized();
            // parallel&collinear
            infer_line_array.emplace_back(*extra_refer_pos, collinear_dir, InferType::PARALLEL_LINE | InferType::CURRENT_LINE).refer_line = {
                ref.para_0, ref.para_1
            };
            infer_line_array.back().refer_point = *extra_refer_pos;
            // vertical
            nous::vec3 perpendicular_x = collinear_dir.cross(nous::vec3(1.0f, 0.0f, 0.0f));
            nous::vec3 perpendicular_y = collinear_dir.cross(nous::vec3(0.0f, 1.0f, 0.0f));
            nous::vec3 perpendicular_z = collinear_dir.cross(nous::vec3(0.0f, 0.0f, 1.0f));
            if(perpendicular_x.squaredNorm() > epsilon * epsilon)
            {
                infer_line_array.emplace_back(*extra_refer_pos, perpendicular_x, InferType::VERTICAL_LINE | InferType::CURRENT_LINE).refer_line = {
                    ref.para_0, ref.para_1
                };
                infer_line_array.back().refer_point = *extra_refer_pos;
            }
            if(perpendicular_y.squaredNorm() > epsilon * epsilon)
            {
                infer_line_array.emplace_back(*extra_refer_pos, perpendicular_y, InferType::VERTICAL_LINE | InferType::CURRENT_LINE).refer_line = {
                    ref.para_0, ref.para_1
                };
                infer_line_array.back().refer_point = *extra_refer_pos;
            }
            if(perpendicular_z.squaredNorm() > epsilon * epsilon)
            {
                infer_line_array.emplace_back(*extra_refer_pos, perpendicular_z, InferType::VERTICAL_LINE | InferType::CURRENT_LINE).refer_line = {
                    ref.para_0, ref.para_1
                };
                infer_line_array.back().refer_point = *extra_refer_pos;
            }

            // 如果当前悬浮在平面上，计算vpc_pos为起点的与平面平行的垂直线
            if(current_infer.infer_type == InferType::POLYGON)
            {
                nous::vec3 perpendicular_polygon = collinear_dir.cross(current_infer.normal);
                if(perpendicular_polygon.squaredNorm() > epsilon * epsilon)
                {
                    infer_line_array.emplace_back(*extra_refer_pos, perpendicular_polygon, InferType::VERTICAL_PLANE | InferType::CURRENT_LINE)
                        .refer_line = { ref.para_0, ref.para_1 };
                    infer_line_array.back().refer_point = *extra_refer_pos;
                }
            }
        }

        if(ref.refer_type == InferType::VPC_CURVE_TYPE && extra_refer_pos)
        {
            nous::vec3 collinear_dir = ref.para_1;
            // parallel&collinear
            infer_attrib& infer_attr = infer_line_array.emplace_back(ref.para_0, collinear_dir, InferType::PARALLEL_CURVE | InferType::CURRENT_LINE);
            infer_attr.refer_line = std::make_pair(ref.para_0 - 1000.0 * ref.para_1, ref.para_0 + 1000.0 * ref.para_1);
            infer_line_array.back().refer_point = ref.para_0;
        }

        if(ref.refer_type == InferType::VPC_PLANE_TYPE && extra_refer_pos)
        {
            infer_line_array.emplace_back(*extra_refer_pos, ref.para_1 - ref.para_0, InferType::VERTICAL_PLANE | InferType::CURRENT_LINE)
                .refer_polygon = ref.polygon;
            infer_line_array.back().refer_point = *extra_refer_pos;
        }

        if(ref.refer_type == InferType::VPC_SURFACE_TYPE && extra_refer_pos)
        {
            infer_attrib& infer_attr = infer_line_array.emplace_back(ref.para_0, ref.para_1, InferType::VERTICAL_SURFACE | InferType::CURRENT_LINE);
            infer_attr.refer_line = std::make_pair(ref.para_0 - 1000.0 * ref.para_1, ref.para_0 + 1000.0 * ref.para_1);
            infer_line_array.back().refer_point = ref.para_0;
        }
    }

    Document* doc = STCore::inst->docMgr()->document();
    auto& cam_viewport = m_camera->viewport();
    const Engine::Mat4f& vp_mat = m_camera->dvpMatrix();
    Engine::Vec3 vOrigin, vDir;
    m_camera->getDocPickRay(doc, (float)x + cam_viewport.x, (float)y + cam_viewport.y, &vOrigin, &vDir);
    nous::vec3 o = nous::vec3(vOrigin.x, vOrigin.y, vOrigin.z);
    nous::vec3 d = o + nous::vec3(vDir.x, vDir.y, vDir.z);

    double pixel_dis_2 = DBL_MAX;
    double collinear_dis_2 = DBL_MAX;
    InferType infer_type_1 = InferType::NONE;
    nous::vec3 infer_intersect_point;
    infer_attrib infer_intersect_line;

    // 排除与当前选中线平行的推导线
    nous::vec3 u;
    if(current_infer.infer_type & InferType::LINE)
    {
        MeshDoc* mesh_doc = static_cast<MeshDoc*>(current_infer.mesh->doc());
        Engine::Mat43 mat = mesh_doc->world_matrix();
        auto geo = mesh_doc->geodata();
        SketchLinePtr line = current_infer.element->to_line();
        nous::vec3 v0 = geo->get_point_position(line->p0);
        nous::vec3 v1 = geo->get_point_position(line->p1);
        v0 = NousConvert::product_v3(mat, v0);
        v1 = NousConvert::product_v3(mat, v1);
        u = v1 - v0;
    }
    for(auto& infer: infer_line_array)
    {
        if(current_infer.infer_type & InferType::LINE)
        {
            nous::vec3 v = infer.dir;
            nous::vec3 u_cross_v = u.normalized().cross(v.normalized());
            float n = u_cross_v.squaredNorm();
            if(abs(n) < epsilon * epsilon)
            {
                continue;
            }
        }

        nous::vec3 p0;
        nous::vec3 p1;
        nous::vec4 view_pos;
        nous::algo::line_intersect_3d(o, d, infer.center, infer.center + infer.dir, p0, p1, false);
        view_pos = NousConvert::convert(vp_mat) * nous::vec4(p1.x(), p1.y(), p1.z(), 1.f);
        if(view_pos.w() > 0.f)
        {
            view_pos.head<3>() = view_pos.head<3>() / view_pos.w();
            nous::vec2 pixel_pos = nous::vec2(view_pos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5f;
            pixel_pos.x() *= cam_viewport.z;
            pixel_pos.y() *= cam_viewport.w;
            double view_pixel_dis_2 = (pixel_pos - nous::vec2(x, y)).squaredNorm();
            double cld = (p1 - infer.center).squaredNorm();
            if(std::abs(view_pixel_dis_2 - pixel_dis_2) < epsilon)
            {
                // 优先 ：更短的推导线 XYZ方向的推导线
                float sub_dist = cld - collinear_dis_2;
                if(abs(sub_dist) < epsilon || sub_dist < -epsilon)
                {
                    if(infer.refer_type & AXIS_TYPE || sub_dist < -epsilon)
                    {
                        collinear_dis_2 = cld;
                        infer_intersect_point = p1;
                        infer_intersect_line = infer;
                        infer_type_1 = infer.refer_type;
                    }
                }
            }
            else if(view_pixel_dis_2 < m_infer_pixel_range_pow2 && view_pixel_dis_2 < pixel_dis_2)
            {
                pixel_dis_2 = view_pixel_dis_2;
                collinear_dis_2 = cld;
                infer_intersect_point = p1;
                infer_intersect_line = infer;
                infer_type_1 = infer.refer_type;
            }
        }
    }

    if(pixel_dis_2 < m_infer_pixel_range_pow2)
    {
        current_infer.reference_points.emplace_back(std::move(infer_intersect_line.refer_point));
        current_infer.reference_lines.emplace_back(std::move(infer_intersect_line.refer_line));
        current_infer.reference_polygons.emplace_back(std::move(infer_intersect_line.refer_polygon));
        current_infer.inference_lines.emplace_back(infer_intersect_line.refer_type, std::pair{ infer_intersect_line.center, infer_intersect_point });

        // check infer line intersect with other element
        if(current_infer.infer_type & InferType::LINE)
        {
            _check_line_intersection_with_inference(current_infer);
        }
        else if(current_infer.infer_type & InferType::POLYGON)
        {
            _check_polygon_intersection_with_inference(current_infer, m_camera);
        }
        else
        {
            // check infer line intersect with other refer points axis
            double infer_lines_intersect_to_current_refer_point_dis_2 = DBL_MAX;
            InferType infer_type_2 = InferType::NONE;
            nous::vec3 infer_lines_intersect_point;
            infer_attrib infer_intersect_line2;
            for(auto& infer: infer_line_array)
            {
                nous::vec3 u = infer_intersect_point - infer_intersect_line.center;
                nous::vec3 u_cross_v = u.normalized().cross(infer.dir);
                float n = u_cross_v.squaredNorm();
                if(abs(n) < epsilon * epsilon)
                {
                    continue;
                }

                nous::vec3 p0;
                nous::vec3 p1;
                if(nous::algo::line_intersect_3d(infer_intersect_line.center, infer_intersect_point, infer.center, infer.center + infer.dir, p0, p1,
                                                 false))
                {
                    float d = (p0 - p1).squaredNorm();
                    if(d < epsilon)
                    {
                        float cd = (p1 - infer_intersect_point).squaredNorm();
                        if(cd < infer_lines_intersect_to_current_refer_point_dis_2)
                        {
                            infer_lines_intersect_to_current_refer_point_dis_2 = cd;
                            infer_lines_intersect_point = p1;
                            infer_intersect_line2 = infer;
                            infer_type_2 = infer.refer_type;
                        }
                    }
                }
            }
            if(infer_lines_intersect_to_current_refer_point_dis_2 < DBL_MAX)
            {
                nous::vec4 view_pos = NousConvert::convert(vp_mat) * nous::vec4(infer_lines_intersect_point.x(), infer_lines_intersect_point.y(),
                                                                                infer_lines_intersect_point.z(), 1.f);
                if(view_pos.w() > 0.f)
                {
                    view_pos.head<3>() = view_pos.head<3>() / view_pos.w();
                    nous::vec2 view_pixel_pos = nous::vec2(view_pos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5f;
                    view_pixel_pos.x() *= cam_viewport.z;
                    view_pixel_pos.y() *= cam_viewport.w;
                    double view_pixel_dis_2 = (view_pixel_pos - nous::vec2(x, y)).squaredNorm();
                    if(view_pixel_dis_2 < m_infer_pixel_range_pow2)
                    {
                        infer_type_1 = InferType(infer_type_1 | infer_type_2);
                        infer_intersect_point = infer_lines_intersect_point;
                        current_infer.reference_points.emplace_back(std::move(infer_intersect_line2.refer_point));
                        current_infer.reference_lines.emplace_back(std::move(infer_intersect_line2.refer_line));
                        current_infer.reference_polygons.emplace_back(std::move(infer_intersect_line2.refer_polygon));
                        current_infer.inference_lines.back().second.second = infer_intersect_point;
                        current_infer.inference_lines.emplace_back(infer_intersect_line2.refer_type,
                                                                   std::pair{ infer_intersect_line2.center, infer_intersect_point });
                    }
                }
            }

            current_infer.infer_type = infer_type_1;
            current_infer.position = infer_intersect_point;
        }
    }
}

void InferenceHelper::_inference_with_force_refer(InferResult& infer, const nous::vec3& cam_pos, const nous::vec3& cam_dir)
{
    // 强制推导可能需要作为附加结果来呈现，从而不影响普通infer的结果
    if(m_force_reference.refer_type == InferType::COLLINEAR_LINE)
    {
        // 这里可能需要使用camera射线来求交优化体验
        nous::vec3 line_dir = m_force_reference.para_1 - m_force_reference.para_0;
        nous::vec3 p_dir = infer.position - m_force_reference.para_0;

        float t = p_dir.dot(line_dir) / line_dir.dot(line_dir);
        nous::vec3 proj_p_on_line = m_force_reference.para_0 + t * line_dir;

        infer.infer_type = m_force_reference_type;
        infer.inference_lines.emplace_back(InferType(InferType::CURRENT_LINE | m_force_reference_type),
                                           std::pair{ m_force_reference.para_0, proj_p_on_line });
        infer.inference_lines.emplace_back(InferType::SEG_LINE, std::pair{ proj_p_on_line, infer.position });

        infer.position = proj_p_on_line;
    }
    if(m_force_reference.refer_type == InferType::PLANE_CUSTOM)
    {
    }
}

bool InferenceHelper::_check_line_intersection_with_inference(InferResult& current_infer)
{
    float epsilon = 1e-2;
    if(current_infer.inference_lines.size() != 1)
        return false;

    MeshDoc* mesh_doc = dynamic_cast<MeshDoc*>(current_infer.mesh->doc());
    if(mesh_doc)
    {
        Engine::Mat43 mat = mesh_doc->world_matrix();
        auto geo = mesh_doc->geodata();
        SketchLinePtr line = current_infer.element->to_line();
        nous::vec3 v0 = geo->get_point_position(line->p0);
        nous::vec3 v1 = geo->get_point_position(line->p1);
        v0 = NousConvert::product_v3(mat, v0);
        v1 = NousConvert::product_v3(mat, v1);

        for(auto& inferLine: current_infer.inference_lines)
        {
            nous::vec3 v2 = inferLine.second.first;
            nous::vec3 v3 = inferLine.second.second;

            nous::vec3 p0;
            nous::vec3 p1;

            nous::vec3 u = v1 - v0;
            nous::vec3 v = v3 - v2;
            nous::vec3 u_cross_v = u.normalized().cross(v.normalized());
            float n = u_cross_v.squaredNorm();
            if(abs(n) > epsilon * epsilon && nous::algo::line_intersect_3d(v0, v1, v2, v3, p0, p1, false))
            {
                if(abs((p0 - p1).norm()) < epsilon)
                {
                    auto is_point_on_segment = [&]() {
                        return (std::min(v0.x(), v1.x()) <= p0.x() + epsilon && p0.x() - epsilon <= std::max(v0.x(), v1.x())) &&
                               (std::min(v0.y(), v1.y()) <= p0.y() + epsilon && p0.y() - epsilon <= std::max(v0.y(), v1.y())) &&
                               (std::min(v0.z(), v1.z()) <= p0.z() + epsilon && p0.z() - epsilon <= std::max(v0.z(), v1.z()));
                    };
                    if(is_point_on_segment())
                    {
                        current_infer.position = p0;
                        inferLine.second.second = p0;
                        return true;
                    }
                }
            }
        }
        current_infer.reference_points.clear();
        current_infer.reference_lines.clear();
        current_infer.reference_polygons.clear();
        current_infer.inference_lines.clear();
    }

    return false;
}

bool InferenceHelper::_check_polygon_intersection_with_inference(InferResult& current_infer, ViewCamera* cam)
{
    // 辅助线与平面同时备选时，优先选择平面
    // 如果需要优先选择辅助线，需要判断辅助线是否被平面遮挡?

    float epsilon = 1e-2;
    if(current_infer.inference_lines.size() != 1)
        return false;

    const Engine::Mat4f& vp_mat = cam->dvpMatrix();
    auto& vp = cam->viewport();

    for(auto& inferLine: current_infer.inference_lines)
    {
        nous::vec3 v0 = inferLine.second.first;
        nous::vec3 v1 = inferLine.second.second;
        current_infer.position;
        current_infer.normal;

        nous::vec3 line_dir = v1 - v0;
        float denom = current_infer.normal.dot(line_dir);
        if(abs(denom) < epsilon)
        {
            if(fabs(current_infer.normal.dot(v0 - current_infer.position)) < epsilon)
            {
                current_infer.position = v1;
                return true;
            }
            current_infer.reference_points.clear();
            current_infer.reference_lines.clear();
            current_infer.reference_polygons.clear();
            current_infer.inference_lines.clear();
            return false;
        }

        float t = (current_infer.position - v0).dot(current_infer.normal) / denom;
        nous::vec3 intersection_point = v0 + t * line_dir;

        nous::vec4 vip = NousConvert::product_v4(vp_mat, intersection_point);
        nous::vec4 v1p = NousConvert::product_v4(vp_mat, v1);
        if(vip.w() > 0.f && v1p.w() > 0.f)
        {
            vip.head<3>() = vip.head<3>() / vip.w();
            nous::vec2 vipp = nous::vec2(vip.head<2>() + nous::vec2(1.f, 1.f)) * 0.5f;
            vipp.x() *= vp.z;
            vipp.y() *= vp.w;

            v1p.head<3>() = v1p.head<3>() / v1p.w();
            nous::vec2 v1pp = nous::vec2(v1p.head<2>() + nous::vec2(1.f, 1.f)) * 0.5f;
            v1pp.x() *= vp.z;
            v1pp.y() *= vp.w;
            double pixel_distance_2 = (vipp - v1pp).squaredNorm();

            if(pixel_distance_2 < m_infer_pixel_range_pow2)
            {
                current_infer.position = intersection_point;
                inferLine.second.second = intersection_point;
                return true;
            }
        }
    }

    current_infer.reference_points.clear();
    current_infer.reference_lines.clear();
    current_infer.reference_polygons.clear();
    current_infer.inference_lines.clear();
    return false;
}
}// namespace sketch
