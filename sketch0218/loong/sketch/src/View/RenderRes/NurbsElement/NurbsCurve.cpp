#include "sketch/View/RenderRes/NurbsElement/NurbsCurve.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"

namespace sketch
{

std::vector<nous::pos_t> NurbsCurve::get_sampled_point_arr()
{
    if(m_dirty)
    {
        _sample_curve();
        m_dirty = false;
    }

    return m_sampled_curve_position;
}

std::vector<nous::pos_t> NurbsCurve::get_sampled_point_arr(const unsigned int count) { return _sample_curve(m_min_u, m_max_u, count); }

std::vector<nous::pos_t> NurbsCurve::get_sampled_point_arr(const nous::scalar min_u, const nous::scalar max_u, const unsigned int count)
{
    return _sample_curve(min_u, max_u, count);
}

void NurbsCurve::generate_render_mesh(IRenderable* render_data)
{
    _sample_curve();

    std::vector<float>& position = render_data->get_render_position();
    std::vector<float>& normal = render_data->get_render_normal();
    std::vector<float>& uv = render_data->get_render_uv();
    std::vector<unsigned int>& lines = render_data->get_render_lines_index();
    std::vector<SubmeshData>& submeshes = render_data->get_render_submesh();

    // position
    size_t begin_vertex = position.size() / 3;
    for(int i = 0; i < m_sampled_curve_position.size(); ++i)
    {
        position.push_back(m_sampled_curve_position[i].x());
        position.push_back(m_sampled_curve_position[i].y());
        position.push_back(m_sampled_curve_position[i].z());
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
    if(m_closed != ENurbsCloseType::Open)
    {
        lines.push_back(lines.back());
        lines.push_back(begin_vertex);
    }

    submeshes.emplace_back("Line", begin_index, lines.size() - begin_index, SubmeshData::Topology::Lines, this->shared_from_this());
}

std::pair<NurbsCurvePtr, NurbsCurvePtr> NurbsCurve::split_curve_at_u(const nous::scalar u)
{
    if(u < 1e-3)
    {
        std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remap_map;
        return std::make_pair(nullptr, clone(remap_map)->to_nurbs_curve());
    }

    if(u > 1 - 1e-3)
    {
        std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remap_map;
        return std::make_pair(clone(remap_map)->to_nurbs_curve(), nullptr);
    }

    auto& result = tinynurbs::curveSplit(*m_inner_curve, u);

    tinynurbs::RationalCurve<nous::scalar> left_curve_core = std::get<0>(result);
    tinynurbs::RationalCurve<nous::scalar> right_curve_core = std::get<1>(result);

    tinynurbs::util::normalize_curve_knots(left_curve_core);
    tinynurbs::util::normalize_curve_knots(right_curve_core);

    NurbsCurvePtr left_curve = std::make_shared<NurbsCurve>();
    NurbsCurvePtr right_curve = std::make_shared<NurbsCurve>();

    left_curve->update_curve_data(left_curve_core);
    right_curve->update_curve_data(right_curve_core);

    return std::make_pair(left_curve, right_curve);
}

void NurbsCurve::_generate_fit_point()
{
    if(m_curve_type == ENurbsCurveType::Fitting && m_inner_curve->control_points.size() > 2)
    {
        m_origin_fitted_position.clear();
        m_origin_fitted_position.resize(m_inner_curve->control_points.size() - 2);
        // 重新生成一批内插点
        for(int i = 0; i < m_origin_fitted_position.size(); ++i)
        {
            nous::scalar u = m_inner_curve->knots[m_inner_curve->degree + i];
            m_origin_fitted_position[i] = tinynurbs::curvePoint(*m_inner_curve, u);
        }
    }
}

void NurbsCurve::transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat)
{
    for(int i = 0; i < m_inner_curve->control_points.size(); ++i)
    {
        auto wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
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

    for(int i = 0; i < m_origin_fitted_position.size(); ++i)
    {
        nous::vec3 ori_pos = m_origin_fitted_position[i];
        nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
        nous::vec4 trans_pos4 = trans_mat * ori_pos4;
        nous::vec3 trans_pos = trans_pos4.head<3>() / trans_pos4.w();
        m_origin_fitted_position[i] = trans_pos;
    }

    dirty();
}

void NurbsCurve::_sample_curve(unsigned int count)
{
    if(m_dirty)
    {
        get_curve_close_type();
        m_sampled_curve_position.clear();

        nous::scalar fitted_min_u = m_min_u;
        nous::scalar fitted_max_u = m_max_u;
        if(m_closed == ENurbsCloseType::SmoothClose)
        {
            fitted_min_u = std::max(m_inner_curve->knots[m_inner_curve->degree], fitted_min_u);
            fitted_max_u = std::min(m_inner_curve->knots[m_inner_curve->knots.size() - m_inner_curve->degree - 1], fitted_max_u);
        }
        fitted_min_u = std::max(fitted_min_u, 0.0f);
        fitted_max_u = std::min(fitted_max_u, 1.0f);

        for(int i = 0; i <= count - 1; ++i)
        {
            const nous::scalar u = fitted_min_u + i * (1.0 / (count - 1)) * (fitted_max_u - fitted_min_u);
            const nous::pos_t position = tinynurbs::curvePoint<nous::scalar>(*m_inner_curve, u);
            m_sampled_curve_position.emplace_back(position);
        }
        m_dirty = false;
    }
}

std::vector<nous::pos_t> NurbsCurve::_sample_curve(const nous::scalar min_u, const nous::scalar max_u, const unsigned int count)
{
    get_curve_close_type();

    if(m_inner_curve->knots.empty())
    {
        return std::vector<nous::pos_t>();
    }

    std::vector<nous::pos_t> result(count, nous::pos_t::Zero());

    nous::scalar fitted_min_u = min_u;
    nous::scalar fitted_max_u = max_u;
    if(m_closed == ENurbsCloseType::SmoothClose)
    {
        fitted_min_u = std::max(m_inner_curve->knots[m_inner_curve->degree], fitted_min_u);
        fitted_max_u = std::min(m_inner_curve->knots[m_inner_curve->knots.size() - m_inner_curve->degree - 1], fitted_max_u);
    }
    fitted_min_u = std::max(fitted_min_u, 0.0f);
    fitted_max_u = std::min(fitted_max_u, 1.0f);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, count), [&](tbb::blocked_range<size_t> r) {
        for(size_t i = r.begin(), num = r.end(); i < num; ++i)
        {
            nous::scalar u = fitted_min_u + i * (1.0 / (count - 1)) * (fitted_max_u - fitted_min_u);
            const nous::pos_t position = tinynurbs::curvePoint<nous::scalar>(*m_inner_curve, u);
            result[i] = position;
        }
    });

