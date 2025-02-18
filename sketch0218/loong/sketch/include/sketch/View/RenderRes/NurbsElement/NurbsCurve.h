#pragma once

#include "../../../Nurbs/tinynurbs.h"
#include "INurbsBase.h"
#include "NurbsPoint.h"
#include "nous_mesh/mesh.h"

namespace sketch
{

class NurbsCurve;
using NurbsCurvePtr = std::shared_ptr<NurbsCurve>;

enum class ENurbsCurveType
{
    Fitting,
    Control
};

class NurbsCurve : public INurbsBase
{
public:
    NurbsCurve() : INurbsBase(EElementType::Curve)
    {
        m_curve_type = ENurbsCurveType::Control;
        m_inner_curve = std::make_shared<tinynurbs::RationalCurve<nous::scalar>>();
    }

    ~NurbsCurve()
    {
        for(int i = 0; i < m_inner_curve->control_points.size(); ++i)
        {
            auto&& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
            wrapped->remove_parent(this);
        }
    }

    void update_curve_data(const unsigned int degree, const std::vector<nous::pos_t>& fitted_point_arr,
                           const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr,
                           const std::vector<nous::scalar>& weight_arr);

    void update_curve_data(const unsigned int degree, const std::vector<SketchNurbsPointPtr>& control_point_arr, const std::vector<float>& knot_arr,
                           const std::vector<nous::scalar>& weight_arr);

    virtual void generate_render_mesh(IRenderable* render_data) override;

    std::pair<NurbsCurvePtr, NurbsCurvePtr> split_curve_at_u(const nous::scalar u);

    void update_curve_data(std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> other_curve);

    void update_curve_data(tinynurbs::RationalCurve<nous::scalar> other_curve);

    inline void replace_curve_control_point(unsigned int idx, SketchNurbsPointPtr ctp_ptr)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[idx]);
        for(int i = 0; i < wrapped->m_parents.size(); ++i)
        {
            if(!wrapped->m_parents[i].expired() && wrapped->m_parents[i].lock() == this->to_nurbs_curve())
            {
                wrapped->m_parents.erase(wrapped->m_parents.begin() + i);
                break;
            }
        }
        m_inner_curve->control_points[idx] = ctp_ptr;
        ctp_ptr->m_parents.emplace_back(this->to_nurbs_curve());
    }

    nous::pos_t get_position(const float u) const;

    nous::vec3 get_derivs(const float u, const size_t num_der) const;

    inline void update_curve_control_point(int idx, const nous::pos_t& pos)
    {
        if(idx < 0 || idx >= m_inner_curve->control_points.size())
        {
            return;
        }

        SketchNurbsPointPtr wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[idx]);
        wrapped->set_position(pos);

        if(m_curve_type == ENurbsCurveType::Fitting)
        {
            // 重新生成一批内插点
            for(int i = 0; i < m_origin_fitted_position.size(); ++i)
            {
                nous::scalar u = m_inner_curve->knots[m_inner_curve->degree + i];
                m_origin_fitted_position[i] = tinynurbs::curvePoint(*m_inner_curve, u);
            }
        }

        dirty();
    }

    inline void update_curve_control_points(const std::vector<nous::pos_t>& control_points)
    {
        if(control_points.size() != m_inner_curve->control_points.size())
        {
            return;
        }

        for(int i = 0; i < control_points.size(); ++i)
        {
            auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
            wrapped->set_position(control_points[i]);
        }

        if(m_curve_type == ENurbsCurveType::Fitting)
        {
            // 重新生成一批内插点
            for(int i = 0; i < m_origin_fitted_position.size(); ++i)
            {
                nous::scalar u = m_inner_curve->knots[m_inner_curve->degree + i];
                m_origin_fitted_position[i] = tinynurbs::curvePoint(*m_inner_curve, u);
            }
        }

        dirty();
    }

    inline std::vector<nous::pos_t> get_fitting_points() const { return m_origin_fitted_position; }

    virtual nous::AABB3 get_bounding_box() override;

    virtual void save(SketchSerializer& serializer) override;

    virtual void load(SketchSerializer& serializer) override;

    std::vector<SketchNurbsPointPtr> get_curve_control_points() const;

    std::vector<nous::scalar> get_curve_weights() const;

    std::vector<nous::scalar> get_curve_knots() const;

    unsigned int get_curve_degree() const;

    inline ENurbsCurveType get_curve_type() const { return m_curve_type; }

    inline void set_curve_type(ENurbsCurveType type) { m_curve_type = type; }

    ENurbsCloseType get_curve_close_type();

    std::vector<nous::pos_t> get_sampled_point_arr();

    std::vector<nous::pos_t> get_sampled_point_arr(const unsigned int count);

    std::vector<nous::pos_t> get_sampled_point_arr(const nous::scalar min_u, const nous::scalar max_u, const unsigned int count = 100);

    virtual bool is_valid() override;

    bool is_curve_closed();

    virtual std::shared_ptr<INurbsBase> clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                              const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    inline void set_parent(std::shared_ptr<INurbsBase> parent) { m_parent = parent; }

    inline std::shared_ptr<INurbsBase> get_parent() const { return m_parent.expired() ? nullptr : m_parent.lock(); }

    virtual void dirty() override
    {
        m_dirty = true;
        get_curve_close_type();
        if(!m_parent.expired())
        {
            m_parent.lock()->dirty();
        }
    }

    inline void set_curve_range(const nous::scalar min_u, const nous::scalar max_u)
    {
        m_min_u = std::max(static_cast<nous::scalar>(0.0), min_u);
        m_max_u = std::min(static_cast<nous::scalar>(1.0), max_u);
    }

    inline std::pair<nous::scalar, nous::scalar> get_curve_range() const { return std::make_pair(m_min_u, m_max_u); }

    inline std::pair<nous::scalar, nous::scalar> get_curve_range_with_limit()
    {
        nous::scalar fitted_min_u = m_min_u;
        nous::scalar fitted_max_u = m_max_u;
        if(get_curve_close_type() == ENurbsCloseType::SmoothClose)
        {
            fitted_min_u = std::max(fitted_min_u, m_inner_curve->knots[m_inner_curve->degree]);
            fitted_max_u = std::min(fitted_max_u, m_inner_curve->knots[m_inner_curve->knots.size() - 1 - m_inner_curve->degree]);
        }
        return std::make_pair(fitted_min_u, fitted_max_u);
    }

    virtual void transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    inline const std::vector<nous::pos_t>& get_curve_position_arr() { return m_sampled_curve_position; }

private:
    void _sample_curve(unsigned int count = 100);

    std::vector<nous::pos_t> _sample_curve(const nous::scalar min_u, const nous::scalar max_u, const unsigned int count = 100);

    void _generate_fit_point();

private:
    std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> m_inner_curve;

    std::vector<nous::pos_t> m_sampled_curve_position;

    std::vector<nous::pos_t> m_origin_fitted_position;

    ENurbsCloseType m_closed = ENurbsCloseType::Open;

    ENurbsCurveType m_curve_type;

    std::weak_ptr<INurbsBase> m_parent;

    nous::scalar m_min_u = 0.0;
    nous::scalar m_max_u = 1.0;
};

};// namespace sketch