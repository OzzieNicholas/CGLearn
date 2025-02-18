#include "sketch/View/RenderRes/NurbsElement/NurbsAlgebraSurface.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsAlgebraSurfaceEdgeElement.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsComplexSurface.h"
#include <tbb/global_control.h>

namespace sketch
{

NurbsAlgebraSurface::~NurbsAlgebraSurface()
{
    for(int i = 0; i < m_inner_surface->control_points.rows(); ++i)
    {
        for(int j = 0; j < m_inner_surface->control_points.cols(); ++j)
        {
            auto&& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
            wrapped->remove_parent(this);
        }
    }

    // 此处的手动析构不可忽略！否则会出现 m_edge_curve 未及时析构的现象，在执行曲面/曲线删除时引用计数会出问题
    for(auto& sub_element: m_boundary_curve_elements)
    {
        sub_element.first->to_nurbs_algebra_surface_element()->m_edge_curve = nullptr;
    }
    m_boundary_curve_elements.clear();
}

void NurbsAlgebraSurface::generate_render_mesh(IRenderable* render_data)
{
    if(m_render_level == ENurbsSurfaceRenderLevel::WireFrame)
    {
        _simple_generate_render_mesh(render_data);
        return;
    }

    _sample_surface();

    std::vector<float>& position = render_data->get_render_position();
    std::vector<float>& normal = render_data->get_render_normal();
    std::vector<float>& uv = render_data->get_render_uv();
    std::vector<unsigned int>& lines = render_data->get_render_lines_index();
    std::vector<unsigned int>& triangles = render_data->get_render_triangles_index();
    std::vector<SubmeshData>& submeshes = render_data->get_render_submesh();

    const size_t basic_position_number = position.size() / 3;
    const size_t basic_triangles_number = triangles.size();

    for(int i = 0; i < m_sampled_surface_position.size(); ++i)
    {
        for(int j = 0; j < m_sampled_surface_position[i].size(); ++j)
        {
            position.emplace_back(m_sampled_surface_position[i][j].x());
            position.emplace_back(m_sampled_surface_position[i][j].y());
            position.emplace_back(m_sampled_surface_position[i][j].z());
            normal.emplace_back(m_sampled_surface_normal[i][j].x());
            normal.emplace_back(m_sampled_surface_normal[i][j].y());
            normal.emplace_back(m_sampled_surface_normal[i][j].z());
            uv.emplace_back(m_sampled_surface_uv[i][j].x());
            uv.emplace_back(m_sampled_surface_uv[i][j].y());
        }
    }

    const int sample_rows = m_sampled_surface_position.size();
    const int sample_cols = m_sampled_surface_position.front().size();

    for(int i = 0; i < sample_rows - 1; ++i)
    {
        for(int j = 0; j < sample_cols - 1; ++j)
        {
            const size_t p0 = basic_position_number + i * sample_cols + j;
            const size_t p1 = basic_position_number + i * sample_cols + (j + 1);
            const size_t p2 = basic_position_number + (i + 1) * sample_cols + j;
            const size_t p3 = basic_position_number + (i + 1) * sample_cols + (j + 1);

            triangles.emplace_back(p0);
            triangles.emplace_back(p1);
            triangles.emplace_back(p2);

            triangles.emplace_back(p1);
            triangles.emplace_back(p3);
            triangles.emplace_back(p2);
        }
    }

    submeshes.emplace_back("Polygon", basic_triangles_number, triangles.size() - basic_triangles_number, SubmeshData::Topology::Triangles,
                           this->shared_from_this());

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
}

nous::AABB3 NurbsAlgebraSurface::get_bounding_box()
{
    _sample_surface();
    nous::AABB3 bounding_box;
    for(int i = 0; i < m_sampled_surface_position.size(); ++i)
    {
        for(int j = 0; j < m_sampled_surface_position[i].size(); ++j)
        {
            bounding_box.extend(m_sampled_surface_position[i][j]);
        }
    }
    return bounding_box;
}

void NurbsAlgebraSurface::save(SketchSerializer& serializer)
{
    serializer.write(this->id(), "id");
    serializer.write((uint32_t)this->type(), "type");
    serializer.write(m_inner_surface->degree_u, "degree_u");
    serializer.write(m_inner_surface->degree_v, "degree_v");
    serializer.write(m_inner_surface->control_points.rows(), "u_number");
    serializer.write(m_inner_surface->control_points.cols(), "v_number");
    serializer.begin_write_array("knots_u");
    for(int i = 0; i < m_inner_surface->knots_u.size(); ++i)
    {
        serializer.write(m_inner_surface->knots_u[i]);
    }
    serializer.end_write_array();
    serializer.begin_write_array("knots_v");
    for(int i = 0; i < m_inner_surface->knots_v.size(); ++i)
    {
        serializer.write(m_inner_surface->knots_v[i]);
    }
    serializer.end_write_array();
    serializer.begin_write_array("control_points");
    for(int i = 0; i < m_inner_surface->control_points.size(); ++i)
    {
        auto wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points[i]);
        serializer.write_sketch_nurbs_point_ref(wrapped);
    }
    serializer.end_write_array();
    serializer.begin_write_array("weights");
    for(int i = 0; i < m_inner_surface->weights.size(); ++i)
    {
        serializer.write(m_inner_surface->weights[i]);
    }
    serializer.end_write_array();
}

