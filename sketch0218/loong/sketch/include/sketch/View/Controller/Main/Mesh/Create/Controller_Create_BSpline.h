#pragma once

#include "../../Controller_Main.h"

namespace sketch
{
class NurbsCurve;
class Controller_Create_BSpline : public Controller_Main
{
public:
    Controller_Create_BSpline(const std::wstring& name);
    ~Controller_Create_BSpline() override;

    virtual SketchParamMap info() const;

    void active(const SketchParamMap& pProps) override;
    void deactive() override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void key_up(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

protected:
    void prepare_render(View* view) override;
    void _generate_curve_by_control_points(std::vector<nous::pos_t>& control_point_arr);
    void _generate_curve_by_fit_points(const std::vector<nous::pos_t>& fit_point_arr);
    void _finish_nurbs_curve();

private:
    nous::vec3 m_begin_pos;
    nous::vec3 m_hover_pos;

    float m_locked_length = 0.0f;

    enum class EBSplineStep
    {
        PENDING = 0,
        FIXING_CURVE = 1,
        FIXING_HEAD_TANGENT = 2,
        FIXING_TAIL_TANGENT = 3
    };

    enum class EBSplineCreateMode
    {
        Control,
        Fit
    };

    EBSplineStep m_bspline_step = EBSplineStep::PENDING;

    EBSplineCreateMode m_bspline_mode = EBSplineCreateMode::Control;

    bool m_need_update_reference = false;

    std::vector<nous::pos_t> m_clicked_position_arr;

    std::shared_ptr<NurbsCurve> m_cached_curve = nullptr;

    bool m_is_closed_curve = false;
};

};// namespace sketch