#pragma once

#include "Controller_Main.h"
#include "nous_base/nous_base.h"

namespace sketch
{
class RenderObject;
class TFObjectDoc;
class MeshDoc;
class NurbsDoc;
class GroupDoc;
class Controller_Common_Operate : public Controller_Main
{
public:
    Controller_Common_Operate(const std::wstring& name);
    virtual ~Controller_Common_Operate();

    virtual void active(const SketchParamMap& props);
    virtual void deactive();

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view);

private:
    void _create_select_obj_to_clipbord(View* view);
    void _show_clipbord_obj(View* view);
    void _paste_obj(View* view);
    void _clear_clipbord(View* view);

protected:
    bool m_is_snap_mesh;
    nous::vec3 m_hover_pos;
    enum OperatorState
    {
        NONE = 0,
        PASTING = 1
    };
    OperatorState m_operate_state;

    std::unordered_map<TFObjectDoc*, Engine::Mat43> m_clipbord_objs;
    nous::vec3 m_clipbord_obj_center;
};
}// namespace sketch
