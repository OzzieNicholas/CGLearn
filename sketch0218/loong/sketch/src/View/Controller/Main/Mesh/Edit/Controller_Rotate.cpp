#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Rotate.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

namespace sketch
{
Controller_Rotate::Controller_Rotate(const std::wstring& name)
    : Controller_Edit(name), m_rotate_step(RotateStep::PICK_ELEMENT), m_rotate_center(nous::vec3::Zero())
{
    m_keep_connection = true;
}

Controller_Rotate::~Controller_Rotate() {}

SketchParamMap Controller_Rotate::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "angle");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Rotate::active(const SketchParamMap& props)
{
    Controller_Main::active(props);
    _clear_affected_meshes();
    _clear_all_tmp_cache();

    this->set_tip(L"旋转");
    this->set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    m_rotate_step = RotateStep::PICK_ELEMENT;
    auto& select_info = view->selector()->get_select_info();
    if(!_is_select_invalid_element(view->selector()))
    {
        m_rotate_step = RotateStep::ROTATE_PLANE_FIXING;
    }
}

void Controller_Rotate::deactive()
{
    Controller_Main::deactive();
    _clear_affected_meshes();
    _clear_all_tmp_cache();

    if(m_rotate_step > RotateStep::PICK_ELEMENT)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
    }
}

void Controller_Rotate::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_rotate_step > PICK_ELEMENT)
    {
        _rollback_to_pick_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Rotate::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_rotate_step == RotateStep::ROTATE_FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto angle = Util::extract_value_and_unit(m_current_value);
                // TODO
                m_radian = angle.first / 180.f * sketch_math::sketch_pi;
                _update_meshes(m_radian);
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        this->_gen_meshes(view);
        _unlock_mouse_move();
        _clear_all_tmp_cache();
        m_rotate_step = RotateStep::PICK_ELEMENT;
        _clear_affected_meshes();
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Rotate::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Rotate::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Rotate::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    auto& infer_info = view->inferHelper()->get_infer_result();
    auto& select_info = view->selector()->get_select_info();
    view->selector()->select_filter_view_group_area(view);
    if(m_rotate_step == RotateStep::PICK_ELEMENT)
    {
        if(!_is_select_invalid_element(view->selector()))
        {
            m_rotate_step = RotateStep::ROTATE_PLANE_FIXING;
        }
    }

    if(m_rotate_step == RotateStep::ROTATE_PLANE_FIXING)
    {
        m_rotate_normal = infer_info.normal.normalized();
        m_rotate_center = infer_info.position;

        RvtBlock blk(view->doc()->scene()->rvtId());
        _prepar_affect_meshes_elements(select_info);

        m_rotate_step = RotateStep::ROTATE_BEGIN_FIXING;
    }
    else if(m_rotate_step == RotateStep::ROTATE_BEGIN_FIXING)
    {
        m_rotate_step = RotateStep::ROTATE_FIXING;
    }
    else if(m_rotate_step == RotateStep::ROTATE_FIXING)
    {
        RvtBlock blk(view->doc()->scene()->rvtId(), true);
        this->_gen_meshes(view);
        //
        _unlock_mouse_move();
        m_rotate_step = RotateStep::PICK_ELEMENT;
        _clear_affected_meshes();
        _clear_all_tmp_cache();
    }
}

