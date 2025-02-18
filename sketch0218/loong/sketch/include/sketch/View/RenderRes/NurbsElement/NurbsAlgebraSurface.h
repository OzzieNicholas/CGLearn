#pragma once

#include "INurbsSurfaceBase.h"
#include "NurbsPoint.h"

namespace sketch
{

class NurbsTrimmedSurface;
class NurbsAlgebraSurfaceEdgeElement;

enum class EAlgebraSurfaceBoundaryType : uint8_t
{
    U0 = 0,
    V1 = 1,
    U1 = 2,
    V0 = 3
};

enum class ENurbsSurfaceRenderLevel : uint8_t
{
    WireFrame,
    Solid
};

enum ENurbsSurfaceWireFrameShowType : uint8_t
{
    None = 0x00,
    U_Boundary = 0x01,
    V_Boundary = 0x02,
    U_Inside = 0x04,
    V_Inside = 0x08,
    UV_Boundary = U_Boundary | V_Boundary,
    UV_Inside = U_Inside | V_Inside,
    UV_Boundary_With_U_Inside = U_Boundary | V_Boundary | U_Inside,
    UV_Boundary_With_V_Inside = U_Boundary | V_Boundary | V_Inside,
    All = UV_Boundary | UV_Inside,
};

class NurbsAlgebraSurface : public INurbsSurfaceBase
{
public:
    NurbsAlgebraSurface() : INurbsSurfaceBase(EElementType::AlgebraSurface)
    {
        m_inner_surface = std::make_shared<tinynurbs::RationalSurface<nous::scalar>>();
    }

    ~NurbsAlgebraSurface();

    virtual void generate_render_mesh(IRenderable* render_data) override;

    virtual nous::AABB3 get_bounding_box() override;

    virtual void save(SketchSerializer& serializer) override;

    virtual void load(SketchSerializer& serializer) override;

    virtual bool is_valid() override;

    virtual void update_surface_data(std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> other);

    const std::vector<std::vector<nous::pos_t>>& get_surface_bounding_lines();

    std::vector<nous::pos_t> get_surface_bounding_lines_1D();

    virtual void get_sampled_triangles_arr(std::vector<nous::pos_t>& positions, std::vector<nous::vec3i>& triangles,
                                           std::vector<nous::vec3>* normals = nullptr, std::vector<nous::vec2>* uvs = nullptr);

    std::vector<std::vector<nous::pos_t>> get_sampled_point_arr();

    std::vector<std::vector<nous::pos_t>> get_sampled_point_arr(const unsigned int u_count, const unsigned int v_count,
                                                                const nous::scalar u_min = nous::scalar(0.0),
                                                                const nous::scalar u_max = nous::scalar(1.0),
                                                                const nous::scalar v_min = nous::scalar(0.0),
                                                                const nous::scalar v_max = nous::scalar(1.0)) const;

    std::vector<std::vector<SketchNurbsPointPtr>> get_surface_control_points() override;

    std::vector<SketchNurbsPointPtr> get_surface_control_points_1D() override;

    inline unsigned int get_surface_degree_u() const { return m_inner_surface->degree_u; }

    inline unsigned int get_surface_degree_v() const { return m_inner_surface->degree_v; }

    inline std::vector<nous::scalar> get_surface_knots_u() const { return m_inner_surface->knots_u; }

    inline std::vector<nous::scalar> get_surface_knots_v() const { return m_inner_surface->knots_v; }

    inline std::vector<std::vector<nous::scalar>> get_surface_weights() const
    {
        std::vector<std::vector<nous::scalar>> weights;
        for(int i = 0; i < m_inner_surface->weights.rows(); ++i)
        {
            std::vector<nous::scalar>& weights_row = weights.emplace_back();
            for(int j = 0; j < m_inner_surface->weights.cols(); ++j)
            {
                weights_row.emplace_back(m_inner_surface->weights(i, j));
            }
        }

        return weights;
    }

    inline nous::pos_t get_position(const nous::scalar u, const nous::scalar v) const
    {
        assert(u >= 0.0 && u <= 1.0);
        assert(v >= 0.0 && v <= 1.0);
        return tinynurbs::surfacePoint(*m_inner_surface, u, v);
    }

    std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> get_surface_elements();

    inline void update_surface_control_points_1D(const std::vector<nous::pos_t>& control_points)
    {
        if(m_inner_surface->control_points.rows() * m_inner_surface->control_points.cols() != control_points.size())
        {
            return;
        }

        for(int i = 0; i < m_inner_surface->control_points.rows(); ++i)
        {
            for(int j = 0; j < m_inner_surface->control_points.cols(); ++j)
            {
                auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(i, j));
                wrapped->set_position(control_points[i * m_inner_surface->control_points.cols() + j]);
            }
        }

        m_dirty = true;
    }

