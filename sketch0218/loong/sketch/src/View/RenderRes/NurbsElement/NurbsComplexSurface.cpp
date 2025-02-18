#include "sketch/View/RenderRes/NurbsElement/NurbsComplexSurface.h"
#include "CDT.h"
#include "VerifyTopology.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/UV/AutoMappingUV.h"
#include "sketch/View/RenderRes/IRenderable.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderRes/NurbsElement/INurbsBase.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"
#include <iostream>

namespace sketch
{

void NurbsTrimmedSurface::generate_render_mesh(IRenderable* render_data)
{
    _sample_surface();
    std::vector<float>& position = render_data->get_render_position();
    std::vector<float>& normal = render_data->get_render_normal();
    std::vector<unsigned int>& lines = render_data->get_render_lines_index();
    std::vector<float>& uv = render_data->get_render_uv();
    std::vector<unsigned int>& triangles = render_data->get_render_triangles_index();
    std::vector<SubmeshData>& submeshes = render_data->get_render_submesh();

    if(!m_sampled_triangle_ref_idx_arr.empty())
    {
        const size_t base_position_idx = position.size() / 3;
        const size_t base_triangle_idx = triangles.size();
        const size_t base_line_idx = lines.size();

        for(int i = 0; i < m_unique_sampled_point_arr.size(); ++i)
        {
            position.emplace_back(m_unique_sampled_point_arr[i].x());
            position.emplace_back(m_unique_sampled_point_arr[i].y());
            position.emplace_back(m_unique_sampled_point_arr[i].z());
            normal.emplace_back(m_unique_sampled_normal_arr[i].x());
            normal.emplace_back(m_unique_sampled_normal_arr[i].y());
            normal.emplace_back(m_unique_sampled_normal_arr[i].z());
            if(!m_unique_sampled_uv_arr.empty())
            {
                uv.emplace_back(m_unique_sampled_uv_arr[i].x());
                uv.emplace_back(m_unique_sampled_uv_arr[i].y());
            }
        }

        for(const auto& tri: m_sampled_triangle_ref_idx_arr)
        {
            triangles.emplace_back(tri.x() + base_position_idx);
            triangles.emplace_back(tri.y() + base_position_idx);
            triangles.emplace_back(tri.z() + base_position_idx);
        }

        submeshes.emplace_back("Polygon", base_triangle_idx, triangles.size() - base_triangle_idx, SubmeshData::Topology::Triangles,
                               this->shared_from_this());
        return;
    }

    for(const auto& sub_edge_element_info: m_complex_surface_elements)
    {
        const auto& bounding_line = sub_edge_element_info.second;
        // position
        size_t begin_vertex = position.size() / 3;
        for(int i = 0; i < bounding_line.size(); ++i)
        {
            position.push_back(bounding_line[i].x());
            position.push_back(bounding_line[i].y());
            position.push_back(bounding_line[i].z());
            normal.push_back(0.0);
            normal.push_back(0.0);
            normal.push_back(0.0);
            uv.push_back(0.0);
            uv.push_back(0.0);
        }
        size_t end_vertex = position.size() / 3;
        // index
        size_t begin_index = lines.size();
        for(int i = begin_vertex + 1; i < end_vertex; ++i)
        {
            lines.push_back(i - 1);
            lines.push_back(i);
        }

        submeshes.emplace_back("Line", begin_index, lines.size() - begin_index, SubmeshData::Topology::Lines, sub_edge_element_info.first);
    }

    return;
}

nous::AABB3 NurbsTrimmedSurface::get_bounding_box()
{
    // 返回外环的 bounding box 就可以了
    return m_exterior_ring->get_bounding_box();
}

void NurbsTrimmedSurface::transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat)
{
    m_exterior_ring->transform(transformed_set, trans_mat);

    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        m_interior_rings[i]->transform(transformed_set, trans_mat);
    }
}