void Controller_Rotate::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    auto& infer_info = infer_helper->get_infer_result();

    if(m_rotate_step == RotateStep::PICK_ELEMENT)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y);
        view->selector()->clear();
        if(infer_info.infer_type & InferType::ELEMENT_TYPE)
        {
            if(infer_info.mesh)
            {
                TFObjectDoc* group_doc = view->get_obj_highest_parent_in_working_group_area(infer_info.mesh->doc());
                if(group_doc && group_doc->_group())
                {
                    view->selector()->add_select_element(group_doc->renderObject(), std::unordered_set<SketchElementPtr>());
                }
                else
                {
                    std::unordered_set<SketchElementPtr> elements;
                    if(infer_info.infer_type & InferType::LINE || infer_info.infer_type & InferType::POLYGON ||
                       infer_info.infer_type & InferType::CURVE || infer_info.infer_type & InferType::SURFACE)
                    {
                        SketchElementPtr valid_element = infer_info.element->group();
                        elements.insert(valid_element ? valid_element : infer_info.element);
                    }
                    else if(infer_info.infer_type & InferType::POINT)
                    {
                        MeshDoc* mesh_doc = dynamic_cast<MeshDoc*>(infer_info.mesh->doc());
                        std::unordered_set<SketchLinePtr> ad_lines =
                            GeoDataUtils::get().get_point_adj_lines(mesh_doc->geodata().get(), infer_info.element->to_point());
                        for(auto& ad_line: ad_lines)
                        {
                            SketchElementPtr valid_element = ad_line->group();
                            elements.insert(valid_element ? valid_element : ad_line);
                        }
                    }
                    view->selector()->add_select_element(infer_info.mesh, elements);
                }
            }
        }
    }
    else if(m_rotate_step == RotateStep::ROTATE_PLANE_FIXING)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y);
    }
    else if(m_rotate_step == RotateStep::ROTATE_BEGIN_FIXING)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y, &m_rotate_center);

        nous::vec3 point_to_plane = infer_info.position - m_rotate_center;
        float distance_to_plane = point_to_plane.dot(m_rotate_normal);
        m_ctrl_pos = infer_info.position - distance_to_plane * m_rotate_normal;
        m_begin_pos = m_ctrl_pos;

        m_infer_operator_proj_to_plane_line = { infer_info.position, m_ctrl_pos };
        m_infer_operator_rotate_begin_line = { m_rotate_center, m_ctrl_pos };
    }
    else if(m_rotate_step == RotateStep::ROTATE_FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y, &m_rotate_center);

            nous::vec3 point_to_plane = infer_info.position - m_rotate_center;
            float distance_to_plane = point_to_plane.dot(m_rotate_normal);
            m_ctrl_pos = infer_info.position - distance_to_plane * m_rotate_normal;

            m_infer_operator_proj_to_plane_line = { infer_info.position, m_ctrl_pos };
            m_infer_operator_rotate_dir_line = { m_rotate_center, m_ctrl_pos };

            m_radian = 0.f;
            if(!sketch_math::point_equal(m_ctrl_pos, m_rotate_center))
            {
                nous::vec3 a = (m_begin_pos - m_rotate_center).normalized();
                nous::vec3 b = (m_ctrl_pos - m_rotate_center).normalized();
                float dp = a.dot(b);
                dp = std::clamp(dp, -1.0f, 1.0f);
                m_radian = std::acos(dp);
                nous::vec3 cp = a.cross(b);
                if(cp.dot(m_rotate_normal) < 0.f)
                {
                    m_radian = -m_radian;
                }
            }
            m_current_value = Util::cut_string_with_two_decimals(180.f * m_radian / sketch_math::sketch_pi);
        }
        _update_meshes(m_radian);
    }
    m_hover_pos = infer_info.position;

    _gen_infer_operator(infer_helper->get_infer_result());
    view->dirtyRT();
}

void Controller_Rotate::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Rotate::mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->update_inference(view, x, y);
    _gen_infer_operator(infer_helper->get_infer_result());
    view->dirtyRT();
}

void Controller_Rotate::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_rotate_step == RotateStep::ROTATE_BEGIN_FIXING)
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_infer_operator_proj_to_plane_line.first),
                              NousConvert::convert(m_infer_operator_proj_to_plane_line.second), 0x0f0fu, 1, false, true);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_infer_operator_rotate_begin_line.first),
                              NousConvert::convert(m_infer_operator_rotate_begin_line.second), 0x00ffu, 1, false, true);
    }
    if(m_rotate_step == RotateStep::ROTATE_FIXING)
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_infer_operator_proj_to_plane_line.first),
                              NousConvert::convert(m_infer_operator_proj_to_plane_line.second), 0x0f0fu, 1, false, true);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_infer_operator_rotate_begin_line.first),
                              NousConvert::convert(m_infer_operator_rotate_begin_line.second), 0x00ffu, 1, false, true);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_infer_operator_rotate_dir_line.first),
                              NousConvert::convert(m_infer_operator_rotate_dir_line.second), 0x00ffu, 1, false, true);
    }

    for(int i = 0; i < m_infer_operator_plane.size(); ++i)
    {
        view->batchRenderLine(1, 1.0f, NousConvert::convert(m_infer_operator_plane_color), NousConvert::convert(m_infer_operator_plane[i]),
                              NousConvert::convert(m_infer_operator_plane[(i + 1) % m_infer_operator_plane.size()]), 0xffffu, 1, false, true);
    }

    for(auto& line: m_infer_operator_plane_scale_lines)
    {
        view->batchRenderLine(1, 1.0f, NousConvert::convert(m_infer_operator_plane_color), NousConvert::convert(line.first),
                              NousConvert::convert(line.second), 0xffffu, 1, false, true);
    }
}