    return result;
}

void NurbsCurve::update_curve_data(const unsigned int degree, const std::vector<nous::pos_t>& fitted_point_arr,
                                   const std::vector<nous::pos_t>& control_point_arr, const std::vector<float>& knot_arr,
                                   const std::vector<nous::scalar>& weight_arr)
{
    if(control_point_arr.size() + degree + 1 != knot_arr.size())
    {
        return;
    }

    // 判断闭合类型
    unsigned int shared_point_number = 0;
    if(nurbs::is_curve_sharp_close(degree, control_point_arr, knot_arr))
    {
        // 尖锐闭合首尾共享一个控制点
        shared_point_number = 1;
    }
    if(nurbs::is_curve_smooth_close(degree, control_point_arr, knot_arr, weight_arr))
    {
        // 平滑闭合首尾共享 degree 个控制点
        shared_point_number = degree;
    }

    m_inner_curve->control_points.clear();
    m_inner_curve->knots.clear();
    m_inner_curve->weights.clear();
    m_origin_fitted_position.clear();

    m_inner_curve->control_points.reserve(control_point_arr.size());

    for(int i = 0; i < control_point_arr.size() - shared_point_number; ++i)
    {
        SketchNurbsPointPtr sk_nurbs_point = std::make_shared<SketchNurbsPoint>(control_point_arr[i]);
        sk_nurbs_point->m_parents.emplace_back(this->to_nurbs_curve());
        m_inner_curve->control_points.emplace_back(sk_nurbs_point);
    }

    for(int i = 0; i < shared_point_number; ++i)
    {
        m_inner_curve->control_points.push_back(m_inner_curve->control_points[i]);
    }

    m_inner_curve->degree = degree;
    m_inner_curve->knots.insert(m_inner_curve->knots.end(), knot_arr.begin(), knot_arr.end());
    m_inner_curve->weights.insert(m_inner_curve->weights.end(), weight_arr.begin(), weight_arr.end());
    m_origin_fitted_position.insert(m_origin_fitted_position.end(), fitted_point_arr.begin(), fitted_point_arr.end());

    dirty();
    get_curve_close_type();
}

