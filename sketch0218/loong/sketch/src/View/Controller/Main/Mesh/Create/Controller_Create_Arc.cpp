#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_Arc.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/STCore.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

namespace sketch
{
Controller_Create_Arc::Controller_Create_Arc(const std::wstring& name) : Controller_Main(name)
{
    m_create_mode = ECreateArcMode::TwoPointDrawArc;
    m_create_step = ECreateArcStep::PickBasePosition;
}

Controller_Create_Arc::~Controller_Create_Arc() {}

SketchParamMap Controller_Create_Arc::info() const
{
    SketchParamMap params;

    if(m_create_mode == ECreateArcMode::RadiusDrawArc)
    {
        if(m_create_step == ECreateArcStep::PickGuidePosition)
        {
            params.set_value("current_param", "distance");
        }

        if(m_create_step == ECreateArcStep::FixingArc)
        {
            params.set_value("current_param", "angle");
        }
    }
    else
    {
        if(m_create_step == ECreateArcStep::PickGuidePosition)
        {
            params.set_value("current_param", "distance");
        }

        if(m_create_step == ECreateArcStep::FixingArc)
        {
            params.set_value("current_param", "bulge");
        }
    }

    params.set_value("name", m_name);
    params.set_value("value", m_current_value);

    return params;
}

void Controller_Create_Arc::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    set_tip(L"圆弧");
    set_cursor(Define::CURSOR_ARROW);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer = view->inferHelper();
    infer->set_infer_filter_type(InferType::ALL_TYPE);
    m_create_mode = ECreateArcMode::TwoPointDrawArc;
    m_create_step = ECreateArcStep::PickBasePosition;
    m_radius_guide_normal = nous::vec3::Zero();
    m_X_pos = nous::pos_t::Zero();
    m_P_pos = nous::pos_t::Zero();
}

void Controller_Create_Arc::deactive() { Controller_Main::deactive(); }

void Controller_Create_Arc::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_create_step == ECreateArcStep::FixingArc)
    {
        _rollback_to_pick_guide_position();
    }
    else if(m_create_step == ECreateArcStep::PickGuidePosition)
    {
        _rollback_to_pick_base_position();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Create_Arc::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();

    if(this->is_lock())
    {
        float value = 0.0;
        if(Util::is_valid_length_expression(m_current_value))
        {
            auto trans = Util::extract_value_and_unit(m_current_value);
            value = trans.first;

            if(m_create_mode == ECreateArcMode::TwoPointDrawArc)
            {
                if(m_create_step == ECreateArcStep::PickGuidePosition)
                {
                    if(value < 1e-2)
                    {
                        return;
                    }
                    const nous::vec3 guide_dir = (m_hover_pos - m_arc_pos0).normalized();
                    m_arc_pos1 = m_arc_pos0 + guide_dir * value;
                    RvtBlock rvt_block(view->doc()->scene()->rvtId());
                    m_tmp_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                    view->inferHelper()->add_ignore(m_tmp_mesh_doc->renderObject());
                    m_create_step = ECreateArcStep::FixingArc;
                }
                else if(m_create_step == ECreateArcStep::FixingArc)
                {
                    if(value < 1e-2)
                    {
                        return;
                    }
                    const nous::vec3 guide_line_dir = (m_arc_pos1 - m_arc_pos0).normalized();
                    m_X_pos = m_arc_pos1 * 0.5 + m_arc_pos0 * 0.5;
                    m_P_pos = sketch_math::point_to_plane_closest(m_hover_pos, m_X_pos, guide_line_dir);
                    _generate_arc_by_two_point_core(value, (m_P_pos - m_X_pos).normalized());
                    _finish_arc(view);
                }
            }
            else
            {
                if(m_create_step == ECreateArcStep::PickGuidePosition)
                {
                    if(value < 1e-2)
                    {
                        return;
                    }
                    const nous::vec3 guide_dir = (m_hover_pos - m_arc_pos0).normalized();
                    m_arc_pos1 = m_arc_pos0 + guide_dir * value;
                    m_radius = (m_arc_pos1 - m_arc_pos0).norm();
                    m_radius_base_dir = (m_arc_pos1 - m_arc_pos0).normalized();
                    m_radius_last_dir = m_radius_base_dir;
                    RvtBlock rvt_block(view->doc()->scene()->rvtId());
                    m_tmp_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                    view->inferHelper()->add_ignore(m_tmp_mesh_doc->renderObject());
                    m_create_step = ECreateArcStep::FixingArc;
                }
                else if(m_create_step == ECreateArcStep::FixingArc)
                {
                    if(value < 1e-5 || m_radius_guide_normal.norm() < 1e-5)
                    {
                        return;
                    }
                    m_accumulate_angle = value * sketch_math::sketch_pi / 180.0;
                    nous::transform3 tmp_rot_mat = nous::transform3::Identity();
                    tmp_rot_mat.rotate(Eigen::AngleAxis<nous::scalar>(m_accumulate_angle, m_radius_guide_normal));
                    m_radius_last_dir = tmp_rot_mat * m_radius_base_dir;

                    _generate_arc_by_radius_core();
                    _finish_arc(view);
                }
            }
        }

        _unlock_mouse_move();
    }
}

