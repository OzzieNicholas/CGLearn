#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Extrude_Polygon.h"

#include "Engine/Math/GeoUtil.h"
#include "Engine/Render/Renderer.h"

#include "sketch/STCore.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"

#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/Select/InferenceHelper.h"

#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

#include "nous_mesh/algorithm/intersect.h"

namespace sketch
{
Controller_Extrude_ByNormal::Controller_Extrude_ByNormal(const std::wstring& name)
    : Controller_Edit(name), m_extrude_mode(ESketchExtrudeMode::MOVE), m_extrude_step(ESketchExtrudeStep::PICK_ELEMENT), m_pick_mesh_doc(nullptr),
      m_pick_polygon_normal(nous::vec3::Zero())
{
}

Controller_Extrude_ByNormal::~Controller_Extrude_ByNormal() {}

SketchParamMap Controller_Extrude_ByNormal::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "distance");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Extrude_ByNormal::active(const SketchParamMap& props)
{
    Controller_Edit::active(props);

    this->set_tip(L"select one face to extrude.");
    this->set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::POLYGON);
    infer_helper->clear_refer();

    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;
    auto& select_info = view->selector()->get_select_info();
    if(select_info.size() == 1 && select_info.begin()->second.elements.size() == 1)
    {
        EElementType type = select_info.begin()->second.elements.begin()->get()->type();
        if(type == EElementType::Polygon || type == EElementType::AlgebraSurface)
        {
            m_extrude_step = ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING;
        }
    }

    if(m_extrude_step != ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        view->selector()->clear();
    }

    if(!NousEngine::is_inited())
    {
        NousEngine::instance()->init();
    }
}

void Controller_Extrude_ByNormal::deactive() { Controller_Edit::deactive(); }

void Controller_Extrude_ByNormal::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        _rollback_to_pick_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Extrude_ByNormal::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto trans = Util::extract_value_and_unit(m_current_value);
                float t1 = trans.first;
                // judge direction
                const nous::vec3 mouse_dir = (m_hover_pos - m_extrude_polygon_begin_position).normalized();
                const int sign = mouse_dir.dot(m_pick_polygon_normal) > 0 ? 1 : -1;

                if(m_extrude_mode == ESketchExtrudeMode::ADD)
                {
                    create_add_operation_mesh(t1 * sign);
                }
                else if(m_extrude_mode == ESketchExtrudeMode::MOVE)
                {
                    create_move_operation_mesh(t1 * sign);
                }
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        commit_extrude_mesh();
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        view->inferHelper()->clear_ignore();
        view->inferHelper()->set_infer_filter_type(InferType::POLYGON);
        _unlock_mouse_move();
        _clear_affected_meshes();
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Extrude_ByNormal::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(shift)
    {
        this->switchMesh();
        if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            m_extrude_mode = ESketchExtrudeMode::ADD;
        }
        else if(m_extrude_mode == ESketchExtrudeMode::ADD)
        {
            m_extrude_mode = ESketchExtrudeMode::MOVE;
        }
        View* pView = STCore::inst->viewMgr()->layout()->focusedView();
        pView->dirtyRT();
    }
}

void Controller_Extrude_ByNormal::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    auto& infer_info = view->inferHelper()->get_infer_result();
    auto select_info = view->selector()->get_select_info();
    if(m_extrude_step == ESketchExtrudeStep::PICK_ELEMENT)
    {
        if(select_info.size() == 1)
        {
            m_extrude_step = ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING;
        }
    }

    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        RvtBlock blk(view->doc()->scene()->rvtId());
        if(!_process_select_info(select_info))
        {
            m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;
            return;
        }

        if(m_move_inferring_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_move_inferring_mesh_doc->renderObject());
            m_move_inferring_mesh_doc->dirty();
        }
        if(m_add_inferring_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_add_inferring_mesh_doc->renderObject());
            m_add_inferring_mesh_doc->dirty();
        }
        if(m_splited_old_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_splited_old_mesh_doc->renderObject());
            m_splited_old_mesh_doc->dirty();
        }
        if(m_copyed_origin_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_copyed_origin_mesh_doc->renderObject());
            m_copyed_origin_mesh_doc->dirty();
        }

        m_extrude_step = ESketchExtrudeStep::EXTRUDE_FIXING;
        if(m_pick_mesh_doc)
        {
            m_pick_mesh_doc->dirty();
            m_is_extrude_nurbs = false;
        }
        if(m_pick_nurbs_doc)
        {
            m_extrude_polygon_begin_position = view->inferHelper()->get_infer_result().position;
            m_pick_polygon_normal = infer_info.normal;
            m_pick_nurbs_doc->dirty();
            m_is_extrude_nurbs = true;
        }
    }
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        RvtBlock blk(view->doc()->scene()->rvtId(), true);
        commit_extrude_mesh();
        view->inferHelper()->clear_ignore();
        view->inferHelper()->set_infer_filter_type(InferType::POLYGON);
    }

    return;
}

void Controller_Extrude_ByNormal::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Edit::mouse_move(button, x, y, ctrl, alt, shift);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    // infer_helper->clear_refer();
    auto& infer_info = infer_helper->get_infer_result();

    if(m_extrude_step == ESketchExtrudeStep::PICK_ELEMENT)
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
                }
                else
                {
                    std::unordered_set<SketchElementPtr> element;
                    if(infer_info.infer_type & InferType::POLYGON || infer_info.infer_type & InferType::SURFACE)
                    {
                        element.insert(infer_info.element);
                    }
                    view->selector()->add_select_element(infer_info.mesh, element);
                }
            }
        }
    }
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y);
    }
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y);
            ViewCamera* curr_camera = view->camera();
            Document* global_doc = STCore::inst->docMgr()->document();
            auto& view_port = curr_camera->viewport();
            Engine::Vec3 view_ori, view_dir;
            curr_camera->getDocPickRay(global_doc, (float)x + view_port.x, (float)y + view_port.y, &view_ori, &view_dir);

            // 通过平面法线方向和相机视角方向的交点得出 offset 值
            double t0, t1;
            rayIntersect(view_ori, view_dir, NousConvert::convert(m_extrude_polygon_begin_position), NousConvert::convert(m_pick_polygon_normal), t0,
                         t1);
            if(t0 < 0)
            {
                return;
            }
            nous::vec3 offset = m_pick_polygon_normal * t1;
            m_current_value = Util::cut_string_with_two_decimals(t1);

            InferType infer_type = infer_helper->get_infer_result().infer_type;
            bool replace_ori_offset = false;
            nous::pos_t infer_position;
            if(infer_type & InferType::ELEMENT_TYPE)
            {
                // 优先点
                infer_position = infer_helper->get_infer_result().position;
                m_hover_pos = infer_position;
                replace_ori_offset = true;
            }
            else
            {
                m_hover_pos = m_extrude_polygon_begin_position + offset;
            }

            if(replace_ori_offset)
            {
                const nous::pos_t proj_pos = sketch_math::point_to_line_closest(infer_position, m_extrude_polygon_begin_position,
                                                                                m_extrude_polygon_begin_position + m_pick_polygon_normal, false);
                t1 = (proj_pos - m_extrude_polygon_begin_position).norm() *
                     (proj_pos - m_extrude_polygon_begin_position).normalized().dot(m_pick_polygon_normal);
            }

            if(!m_is_extrude_nurbs)
            {
                if(m_extrude_mode == ESketchExtrudeMode::ADD)
                {
                    create_add_operation_mesh(t1);
                }
                else if(m_extrude_mode == ESketchExtrudeMode::MOVE)
                {
                    create_move_operation_mesh(t1);
                }
            }
            else
            {
                _create_nurbs_operation_mesh(t1);
            }
        }
    }

    view->dirtyRT();
    return;
}