void NurbsAlgebraSurface::load(SketchSerializer& serializer)
{
    uint32_t id;
    serializer.read(id, "id");
    this->set_id(id);
    serializer.read(m_inner_surface->degree_u, "degree_u");
    serializer.read(m_inner_surface->degree_v, "degree_v");
    int u_number, v_number;
    serializer.read(u_number, "u_number");
    serializer.read(v_number, "v_number");

    size_t knots_u_number = 0, knots_v_number = 0, control_points_number = 0, weights_number = 0;
    serializer.begin_read_array("knots_u", knots_u_number);
    m_inner_surface->knots_u.clear();
    m_inner_surface->knots_u.resize(knots_u_number);
    for(int i = 0; i < knots_u_number; ++i)
    {
        serializer.read(m_inner_surface->knots_u[i]);
    }
    serializer.end_read_array();

    serializer.begin_read_array("knots_v", knots_v_number);
    m_inner_surface->knots_v.clear();
    m_inner_surface->knots_v.resize(knots_v_number);
    for(int i = 0; i < knots_v_number; ++i)
    {
        serializer.read(m_inner_surface->knots_v[i]);
    }
    serializer.end_read_array();

    serializer.begin_read_array("control_points", control_points_number);
    m_inner_surface->control_points.clear();
    m_inner_surface->control_points.resize(u_number, v_number, nullptr);
    for(int i = 0; i < control_points_number; ++i)
    {
        auto& wrapped = serializer.read_sketch_nurbs_point_ref();
        wrapped->m_parents.emplace_back(this->to_nurbs_algebra_surface());
        m_inner_surface->control_points[i] = wrapped;
    }
    serializer.end_read_array();

    serializer.begin_read_array("weights", weights_number);
    m_inner_surface->weights.clear();
    m_inner_surface->weights.resize(u_number, v_number, 1.0);
    for(int i = 0; i < weights_number; ++i)
    {
        serializer.read(m_inner_surface->weights[i]);
    }
    serializer.end_read_array();
}

bool NurbsAlgebraSurface::is_valid() { return tinynurbs::surfaceIsValid<nous::scalar>(*m_inner_surface); }

void NurbsAlgebraSurface::update_surface_data(std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> other)
{
    m_inner_surface->control_points.clear();
    m_inner_surface->weights.clear();
    m_inner_surface->knots_u.clear();
    m_inner_surface->knots_v.clear();

    m_inner_surface->degree_u = other->degree_u;
    m_inner_surface->degree_v = other->degree_v;
    m_inner_surface->knots_u.insert(m_inner_surface->knots_u.end(), other->knots_u.begin(), other->knots_u.end());
    m_inner_surface->knots_v.insert(m_inner_surface->knots_v.end(), other->knots_v.begin(), other->knots_v.end());
    m_inner_surface->weights.copy(other->weights);
    m_inner_surface->control_points.resize(other->control_points.rows(), other->control_points.cols(), nullptr);

    for(int i = 0; i < other->control_points.size(); ++i)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(other->control_points[i]);
        bool existed = false;
        for(int j = 0; j < wrapped->m_parents.size(); ++j)
        {
            if(!wrapped->m_parents[j].expired() && wrapped->m_parents[j].lock() == this->to_nurbs_algebra_surface())
            {
                existed = true;
                break;
            }
        }
        if(!existed)
        {
            wrapped->m_parents.emplace_back(this->to_nurbs_algebra_surface());
        }
        m_inner_surface->control_points[i] = wrapped;
    }

    m_dirty = true;
}

