#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Scale.h"

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
Controller_Scale::Controller_Scale(const std::wstring& name)
    : Controller_Edit(name), m_scale_step(ScaleStep::PICK_ELEMENT), m_center_scale_mode(false)
{
    m_keep_connection = true;
}

Controller_Scale::~Controller_Scale() {}

SketchParamMap Controller_Scale::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "scale");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Scale::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);
    _clear_affected_meshes();

    this->set_tip(L"缩放");
    this->set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::ALL_TYPE);

    m_scale_step = ScaleStep::PICK_ELEMENT;
    if(!_is_select_invalid_element(view->selector()))
    {
        m_scale_step = ScaleStep::SCALE_BEGIN_FIXING;
    }
}

void Controller_Scale::deactive()
{
    Controller_Main::deactive();
    _clear_affected_meshes();

    if(m_scale_step > ScaleStep::PICK_ELEMENT)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
    }
}

void Controller_Scale::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_scale_step > PICK_ELEMENT)
    {
        _roll_back_to_pick_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Scale::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_scale_step == ScaleStep::SCALE_FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto scale = Util::extract_value_and_unit(m_current_value);
                // TODO
                m_scale = scale.first;
                _update_meshes(m_scale);
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        this->_gen_meshes(view);
        _unlock_mouse_move();
        m_scale_step = ScaleStep::PICK_ELEMENT;
        _clear_affected_meshes();
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Scale::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(ctrl)
    {
        m_center_scale_mode = !m_center_scale_mode;
    }
}

void Controller_Scale::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Scale::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    auto& infer_info = view->inferHelper()->get_infer_result();
    auto& select_info = view->selector()->get_select_info();
    if(m_scale_step == ScaleStep::PICK_ELEMENT)
    {
        view->selector()->select_filter_view_group_area(view);
        if(!_is_select_invalid_element(view->selector()))
        {
            m_scale_step = ScaleStep::SCALE_BEGIN_FIXING;
        }
    }

    if(m_scale_step == ScaleStep::SCALE_BEGIN_FIXING)
    {
        RvtBlock blk(view->doc()->scene()->rvtId());
        _prepar_affect_meshes_elements(select_info);
        m_scale_step = ScaleStep::SCALE_PICK_BEGIN_POS;
    }
    else if(m_scale_step == SCALE_PICK_BEGIN_POS)
    {
        m_begin_pos = infer_info.position;
        m_scale_step = ScaleStep::SCALE_PICK_END_POS;
    }
    else if(m_scale_step == ScaleStep::SCALE_PICK_END_POS)
    {
        if(!sketch_math::point_equal(m_hover_pos, m_begin_pos))
        {
            m_end_pos = m_hover_pos;
            m_scale_step = ScaleStep::SCALE_FIXING;
        }
    }
    else if(m_scale_step == ScaleStep::SCALE_FIXING)
    {
        RvtBlock blk(view->doc()->scene()->rvtId(), true);
        this->_gen_meshes(view);
        //
        _unlock_mouse_move();
        m_scale_step = ScaleStep::PICK_ELEMENT;
        _clear_affected_meshes();
    }
}

void Controller_Scale::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    auto& infer_info = infer_helper->get_infer_result();

    if(m_scale_step == ScaleStep::PICK_ELEMENT)
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
                    view->selector()->add_select_element(infer_info.mesh, elements);
                }
            }
        }
    }
    else if(m_scale_step == ScaleStep::SCALE_BEGIN_FIXING)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y);
    }
    else if(m_scale_step == ScaleStep::SCALE_PICK_BEGIN_POS)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y, &m_begin_pos);
    }
    else if(m_scale_step == ScaleStep::SCALE_PICK_END_POS)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y, &m_begin_pos);
    }
    else if(m_scale_step == ScaleStep::SCALE_FIXING)
    {
        if(!this->is_lock())
        {
            ViewCamera* cam = view->camera();
            Document* doc = STCore::inst->docMgr()->document();
            auto& vp = cam->viewport();
            Engine::Vec3 cam_pos, pick_dir;
            cam->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &cam_pos, &pick_dir);
            float t0;
            sketch_math::line_intersect(NousConvert::convert(cam_pos), NousConvert::convert(cam_pos + pick_dir), m_begin_pos, m_end_pos, &t0,
                                        &m_scale);
            m_current_value = Util::cut_string_with_two_decimals(m_scale);
        }
        _update_meshes(m_scale);
        m_ctrl_pos = m_begin_pos + (m_end_pos - m_begin_pos) * m_scale;
    }
    m_hover_pos = infer_info.position;

    view->dirtyRT();
}