void Controller_Extrude_ByNormal::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_ByNormal::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);
}

std::unordered_set<SketchRingPtr> Controller_Extrude_ByNormal::get_valid_coplane_rings(std::unordered_set<SketchRingPtr>& line_adj_rings,
                                                                                       std::unordered_set<SketchRingPtr>& self_rings,
                                                                                       GeoData* geodata)
{
    std::unordered_set<SketchRingPtr> result;

    // 只有两个面或者三个面的时候可能存在垂直共面
    if(line_adj_rings.size() != 2 && line_adj_rings.size() != 3)
    {
        return result;
    }

    nous::vec3 self_normal = (*self_rings.begin())->normal;

    for(SketchRingPtr adj_ring: line_adj_rings)
    {
        if(self_rings.find(adj_ring) != self_rings.end())
        {
            continue;
        }

        nous::vec3 adj_ring_normal = adj_ring->normal;
        double dot_value = adj_ring_normal.dot(m_pick_polygon_normal);

        if(!(dot_value > -1e-5 && dot_value < 1e-5))
        {
            result.clear();
            return result;
        }

        result.insert(adj_ring);
    }

    return result;
}

std::vector<nous::pos_t> Controller_Extrude_ByNormal::offset_ring_positions(GeoData* picked_geo, SketchRingPtr ring, SketchPointPtr prev_pt,
                                                                            SketchPointPtr next_pt, double offset)
{
    std::vector<nous::pos_t> result;
    std::vector<SketchPointPtr> ring_pt_arr = GeoDataUtils::get().get_ring_topo(picked_geo, ring);

    bool add_order = true;
    bool first_find = true;

    std::pair<int, int> check_flags = { -1, -1 };

    for(int i = 0; i < ring_pt_arr.size(); ++i)
    {
        // TODO: 这里要判要不要加点，共线用原先的，不共线加一个新的

        // 线不加，不共线加，仅针对单个面共面使用，双面共面不走这个方法
        SketchPointPtr curr_pt = ring_pt_arr[i];
        const nous::pos_t pt_pos = picked_geo->get_point_position(curr_pt);
        const nous::pos_t offset_pos = pt_pos + m_pick_polygon_normal * offset;
        SketchPointPtr curr_next_pt = ring_pt_arr[(i + 1) % ring_pt_arr.size()];

        if(curr_pt == prev_pt)
        {
            nous::pos_t infer_pos;
            // prev_pt 和 next_pt 一定相邻，所以下一个不是 next 那上一个一定是 next
            if(curr_next_pt == next_pt)
            {
                // 向前找一个，计算方向
                SketchPointPtr curr_prev_pt = ring_pt_arr[(i - 1 + ring_pt_arr.size()) % ring_pt_arr.size()];
                infer_pos = picked_geo->get_point_position(curr_prev_pt);
                if(first_find)
                {
                    add_order = true;
                    first_find = false;
                }
            }
            else
            {
                infer_pos = picked_geo->get_point_position(curr_next_pt);
                if(first_find)
                {
                    add_order = false;
                    first_find = false;
                }
            }

            const nous::vec3 line_dir = (pt_pos - infer_pos).normalized();
            const double dot_v = line_dir.dot(m_pick_polygon_normal);

            // 不共线的话加入一个重复当前点
            if(dot_v > -1.0 + 1e-5 && dot_v < 1.0 - 1e-5)
            {
                if(add_order)
                {
                    result.emplace_back(pt_pos);
                    result.emplace_back(offset_pos);
                }
                else
                {
                    result.emplace_back(offset_pos);
                    result.emplace_back(pt_pos);
                }
            }
            else
            {
                result.emplace_back(offset_pos);
                if(check_flags.first < 0)
                {
                    check_flags.first = result.size() - 1;
                }
                else
                {
                    check_flags.second = result.size() - 1;
                }
            }

            add_order = !add_order;
        }
        // 同理应用于 next_pt
        else if(curr_pt == next_pt)
        {
            SketchPointPtr curr_next_pt = ring_pt_arr[(i + 1) % ring_pt_arr.size()];
            nous::pos_t infer_pos;
            // prev_pt 和 next_pt 一定相邻，所以下一个不是 next 那上一个一定是 next
            if(curr_next_pt == prev_pt)
            {
                // 向前找一个，计算方向
                SketchPointPtr curr_prev_pt = ring_pt_arr[(i - 1 + ring_pt_arr.size()) % ring_pt_arr.size()];
                infer_pos = picked_geo->get_point_position(curr_prev_pt);
                if(first_find)
                {
                    add_order = true;
                    first_find = false;
                }
            }
            else
            {
                infer_pos = picked_geo->get_point_position(curr_next_pt);
                if(first_find)
                {
                    add_order = false;
                    first_find = false;
                }
            }

            const nous::vec3 line_dir = (pt_pos - infer_pos).normalized();
            const double dot_v = line_dir.dot(m_pick_polygon_normal);

            // 不共线的话加入一个重复当前点
            if(dot_v > -1.0 + 1e-5 && dot_v < 1.0 - 1e-5)
            {
                if(add_order)
                {
                    result.emplace_back(pt_pos);
                    result.emplace_back(offset_pos);
                }
                else
                {
                    result.emplace_back(offset_pos);
                    result.emplace_back(pt_pos);
                }
            }
            else
            {
                result.emplace_back(offset_pos);

                if(check_flags.first < 0)
                {
                    check_flags.first = result.size() - 1;
                }
                else
                {
                    check_flags.second = result.size() - 1;
                }
            }

            add_order = !add_order;
        }
        else
        {
            result.emplace_back(pt_pos);
        }
    }

    if(check_flags.first >= 0)
    {
        std::set<size_t, std::greater<size_t>> delete_id_set;
        // 至少存在一个点共线，需要检测这个 result 中是否有与需检测点连接且方向与 m_pick_normal 一致且多余的点

        for(int i = 0; i < result.size(); ++i)
        {
            const nous::pos_t prev_pos = result[i];
            const nous::pos_t next_pos = result[(i + 1) % result.size()];
            const nous::vec3 line_dir = (next_pos - prev_pos).normalized();

            const double dot_v = line_dir.dot(m_pick_polygon_normal);
            // 某线段方向和法线平行，向后继续找到不平行的点为止
            if(dot_v < -1.0 + 1e-5 || dot_v > 1.0 - 1e-5)
            {
                int j = i + 1;
                while(true)
                {
                    const nous::pos_t pp_pos = result[j % result.size()];
                    const nous::pos_t pn_pos = result[(j + 1) % result.size()];
                    const nous::vec3 line_dir = (pn_pos - pp_pos).normalized();

                    const double dot_v = line_dir.dot(m_pick_polygon_normal);
                    if(dot_v > -1.0 + 1e-5 && dot_v < 1.0 - 1e-5)
                    {
                        break;
                    }
                    ++j;
                }

                // i 和 j 之间的点看有没有冗余
                std::unordered_set<int> record_idx_set;
                bool use_this_set = false;
                for(int m = i; m <= j; ++m)
                {
                    const int act_idx = m % result.size();
                    if(act_idx == check_flags.first || act_idx == check_flags.second)
                    {
                        use_this_set = true;
                    }
                    else if(m > i && m < j)
                    {
                        record_idx_set.insert(act_idx);
                    }
                }

                if(use_this_set)
                {
                    delete_id_set.insert(record_idx_set.begin(), record_idx_set.end());
                }

                i = j;
            }
        }

        for(size_t last_idx: delete_id_set)
        {
            result.erase(result.begin() + last_idx);
        }
    }

    return result;
}