void NurbsTrimmedSurface::save(SketchSerializer& serializer)
{
    SketchElement::save(serializer);
    serializer.write_sketch_nurbs_item_ref(m_exterior_ring, "exring");
    serializer.begin_write_array("inrings");
    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        serializer.write_sketch_nurbs_item_ref(m_interior_rings[i]);
    }
    serializer.end_write_array();
}

void NurbsTrimmedSurface::load(SketchSerializer& serializer)
{
    SketchElement::load(serializer);

    m_exterior_ring = serializer.read_sketch_nurbs_item_ref("exring")->to_nurbs_surface_base();
    m_exterior_ring->to_nurbs_algebra_surface()->set_parent_as_exring(shared_from_this()->to_nurbs_trimmed_surface());

    size_t interior_ring_count = 0;
    serializer.begin_read_array("inrings", interior_ring_count);
    for(int i = 0; i < interior_ring_count; ++i)
    {
        auto& inring = serializer.read_sketch_nurbs_item_ref()->to_nurbs_surface_base();
        inring->set_parent_as_inring(shared_from_this()->to_nurbs_trimmed_surface());
        m_interior_rings.emplace_back(inring);
    }
    serializer.end_read_array();
}

bool NurbsTrimmedSurface::is_valid()
{
    if(m_exterior_ring)
    {
        return true;
    }
    return false;
}

std::shared_ptr<INurbsBase> NurbsTrimmedSurface::clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                                       const nous::mat4& trans_mat)
{
    std::shared_ptr<NurbsTrimmedSurface> clone_complex_surface = std::make_shared<NurbsTrimmedSurface>();

    clone_complex_surface->set_exterior_ring(m_exterior_ring->clone(remaped_map, trans_mat)->to_nurbs_surface_base());

    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        clone_complex_surface->add_interior_ring(m_interior_rings[i]->clone(remaped_map, trans_mat)->to_nurbs_surface_base());
    }

    return clone_complex_surface;
}

std::vector<std::vector<SketchNurbsPointPtr>> NurbsTrimmedSurface::get_surface_control_points()
{
    std::vector<std::vector<SketchNurbsPointPtr>> result;
    if(m_exterior_ring)
    {
        auto&& exring_ctp_arr = m_exterior_ring->get_surface_control_points_1D();
        result.emplace_back(exring_ctp_arr);
    }

    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        auto&& inring_ctp_arr = m_interior_rings[i]->get_surface_control_points_1D();
        result.emplace_back(inring_ctp_arr);
    }

    return result;
}

std::vector<SketchNurbsPointPtr> NurbsTrimmedSurface::get_surface_control_points_1D()
{
    std::vector<SketchNurbsPointPtr> result;
    if(m_exterior_ring)
    {
        auto&& exring_ctp_arr = m_exterior_ring->get_surface_control_points_1D();
        result.insert(result.end(), exring_ctp_arr.begin(), exring_ctp_arr.end());
    }

    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        auto&& inring_ctp_arr = m_interior_rings[i]->get_surface_control_points_1D();
        result.insert(result.end(), inring_ctp_arr.begin(), inring_ctp_arr.end());
    }

    return result;
}

std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> NurbsTrimmedSurface::get_complex_surface_elements()
{
    _sample_surface();
    return m_complex_surface_elements;
}

void NurbsTrimmedSurface::get_surface_sampled_triangles(std::vector<nous::vec3>& sampled_point_arr, std::vector<nous::vec3i>& triangle_idx_arr)
{
    _sample_surface();
    sampled_point_arr.insert(sampled_point_arr.end(), m_unique_sampled_point_arr.begin(), m_unique_sampled_point_arr.end());
    triangle_idx_arr.insert(triangle_idx_arr.end(), m_sampled_triangle_ref_idx_arr.begin(), m_sampled_triangle_ref_idx_arr.end());
}

void NurbsTrimmedSurface::get_surface_sampled_triangles_without_uv_split(std::vector<nous::vec3>& sampled_point_arr,
                                                                         std::vector<nous::vec3i>& triangle_idx_arr)
{
    _sample_surface();
    sampled_point_arr.insert(sampled_point_arr.end(), m_unique_sampled_point_arr_without_uv_split.begin(),
                             m_unique_sampled_point_arr_without_uv_split.end());
    triangle_idx_arr.insert(triangle_idx_arr.end(), m_sampled_triangle_ref_idx_arr_without_uv_split.begin(),
                            m_sampled_triangle_ref_idx_arr_without_uv_split.end());
}