void Controller_Create_Arc::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(m_create_step == ECreateArcStep::PickBasePosition && shift)
    {
        if(m_create_mode == ECreateArcMode::RadiusDrawArc)
        {
            m_create_mode = ECreateArcMode::TwoPointDrawArc;
        }
        else
        {
            m_create_mode = ECreateArcMode::RadiusDrawArc;
            m_radius_guide_normal = nous::vec3::Zero();
        }
    }
}

void Controller_Create_Arc::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Arc::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
    {
        return;
    }

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_create_step == ECreateArcStep::PickBasePosition)
    {
        m_arc_pos0 = m_hover_pos;
        m_create_step = ECreateArcStep::PickGuidePosition;
    }
    else if(m_create_step == ECreateArcStep::PickGuidePosition)
    {
        if(sketch_math::point_equal(m_hover_pos, m_arc_pos0))
        {
            // ToDo：向前端发送消息，半径为 0，非法的创建圆弧
            return;
        }
        m_arc_pos1 = m_hover_pos;

        {
            // 半径画弧模式
            m_radius = (m_arc_pos1 - m_arc_pos0).norm();
            m_radius_base_dir = (m_arc_pos1 - m_arc_pos0).normalized();
            m_radius_last_dir = m_radius_base_dir;
        }

        RvtBlock rvt_block(view->doc()->scene()->rvtId());
        m_tmp_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
        view->inferHelper()->add_ignore(m_tmp_mesh_doc->renderObject());
        m_create_step = ECreateArcStep::FixingArc;
    }
    else
    {
        _finish_arc(view);
    }
}

void Controller_Create_Arc::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->inferHelper()->update_inference(view, x, y);
    auto&& infer_result = view->inferHelper()->get_infer_result();
    if(infer_result.infer_type != InferType::NONE)
    {
        m_hover_pos = infer_result.position;
        m_radius_mode_arc_normal = infer_result.normal;
    }
    else
    {
        ViewCamera* curr_camera = view->camera();
        Document* global_doc = STCore::inst->docMgr()->document();
        auto& view_port = curr_camera->viewport();
        Engine::Vec3 view_ori, view_dir;
        curr_camera->getDocPickRay(global_doc, (float)x + view_port.x, (float)y + view_port.y, &view_ori, &view_dir);
        m_hover_pos = NousConvert::convert(view_ori);
    }

    if(m_create_mode == ECreateArcMode::RadiusDrawArc)
    {
        if(m_create_step >= ECreateArcStep::PickGuidePosition)
        {
            if(!sketch_math::point_on_plane(m_hover_pos, m_arc_pos0, m_radius_mode_arc_normal))
            {
                m_hover_pos = sketch_math::point_to_plane_closest(m_hover_pos, m_arc_pos0, m_radius_mode_arc_normal);
            }
        }
    }

    if(m_create_step == ECreateArcStep::PickGuidePosition)
    {
        m_current_value = Util::cut_string_with_two_decimals((m_hover_pos - m_arc_pos0).norm());
    }

    if(m_create_step == ECreateArcStep::FixingArc)
    {
        if(m_create_mode == ECreateArcMode::TwoPointDrawArc)
        {
            _generate_arc_by_two_point();
        }
        else
        {
            _generate_arc_by_radius();
        }
    }

    _update_plane_helper_mesh(infer_result);

    view->dirtyRT();
}