std::vector<std::vector<nous::vec3>> Controller_Extrude_ByNormal::get_offseted_polygon_positions(GeoData* picked_geo, SketchPolygonPtr polygon,
                                                                                                 SketchPointPtr prev_pt, SketchPointPtr next_pt,
                                                                                                 double offset)
{
    std::vector<std::vector<nous::vec3>> result;
    std::vector<nous::vec3> exring_pos_arr = offset_ring_positions(picked_geo, polygon->ring, prev_pt, next_pt, offset);
    result.emplace_back(std::move(exring_pos_arr));

    for(auto& inner_ring: polygon->inner_rings)
    {
        std::vector<nous::vec3> inring_pos_arr = offset_ring_positions(picked_geo, inner_ring, prev_pt, next_pt, offset);
        result.emplace_back(std::move(inring_pos_arr));
    }

    return result;
}

Controller_Extrude_ByNormal::ESketchExtrudeMoveValidDir Controller_Extrude_ByNormal::need_flip_picked_normal(GeoData* geodata,
                                                                                                             SketchPolygonPtr pick_polygon)
{
    // 法线加一点坐标定义当前平面
    const nous::vec3 plane_normal = m_pick_polygon_normal;
    // const nous::vec3 plane_pos = geodata->get_point_position(pick_polygon->ring->ring_prim()->vertex_ptr()->point_ptr());

    unsigned char is_positive_plane_normal_valid = 1;
    unsigned char is_negative_plane_normal_valid = 2;

    for(auto& ring: m_selected_polygon_topo)
    {
        for(int i = 0; i < ring.size(); ++i)
        {
            SketchPointPtr prev_point = ring[i];
            SketchPointPtr next_point = ring[(i + 1) % ring.size()];

            const nous::vec3 prev_pt_pos = geodata->get_point_position(prev_point);
            const nous::vec3 next_pt_pos = geodata->get_point_position(next_point);

            const nous::vec3 line_dir = (next_pt_pos - prev_pt_pos).normalized();

            const nous::vec3 positive_offset_prev_pos = prev_pt_pos + 0.011 * plane_normal;
            const nous::vec3 positive_offset_next_pos = next_pt_pos + 0.011 * plane_normal;

            const nous::vec3 negative_offset_prev_pos = prev_pt_pos - 0.011 * plane_normal;
            const nous::vec3 negative_offset_next_pos = next_pt_pos - 0.011 * plane_normal;

            for(const nous::index_t nei_pt_idx: geodata->get_mesh()->point_neighbours_range(prev_point->point->idx()))
            {
                nous::Point* nei_pt = geodata->get_mesh()->point(nei_pt_idx);
                const nous::vec3 nei_pt_pos = geodata->get_point_position(nei_pt);

                if(nei_pt_idx == next_point->point->idx())
                {
                    continue;
                }

                const nous::vec3 link_line_dir = (nei_pt_pos - prev_pt_pos).normalized();

                const double dot_v = link_line_dir.dot(line_dir);
                if(dot_v > -1e-5 && dot_v < 1e-5)
                {
                    continue;
                }

                if(is_positive_plane_normal_valid)
                {
                    nous::pos_t ca, cb;
                    const bool is_intersect =
                        nous::algo::line_intersect_3d(positive_offset_prev_pos, positive_offset_next_pos, prev_pt_pos, nei_pt_pos, ca, cb, true);
                    if(is_intersect && (ca - cb).norm() < 1e-5)
                    {
                        is_positive_plane_normal_valid = false;
                    }
                }

                if(is_negative_plane_normal_valid)
                {
                    nous::pos_t ca, cb;
                    const bool is_intersect =
                        nous::algo::line_intersect_3d(negative_offset_prev_pos, negative_offset_next_pos, prev_pt_pos, nei_pt_pos, ca, cb, true);
                    if(is_intersect && (ca - cb).norm() < 1e-5)
                    {
                        is_negative_plane_normal_valid = false;
                    }
                }
            }

            for(const nous::index_t nei_pt_idx: geodata->get_mesh()->point_neighbours_range(next_point->point->idx()))
            {
                nous::Point* nei_pt = geodata->get_mesh()->point(nei_pt_idx);
                const nous::vec3 nei_pt_pos = geodata->get_point_position(nei_pt);

                if(nei_pt_idx == prev_point->point->idx())
                {
                    continue;
                }

                const nous::vec3 link_line_dir = (nei_pt_pos - next_pt_pos).normalized();
                const double dot_v = link_line_dir.dot(line_dir);
                if(dot_v > -1e-5 && dot_v < 1e-5)
                {
                    continue;
                }

                if(is_positive_plane_normal_valid)
                {
                    nous::pos_t ca, cb;
                    const bool is_intersect =
                        nous::algo::line_intersect_3d(positive_offset_prev_pos, positive_offset_next_pos, next_pt_pos, nei_pt_pos, ca, cb, true);
                    if(is_intersect && (ca - cb).norm() < 1e-5)
                    {
                        is_positive_plane_normal_valid = false;
                    }
                }

                if(is_negative_plane_normal_valid)
                {
                    nous::pos_t ca, cb;
                    const bool is_intersect =
                        nous::algo::line_intersect_3d(negative_offset_prev_pos, negative_offset_next_pos, next_pt_pos, nei_pt_pos, ca, cb, true);
                    if(is_intersect && (ca - cb).norm() < 1e-5)
                    {
                        is_negative_plane_normal_valid = false;
                    }
                }
            }
        }
    }

    return static_cast<Controller_Extrude_ByNormal::ESketchExtrudeMoveValidDir>(is_positive_plane_normal_valid + is_negative_plane_normal_valid);
}