void Controller_Rotate::_update_helper_mesh(InferenceHelper* infer_helper)
{
    m_infer_point_tri.clear();
    m_infer_point_edge.clear();
    m_infer_lines.clear();
    m_refer_point_tri.clear();
    m_refer_point_edge.clear();
    m_refer_lines.clear();
    m_refer_polygons_edge.clear();

    auto set_look_at = [](const nous::mat4& org_mat, const nous::vec3& forward, const nous::vec3& up) {
        nous::vec3 eye = org_mat.col(3).head<3>();

        float scaleX = org_mat.col(0).head<3>().norm();
        float scaleY = org_mat.col(1).head<3>().norm();
        float scaleZ = org_mat.col(2).head<3>().norm();

        nous::vec3 right = up.cross(forward).normalized();
        nous::vec3 newUp = forward.cross(right).normalized();

        nous::mat4 rotationMatrix;
        rotationMatrix.col(0).head<3>() = right * scaleX;
        rotationMatrix.col(0).w() = org_mat.col(0).w();
        rotationMatrix.col(1).head<3>() = newUp * scaleY;
        rotationMatrix.col(1).w() = org_mat.col(1).w();
        rotationMatrix.col(2).head<3>() = forward * scaleZ;
        rotationMatrix.col(2).w() = org_mat.col(2).w();
        rotationMatrix.col(3).head<3>() = eye;
        rotationMatrix.col(3).w() = org_mat.col(3).w();

        return rotationMatrix;
    };

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    ViewCamera* cam = view->camera();
    const InferResult& infer_result = infer_helper->get_infer_result();
    Document* doc = STCore::inst->docMgr()->document();

    // inference point
    nous::vec3 cp = NousConvert::convert(doc->v2d(cam->pos()));
    Engine::Vec4f cv = cam->viewport();
    Engine::Vec4f ov = cam->orthoView();
    Engine::Vec3 cud = cam->upDir();
    Engine::Vec3 cfd = cam->forwardDir();
    int camera_project_type = cam->projType();
    float scale_factor = 1.0f;
    if(camera_project_type == 0)
    {
        scale_factor = (cp - infer_result.position).norm() * 0.003f * (700.0f / cv.z);
    }
    else
    {
        scale_factor = 100.0f * (ov.y - ov.x) / cv.w;
    }

    nous::transform3 transform = nous::transform3::Identity();
    transform.translate(infer_result.position);
    transform.scale(scale_factor);
    nous::mat4 mat = transform.matrix();
    mat = set_look_at(mat, NousConvert::convert(cfd), NousConvert::convert(cud));

    // infer point
    if(infer_result.is_point() || infer_result.infer_type == InferType::TRIANGLE_POINT)
    {
        float rotate_circle_radius = 3.f;
        int rotate_line_count = 6;
        std::vector<nous::vec3> circle;
        for(int i = 0; i < rotate_line_count; ++i)
        {
            float radians = 3.141592f * 2.0f * i / rotate_line_count;
            nous::vec4 vp = nous::vec4(rotate_circle_radius * cos(radians), rotate_circle_radius * sin(radians), 0.0f, 1.0f);
            circle.emplace_back((mat * vp).head<3>() / vp.w());
        }
        nous::vec4 color = nous::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        if(infer_result.infer_type == InferType::LINE_CENTER)
        {
            color = nous::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }

        nous::vec3 circle_center = mat.col(3).head<3>() / mat.col(3).w();
        for(int i = 0; i < rotate_line_count; ++i)
        {
            m_infer_point_edge.emplace_back(edge_color, std::pair{ circle[i], circle[(i + 1) % rotate_line_count] });
            m_infer_point_tri.emplace_back(
                color, std::vector{ circle_center, m_infer_point_edge.back().second.first, m_infer_point_edge.back().second.second });
        }
    }
    else if(infer_result.infer_type == InferType::LINE || infer_result.is_world_axis() || infer_result.infer_type == InferType::TRIANGLE_LINE)
    {
        float size = 3.f;
        std::vector<nous::vec3> rect;
        nous::vec4 p0 = mat * nous::vec4(-size, 0.f, 0.f, 1.f);
        nous::vec4 p1 = mat * nous::vec4(0.f, -size, 0.f, 1.f);
        nous::vec4 p2 = mat * nous::vec4(size, 0.f, 0.f, 1.f);
        nous::vec4 p3 = mat * nous::vec4(0.f, size, 0.f, 1.f);

        rect.emplace_back(p0.head<3>() / p0.w());
        rect.emplace_back(p1.head<3>() / p1.w());
        rect.emplace_back(p2.head<3>() / p2.w());
        rect.emplace_back(p3.head<3>() / p3.w());

        nous::vec4 color = nous::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[0], rect[1] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[1], rect[2] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[2], rect[3] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[3], rect[0] });

        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[1], rect[2] });
        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[2], rect[3] });
    }
    else if(infer_result.infer_type == InferType::POLYGON || infer_result.infer_type == InferType::TRIANGLE_FACE)
    {
        float size = 2.f;
        std::vector<nous::vec3> rect;
        nous::vec4 p0 = mat * nous::vec4(-size, -size, 0.f, 1.f);
        nous::vec4 p1 = mat * nous::vec4(-size, size, 0.f, 1.f);
        nous::vec4 p2 = mat * nous::vec4(size, size, 0.f, 1.f);
        nous::vec4 p3 = mat * nous::vec4(size, -size, 0.f, 1.f);

        rect.emplace_back(p0.head<3>() / p0.w());
        rect.emplace_back(p1.head<3>() / p1.w());
        rect.emplace_back(p2.head<3>() / p2.w());
        rect.emplace_back(p3.head<3>() / p3.w());

        nous::vec4 color = nous::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[0], rect[1] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[1], rect[2] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[2], rect[3] });
        m_infer_point_edge.emplace_back(edge_color, std::pair{ rect[3], rect[0] });

        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[1], rect[2] });
        m_infer_point_tri.emplace_back(color, std::vector{ rect[0], rect[2], rect[3] });
    }

    // inference lines
    for(auto& inferLine: infer_result.inference_lines)
    {
        float patten = 0.0f;
        if(inferLine.first & InferType::AXIS_X)
            m_infer_lines.emplace_back(nous::vec4(1.0f, 0.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::AXIS_Y)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 1.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::AXIS_Z)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 0.0f, 1.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::VPC_TYPE)
            m_infer_lines.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
        else if(inferLine.first & InferType::SEG_LINE)
            m_infer_lines.emplace_back(nous::vec4(0.0f, 0.0f, 0.0f, patten), std::pair{ inferLine.second.first, inferLine.second.second });
    }

    // reference points
    for(auto& refer_point: infer_result.reference_points)
    {
        Engine::Vec3 cp = doc->v2d(cam->pos());
        Engine::Vec4f cv = cam->viewport();
        Engine::Vec4f ov = cam->orthoView();
        Engine::Vec3 cud = cam->upDir();
        Engine::Vec3 cfd = cam->forwardDir();
        int camera_project_type = cam->projType();
        float scale_factor = 1.0f;
        if(camera_project_type == 0)
        {
            scale_factor = (cp - NousConvert::convert(refer_point)).len() * 0.003f * (700.0f / cv.z);
        }
        else
        {
            scale_factor = 100.0f * (ov.y - ov.x) / cv.w;
        }
        nous::transform3 transform = nous::transform3::Identity();
        transform.translate(refer_point);
        transform.scale(scale_factor);
        nous::mat4 mat = transform.matrix();
        mat = set_look_at(mat, NousConvert::convert(cfd), NousConvert::convert(cud));

        float rotate_circle_radius = 3.f;
        int rotate_line_count = 6;
        std::vector<nous::vec3> circle;
        for(int i = 0; i < rotate_line_count; ++i)
        {
            float radians = 3.141592f * 2.0f * i / rotate_line_count;
            nous::vec4 vp = nous::vec4(rotate_circle_radius * cos(radians), rotate_circle_radius * sin(radians), 0.0f, 1.0f);
            circle.emplace_back((mat * vp).head<3>() / vp.w());
        }
        nous::vec4 color = nous::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        nous::vec4 edge_color = nous::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        nous::vec3 circle_center = mat.col(3).head<3>() / mat.col(3).w();
        for(int i = 0; i < rotate_line_count; ++i)
        {
            m_refer_point_edge.emplace_back(edge_color, std::pair{ circle[i], circle[(i + 1) % rotate_line_count] });
            m_refer_point_tri.emplace_back(
                color, std::vector{ circle_center, m_refer_point_edge.back().second.first, m_refer_point_edge.back().second.second });
        }
    }

    // reference lines
    for(auto& refer_line: infer_result.reference_lines)
    {
        m_refer_lines.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, 1.0f), refer_line);
    }

    // reference polygons
    for(auto& refer_polygon: infer_result.reference_polygons)
    {
        std::vector<std::pair<nous::vec3, nous::vec3>> edges;
        for(auto ring: refer_polygon)
        {
            for(int i = 0; i < ring.size(); ++i)
            {
                edges.emplace_back(ring[i], ring[(i + 1) % ring.size()]);
            }
        }
        m_refer_polygons_edge.emplace_back(nous::vec4(1.0f, 0.4f, 0.7f, 1.0f), std::move(edges));
    }
}

