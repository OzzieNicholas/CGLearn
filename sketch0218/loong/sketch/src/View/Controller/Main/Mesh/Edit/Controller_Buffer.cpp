#include "sketch/View/Controller/Main/Mesh/Edit/Controller_Buffer.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/NousEngine/SketchMath.h"
#include "sketch/Util/Util.h"

namespace sketch
{
Controller_Buffer::Controller_Buffer(const std::wstring& name) : Controller_Main(name), m_buffer_step(BufferStep::PICK_ELEMENT) {}

Controller_Buffer::~Controller_Buffer() {}

SketchParamMap Controller_Buffer::info() const
{
    SketchParamMap para;
    para.set_value("current_param", "offset");
    para.set_value("name", m_name);
    para.set_value("value", m_current_value);
    return para;
}

void Controller_Buffer::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    this->set_tip(L"偏移");
    this->set_cursor(Define::CURSOR_CROSS);

    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* infer_helper = view->inferHelper();
    infer_helper->set_infer_filter_type(InferType::POLYGON);
    infer_helper->clear_refer();

    m_buffer_step = BufferStep::PICK_ELEMENT;
    auto& select_info = view->selector()->get_select_info();
    if(select_info.size() == 1 && select_info.begin()->second.elements.size() == 1)
    {
        EElementType type = select_info.begin()->second.elements.begin()->get()->type();
        if(type == EElementType::Polygon || type == EElementType::AlgebraSurface)
        {
            m_buffer_step = BufferStep::BUFFER_BEGIN_FIXING;
        }
    }

    if(m_buffer_step != BufferStep::BUFFER_BEGIN_FIXING)
    {
        view->selector()->clear();
    }
}

void Controller_Buffer::deactive()
{
    Controller_Main::deactive();
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* inferHelper = view->inferHelper();
    inferHelper->set_infer_filter_type(InferType::ALL_TYPE);
}

void Controller_Buffer::escape(const SketchParamMap& props)
{
    Controller_Main::escape(props);

    if(m_buffer_step > PICK_ELEMENT)
    {
        _roll_back_to_pick_element();
    }
    else
    {
        this->deactive();
    }
}

void Controller_Buffer::enter(const SketchParamMap& props)
{
    Controller_Main::enter(props);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(m_buffer_step == BufferStep::BUFFER_FIXING)
    {
        if(this->is_lock())
        {
            if(Util::is_valid_length_expression(m_current_value))
            {
                auto lenth = Util::extract_value_and_unit(m_current_value);
                topo::ESketchTopoRelation geo_topo =
                    topo::calc_point_and_ring_relation(sketch_math::ring_normal(m_org_ring), m_hover_pos, m_org_ring);
                if(geo_topo == topo::ESketchTopoRelation::Inside)
                {
                    m_buffer_offset = -lenth.first;
                }
                else if(geo_topo == topo::ESketchTopoRelation::OnBoundary)
                {
                    m_buffer_offset = 0.0;
                }
                else
                {
                    m_buffer_offset = lenth.first;
                }
                // TODO
            }
            else
            {
                _unlock_mouse_move();
                return;
            }
        }
        this->_gen_offset(m_buffer_offset);
        this->_gen_offset_mesh(view);
        _unlock_mouse_move();
        m_buffer_step = BufferStep::PICK_ELEMENT;
    }
    else
    {
        m_current_value.clear();
    }
}

void Controller_Buffer::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Buffer::key_up(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Buffer::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    if(button != 0)
        return;

    View* view = STCore::inst->viewMgr()->layout()->focusedView();

    auto& infer_info = view->inferHelper()->get_infer_result();
    auto& select_info = view->selector()->get_select_info();
    if(m_buffer_step == BufferStep::PICK_ELEMENT)
    {
        if(select_info.size() == 1 && select_info.begin()->second.elements.size() == 1)
        {
            EElementType type = select_info.begin()->second.elements.begin()->get()->type();
            if(type == EElementType::Polygon || type == EElementType::AlgebraSurface)
            {
                m_buffer_step = BufferStep::BUFFER_BEGIN_FIXING;
            }
        }
    }

    if(m_buffer_step == BufferStep::BUFFER_BEGIN_FIXING)
    {
        SketchPolygonPtr polygon = infer_info.element->to_polygon();
        m_begin_pos = infer_info.position;
        m_end_pos = static_cast<Mesh*>(infer_info.mesh)->get_world_position_closest_with_ring_edge(polygon->ring, m_begin_pos);
        m_buffer_normal = infer_info.normal;
        m_org_ring.clear();
        static_cast<Mesh*>(infer_info.mesh)->get_ring_world_position(polygon->ring, m_org_ring);

        m_buffer_step = BufferStep::BUFFER_FIXING;
    }
    else if(m_buffer_step == BufferStep::BUFFER_FIXING)
    {
        this->_gen_offset_mesh(view);
        _unlock_mouse_move();
        m_buffer_step = BufferStep::PICK_ELEMENT;
    }
}