void Controller_Extrude_ByNormal::processPickPolygon(GeoData* picked_geo, SketchRingPtr ring)
{
    // 将法线从模型空间旋转到世界空间
    Engine::Mat43 mat = m_pick_mesh_doc->world_matrix();
    SketchPolygonPtr pick_polygon = picked_geo->get_polygon(ring);
    m_pick_polygon_normal = pick_polygon->ring->normal;
    m_pick_polygon_normal = NousConvert::product_v3(mat, m_pick_polygon_normal - NousConvert::convert(mat.col[3]).normalized());

    m_extrude_polygon_begin_position = picked_geo->get_mesh()->prim_barycenter(ring->ring_prim->idx());

    // 这是处理多个离散内环的必要步骤，需要将多个相邻且离散的内环合并成一个进行后续的 extrude 计算
    std::vector<std::vector<SketchPointPtr>> pick_polygen_rings_topo;
    std::vector<SketchPointPtr> out_ring = GeoDataUtils::get().get_ring_topo(picked_geo, pick_polygon->ring);
    if(pick_polygon->inner_rings.size())
    {
        std::vector<std::vector<SketchPointPtr>> inner_rings;
        for(auto& inner_ring: pick_polygon->inner_rings)
        {
            auto& iv = inner_rings.emplace_back();
            GeoDataUtils::get().get_ring_topo(picked_geo, inner_ring, iv);
        }
        std::vector<std::vector<std::vector<SketchPointPtr>>> valid_polygons;
        geodata_topo::ring_difference_other_rings(picked_geo, out_ring, inner_rings, pick_polygon->ring->normal, valid_polygons);
        if(valid_polygons.size() == 1)
        {
            pick_polygen_rings_topo.insert(pick_polygen_rings_topo.end(), valid_polygons[0].begin(), valid_polygons[0].end());
        }
    }
    else
    {
        pick_polygen_rings_topo.emplace_back(std::move(out_ring));
    }

    m_selected_polygon_topo = pick_polygen_rings_topo;

    std::unordered_set<ObjectDoc*> hide_docs;

    prepare_add_mode_inferring_mesh();
    prepare_move_mode_inferring_mesh(picked_geo, pick_polygon);

    if(m_extrude_mode == ESketchExtrudeMode::MOVE)
    {
        if(m_add_inferring_mesh_doc && m_copyed_origin_mesh_doc)
        {
            hide_docs.insert(m_add_inferring_mesh_doc);
            hide_docs.insert(m_copyed_origin_mesh_doc);
        }
    }
    else
    {
        if(m_move_inferring_mesh_doc && m_pick_mesh_doc)
        {
            hide_docs.insert(m_move_inferring_mesh_doc);
            hide_docs.insert(m_pick_mesh_doc);
        }
    }
    hide_docs.insert(m_splited_old_mesh_doc);

    // 隐藏这个用于恢复变更的 polygon
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->setHideObjects(hide_docs);

    if(m_add_inferring_mesh_doc)
    {
        m_add_inferring_mesh_doc->dirty();
    }

    if(m_move_inferring_mesh_doc)
    {
        m_move_inferring_mesh_doc->dirty();
    }

    m_pick_mesh_doc->dirty();
}

void Controller_Extrude_ByNormal::create_move_operation_mesh(double offset)
{
    if(offset > m_positive_offset_limit)
    {
        offset = m_positive_offset_limit;
    }

    if(offset < -m_negative_offset_limit)
    {
        offset = -m_negative_offset_limit;
    }

    if(m_move_inferring_mesh_doc)
    {
        double delta_offset = offset - m_move_last_offset;
        m_move_last_offset = offset;
        nous::vec3 fitted_offset = delta_offset * m_pick_polygon_normal;

        for(const auto& pt: m_new_move_polygon_topo)
        {
            const nous::pos_t middle_pos = m_move_inferring_mesh_doc->geodata()->get_point_position(pt);
            const nous::pos_t new_pos = middle_pos + fitted_offset;
            m_move_inferring_mesh_doc->geodata()->set_point_position(pt, new_pos);
        }

        m_move_inferring_mesh_doc->geodata()->mark_all_sketch_polygon_dirty();
        m_move_inferring_mesh_doc->dirty();
    }
}

void Controller_Extrude_ByNormal::create_add_operation_mesh(double offset)
{
    if(m_add_inferring_mesh_doc)
    {
        double delta_offset = offset - m_add_last_offset;
        m_add_last_offset = offset;
        nous::vec3 fitted_offset = delta_offset * m_pick_polygon_normal;

        for(const auto& pt: m_new_add_polygon_topo)
        {
            const nous::pos_t middle_pos = m_add_inferring_mesh_doc->geodata()->get_point_position(pt);
            const nous::pos_t new_pos = middle_pos + fitted_offset;
            m_add_inferring_mesh_doc->geodata()->set_point_position(pt, new_pos);
        }

        m_add_inferring_mesh_doc->geodata()->mark_all_sketch_polygon_dirty();
        m_add_inferring_mesh_doc->dirty();
    }
}

void Controller_Extrude_ByNormal::commit_extrude_mesh()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    SceneDoc* scene = view->doc()->scene();
    view->clearHideObjects();

    if(m_pick_mesh_doc)
    {
        if(m_extrude_mode == ESketchExtrudeMode::ADD)
        {
            if(m_splited_old_mesh_doc)
            {
                m_pick_mesh_doc->merge(m_splited_old_mesh_doc);
                m_splited_old_mesh_doc = nullptr;
            }

            if(m_add_inferring_mesh_doc)
            {
                m_pick_mesh_doc->merge(m_add_inferring_mesh_doc);
                view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
                m_add_inferring_mesh_doc = nullptr;
            }

            if(m_move_inferring_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
                m_move_inferring_mesh_doc = nullptr;
            }
        }
        else if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            if(m_move_inferring_mesh_doc)
            {
                fix_inferring_mesh_doc();
                const bool is_success = m_pick_mesh_doc->merge(m_move_inferring_mesh_doc);
                view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
                m_move_inferring_mesh_doc = nullptr;

                if(!is_success)
                {
                    deactive();
                    std::cout << "This Operation Caused Topology Error, End Extrude Operation!" << std::endl;
                }
            }

            if(m_splited_old_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
                m_splited_old_mesh_doc = nullptr;
            }

            if(m_add_inferring_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
                m_add_inferring_mesh_doc = nullptr;
            }
        }

        if(m_copyed_origin_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
            m_copyed_origin_mesh_doc = nullptr;
        }
    }

    if(m_pick_nurbs_doc)
    {
        NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
        for(auto& item_id: m_tmp_surfaces_guid)
        {
            auto& tmp_nurbs_item = nurbs_data->get_item(item_id);
            if(tmp_nurbs_item->type() == EElementType::AlgebraSurface || tmp_nurbs_item->type() == EElementType::ClosedSurface)
            {
                nurbs_data->remove_item(item_id);
                if(tmp_nurbs_item->type() == EElementType::AlgebraSurface)
                {
                    tmp_nurbs_item->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::Solid);
                }
                NurbsTrimmedSurfacePtr complex_surface = std::make_shared<NurbsTrimmedSurface>();
                complex_surface->set_exterior_ring(tmp_nurbs_item->to_nurbs_surface_base());
                nurbs_data->add_item(complex_surface);
            }
        }
        m_pick_nurbs_doc->dirty();
    }

    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;

    clear_limits();
    m_new_move_polygon_topo.clear();
    m_new_add_polygon_topo.clear();
    m_tmp_surfaces_guid.clear();
}