void NurbsTrimmedSurface::set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) {}

void NurbsTrimmedSurface::set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) {}

void NurbsTrimmedSurface::set_basic_algebra_surface(std::shared_ptr<NurbsAlgebraSurface> algebra_surface)
{
    m_basic_algebra_surface = algebra_surface;
}

NurbsAlgebraSurfacePtr NurbsTrimmedSurface::get_basic_algebra_surface() const { return m_basic_algebra_surface; }

size_t NurbsTrimmedSurface::get_inring_number() const { return m_interior_rings.size(); }

EElementType NurbsTrimmedSurface::get_surface_exring_type() const
{
    if(m_exterior_ring)
    {
        return m_exterior_ring->type();
    }
    return EElementType::None;
}

bool NurbsTrimmedSurface::is_surface_coplane(nous::vec3& coplane_normal)
{
    nous::vec3 base_normal;

    // 可能存在的基准参数曲面，若不是共面的直接返回
    if(m_basic_algebra_surface)
    {
        const bool basic_surface_coplane = m_basic_algebra_surface->is_surface_coplane(base_normal);
        if(!basic_surface_coplane)
        {
            return false;
        }
    }

    // 内环和外环的所有控制点共面
    // 如果有内环先简单检测环的 normal 是否近似平行
    if(!m_exterior_ring)
    {
        return false;
    }

    const bool is_exring_coplane = m_exterior_ring->is_surface_coplane(base_normal);
    if(!is_exring_coplane)
    {
        return false;
    }

    bool all_coplane = true;
    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        nous::vec3 inring_normal;
        bool is_inring_coplane = m_interior_rings[i]->is_surface_coplane(inring_normal);
        bool is_exring_inring_parallel = sketch_math::direction_parallel(base_normal, inring_normal);

        if(!is_inring_coplane || !is_exring_inring_parallel)
        {
            all_coplane = false;
            break;
        }
    }

    return all_coplane;
}

void NurbsTrimmedSurface::get_sampled_triangles_low_resolution_when_exring_algebra_surface(unsigned int rows, unsigned int cols,
                                                                                           std::vector<nous::vec3>& sampled_point_arr,
                                                                                           std::vector<nous::vec3i>& triangle_idx_arr)
{
    if(m_exterior_ring->type() != EElementType::AlgebraSurface)
    {
        return;
    }

    NurbsAlgebraSurfacePtr algebra_surface = m_exterior_ring->to_nurbs_algebra_surface();
    if(algebra_surface)
    {
        auto&& simple_sampled_point_mat = algebra_surface->get_sampled_point_arr(rows, cols);
        for(int i = 0; i < rows - 1; ++i)
        {
            for(int j = 0; j < cols - 1; ++j)
            {
                const size_t p0 = i * cols + j;
                const size_t p1 = i * cols + (j + 1);
                const size_t p2 = (i + 1) * cols + j;
                const size_t p3 = (i + 1) * cols + (j + 1);

                triangle_idx_arr.emplace_back(p0, p1, p2);
                triangle_idx_arr.emplace_back(p1, p3, p2);
            }
        }

        for(int i = 0; i < rows; ++i)
        {
            sampled_point_arr.insert(sampled_point_arr.end(), simple_sampled_point_mat[i].begin(), simple_sampled_point_mat[i].end());
        }
    }
}

void NurbsTrimmedSurface::_sample_surface()
{
    if(!m_dirty)
    {
        return;
    }

    _generate_sub_elements();
    _generate_render_resources();
}

void NurbsTrimmedSurface::_generate_sub_elements()
{
    m_complex_surface_elements.clear();

    _generate_element_by_subsurface(m_complex_surface_elements, m_exterior_ring);

    for(int m = 0; m < m_interior_rings.size(); ++m)
    {
        auto& inring = m_interior_rings[m];
        _generate_element_by_subsurface(m_complex_surface_elements, inring);
    }
}