void Controller_Create_Arc::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Arc::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_Arc::prepare_render(View* view)
{
    _update_helper_mesh(view->inferHelper());
    _render_helper_mesh(view);
    if(static_cast<uint8_t>(m_create_step) > static_cast<uint8_t>(ECreateArcStep::PickBasePosition))
    {
        view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(m_arc_pos0));

        if(m_create_step == ECreateArcStep::PickGuidePosition)
        {
            view->batchRenderLine(1, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_arc_pos0), NousConvert::convert(m_hover_pos), 0xffff,
                                  1, true, false);
        }
    }

    if(static_cast<uint8_t>(m_create_step) > static_cast<uint8_t>(ECreateArcStep::PickGuidePosition))
    {
        view->batchRenderCtrlPoints(0, false, false, NousConvert::convert(m_arc_pos1));
        view->batchRenderLine(1, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_arc_pos0), NousConvert::convert(m_arc_pos1), 0xffff, 1,
                              true, false);
    }

    if(m_create_step == ECreateArcStep::FixingArc)
    {
        if(m_create_mode == ECreateArcMode::TwoPointDrawArc)
        {
            view->batchRenderLine(1, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_X_pos), NousConvert::convert(m_P_pos), 0xffff, 1, true,
                                  false);
        }
        else
        {
            const nous::pos_t middle_pos = m_arc_pos0 + m_radius_last_dir * m_radius;
            view->batchRenderLine(1, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_arc_pos0), NousConvert::convert(middle_pos), 0xffff, 1,
                                  true, false);
            view->batchRenderLine(1, 1.0, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(middle_pos), NousConvert::convert(m_hover_pos), 0xf00f,
                                  1, true, false);
        }
    }

    if(m_create_mode == ECreateArcMode::RadiusDrawArc)
    {
        for(int i = 0; i < m_infer_operator_plane.size(); ++i)
        {
            view->batchRenderLine(1, 1.0f, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(m_infer_operator_plane[i]),
                                  NousConvert::convert(m_infer_operator_plane[(i + 1) % m_infer_operator_plane.size()]), 0xffffu, 1, false, true);
        }

        for(auto& line: m_infer_operator_plane_scale_lines)
        {
            view->batchRenderLine(1, 1.0f, Engine::Vec3f(0.0, 0.0, 0.0), NousConvert::convert(line.first), NousConvert::convert(line.second), 0xffffu,
                                  1, false, true);
        }
    }
}

void Controller_Create_Arc::_finish_arc(View* view)
{
    // _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ m_tmp_mesh_doc }, std::unordered_set<MeshDoc*>(), view->doc()->scene());

    std::unordered_set<TFObjectDoc*> scene_docs;
    for(auto& obj: view->visObjects())
    {
        if(obj->doc() != m_tmp_mesh_doc)
        {
            scene_docs.insert(obj->doc());
        }
    }
    std::unordered_set<TFObjectDoc*> curr_docs;
    curr_docs.insert(m_tmp_mesh_doc);
    view->check_intersection_and_merge_meshes(curr_docs, scene_docs);

    view->inferHelper()->clear_ignore();
    m_tmp_mesh_doc = nullptr;
    _rollback_to_pick_guide_position();
    _rollback_to_pick_base_position();
}