void Controller_Extrude_ByNormal::switchMesh()
{
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        view->clearHideObjects();
        std::unordered_set<ObjectDoc*> hide_docs;

        if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            if(m_splited_old_mesh_doc && m_move_inferring_mesh_doc)
            {
                hide_docs.insert(m_move_inferring_mesh_doc);
                hide_docs.insert(m_splited_old_mesh_doc);
                hide_docs.insert(m_pick_mesh_doc);
            }
            create_add_operation_mesh(m_move_last_offset);
        }
        else
        {
            if(m_add_inferring_mesh_doc)
            {
                hide_docs.insert(m_add_inferring_mesh_doc);
                hide_docs.insert(m_splited_old_mesh_doc);
                hide_docs.insert(m_copyed_origin_mesh_doc);
            }
            create_move_operation_mesh(m_add_last_offset);
        }
        view->setHideObjects(hide_docs);
    }
}

void Controller_Extrude_ByNormal::prepare_add_mode_inferring_mesh()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    SceneDoc* scene = view->doc()->scene();
    if(m_add_inferring_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
        m_add_inferring_mesh_doc = nullptr;
    }
    m_add_inferring_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);

    if(m_copyed_origin_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
        m_copyed_origin_mesh_doc = nullptr;
    }
    m_copyed_origin_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    m_copyed_origin_mesh_doc->copy_from(m_pick_mesh_doc);

    double move_delta = 0.1;
    m_add_last_offset = move_delta;

    std::vector<std::vector<nous::pos_t>> new_polygon_pos;
    for(int i = 0; i < m_selected_polygon_topo.size(); ++i)
    {
        const std::vector<SketchPointPtr> ring = m_selected_polygon_topo[i];

        std::vector<nous::pos_t>& new_top_ring = new_polygon_pos.emplace_back();
        for(int j = 0; j < ring.size(); ++j)
        {
            SketchPointPtr curr_pt = ring[j];
            SketchPointPtr next_pt = ring[(j + 1) % ring.size()];
            const nous::pos_t curr_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(curr_pt);
            const nous::pos_t next_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(next_pt);
            const nous::pos_t new_curr_pt_pos = curr_pt_pos + m_add_last_offset * m_pick_polygon_normal;
            const nous::pos_t new_next_pt_pos = next_pt_pos + m_add_last_offset * m_pick_polygon_normal;

            std::vector<std::vector<nous::pos_t>> new_side_polygon = { { curr_pt_pos, next_pt_pos, new_next_pt_pos, new_curr_pt_pos } };
            m_add_inferring_mesh_doc->geodata()->add_polygon(new_side_polygon);

            new_top_ring.emplace_back(new_curr_pt_pos);
        }
    }

    m_new_add_polygon_topo_arr = m_add_inferring_mesh_doc->geodata()->add_polygon(new_polygon_pos);
    for(const auto& ring_pt_arr: m_new_add_polygon_topo_arr)
    {
        for(auto& pt: ring_pt_arr)
        {
            m_new_add_polygon_topo.insert(pt);
        }
    }

    m_add_inferring_mesh_doc->dirty();
    m_add_inferring_mesh_doc->geodata()->dirty();
}

