#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Polygon.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/Nurbs/util/nurbs_factory.h"
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
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsData.h"

#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

namespace sketch
{
Controller_Create_Polygon::Controller_Create_Polygon(const std::wstring& name)
    : Controller_Main(name), m_polygon_edge_num(6), m_create_polygon_step(CreatePolygonStep::PENDING)
{
}

Controller_Create_Polygon::~Controller_Create_Polygon() {}

SketchParamMap Controller_Create_Polygon::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "radius");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Create_Polygon::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    set_tip(L"多边形");
    set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    m_polygon_points.clear();
    m_create_polygon_step = CreatePolygonStep::PENDING;
}

void Controller_Create_Polygon::deactive()
{
    Controller_Main::deactive();

    m_polygon_points.clear();
}

void Controller_Create_Polygon::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_create_polygon_step == FIXING)
    {
        _rollback_to_pending();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Create_Polygon::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_create_polygon_step == CreatePolygonStep::FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto lenth = Util::extract_value_and_unit(m_current_value);
                // TODO
                m_hover_pos = m_center_pos + (m_hover_pos - m_center_pos).normalized() * lenth.first;
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        this->_gen_polygon();
        this->_gen_polygon_mesh(view);
        _unlock_mouse_move();
        m_create_polygon_step = CreatePolygonStep::PENDING;
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Create_Polygon::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Polygon::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Polygon::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;
    View* view = STCore::inst->viewMgr()->layout()->focusedView();

    if(m_create_polygon_step == CreatePolygonStep::PENDING)
    {
        m_center_pos = m_hover_pos;
        auto& infer = view->inferHelper()->get_infer_result();

        m_infer_polygon_points.clear();
        m_polygon_normal = infer.normal;
        m_create_polygon_step = CreatePolygonStep::FIXING;
    }
    else if(m_create_polygon_step == CreatePolygonStep::FIXING)
    {
        _unlock_mouse_move();
        this->_gen_polygon_mesh(view);
        m_create_polygon_step = CreatePolygonStep::PENDING;
    }
}

void Controller_Create_Polygon::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    auto& infer = infer_helper->get_infer_result();
    if(m_create_polygon_step == CreatePolygonStep::FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y);
            m_hover_pos = infer.position;
            float radius = (m_center_pos - m_hover_pos).norm();
            m_current_value = Util::cut_string_with_two_decimals(radius);
        }
        _gen_polygon();
    }
    else
    {
        _unlock_mouse_move();

        infer_helper->update_inference(view, x, y);
        m_hover_pos = infer.position;
        _gen_infer_polygon(infer);
    }
    view->dirtyRT();
}

void Controller_Create_Polygon::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Polygon::mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    if(m_create_polygon_step == CreatePolygonStep::FIXING)
    {
    }
    else
    {
        infer_helper->update_inference(view, x, y);
        _gen_infer_polygon(infer_helper->get_infer_result());
        view->dirtyRT();
    }
}

void Controller_Create_Polygon::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Polygon::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_create_polygon_step == CreatePolygonStep::FIXING && m_polygon_points.size())
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3(0, 0, 0), NousConvert::convert(m_center_pos), NousConvert::convert(m_polygon_points[0]), 0xffffu,
                              1, false, false);
        for(int i = 0; i < m_polygon_points.size(); ++i)
        {
            view->batchRenderLine(1, 1.0f, NousConvert::convert(m_polygon_color), NousConvert::convert(m_polygon_points[i]),
                                  NousConvert::convert(m_polygon_points[(i + 1) % m_polygon_points.size()]), 0xffffu, 1, false, false);
        }
    }
    else if(m_create_polygon_step == CreatePolygonStep::PENDING && m_infer_polygon_points.size())
    {
        for(int i = 0; i < m_infer_polygon_points.size(); ++i)
        {
            view->batchRenderLine(1, 1.0f, NousConvert::convert(m_infer_polygon_color), NousConvert::convert(m_infer_polygon_points[i]),
                                  NousConvert::convert(m_infer_polygon_points[(i + 1) % m_infer_polygon_points.size()]), 0xffffu, 1, false, false);
        }
    }
}

void Controller_Create_Polygon::_gen_polygon_mesh(View* view)
{
    SceneDoc* scene = view->doc()->scene();
    RvtBlock blk(scene->rvtId());
    MeshDoc* new_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    auto new_geo = new_mesh_doc->geodata(true);
    new_geo->add_polygon_without_check(std::vector<std::vector<nous::vec3>>{ m_polygon_points }, true);
    // NoBranchLineGroupPtr weld_line_group = std::make_shared<NoBranchLineGroup>();
    // for(auto& sk_line: new_geo->get_all_lines())
    //{
    //     weld_line_group->add_element(sk_line.second);
    // }
    // new_geo->add_element_group(weld_line_group);
    new_mesh_doc->dirty();

    // merge
    _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ new_mesh_doc }, std::unordered_set<MeshDoc*>{}, view->doc()->scene());
}