void Controller_Rotate::_update_meshes(float radian)
{
    Engine::Mat43 rotate_mat;
    rotate_mat.rotateAround(NousConvert::convert(m_rotate_center), NousConvert::convert(m_rotate_normal), radian);

    for(auto& bundle: m_affected_bundles)
    {
        bundle.first->set_matrix_from_world(rotate_mat * NousConvert::convert(bundle.second));
    }

    for(auto& nurbs_change_info: m_affected_nurbs_points)
    {
        NurbsDoc* nurbs_doc = nurbs_change_info.first;
        Engine::Mat43 mat = nurbs_doc->world_matrix();
        Engine::Mat43 inv_mat = nurbs_doc->world_matrix().inverse();
        for(auto& ctp_ptr: nurbs_change_info.second)
        {
            const nous::pos_t ori_pos = ctp_ptr.second;
            const nous::pos_t new_pos = NousConvert::convert(inv_mat * rotate_mat * mat * NousConvert::convert(ori_pos));
            ctp_ptr.first->set_position(new_pos);
        }
        nurbs_doc->dirty();
    }

    for(auto& mesh_change_info: m_affected_mesh_local_position)
    {
        Engine::Mat43 mat = mesh_change_info.first->world_matrix();
        Engine::Mat43 inv_mat = mesh_change_info.first->world_matrix_inv();
        auto geo = mesh_change_info.first->geodata();

        std::vector<std::pair<SketchPointPtr, nous::vec3>> new_pos;
        for(auto& point: mesh_change_info.second)
        {
            new_pos.emplace_back(point.first, NousConvert::convert(inv_mat * rotate_mat * mat * NousConvert::convert(point.second)));
        }
        GeoDataTopoOperations::get().change_point_positon(geo.get(), new_pos);
    }
    for(auto& affected_mesh_polygon: m_affected_mesh_polygons)
    {
        for(auto& polygons: affected_mesh_polygon.second)
        {
            polygons->m_changed = true;
        }
    }
}

