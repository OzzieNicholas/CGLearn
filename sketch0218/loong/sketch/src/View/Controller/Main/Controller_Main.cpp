#include "sketch/View/Controller/Main/Controller_Main.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "Engine/Util/StrUtil.h"

using namespace Engine;

namespace sketch
{
Controller_Main::Controller_Main(const std::wstring& name) : Controller(name), m_lock_mouse_x(0), m_lock_mouse_y(0) {}

Controller_Main::~Controller_Main() = default;

void Controller_Main::active(const SketchParamMap& props)
{
    m_current_value = "";
    Controller::active(props);
}

void Controller_Main::deactive() { Controller::deactive(); }

void Controller_Main::escape(const SketchParamMap& props) { _unlock_mouse_move(); }

void Controller_Main::enter(const SketchParamMap& props)
{
    std::wstring ws_value = props.get_value<std::wstring>("value");
    m_current_value = StrUtil::w2s(ws_value);
    //_unlock_mouse_move();
}

void Controller_Main::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(m_lock)
    {
        const int dx = x - m_lock_mouse_x;
        const int dy = y - m_lock_mouse_y;
        if(dx * dx + dy * dy > 2500)
        {
            m_lock_mouse_x = x;
            m_lock_mouse_y = y;
            m_lock = false;
        }
    }
}

void Controller_Main::event_filter(const std::string& event, const SketchParamMap& props)
{
    // simulate key input
    if(event == "controller_input")
    {
        if(props.get_value<bool>("confirm"))
        {
            enter(props);
        }
        else
        {
            _lock_mouse_move();
        }
    }
    else if(event == "controller_escape")// user input escape
    {
        escape(props);
    }
}

void Controller_Main::_lock_mouse_move()
{
    m_lock = true;
    m_lock_mouse_x = STCore::inst->mousePhysicX();
    m_lock_mouse_y = STCore::inst->mousePhysicY();
}

void Controller_Main::_unlock_mouse_move()
{
    m_lock = false;
    m_current_value = "";
}

