#pragma once
#include "../../../../../Document/Scene/Mesh/NurbsDoc.h"
#include "../../../../../Document/Scene/TFObjectDoc.h"
#include "../../../../../View/Controller/Main/Mesh/Controller_Edit.h"
#include "../../../../../View/RenderRes/NurbsData.h"

namespace sketch
{
class Handle;
class Controller_Edit_Nurbs : public Controller_Edit
{
public:
    Controller_Edit_Nurbs(const std::wstring& name);
    ~Controller_Edit_Nurbs();

    virtual SketchParamMap info() const;

    void active(const SketchParamMap& props) override;
    void deactive() override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    virtual void key_down(int key, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    virtual void prepare_render(View* view) override;

private:
    bool _preprocess_nurbs_control_points();
    void _refresh_control_points();
    void _transfer_directly_to_gizmo();
    void _transfer_gizmo_to_directly();
    void _step_back_to_pick_control_point();
    void _step_back_to_pick_element();
    void _try_change_pick_control_point();
    void _try_change_pick_nurbs_item(View* view);

private:
    NurbsDoc* m_picked_doc = nullptr;
    SketchElementPtr m_picked_element = nullptr;
    std::shared_ptr<NurbsData> m_picked_nurbs = nullptr;
    NurbsCurvePtr m_picked_curve = nullptr;
    NurbsAlgebraSurfacePtr m_picked_algebra_surface = nullptr;
    NurbsClosedSurfacePtr m_picked_closed_surface = nullptr;
    NurbsTrimmedSurfacePtr m_picked_trimmed_surface = nullptr;

    std::vector<SketchNurbsPointPtr> m_prepared_select_ctp_points;
    std::vector<nous::pos_t> m_prepared_select_points;
    int64_t m_picked_control_point_idx = -1;
    int64_t m_hovered_control_point_idx = -1;

    bool m_begin_drag_gizmo = false;

    enum class EEditNurbStep : int8_t
    {
        PickElement,
        PickControlPoint,
        FixingControlPoint
    };

    enum class EEditNurbMode
    {
        ControlPoint,
        FitPoint
    };

    EEditNurbStep m_edit_step = EEditNurbStep::PickElement;

    EEditNurbMode m_edit_mode = EEditNurbMode::ControlPoint;

    std::unique_ptr<Handle> m_handle_gizmo;

    nous::pos_t m_hover_pos;
};

};// namespace sketch