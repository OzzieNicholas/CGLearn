#include "sketch/View/Select/ElementSelector.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"

#include "nous_mesh/algorithm/intersect.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/View/RenderRes/NurbsData.h"

namespace sketch
{
ElementSelector::ElementSelector() : m_select_pixel_range(6 * 6), m_select_mode(SelectMode::NORMAL) {}

ElementSelector::~ElementSelector() {}

void ElementSelector::update(View* pView, int x, int y)
{
    if(!pView)
        return;

    RenderObject* obj = nullptr;
    SketchElementPtr ele = nullptr;
    _get_select_info(pView, x, y, &obj, &ele);
    if(ele && ele->group())
    {
        ele = ele->group();
    }

    if(m_select_mode == SelectMode::NORMAL)
    {
        m_selectInfo.clear();
    }
    if(obj)
    {
        if(m_select_mode == (SelectMode::ADD | SelectMode::SUB))
        {
            m_selectInfo[obj].mesh = obj;
            if(m_selectInfo[obj].elements.insert(ele).second == false)
            {
                m_selectInfo[obj].elements.erase(ele);
            }
        }
        else if(m_select_mode == SelectMode::SUB)
        {
            m_selectInfo[obj].mesh = obj;
            m_selectInfo[obj].elements.erase(ele);
        }
        else if(m_select_mode == SelectMode::NORMAL || m_select_mode == SelectMode::ADD)
        {
            m_selectInfo[obj].mesh = obj;
            m_selectInfo[obj].elements.insert(ele);
        }
    }
}

void ElementSelector::update(View* pView, int minx, int miny, int maxx, int maxy, bool containOnly)
{
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    ViewCamera* pCam = pView->camera();
    const Engine::Vec4f& viewport = pCam->viewport();
    nous::vec2 vp0 = NousConvert::convert((Engine::Vec2f(minx, miny) - viewport.xy) / viewport.zw * 2.0f - 1.0f);
    nous::vec2 vp1 = NousConvert::convert((Engine::Vec2f(maxx, maxy) - viewport.xy) / viewport.zw * 2.0f - 1.0f);

    auto& objs = pView->visObjects();
    if(m_select_mode == SelectMode::NORMAL)
    {
        m_selectInfo.clear();
    }

    for(auto& pObj: objs)
    {
        Mesh* pMesh = dynamic_cast<Mesh*>(pObj);
        std::unordered_set<SketchElementPtr> select_ele;
        if(pMesh && pMesh->rectangle_intersect_view_ele(vp0, vp1, containOnly, select_ele))
        {
            m_selectInfo[pObj].mesh = pObj;
            if(m_select_mode == (SelectMode::ADD | SelectMode::SUB))
            {
                for(auto& ele: select_ele)
                {
                    SketchElementPtr valid_ele = ele->group() ? ele->group() : ele;
                    if(m_selectInfo[pObj].elements.erase(valid_ele) == 0)
                    {
                        m_selectInfo[pObj].elements.insert(valid_ele);
                    }
                }
            }
            else if(m_select_mode == SelectMode::SUB)
            {
                for(auto& ele: select_ele)
                {
                    SketchElementPtr valid_ele = ele->group() ? ele->group() : ele;
                    m_selectInfo[pObj].elements.erase(valid_ele);
                }
            }
            else if(m_select_mode == SelectMode::NORMAL || m_select_mode == SelectMode::ADD)
            {
                for(auto& ele: select_ele)
                {
                    SketchElementPtr valid_ele = ele->group() ? ele->group() : ele;
                    m_selectInfo[pObj].elements.insert(valid_ele);
                }
            }
        }
    }
}

SelectInfo ElementSelector::get_select_info(View* view, int x, int y)
{
    RenderObject* obj = nullptr;
    SketchElementPtr ele = nullptr;
    _get_select_info(view, x, y, &obj, &ele);

    if(obj)
    {
        SelectInfo info;
        info.mesh = obj;
        info.elements.insert(ele);
        return info;
    }
    return SelectInfo();
}

void ElementSelector::render_obj_changed(Define::Message method, RenderObject* obj)
{
    switch(method)
    {
    case Define::MSG_OBJ_DELETE: {
        auto& pos = m_selectInfo.find(obj);
        if(pos != m_selectInfo.end())
        {
            m_selectInfo.erase(pos);
        }
    }
    break;
    case Define::MSG_OBJ_UPDATE: {
        auto& pos = m_selectInfo.find(obj);
        if(pos != m_selectInfo.end())
        {
            std::unordered_set<SketchElementPtr> invalid_ele;
            if(obj->type() == Define::OBJ_D5_MESH)
            {
                MeshDoc* mesh_doc = static_cast<MeshDoc*>(obj->doc());
                auto geo_data = mesh_doc->geodata();

                for(auto ele: pos->second.elements)
                {
                    if(!geo_data->is_valid(ele))
                    {
                        invalid_ele.insert(ele);
                    }
                }
            }
            else if(obj->doc()->type() == Define::ObjectType::OBJ_D5_NURBS)
            {
                NurbsDoc* mesh_doc = static_cast<NurbsDoc*>(obj->doc());
                auto nurbs_data = mesh_doc->nurbsdata();

                for(auto lk: pos->second.elements)
                {
                    const bool is_exist = nurbs_data->item_exist(lk->id());
                    if(!is_exist)
                    {
                        invalid_ele.insert(lk);
                    }
                }
            }

            for(auto lk: invalid_ele)
            {
                pos->second.elements.erase(lk);
            }

            if(!pos->second.elements.size() && !pos->second.elements.size())
            {
                m_selectInfo.erase(pos);
            }
        }
    }
    break;
    }
}

void ElementSelector::select_filter_view_group_area(View* view)
{
    TFObjectDoc* current_group_area = nullptr;
    if(view->working_group_area())
    {
        current_group_area = view->working_group_area();
    }

    std::unordered_set<RenderObject*> valid_group_objs;
    std::unordered_set<RenderObject*> invalid_objs;
    for(auto& select_mesh: m_selectInfo)
    {
        TFObjectDoc* current = select_mesh.first->doc();
        while(current)
        {
            if(current->parent() == current_group_area)
            {
                break;
            }
            current = current->parent();
        }
        if(!current)// 在当前group area外
        {
            invalid_objs.insert(select_mesh.first);
        }
        else if(current != select_mesh.first->doc())// 在当前group area内的子节点中，需要加入group area次级节点，删除自身
        {
            valid_group_objs.insert(current->renderObject());
            invalid_objs.insert(select_mesh.first);
        }
        else
        {
            // 自身为group area次级节点，可以被直接选中，什么都不需要做。
        }
    }
    for(auto& obj: invalid_objs)
    {
        m_selectInfo.erase(obj);
    }
    for(auto& obj: valid_group_objs)
    {
        m_selectInfo[obj].mesh = obj;
    }
}

void ElementSelector::_get_select_info(View* view, int x, int y, RenderObject** pick_obj, SketchElementPtr* pick_ele)
{
    ViewCamera* cam = view->camera();
    auto& vp = cam->viewport();
    nous::mat4 cam_vp_mat = NousConvert::convert(cam->dvpMatrix());
    Engine::Vec3 o, d;
    Document* doc = STCore::inst->docMgr()->document();
    cam->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &o, &d);