void Controller_Extrude_ByNormal::prepare_move_mode_inferring_mesh(GeoData* picked_geo, SketchPolygonPtr pick_polygon)
{
    // 判断是否需要做一个法线反向
    // 即如果有和这个面相接的面，这些面在当前面上的投影一定不能存在，否则不能向这个方向进行推动
    // 算一个其余面这个面所在平面上的投影，若两个多边形相交，则不能往这个方向推动，若两个法线方向均不可以，则不能做任何事情，直接返回
    // 若任意一个可以则需要把法线设置到对应的方向上去
    m_valid_move_dir = need_flip_picked_normal(picked_geo, pick_polygon);

    if(m_valid_move_dir == ESketchExtrudeMoveValidDir::None)
    {
        // 前后都移不了别搞了
        commit_extrude_mesh();
        return;
    }

    double move_delta = 0.1;
    if(m_valid_move_dir == ESketchExtrudeMoveValidDir::Negative)
    {
        move_delta = -move_delta;
    }
    m_move_last_offset = move_delta;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    SceneDoc* scene = view->doc()->scene();
    if(m_move_inferring_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
        view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
        m_move_inferring_mesh_doc = nullptr;
        m_splited_old_mesh_doc = nullptr;
    }
    m_move_inferring_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    m_splited_old_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);

    std::unordered_map<SketchLinePtr, bool> line_corr_coplane_ring_map;

    // 基于选中的面，查找构成这个面的每条线段是否有与之共面的 polygon，若有则进行记录
    // 从处理逻辑上说可划分为以下三种
    // 1. 若一条边不存在与之共面的 ring，则需要创建一个新面
    // 2. 若一条边有与之共面的 ring，move 操作不需要创建一个新面，进入下面的分支逻辑
    //    2.1 若新的点的位置与之前共线，则直接更新点的位置即可
    //    2.2 若新的点的位置与之前不共线，则需要增加一条线段
    // 更大的问题是如何去处理 limit 的限制，目前按预置计算的逻辑处理

    std::unordered_set<SketchPointPtr> all_selected_point_arr;
    for(const auto& ring: m_selected_polygon_topo)
    {
        all_selected_point_arr.insert(ring.begin(), ring.end());
    }

    std::unordered_set<SketchRingPtr> self_rings;
    self_rings.insert(pick_polygon->ring);
    self_rings.insert(pick_polygon->inner_rings.begin(), pick_polygon->inner_rings.end());

    // 每条线对应可能存在的共面ring
    std::unordered_set<SketchPolygonPtr> need_delete_vertical_polygons;
    for(int i = 0; i < m_selected_polygon_topo.size(); ++i)
    {
        const auto& ring_pts = m_selected_polygon_topo[i];

        // 转一下 ring_pts 的顺序，一定要从非三点共线的位置开始做......
        std::vector<nous::pos_t> ring_pt_pos_arr;
        GeoDataUtils::get().get_ring_positions(picked_geo, ring_pts, ring_pt_pos_arr);
        int first_corner_idx = util::calc_ring_first_corner_index(ring_pt_pos_arr);

        std::vector<SketchPointPtr> fitted_ring_pts;
        if(first_corner_idx >= 0)
        {
            fitted_ring_pts.resize(ring_pts.size(), nullptr);
            for(int j = 0; j < ring_pts.size(); ++j)
            {
                fitted_ring_pts[j] = ring_pts[(j + first_corner_idx) % ring_pts.size()];
            }
        }
        else
        {
            fitted_ring_pts.insert(fitted_ring_pts.end(), ring_pts.begin(), ring_pts.end());
        }

        for(int j = 0; j < ring_pts.size(); ++j)
        {
            SketchPointPtr prev_point = ring_pts[j];
            SketchPointPtr next_point = ring_pts[(j + 1) % ring_pts.size()];

            SketchLinePtr line = picked_geo->get_line(GeoData::line_key(prev_point, next_point));
            bool has_coplane_polygon = false;

            // 改成是否只存在与其共面的 1-2 个 polygon
            auto adj_rings = picked_geo->get_line_adj_rings(line);
            auto valid_coplane_rings = get_valid_coplane_rings(adj_rings, self_rings, picked_geo);

            if(!valid_coplane_rings.empty())
            {
                has_coplane_polygon = true;

                // 先解决 Top 的那一圈 Point* 对应的是 inferring 中的哪些 Point*
                // 所以先把已有共享面的那部分加入进 inferring mesh
                for(SketchRingPtr coplane_ring: valid_coplane_rings)
                {
                    // line 作为 polygon 边界滑动，不可触碰到其余 polygon 或自己，即不能碰到任何其它环
                    SketchPolygonPtr inside_polygon = picked_geo->get_polygon(coplane_ring);
                    if(inside_polygon && need_delete_vertical_polygons.find(inside_polygon) == need_delete_vertical_polygons.end())
                    {
                        // 将临近 ring 影响到的全部 polygon 加入 old 用于恢复
                        std::vector<std::vector<nous::vec3>> polygon_positions;
                        GeoDataUtils::get().get_polygon_positions(picked_geo, inside_polygon, polygon_positions);
                        m_splited_old_mesh_doc->geodata()->add_polygon(polygon_positions);

                        // 计算可移动 limit 限制
                        std::optional<topo::SketchMoveLimitAloneDirection> limit = topo::calc_edge_move_distance_limit_alone_direction_in_polygon(
                            polygon_positions, picked_geo->get_point_position(prev_point), picked_geo->get_point_position(next_point),
                            m_pick_polygon_normal);
                        if(limit.has_value())
                        {
                            topo::SketchMoveLimitAloneDirection act_limit = limit.value();
                            if(act_limit.m_has_negative_limit)
                            {
                                m_negative_offset_limit = std::min(m_negative_offset_limit, act_limit.m_negative_distance_limit);
                            }

                            if(act_limit.m_has_positive_limit)
                            {
                                m_positive_offset_limit = std::min(m_positive_offset_limit, act_limit.m_positive_distance_limit);
                            }
                        }

                        std::vector<std::vector<nous::vec3>>&& offseted_polygon_positions =
                            get_offseted_polygon_positions(picked_geo, inside_polygon, prev_point, next_point, move_delta);
                        // m_move_inferring_mesh_doc->geometry()->add_polygon_without_check(offseted_polygon_positions, true);
                        GeoDataTopoOperations::get().add_polygon_without_split_line(m_move_inferring_mesh_doc->geodata().get(),
                                                                                    offseted_polygon_positions);
                        need_delete_vertical_polygons.insert(inside_polygon);
                    }

                    SketchPolygonPtr outside_polygon = picked_geo->get_parent_polygon(coplane_ring);
                    if(outside_polygon && need_delete_vertical_polygons.find(outside_polygon) == need_delete_vertical_polygons.end())
                    {
                        std::vector<std::vector<nous::vec3>> polygon_positions;
                        GeoDataUtils::get().get_polygon_positions(picked_geo, outside_polygon, polygon_positions);
                        m_splited_old_mesh_doc->geodata()->add_polygon(polygon_positions);

                        // 计算可移动 limit 限制
                        std::optional<topo::SketchMoveLimitAloneDirection> limit = topo::calc_edge_move_distance_limit_alone_direction_in_polygon(
                            polygon_positions, picked_geo->get_point_position(prev_point), picked_geo->get_point_position(next_point),
                            m_pick_polygon_normal);
                        if(limit.has_value())
                        {
                            topo::SketchMoveLimitAloneDirection act_limit = limit.value();
                            if(act_limit.m_has_negative_limit)
                            {
                                m_negative_offset_limit = std::min(m_negative_offset_limit, act_limit.m_negative_distance_limit);
                            }

                            if(act_limit.m_has_positive_limit)
                            {
                                m_positive_offset_limit = std::min(m_positive_offset_limit, act_limit.m_positive_distance_limit);
                            }
                        }

                        std::vector<std::vector<nous::vec3>>&& offseted_polygon_positions =
                            get_offseted_polygon_positions(picked_geo, outside_polygon, prev_point, next_point, move_delta);
                        // m_move_inferring_mesh_doc->geometry()->add_polygon_without_check(offseted_polygon_positions, true);
                        GeoDataTopoOperations::get().add_polygon_without_split_line(m_move_inferring_mesh_doc->geodata().get(),
                                                                                    offseted_polygon_positions);
                        need_delete_vertical_polygons.insert(outside_polygon);
                    }
                }
            }

            line_corr_coplane_ring_map.insert({ line, has_coplane_polygon });
        }
    }

    for(SketchPolygonPtr need_delete_skp: need_delete_vertical_polygons)
    {
        picked_geo->delete_polygon(need_delete_skp, true, false, &all_selected_point_arr);
    }

    // 也要把自己加到 old 里，恢复的时候直接用
    std::vector<std::vector<nous::vec3>> self_positions;
    GeoDataUtils::get().get_polygon_positions(picked_geo, m_selected_polygon_topo, self_positions);
    m_splited_old_mesh_doc->geodata()->add_polygon(self_positions);

    for(auto& pair: line_corr_coplane_ring_map)
    {
        SketchLinePtr line = pair.first;
        const bool& has_coplane_polygon = pair.second;

        // 已经找到垂直的 ring 的 line 直接 continue 就行
        if(has_coplane_polygon)
        {
            continue;
        }

        if(picked_geo->is_valid(line))
        {
            // 没有 prim 与之绑定的话，就要生成一个 ring 丢到 inferring 里去
            const nous::pos_t curr_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(line->p0);
            const nous::pos_t next_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(line->p1);
            const nous::pos_t new_curr_pt_pos = curr_pt_pos + m_move_last_offset * m_pick_polygon_normal;
            const nous::pos_t new_next_pt_pos = next_pt_pos + m_move_last_offset * m_pick_polygon_normal;

            std::vector<std::vector<nous::pos_t>> new_side_ring = { { curr_pt_pos, next_pt_pos, new_next_pt_pos, new_curr_pt_pos } };
            GeoDataTopoOperations::get().add_polygon_without_split_line(m_move_inferring_mesh_doc->geodata().get(), new_side_ring);
        }
        else
        {
            std::cout << "Line Key Not Found, Please Check!" << std::endl;
        }
    }

    for(int i = 0; i < self_positions.size(); ++i)
    {
        for(int j = 0; j < self_positions[i].size(); ++j)
        {
            self_positions[i][j] += m_move_last_offset * m_pick_polygon_normal;
        }
    }

    // 删掉现有的底
    picked_geo->delete_polygon(pick_polygon, true, false);
    m_pick_mesh_doc->dirty();

    // 替换为新的顶
    m_new_move_polygon_topo_arr = m_move_inferring_mesh_doc->geodata()->add_polygon(self_positions);
    for(const auto& ring_pt_arr: m_new_move_polygon_topo_arr)
    {
        for(auto& pt: ring_pt_arr)
        {
            m_new_move_polygon_topo.insert(pt);
        }
    }

    if(m_valid_move_dir == ESketchExtrudeMoveValidDir::None)
    {
        m_negative_offset_limit = 0.0;
        m_positive_offset_limit = 0.0;
    }
    else if(m_valid_move_dir == ESketchExtrudeMoveValidDir::Positive)
    {
        m_negative_offset_limit = 0.0;
    }
    else if(m_valid_move_dir == ESketchExtrudeMoveValidDir::Negative)
    {
        m_positive_offset_limit = 0.0;
    }
}