    inline void update_curve_control_point(int row, int col, const nous::pos_t& pos)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(row, col));
        wrapped->set_position(pos);
        m_dirty = true;
    }

    inline size_t get_rows() const { return m_inner_surface->control_points.rows(); }

    inline size_t get_cols() const { return m_inner_surface->control_points.cols(); }

    virtual void transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    virtual std::shared_ptr<INurbsBase> clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                              const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    inline void set_render_level(const ENurbsSurfaceRenderLevel level, int sample_count_multiple_u = -1, int sample_count_multiple_v = -1)
    {
        m_render_level = level;
        m_sample_count_multiple_u = sample_count_multiple_u >= 1 ? sample_count_multiple_u : 1;
        m_sample_count_multiple_v = sample_count_multiple_v >= 1 ? sample_count_multiple_v : 1;
        dirty();
    }

    inline void set_render_wireframe_type(ENurbsSurfaceWireFrameShowType wireframe_type) { m_wireframe_type = wireframe_type; }

    ENurbsCloseType get_surface_u_close_type();

    ENurbsCloseType get_surface_v_close_type();

    bool is_surface_u_closed();

    bool is_surface_v_closed();

    inline void set_surface_u_range(const nous::scalar min_u, const nous::scalar max_u)
    {
        m_min_u = min_u;
        m_max_u = max_u;
    }

    inline void set_surface_v_range(const nous::scalar min_v, const nous::scalar max_v)
    {
        m_min_v = min_v;
        m_max_v = max_v;
    }

    inline std::pair<nous::scalar, nous::scalar> get_surface_u_range() const { return std::make_pair(m_min_u, m_max_u); }

    inline std::pair<nous::scalar, nous::scalar> get_surface_v_range() const { return std::make_pair(m_min_v, m_max_v); }

    inline std::pair<nous::scalar, nous::scalar> get_surface_u_range_with_limit()
    {
        ENurbsCloseType u_close_type = get_surface_u_close_type();
        nous::scalar fitted_min_u = m_min_u;
        nous::scalar fitted_max_u = m_max_u;
        if(u_close_type == ENurbsCloseType::SmoothClose)
        {
            fitted_min_u = std::max(fitted_min_u, m_inner_surface->knots_u[m_inner_surface->degree_u]);
            fitted_max_u = std::min(fitted_max_u, m_inner_surface->knots_u[m_inner_surface->knots_u.size() - m_inner_surface->degree_u - 1]);
        }
        return std::make_pair(fitted_min_u, fitted_max_u);
    }

    inline std::pair<nous::scalar, nous::scalar> get_surface_v_range_with_limit()
    {
        ENurbsCloseType v_close_type = get_surface_v_close_type();
        nous::scalar fitted_min_v = m_min_v;
        nous::scalar fitted_max_v = m_max_v;
        if(v_close_type == ENurbsCloseType::SmoothClose)
        {
            fitted_min_v = std::max(fitted_min_v, m_inner_surface->knots_v[m_inner_surface->degree_v]);
            fitted_max_v = std::min(fitted_max_v, m_inner_surface->knots_v[m_inner_surface->knots_v.size() - m_inner_surface->degree_v - 1]);
        }
        return std::make_pair(fitted_min_v, fitted_max_v);
    }

    inline void replace_surface_control_point(unsigned int row_idx, unsigned int col_idx, SketchNurbsPointPtr ctp_ptr)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_surface->control_points(row_idx, col_idx));
        for(int i = 0; i < wrapped->m_parents.size(); ++i)
        {
            if(!wrapped->m_parents[i].expired() && wrapped->m_parents[i].lock() == this->to_nurbs_algebra_surface())
            {
                wrapped->m_parents.erase(wrapped->m_parents.begin() + i);
                break;
            }
        }
        m_inner_surface->control_points(row_idx, col_idx) = ctp_ptr;
        ctp_ptr->m_parents.emplace_back(this->to_nurbs_algebra_surface());
    }

    virtual void dirty() override;

    void set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    void set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    std::pair<std::shared_ptr<NurbsTrimmedSurface>, std::shared_ptr<NurbsTrimmedSurface>> get_parent() const;

    virtual bool is_surface_coplane(nous::vec3& coplane_normal) override;

private:
    void _sample_surface();

    std::vector<std::vector<nous::pos_t>> _sample_surface(const unsigned int u_count, const unsigned int v_count,
                                                          const nous::scalar u_min = nous::scalar(0.0), const nous::scalar u_max = nous::scalar(1.0),
                                                          const nous::scalar v_min = nous::scalar(0.0),
                                                          const nous::scalar v_max = nous::scalar(1.0)) const;

    void _simple_sample_surface();

    void _simple_generate_render_mesh(IRenderable* render_data);

private:
    bool m_boundary_dirty = false;

    std::shared_ptr<tinynurbs::RationalSurface<nous::scalar>> m_inner_surface;

    std::vector<std::vector<nous::pos_t>> m_sampled_surface_position;
    std::vector<std::vector<nous::vec3>> m_sampled_surface_normal;
    std::vector<std::vector<nous::vec2>> m_sampled_surface_uv;

    std::vector<std::vector<nous::pos_t>> m_sampled_surface_boundary;

    ENurbsSurfaceRenderLevel m_render_level = ENurbsSurfaceRenderLevel::Solid;

    int m_sample_count_multiple_u = 1;
    int m_sample_count_multiple_v = 1;

    ENurbsSurfaceWireFrameShowType m_wireframe_type = ENurbsSurfaceWireFrameShowType::All;

    nous::scalar m_min_u = nous::scalar(0.0);
    nous::scalar m_max_u = nous::scalar(1.0);
    nous::scalar m_min_v = nous::scalar(0.0);
    nous::scalar m_max_v = nous::scalar(1.0);

    std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> m_boundary_curve_elements;
    std::shared_ptr<NurbsTrimmedSurface> m_parent_ex = nullptr;
    std::shared_ptr<NurbsTrimmedSurface> m_parent_in = nullptr;
};

using NurbsAlgebraSurfacePtr = std::shared_ptr<NurbsAlgebraSurface>;

};// namespace sketch