    auto& objs = view->visObjects();
    std::vector<Mesh*> ray_cast_objs;
    std::vector<std::pair<nous::vec3, nous::vec3>> ray_cast_objs_local_ray;
    for(auto& obj: objs)
    {
        if(obj->doc()->type() != Define::OBJ_D5_MESH && obj->doc()->type() != Define::OBJ_D5_NURBS)
        {
            continue;
        }
        // check bbox first
        Engine::Vec3 bbox_size = obj->doc()->bbox().sizeW();
        if(!obj->doc()->bbox().intersectRay(o, d, Engine::Vec3((bbox_size.x + bbox_size.y + bbox_size.z) * 0.2)))
            continue;

        auto& inv_world_mat = obj->doc()->world_matrix_inv();
        Engine::Vec3 local_o = inv_world_mat * o;
        Engine::Vec3 local_v = (inv_world_mat * d - inv_world_mat.pos).normalize();

        Mesh* mesh = static_cast<Mesh*>(obj);
        ray_cast_objs.push_back(mesh);
        ray_cast_objs_local_ray.emplace_back(NousConvert::convert(local_o), NousConvert::convert(local_v));
    }

    auto valid_select = [&view](double closest_pixel_dist, Mesh* select_mesh, double select_distance) -> bool {
        bool replace = false;
        double d = closest_pixel_dist - select_distance;
        if(d > -sketch_math::sketch_epsilon)
        {
            if(select_mesh && abs(d) < sketch_math::sketch_epsilon)
            {
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

    Mesh* select_line_mesh = nullptr;
    SketchElementPtr select_line_ele = nullptr;
    nous::vec3 select_line_pos = nous::vec3::Zero();
    double closest_pixel_dist = DBL_MAX;

    for(int i = 0; i < ray_cast_objs.size(); ++i)
    {
        double pixel_distance = DBL_MAX;
        nous::vec3 pos;
        nous::vec3 tangent;
        SketchElementPtr line = nullptr;
        if((pixel_distance = ray_cast_objs[i]->ray_intersect_view_line(ray_cast_objs_local_ray[i].first, ray_cast_objs_local_ray[i].second,
                                                                       nous::vec2(x, y), nous::vec2(vp.z, vp.w), cam_vp_mat, pos, tangent, &line)) <
           m_select_pixel_range)
        {
            if(valid_select(closest_pixel_dist, ray_cast_objs[i], pixel_distance))
            {
                closest_pixel_dist = pixel_distance;
                select_line_pos = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), pos);
                select_line_mesh = ray_cast_objs[i];
                select_line_ele = line;
            }
        }
    }

    Mesh* select_face_mesh = nullptr;
    SketchElementPtr select_face_ele = nullptr;
    int select_polygon_vid = -1;
    double closest_dist = DBL_MAX;

    for(int i = 0; i < ray_cast_objs.size(); ++i)
    {
        size_t closest_id = -1;
        nous::vec3 pos;
        nous::vec3 normal;
        SketchElementPtr face = nullptr;
        if(ray_cast_objs[i]->ray_intersect_face(ray_cast_objs_local_ray[i].first, ray_cast_objs_local_ray[i].second, pos, normal, &face))
        {
            pos = NousConvert::product_v3(ray_cast_objs[i]->doc()->world_matrix(), pos);
            float distance = (NousConvert::convert(o) - pos).norm();
            if(valid_select(closest_dist, ray_cast_objs[i], distance))
            {
                closest_dist = distance;
                select_face_mesh = ray_cast_objs[i];
                select_polygon_vid = i;
                select_face_ele = face;
            }
        }
    }

    //
    if(select_line_mesh && select_face_mesh)
    {
        nous::vec3 vo = ray_cast_objs_local_ray[select_polygon_vid].first;
        nous::vec3 vd = (select_line_pos - NousConvert::convert(o)).normalized();
        auto& inv_world_mat = select_face_mesh->doc()->world_matrix_inv();
        vd = (NousConvert::product_v3(inv_world_mat, vd) - NousConvert::convert(inv_world_mat.pos)).normalized();

        nous::vec3 pos;
        nous::vec3 normal;
        SketchElementPtr cp = nullptr;
        float el = FLT_MAX;
        if(select_face_mesh->ray_intersect_face(vo, vd, pos, normal, &cp))
        {
            pos = NousConvert::product_v3(select_face_mesh->doc()->world_matrix(), pos);
            el = (NousConvert::convert(o) - pos).norm();
        }
        float vl = (NousConvert::convert(o) - select_line_pos).norm();
        if(vl - el > 1e-2)
        {
            *pick_obj = select_face_mesh;
            *pick_ele = select_face_ele;
        }
        else
        {
            *pick_obj = select_line_mesh;
            *pick_ele = select_line_ele;
        }
    }
    else if(select_line_mesh)
    {
        *pick_obj = select_line_mesh;
        *pick_ele = select_line_ele;
    }
    else if(select_face_mesh)
    {
        *pick_obj = select_face_mesh;
        *pick_ele = select_face_ele;
    }
}
}// namespace sketch