const std::vector<std::vector<nous::pos_t>>& NurbsAlgebraSurface::get_surface_bounding_lines()
{
    _sample_surface();

    if(m_boundary_dirty)
    {
        m_boundary_dirty = false;
        // 绕曲面采样点一圈
        m_sampled_surface_boundary.clear();
        if(is_surface_u_closed() && is_surface_v_closed())
        {
            return m_sampled_surface_boundary;
        }

        if(is_surface_v_closed())
        {
            m_sampled_surface_boundary.emplace_back(m_sampled_surface_position.front());
            m_sampled_surface_boundary.emplace_back(m_sampled_surface_position.back());
            return m_sampled_surface_boundary;
        }

        if(is_surface_u_closed())
        {
            std::vector<nous::pos_t>& one_line = m_sampled_surface_boundary.emplace_back();
            for(int i = 0; i < m_sampled_surface_position.size(); ++i)
            {
                one_line.emplace_back(m_sampled_surface_position[i].back());
            }

            std::vector<nous::pos_t>& two_line = m_sampled_surface_boundary.emplace_back();
            for(int i = 0; i < m_sampled_surface_position.size(); ++i)
            {
                two_line.emplace_back(m_sampled_surface_position[i].front());
            }

            return m_sampled_surface_boundary;
        }

        m_sampled_surface_boundary.emplace_back(m_sampled_surface_position.front());
        m_sampled_surface_boundary.emplace_back(m_sampled_surface_position.back());
        std::vector<nous::pos_t>& one_line = m_sampled_surface_boundary.emplace_back();
        for(int i = 0; i < m_sampled_surface_position.size(); ++i)
        {
            one_line.emplace_back(m_sampled_surface_position[i].back());
        }

        std::vector<nous::pos_t>& two_line = m_sampled_surface_boundary.emplace_back();
        for(int i = 0; i < m_sampled_surface_position.size(); ++i)
        {
            two_line.emplace_back(m_sampled_surface_position[i].front());
        }
    }

    return m_sampled_surface_boundary;
}

std::vector<nous::pos_t> NurbsAlgebraSurface::get_surface_bounding_lines_1D()
{
    _sample_surface();

    std::vector<nous::pos_t> bounding_line_1D;
    if(is_surface_u_closed() || is_surface_v_closed())
    {
        return bounding_line_1D;
    }

    unsigned int rows = m_sampled_surface_position.size();
    unsigned int cols = m_sampled_surface_position.front().size();

    m_sampled_surface_boundary.emplace_back(m_sampled_surface_position.front());
    for(int i = 1; i < rows; ++i)
    {
        bounding_line_1D.emplace_back(m_sampled_surface_position[i].back());
    }
    for(int i = 1; i < cols; ++i)
    {
        bounding_line_1D.emplace_back(m_sampled_surface_position[rows - 1][cols - 1 - i]);
    }
    for(int i = 1; i < rows - 1; ++i)
    {
        bounding_line_1D.emplace_back(m_sampled_surface_position[rows - 1 - i].front());
    }

    return bounding_line_1D;
}

ENurbsCloseType NurbsAlgebraSurface::get_surface_u_close_type()
{
    if(tinynurbs::surfaceIsClosedU(*m_inner_surface))
    {
        return ENurbsCloseType::SmoothClose;
    }

    bool is_all_control_points_closed = true;
    const unsigned int rows = m_inner_surface->control_points.rows();
    const unsigned int cols = m_inner_surface->control_points.cols();

    for(int i = 0; i < cols; ++i)
    {
        if(!sketch_math::point_equal(m_inner_surface->control_points(0, i)->m_point, m_inner_surface->control_points(rows - 1, i)->m_point))
        {
            is_all_control_points_closed = false;
            break;
        }
    }

    bool is_knots_closed = true;
    const auto& surface_knots_u = m_inner_surface->knots_u;
    for(int i = 0; i < m_inner_surface->degree_u; ++i)
    {
        if(!(surface_knots_u[i] == surface_knots_u[i + 1] && surface_knots_u[i] == 0.0))
        {
            is_knots_closed = false;
            break;
        }

        if(!(surface_knots_u[surface_knots_u.size() - i - 1] == surface_knots_u[surface_knots_u.size() - i - 2] &&
             surface_knots_u[surface_knots_u.size() - i - 1] == 1.0))
        {
            is_knots_closed = false;
            break;
        }
    }

    return (is_knots_closed && is_all_control_points_closed) ? ENurbsCloseType::SharpClose : ENurbsCloseType::Open;
}