void NurbsTrimmedSurface::_generate_element_by_subsurface(std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>>& complex_surface_elements,
                                                          INurbsSurfaceBasePtr ring)
{
    // 这个更细一点，每个内部面都要拆分到曲线层面
    if(ring->type() == EElementType::ClosedSurface)
    {
        auto& closed_surface = ring->to_nurbs_closed_surface();
        const auto&& closed_surface_elements = closed_surface->get_surface_elements();
        for(auto& pair_info: closed_surface_elements)
        {
            complex_surface_elements.insert({ pair_info.first, pair_info.second });
        }
    }
    else if(ring->type() == EElementType::AlgebraSurface)
    {
        auto& algebra_surface = ring->to_nurbs_algebra_surface();
        const auto&& algebra_surface_elements = algebra_surface->get_surface_elements();
        for(auto& pair_info: algebra_surface_elements)
        {
            complex_surface_elements.insert({ pair_info.first, pair_info.second });
        }
    }
}

void NurbsTrimmedSurface::_generate_render_resources()
{
    m_unique_sampled_point_arr.clear();
    m_unique_sampled_normal_arr.clear();
    m_unique_sampled_uv_arr.clear();
    m_sampled_triangle_ref_idx_arr.clear();
    m_unique_sampled_point_arr_without_uv_split.clear();
    m_sampled_triangle_ref_idx_arr_without_uv_split.clear();

    if(m_exterior_ring->type() == EElementType::ClosedSurface)
    {
        // 外环和内环，分情况讨论
        // 基于 CDT 的三角化逻辑应该可以应付这种情况
        // NurbsSurface 的边界是限制边，NurbsCurveBRep 的边界就是边界
        CDT::EdgeVec constrained_edges;

        auto&& process_line_to_CDT_edge = [&](const std::vector<nous::pos_t>& boundary_line) -> void {
            unsigned int last_pt_ref_idx = -1;
            unsigned int first_pt_ref_idx = -1;
            for(int i = 0; i < boundary_line.size(); ++i)
            {
                unsigned int curr_pt_ref_idx = -1;
                for(int j = 0; j < m_unique_sampled_point_arr_without_uv_split.size(); ++j)
                {
                    if(sketch_math::point_equal(boundary_line[i], m_unique_sampled_point_arr_without_uv_split[j]))
                    {
                        curr_pt_ref_idx = j;
                        break;
                    }
                }
                if(curr_pt_ref_idx == -1)
                {
                    curr_pt_ref_idx = m_unique_sampled_point_arr_without_uv_split.size();
                    m_unique_sampled_point_arr_without_uv_split.emplace_back(boundary_line[i]);
                }

                if(i > 0)
                {
                    constrained_edges.push_back(CDT::Edge(last_pt_ref_idx, curr_pt_ref_idx));
                }
                else
                {
                    first_pt_ref_idx = curr_pt_ref_idx;
                }

                if(i == boundary_line.size() - 1)
                {
                    constrained_edges.push_back(CDT::Edge(curr_pt_ref_idx, first_pt_ref_idx));
                }

                last_pt_ref_idx = curr_pt_ref_idx;
            }
        };

        for(int i = 0; i < m_interior_rings.size(); ++i)
        {
            auto& inring_base = m_interior_rings[i];
            if(inring_base->type() == EElementType::AlgebraSurface)
            {
                auto&& boundary_line = inring_base->to_nurbs_algebra_surface()->get_surface_bounding_lines_1D();
                process_line_to_CDT_edge(boundary_line);
            }

            if(inring_base->type() == EElementType::ClosedSurface)
            {
                auto&& boundary_line = inring_base->to_nurbs_closed_surface()->get_sampled_boundary();
                process_line_to_CDT_edge(boundary_line);
            }
        }

        nous::vec3 exring_normal = nous::pos_t::Identity();
        NurbsClosedSurfacePtr exring_closed_surface = m_exterior_ring->to_nurbs_closed_surface();
        std::vector<nous::pos_t> boundary_line = exring_closed_surface->get_sampled_boundary();
        process_line_to_CDT_edge(boundary_line);
        exring_normal = sketch_math::ring_normal(boundary_line);

        nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(exring_normal, nous::vec3(0.f, 0.f, 1.f));
        std::vector<CDT::V2d<float>> cdt_positions;
        for(int i = 0; i < m_unique_sampled_point_arr_without_uv_split.size(); ++i)
        {
            const nous::pos_t xoy_pos = rotate_to_xy_plane * m_unique_sampled_point_arr_without_uv_split[i];
            CDT::V2d<float> cdt_pos;
            cdt_pos.x = xoy_pos.x();
            cdt_pos.y = xoy_pos.y();
            cdt_positions.emplace_back(cdt_pos);
        }

        try
        {
            CDT::Triangulation<float> cdt;
            cdt.insertVertices(cdt_positions);
            cdt.insertEdges(constrained_edges);
            const bool is_valid = CDT::verifyTopology(cdt);
            if(is_valid)
            {
                cdt.eraseOuterTrianglesAndHoles();

                for(int i = 0; i < cdt.triangles.size(); ++i)
                {
                    m_sampled_triangle_ref_idx_arr_without_uv_split.emplace_back(cdt.triangles[i].vertices[0], cdt.triangles[i].vertices[2],
                                                                                 cdt.triangles[i].vertices[1]);
                }

                auto&& uv_submesh_info = uv::auto_mapping_trimesh_uv(m_unique_sampled_point_arr_without_uv_split,
                                                                     m_sampled_triangle_ref_idx_arr_without_uv_split, false, 60.0, 0.02);
                for(int i = 0; i < uv_submesh_info.size(); ++i)
                {
                    unsigned int basic_pt_idx = m_unique_sampled_point_arr.size();
                    std::vector<nous::vec3> submesh_normal_arr(uv_submesh_info[i].vertices.size(), nous::vec3::Zero());
                    std::vector<float> submesh_pt_trinum_arr(uv_submesh_info[i].vertices.size(), 0.0);
                    for(int j = 0; j < uv_submesh_info[i].triangles.size(); ++j)
                    {
                        const unsigned int p0 = uv_submesh_info[i].triangles[j].x() - basic_pt_idx;
                        const unsigned int p1 = uv_submesh_info[i].triangles[j].y() - basic_pt_idx;
                        const unsigned int p2 = uv_submesh_info[i].triangles[j].z() - basic_pt_idx;

                        const nous::vec3 triangle_normal = -sketch_math::ring_normal(std::vector<nous::pos_t>{
                            uv_submesh_info[i].vertices[p0], uv_submesh_info[i].vertices[p1], uv_submesh_info[i].vertices[p2] });

                        submesh_normal_arr[p0] += triangle_normal;
                        submesh_normal_arr[p1] += triangle_normal;
                        submesh_normal_arr[p2] += triangle_normal;

                        submesh_pt_trinum_arr[p0]++;
                        submesh_pt_trinum_arr[p1]++;
                        submesh_pt_trinum_arr[p2]++;

                        m_sampled_triangle_ref_idx_arr.emplace_back(uv_submesh_info[i].triangles[j].x(), uv_submesh_info[i].triangles[j].y(),
                                                                    uv_submesh_info[i].triangles[j].z());
                    }

                    for(int j = 0; j < uv_submesh_info[i].vertices.size(); ++j)
                    {
                        m_unique_sampled_point_arr.emplace_back(uv_submesh_info[i].vertices[j]);
                        m_unique_sampled_uv_arr.emplace_back(uv_submesh_info[i].uvs[j]);
                        const nous::vec3 avg_normal = (submesh_normal_arr[j] / submesh_pt_trinum_arr[j]).normalized();
                        m_unique_sampled_normal_arr.emplace_back(avg_normal);
                    }
                }
            }
        }
        catch(CDT::IntersectingConstraintsError ice)
        {
            // 退化给自交检测
            return;
        }
        catch(CDT::DuplicateVertexError dve)
        {
            return;
        }
    }

    if(m_exterior_ring->type() == EElementType::AlgebraSurface)
    {
        // 这个处理会相对比较麻烦，按以下逻辑处理，目前暂时用不上
        // 1. 将内环边界提取出来，反算这些点的投影在外环曲面上的 uv 值
        // 2. 获取外环曲面上的采样点，携带对应的 uv 值，移除那些被内环包含的采样点
        // 3. 基于内环边界的 uv 值和剩余外环曲面上采样点的 uv 值在 2D uv 控件执行 CDT
        // 4. 基于 CDT 算法返回的三角形索引，反向获取 uv 对应的空间坐标
        if(m_interior_rings.empty())
        {
            // 没内环的要退化
            m_exterior_ring->to_nurbs_algebra_surface()->get_sampled_triangles_arr(m_unique_sampled_point_arr, m_sampled_triangle_ref_idx_arr,
                                                                                   &m_unique_sampled_normal_arr, &m_unique_sampled_uv_arr);
        }
        else
        {
        }
    }

    m_dirty = false;

    return;
}

