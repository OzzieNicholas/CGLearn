#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_BSpline.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

namespace sketch
{
Controller_Create_BSpline::Controller_Create_BSpline(const std::wstring& name) : Controller_Main(name)
{
    m_bspline_step = EBSplineStep::PENDING;
    m_need_update_reference = false;
}

Controller_Create_BSpline::~Controller_Create_BSpline() {}

SketchParamMap Controller_Create_BSpline::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "distance");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Create_BSpline::active(const SketchParamMap& props)
{
    Controller_Main::active(props);

    set_tip(L"样条曲线");
    set_cursor(Define::CURSOR_ARROW);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer = view->inferHelper();
    infer->set_infer_filter_type(InferType::ALL_TYPE);
    m_bspline_step = EBSplineStep::PENDING;
    m_bspline_mode = EBSplineCreateMode::Control;
    m_cached_curve = std::make_shared<NurbsCurve>();
}

void Controller_Create_BSpline::deactive()
{
    Controller_Main::deactive();

    _finish_nurbs_curve();
}

void Controller_Create_BSpline::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_bspline_step == EBSplineStep::FIXING_CURVE)
    {
        _finish_nurbs_curve();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Create_BSpline::enter(const SketchParamMap& props) { Controller_Main::enter(props); }

void Controller_Create_BSpline::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(shift && m_bspline_step == EBSplineStep::PENDING)
    {
        if(m_bspline_mode == EBSplineCreateMode::Control)
        {
            m_bspline_mode = EBSplineCreateMode::Fit;
        }
        else
        {
            m_bspline_mode = EBSplineCreateMode::Control;
        }
    }
}

void Controller_Create_BSpline::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Create_BSpline::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_bspline_step == EBSplineStep::PENDING)
    {
        m_bspline_step = EBSplineStep::FIXING_CURVE;
        m_need_update_reference = true;
        m_clicked_position_arr.emplace_back(m_hover_pos);
        m_locked_length = -1.0f;
    }
    else if(m_bspline_step == EBSplineStep::FIXING_CURVE)
    {
        if((m_hover_pos - m_clicked_position_arr.back()).norm() > 0.01)
        {
            m_clicked_position_arr.emplace_back(m_hover_pos);
            if(sketch_math::point_equal(m_hover_pos, m_clicked_position_arr.front()))
            {
                _finish_nurbs_curve();
            }
        }
    }

    m_begin_pos = m_hover_pos;
}

void Controller_Create_BSpline::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
    {
        return;
    }

    InferenceHelper* infer_helper = view->inferHelper();
    if(m_bspline_step == EBSplineStep::FIXING_CURVE)
    {
        infer_helper->update_inference(view, x, y, &m_begin_pos);
    }
    else
    {
        infer_helper->update_inference(view, x, y);
    }

    const InferResult& infer = infer_helper->get_infer_result();
    if(infer.infer_type != InferType::NONE)
    {
        m_hover_pos = infer.position;
        if(m_locked_length > 0.f)
        {
            m_hover_pos = m_begin_pos + (m_hover_pos - m_begin_pos).normalized() * m_locked_length;
        }

        if(!m_clicked_position_arr.empty())
        {
            const Engine::Mat4f view_projection_matrix = view->camera()->dvpMatrix();
            nous::vec4 vpos = NousConvert::product_v4(view_projection_matrix, m_clicked_position_arr.front());
            if(vpos.w() > 0.0f)// invisible
            {
                vpos.head<3>() /= vpos.w();
                nous::vec2 npos = (vpos.head<2>() + nous::vec2(1.f, 1.f)) * 0.5;
                npos.x() *= view->camera()->viewport().z;
                npos.y() *= view->camera()->viewport().w;
                float distance2 = (npos - nous::vec2(x, y)).squaredNorm();
                if(distance2 < 10.0)
                {
                    m_hover_pos = m_clicked_position_arr.front();
                }
            }
        }
    }

    if(!m_clicked_position_arr.empty())
    {
        std::vector<nous::pos_t> temp_array;
        temp_array.insert(temp_array.end(), m_clicked_position_arr.begin(), m_clicked_position_arr.end());
        if((m_hover_pos - m_clicked_position_arr.back()).norm() > 0.01)
        {
            temp_array.emplace_back(m_hover_pos);
        }

        if(temp_array.size() > 1)
        {
            if(m_bspline_mode == EBSplineCreateMode::Control)
            {
                _generate_curve_by_control_points(temp_array);
            }
            else
            {
                _generate_curve_by_fit_points(temp_array);
            }
        }
    }

    view->dirtyRT();
}

void Controller_Create_BSpline::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_BSpline::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_BSpline::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_cached_curve->is_valid())
    {
        const auto&& curve_point_arr = m_cached_curve->get_sampled_point_arr();
        int loop_count = m_cached_curve->is_curve_closed() ? curve_point_arr.size() : curve_point_arr.size() - 1;
        for(int i = 0; i < loop_count; ++i)
        {
            const nous::pos_t prev = curve_point_arr[i];
            const nous::pos_t next = curve_point_arr[(i + 1) % curve_point_arr.size()];
            Engine::Vec3f begin(prev.x(), prev.y(), prev.z());
            Engine::Vec3f end(next.x(), next.y(), next.z());
            view->batchRenderLine(2.f, 1.f, Engine::Vec3f::BLACK, begin, end, 0xffff, 1, false, true);
        }
    }

    for(int i = 0; i < m_clicked_position_arr.size(); ++i)
    {
        view->batchRenderCtrlPoints(0, false, true, NousConvert::convert(m_clicked_position_arr[i]));
    }
    view->batchRenderCtrlPoints(0, false, true, NousConvert::convert(m_hover_pos));
}

