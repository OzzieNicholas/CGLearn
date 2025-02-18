#pragma once
#include "../Controller_Edit.h"
#include "Engine/Math/OBB.h"

namespace sketch
{
class Handle;
class View;
class InferenceHelper;
class Controller_TRS : public Controller_Edit
{
public:
    Controller_TRS(const std::wstring& name);
    virtual ~Controller_TRS();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& pProps);
    virtual void deactive();

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view) override;

protected:
    void _active_handle();
    void _deactive_handle();
    void _calculateBox();
    void _gen_meshes(View* view);
    bool _is_select_invalid_element(ElementSelector* selector);

    void _finish_TRS_operation();

protected:
    std::unique_ptr<Handle> m_handle_gizmo;

    Engine::OBB m_pick_mesh_box;
};
}// namespace sketch
