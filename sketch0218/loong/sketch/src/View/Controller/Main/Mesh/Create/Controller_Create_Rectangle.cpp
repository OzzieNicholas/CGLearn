#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Rectangle.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Nurbs/util/nurbs_factory.h"
#include "sketch/View/RenderRes/NurbsData.h"

#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

namespace sketch
{
Controller_Create_Rectangle::Controller_Create_Rectangle(const std::wstring& name)
    : Controller_Main(name), m_create_rect_step(CreateRectStep::PENDING), m_create_rect_mode(CreateRectMode::Mesh)
{
}

Controller_Create_Rectangle::~Controller_Create_Rectangle() {}

SketchParamMap Controller_Create_Rectangle::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "diagonal");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Create_Rectangle::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    set_tip(L"矩形");
    set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    m_rect_points.clear();
    m_create_rect_step = CreateRectStep::PENDING;
}

void Controller_Create_Rectangle::deactive()
{
    Controller_Main::deactive();
    m_rect_points.clear();
}

void Controller_Create_Rectangle::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_create_rect_step == FIXING)
    {
        _rollback_to_pending();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Create_Rectangle::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_create_rect_step == CreateRectStep::FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto lenth = Util::extract_value_and_unit(m_current_value);
                // TODO
                m_hover_pos = m_begin_pos + (m_hover_pos - m_begin_pos).normalized() * lenth.first;
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        this->_gen_rect();
        this->_gen_rect_mesh(view);
        _unlock_mouse_move();
        m_create_rect_step = CreateRectStep::PENDING;
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Create_Rectangle::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(shift && m_create_rect_step == CreateRectStep::PENDING)
    {
        if(m_create_rect_mode == CreateRectMode::Mesh)
        {
            m_create_rect_mode = CreateRectMode::Nurbs;
        }
        else
        {
            m_create_rect_mode = CreateRectMode::Mesh;
        }
    }
}

void Controller_Create_Rectangle::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Rectangle::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_create_rect_step == CreateRectStep::PENDING)
    {
        m_begin_pos = m_hover_pos;
        auto& infer = view->inferHelper()->get_infer_result();

        m_rect_dir = nous::vec3::Zero();
        m_rect_dir_fixed = false;
        if(infer.infer_type & InferType::LINE)
        {
            m_rect_dir = infer.tangent;
            m_rect_dir_fixed = true;
        }
        m_infer_rect_points.clear();
        m_create_rect_step = CreateRectStep::FIXING;
    }
    else if(m_create_rect_step == CreateRectStep::FIXING)
    {
        if(m_create_rect_mode == CreateRectMode::Mesh)
        {
            _gen_rect_mesh(view);
        }
        else
        {
            SceneDoc* scene = view->doc()->scene();
            RvtBlock blk(scene->rvtId());
            NurbsDoc* new_nurbs_doc = (NurbsDoc*)view->create_obj_in_working_group(Define::OBJ_D5_NURBS);
            CmdManager* cmd_mgr = STCore::inst->cmdMgr();
            auto new_nurbs = new_nurbs_doc->nurbsdata(true);
            auto rect_surface =
                sketch::nurbs::make_nurbs_rectangle(m_rect_points[0], m_rect_points[1], m_rect_points[3], m_rect_points[2], 3, 3, 5, 5);
            auto rect_nurb_surface = std::make_shared<NurbsAlgebraSurface>();
            rect_nurb_surface->update_surface_data(rect_surface);
            auto complex_surface = std::make_shared<NurbsTrimmedSurface>();
            complex_surface->set_exterior_ring(rect_nurb_surface);
            new_nurbs->add_item(complex_surface);
            new_nurbs_doc->dirty();
        }

        m_create_rect_step = CreateRectStep::PENDING;
    }
}