void Controller_Extrude_ByNormal::clear_all_cache_mesh_doc()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->clearHideObjects();
    clear_inferring_mesh_doc();
    SceneDoc* scene = view->doc()->scene();
    if(m_splited_old_mesh_doc)
    {
        m_pick_mesh_doc->merge(m_splited_old_mesh_doc);
        view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
        m_splited_old_mesh_doc = nullptr;
    }

    if(m_copyed_origin_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
        m_copyed_origin_mesh_doc = nullptr;
    }

    if(m_pick_nurbs_doc)
    {
        NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
        for(const auto& guid: m_tmp_surfaces_guid)
        {
            nurbs_data->remove_item(guid);
        }
        m_tmp_surfaces_guid.clear();
        m_pick_nurbs_doc->dirty();
        m_pick_nurbs_doc = nullptr;
    }
}

void Controller_Extrude_ByNormal::clear_inferring_mesh_doc()
{
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        SceneDoc* pScene = view->doc()->scene();
        if(m_move_inferring_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
            m_move_inferring_mesh_doc = nullptr;
        }

        if(m_add_inferring_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
            m_add_inferring_mesh_doc = nullptr;
        }
    }
}

void Controller_Extrude_ByNormal::clear_limits()
{
    m_negative_offset_limit = std::numeric_limits<double>::max();

    m_positive_offset_limit = std::numeric_limits<double>::max();

    m_valid_move_dir = ESketchExtrudeMoveValidDir::None;

    m_move_last_offset = 0.0;

    m_add_last_offset = 0.0;
}

void Controller_Extrude_ByNormal::fix_inferring_mesh_doc()
{
    if(m_move_inferring_mesh_doc)
    {
        GeoData* inferring_geodata = m_move_inferring_mesh_doc->geodata().get();
        std::unordered_set<SketchPolygonPtr> all_sketch_polygons = inferring_geodata->get_all_polygons();

        std::vector<std::vector<std::vector<nous::pos_t>>> splited_polygon_positions;
        for(SketchPolygonPtr polygon: all_sketch_polygons)
        {
            if(polygon->ring->key == GeoData::ring_key(m_new_move_polygon_topo_arr.front()))
            {
                continue;
            }

            std::vector<std::vector<std::vector<SketchPointPtr>>> splited_polygons;
            std::vector<std::vector<SketchPointPtr>> polygon_topo = GeoDataUtils::get().get_polygon_topo(inferring_geodata, polygon);
            std::vector<std::vector<SketchPointPtr>> polygon_inner_topo;
            polygon_inner_topo.insert(polygon_inner_topo.end(), polygon_topo.begin() + 1, polygon_topo.end());
            geodata_topo::ring_difference_other_rings(inferring_geodata, polygon_topo[0], polygon_inner_topo, polygon->ring->normal,
                                                      splited_polygons);
            if(splited_polygons.empty())
            {
                std::vector<std::vector<SketchPointPtr>>& points = splited_polygons.emplace_back();
                std::vector<SketchPointPtr>& exring_pts = points.emplace_back();
                GeoDataUtils::get().get_ring_topo(inferring_geodata, polygon->ring, exring_pts);
                for(const auto inner_ring: polygon->inner_rings)
                {
                    std::vector<SketchPointPtr>& inring_pts = points.emplace_back();
                    GeoDataUtils::get().get_ring_topo(inferring_geodata, inner_ring, inring_pts);
                }
            }

            for(int i = 0; i < splited_polygons.size(); ++i)
            {
                std::vector<std::vector<nous::pos_t>> part_positions;
                bool added_spec = false;

                // 找到和顶面连接的那条边
                for(int j = 0; j < splited_polygons[i].size(); ++j)
                {
                    bool do_extra_process = false;
                    std::vector<std::vector<nous::pos_t>> checked_ring_pos_arr;
                    std::vector<nous::pos_t> curr_ring_pos_arr;
                    GeoDataUtils::get().get_ring_positions(inferring_geodata, splited_polygons[i][j], curr_ring_pos_arr);
                    for(int k = 0; k < splited_polygons[i][j].size(); ++k)
                    {
                        SketchPointPtr prev_pt = splited_polygons[i][j][k];
                        SketchPointPtr next_pt = splited_polygons[i][j][(k + 1) % splited_polygons[i][j].size()];
                        SketchLinePtr line = inferring_geodata->get_line(GeoData::line_key(prev_pt, next_pt));

                        for(auto& top_ring: m_new_move_polygon_topo_arr)
                        {
                            const bool is_shared_line = GeoDataUtils::get().is_line_on_ring(inferring_geodata, line, top_ring);
                            if(is_shared_line)
                            {
                                checked_ring_pos_arr = topo::split_self_cross_ring(curr_ring_pos_arr, k);
                                do_extra_process = true;
                            }
                        }
                    }

                    // 外环被切开了，这个时候要去分内环
                    if(do_extra_process && j == 0 && checked_ring_pos_arr.size() > 1)
                    {
                        // 这个分支用于处理共享线在外环上且外环被分成多个的 case
                        for(int m = 0; m < checked_ring_pos_arr.size(); ++m)
                        {
                            std::vector<std::vector<nous::pos_t>> spec_part_positions;
                            spec_part_positions.emplace_back(std::move(checked_ring_pos_arr[m]));
                            geos::geom::Geometry* exring_polygon =
                                NousGeometryConverter::position_convert_to_geos_polygon(spec_part_positions.front(), polygon->ring->normal);

                            for(int n = 1; n < splited_polygons[i].size(); ++n)
                            {
                                std::map<geos::geom::Coordinate, SketchPointPtr> revert_map;
                                geos::geom::Geometry* inring_polygon = convert::convert_polygon_to_geometry_with_revert_map(
                                    inferring_geodata, splited_polygons[i][n], polygon->ring->normal, revert_map);

                                if(geos_util::is_contain(exring_polygon, inring_polygon))
                                {
                                    std::vector<nous::pos_t>& spec_ring_positions = spec_part_positions.emplace_back();
                                    GeoDataUtils::get().get_ring_positions(inferring_geodata, splited_polygons[i][n], spec_ring_positions);
                                }
                            }

                            splited_polygon_positions.emplace_back(std::move(spec_part_positions));
                        }
                        added_spec = true;
                        break;
                    }
                    else if(!do_extra_process)
                    {
                        // 这个分支用于处理共享线不在内环或者不在外环上的 case
                        std::vector<nous::pos_t>& ring_pos_arr = part_positions.emplace_back();
                        GeoDataUtils::get().get_ring_positions(inferring_geodata, splited_polygons[i][j], ring_pos_arr);
                    }
                    else
                    {
                        // 这个分支用于处理共享线在内外环上，且外环没有被切割，或内环被切割的 case
                        for(int k = 0; k < checked_ring_pos_arr.size(); ++k)
                        {
                            part_positions.emplace_back(std::move(checked_ring_pos_arr[k]));
                        }
                    }
                }

                if(!added_spec && !part_positions.empty())
                {
                    splited_polygon_positions.emplace_back(std::move(part_positions));
                }
            }

            inferring_geodata->delete_polygon(polygon, true, false);
        }

        for(auto& polygon_topo: splited_polygon_positions)
        {
            if(!polygon_topo.empty())
            {
                if(geos_util::is_polygon_valid(polygon_topo))
                {
                    GeoDataTopoOperations::get().add_polygon_without_split_line(inferring_geodata, polygon_topo);
                }
                else
                {
                    // 这里有可能 Geos 算成那个环被扣掉了，可以优化一下
                    sketch::topo::fix_one_point_repeated_polygon(polygon_topo);
                    if(geos_util::is_polygon_valid(polygon_topo))
                    {
                        GeoDataTopoOperations::get().add_polygon_without_split_line(inferring_geodata, polygon_topo);
                    }
                }
            }
        }
    }
}

