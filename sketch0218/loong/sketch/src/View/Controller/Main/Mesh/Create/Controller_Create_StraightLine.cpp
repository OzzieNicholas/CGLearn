#include "sketch/View/Controller/Main/Mesh/Create/Controller_Create_StraightLine.h"

#include "sketch/STCore.h"

#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

#include <functional>

namespace sketch
{
Controller_Create_StraightLine::Controller_Create_StraightLine(const std::wstring& name)
    : Controller_Main(name), m_straight_line_step(StraightLineStep::PENDING), m_need_update_reference(false)
{
}

Controller_Create_StraightLine::~Controller_Create_StraightLine() {}

SketchParamMap Controller_Create_StraightLine::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "length");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Create_StraightLine::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    set_tip(L"直线");
    set_cursor(Define::CURSOR_ARROW);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer = view->inferHelper();
    infer->set_infer_filter_type(InferType::ALL_TYPE);
    m_straight_line_step = StraightLineStep::PENDING;
}

void Controller_Create_StraightLine::deactive() { Controller_Main::deactive(); }

void Controller_Create_StraightLine::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_straight_line_step == FIXING)
    {
        _roll_back_to_pending();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Create_StraightLine::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_straight_line_step == StraightLineStep::FIXING)
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
        _gen_straight_line(view);

        _unlock_mouse_move();
        m_begin_pos = m_hover_pos;
        m_straight_line_step = StraightLineStep::FIXING;
        m_need_update_reference = true;
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Create_StraightLine::key_down(int key, bool ctrl, bool alt, bool shift)
{
    if(shift)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        InferenceHelper* infer_helper = view->inferHelper();
        const InferResult& infer = infer_helper->get_infer_result();
        if(infer.infer_type & InferType::POINT || infer.infer_type & InferType::CENTER || infer.infer_type & InferType::VPC_TYPE ||
           infer.infer_type & InferType::AXIS_TYPE)
        {
            ReferAttrib ra(m_begin_pos, infer.position, InferType::COLLINEAR_LINE);
            InferType it = infer.infer_type;
            if(infer.infer_type & InferType::CENTER)
            {
                it = InferType::POINT;
            }
            infer_helper->force_reference(ra, it);
        }
    }
}

void Controller_Create_StraightLine::key_up(int key, bool ctrl, bool alt, bool shift)
{
    if(!shift)
    {
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        InferenceHelper* infer_helper = view->inferHelper();
        infer_helper->clear_force_reference();
    }
}

void Controller_Create_StraightLine::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    if(m_straight_line_step == StraightLineStep::FIXING)
    {
        const bool is_gen_new_polygon = _gen_straight_line(view);
        if(is_gen_new_polygon)
        {
            _roll_back_to_pending();
            return;
        }
    }

    _unlock_mouse_move();
    m_begin_pos = m_hover_pos;
    m_straight_line_step = StraightLineStep::FIXING;
    m_need_update_reference = true;
}

void Controller_Create_StraightLine::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(view == nullptr)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    const InferResult& infer = infer_helper->get_infer_result();
    if(m_straight_line_step == StraightLineStep::FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y, &m_begin_pos);
            m_hover_pos = infer.position;
            float lenth = (m_begin_pos - m_hover_pos).norm();
            m_current_value = Util::cut_string_with_two_decimals(lenth);
        }
    }
    else
    {
        _unlock_mouse_move();

        infer_helper->update_inference(view, x, y);
        m_hover_pos = infer.position;
    }

    // 将当前鼠标infer的结果添加到参考中
    if(infer.infer_type != InferType::NONE)
    {
        m_hover_pos = infer.position;
        if(m_need_update_reference)
        {
            m_need_update_reference = false;
            _add_hovered_reference(infer_helper, infer);
        }
    }

    view->dirtyRT();
}

void Controller_Create_StraightLine::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Create_StraightLine::mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

