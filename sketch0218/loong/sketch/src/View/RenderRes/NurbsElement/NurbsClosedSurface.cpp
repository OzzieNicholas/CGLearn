#include "sketch/View/RenderRes/NurbsElement/NurbsClosedSurface.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsClosedSurfaceEdgeElement.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsComplexSurface.h"

namespace sketch
{

NurbsClosedSurface::~NurbsClosedSurface() { m_boundary_curve_elements.clear(); }

void NurbsClosedSurface::generate_render_mesh(IRenderable* render_data)
{
    _sample_object();

    std::vector<float>& position = render_data->get_render_position();
    std::vector<float>& normal = render_data->get_render_normal();
    std::vector<unsigned int>& lines = render_data->get_render_lines_index();
    std::vector<unsigned int>& triangles = render_data->get_render_triangles_index();
    std::vector<SubmeshData>& submeshes = render_data->get_render_submesh();

    for(const auto& sub_edge_element_info: m_boundary_curve_elements)
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

    size_t position_start_idx = position.size() / 3;
    const size_t basic_tri_size = triangles.size();
    for(int i = 0; i < m_cached_sample_triangles.size(); ++i)
    {
        const nous::pos_t tri_a = m_cached_sample_triangles[i][0];
        const nous::pos_t tri_b = m_cached_sample_triangles[i][1];
        const nous::pos_t tri_c = m_cached_sample_triangles[i][2];
        position.emplace_back(tri_a.x());
        position.emplace_back(tri_a.y());
        position.emplace_back(tri_a.z());
        position.emplace_back(tri_b.x());
        position.emplace_back(tri_b.y());
        position.emplace_back(tri_b.z());
        position.emplace_back(tri_c.x());
        position.emplace_back(tri_c.y());
        position.emplace_back(tri_c.z());

        const nous::vec3 tri_normal = sketch_math::ring_normal(m_cached_sample_triangles[i]);
        normal.emplace_back(tri_normal.x());
        normal.emplace_back(tri_normal.y());
        normal.emplace_back(tri_normal.z());
        normal.emplace_back(tri_normal.x());
        normal.emplace_back(tri_normal.y());
        normal.emplace_back(tri_normal.z());
        normal.emplace_back(tri_normal.x());
        normal.emplace_back(tri_normal.y());
        normal.emplace_back(tri_normal.z());

        triangles.emplace_back(position_start_idx + i * 3);
        triangles.emplace_back(position_start_idx + i * 3 + 1);
        triangles.emplace_back(position_start_idx + i * 3 + 2);
    }
    submeshes.emplace_back("Polygon", basic_tri_size, triangles.size() - basic_tri_size, SubmeshData::Topology::Triangles, this->shared_from_this());
}

nous::AABB3 NurbsClosedSurface::get_bounding_box()
{
    _sample_object();

    nous::AABB3 bounding_box;
    for(int i = 0; i < m_cached_sample_points.size(); ++i)
    {
        bounding_box.extend(m_cached_sample_points[i]);
    }

    return bounding_box;
}

void NurbsClosedSurface::save(SketchSerializer& serializer)
{
    serializer.write(this->id(), "id");
    serializer.write((uint32_t)this->type(), "type");
    serializer.begin_write_array("curves");
    for(int i = 0; i < m_inner_curve_reps.size(); ++i)
    {
        serializer.begin_write_object();
        serializer.write_sketch_nurbs_item_ref(m_inner_curve_reps[i].m_inner_curve, "curve");
        serializer.write(m_inner_curve_reps[i].m_min_u, "min_u");
        serializer.write(m_inner_curve_reps[i].m_max_u, "max_u");
        serializer.end_write_object();
    }
    serializer.end_write_array();
}

void NurbsClosedSurface::load(SketchSerializer& serializer)
{
    uint32_t id;
    serializer.read(id, "id");
    this->set_id(id);
    uint32_t c_type;
    serializer.read(c_type, "type");
    this->set_type(static_cast<EElementType>(c_type));

    size_t curve_rep_count;
    serializer.begin_read_array("curves", curve_rep_count);
    for(int i = 0; i < curve_rep_count; ++i)
    {
        serializer.begin_read_object();
        NurbsClosedSurfaceCurve curve_range;
        NurbsCurvePtr curve_info = serializer.read_sketch_nurbs_item_ref("curve")->to_nurbs_curve();
        curve_info->set_parent(shared_from_this()->to_nurbs_closed_surface());
        curve_range.m_inner_curve = curve_info;
        serializer.read(curve_range.m_min_u, "min_u");
        serializer.read(curve_range.m_max_u, "max_u");
        serializer.end_read_object();
        m_inner_curve_reps.emplace_back(std::move(curve_range));
    }
    serializer.end_read_array();
}

bool NurbsClosedSurface::is_valid() { return m_inner_curve_reps.size() > 0; }

void NurbsClosedSurface::transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat)
{
    for(int i = 0; i < m_inner_curve_reps.size(); ++i)
    {
        m_inner_curve_reps[i].m_inner_curve->transform(transformed_set, trans_mat);
    }

    dirty();
}