bool Controller_Extrude_ByNormal::_process_select_info(const std::unordered_map<RenderObject*, SelectInfo>& select_info_map)
{
    int valid_element_number = 0;

    m_selected_algebra_surface = nullptr;
    m_selected_closed_surface = nullptr;
    m_selected_trimmed_surface = nullptr;

    for(auto& select_pair: select_info_map)
    {
        if(select_pair.first->type() == Define::ObjectType::OBJ_D5_MESH)
        {
            if(select_pair.second.elements.size() > 1 || valid_element_number > 0)
            {
                if(select_pair.second.elements.begin()->get()->to_polygon())
                {
                    valid_element_number += select_pair.second.elements.size();
                    break;
                }
            }

            for(auto& ele: select_pair.second.elements)
            {
                if(ele->to_polygon())
                {
                    m_pick_mesh_doc = dynamic_cast<MeshDoc*>(select_pair.first->doc());
                    m_pick_nurbs_doc = nullptr;
                    GeoData* geodata = m_pick_mesh_doc->geodata(true).get();
                    this->processPickPolygon(geodata, ele->to_polygon()->ring);
                    valid_element_number++;
                }
            }
        }

        if(select_pair.first->type() == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* tmp_nurbs_doc = dynamic_cast<NurbsDoc*>(select_pair.first->doc());
            NurbsDataPtr picked_nurbs_data = tmp_nurbs_doc->nurbsdata(true);
            const Engine::Mat43 world_mat = tmp_nurbs_doc->world_matrix();
            const nous::mat4 trans_mat = NousConvert::convert(world_mat);
            for(auto& ele: select_pair.second.elements)
            {
                if(ele->type() == EElementType::AlgebraSurface)
                {
                    m_pick_nurbs_doc = tmp_nurbs_doc;
                    m_pick_mesh_doc = nullptr;
                    m_selected_algebra_surface = ele->to_nurbs_algebra_surface();
                    valid_element_number++;
                }

                if(ele->type() == EElementType::ClosedSurface)
                {
                    m_pick_nurbs_doc = tmp_nurbs_doc;
                    m_pick_mesh_doc = nullptr;
                    m_selected_closed_surface = ele->to_nurbs_closed_surface();
                    valid_element_number++;
                }

                if(ele->type() == EElementType::TrimmedSurface)
                {
                    m_pick_nurbs_doc = tmp_nurbs_doc;
                    m_pick_mesh_doc = nullptr;
                    m_selected_trimmed_surface = ele->to_nurbs_trimmed_surface();
                    valid_element_number++;
                }
            }
        }
    }

    if(valid_element_number != 1)
    {
        m_selected_algebra_surface = nullptr;
        m_selected_closed_surface = nullptr;
        return false;
    }

    return true;
}

void Controller_Extrude_ByNormal::_create_nurbs_operation_mesh(const nous::scalar value)
{
    if(m_selected_algebra_surface)
    {
        _generate_algebra_surface_operation_mesh(value);
    }

    if(m_selected_closed_surface)
    {
        _generate_closed_surface_operation_mesh(value);
    }

    if(m_selected_trimmed_surface)
    {
        _generate_complex_surface_operation_mesh(value);
    }
}

void Controller_Extrude_ByNormal::_generate_algebra_surface_operation_mesh(const nous::scalar value)
{
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    if(fabs(value) < 1e-5)
    {
        return;
    }

    const std::vector<std::shared_ptr<INurbsBase>> new_surfaces = nurbs::extrude_surface_by_normal_keep_distance(m_selected_algebra_surface, value);
    for(int i = 0; i < new_surfaces.size(); ++i)
    {
        if(new_surfaces[i]->type() == EElementType::AlgebraSurface)
        {
            new_surfaces[i]->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame);
        }
        nurbs_data->add_item(new_surfaces[i]);
        m_tmp_surfaces_guid.insert(new_surfaces[i]->id());
    }

    m_pick_nurbs_doc->dirty();
}

void Controller_Extrude_ByNormal::_generate_closed_surface_operation_mesh(const nous::scalar value)
{
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    if(fabs(value) < 1e-5)
    {
        return;
    }

    const std::vector<std::shared_ptr<INurbsBase>> new_surfaces =
        nurbs::extrude_surface_by_normal(m_selected_closed_surface, value, m_pick_polygon_normal);

    for(int i = 0; i < new_surfaces.size(); ++i)
    {
        if(new_surfaces[i]->type() == EElementType::AlgebraSurface)
        {
            new_surfaces[i]->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame);
        }
        nurbs_data->add_item(new_surfaces[i]);
        m_tmp_surfaces_guid.insert(new_surfaces[i]->id());
    }

    m_pick_nurbs_doc->dirty();
}

void Controller_Extrude_ByNormal::_generate_complex_surface_operation_mesh(const nous::scalar value)
{
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    if(fabs(value) < 1e-5)
    {
        return;
    }

    std::vector<std::shared_ptr<INurbsBase>> new_surfaces = nurbs::extrude_surface_by_normal_keep_distance(m_selected_trimmed_surface, value);
    for(int i = 0; i < new_surfaces.size(); ++i)
    {
        if(new_surfaces[i]->type() == EElementType::AlgebraSurface)
        {
            new_surfaces[i]->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame);
        }
        nurbs_data->add_item(new_surfaces[i]);
        new_surfaces[i]->dirty();
        m_tmp_surfaces_guid.insert(new_surfaces[i]->id());
    }
    new_surfaces.clear();

    m_pick_nurbs_doc->dirty();
}

void Controller_Extrude_ByNormal::_rollback_to_pick_element()
{
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        clear_all_cache_mesh_doc();
    }
    m_pick_mesh_doc = nullptr;
    clear_limits();

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* inferHelper = view->inferHelper();
    inferHelper->set_infer_filter_type(InferType::ALL_TYPE);
    m_new_move_polygon_topo.clear();
    m_new_add_polygon_topo.clear();
    m_tmp_surfaces_guid.clear();
    view->inferHelper()->clear_ignore();

    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;
}

}// namespace sketch