ENurbsCloseType NurbsAlgebraSurface::get_surface_v_close_type()
{
    if(tinynurbs::surfaceIsClosedV(*m_inner_surface))
    {
        return ENurbsCloseType::SmoothClose;
    }

    bool is_all_control_points_closed = true;
    const unsigned int rows = m_inner_surface->control_points.rows();
    const unsigned int cols = m_inner_surface->control_points.cols();

    for(int i = 0; i < rows; ++i)
    {
        if(!sketch_math::point_equal(m_inner_surface->control_points(i, 0)->m_point, m_inner_surface->control_points(i, cols - 1)->m_point))
        {
            is_all_control_points_closed = false;
            break;
        }
    }

    bool is_knots_closed = true;
    const auto& surface_knots_v = m_inner_surface->knots_v;
    for(int i = 0; i < m_inner_surface->degree_v; ++i)
    {
        if(!(surface_knots_v[i] == surface_knots_v[i + 1] && surface_knots_v[i] == 0.0))
        {
            is_knots_closed = false;
            break;
        }

        if(!(surface_knots_v[surface_knots_v.size() - i - 1] == surface_knots_v[surface_knots_v.size() - i - 2] &&
             surface_knots_v[surface_knots_v.size() - i - 1] == 1.0))
        {
            is_knots_closed = false;
            break;
        }
    }

    return (is_knots_closed && is_all_control_points_closed) ? ENurbsCloseType::SharpClose : ENurbsCloseType::Open;
}

bool NurbsAlgebraSurface::is_surface_u_closed()
{
    ENurbsCloseType close_type = get_surface_u_close_type();
    return close_type != ENurbsCloseType::Open;
}

bool NurbsAlgebraSurface::is_surface_v_closed()
{
    ENurbsCloseType close_type = get_surface_v_close_type();
    return close_type != ENurbsCloseType::Open;
}

void NurbsAlgebraSurface::_sample_surface()
{
    if(m_dirty)
    {
        m_boundary_curve_elements.clear();
        auto&& surface_boundaries = nurbs::extract_surface_boundary_curve(shared_from_this()->to_nurbs_algebra_surface());
        for(auto& pair_info: surface_boundaries)
        {
            std::shared_ptr<NurbsAlgebraSurfaceEdgeElement> element = std::make_shared<NurbsAlgebraSurfaceEdgeElement>();
            element->m_parent = shared_from_this()->to_nurbs_algebra_surface();
            element->m_edge_type = pair_info.first;
            element->m_edge_curve = pair_info.second;
            std::vector<nous::pos_t> curve_sampled_points = element->m_edge_curve->get_sampled_point_arr(100);
            m_boundary_curve_elements.insert({ element, curve_sampled_points });
        }
    }

    if(m_render_level == ENurbsSurfaceRenderLevel::WireFrame)
    {
        return _simple_sample_surface();
    }

    if(m_dirty)
    {
        m_boundary_dirty = true;
        unsigned int sample_rows = 60;
        unsigned int sample_cols = 60;

        std::vector<float> sample_u_arr;
        std::vector<float> sample_v_arr;
        if(m_inner_surface->degree_u == 1)
        {
            sample_rows = (m_inner_surface->knots_u.size() - m_inner_surface->degree_u * 2);
            for(int i = 1; i < m_inner_surface->knots_u.size() - 1; ++i)
            {
                sample_u_arr.emplace_back(m_inner_surface->knots_u[i]);
            }
            if(m_inner_surface->degree_v > 1)
            {
                sample_cols = 2000 / sample_rows;
            }
        }
        if(m_inner_surface->degree_v == 1)
        {
            sample_cols = (m_inner_surface->knots_v.size() - m_inner_surface->degree_v * 2);
            for(int i = 1; i < m_inner_surface->knots_v.size() - 1; ++i)
            {
                sample_v_arr.emplace_back(m_inner_surface->knots_v[i]);
            }
            if(m_inner_surface->degree_u > 1)
            {
                sample_rows = 2000 / sample_cols;
            }
        }
        if(m_sampled_surface_position.size() != sample_rows || m_sampled_surface_position.front().size() != sample_cols)
        {
            m_sampled_surface_position.clear();
            m_sampled_surface_normal.clear();
            m_sampled_surface_uv.clear();
            m_sampled_surface_position.resize(sample_rows, std::vector<nous::pos_t>(sample_cols, nous::pos_t::Zero()));
            m_sampled_surface_normal.resize(sample_rows, std::vector<nous::pos_t>(sample_cols, nous::pos_t::Zero()));
            m_sampled_surface_uv.resize(sample_rows, std::vector<nous::vec2>(sample_cols, nous::vec2::Zero()));
        }

        const ENurbsCloseType u_closed_type = get_surface_u_close_type();
        const ENurbsCloseType v_closed_type = get_surface_v_close_type();
        nous::scalar min_u = m_min_u;
        nous::scalar max_u = m_max_u;
        nous::scalar min_v = m_min_v;
        nous::scalar max_v = m_max_v;
        if(u_closed_type == ENurbsCloseType::SmoothClose)
        {
            min_u = std::max(m_inner_surface->knots_u[m_inner_surface->degree_u], min_u);
            max_u = std::min(m_inner_surface->knots_u[m_inner_surface->knots_u.size() - m_inner_surface->degree_u - 1], max_u);
        }
        if(v_closed_type == ENurbsCloseType::SmoothClose)
        {
            min_v = std::max(m_inner_surface->knots_v[m_inner_surface->degree_v], min_v);
            max_v = std::min(m_inner_surface->knots_v[m_inner_surface->knots_v.size() - m_inner_surface->degree_v - 1], max_v);
        }

        min_u = std::max(min_u, nous::scalar(0.0));
        max_u = std::min(max_u, nous::scalar(1.0));
        min_v = std::max(min_v, nous::scalar(0.0));
        max_v = std::min(max_v, nous::scalar(1.0));

        if(sample_u_arr.empty())
        {
            for(int i = 0; i < sample_rows; ++i)
            {
                nous::scalar u = min_u + i * (1.0 / (sample_rows - 1)) * (max_u - min_u);
                sample_u_arr.emplace_back(u);
            }
        }

        if(sample_v_arr.empty())
        {
            for(int i = 0; i < sample_cols; ++i)
            {
                nous::scalar v = min_v + i * (1.0 / (sample_cols - 1)) * (max_v - min_v);
                sample_v_arr.emplace_back(v);
            }
        }

        auto mp = tbb::global_control::max_allowed_parallelism;
        tbb::global_control gc(mp, 10);
        tbb::task_arena a(3 * 10 / 2);
        tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sample_rows), [&](tbb::blocked_range<unsigned int> r) -> void {
            for(unsigned int i = r.begin(), num = r.end(); i < num; ++i)
            {
                nous::scalar u = sample_u_arr[i];
                tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sample_cols), [&](tbb::blocked_range<unsigned int> p) -> void {
                    for(unsigned int j = p.begin(), num2 = p.end(); j < num2; ++j)
                    {
                        nous::scalar v = sample_v_arr[j];
                        const nous::pos_t& pos = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                        const nous::pos_t& normal = tinynurbs::surfaceNormal(*m_inner_surface, u, v);
                        m_sampled_surface_position[i][j] = pos;
                        m_sampled_surface_normal[i][j] = normal;
                        m_sampled_surface_uv[i][j] = nous::vec2(u, v);
                    }
                });
            }
        });
        m_dirty = false;
    }
}

