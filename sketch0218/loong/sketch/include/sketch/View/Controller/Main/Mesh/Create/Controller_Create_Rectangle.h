#pragma once

#include "../../Controller_Main.h"
#include "nous_base/nous_base.h"

namespace sketch
{
class View;
struct InferResult;
class Controller_Create_Rectangle : public Controller_Main
{
public:
    Controller_Create_Rectangle(const std::wstring& name);
    ~Controller_Create_Rectangle() override;

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
    void _gen_rect_mesh(View* view);
    void _gen_rect();
    void _gen_infer_rect(const InferResult& infer);

    void _rollback_to_pending();

private:
    enum CreateRectStep
    {
        PENDING = 0,
        FIXING = 1
    };

    enum CreateRectMode
    {
        Mesh = 0,
        Nurbs = 1
    };

    CreateRectStep m_create_rect_step;

    bool m_rect_dir_fixed;
    nous::vec3 m_rect_dir;
    nous::vec3 m_begin_pos;
    nous::vec3 m_hover_pos;
    nous::vec3 m_rect_color;
    std::vector<nous::vec3> m_rect_points;

    nous::vec3 m_infer_rect_color;
    std::vector<nous::vec3> m_infer_rect_points;
    CreateRectMode m_create_rect_mode;
};
}// namespace sketch