NurbsTrimmedSurface::NurbsTrimmedSurface() : INurbsSurfaceBase(EElementType::TrimmedSurface) {}

NurbsTrimmedSurface::NurbsTrimmedSurface(std::shared_ptr<INurbsSurfaceBase> exterior_ring) : INurbsSurfaceBase(EElementType::TrimmedSurface)
{
    m_exterior_ring = exterior_ring;
}

NurbsTrimmedSurface::NurbsTrimmedSurface(std::shared_ptr<INurbsSurfaceBase> exterior_ring,
                                         const std::vector<std::shared_ptr<INurbsSurfaceBase>>& interior_rings)
    : INurbsSurfaceBase(EElementType::TrimmedSurface)
{
    m_exterior_ring = exterior_ring;
    m_interior_rings.insert(m_interior_rings.end(), interior_rings.begin(), interior_rings.end());
}

NurbsTrimmedSurface::~NurbsTrimmedSurface()
{
    finalization();
    std::cout << "Complex Surface Deconstruct!" << std::endl;
}

void NurbsTrimmedSurface::finalization()
{
    if(m_exterior_ring)
    {
        m_exterior_ring->set_parent_as_exring(nullptr);
    }
    m_exterior_ring = nullptr;

    for(int i = 0; i < m_interior_rings.size(); ++i)
    {
        m_interior_rings[i]->set_parent_as_inring(nullptr);
    }
    m_interior_rings.clear();

    m_complex_surface_elements.clear();
}