std::shared_ptr<INurbsBase> NurbsClosedSurface::clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                                      const nous::mat4& trans_mat)
{
    NurbsClosedSurfacePtr clone_item = std::make_shared<NurbsClosedSurface>();
    for(int i = 0; i < m_inner_curve_reps.size(); ++i)
    {
        NurbsClosedSurfaceCurve clone_curve_range;
        clone_curve_range.m_inner_curve = m_inner_curve_reps[i].m_inner_curve->clone(remaped_map, trans_mat)->to_nurbs_curve();
        clone_curve_range.m_inner_curve->set_parent(clone_item);
        clone_curve_range.m_min_u = m_inner_curve_reps[i].m_min_u;
        clone_curve_range.m_max_u = m_inner_curve_reps[i].m_max_u;
        clone_item->m_inner_curve_reps.emplace_back(clone_curve_range);
    }
    clone_item->dirty();
    return clone_item;
}

std::vector<std::vector<nous::pos_t>> NurbsClosedSurface::get_sampled_triangles()
{
    _sample_object();
    return m_cached_sample_triangles;
}

std::vector<nous::pos_t> NurbsClosedSurface::get_sampled_boundary()
{
    _sample_object();
    return m_cached_sample_points;
}

std::vector<std::vector<SketchNurbsPointPtr>> NurbsClosedSurface::get_surface_control_points()
{
    std::vector<std::vector<SketchNurbsPointPtr>> control_points;
    for(int i = 0; i < m_inner_curve_reps.size(); ++i)
    {
        const auto&& sub_control_points = m_inner_curve_reps[i].m_inner_curve->get_curve_control_points();
        control_points.emplace_back(sub_control_points);
    }
    return control_points;
}

std::vector<SketchNurbsPointPtr> NurbsClosedSurface::get_surface_control_points_1D()
{
    std::vector<SketchNurbsPointPtr> control_points;
    for(int i = 0; i < m_inner_curve_reps.size(); ++i)
    {
        const auto&& sub_control_points = m_inner_curve_reps[i].m_inner_curve->get_curve_control_points();
        control_points.insert(control_points.end(), sub_control_points.begin(), sub_control_points.end());
    }
    return control_points;
}

std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> NurbsClosedSurface::get_surface_elements()
{
    _sample_object();
    return m_boundary_curve_elements;
}

void NurbsClosedSurface::set_curve_control_points(const unsigned int curve_idx, const unsigned int pt_idx, const nous::pos_t& position)
{
    m_inner_curve_reps[curve_idx].m_inner_curve->update_curve_control_point(pt_idx, position);
    dirty();
}

void NurbsClosedSurface::add_sub_curve(NurbsCurvePtr curve, double min_u, double max_u)
{
    NurbsCurvePtr captured_curve = nurbs::capture_curve(curve, min_u, max_u);
    captured_curve->set_parent(this->to_nurbs_closed_surface());
    if(!m_inner_curve_reps.empty())
    {
        NurbsClosedSurfaceCurve last_range = m_inner_curve_reps.back();
        auto& last_curve = last_range.m_inner_curve;
        const auto& last_curve_end_pos = last_curve->get_position(1.0);
        const auto& curr_curve_start_pos = curve->get_position(0.0);
        if(sketch_math::point_equal(last_curve_end_pos, curr_curve_start_pos))
        {
            captured_curve->replace_curve_control_point(0, last_curve->get_curve_control_points().back());
            NurbsClosedSurfaceCurve curve_range;
            curve_range.m_inner_curve = captured_curve;
            curve_range.m_min_u = 0.0;
            curve_range.m_max_u = 1.0;
            captured_curve->set_parent(this->to_nurbs_closed_surface());
            m_inner_curve_reps.emplace_back(curve_range);
            m_dirty = true;
        }
    }
    else
    {
        NurbsClosedSurfaceCurve curve_range;
        curve_range.m_inner_curve = captured_curve;
        curve_range.m_min_u = 0.0;
        curve_range.m_max_u = 1.0;
        captured_curve->set_parent(this->to_nurbs_closed_surface());
        m_inner_curve_reps.emplace_back(curve_range);
        m_dirty = true;
    }
}