void Controller_Main::_update_helper_mesh(InferenceHelper* infer_helper)
{
    m_infer_point_tri.clear();
    m_infer_point_edge.clear();
    m_infer_lines.clear();
    m_refer_point_tri.clear();
    m_refer_point_edge.clear();
    m_refer_lines.clear();
    m_refer_polygons_edge.clear();

    auto set_look_at = [](const nous::mat4& org_mat, const nous::vec3& forward, const nous::vec3& up) {
        nous::vec3 eye = org_mat.col(3).head<3>();

        float scaleX = org_mat.col(0).head<3>().norm();
        float scaleY = org_mat.col(1).head<3>().norm();
        float scaleZ = org_mat.col(2).head<3>().norm();

        nous::vec3 right = up.cross(forward).normalized();
        nous::vec3 newUp = forward.cross(right).normalized();

        nous::mat4 rotationMatrix;
        rotationMatrix.col(0).head<3>() = right * scaleX;
        rotationMatrix.col(0).w() = org_mat.col(0).w();
        rotationMatrix.col(1).head<3>() = newUp * scaleY;
        rotationMatrix.col(1).w() = org_mat.col(1).w();
        rotationMatrix.col(2).head<3>() = forward * scaleZ;
        rotationMatrix.col(2).w() = org_mat.col(2).w();
        rotationMatrix.col(3).head<3>() = eye;
        rotationMatrix.col(3).w() = org_mat.col(3).w();

        return rotationMatrix;
    };

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ViewCamera* cam = view->camera();
    const InferResult& infer_result = infer_helper->get_infer_result();
    Document* doc = STCore::inst->docMgr()->document();

    // inference point
    nous::vec3 cp = NousConvert::convert(doc->v2d(cam->pos()));
    Engine::Vec4f cv = cam->viewport();
    Engine::Vec4f ov = cam->orthoView();
    Engine::Vec3 cud = cam->upDir();
    Engine::Vec3 cfd = cam->forwardDir();
    int camera_project_type = cam->projType();
    float scale_factor = 1.0f;
    if(camera_project_type == 0)
    {
        scale_factor = (cp - infer_result.position).norm() * 0.003f * (700.0f / cv.z);
    }
    else
    {
        scale_factor = 100.0f * (ov.y - ov.x) / cv.w;
    }

    nous::transform3 transform = nous::transform3::Identity();
    transform.translate(infer_result.position);
    transform.scale(scale_factor);
    nous::mat4 mat = transform.matrix();
    mat = set_look_at(mat, NousConvert::convert(cfd), NousConvert::convert(cud));

    // infer point
    if(infer_result.is_point() || infer_result.infer_type == InferType::TRIANGLE_POINT)
    {
        float rotate_circle_radius = 3.f;
        int rotate_line_count = 6;
        std::vector<nous::vec3> circle;
        for(int i = 0; i < rotate_line_count; ++i)
        {
            float radians = 3.141592f * 2.0f * i / rotate_line_count;
            nous::vec4 vp = nous::vec4(rotate_circle_radius * cos(radians), rotate_circle_radius * sin(radians), 0.0f, 1.0f);
            circle.emplace_back((mat * vp).head<3>() / vp.w());
        }
        nous::vec4 color = nous::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        if(infer_result.infer_type == InferType::LINE_CENTER)
        {
            color = nous::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }

        nous::vec3 circle_center = mat.col(3).head<3>() / mat.col(3).w();
        for(int i = 0; i < rotate_line_count; ++i)
        {
            m_infer_point_edge.emplace_back(edge_color, std::pair{ circle[i], circle[(i + 1) % rotate_line_count] });
            m_infer_point_tri.emplace_back(
                color, std::vector{ circle_center, m_infer_point_edge.back().second.first, m_infer_point_edge.back().second.second });
        }
    }
    else if(infer_result.infer_type == InferType::LINE || infer_result.infer_type == InferType::CURVE || infer_result.is_world_axis() ||
            infer_result.infer_type == InferType::TRIANGLE_LINE)
    {
        float size = 3.f;
        std::vector<nous::vec3> rect;
        nous::vec4 p0 = mat * nous::vec4(-size, 0.f, 0.f, 1.f);
        nous::vec4 p1 = mat * nous::vec4(0.f, -size, 0.f, 1.f);
        nous::vec4 p2 = mat * nous::vec4(size, 0.f, 0.f, 1.f);
        nous::vec4 p3 = mat * nous::vec4(0.f, size, 0.f, 1.f);

        rect.emplace_back(p0.head<3>() / p0.w());
        rect.emplace_back(p1.head<3>() / p1.w());
        rect.emplace_back(p2.head<3>() / p2.w());
        rect.emplace_back(p3.head<3>() / p3.w());

        nous::vec4 color = nous::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[0], rect[1] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[1], rect[2] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[2], rect[3] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[3], rect[0] });

        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[1], rect[2] });
        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[2], rect[3] });
    }
    else if(infer_result.infer_type == InferType::POLYGON || infer_result.infer_type == InferType::SURFACE ||
            infer_result.infer_type == InferType::TRIANGLE_FACE)
    {
        float size = 2.f;
        std::vector<nous::vec3> rect;
        nous::vec4 p0 = mat * nous::vec4(-size, -size, 0.f, 1.f);
        nous::vec4 p1 = mat * nous::vec4(-size, size, 0.f, 1.f);
        nous::vec4 p2 = mat * nous::vec4(size, size, 0.f, 1.f);
        nous::vec4 p3 = mat * nous::vec4(size, -size, 0.f, 1.f);

        rect.emplace_back(p0.head<3>() / p0.w());
        rect.emplace_back(p1.head<3>() / p1.w());
        rect.emplace_back(p2.head<3>() / p2.w());
        rect.emplace_back(p3.head<3>() / p3.w());

        nous::vec4 color = nous::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[0], rect[1] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[1], rect[2] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[2], rect[3] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[3], rect[0] });

        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[1], rect[2] });
        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[2], rect[3] });
    }

    // inference lines
    for(auto& inferLine: infer_result.inference_lines)
    {
        float patten = inferLine.first & InferType::CURRENT_LINE ? 1.0f : 0.0f;
        if(inferLine.first & InferType::AXIS_X)
            m_infer_lines.emplace_back(nous::vec4(1.0f, 0.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::AXIS_Y)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 1.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::AXIS_Z)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 0.0f, 1.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::VPC_TYPE)
            m_infer_lines.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::SEG_LINE)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 0.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else
            m_infer_lines.emplace_back(nous::vec4(1.0f, 1.0f, 1.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
    }

    // reference points
    for(auto& refer_point: infer_result.reference_points)
    {
        Engine::Vec3 cp = doc->v2d(cam->pos());
        Engine::Vec4f cv = cam->viewport();
        Engine::Vec4f ov = cam->orthoView();
        Engine::Vec3 cud = cam->upDir();
        Engine::Vec3 cfd = cam->forwardDir();
        int camera_project_type = cam->projType();
        float scale_factor = 1.0f;
        if(camera_project_type == 0)
        {
            scale_factor = (cp - NousConvert::convert(refer_point)).len() * 0.003f * (700.0f / cv.z);
        }
        else
        {
            scale_factor = 100.0f * (ov.y - ov.x) / cv.w;
        }
        nous::transform3 transform = nous::transform3::Identity();
        transform.translate(refer_point);
        transform.scale(scale_factor);
        nous::mat4 mat = transform.matrix();
        mat = set_look_at(mat, NousConvert::convert(cfd), NousConvert::convert(cud));

        float rotate_circle_radius = 3.f;
        int rotate_line_count = 6;
        std::vector<nous::vec3> circle;
        for(int i = 0; i < rotate_line_count; ++i)
        {
            float radians = 3.141592f * 2.0f * i / rotate_line_count;
            nous::vec4 vp = nous::vec4(rotate_circle_radius * cos(radians), rotate_circle_radius * sin(radians), 0.0f, 1.0f);
            circle.emplace_back((mat * vp).head<3>() / vp.w());
        }
        nous::vec4 color = nous::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        nous::vec3 circle_center = mat.col(3).head<3>() / mat.col(3).w();
        for(int i = 0; i < rotate_line_count; ++i)
        {
            m_refer_point_edge.emplace_back(edge_color, std::pair{ circle[i], circle[(i + 1) % rotate_line_count] });
            m_refer_point_tri.emplace_back(
                color, std::vector{ circle_center, m_refer_point_edge.back().second.first, m_refer_point_edge.back().second.second });
        }
    }

    // reference lines
    for(auto& refer_line: infer_result.reference_lines)
    {
        m_refer_lines.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, 1.0f), refer_line);
    }

    // reference polygons
    for(auto& refer_polygon: infer_result.reference_polygons)
    {
        std::vector<std::pair<nous::vec3, nous::vec3>> edges;
        for(auto ring: refer_polygon)
        {
            for(int i = 0; i < ring.size(); ++i)
            {
                edges.emplace_back(ring[i], ring[(i + 1) % ring.size()]);
            }
        }
        m_refer_polygons_edge.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, 1.0f), std::move(edges));
    }
}