void Controller_Create_Arc::_rollback_to_pick_guide_position()
{
    if(m_create_step == ECreateArcStep::FixingArc)
    {
        if(m_tmp_mesh_doc)
        {
            View* view = STCore::inst->viewMgr()->layout()->focusedView();
            view->delete_obj_and_try_delete_relation(m_tmp_mesh_doc);
            m_tmp_mesh_doc = nullptr;
        }
        m_arc_pos1 = nous::pos_t::Zero();
        m_create_step = ECreateArcStep::PickGuidePosition;
        m_accumulate_angle = 0.0;
        m_radius_guide_normal = nous::vec3::Zero();
        m_X_pos = nous::pos_t::Zero();
        m_P_pos = nous::pos_t::Zero();
    }
}

void Controller_Create_Arc::_rollback_to_pick_base_position()
{
    if(m_create_step == ECreateArcStep::PickGuidePosition)
    {
        m_arc_pos0 = nous::pos_t::Zero();
        m_create_step = ECreateArcStep::PickBasePosition;
    }
}

void Controller_Create_Arc::_generate_arc_by_two_point()
{
    const nous::vec3 guide_line_dir = (m_arc_pos1 - m_arc_pos0).normalized();
    m_X_pos = m_arc_pos1 * 0.5 + m_arc_pos0 * 0.5;
    m_P_pos = sketch_math::point_to_plane_closest(m_hover_pos, m_X_pos, guide_line_dir);
    const double bulge_dis = (m_X_pos - m_P_pos).norm();
    m_current_value = Util::cut_string_with_two_decimals(bulge_dis);
    _generate_arc_by_two_point_core((m_X_pos - m_P_pos).norm(), (m_P_pos - m_X_pos).normalized());
}

void Controller_Create_Arc::_generate_arc_by_radius()
{
    const nous::vec3 curr_dir = (m_hover_pos - m_arc_pos0).normalized();
    if(curr_dir.norm() < 1e-5)
    {
        return;
    }
    double delta_dot_v = m_radius_last_dir.dot(curr_dir);
    if(delta_dot_v > 1.0)
    {
        delta_dot_v = 1.0;
    }
    const double delta_angle = acos(delta_dot_v);
    if(delta_angle <= sketch_math::sketch_pi / 18000.0)
    {
        return;
    }

    m_tmp_mesh_doc->geodata()->clear();
    if(m_radius_guide_normal.norm() < 1e-5)
    {
        m_radius_guide_normal = -curr_dir.cross(m_radius_base_dir).normalized();
        m_accumulate_angle += delta_angle;
    }
    else
    {
        const nous::vec3 move_delta_normal = m_radius_last_dir.cross(curr_dir).normalized();
        const double order_dot_v = move_delta_normal.dot(m_radius_guide_normal);
        double new_angle = 0.0;
        if(order_dot_v > 0)
        {
            new_angle = m_accumulate_angle + delta_angle;
            if(new_angle > sketch_math::sketch_pi * 2)
            {
                new_angle -= (sketch_math::sketch_pi * 2);
            }
        }
        else
        {
            new_angle = m_accumulate_angle - delta_angle;
            if(new_angle < 0)
            {
                new_angle = -new_angle;
                m_radius_guide_normal = -m_radius_guide_normal;
            }
        }
        m_accumulate_angle = new_angle;
    }

    double tmp_angle = m_accumulate_angle;
    unsigned int multiply = 0;
    while(tmp_angle - (sketch_math::sketch_pi / 12.0) > 0)
    {
        tmp_angle -= (sketch_math::sketch_pi / 12.0);
        ++multiply;
    }
    if(tmp_angle <= 1 / 180.0 * sketch_math::sketch_pi)
    {
        m_accumulate_angle = multiply * (sketch_math::sketch_pi / 12.0);
    }
    else if(tmp_angle >= 14 / 180.0 * sketch_math::sketch_pi)
    {
        m_accumulate_angle = (multiply + 1) * (sketch_math::sketch_pi / 12.0);
    }

    nous::transform3 tmp_rot_mat = nous::transform3::Identity();
    tmp_rot_mat.rotate(Eigen::AngleAxis<nous::scalar>(m_accumulate_angle, m_radius_guide_normal));
    m_radius_last_dir = tmp_rot_mat * m_radius_base_dir;

    m_current_value = Util::cut_string_with_two_decimals(m_accumulate_angle * 180.0 / sketch_math::sketch_pi);

    _generate_arc_by_radius_core();
}