void Controller_Create_Polygon::_gen_polygon()
{
    nous::vec3 v = m_hover_pos - m_center_pos;
    float d = v.dot(m_polygon_normal);

    m_r = v.norm();

    // params
    nous::vec3 normal = m_polygon_normal;
    nous::vec3 center = m_center_pos;
    nous::vec3 first_positon = m_hover_pos - d * m_polygon_normal;

    // Calculate other points on the circle
    m_polygon_points.clear();
    m_polygon_points.push_back(first_positon);
    float angleIncrement = 2.0 * 3.14159 / m_polygon_edge_num;
    normal = normal.normalized();
    auto rotatePoint = [&center, &first_positon, &normal](float angle, std::vector<nous::vec3>& vp) {
        nous::vec3 translatedP = first_positon - center;

        float c = cos(angle);
        float s = sin(angle);

        float rotationMatrix[3][3] = { { c + normal.x() * normal.x() * (1 - c), normal.x() * normal.y() * (1 - c) - normal.z() * s,
                                         normal.x() * normal.z() * (1 - c) + normal.y() * s },
                                       { normal.y() * normal.x() * (1 - c) + normal.z() * s, c + normal.y() * normal.y() * (1 - c),
                                         normal.y() * normal.z() * (1 - c) - normal.x() * s },
                                       { normal.z() * normal.x() * (1 - c) - normal.y() * s, normal.z() * normal.y() * (1 - c) + normal.x() * s,
                                         c + normal.z() * normal.z() * (1 - c) } };

        float x = rotationMatrix[0][0] * translatedP.x() + rotationMatrix[0][1] * translatedP.y() + rotationMatrix[0][2] * translatedP.z();
        float y = rotationMatrix[1][0] * translatedP.x() + rotationMatrix[1][1] * translatedP.y() + rotationMatrix[1][2] * translatedP.z();
        float z = rotationMatrix[2][0] * translatedP.x() + rotationMatrix[2][1] * translatedP.y() + rotationMatrix[2][2] * translatedP.z();

        vp.emplace_back(x + center.x(), y + center.y(), z + center.z());
    };

    for(size_t i = 1; i < m_polygon_edge_num; i++)
    {
        float angle = i * angleIncrement;
        rotatePoint(angle, m_polygon_points);
    }

    auto calculate_circle_color = [](const nous::vec3& n) {
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
    m_polygon_color = calculate_circle_color(normal);
}

void Controller_Create_Polygon::_gen_infer_polygon(const InferResult& infer)
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
        view_scale = (cam_pos - infer.position).norm() * 0.0035f * (700.0f / cam_view_port.z());
    else
        view_scale = 100.0f * (cam_ortho_view.y() - cam_ortho_view.x()) / cam_view_port.w();
    view_scale *= 10.f;

    nous::vec3 arbitrary_dir;
    nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
    for(const auto& axis: axes)
    {
        if(sketch_math::direction_parallel(infer.normal, axis))
        {
            continue;
        }
        arbitrary_dir = axis;
        break;
    }
    nous::vec3 first_positon_dir = infer.normal.cross(arbitrary_dir).normalized();
    nous::vec3 first_positon = infer.position + first_positon_dir * view_scale;

    // params
    nous::vec3 normal = infer.normal;
    nous::vec3 center = infer.position;

    // Calculate other points on the circle
    m_infer_polygon_points.clear();
    m_infer_polygon_points.push_back(first_positon);
    float angleIncrement = 2.0 * 3.14159 / m_polygon_edge_num;
    normal = normal.normalized();
    auto rotatePoint = [&center, &first_positon, &normal](float angle, std::vector<nous::vec3>& vp) {
        nous::vec3 translatedP = first_positon - center;

        float c = cos(angle);
        float s = sin(angle);

        float rotationMatrix[3][3] = { { c + normal.x() * normal.x() * (1 - c), normal.x() * normal.y() * (1 - c) - normal.z() * s,
                                         normal.x() * normal.z() * (1 - c) + normal.y() * s },
                                       { normal.y() * normal.x() * (1 - c) + normal.z() * s, c + normal.y() * normal.y() * (1 - c),
                                         normal.y() * normal.z() * (1 - c) - normal.x() * s },
                                       { normal.z() * normal.x() * (1 - c) - normal.y() * s, normal.z() * normal.y() * (1 - c) + normal.x() * s,
                                         c + normal.z() * normal.z() * (1 - c) } };

        float x = rotationMatrix[0][0] * translatedP.x() + rotationMatrix[0][1] * translatedP.y() + rotationMatrix[0][2] * translatedP.z();
        float y = rotationMatrix[1][0] * translatedP.x() + rotationMatrix[1][1] * translatedP.y() + rotationMatrix[1][2] * translatedP.z();
        float z = rotationMatrix[2][0] * translatedP.x() + rotationMatrix[2][1] * translatedP.y() + rotationMatrix[2][2] * translatedP.z();

        vp.emplace_back(x + center.x(), y + center.y(), z + center.z());
    };

    for(size_t i = 1; i < m_polygon_edge_num; i++)
    {
        float angle = i * angleIncrement;
        rotatePoint(angle, m_infer_polygon_points);
    }

    auto calculate_circle_color = [](const nous::vec3& n) {
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
    m_infer_polygon_color = calculate_circle_color(normal);
}

void Controller_Create_Polygon::_rollback_to_pending()
{
    m_polygon_normal = nous::vec3::Zero();
    m_center_pos = nous::pos_t::Zero();
    m_polygon_points.clear();
    m_infer_polygon_points.clear();

    m_create_polygon_step = PENDING;
}

}// namespace sketch
