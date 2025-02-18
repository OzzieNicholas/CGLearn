#pragma once

#include "../../Controller_Main.h"

namespace sketch
{

class Controller_Create_Arc : public Controller_Main
{
public:
    Controller_Create_Arc(const std::wstring& name);
    ~Controller_Create_Arc() override;

    virtual SketchParamMap info() const override;

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
    void _finish_arc(View* view);
    void _rollback_to_pick_guide_position();
    void _rollback_to_pick_base_position();

    void _generate_arc_by_two_point();
    void _generate_arc_by_radius();

    void _update_plane_helper_mesh(InferResult infer);

    void _generate_arc_by_two_point_core(const double bulge, const nous::vec3& dir);
    void _generate_arc_by_radius_core();

private:
    enum class ECreateArcMode : uint8_t
    {
        // SketchUp 的两点画圆，对应 CAD 的折线凸度圆
        TwoPointDrawArc,
        // SketchUp 的半径画圆，对应其余软件的三点画圆（圆心，半径，角度范围）
        RadiusDrawArc
    };

    enum class ECreateArcStep : uint8_t
    {
        PickBasePosition = 0,
        PickGuidePosition = 1,
        FixingArc = 2
    };

    ECreateArcMode m_create_mode;
    ECreateArcStep m_create_step;

    MeshDoc* m_tmp_mesh_doc = nullptr;

    nous::pos_t m_arc_pos0;
    nous::pos_t m_arc_pos1;

    nous::pos_t m_hover_pos;

    nous::pos_t m_X_pos;
    nous::pos_t m_P_pos;

    nous::vec3 m_radius_mode_arc_normal;
    nous::vec3 m_radius_guide_normal;
    nous::pos_t m_radius_base_dir;
    nous::pos_t m_radius_last_dir;
    double m_radius = 0.0;
    double m_accumulate_angle = 0.0;

    std::vector<nous::vec3> m_infer_operator_plane;
    std::vector<std::pair<nous::vec3, nous::vec3>> m_infer_operator_plane_scale_lines;
};

};// namespace sketch