bool Controller_Create_StraightLine::_gen_straight_line(View* view)
{
    if(sketch_math::point_equal(m_begin_pos, m_hover_pos))
        return false;

    bool stop_draw = false;

    SceneDoc* scene = view->doc()->scene();
    RvtBlock rvt_block(scene->rvtId());

    auto& vist_objs = view->visObjects();
    std::vector<RenderObject*> intersect_objs;
    Engine::OBB line_obb;
    line_obb.add(NousConvert::convert(m_begin_pos));
    line_obb.add(NousConvert::convert(m_hover_pos));
    line_obb.set(line_obb.m_min - Engine::Vec3(0.1), line_obb.m_max + Engine::Vec3(0.1), line_obb.m_matrix);

    TFObjectDoc* group_obj = view->working_group_area();
    for(auto obj: vist_objs)
    {
        Engine::OBB obj_obb = obj->doc()->bbox();
        if(obj->doc()->parent() == group_obj && obj->doc()->type() == Define::OBJ_D5_MESH && obj_obb.intersect(line_obb))
        {
            intersect_objs.push_back(obj);
        }
    }

    if(intersect_objs.size())
    {
        MeshDoc* base_doc = static_cast<MeshDoc*>(intersect_objs[0]->doc());
        base_doc->record();
        for(int i = 1; i < intersect_objs.size(); ++i)
        {
            MeshDoc* cd = (MeshDoc*)intersect_objs[i]->doc();

            cd->record();
            base_doc->merge_without_check(cd);
            view->delete_obj_and_try_delete_relation(cd);
        }
        const auto& imat = base_doc->world_matrix_inv();
        const unsigned int base_doc_prev_polygon_number = base_doc->geodata()->get_all_polygons().size();
        std::vector<SketchLinePtr> new_lines =
            base_doc->geodata()->add_line(NousConvert::product_v3(imat, m_begin_pos), NousConvert::product_v3(imat, m_hover_pos), true);
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(base_doc->geodata().get());
        GeoDataTopoOperations::get().check_and_recreate_smooth_group(base_doc->geodata().get());
        const unsigned int base_doc_next_polygon_number = base_doc->geodata()->get_all_polygons().size();
        std::unordered_set<SketchPointPtr> all_attached_pt_set;
        bool all_point_degree_larger_than_two = true;
        bool all_line_has_ring = true;

        for(auto& line: new_lines)
        {
            all_attached_pt_set.insert(line->p0);
            all_attached_pt_set.insert(line->p1);
            if(base_doc->geodata()->get_line_adj_rings(line).empty())
            {
                all_line_has_ring = false;
            }
        }

        for(auto& pt: all_attached_pt_set)
        {
            auto&& neigh_pts = base_doc->geodata()->get_mesh()->point_neighbours_range(pt->point->idx()) | nous::ranges::ToArray();
            if(neigh_pts.size() < 2)
            {
                all_point_degree_larger_than_two = false;
                break;
            }
        }

        stop_draw = base_doc_next_polygon_number > base_doc_prev_polygon_number && all_point_degree_larger_than_two && all_line_has_ring;

        Engine::OBB base_obb = base_doc->bbox();
        // new obj merge
        intersect_objs.clear();
        for(auto obj: vist_objs)
        {
            Engine::OBB obj_obb = obj->doc()->bbox();
            if(obj->doc()->parent() == group_obj && obj->doc() != base_doc && obj->doc()->type() == Define::OBJ_D5_MESH &&
               obj_obb.intersect(base_obb))
            {
                intersect_objs.push_back(obj);
            }
        }
        for(int i = 0; i < intersect_objs.size(); ++i)
        {
            MeshDoc* cd = (MeshDoc*)intersect_objs[i]->doc();
            cd->record();
            base_doc->merge(cd);
            view->delete_obj_and_try_delete_relation(cd);
        }
        base_doc->dirty();
    }
    else
    {
        MeshDoc* new_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
        auto new_geo = new_mesh_doc->geodata(true);
        new_geo->add_line(m_begin_pos, m_hover_pos, true);
        new_mesh_doc->dirty();
    }

    return stop_draw;
}

void Controller_Create_StraightLine::_add_hovered_reference(InferenceHelper* infer_helper, const InferResult& infer)
{
    InferResult pick_infer = infer;

    if(pick_infer.infer_type & InferType::POINT)
    {
        MeshDoc* mesh_doc = static_cast<MeshDoc*>(pick_infer.mesh->doc());
        auto geo_data = mesh_doc->geodata();
        std::unordered_set<SketchLinePtr> adj_lines;
        std::unordered_set<SketchRingPtr> adj_rings;

        geo_data->get_point_adj_rings(pick_infer.element->to_point(), adj_rings);
        geo_data->get_point_adj_lines(pick_infer.element->to_point(), adj_lines);

        for(SketchRingPtr ring: adj_rings)
        {
            SketchPolygonPtr skp = geo_data->get_polygon(ring);
            if(skp)
            {
                pick_infer.infer_type = InferType::POLYGON;
                pick_infer.element = skp;
                pick_infer.normal = skp->ring->normal;
                infer_helper->attach_reference(pick_infer);
                break;
            }
        }
        if(adj_lines.size())
        {
            pick_infer.infer_type = InferType::LINE;
            pick_infer.element = *adj_lines.begin();
            infer_helper->attach_reference(pick_infer);
        }
    }
    else if(pick_infer.infer_type & InferType::POLYGON)
    {
        infer_helper->attach_reference(pick_infer);
    }
    else if(pick_infer.infer_type & InferType::LINE)
    {
        infer_helper->attach_reference(pick_infer);
    }
}

void Controller_Create_StraightLine::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    Engine::Vec3 begin = NousConvert::convert(m_begin_pos);
    Engine::Vec3 end = NousConvert::convert(m_hover_pos);

    if(m_straight_line_step == StraightLineStep::FIXING)
    {
        view->batchRenderLine(2.f, 1.f, Engine::Vec3f::BLACK, begin, end, 0xffff, 1, false, true);
    }
}

void Controller_Create_StraightLine::_roll_back_to_pending()
{
    if(m_straight_line_step == FIXING)
    {
        m_begin_pos = nous::pos_t::Zero();
        m_straight_line_step = PENDING;
    }
}

}// namespace sketch
