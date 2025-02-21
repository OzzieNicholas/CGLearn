#pragma once

#include "../../Controller_Main.h"
#include "nous_base/nous_base.h"

namespace sketch
{
class View;
struct InferResult;
class Controller_Create_Circle : public Controller_Main
{
public:
    Controller_Create_Circle(const std::wstring& name);
    ~Controller_Create_Circle() override;

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
    void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) override;
    void mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

protected:
    void prepare_render(View* view) override;

private:
    void _gen_circle_mesh(View* view);
    void _gen_circle();
    void _gen_infer_circle(const InferResult& infer);

    void _rollback_to_pending();

private:
    int m_circle_edge_num;
    nous::vec3 m_circle_normal;
    nous::vec3 m_center_pos;
    nous::vec3 m_hover_pos;
    nous::vec3 m_circle_color;
    std::vector<nous::vec3> m_circle_points;
    nous::vec3 m_infer_circle_color;
    std::vector<nous::vec3> m_infer_circle_points;

    enum CreateCircleStep
    {
        PENDING = 0,
        FIXING = 1
    };

    enum CreateCircleMode
    {
        Mesh,
        Nurbs
    };

    CreateCircleStep m_create_circle_step;
    CreateCircleMode m_create_circle_mode;

    float m_O;
    float m_r;
};
}// namespace sketch