void NurbsCurve::update_curve_data(const unsigned int degree, const std::vector<SketchNurbsPointPtr>& control_point_arr,
                                   const std::vector<float>& knot_arr, const std::vector<nous::scalar>& weight_arr)
{
    if(control_point_arr.size() + degree + 1 != knot_arr.size())
    {
        return;
    }

    m_inner_curve->control_points.clear();
    m_inner_curve->knots.clear();
    m_inner_curve->weights.clear();
    m_origin_fitted_position.clear();

    m_inner_curve->control_points.insert(m_inner_curve->control_points.end(), control_point_arr.begin(), control_point_arr.end());
    std::unordered_set<tinynurbs::NurbsPointPtr<nous::scalar>> tmp_set(m_inner_curve->control_points.begin(), m_inner_curve->control_points.end());
    for(auto& ori_cpt: tmp_set)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(ori_cpt);
        wrapped->m_parents.emplace_back(this->to_nurbs_curve());
    }
    m_inner_curve->degree = degree;
    m_inner_curve->knots.insert(m_inner_curve->knots.end(), knot_arr.begin(), knot_arr.end());
    m_inner_curve->weights.insert(m_inner_curve->weights.end(), weight_arr.begin(), weight_arr.end());

    _generate_fit_point();

    dirty();
}

std::vector<SketchNurbsPointPtr> NurbsCurve::get_curve_control_points() const
{
    std::vector<SketchNurbsPointPtr> result;
    if(!m_inner_curve)
    {
        return result;
    }

    for(int i = 0; i < m_inner_curve->control_points.size(); ++i)
    {
        auto wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
        result.push_back(wrapped);
    }
    return result;
}

void NurbsCurve::update_curve_data(std::shared_ptr<tinynurbs::RationalCurve<nous::scalar>> other_curve) { update_curve_data(*other_curve); }

void NurbsCurve::update_curve_data(tinynurbs::RationalCurve<nous::scalar> other_curve)
{
    m_inner_curve->control_points.clear();
    m_inner_curve->knots.clear();
    m_inner_curve->weights.clear();
    m_origin_fitted_position.clear();

    for(int i = 0; i < other_curve.control_points.size(); ++i)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(other_curve.control_points[i]);
        // 防止是 tinynurbs 内部生成的对象，要注意多转一道
        if(!wrapped)
        {
            wrapped = std::make_shared<SketchNurbsPoint>(other_curve.control_points[i]->m_point);
        }
        wrapped->m_parents.emplace_back(this->to_nurbs_curve());
        m_inner_curve->control_points.emplace_back(wrapped);
    }

    m_inner_curve->degree = other_curve.degree;
    m_inner_curve->knots.insert(m_inner_curve->knots.end(), other_curve.knots.begin(), other_curve.knots.end());
    m_inner_curve->weights.insert(m_inner_curve->weights.end(), other_curve.weights.begin(), other_curve.weights.end());

    _generate_fit_point();
    dirty();
    get_curve_close_type();
}

nous::pos_t NurbsCurve::get_position(const float u) const { return tinynurbs::curvePoint<nous::scalar>(*m_inner_curve, u); }

nous::vec3 NurbsCurve::get_derivs(const float u, const size_t num_der) const
{
    auto&& derivs = tinynurbs::curveDerivatives(*m_inner_curve, num_der, u);
    return derivs.back();
}