void Controller_Scale::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Scale::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_scale_step == ScaleStep::SCALE_PICK_END_POS)
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_begin_pos), NousConvert::convert(m_hover_pos), 0x00ffu, 1, false,
                              false);
    }
    if(m_scale_step == ScaleStep::SCALE_FIXING)
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3f(1.0f, 0.4f, 0.7f), NousConvert::convert(m_begin_pos), NousConvert::convert(m_end_pos), 0xffffu,
                              1, false, false);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_end_pos), NousConvert::convert(m_ctrl_pos), 0xffffu, 1, false,
                              false);
    }
}

void Controller_Scale::_update_helper_mesh(InferenceHelper* infer_helper)
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

void Controller_Scale::_update_meshes(float scale)
{
    nous::vec3 scale_base_dir = m_end_pos - m_begin_pos;
    nous::vec3 scale_dir = scale_base_dir * scale;
    nous::vec3 scale_vec;
    if(!m_center_scale_mode)
    {
        // scale_vec = nous::vec3(1.f, 1.f, 1.f) + scale_base_dir.normalized() * (scale_dir.norm() / scale_base_dir.norm() - 1.f);

        float scale_x = scale_dir.x() / scale_base_dir.x() - 1.f;
        float scale_y = scale_dir.y() / scale_base_dir.y() - 1.f;
        float scale_z = scale_dir.z() / scale_base_dir.z() - 1.f;
        scale_vec.x() = 1.f + (std::isnan(scale_x) ? 0.f : scale_x);
        scale_vec.y() = 1.f + (std::isnan(scale_y) ? 0.f : scale_y);
        scale_vec.z() = 1.f + (std::isnan(scale_z) ? 0.f : scale_z);
    }
    else
    {
        scale_vec.x() = scale;
        scale_vec.y() = scale;
        scale_vec.z() = scale;
    }

    Engine::Mat43 transMat;
    transMat.translate(NousConvert::convert(nous::vec3(-m_begin_pos)));
    Engine::Mat43 scaleMat;
    scaleMat.scale(NousConvert::convert(scale_vec));
    scaleMat = transMat.inverse() * scaleMat * transMat;

    for(auto& bundle: m_affected_bundles)
    {
        bundle.first->set_matrix_from_world(scaleMat * NousConvert::convert(bundle.second));
    }

    for(auto& nurbs_change_info: m_affected_nurbs_points)
    {
        NurbsDoc* nurbs_doc = nurbs_change_info.first;
        Engine::Mat43 mat = nurbs_doc->world_matrix();
        Engine::Mat43 inv_mat = nurbs_doc->world_matrix().inverse();
        for(auto& ctp_ptr: nurbs_change_info.second)
        {
            const nous::pos_t ori_pos = ctp_ptr.second;
            const nous::pos_t new_pos = NousConvert::convert(inv_mat * scaleMat * mat * NousConvert::convert(ori_pos));
            ctp_ptr.first->set_position(new_pos);
        }
        nurbs_doc->dirty();
    }

    for(auto& mesh_change_info: m_affected_mesh_local_position)
    {
        Engine::Mat43 mat = mesh_change_info.first->world_matrix();
        Engine::Mat43 invMat = mesh_change_info.first->world_matrix_inv();
        auto geo = mesh_change_info.first->geodata();
        std::vector<std::pair<SketchPointPtr, nous::vec3>> new_pos;
        for(auto& point: mesh_change_info.second)
        {
            new_pos.emplace_back(point.first, NousConvert::convert(invMat * scaleMat * mat * NousConvert::convert(point.second)));
        }
        GeoDataTopoOperations::get().change_point_positon(geo.get(), new_pos);
        // mesh->dirty();
    }
    //_update_new_meshes();
    for(auto& affected_mesh_polygon: m_affected_mesh_polygons)
    {
        for(auto& polygons: affected_mesh_polygon.second)
        {
            polygons->m_changed = true;
        }
    }
}

void Controller_Scale::_gen_meshes(View* view)
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
bool Controller_Scale::_is_select_invalid_element(ElementSelector* selector)
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

void Controller_Scale::_roll_back_to_pick_element()
{
    if(m_scale_step == SCALE_BEGIN_FIXING || m_scale_step == SCALE_PICK_BEGIN_POS || m_scale_step == SCALE_PICK_END_POS ||
       m_scale_step == SCALE_FIXING)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        CmdManager* cmd_mgr = STCore::inst->cmdMgr();
        cmd_mgr->pop_undo(view->doc()->scene()->rvtId());
        _clear_affected_meshes();
        _clear_all_tmp_cache();
        m_scale_step = PICK_ELEMENT;
    }
}

void Controller_Scale::_clear_all_tmp_cache()
{
    m_scale = 0.0;
    nous::vec3 m_end_pos = nous::pos_t::Zero();
    nous::vec3 m_begin_pos = nous::pos_t::Zero();
    nous::vec3 m_ctrl_pos = nous::pos_t::Zero();
    nous::vec3 m_hover_pos = nous::pos_t::Zero();
}
}// namespace sketch