void Controller_Create_Arc::_update_plane_helper_mesh(InferResult infer)
{
    // params
    nous::vec3 normal = m_radius_mode_arc_normal;
    nous::vec3 center = m_arc_pos0;
    if(m_create_step <= ECreateArcStep::PickBasePosition)
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
    nous::vec3 first_positon_dir = (m_arc_pos1 - m_arc_pos0).normalized();
    if(m_create_step == ECreateArcStep::PickBasePosition)
    {
        nous::vec3 arbitrary_dir;
        nous::vec3 axes[] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
        for(const auto& axis: axes)
        {
            float d = std::abs(normal.dot(axis));
            if(abs(abs(d) - 1.f) < sketch_math::sketch_epsilon * sketch_math::sketch_epsilon)
            {
                continue;
            }
            arbitrary_dir = axis;
            break;
        }
        first_positon_dir = normal.cross(arbitrary_dir).normalized();
    }
    else if(m_create_step == ECreateArcStep::PickGuidePosition)
    {
        first_positon_dir = (m_hover_pos - m_arc_pos0).normalized();
    }

    nous::vec3 first_positon = center + first_positon_dir * view_scale;

    // Calculate other points on the circle
    m_infer_operator_plane_scale_lines.clear();
    m_infer_operator_plane.clear();
    float angleIncrement = 2.0 * 3.14159 / 24;
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

    for(size_t i = 0; i < 24; i++)
    {
        float angle = i * angleIncrement;
        gen_circle_and_scale_line(angle, m_infer_operator_plane, m_infer_operator_plane_scale_lines);
    }
    m_infer_operator_plane_scale_lines.emplace_back(m_infer_operator_plane[0], m_infer_operator_plane[12]);
}