std::vector<std::vector<nous::pos_t>> NurbsAlgebraSurface::_sample_surface(const unsigned int u_count, const unsigned int v_count,
                                                                           const nous::scalar u_min, const nous::scalar u_max,
                                                                           const nous::scalar v_min, const nous::scalar v_max) const
{
    std::vector<std::vector<nous::pos_t>> result(u_count, std::vector<nous::pos_t>(v_count));

    nous::scalar fitted_min_u = std::max(u_min, m_min_u);
    nous::scalar fitted_max_u = std::min(u_max, m_max_u);
    nous::scalar fitted_min_v = std::max(v_min, m_min_v);
    nous::scalar fitted_max_v = std::min(v_max, m_max_v);

    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, u_count), [&](tbb::blocked_range<unsigned int> r) -> void {
        for(unsigned int i = r.begin(), num = r.end(); i < num; ++i)
        {
            const nous::scalar u = fitted_min_u + i * (1.0 / (u_count - 1)) * (fitted_max_u - fitted_min_u);
            tbb::parallel_for(tbb::blocked_range<unsigned int>(0, v_count), [&](tbb::blocked_range<unsigned int> p) -> void {
                for(unsigned int j = p.begin(), num2 = p.end(); j < num2; ++j)
                {
                    const nous::scalar v = fitted_min_v + j * (1.0 / (v_count - 1)) * (fitted_max_v - fitted_min_v);
                    const nous::pos_t& pos = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                    result[i][j] = pos;
                }
            });
        }
    });

    return result;
}