void Controller_Rotate::_gen_meshes(View* view)
{
    for(auto mesh_polygon: m_affected_mesh_polygons)
    {
        auto pick_geo = mesh_polygon.first->geodata();
        for(auto polygon: mesh_polygon.second)
        {
            pick_geo->delete_polygon(polygon);
        }
    }
    _update_new_meshes();
    // recreate lines & recreate no branch line groups relation
    for(auto mesh_line: m_affected_mesh_local_line)
    {
        auto geo = mesh_line.first->geodata();
        std::unordered_map<ElementGroupPtr, std::vector<std::pair<nous::vec3, nous::vec3>>> new_lines;
        for(auto& line: mesh_line.second)
        {
            nous::vec3 line_pos_0 = geo->get_point_position(line->p0);
            nous::vec3 line_pos_1 = geo->get_point_position(line->p1);
            ElementGroupPtr line_group = line->group();
            new_lines[line_group].emplace_back(std::move(line_pos_0), std::move(line_pos_1));
        }
        geo->delete_lines(mesh_line.second, false, false);
        for(auto& line_pair: new_lines)
        {
            ElementGroupPtr lines_group = line_pair.first;
            if(lines_group)
            {
                if(lines_group->type() == EElementType::NoBranchLineGroup)
                {
                    GeoDataTopoOperations::get().add_lines_to_no_branch_line_group(geo.get(), line_pair.second, lines_group);
                }
                else if(lines_group->type() == EElementType::SmoothGroup)
                {
                    GeoDataTopoOperations::get().add_lines_to_smooth_group(geo.get(), line_pair.second, lines_group);
                }
            }
            else
            {
                for(auto& line: line_pair.second)
                {
                    geo->add_line(line.first, line.second, false);
                }
            }
        }
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(geo.get());
        // GeoDataTopoOperations::get().check_and_recreate_smooth_group(geo.get());
        mesh_line.first->dirty();
    }
    // recreate polygon
    for(auto& mesh_new_polygon: m_mesh_new_polygons)
    {
        auto geo = mesh_new_polygon.first->geodata();
        for(auto& polygons_map: mesh_new_polygon.second)
        {
            for(auto& polygon: polygons_map.second)
            {
                geo->add_polygon(polygon);
            }
        }
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(geo.get());
        GeoDataTopoOperations::get().check_and_recreate_smooth_group(geo.get());
        mesh_new_polygon.first->dirty();
    }

    // merge
    std::unordered_set<MeshDoc*> change_meshes;
    for(auto& change_mesh_info: m_affected_mesh_local_position)
    {
        change_meshes.insert(change_mesh_info.first);
    }
    _check_intersection_and_merge_mesh(change_meshes, std::unordered_set<MeshDoc*>{}, view->doc()->scene());
}

