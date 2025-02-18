#include "sketch/UV/AutoMappingUV.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/UV/Parametrizer/BLI_memarena.h"
#include "sketch/UV/Parametrizer/parametrizer.h"
#include "sketch/UV/Parametrizer/parametrizer_intern.h"

namespace sketch
{
namespace uv
{

PFace* get_start_face(ParamHandle* handle, std::vector<nous::vec3>& normals, std::vector<size_t>& visited, int triangles_num, nous::vec3& normal,
                      float thresh)
{
    PChart* chart;
    int i;
    PHandle* phandle = (PHandle*)handle;

    PFace* s = NULL;
    double max = -100.0;

    for(i = 0; i < phandle->ncharts; i++)
    {
        chart = phandle->charts[i];
        PFace* f;

        for(f = chart->faces; f; f = f->nextlink)
        {

            int index = f->idx;
            if(visited[index])
                continue;
            double t = normal.dot(normals[index]);
            if(t > thresh && t > max)
            {
                s = f;
                max = t;
            }
        }
    }

    return s;
}

ParamHandle* construct_param_handle_from_data(std::vector<nous::pos_t>& vertices, const std::vector<nous::vec3i>& triangles,
                                              std::vector<nous::vec3>& normals, bool fill = false, bool implicit = false)
{
    const size_t triangles_num = triangles.size();
    const size_t vertices_num = vertices.size();

    ParamHandle* handle;
    handle = param_construct_begin();

    for(int i = 0; i < triangles_num; ++i)
    {
        ParamKey key, vkeys[4];
        bool pin[4], select[4];// was ParamBool
        float* co[4];
        float* uv[4];
        int nverts = 3;

        key = i;
        vkeys[0] = triangles[i].x();
        vkeys[1] = triangles[i].y();
        vkeys[2] = triangles[i].z();

        co[0] = vertices[vkeys[0]].data();
        co[1] = vertices[vkeys[1]].data();
        co[2] = vertices[vkeys[2]].data();

        uv[0] = nullptr;
        uv[1] = nullptr;
        uv[2] = nullptr;

        pin[0] = false;
        pin[1] = false;
        pin[2] = false;

        select[0] = false;
        select[1] = false;
        select[2] = false;

        param_face_add(handle, key, nverts, vkeys, co, uv, pin, select);
        normals[i] = sketch_math::ring_normal({ vertices[vkeys[0]], vertices[vkeys[1]], vertices[vkeys[2]] });
    }

    param_construct_data_end(handle, fill != 0, implicit != 0);

    return handle;
}

ParamHandle* construct_new_param_handle(ParamHandle* old_handle, std::vector<nous::pos_t>& vertices, const std::vector<nous::vec3i>& triangles,
                                        std::vector<size_t>& visited, bool fill = false, bool implicit = false)
{
    size_t triangles_num = triangles.size();
    size_t vertices_num = vertices.size();

    ParamHandle* handle;
    handle = param_construct_begin();

    PChart* chart;
    PHandle* phandle = (PHandle*)old_handle;

    for(int i = 0; i < phandle->ncharts; i++)
    {
        chart = phandle->charts[i];
        PFace* f;
        for(f = chart->faces; f; f = f->nextlink)
        {

            ParamKey key, vkeys[4];
            bool pin[4], select[4];// was ParamBool
            float* co[4];
            float* uv[4];
            int nverts = 3;

            int a = f->idx;

            key = (ParamKey)a;
            vkeys[0] = (ParamKey)triangles[a].x();
            vkeys[1] = (ParamKey)triangles[a].y();
            vkeys[2] = (ParamKey)triangles[a].z();

            co[0] = vertices[vkeys[0]].data();
            co[1] = vertices[vkeys[1]].data();
            co[2] = vertices[vkeys[2]].data();

            uv[0] = NULL;
            uv[1] = NULL;
            uv[2] = NULL;

            pin[0] = false;
            pin[1] = false;
            pin[2] = false;

            select[0] = false;
            select[1] = false;
            select[2] = false;

            param_face_add(handle, key, nverts, vkeys, co, uv, pin, select);
        }
    }

    for(int i = 0; i < phandle->ncharts; i++)
    {
        chart = phandle->charts[i];
        PEdge* e;

        for(e = chart->edges; e; e = e->nextlink)
        {
            PFace* f = e->face;
            if(e->pair)
            {
                PFace* pair_f = e->pair->face;

                int f_index = e->face->idx;
                int pair_f_index = e->pair->face->idx;

                if(visited[f_index] != visited[pair_f_index])
                {

                    ParamKey vkeys[2];
                    vkeys[0] = (ParamKey)e->vert->u.key;
                    vkeys[1] = (ParamKey)e->pair->vert->u.key;
                    param_edge_set_seam(handle, vkeys);
                }
            }
        }
    }

    param_construct_end(handle, fill != 0, implicit != 0);

    return handle;
}

void chart_projection(ParamHandle* handle, const std::vector<nous::vec3>& chart_normals, std::vector<size_t>& visited)
{
    PChart* chart;
    PHandle* phandle = (PHandle*)handle;

    nous::vec3 positive_x_direction(1.0, 0.0, 0.0);
    nous::vec3 positive_y_direction(0.0, 1.0, 0.0);

    for(int i = 0; i < phandle->ncharts; i++)
    {
        chart = phandle->charts[i];
        PFace* f;
        int index = chart->faces->idx;
        int chart_indexs = visited[index];

        nous::vec3 n_chart = chart_normals[chart_indexs];
        nous::vec3 n_x, n_y;
        n_x = positive_x_direction.cross(n_chart);
        if(n_x.norm() < 1e-5)
        {
            n_x = positive_y_direction;
        }
        n_x = n_x.normalized();

        n_y = n_x.cross(n_chart);
        n_y = n_y.normalized();

        for(f = chart->faces; f; f = f->nextlink)
        {
            int index = f->idx;
            int chart_index = visited[index];
            if(chart_index != chart_indexs)
                break;

            PEdge *e1 = f->edge, *e2 = e1->next, *e3 = e2->next;
            PVert *v1 = e1->vert, *v2 = e2->vert, *v3 = e3->vert;

            nous::vec3 v_vector(v1->co[0], v1->co[1], v1->co[2]);
            float u_x = v_vector.dot(n_x);
            float u_y = v_vector.dot(n_y);
            if(!(v1->flag & PVERT_VISIT))
            {
                v1->uv[0] = u_x;
                v1->uv[1] = u_y;
                v1->flag |= PVERT_VISIT;
            }

            v_vector = nous::vec3(v2->co[0], v2->co[1], v2->co[2]);
            u_x = v_vector.dot(n_x);
            u_y = v_vector.dot(n_y);
            if(!(v2->flag & PVERT_VISIT))
            {
                v2->uv[0] = u_x;
                v2->uv[1] = u_y;
                v2->flag |= PVERT_VISIT;
            }

            v_vector = nous::vec3(v3->co[0], v3->co[1], v3->co[2]);
            u_x = v_vector.dot(n_x);
            u_y = v_vector.dot(n_y);
            if(!(v3->flag & PVERT_VISIT))
            {
                v3->uv[0] = u_x;
                v3->uv[1] = u_y;
                v3->flag |= PVERT_VISIT;
            }
        }
    }
}

std::vector<DecomposedSubMesh> auto_mapping_trimesh_uv(std::vector<nous::pos_t>& vertices, const std::vector<nous::vec3i>& triangles,
                                                       const bool use_LSCM, const nous::scalar angle, const nous::scalar margin)
{
    const size_t triangles_num = triangles.size();
    std::vector<nous::vec3> normals(triangles_num, nous::vec3::Identity());
    ParamHandle* handle = construct_param_handle_from_data(vertices, triangles, normals, false, 0);

    std::vector<size_t> visited(triangles_num, 0);

    const nous::vec3 x_axis(1.0, 0.0, 0.0);
    const nous::vec3 y_axis(0.0, 1.0, 0.0);
    const nous::vec3 z_axis(0.0, 0.0, 1.0);

    nous::vec3 xyz_axis[6];
    xyz_axis[0] = x_axis;
    xyz_axis[1] = x_axis * -1.0;
    xyz_axis[2] = y_axis;
    xyz_axis[3] = y_axis * -1.0;
    xyz_axis[4] = z_axis;
    xyz_axis[5] = z_axis * -1.0;

    int d = 0;
    nous::vec3 normal;
    PFace* start_face = NULL;

    nous::scalar thresh = static_cast<nous::scalar>(cos(2 * sketch_math::sketch_pi * angle / 360.0));
    while(d < 6)
    {
        start_face = get_start_face(handle, normals, visited, triangles_num, xyz_axis[d], thresh);
        if(start_face != NULL)
        {
            normal = xyz_axis[d];
            d++;
            break;
        }
        d++;
    }

    if(start_face == NULL)
    {
        start_face = get_start_face(handle, normals, visited, triangles_num, xyz_axis[0], -10.0);
        normal = normals[start_face->idx];
    }

    std::vector<nous::vec3> chart_normal;//(chart_num,VEC3(0.0,0.0,0.0));
    chart_normal.push_back(nous::vec3::Zero());
    chart_normal.push_back(normal);

    int chart_num = 1;

    while(start_face != NULL)
    {
        int index = start_face->idx;
        if(visited[index])
        {
            continue;
        }

        visited[index] = chart_num;
        normal = normal.normalized();

        std::list<PFace*> faces;
        faces.push_back(start_face);

        while(!faces.empty())
        {
            PFace* f = faces.front();
            faces.pop_front();
            int index_f = f->idx;

            PEdge* e = f->edge;
            do
            {
                PEdge* pair_e = e->pair;
                e = e->next;
                PEdge* we = pair_e;

                if(pair_e)
                {
                    PFace* pair_f = pair_e->face;
                    int pair_f_index = pair_f->idx;
                    if(visited[pair_f_index])
                        continue;

                    nous::vec3 normal_f = normals[pair_f_index];

                    if(normal_f.dot(normal) > thresh)
                    {

                        faces.push_front(pair_f);
                        visited[pair_f_index] = chart_num;
                    }
                }

            } while(e && e != f->edge);
        }

        if(d == 6)
        {
            d = 0;
        }

        PFace* new_start_face = NULL;

        while(d < 6)
        {
            new_start_face = get_start_face(handle, normals, visited, triangles_num, xyz_axis[d], thresh);
            if(new_start_face != NULL)
            {
                normal = xyz_axis[d];
                d++;
                break;
            }
            d++;
        }

        if(d == 6 && new_start_face == NULL)
        {
            new_start_face = get_start_face(handle, normals, visited, triangles_num, xyz_axis[0], -10.0);
            if(new_start_face != NULL)
            {
                normal = normals[new_start_face->idx];
            }
        }

        start_face = new_start_face;
        chart_normal.push_back(normal);
        chart_num++;
    }

    std::vector<std::vector<int>> chart_index;
    chart_index.resize(chart_num);

    for(int index = 0; index < triangles_num; index++)
    {
        chart_index[visited[index]].push_back(index);
    }

    for(int index = 1; index < chart_num; index++)
    {
        chart_normal[index] = chart_normal[index].normalized();
    }

    ParamHandle* new_handle = construct_new_param_handle(handle, vertices, triangles, visited);

    if(!use_LSCM)
    {
        chart_projection(new_handle, chart_normal, visited);
    }
    else
    {

        param_lscm_begin(new_handle, PARAM_FALSE, true);
        param_lscm_solve(new_handle);
        param_lscm_end(new_handle);
    }

    param_pack(new_handle, margin);
    param_flush(new_handle);

    std::vector<DecomposedSubMesh> result;
    // 解析 UV 出来
    int index = 0;
    PChart* chart;
    PHandle* phandle = (PHandle*)new_handle;
    for(int i = 0; i < phandle->ncharts; i++)
    {
        DecomposedSubMesh submesh_info;
        chart = phandle->charts[i];

        PVert* v;
        for(v = chart->verts; v; v = v->nextlink)
        {
            if(v->uv)
            {
                nous::vec2 tc(v->uv[0], v->uv[1]);
                submesh_info.uvs.emplace_back(tc);
                submesh_info.vertices.emplace_back(v->co[0], v->co[1], v->co[2]);
                v->u.id = index;
                index++;
            }
        }

        PFace* f;
        for(f = chart->faces; f; f = f->nextlink)
        {
            PEdge *e1 = f->edge, *e2 = e1->next, *e3 = e2->next;
            PVert *v1 = e1->vert, *v2 = e2->vert, *v3 = e3->vert;

            int uv1_idx = v1->u.id + 1;
            int uv2_idx = v2->u.id + 1;
            int uv3_idx = v3->u.id + 1;

            int a = f->idx;
            submesh_info.triangles.emplace_back(nous::vec3i(uv1_idx - 1, uv2_idx - 1, uv3_idx - 1));
        }

        result.emplace_back(submesh_info);
    }

    param_delete(handle);
    param_delete(new_handle);

    return result;
}

};// namespace uv
};// namespace sketch