void NurbsAlgebraSurface::_simple_sample_surface()
{
    if(m_dirty)
    {
        unsigned int sample_u_number = (m_inner_surface->knots_u.size() - m_inner_surface->degree_u * 2) * m_sample_count_multiple_u;
        unsigned int sample_v_number = (m_inner_surface->knots_v.size() - m_inner_surface->degree_v * 2) * m_sample_count_multiple_v;
        m_sampled_surface_position.clear();
        m_sampled_surface_normal.clear();
        m_sampled_surface_uv.clear();

        m_sampled_surface_position.resize(sample_u_number, std::vector<nous::pos_t>(sample_v_number));
        m_sampled_surface_normal.resize(sample_u_number, std::vector<nous::vec3>(sample_v_number));
        m_sampled_surface_uv.resize(sample_u_number, std::vector<nous::vec2>(sample_v_number));
        const nous::scalar max_u = m_inner_surface->knots_u[m_inner_surface->knots_u.size() - m_inner_surface->degree_u - 1];
        const nous::scalar min_u = m_inner_surface->knots_u[m_inner_surface->degree_u];
        const nous::scalar max_v = m_inner_surface->knots_v[m_inner_surface->knots_v.size() - m_inner_surface->degree_v - 1];
        const nous::scalar min_v = m_inner_surface->knots_v[m_inner_surface->degree_v];

        if(sample_u_number < 5)
        {
            for(int i = 0; i < sample_u_number; ++i)
            {
                const nous::scalar u = (max_u - min_u) / (sample_u_number - 1) * i + min_u;
                if(sample_v_number < 5)
                {
                    for(int j = 0; j < sample_v_number; ++j)
                    {
                        const nous::scalar v = (max_v - min_v) / (sample_v_number - 1) * j + min_v;
                        m_sampled_surface_position[i][j] = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                        m_sampled_surface_normal[i][j] = tinynurbs::surfaceNormal(*m_inner_surface, u, v);
                        m_sampled_surface_uv[i][j] = nous::vec2(u, v);
                    }
                }
                else
                {
                    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sample_v_number), [&](tbb::blocked_range<unsigned int> p) -> void {
                        for(unsigned int j = p.begin(), num2 = p.end(); j < num2; ++j)
                        {
                            const nous::scalar v = (max_v - min_v) / (sample_v_number - 1) * j + min_v;
                            m_sampled_surface_position[i][j] = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                            m_sampled_surface_normal[i][j] = tinynurbs::surfaceNormal(*m_inner_surface, u, v);
                            m_sampled_surface_uv[i][j] = nous::vec2(u, v);
                        }
                    });
                }
            }
        }
        else
        {
            tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sample_u_number), [&](tbb::blocked_range<unsigned int> r) -> void {
                for(unsigned int i = r.begin(), num = r.end(); i < num; ++i)
                {
                    const nous::scalar u = (max_u - min_u) / (sample_u_number - 1) * i + min_u;
                    if(sample_v_number < 5)
                    {
                        for(int j = 0; j < sample_v_number; ++j)
                        {
                            const nous::scalar v = (max_v - min_v) / (sample_v_number - 1) * j + min_v;
                            m_sampled_surface_position[i][j] = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                            m_sampled_surface_normal[i][j] = tinynurbs::surfaceNormal(*m_inner_surface, u, v);
                            m_sampled_surface_uv[i][j] = nous::vec2(u, v);
                        }
                    }
                    else
                    {
                        tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sample_v_number), [&](tbb::blocked_range<unsigned int> p) -> void {
                            for(unsigned int j = p.begin(), num2 = p.end(); j < num2; ++j)
                            {
                                const nous::scalar v = (max_v - min_v) / (sample_v_number - 1) * j + min_v;
                                m_sampled_surface_position[i][j] = tinynurbs::surfacePoint(*m_inner_surface, u, v);
                                m_sampled_surface_normal[i][j] = tinynurbs::surfaceNormal(*m_inner_surface, u, v);
                                m_sampled_surface_uv[i][j] = nous::vec2(u, v);
                            }
                        });
                    }
                }
            });
        }

        m_dirty = false;
    }
}