void Controller_Main::_render_helper_mesh(View* view)
{
    // infer
    for(auto line: m_infer_lines)
    {
        uint16_t pattern = 0xffffu;
        if(line.first.w() < 1.f)
        {
            pattern = 0xf0f0u;
        }
        view->batchRenderLine(1.f, 1.f, NousConvert::convert(nous::vec3(line.first.head<3>())), NousConvert::convert(line.second.first),
                              NousConvert::convert(line.second.second), pattern, 1, false, false);
    }
    for(auto tri: m_infer_point_tri)
    {
        view->batchRenderTri(NousConvert::convert(nous::vec3(tri.first.head<3>())), NousConvert::convert(tri.second[0]),
                             NousConvert::convert(tri.second[1]), NousConvert::convert(tri.second[2]), false, true, false, 1.0f);
    }
    for(auto edge: m_infer_point_edge)
    {
        uint16_t pattern = 0xffffu;
        if(edge.first.w() < 1.f)
        {
            pattern = 0xf0f0u;
        }
        view->batchRenderLine(1.f, 1.f, NousConvert::convert(nous::vec3(edge.first.head<3>())), NousConvert::convert(edge.second.first),
                              NousConvert::convert(edge.second.second), pattern, 1, false, true);
    }

    // refer
    for(auto& edges: m_refer_polygons_edge)
    {
        uint16_t pattern = 0xffffu;
        if(edges.first.w() < 1.f)
        {
            pattern = 0xf0f0u;
        }
        for(auto& line: edges.second)
        {
            view->batchRenderLine(2.f, 1.f, NousConvert::convert(nous::vec3(edges.first.head<3>())), NousConvert::convert(line.first),
                                  NousConvert::convert(line.second), pattern, 1, false, true);
        }
    }
    for(auto line: m_refer_lines)
    {
        uint16_t pattern = 0xffffu;
        if(line.first.w() < 1.f)
        {
            pattern = 0xf0f0u;
        }
        view->batchRenderLine(2.f, 1.f, NousConvert::convert(nous::vec3(line.first.head<3>())), NousConvert::convert(line.second.first),
                              NousConvert::convert(line.second.second), pattern, 1, false, true);
    }
    for(auto tri: m_refer_point_tri)
    {
        view->batchRenderTri(NousConvert::convert(nous::vec3(tri.first.head<3>())), NousConvert::convert(tri.second[0]),
                             NousConvert::convert(tri.second[1]), NousConvert::convert(tri.second[2]), false, true, false, 1.0f);
    }
    for(auto edge: m_refer_point_edge)
    {
        uint16_t pattern = 0xffffu;
        if(edge.first.w() < 1.f)
        {
            pattern = 0xf0f0u;
        }
        view->batchRenderLine(1.f, 1.f, NousConvert::convert(nous::vec3(edge.first.head<3>())), NousConvert::convert(edge.second.first),
                              NousConvert::convert(edge.second.second), pattern, 1, false, true);
    }
}