void Controller_Rotate::_gen_infer_operator(const InferResult& infer)
{
    float epsilon = 1e-2;

    // params
    nous::vec3 normal = m_rotate_normal;
    nous::vec3 center = m_rotate_center;
    if(m_rotate_step <= RotateStep::ROTATE_PLANE_FIXING)
    {
        center = infer.position;
        normal = infer.normal;
    }

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
        view_scale = (cam_pos - center).norm() * 0.0035f * (700.0f / cam_view_port.z());
    else
        view_scale = 100.0f * (cam_ortho_view.y() - cam_ortho_view.x()) / cam_view_port.w();
    view_scale *= 20.f;

    //
    nous::vec3 first_positon_dir = (m_begin_pos - m_rotate_center).normalized();
    if(m_rotate_step == RotateStep::PICK_ELEMENT)
    {
        nous::vec3 arbitrary_dir;
        nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
        for(const auto& axis: axes)
        {
            float d = std::abs(normal.dot(axis));
            if(abs(abs(d) - 1.f) < epsilon * epsilon)
            {
                continue;
            }
            arbitrary_dir = axis;
            break;
        }
        first_positon_dir = normal.cross(arbitrary_dir).normalized();
    }
    nous::vec3 first_positon = center + first_positon_dir * view_scale;

    // Calculate other points on the circle
    m_infer_operator_plane_scale_lines.clear();
    m_infer_operator_plane.clear();
    float angleIncrement = 2.0 * 3.14159 / 16;
    normal = normal.normalized();
    auto gen_circle_and_scale_line = [&center, &first_positon, &normal](float angle, std::vector<nous::vec3>& vp,
                                                                        std::vector<std::pair<nous::vec3, nous::vec3>>& scale_line) {
        nous::vec3 translated_p0 = first_positon - center;

        float c = cos(angle);
        float s = sin(angle);

        float rotationMatrix[3][3] = { { c + normal.x() * normal.x() * (1 - c), normal.x() * normal.y() * (1 - c) - normal.z() * s,
                                         normal.x() * normal.z() * (1 - c) + normal.y() * s },
                                       { normal.y() * normal.x() * (1 - c) + normal.z() * s, c + normal.y() * normal.y() * (1 - c),
                                         normal.y() * normal.z() * (1 - c) - normal.x() * s },
                                       { normal.z() * normal.x() * (1 - c) - normal.y() * s, normal.z() * normal.y() * (1 - c) + normal.x() * s,
                                         c + normal.z() * normal.z() * (1 - c) } };

        float x = rotationMatrix[0][0] * translated_p0.x() + rotationMatrix[0][1] * translated_p0.y() + rotationMatrix[0][2] * translated_p0.z();
        float y = rotationMatrix[1][0] * translated_p0.x() + rotationMatrix[1][1] * translated_p0.y() + rotationMatrix[1][2] * translated_p0.z();
        float z = rotationMatrix[2][0] * translated_p0.x() + rotationMatrix[2][1] * translated_p0.y() + rotationMatrix[2][2] * translated_p0.z();

        vp.emplace_back(x + center.x(), y + center.y(), z + center.z());
        scale_line.emplace_back(nous::vec3(x, y, z) + center, nous::vec3(x, y, z) * 0.8f + center);
    };

    for(size_t i = 0; i < 16; i++)
    {
        float angle = i * angleIncrement;
        gen_circle_and_scale_line(angle, m_infer_operator_plane, m_infer_operator_plane_scale_lines);
    }
    m_infer_operator_plane_scale_lines.emplace_back(m_infer_operator_plane[0], m_infer_operator_plane[8]);

    auto calculate_color = [&epsilon](const nous::vec3& n) {
        nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
        for(const auto& axis: axes)
        {
            float dotProduct = std::abs(n.dot(axis));
            if(abs(abs(dotProduct) - 1.f) < epsilon * epsilon)
            {
                return nous::vec3(abs(n.x()), abs(n.y()), abs(n.z()));
            }
        }
        return nous::vec3(1.f, 1.f, 1.f);
    };
    m_infer_operator_plane_color = calculate_color(normal);
}