void NurbsAlgebraSurface::_simple_generate_render_mesh(IRenderable* render_data)
{
    _simple_sample_surface();

    std::vector<float>& position = render_data->get_render_position();
    std::vector<float>& normal = render_data->get_render_normal();
    std::vector<float>& uv = render_data->get_render_uv();
    std::vector<unsigned int>& lines = render_data->get_render_lines_index();
    std::vector<SubmeshData>& submeshes = render_data->get_render_submesh();

    const size_t basic_position_number = position.size() / 3;
    const size_t basic_lines_number = lines.size();

    for(int i = 0; i < m_sampled_surface_position.size(); ++i)
    {
        for(int j = 0; j < m_sampled_surface_position[i].size(); ++j)
        {
            position.emplace_back(m_sampled_surface_position[i][j].x());
            position.emplace_back(m_sampled_surface_position[i][j].y());
            position.emplace_back(m_sampled_surface_position[i][j].z());
            normal.emplace_back(m_sampled_surface_normal[i][j].x());
            normal.emplace_back(m_sampled_surface_normal[i][j].y());
            normal.emplace_back(m_sampled_surface_normal[i][j].z());
            uv.emplace_back(0.0);
            uv.emplace_back(0.0);
        }
    }

    const int sample_rows = m_sampled_surface_position.size();
    const int sample_cols = m_sampled_surface_position.front().size();

    for(int i = 0; i < sample_rows - 1; ++i)
    {
        for(int j = 0; j < sample_cols - 1; ++j)
        {
            const size_t p0 = basic_position_number + i * sample_cols + j;
            const size_t p1 = basic_position_number + i * sample_cols + (j + 1);
            const size_t p2 = basic_position_number + (i + 1) * sample_cols + j;
            const size_t p3 = basic_position_number + (i + 1) * sample_cols + (j + 1);

            if(m_wireframe_type & ENurbsSurfaceWireFrameShowType::U_Boundary)
            {
                if(i == 0)
                {
                    lines.emplace_back(p0);
                    lines.emplace_back(p1);
                }

                if(i == sample_rows - 2)
                {
                    lines.emplace_back(p2);
                    lines.emplace_back(p3);
                }
            }

            if(m_wireframe_type & ENurbsSurfaceWireFrameShowType::V_Boundary)
            {
                if(j == 0)
                {
                    lines.emplace_back(p0);
                    lines.emplace_back(p2);
                }

                if(j == sample_cols - 2)
                {
                    lines.emplace_back(p1);
                    lines.emplace_back(p3);
                }
            }

            if(m_wireframe_type & ENurbsSurfaceWireFrameShowType::U_Inside)
            {
                if(i > 0)
                {
                    lines.emplace_back(p0);
                    lines.emplace_back(p1);
                }
            }

            if(m_wireframe_type & ENurbsSurfaceWireFrameShowType::V_Inside)
            {
                if(j > 0)
                {
                    lines.emplace_back(p0);
                    lines.emplace_back(p2);
                }
            }
        }
    }

    submeshes.emplace_back("Line", basic_lines_number, lines.size() - basic_lines_number, SubmeshData::Topology::Lines, this->shared_from_this());
}

void NurbsAlgebraSurface::get_sampled_triangles_arr(std::vector<nous::pos_t>& positions, std::vector<nous::vec3i>& triangles,
                                                    std::vector<nous::vec3>* normals, std::vector<nous::vec2>* uvs)
{
    _sample_surface();

    positions.clear();
    triangles.clear();
    if(normals)
    {
        normals->clear();
    }
    if(uvs)
    {
        uvs->clear();
    }

    for(int i = 0; i < m_sampled_surface_position.size(); ++i)
    {
        positions.insert(positions.end(), m_sampled_surface_position[i].begin(), m_sampled_surface_position[i].end());
        if(normals)
        {
            normals->insert(normals->end(), m_sampled_surface_normal[i].begin(), m_sampled_surface_normal[i].end());
        }
        if(uvs)
        {
            uvs->insert(uvs->end(), m_sampled_surface_uv[i].begin(), m_sampled_surface_uv[i].end());
        }
    }

    const int sample_rows = m_sampled_surface_position.size();
    const int sample_cols = m_sampled_surface_position.front().size();

    for(int i = 0; i < sample_rows - 1; ++i)
    {
        for(int j = 0; j < sample_cols - 1; ++j)
        {
            const size_t p0 = i * sample_cols + j;
            const size_t p1 = i * sample_cols + (j + 1);
            const size_t p2 = (i + 1) * sample_cols + j;
            const size_t p3 = (i + 1) * sample_cols + (j + 1);

            triangles.emplace_back(p0, p1, p2);
            triangles.emplace_back(p1, p3, p2);
        }
    }
}

std::vector<std::vector<nous::pos_t>> NurbsAlgebraSurface::get_sampled_point_arr()
{
    // 尝试下自适应采样吧？不要急
    _sample_surface();
    return m_sampled_surface_position;
}

std::vector<std::vector<nous::pos_t>> NurbsAlgebraSurface::get_sampled_point_arr(const unsigned int u_count, const unsigned int v_count,
                                                                                 const nous::scalar u_min, const nous::scalar u_max,
                                                                                 const nous::scalar v_min, const nous::scalar v_max) const
{
    return _sample_surface(u_count, v_count, u_min, u_max, v_min, v_max);
}

std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> NurbsAlgebraSurface::get_surface_elements()
{
    _sample_surface();
    return m_boundary_curve_elements;
}