void Controller_Create_BSpline::_generate_curve_by_fit_points(const std::vector<nous::pos_t>& fit_point_arr)
{
    const auto& tmp_curve = nurbs::solve_tridiagonal(fit_point_arr, true, nous::vec3::Zero(), true, nous::vec3::Zero(), ENurbsCloseType::SmoothClose);

    if(tmp_curve)
    {
        m_cached_curve->set_curve_type(ENurbsCurveType::Fitting);
        if(sketch_math::point_equal(fit_point_arr.front(), fit_point_arr.back()))
        {
            m_cached_curve->update_curve_data(tmp_curve);
            m_is_closed_curve = true;
        }
        else
        {
            m_cached_curve->update_curve_data(tmp_curve);
            m_is_closed_curve = false;
        }
    }
}

void Controller_Create_BSpline::_finish_nurbs_curve()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view && m_bspline_step == EBSplineStep::FIXING_CURVE)
    {
        if(m_bspline_mode == EBSplineCreateMode::Control)
        {
            _generate_curve_by_control_points(m_clicked_position_arr);
        }
        else
        {
            _generate_curve_by_fit_points(m_clicked_position_arr);
        }

        SceneDoc* scene = view->doc()->scene();
        RvtBlock rvt_block(scene->rvtId());

        NurbsDoc* new_nurbs_doc = (NurbsDoc*)view->create_obj_in_working_group(Define::OBJ_D5_NURBS);
        auto new_nurbs = new_nurbs_doc->nurbsdata(true);
        if(m_is_closed_curve && nurbs::is_curve_coplane(m_cached_curve))
        {
            NurbsClosedSurfacePtr brep_surface = std::make_shared<NurbsClosedSurface>();
            brep_surface->add_sub_curve_without_check(m_cached_curve);
            NurbsTrimmedSurfacePtr complex_surface = std::make_shared<NurbsTrimmedSurface>();
            complex_surface->set_exterior_ring(brep_surface);
            new_nurbs->add_item(complex_surface);
        }
        else
        {
            new_nurbs->add_item(m_cached_curve);
        }
        m_cached_curve = std::make_shared<NurbsCurve>();
        new_nurbs_doc->dirty();
    }

    m_clicked_position_arr.clear();
    m_bspline_step = EBSplineStep::PENDING;
}

void Controller_Create_BSpline::_generate_curve_by_control_points(std::vector<nous::pos_t>& control_point_arr)
{
    // 基于控制点生成均匀 bspline
    if(control_point_arr.size() > 1)
    {
        if(control_point_arr.size() > 1 && (control_point_arr.front() - control_point_arr.back()).norm() < 0.01)
        {
            std::vector<float> knot_arr;
            const unsigned int degree = std::min(control_point_arr.size() - 1, static_cast<size_t>(3));

            std::vector<nous::pos_t> fitted_control_points;
            fitted_control_points.insert(fitted_control_points.end(), control_point_arr.begin(), control_point_arr.end() - 1);
            fitted_control_points.insert(fitted_control_points.end(), control_point_arr.begin(), control_point_arr.begin() + degree);

            const int sum_knot_count = fitted_control_points.size() + degree + 1;
            const int middle_knot_count = sum_knot_count - (degree + 1) * 2;

            knot_arr.resize(sum_knot_count, 0.0);
            for(int i = 0; i < sum_knot_count; ++i)
            {
                knot_arr[i] = (1.0 / (sum_knot_count - 1)) * (i);
            }
            std::vector<nous::scalar> weight_arr(fitted_control_points.size(), 1.0);
            m_cached_curve->update_curve_data(degree, std::vector<nous::pos_t>(), fitted_control_points, knot_arr, weight_arr);
            m_is_closed_curve = true;
        }
        else
        {
            std::vector<float> knot_arr;
            const unsigned int degree = std::min(control_point_arr.size() - 1, static_cast<size_t>(3));
            const int sum_knot_count = control_point_arr.size() + degree + 1;
            const int middle_knot_count = sum_knot_count - (degree + 1) * 2;

            knot_arr.resize(sum_knot_count, 0.0);
            for(int i = 0; i <= degree; ++i)
            {
                knot_arr[i] = 0.0f;
            }
            for(int i = 0; i < middle_knot_count; ++i)
            {
                knot_arr[i + degree + 1] = (1.0 / (middle_knot_count + 1)) * (i + 1);
            }
            for(int i = 0; i <= degree; ++i)
            {
                knot_arr[degree + 1 + middle_knot_count + i] = 1.0f;
            }
            std::vector<nous::scalar> weight_arr(control_point_arr.size(), 1.0);
            m_cached_curve->update_curve_data(degree, std::vector<nous::pos_t>(), control_point_arr, knot_arr, weight_arr);
            m_is_closed_curve = false;
        }
        m_cached_curve->set_curve_type(ENurbsCurveType::Control);
    }
}

};// namespace sketch