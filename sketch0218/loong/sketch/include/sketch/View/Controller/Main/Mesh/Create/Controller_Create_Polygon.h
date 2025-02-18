#pragma once
#pragma once

#include "../../Controller_Main.h"
#include "nous_base/nous_base.h"

namespace sketch
{
class View;
struct InferResult;
class Controller_Create_Polygon : public Controller_Main
{
public:
    Controller_Create_Polygon(const std::wstring& name);
    ~Controller_Create_Polygon() override;

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
    void _gen_polygon_mesh(View* view);
    void _gen_polygon();
    void _gen_infer_polygon(const InferResult& infer);

    void _rollback_to_pending();

private:
    int m_polygon_edge_num;
    nous::vec3 m_polygon_normal;
    nous::vec3 m_center_pos;
    nous::vec3 m_hover_pos;
    nous::vec3 m_polygon_color;
    std::vector<nous::vec3> m_polygon_points;
    nous::vec3 m_infer_polygon_color;
    std::vector<nous::vec3> m_infer_polygon_points;

    enum CreatePolygonStep
    {
        PENDING = 0,
        FIXING = 1
    };

    CreatePolygonStep m_create_polygon_step;

    float m_O;
    float m_r;
};
}// namespace sketch