void Controller_Buffer::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Controller_Main::mouse_move(button, x, y, ctrl, alt, shift);
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    InferenceHelper* infer_helper = view->inferHelper();
    auto& infer_info = infer_helper->get_infer_result();

    ViewCamera* cam = view->camera();
    Document* doc = STCore::inst->docMgr()->document();
    auto& vp = cam->viewport();
    Engine::Vec3 cam_pos, pick_dir;
    cam->getDocPickRay(doc, (float)x + vp.x, (float)y + vp.y, &cam_pos, &pick_dir);
    if(infer_info.infer_type != InferType::NONE)
    {
        m_hover_pos = infer_info.position;
    }
    else
    {
        // 应该是 cam_pos 和 pick_dir 和选择面的交点
        if(m_buffer_step > PICK_ELEMENT)
        {
            m_hover_pos = sketch_math::line_intersect_plane(NousConvert::convert(cam_pos), NousConvert::convert(pick_dir), m_org_ring.front(),
                                                            sketch_math::ring_normal(m_org_ring));
        }
        else
        {
            m_hover_pos = NousConvert::convert(cam_pos);
        }
    }

    if(m_buffer_step == BufferStep::PICK_ELEMENT)
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
                    std::unordered_set<SketchElementPtr> elements;
                    if(infer_info.infer_type & InferType::POLYGON)
                    {
                        if(!infer_info.element->group())
                        {
                            elements.insert(infer_info.element);
                        }
                    }
                    view->selector()->add_select_element(infer_info.mesh, elements);
                }
            }
        }
    }
    else if(m_buffer_step == BufferStep::BUFFER_BEGIN_FIXING)
    {
        _unlock_mouse_move();
        infer_helper->update_inference(view, x, y);
    }
    else if(m_buffer_step == BufferStep::BUFFER_FIXING)
    {
        if(!this->is_lock())
        {
            infer_helper->update_inference(view, x, y);
            nous::pos_t line1_p;
            sketch_math::line_intersect(NousConvert::convert(cam_pos), NousConvert::convert(cam_pos + pick_dir), m_begin_pos, m_end_pos, line1_p,
                                        m_ctrl_pos);

            nous::vec3 cyrl_dir = m_ctrl_pos - m_end_pos;
            nous::vec3 buffer_dir = m_begin_pos - m_end_pos;
            m_buffer_offset = -cyrl_dir.normalized().dot(buffer_dir.normalized()) * cyrl_dir.norm();
            m_current_value = Util::cut_string_with_two_decimals(m_buffer_offset);
        }
        _gen_offset(m_buffer_offset);
    }
    view->dirtyRT();
}

void Controller_Buffer::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Buffer::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);

    if(m_buffer_step == BufferStep::BUFFER_FIXING)
    {
        view->batchRenderLine(1, 1.0f, Engine::Vec3f(1.0f, 0.4f, 0.7f), NousConvert::convert(m_begin_pos), NousConvert::convert(m_end_pos), 0xffffu,
                              1, false, false);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_end_pos), NousConvert::convert(m_ctrl_pos), 0xffffu, 1, false,
                              false);
        view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_ctrl_pos), NousConvert::convert(m_hover_pos), 0x000fu, 1, false,
                              false);

        for(int i = 0; i < m_buffer_ring.size(); ++i)
        {
            view->batchRenderLine(1, 1.0f, Engine::Vec3f::WHITE, NousConvert::convert(m_buffer_ring[i]),
                                  NousConvert::convert(m_buffer_ring[(i + 1) % m_buffer_ring.size()]), 0xffffu, 1, false, true);
        }
    }
}

void Controller_Buffer::_gen_offset_mesh(View* view)
{
    SceneDoc* scene = view->doc()->scene();
    RvtBlock blk(scene->rvtId());
    MeshDoc* buffer_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    auto buffer_geo = buffer_mesh_doc->geodata();
    for(int i = 0; i < m_buffer_ring.size(); ++i)
    {
        buffer_geo->add_line(m_buffer_ring[i], m_buffer_ring[(i + 1) % m_buffer_ring.size()]);
    }
    buffer_mesh_doc->record();

    // merge
    _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>{ buffer_mesh_doc }, std::unordered_set<MeshDoc*>{}, view->doc()->scene());
}

void Controller_Buffer::_gen_offset(float offset)
{
    m_buffer_ring.clear();
    std::vector<std::vector<std::vector<nous::vec3>>> result;
    geos_util::buffer_ring(m_org_ring, m_buffer_normal, offset, result);
    for(auto& polygon: result)
    {
        for(auto& ring: polygon)
        {
            m_buffer_ring.insert(m_buffer_ring.end(), ring.begin(), ring.end());
        }
    }
}

void Controller_Buffer::_roll_back_to_pick_element()
{
    if(m_buffer_step == BUFFER_FIXING || m_buffer_step == BUFFER_BEGIN_FIXING)
    {
        m_buffer_ring.clear();
        m_buffer_offset = 0.0;
        m_org_ring.clear();
        m_buffer_ring.clear();

        m_buffer_step = PICK_ELEMENT;
        set_cursor(Define::MouseCursor::CURSOR_CROSS);
    }
}
}// namespace sketch