void NurbsClosedSurface::add_sub_curve_without_check(NurbsCurvePtr curve)
{
    NurbsClosedSurfaceCurve curve_range;
    curve_range.m_inner_curve = curve;
    curve_range.m_min_u = 0.0;
    curve_range.m_max_u = 1.0;
    curve->set_parent(this->to_nurbs_closed_surface());
    m_inner_curve_reps.emplace_back(curve_range);
    m_dirty = true;
}

void NurbsClosedSurface::_sample_object()
{
    if(m_dirty)
    {
        m_boundary_curve_elements.clear();
        for(int i = 0; i < m_inner_curve_reps.size(); ++i)
        {
            std::shared_ptr<NurbsClosedSurfaceEdgeElement> sub_curve_element = std::make_shared<NurbsClosedSurfaceEdgeElement>();
            sub_curve_element->m_edge_curve = m_inner_curve_reps[i].m_inner_curve;
            sub_curve_element->m_parent = shared_from_this()->to_nurbs_closed_surface();
            auto&& sampled_point_arr = sub_curve_element->m_edge_curve.lock()->get_sampled_point_arr(100);
            m_boundary_curve_elements.insert({ sub_curve_element, sampled_point_arr });
        }

        m_cached_sample_points.clear();
        for(int i = 0; i < m_inner_curve_reps.size(); ++i)
        {
            const NurbsClosedSurfaceCurve& sub_curve_info = m_inner_curve_reps[i];
            std::vector<nous::pos_t> sub_boundary =
                sub_curve_info.m_inner_curve->get_sampled_point_arr(sub_curve_info.m_min_u, sub_curve_info.m_max_u, 100);
            if(!sub_boundary.empty())
            {
                m_cached_sample_points.insert(m_cached_sample_points.end(), sub_boundary.begin(), sub_boundary.end() - 1);
            }
        }

        std::vector<std::vector<nous::pos_t>> polygon;
        polygon.emplace_back(m_cached_sample_points);
        m_cached_sample_triangles = geodata_topo::triangulate_polygon(polygon);

        m_dirty = false;
    }
}

void NurbsClosedSurface::dirty()
{
    m_dirty = true;
    if(!m_parent_ex.expired())
    {
        m_parent_ex.lock()->dirty();
    }

    if(!m_parent_in.expired())
    {
        m_parent_in.lock()->dirty();
    }
}

void NurbsClosedSurface::set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) { m_parent_ex = parent; }

void NurbsClosedSurface::set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) { m_parent_in = parent; }

std::pair<std::shared_ptr<NurbsTrimmedSurface>, std::shared_ptr<NurbsTrimmedSurface>> NurbsClosedSurface::get_parent() const
{
    std::shared_ptr<NurbsTrimmedSurface> ex = m_parent_ex.expired() ? nullptr : m_parent_ex.lock();
    std::shared_ptr<NurbsTrimmedSurface> in = m_parent_in.expired() ? nullptr : m_parent_in.lock();
    return std::make_pair(ex, in);
}

bool NurbsClosedSurface::is_surface_coplane(nous::vec3& coplane_normal)
{
    auto&& control_points_array = get_surface_control_points_1D();
    std::vector<nous::pos_t> ctp_position_arr;
    for(auto& ctp: control_points_array)
    {
        ctp_position_arr.emplace_back(ctp->m_point);
    }

    bool is_coplane = topo::calc_point_set_topo(ctp_position_arr, coplane_normal) == topo::EPointSetTopo::Coplane;

    if(is_coplane)
    {
        coplane_normal = sketch_math::ring_normal(ctp_position_arr);
    }

    return is_coplane;
}

};// namespace sketch