std::shared_ptr<INurbsSurfaceBase> NurbsTrimmedSurface::get_exterior_ring() const { return m_exterior_ring; }

std::shared_ptr<INurbsSurfaceBase> NurbsTrimmedSurface::get_interior_ring_at(const unsigned int idx) const
{
    if(idx < m_interior_rings.size() && idx >= 0)
    {
        return m_interior_rings[idx];
    }
    return nullptr;
}

std::vector<INurbsSurfaceBasePtr> NurbsTrimmedSurface::get_interior_rings() const { return m_interior_rings; }

void NurbsTrimmedSurface::set_exterior_ring(std::shared_ptr<INurbsSurfaceBase> surface)
{
    m_exterior_ring = surface;
    surface->set_parent_as_exring(shared_from_this()->to_nurbs_trimmed_surface());
    dirty();
}

void NurbsTrimmedSurface::add_interior_ring(std::shared_ptr<INurbsSurfaceBase> surface)
{
    m_interior_rings.push_back(surface);
    surface->set_parent_as_inring(shared_from_this()->to_nurbs_trimmed_surface());
    dirty();
}

void NurbsTrimmedSurface::remove_interior_ring_at(const unsigned int idx)
{
    if(idx < m_interior_rings.size() && idx >= 0)
    {
        m_interior_rings.erase(m_interior_rings.begin() + idx);
    }
}

};// namespace sketch