void Controller_Create_Rectangle::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    const InferResult& infer = infer_helper->get_infer_result();
    if(m_create_rect_step == CreateRectStep::FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y, &m_begin_pos, InferType::PLANE_XYZ_TYPE);
            m_hover_pos = infer.position;
            if(!m_rect_dir_fixed)
            {
                m_rect_dir = nous::vec3(0.f, 0.f, 1.f);
                if(sketch_math::point_on_plane(m_hover_pos, m_begin_pos, nous::vec3(.0f, .0f, 1.f)))
                {
                    m_rect_dir = nous::vec3(1.f, 0.f, 0.f);
                }
            }
            float diagonal = (m_begin_pos - m_hover_pos).norm();
            m_current_value = Util::cut_string_with_two_decimals(diagonal);
        }
        _gen_rect();
    }
    else
    {
        _unlock_mouse_move();

        infer_helper->update_inference(view, x, y);
        m_hover_pos = infer.position;
        _gen_infer_rect(infer);
    }
    view->dirtyRT();
}

void Controller_Create_Rectangle::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Rectangle::mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    if(m_create_rect_step == CreateRectStep::FIXING)
    {
    }
    else
    {
        infer_helper->update_inference(view, x, y);
        _gen_infer_rect(infer_helper->get_infer_result());
        view->dirtyRT();
    }
}

void Controller_Create_Rectangle::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Rectangle::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);
    if(m_create_rect_step == CreateRectStep::FIXING && m_rect_points.size())
    {
        bool color_ready = true;
        if(m_rect_color.squaredNorm() < 0.5)
        {
            color_ready = false;
        }

        auto calculate_line_color = [](const nous::vec3& p0, const nous::vec3& p1) {
            nous::vec3 v = (p1 - p0).normalized();
            nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
            for(const auto& axis: axes)
            {
                if(sketch_math::direction_parallel(v, axis))
                {
                    return axis;
                }
            }
            return nous::vec3(1.f, 1.f, 1.f);
        };
        for(int i = 0; i < m_rect_points.size(); ++i)
        {
            nous::vec3 color = color_ready ? m_rect_color : calculate_line_color(m_rect_points[i], m_rect_points[(i + 1) % m_rect_points.size()]);
            view->batchRenderLine(1, 1.0f, NousConvert::convert(color), NousConvert::convert(m_rect_points[i]),
                                  NousConvert::convert(m_rect_points[(i + 1) % m_rect_points.size()]), 0xffffu, 1, false, true);
        }
    }
    else
    {
        for(int i = 0; i < m_infer_rect_points.size(); ++i)
        {
            view->batchRenderLine(1, 1.0f, NousConvert::convert(m_infer_rect_color), NousConvert::convert(m_infer_rect_points[i]),
                                  NousConvert::convert(m_infer_rect_points[(i + 1) % m_infer_rect_points.size()]), 0xffffu, 1, false, true);
        }
    }
}

void Controller_Create_Rectangle::_gen_rect_mesh(View* view)
{
    SceneDoc* scene = view->doc()->scene();
    RvtBlock blk(scene->rvtId());
    MeshDoc* new_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    auto new_geo = new_mesh_doc->geodata(true);
    new_geo->add_polygon_without_check(std::vector<std::vector<nous::vec3>>{ m_rect_points }, true);
    new_mesh_doc->dirty();

    // merge
    _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ new_mesh_doc }, std::unordered_set<MeshDoc*>{}, view->doc()->scene());
}