void Controller_Create_Arc::_generate_arc_by_two_point_core(const double bulge, const nous::vec3& dir)
{
    m_tmp_mesh_doc->geodata()->clear();
    const double d = (m_arc_pos1 - m_arc_pos0).norm() / 2.0;
    const double l = bulge;
    const double r = (d * d + l * l) / (2 * l);
    const nous::vec3 verticle_dir = dir;
    nous::pos_t O = m_X_pos;
    if(d < l)
    {
        O += verticle_dir * (l - r);
    }
    else
    {
        O -= verticle_dir * (r - l);
    }

    const nous::vec3 dir_OM = (m_arc_pos0 - O).normalized();
    const nous::vec3 dir_ON = (m_arc_pos1 - O).normalized();

    // 分两段构建圆弧，OM 转到 OP，OP 再转到 ON
    NoBranchLineGroupPtr weld_arc_obj = std::make_shared<NoBranchLineGroup>();
    Eigen::Quaternion<nous::scalar> identity_quat = Eigen::Quaternion<nous::scalar>::Identity();
    SketchPointPtr last_pt = nullptr;
    {
        Eigen::Quaternion<nous::scalar> OM_to_OP_quat = Eigen::Quaternion<nous::scalar>::FromTwoVectors(dir_OM, verticle_dir);
        for(int i = 0; i <= 12; ++i)
        {
            const double percent = 1.0 / 12.0 * i;
            Eigen::Quaternion<nous::scalar> interpolate_quat = identity_quat.slerp(percent, OM_to_OP_quat);
            const nous::vec3 rot_dir = interpolate_quat.matrix() * dir_OM;
            const nous::pos_t curr_pos = O + rot_dir * r;
            SketchPointPtr curr_pt = m_tmp_mesh_doc->geodata()->add_point_without_check(curr_pos);
            if(i > 0)
            {
                auto& sk_line = m_tmp_mesh_doc->geodata()->add_line_without_check(last_pt, curr_pt);
                weld_arc_obj->add_element(sk_line);
            }
            last_pt = curr_pt;
        }
    }
    {
        Eigen::Quaternion<nous::scalar> OP_to_ON_quat = Eigen::Quaternion<nous::scalar>::FromTwoVectors(verticle_dir, dir_ON);
        for(int i = 0; i <= 12; ++i)
        {
            const double percent = 1.0 / 12.0 * i;
            Eigen::Quaternion<nous::scalar> interpolate_quat = identity_quat.slerp(percent, OP_to_ON_quat);
            const nous::vec3 rot_dir = interpolate_quat.matrix() * verticle_dir;
            const nous::pos_t curr_pos = O + rot_dir * r;
            SketchPointPtr curr_pt = m_tmp_mesh_doc->geodata()->add_point_without_check(curr_pos);
            auto& sk_line = m_tmp_mesh_doc->geodata()->add_line_without_check(last_pt, curr_pt);
            weld_arc_obj->add_element(sk_line);
            last_pt = curr_pt;
        }
    }

    m_tmp_mesh_doc->geodata()->add_element_group(weld_arc_obj);
    m_tmp_mesh_doc->dirty();
}

void Controller_Create_Arc::_generate_arc_by_radius_core()
{
    m_tmp_mesh_doc->geodata()->clear();
    NoBranchLineGroupPtr weld_arc_obj = std::make_shared<NoBranchLineGroup>();

    if(m_accumulate_angle == sketch_math::sketch_pi * 2)
    {
        std::vector<nous::pos_t> circle_pos_arr;
        for(int i = 0; i <= 24; ++i)
        {
            const double rot_angle = m_accumulate_angle / 24.0 * i;
            nous::transform3 rot_transform = nous::transform3::Identity();
            rot_transform.rotate(Eigen::AngleAxis<nous::scalar>(rot_angle, m_radius_guide_normal));
            const nous::pos_t curr_pos = rot_transform * m_radius_base_dir * m_radius + m_arc_pos0;
            circle_pos_arr.emplace_back(curr_pos);
        }
        m_tmp_mesh_doc->geodata()->add_polygon(std::vector<std::vector<nous::pos_t>>{ circle_pos_arr });
        for(auto& sk_line: m_tmp_mesh_doc->geodata()->get_all_lines())
        {
            weld_arc_obj->add_element(sk_line.second);
        }
    }
    else
    {
        SketchPointPtr prev_pt = nullptr;
        for(int i = 0; i <= 24; ++i)
        {
            const double rot_angle = m_accumulate_angle / 24.0 * i;
            nous::transform3 rot_transform = nous::transform3::Identity();
            rot_transform.rotate(Eigen::AngleAxis<nous::scalar>(rot_angle, m_radius_guide_normal));
            const nous::pos_t curr_pos = rot_transform * m_radius_base_dir * m_radius + m_arc_pos0;
            SketchPointPtr curr_pt = m_tmp_mesh_doc->geodata()->add_point_without_check(curr_pos);
            if(i > 0)
            {
                auto& sk_line = m_tmp_mesh_doc->geodata()->add_line_without_check(prev_pt, curr_pt);
                weld_arc_obj->add_element(sk_line);
            }
            prev_pt = curr_pt;
        }
    }

    m_tmp_mesh_doc->geodata()->add_element_group(weld_arc_obj);
    m_tmp_mesh_doc->dirty();
}

};// namespace sketch