void Controller_Main::_check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>& meshes, std::unordered_set<MeshDoc*>& ignore_meshes,
                                                         SceneDoc* scene)
{
    std::unordered_set<TFObjectDoc*> need_delete_mesh;
    std::unordered_set<MeshDoc*> already_processed_mesh;
    auto valid_merge_obj = [&already_processed_mesh, &meshes, &ignore_meshes](MeshDoc* t_mesh) {
        return already_processed_mesh.find(t_mesh) == already_processed_mesh.end() && ignore_meshes.find(t_mesh) == ignore_meshes.end() &&
               t_mesh->type() == Define::OBJ_D5_MESH;
    };
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    for(auto& change_mesh: meshes)
    {
        TFObjectDoc* current_parent = change_mesh->parent();
        if(!valid_merge_obj(static_cast<MeshDoc*>(change_mesh)))
        {
            continue;
        }
        std::vector<RenderObject*> intersect_objs;

        Engine::OBB merge_mesh_obb = change_mesh->bbox();
        merge_mesh_obb.set(merge_mesh_obb.m_min - Engine::Vec3(0.1f), merge_mesh_obb.m_max + Engine::Vec3(0.1f), merge_mesh_obb.m_matrix);

        auto& vist_objs = view->visObjects();
        for(auto& obj: vist_objs)
        {
            Engine::OBB obj_obb = obj->doc()->bbox();
            if(obj->doc()->parent() == current_parent && obj->doc() != change_mesh && valid_merge_obj(static_cast<MeshDoc*>(obj->doc())) &&
               obj_obb.intersect(merge_mesh_obb))
            {
                intersect_objs.push_back(obj);
                already_processed_mesh.insert(static_cast<MeshDoc*>(obj->doc()));
            }
        }

        if(intersect_objs.size())
        {
            MeshDoc* base_doc = static_cast<MeshDoc*>(intersect_objs[0]->doc());
            base_doc->record();
            for(int i = 1; i < intersect_objs.size(); ++i)
            {
                MeshDoc* cd = (MeshDoc*)intersect_objs[i]->doc();
                base_doc->merge_without_check(cd);
                need_delete_mesh.insert(cd);
            }
            base_doc->merge(change_mesh);
            need_delete_mesh.insert(change_mesh);
            already_processed_mesh.insert(change_mesh);

            // new obj merge
            intersect_objs.clear();
            Engine::OBB base_mesh_obb = base_doc->bbox();
            base_mesh_obb.set(base_mesh_obb.m_min - Engine::Vec3(0.1f), base_mesh_obb.m_max + Engine::Vec3(0.1f), base_mesh_obb.m_matrix);
            for(auto obj: vist_objs)
            {
                Engine::OBB obj_obb = obj->doc()->bbox();
                if(obj->doc()->parent() == current_parent && obj->doc() != base_doc && valid_merge_obj(static_cast<MeshDoc*>(obj->doc())) &&
                   obj_obb.intersect(base_mesh_obb))
                {
                    intersect_objs.push_back(obj);
                    already_processed_mesh.insert(static_cast<MeshDoc*>(obj->doc()));
                }
            }
            for(int i = 0; i < intersect_objs.size(); ++i)
            {
                MeshDoc* cd = (MeshDoc*)intersect_objs[i]->doc();
                base_doc->merge(cd);
                need_delete_mesh.insert(cd);
            }
            base_doc->dirty();
        }
    }

    for(auto delete_mesh: need_delete_mesh)
    {
        if(delete_mesh->parent()->_group())
        {
            delete_mesh->parent()->_group()->removeChild(delete_mesh);
        }
        view->delete_obj_and_try_delete_relation(delete_mesh);
    }
}

}// namespace sketch