bool Controller_Rotate::_is_select_invalid_element(ElementSelector* selector)
{
    int valid_element_count = 0;
    auto& select_info = selector->get_select_info();
    for(auto& select: select_info)
    {
        if(select.first->type() == Define::OBJ_D5_GROUP)
        {
            valid_element_count++;
        }
        else
        {
            for(auto& element: select.second.elements)
            {
                valid_element_count++;
                EElementType type = element->type();
                if(type == EElementType::Point)
                {
                    return true;
                }
            }
        }
    }
    return valid_element_count == 0;
}

void Controller_Rotate::_rollback_to_pick_element()
{
    if(m_rotate_step == ROTATE_PLANE_FIXING || m_rotate_step == ROTATE_FIXING || m_rotate_step == ROTATE_BEGIN_FIXING)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
        _clear_affected_meshes();
        _clear_all_tmp_cache();
        m_rotate_step = PICK_ELEMENT;
    }
}

void Controller_Rotate::_clear_all_tmp_cache()
{
    m_infer_operator_proj_to_plane_line = std::make_pair(nous::pos_t::Zero(), nous::pos_t::Zero());
    m_infer_operator_rotate_begin_line = std::make_pair(nous::pos_t::Zero(), nous::pos_t::Zero());
    m_infer_operator_rotate_dir_line = std::make_pair(nous::pos_t::Zero(), nous::pos_t::Zero());
    m_rotate_center = nous::pos_t::Zero();
    m_rotate_normal = nous::pos_t::Zero();
    m_begin_pos = nous::pos_t::Zero();
    m_hover_pos = nous::pos_t::Zero();
    m_ctrl_pos = nous::pos_t::Zero();
}

}// namespace sketch