void NurbsAlgebraSurface::transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat)
{
    for(int i = 0; i < m_inner_surface->control_points.rows(); ++i)
    {
        for(int j = 0; j < m_inner_surface->control_points.cols(); ++j)
        {
            auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
            if(transformed_set.find(wrapped) != transformed_set.end())
            {
                continue;
            }
            nous::vec3 ori_pos = wrapped->get_position();
            nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
            nous::vec4 trans_pos4 = trans_mat * ori_pos4;
            nous::vec3 trans_pos = trans_pos4.head<3>() / trans_pos4.w();
            wrapped->set_position(trans_pos);
            transformed_set.insert(wrapped);
        }
    }

    dirty();
}

std::shared_ptr<INurbsBase> NurbsAlgebraSurface::clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                                       const nous::mat4& trans_mat)
{
    std::shared_ptr<NurbsAlgebraSurface> clone_surface = std::make_shared<NurbsAlgebraSurface>();
    clone_surface->m_inner_surface->weights.copy(m_inner_surface->weights);
    clone_surface->m_inner_surface->degree_u = m_inner_surface->degree_u;
    clone_surface->m_inner_surface->degree_v = m_inner_surface->degree_v;
    clone_surface->m_inner_surface->knots_u.insert(clone_surface->m_inner_surface->knots_u.end(), m_inner_surface->knots_u.begin(),
                                                   m_inner_surface->knots_u.end());
    clone_surface->m_inner_surface->knots_v.insert(clone_surface->m_inner_surface->knots_v.end(), m_inner_surface->knots_v.begin(),
                                                   m_inner_surface->knots_v.end());

    const unsigned int rows = m_inner_surface->control_points.rows();
    const unsigned int cols = m_inner_surface->control_points.cols();
    clone_surface->m_inner_surface->control_points.resize(rows, cols, nullptr);
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
            if(remaped_map.find(wrapped) == remaped_map.end())
            {
                nous::vec3 ori_pos = wrapped->get_position();
                nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
                nous::vec4 trans_pos4 = trans_mat * ori_pos4;
                nous::vec3 trans_pos = trans_pos4.head<3>() / trans_pos4.w();
                SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>(trans_pos);
                remaped_map.insert({ wrapped, nurbs_point });
            }
            auto& new_wrapped = remaped_map[wrapped];
            new_wrapped->m_parents.emplace_back(clone_surface);
            clone_surface->m_inner_surface->control_points(i, j) = new_wrapped;
        }
    }
    clone_surface->m_dirty = true;
    return clone_surface;
}

void NurbsAlgebraSurface::dirty()
{
    m_dirty = true;
    if(m_parent_ex)
    {
        m_parent_ex->dirty();
    }

    if(m_parent_in)
    {
        m_parent_in->dirty();
    }
}

void NurbsAlgebraSurface::set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) { m_parent_ex = parent; }

void NurbsAlgebraSurface::set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) { m_parent_in = parent; }

std::pair<std::shared_ptr<NurbsTrimmedSurface>, std::shared_ptr<NurbsTrimmedSurface>> NurbsAlgebraSurface::get_parent() const
{
    return std::make_pair(m_parent_ex, m_parent_in);
}

bool NurbsAlgebraSurface::is_surface_coplane(nous::vec3& coplane_normal)
{
    auto&& control_points_array = get_surface_control_points_1D();
    std::vector<nous::pos_t> ctp_position_arr;
    for(auto& ctp: control_points_array)
    {
        ctp_position_arr.emplace_back(ctp->m_point);
    }

    bool is_coplane = topo::calc_point_set_topo(ctp_position_arr, coplane_normal) == topo::EPointSetTopo::Coplane;

    return is_coplane;
}

std::vector<std::vector<SketchNurbsPointPtr>> NurbsAlgebraSurface::get_surface_control_points()
{
    std::vector<std::vector<SketchNurbsPointPtr>> control_points;
    for(int i = 0; i < m_inner_surface->control_points.rows(); ++i)
    {
        std::vector<SketchNurbsPointPtr>& control_point_row = control_points.emplace_back();
        for(int j = 0; j < m_inner_surface->control_points.cols(); ++j)
        {
            auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
            control_point_row.emplace_back(wrapped);
        }
    }

    return control_points;
}

std::vector<SketchNurbsPointPtr> NurbsAlgebraSurface::get_surface_control_points_1D()
{
    std::vector<SketchNurbsPointPtr> control_points;
    for(int i = 0; i < m_inner_surface->control_points.rows(); ++i)
    {
        for(int j = 0; j < m_inner_surface->control_points.cols(); ++j)
        {
            auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
            control_points.emplace_back(wrapped);
        }
    }

    return control_points;
}

};// namespace sketch