std::vector<nous::scalar> NurbsCurve::get_curve_weights() const
{
    if(!m_inner_curve)
    {
        return std::vector<nous::scalar>();
    }

    return m_inner_curve->weights;
}

std::vector<nous::scalar> NurbsCurve::get_curve_knots() const
{
    if(!m_inner_curve)
    {
        return std::vector<nous::scalar>();
    }

    return m_inner_curve->knots;
}

unsigned int NurbsCurve::get_curve_degree() const
{
    if(!m_inner_curve)
    {
        return 0;
    }

    return m_inner_curve->degree;
}

nous::AABB3 NurbsCurve::get_bounding_box()
{
    _sample_curve();

    nous::AABB3 bounding_box;
    for(int i = 0; i < m_sampled_curve_position.size(); ++i)
    {
        bounding_box.extend(m_sampled_curve_position[i]);
    }
    return bounding_box;
}

void NurbsCurve::save(SketchSerializer& serializer)
{
    serializer.write(this->id(), "id");
    serializer.write((uint32_t)this->type(), "type");
    serializer.write(static_cast<char>(m_closed), "closed");
    serializer.write(m_inner_curve->degree, "degree");
    serializer.write(static_cast<char>(m_curve_type), "curve_type");
    serializer.begin_write_array("control_points");
    for(int i = 0; i < m_inner_curve->control_points.size(); ++i)
    {
        auto wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
        serializer.write_sketch_nurbs_point_ref(wrapped);
    }
    serializer.end_write_array();
    serializer.begin_write_array("knots");
    for(int i = 0; i < m_inner_curve->knots.size(); ++i)
    {
        serializer.write(m_inner_curve->knots[i]);
    }
    serializer.end_write_array();
    serializer.begin_write_array("weights");
    for(int i = 0; i < m_inner_curve->weights.size(); ++i)
    {
        serializer.write(m_inner_curve->weights[i]);
    }
    serializer.end_write_array();
    serializer.begin_write_array("fit_points");
    for(int i = 0; i < m_origin_fitted_position.size(); ++i)
    {
        serializer.advance_write(m_origin_fitted_position[i]);
    }
    serializer.end_write_array();
}

void NurbsCurve::load(SketchSerializer& serializer)
{
    uint32_t id;
    serializer.read(id, "id");
    this->set_id(id);
    char close_flag;
    serializer.read(close_flag, "closed");
    m_closed = static_cast<ENurbsCloseType>(close_flag);
    char curve_type_flag;
    serializer.read(curve_type_flag, "curve_type");
    m_curve_type = static_cast<ENurbsCurveType>(curve_type_flag);
    serializer.read(m_inner_curve->degree, "degree");

    size_t control_point_number = 0;
    serializer.begin_read_array("control_points", control_point_number);
    m_inner_curve->control_points.clear();
    m_inner_curve->control_points.resize(control_point_number);
    for(int i = 0; i < control_point_number; ++i)
    {
        auto& wrapped = serializer.read_sketch_nurbs_point_ref();
        wrapped->m_parents.emplace_back(this->to_nurbs_curve());
        m_inner_curve->control_points[i] = wrapped;
    }
    serializer.end_read_array();

    size_t knot_value_number = 0;
    serializer.begin_read_array("knots", knot_value_number);
    m_inner_curve->knots.clear();
    m_inner_curve->knots.resize(knot_value_number);
    for(int i = 0; i < knot_value_number; ++i)
    {
        serializer.read(m_inner_curve->knots[i]);
    }
    serializer.end_read_array();

    size_t weight_number = 0;
    serializer.begin_read_array("weights", weight_number);
    m_inner_curve->weights.clear();
    m_inner_curve->weights.resize(weight_number);
    for(int i = 0; i < weight_number; ++i)
    {
        serializer.read(m_inner_curve->weights[i]);
    }
    serializer.end_read_array();

    size_t fit_point_number = 0;
    serializer.begin_read_array("fit_points", fit_point_number);
    m_origin_fitted_position.clear();
    m_origin_fitted_position.resize(fit_point_number);
    for(int i = 0; i < fit_point_number; ++i)
    {
        serializer.advance_read(m_origin_fitted_position[i]);
    }
    serializer.end_read_array();

    m_dirty = true;
}