void Controller_Create_Rectangle::_gen_rect()
{
    m_rect_points.clear();
    nous::vec3 rect_dir = m_rect_dir.normalized();
    nous::vec3 p0 = m_begin_pos;
    nous::vec3 p2 = m_hover_pos;

    nous::vec3 v = p2 - p0;
    float t = v.dot(rect_dir);
    if(std::abs(t) < sketch_math::sketch_epsilon)
    {
        rect_dir = nous::vec3(0.f, 0.f, 1.f);
        if(sketch_math::point_on_plane(m_hover_pos, m_begin_pos, nous::vec3(.0f, .0f, 1.f)))
        {
            rect_dir = nous::vec3(1.f, 0.f, 0.f);
        }
        t = v.dot(rect_dir);
    }
    nous::vec3 p1 = p0 + t * rect_dir;
    nous::vec3 dir = (p1 - p0).cross(v).cross(rect_dir).normalized();
    float t2 = v.dot(dir);
    nous::vec3 p3 = p0 + t2 * dir;

    m_rect_points.push_back(p0);
    m_rect_points.push_back(p1);
    m_rect_points.push_back(p2);
    m_rect_points.push_back(p3);

    auto calculate_rect_color = [](const nous::vec3& n) {
        nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
        for(const auto& axis: axes)
        {
            if(sketch_math::direction_parallel(n, axis))
            {
                return nous::vec3(abs(n.x()), abs(n.y()), abs(n.z()));
            }
        }
        return nous::vec3(0.f, 0.f, 0.f);
    };
    m_rect_color = calculate_rect_color(v.cross(rect_dir).normalized());
}

void Controller_Create_Rectangle::_gen_infer_rect(const InferResult& infer)
{
    // view scale
    Document* doc = STCore::inst->docMgr()->document();
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ViewCamera* cam = view->camera();
    nous::vec3 cam_pos = NousConvert::convert(doc->v2d(cam->pos()));
    nous::vec4 cam_view_port = NousConvert::convert(cam->viewport());
    nous::vec4 cam_ortho_view = NousConvert::convert(cam->orthoView());
    int m_cameraProjectType = cam->projType();

    float view_scale = 1.0f;
    if(m_cameraProjectType == 0)
        view_scale = (cam_pos - m_hover_pos).norm() * 0.0035f * (700.0f / cam_view_port.z());
    else
        view_scale = 100.0f * (cam_ortho_view.y() - cam_ortho_view.x()) / cam_view_port.w();

    view_scale *= 20.f;

    nous::vec3 rect_dir;
    nous::vec3 rect_normal = infer.normal;

    nous::vec3 fixed_dir(0.f, 0.f, 1.f);
    nous::vec3 plane_vec = fixed_dir.cross(rect_normal);
    if(plane_vec.squaredNorm() < sketch_math::sketch_epsilon * sketch_math::sketch_epsilon)
    {
        fixed_dir = nous::vec3(1.f, 0.f, 0.f);
        plane_vec = fixed_dir.cross(rect_normal);
    }
    rect_dir = plane_vec;

    //
    m_infer_rect_points.clear();
    rect_dir = rect_dir.normalized();
    nous::vec3 p0 = m_hover_pos;
    nous::vec3 p1 = m_hover_pos + rect_dir * view_scale;
    nous::vec3 rect_side_dir = rect_normal.cross(rect_dir).normalized();
    nous::vec3 p2 = p1 + rect_side_dir * view_scale;
    nous::vec3 p3 = p0 + rect_side_dir * view_scale;

    m_infer_rect_points.push_back(p0);
    m_infer_rect_points.push_back(p1);
    m_infer_rect_points.push_back(p2);
    m_infer_rect_points.push_back(p3);

    auto calculate_rect_color = [](const nous::vec3& n) {
        nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
        for(const auto& axis: axes)
        {
            if(sketch_math::direction_parallel(n, axis))
            {
                return nous::vec3(abs(n.x()), abs(n.y()), abs(n.z()));
            }
        }
        return nous::vec3(1.f, 1.f, 1.f);
    };
    m_infer_rect_color = calculate_rect_color(rect_normal);
}

void Controller_Create_Rectangle::_rollback_to_pending()
{
    if(m_create_rect_step == FIXING)
    {
        m_rect_dir_fixed = false;
        m_rect_dir = nous::vec3::Zero();
        m_begin_pos = nous::pos_t::Zero();
        m_rect_points.clear();

        m_create_rect_step = PENDING;
    }
}
}// namespace sketch
