#pragma once

#include "sketch/Core.h"

#include "..\Controller_Main.h"
#include "Engine\Math\Mat43.h"
#include "Engine\Math\Plane.h"
#include "Engine\Math\Planef.h"
#include "Engine\Math\Vec2i.h"
#include "Engine\Math\Vec3f.h"

namespace sketch
{
class Controller_Camera_TRS : public Controller_Main
{
public:
    Controller_Camera_TRS(const std::wstring& name);
    virtual ~Controller_Camera_TRS();

public:
    virtual void active(const SketchParamMap& pProps);
    virtual void deactive();

    virtual void key_down(int key, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_out(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

protected:
    int m_cur_mode;// 0: none, 1: rotate, 2: translate, 3:scale

    // Ðý×ª/Æ½ÒÆ
    Engine::Vec2i m_mouse_begin;
    Engine::Mat43 m_mat_cam_begin;
    Engine::Vec3 m_pick_origin_begin, m_pick_dir_begin;

    Engine::Vec3 m_rot_target;

    Engine::Plane m_trans_plane;
    Engine::Vec3 m_trans_begin_pos;
};
}// namespace sketch