bool NurbsCurve::is_valid() { return tinynurbs::curveIsValid<nous::scalar>(*m_inner_curve); }

ENurbsCloseType NurbsCurve::get_curve_close_type()
{
    if(m_dirty)
    {
        m_closed = ENurbsCloseType::Open;
        // 平滑闭合
        if(tinynurbs::curveIsClosed(*m_inner_curve))
        {
            m_closed = ENurbsCloseType::SmoothClose;
        }

        // 尖锐闭合
        auto&& curve_ctps = m_inner_curve->control_points;
        auto&& curve_knots = m_inner_curve->knots;
        if(sketch_math::point_equal(curve_ctps.front()->m_point, curve_ctps.back()->m_point))
        {
            m_closed = ENurbsCloseType::SharpClose;
            // 第一个和最后一个控制点相等
            // 前 degree + 1 个 knot == 0 且 后 degree + 1 个 knot == 1 是尖锐闭合
            for(int i = 0; i < m_inner_curve->degree; ++i)
            {
                if(!(curve_knots[i] == curve_knots[i + 1] && curve_knots[i] == 0.0))
                {
                    m_closed = ENurbsCloseType::Open;
                    break;
                }

                if(!(curve_knots[curve_knots.size() - i - 1] == curve_knots[curve_knots.size() - i - 2] &&
                     curve_knots[curve_knots.size() - i - 1] == 1.0))
                {
                    m_closed = ENurbsCloseType::Open;
                    break;
                }
            }
        }
    }
    return m_closed;
}

bool NurbsCurve::is_curve_closed()
{
    get_curve_close_type();
    return m_closed != ENurbsCloseType::Open;
}

std::shared_ptr<INurbsBase> NurbsCurve::clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map, const nous::mat4& trans_mat)
{
    std::shared_ptr<NurbsCurve> clone_curve = std::make_shared<NurbsCurve>();
    clone_curve->m_closed = m_closed;
    clone_curve->m_inner_curve->degree = m_inner_curve->degree;
    clone_curve->m_inner_curve->knots.insert(clone_curve->m_inner_curve->knots.end(), m_inner_curve->knots.begin(), m_inner_curve->knots.end());
    clone_curve->m_inner_curve->weights.insert(clone_curve->m_inner_curve->weights.end(), m_inner_curve->weights.begin(),
                                               m_inner_curve->weights.end());
    for(int i = 0; i < m_inner_curve->control_points.size(); ++i)
    {
        auto& wrapped = std::dynamic_pointer_cast<SketchNurbsPoint>(m_inner_curve->control_points[i]);
        if(remaped_map.find(wrapped) == remaped_map.end())
        {
            nous::vec3 ori_pos = m_inner_curve->control_points[i]->m_point;
            nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
            nous::vec4 trans_pos4 = trans_mat * ori_pos4;
            nous::vec3 trans_pos = trans_pos4.head<3>() / trans_pos4.w();
            SketchNurbsPointPtr new_pt = std::make_shared<SketchNurbsPoint>(trans_pos);
            remaped_map.insert({ wrapped, new_pt });
        }
        auto& new_wrapped = remaped_map[wrapped];
        new_wrapped->m_parents.emplace_back(clone_curve);
        clone_curve->m_inner_curve->control_points.emplace_back(new_wrapped);
    }

    for(int i = 0; i < m_origin_fitted_position.size(); ++i)
    {
        nous::vec3 ori_pos = m_origin_fitted_position[i];
        nous::vec4 ori_pos4(ori_pos.x(), ori_pos.y(), ori_pos.z(), 1.0);
        nous::vec4 trans_pos4 = trans_mat * ori_pos4;
        nous::vec3 trans_pos = trans_pos4.head<3>() / trans_pos4.w();
        clone_curve->m_origin_fitted_position.emplace_back(trans_pos);
    }

    clone_curve->set_curve_type(m_curve_type);
    clone_curve->dirty();

    clone_curve->m_min_u = m_min_u;
    clone_curve->m_max_u = m_max_u;

    return clone_curve;
}

};// namespace sketch