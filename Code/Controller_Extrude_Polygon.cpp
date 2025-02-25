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

// 函数功能：返回当前挤出控制器的参数信息，包括当前参数名称、控制器名称和当前值
SketchParamMap Controller_Extrude_ByNormal::info() const
{
    // 创建一个参数映射对象
    SketchParamMap para;
    // 设置当前参数名称为 "distance"
    para.set_value("current_param", "distance");
    // 设置控制器名称为 m_name
    para.set_value("name", m_name);
    // 设置当前值为 m_current_value
    para.set_value("value", m_current_value);
    // 返回参数映射对象
    return para;
}

// 函数功能：激活挤出控制器，设置提示信息和光标样式，初始化推断助手，检查选择信息并设置挤出步骤，初始化NousEngine
void Controller_Extrude_ByNormal::active(const SketchParamMap& props)
{
    // 调用基类的active函数
    Controller_Edit::active(props);

    // 设置提示信息为 "select one face to extrude."
    this->set_tip(L"select one face to extrude.");
    // 设置光标样式为十字光标
    this->set_cursor(Define::CURSOR_CROSS);

    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    // 获取推断助手
    InferenceHelper* infer_helper = view->inferHelper();
    // 设置推断过滤类型为多边形
    infer_helper->set_infer_filter_type(InferType::POLYGON);
    // 清除推断助手的参考信息
    infer_helper->clear_refer();

    // 设置挤出步骤为选择元素
    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;
    // 获取选择信息
    auto& select_info = view->selector()->get_select_info();
    // 如果选择信息中只有一个元素且该元素是多边形或代数曲面
    if(select_info.size() == 1 && select_info.begin()->second.elements.size() == 1)
    {
        EElementType type = select_info.begin()->second.elements.begin()->get()->type();
        if(type == EElementType::Polygon || type == EElementType::AlgebraSurface)
        {
            // 设置挤出步骤为开始固定挤出
            m_extrude_step = ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING;
        }
    }

    // 如果挤出步骤不是开始固定挤出
    if(m_extrude_step != ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        // 清除选择信息
        view->selector()->clear();
    }

    // 如果NousEngine没有初始化
    if(!NousEngine::is_inited())
    {
        // 初始化NousEngine
        NousEngine::instance()->init();
    }
}

void Controller_Extrude_ByNormal::deactive() { Controller_Edit::deactive(); }

// 函数功能：处理挤出操作的取消或回退，根据当前的挤出步骤执行相应的操作
void Controller_Extrude_ByNormal::escape(const SketchParamMap& props)
{
    // 调用基类的escape函数
    Controller_Main::escape(props);
    
    // 如果当前的挤出步骤是EXTRUDE_FIXING
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 回滚到选择元素的状态
        _rollback_to_pick_element();
    }
    else
    {
        // 否则，停用当前控制器
        this->deactive();
    }
}

// 函数功能：处理挤出操作的确认阶段，根据当前的挤出模式和输入值生成相应的操作网格，并提交挤出结果
void Controller_Extrude_ByNormal::enter(const SketchParamMap& props)
{
    // 调用基类的enter函数
    Controller_Main::enter(props);

    // 如果当前的挤出步骤是EXTRUDE_FIXING
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 如果当前操作被锁定
        if(this->is_lock())
        {
            // 如果当前值是有效的长度表达式
            if(Util::is_valid_length_expression(m_current_value))
            {
                // 提取值和单位
                auto trans = Util::extract_value_and_unit(m_current_value);
                float t1 = trans.first;

                // 判断方向
                const nous::vec3 mouse_dir = (m_hover_pos - m_extrude_polygon_begin_position).normalized();
                const int sign = mouse_dir.dot(m_pick_polygon_normal) > 0 ? 1 : -1;

                // 根据挤出模式生成相应的操作网格
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
                // 如果当前值无效，解锁鼠标移动并返回
                _unlock_mouse_move();
                return;
            }
        }

        // 提交挤出网格
        commit_extrude_mesh();

        // 获取当前聚焦的视图
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        // 清除推断助手的忽略列表
        view->inferHelper()->clear_ignore();
        // 设置推断过滤类型为多边形
        view->inferHelper()->set_infer_filter_type(InferType::POLYGON);
        // 解锁鼠标移动
        _unlock_mouse_move();
        // 清除受影响的网格
        _clear_affected_meshes();
    }
    else
    {
        // 如果当前的挤出步骤不是EXTRUDE_FIXING，清除当前值
        m_current_value.clear();
    }
}

// 函数功能：处理键盘按下事件，切换挤出模式（移动/添加），并更新视图
void Controller_Extrude_ByNormal::key_down(int key, bool ctrl, bool alt, bool shift)
{
    // 如果按下的是Shift键
    if(shift)
    {
        // 切换当前的挤出网格
        this->switchMesh();
        // 如果当前的挤出模式是移动模式
        if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            // 切换为添加模式
            m_extrude_mode = ESketchExtrudeMode::ADD;
        }
        // 如果当前的挤出模式是添加模式
        else if(m_extrude_mode == ESketchExtrudeMode::ADD)
        {
            // 切换为移动模式
            m_extrude_mode = ESketchExtrudeMode::MOVE;
        }
        // 获取当前聚焦的视图
        View* pView = STCore::inst->viewMgr()->layout()->focusedView();
        // 标记视图需要重新渲染
        pView->dirtyRT();
    }
}

// 函数功能：处理鼠标按下事件，根据当前的挤出步骤执行相应的操作，包括选择元素、开始固定挤出和提交挤出网格
void Controller_Extrude_ByNormal::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    // 如果按下的不是左键，直接返回
    if(button != 0)
        return;

    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    // 获取推断助手的推断结果
    auto& infer_info = view->inferHelper()->get_infer_result();
    // 获取选择信息
    auto select_info = view->selector()->get_select_info();
    
    // 如果当前的挤出步骤是选择元素
    if(m_extrude_step == ESketchExtrudeStep::PICK_ELEMENT)
    {
        // 如果选择信息中只有一个元素
        if(select_info.size() == 1)
        {
            // 设置挤出步骤为开始固定挤出
            m_extrude_step = ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING;
        }
    }

    // 如果当前的挤出步骤是开始固定挤出
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        // 创建一个可撤销的块
        RvtBlock blk(view->doc()->scene()->rvtId());
        // 处理选择信息，如果处理失败，重置挤出步骤为选择元素并返回
        if(!_process_select_info(select_info))
        {
            m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;
            return;
        }

        // 如果存在移动推断网格文档，将其添加到推断助手的忽略列表并标记为脏数据
        if(m_move_inferring_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_move_inferring_mesh_doc->renderObject());
            m_move_inferring_mesh_doc->dirty();
        }
        // 如果存在添加推断网格文档，将其添加到推断助手的忽略列表并标记为脏数据
        if(m_add_inferring_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_add_inferring_mesh_doc->renderObject());
            m_add_inferring_mesh_doc->dirty();
        }
        // 如果存在分割的旧网格文档，将其添加到推断助手的忽略列表并标记为脏数据
        if(m_splited_old_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_splited_old_mesh_doc->renderObject());
            m_splited_old_mesh_doc->dirty();
        }
        // 如果存在复制的原始网格文档，将其添加到推断助手的忽略列表并标记为脏数据
        if(m_copyed_origin_mesh_doc)
        {
            view->inferHelper()->add_ignore(m_copyed_origin_mesh_doc->renderObject());
            m_copyed_origin_mesh_doc->dirty();
        }

        // 设置挤出步骤为固定挤出
        m_extrude_step = ESketchExtrudeStep::EXTRUDE_FIXING;
        // 如果选择的是网格文档，标记为脏数据并设置挤出类型为非NURBS
        if(m_pick_mesh_doc)
        {
            m_pick_mesh_doc->dirty();
            m_is_extrude_nurbs = false;
        }
        // 如果选择的是NURBS文档，获取推断结果的位置和法线，标记为脏数据并设置挤出类型为NURBS
        if(m_pick_nurbs_doc)
        {
            m_extrude_polygon_begin_position = view->inferHelper()->get_infer_result().position;
            m_pick_polygon_normal = infer_info.normal;
            m_pick_nurbs_doc->dirty();
            m_is_extrude_nurbs = true;
        }
    }
    // 如果当前的挤出步骤是固定挤出
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 创建一个可撤销的块，并提交挤出网格
        RvtBlock blk(view->doc()->scene()->rvtId(), true);
        commit_extrude_mesh();
        // 清除推断助手的忽略列表，并设置推断过滤类型为多边形
        view->inferHelper()->clear_ignore();
        view->inferHelper()->set_infer_filter_type(InferType::POLYGON);
    }

    return;
}

// 函数功能：处理鼠标移动事件，根据当前的挤出步骤执行相应的操作，包括选择元素、开始固定挤出和更新挤出网格
void Controller_Extrude_ByNormal::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    // 调用基类的鼠标移动处理函数
    Controller_Edit::mouse_move(button, x, y, ctrl, alt, shift);
    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    if(!view)
        return;

    // 获取推断助手
    InferenceHelper* infer_helper = view->inferHelper();
    // 获取推断结果
    auto& infer_info = infer_helper->get_infer_result();

    // 如果当前的挤出步骤是选择元素
    if(m_extrude_step == ESketchExtrudeStep::PICK_ELEMENT)
    {
        // 解锁鼠标移动
        _unlock_mouse_move();
        // 更新推断信息
        infer_helper->update_inference(view, x, y);
        // 清除选择信息
        view->selector()->clear();
        // 如果推断结果是元素类型
        if(infer_info.infer_type & InferType::ELEMENT_TYPE)
        {
            // 如果推断结果中有网格
            if(infer_info.mesh)
            {
                // 获取工作组中的最高父对象
                TFObjectDoc* group_doc = view->get_obj_highest_parent_in_working_group_area(infer_info.mesh->doc());
                if(group_doc && group_doc->_group())
                {
                }
                else
                {
                    // 创建一个元素集合
                    std::unordered_set<SketchElementPtr> element;
                    // 如果推断结果是多边形或曲面
                    if(infer_info.infer_type & InferType::POLYGON || infer_info.infer_type & InferType::SURFACE)
                    {
                        // 将推断结果的元素添加到集合中
                        element.insert(infer_info.element);
                    }
                    // 添加选择的元素到选择器中
                    view->selector()->add_select_element(infer_info.mesh, element);
                }
            }
        }
    }
    // 如果当前的挤出步骤是开始固定挤出
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_BEGIN_FIXING)
    {
        // 解锁鼠标移动
        _unlock_mouse_move();
        // 更新推断信息
        infer_helper->update_inference(view, x, y);
    }
    // 如果当前的挤出步骤是固定挤出
    else if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 如果当前操作未锁定
        if(!this->is_lock())
        {
            // 更新推断信息
            infer_helper->update_inference(view, x, y);
            // 获取当前视图的相机
            ViewCamera* curr_camera = view->camera();
            // 获取全局文档
            Document* global_doc = STCore::inst->docMgr()->document();
            // 获取视口
            auto& view_port = curr_camera->viewport();
            Engine::Vec3 view_ori, view_dir;
            // 获取相机的拾取射线
            curr_camera->getDocPickRay(global_doc, (float)x + view_port.x, (float)y + view_port.y, &view_ori, &view_dir);

            // 通过平面法线方向和相机视角方向的交点得出 offset 值
            double t0, t1;
            rayIntersect(view_ori, view_dir, NousConvert::convert(m_extrude_polygon_begin_position), NousConvert::convert(m_pick_polygon_normal), t0, t1);
            if(t0 < 0)
            {
                return;
            }
            // 计算偏移量
            nous::vec3 offset = m_pick_polygon_normal * t1;
            // 更新当前值
            m_current_value = Util::cut_string_with_two_decimals(t1);

            // 获取推断结果的类型
            InferType infer_type = infer_helper->get_infer_result().infer_type;
            bool replace_ori_offset = false;
            nous::pos_t infer_position;
            // 如果推断结果是元素类型
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

            // 如果需要替换原始偏移量
            if(replace_ori_offset)
            {
                const nous::pos_t proj_pos = sketch_math::point_to_line_closest(infer_position, m_extrude_polygon_begin_position, m_extrude_polygon_begin_position + m_pick_polygon_normal, false);
                t1 = (proj_pos - m_extrude_polygon_begin_position).norm() * (proj_pos - m_extrude_polygon_begin_position).normalized().dot(m_pick_polygon_normal);
            }

            // 根据挤出模式生成相应的操作网格
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

    // 标记视图需要重新渲染
    view->dirtyRT();
    return;
}

void Controller_Extrude_ByNormal::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

void Controller_Extrude_ByNormal::prepare_render(View* view)
{
    this->_update_helper_mesh(view->inferHelper());
    this->_render_helper_mesh(view);
}

// 函数功能：获取与给定环共面的有效环集合。
// 该函数首先检查相邻环的数量是否为2或3，如果不是则返回空集合。
// 然后遍历相邻环，计算其法线与选中多边形法线的点积，如果点积接近0则认为共面，加入结果集合。
// 如果有任何一个环不共面，则清空结果集合并返回。
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

    // 遍历相邻环，检查其法线与选中多边形法线的点积
    for(SketchRingPtr adj_ring: line_adj_rings)
    {
        if(self_rings.find(adj_ring) != self_rings.end())
        {
            continue;
        }

        nous::vec3 adj_ring_normal = adj_ring->normal;
        double dot_value = adj_ring_normal.dot(m_pick_polygon_normal);

        // 如果点积不接近0，则不共面，清空结果集合并返回
        if(!(dot_value > -1e-5 && dot_value < 1e-5))
        {
            result.clear();
            return result;
        }

        // 如果共面，则加入结果集合
        result.insert(adj_ring);
    }

    return result;
}

// 函数功能：根据给定的偏移量计算环上各点的新位置，处理共线和不共线的情况，生成新的点位置数组
std::vector<nous::pos_t> Controller_Extrude_ByNormal::offset_ring_positions(GeoData* picked_geo, SketchRingPtr ring, SketchPointPtr prev_pt,
                                                                            SketchPointPtr next_pt, double offset)
{
    // 创建一个存储结果的向量
    std::vector<nous::pos_t> result;
    // 获取环上的点数组
    std::vector<SketchPointPtr> ring_pt_arr = GeoDataUtils::get().get_ring_topo(picked_geo, ring);

    // 初始化添加顺序和第一次查找标志
    bool add_order = true;
    bool first_find = true;

    // 初始化检查标志
    std::pair<int, int> check_flags = { -1, -1 };

    // 遍历环上的每个点
    for(int i = 0; i < ring_pt_arr.size(); ++i)
    {
        // 获取当前点和位置
        SketchPointPtr curr_pt = ring_pt_arr[i];
        const nous::pos_t pt_pos = picked_geo->get_point_position(curr_pt);
        // 计算偏移后的新位置
        const nous::pos_t offset_pos = pt_pos + m_pick_polygon_normal * offset;
        SketchPointPtr curr_next_pt = ring_pt_arr[(i + 1) % ring_pt_arr.size()];

        // 如果当前点是前一个点
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

            // 计算线段方向和法线方向的点积
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

            // 计算线段方向和法线方向的点积
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

    // 检查是否存在共线的点
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

        // 删除冗余的点
        for(size_t last_idx: delete_id_set)
        {
            result.erase(result.begin() + last_idx);
        }
    }

    // 返回结果
    return result;
}

// 函数功能：获取偏移后的多边形位置，包括外环和内环
std::vector<std::vector<nous::vec3>> Controller_Extrude_ByNormal::get_offseted_polygon_positions(GeoData* picked_geo, SketchPolygonPtr polygon,
                                                                                                 SketchPointPtr prev_pt, SketchPointPtr next_pt,
                                                                                                 double offset)
{
    // 创建一个存储结果的二维向量
    std::vector<std::vector<nous::vec3>> result;

    // 获取偏移后的外环位置，并将其添加到结果中
    std::vector<nous::vec3> exring_pos_arr = offset_ring_positions(picked_geo, polygon->ring, prev_pt, next_pt, offset);
    result.emplace_back(std::move(exring_pos_arr));

    // 遍历多边形的每个内环，获取偏移后的内环位置，并将其添加到结果中
    for(auto& inner_ring: polygon->inner_rings)
    {
        std::vector<nous::vec3> inring_pos_arr = offset_ring_positions(picked_geo, inner_ring, prev_pt, next_pt, offset);
        result.emplace_back(std::move(inring_pos_arr));
    }

    // 返回结果
    return result;
}

// 函数功能：判断一个多边形的正负法线方向上是否有其余 Polygon 限制其移动
Controller_Extrude_ByNormal::ESketchExtrudeMoveValidDir Controller_Extrude_ByNormal::need_flip_picked_normal(GeoData* geodata,
                                                                                                             SketchPolygonPtr pick_polygon)
{
    // 法线加一点坐标定义当前平面
    const nous::vec3 plane_normal = m_pick_polygon_normal;
    // const nous::vec3 plane_pos = geodata->get_point_position(pick_polygon->ring->ring_prim()->vertex_ptr()->point_ptr());

    unsigned char is_positive_plane_normal_valid = 1;
    unsigned char is_negative_plane_normal_valid = 2;

    // 遍历选中的多边形拓扑
    for(auto& ring: m_selected_polygon_topo)
    {
        // 遍历多边形的每条边
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

            // 遍历前一个点的邻居点
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

                // 检查正向偏移是否与其他多边形相交
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

                // 检查负向偏移是否与其他多边形相交
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

            // 遍历下一个点的邻居点
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

                // 检查正向偏移是否与其他多边形相交
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

                // 检查负向偏移是否与其他多边形相交
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

    // 返回有效的移动方向
    return static_cast<Controller_Extrude_ByNormal::ESketchExtrudeMoveValidDir>(is_positive_plane_normal_valid + is_negative_plane_normal_valid);
}

// 函数功能：处理选中的多边形，计算法线和初始位置，准备添加和移动模式的推断网格，并隐藏相关对象
void Controller_Extrude_ByNormal::processPickPolygon(GeoData* picked_geo, SketchRingPtr ring)
{
    // 将法线从模型空间旋转到世界空间
    Engine::Mat43 mat = m_pick_mesh_doc->world_matrix();
    SketchPolygonPtr pick_polygon = picked_geo->get_polygon(ring);
    m_pick_polygon_normal = pick_polygon->ring->normal;
    m_pick_polygon_normal = NousConvert::product_v3(mat, m_pick_polygon_normal - NousConvert::convert(mat.col[3]).normalized());

    // 计算选中多边形的初始位置
    m_extrude_polygon_begin_position = picked_geo->get_mesh()->prim_barycenter(ring->ring_prim->idx());

    // 处理多个离散内环的必要步骤，将多个相邻且离散的内环合并成一个进行后续的挤出计算
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

    // 设置选中的多边形拓扑
    m_selected_polygon_topo = pick_polygen_rings_topo;

    // 准备添加和移动模式的推断网格
    std::unordered_set<ObjectDoc*> hide_docs;
    prepare_add_mode_inferring_mesh();
    prepare_move_mode_inferring_mesh(picked_geo, pick_polygon);

    // 根据挤出模式隐藏相关对象
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

    // 隐藏用于恢复变更的多边形
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    view->setHideObjects(hide_docs);

    // 标记推断网格文档为脏数据
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

// 函数功能：创建移动操作的网格，根据给定的偏移量更新推断网格文档中的多边形位置，并标记为脏数据
void Controller_Extrude_ByNormal::create_move_operation_mesh(double offset)
{
    // 如果偏移量大于正向偏移限制，则将偏移量设置为正向偏移限制
    if(offset > m_positive_offset_limit)
    {
        offset = m_positive_offset_limit;
    }

    // 如果偏移量小于负向偏移限制，则将偏移量设置为负向偏移限制
    if(offset < -m_negative_offset_limit)
    {
        offset = -m_negative_offset_limit;
    }

    // 如果存在移动推断网格文档
    if(m_move_inferring_mesh_doc)
    {
        // 计算偏移量的增量
        double delta_offset = offset - m_move_last_offset;
        // 更新最后的偏移量
        m_move_last_offset = offset;
        // 计算适配的偏移量
        nous::vec3 fitted_offset = delta_offset * m_pick_polygon_normal;

        // 遍历新的移动多边形拓扑集合中的每个点
        for(const auto& pt: m_new_move_polygon_topo)
        {
            // 获取当前点的位置
            const nous::pos_t middle_pos = m_move_inferring_mesh_doc->geodata()->get_point_position(pt);
            // 计算新的点位置
            const nous::pos_t new_pos = middle_pos + fitted_offset;
            // 设置新的点位置
            m_move_inferring_mesh_doc->geodata()->set_point_position(pt, new_pos);
        }

        // 标记所有的多边形为脏数据
        m_move_inferring_mesh_doc->geodata()->mark_all_sketch_polygon_dirty();
        // 标记移动推断网格文档为脏数据
        m_move_inferring_mesh_doc->dirty();
    }
}

// 函数功能：创建添加操作的网格，根据给定的偏移量更新推断网格文档中的多边形位置，并标记为脏数据
void Controller_Extrude_ByNormal::create_add_operation_mesh(double offset)
{
    // 检查是否存在添加推断网格文档
    if(m_add_inferring_mesh_doc)
    {
        // 计算偏移量的增量
        double delta_offset = offset - m_add_last_offset;
        // 更新最后的偏移量
        m_add_last_offset = offset;
        // 计算适配的偏移量
        nous::vec3 fitted_offset = delta_offset * m_pick_polygon_normal;

        // 遍历新的添加多边形拓扑集合中的每个点
        for(const auto& pt: m_new_add_polygon_topo)
        {
            // 获取当前点的位置
            const nous::pos_t middle_pos = m_add_inferring_mesh_doc->geodata()->get_point_position(pt);
            // 计算新的点位置
            const nous::pos_t new_pos = middle_pos + fitted_offset;
            // 设置新的点位置
            m_add_inferring_mesh_doc->geodata()->set_point_position(pt, new_pos);
        }

        // 标记所有的多边形为脏数据
        m_add_inferring_mesh_doc->geodata()->mark_all_sketch_polygon_dirty();
        // 标记添加推断网格文档为脏数据
        m_add_inferring_mesh_doc->dirty();
    }
}

// 函数功能：提交挤出网格，将临时网格合并到选中的网格文档中，并处理NURBS文档的更新
void Controller_Extrude_ByNormal::commit_extrude_mesh()
{
    // 获取当前聚焦的视图和场景文档
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    SceneDoc* scene = view->doc()->scene();
    // 清除隐藏的对象
    view->clearHideObjects();

    // 如果选中的网格文档存在
    if(m_pick_mesh_doc)
    {
        // 如果挤出模式为添加模式
        if(m_extrude_mode == ESketchExtrudeMode::ADD)
        {
            // 如果存在分割的旧网格文档，将其合并到选中的网格文档中，并删除分割的旧网格文档
            if(m_splited_old_mesh_doc)
            {
                m_pick_mesh_doc->merge(m_splited_old_mesh_doc);
                m_splited_old_mesh_doc = nullptr;
            }

            // 如果存在添加推断网格文档，将其合并到选中的网格文档中，并删除添加推断网格文档
            if(m_add_inferring_mesh_doc)
            {
                m_pick_mesh_doc->merge(m_add_inferring_mesh_doc);
                view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
                m_add_inferring_mesh_doc = nullptr;
            }

            // 如果存在移动推断网格文档，删除移动推断网格文档
            if(m_move_inferring_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
                m_move_inferring_mesh_doc = nullptr;
            }
        }
        // 如果挤出模式为移动模式
        else if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            // 如果存在移动推断网格文档，修复推断网格文档，并将其合并到选中的网格文档中，删除移动推断网格文档
            if(m_move_inferring_mesh_doc)
            {
                fix_inferring_mesh_doc();
                const bool is_success = m_pick_mesh_doc->merge(m_move_inferring_mesh_doc);
                view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
                m_move_inferring_mesh_doc = nullptr;

                // 如果合并失败，停用控制器并输出错误信息
                if(!is_success)
                {
                    deactive();
                    std::cout << "This Operation Caused Topology Error, End Extrude Operation!" << std::endl;
                }
            }

            // 如果存在分割的旧网格文档，删除分割的旧网格文档
            if(m_splited_old_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
                m_splited_old_mesh_doc = nullptr;
            }

            // 如果存在添加推断网格文档，删除添加推断网格文档
            if(m_add_inferring_mesh_doc)
            {
                view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
                m_add_inferring_mesh_doc = nullptr;
            }
        }

        // 如果存在复制的原始网格文档，删除复制的原始网格文档
        if(m_copyed_origin_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
            m_copyed_origin_mesh_doc = nullptr;
        }
    }

    // 如果选中的NURBS文档存在
    if(m_pick_nurbs_doc)
    {
        // 获取NURBS数据
        NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
        // 遍历临时表面的GUID
        for(auto& item_id: m_tmp_surfaces_guid)
        {
            // 获取临时表面
            auto& tmp_nurbs_item = nurbs_data->get_item(item_id);
            // 如果临时表面是代数曲面或封闭曲面，移除临时表面，并将其转换为修剪曲面添加到NURBS数据中
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
        // 标记NURBS文档为脏数据
        m_pick_nurbs_doc->dirty();
    }

    // 重置挤出步骤为选择元素
    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT;

    // 清除挤出操作的限制参数
    clear_limits();
    // 清除新的移动多边形拓扑
    m_new_move_polygon_topo.clear();
    // 清除新的添加多边形拓扑
    m_new_add_polygon_topo.clear();
    // 清除临时表面的GUID
    m_tmp_surfaces_guid.clear();
}

// 在挤出操作确认阶段管理临时网格的显示状态，根据当前操作模式（移动/添加）隐藏预览网格并生成最终操作结果网格
void Controller_Extrude_ByNormal::switchMesh()
{
    // 检查当前的挤出步骤是否为 EXTRUDE_FIXING
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 获取当前聚焦的视图
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        // 清除隐藏的对象
        view->clearHideObjects();
        // 创建一个用于存储隐藏对象的集合
        std::unordered_set<ObjectDoc*> hide_docs;

        // 如果当前的挤出模式为 MOVE
        if(m_extrude_mode == ESketchExtrudeMode::MOVE)
        {
            // 检查是否存在分割的旧网格文档和移动推断网格文档
            if(m_splited_old_mesh_doc && m_move_inferring_mesh_doc)
            {
                // 将移动推断网格文档、分割的旧网格文档和选中的网格文档添加到隐藏对象集合中
                hide_docs.insert(m_move_inferring_mesh_doc);
                hide_docs.insert(m_splited_old_mesh_doc);
                hide_docs.insert(m_pick_mesh_doc);
            }
            // 创建添加操作的网格
            create_add_operation_mesh(m_move_last_offset);
        }
        else
        {
            // 如果当前的挤出模式为 ADD
            if(m_add_inferring_mesh_doc)
            {
                // 将添加推断网格文档、分割的旧网格文档和复制的原始网格文档添加到隐藏对象集合中
                hide_docs.insert(m_add_inferring_mesh_doc);
                hide_docs.insert(m_splited_old_mesh_doc);
                hide_docs.insert(m_copyed_origin_mesh_doc);
            }
            // 创建移动操作的网格
            create_move_operation_mesh(m_add_last_offset);
        }
        // 设置隐藏对象
        view->setHideObjects(hide_docs);
    }
}

// 为添加模式创建实时预览网格，通过沿法线方向偏移选中多边形生成侧面和顶面几何结构，并维护原始网格副本用于操作回退
void Controller_Extrude_ByNormal::prepare_add_mode_inferring_mesh()
{
    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    // 获取当前视图的场景文档
    SceneDoc* scene = view->doc()->scene();
    
    // 如果存在添加推断网格文档，则删除它并将指针置为空
    if(m_add_inferring_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
        m_add_inferring_mesh_doc = nullptr;
    }
    // 创建一个新的添加推断网格文档
    m_add_inferring_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);

    // 如果存在复制的原始网格文档，则删除它并将指针置为空
    if(m_copyed_origin_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
        m_copyed_origin_mesh_doc = nullptr;
    }
    // 创建一个新的复制的原始网格文档并从选中的网格文档中复制数据
    m_copyed_origin_mesh_doc = (MeshDoc*)view->create_obj_in_working_group(Define::OBJ_D5_MESH);
    m_copyed_origin_mesh_doc->copy_from(m_pick_mesh_doc);

    // 设置移动增量为0.1
    double move_delta = 0.1;
    m_add_last_offset = move_delta;

    // 创建一个新的多边形位置数组
    std::vector<std::vector<nous::pos_t>> new_polygon_pos;
    // 遍历选中的多边形拓扑
    for(int i = 0; i < m_selected_polygon_topo.size(); ++i)
    {
        // 获取当前环的点
        const std::vector<SketchPointPtr> ring = m_selected_polygon_topo[i];

        // 创建一个新的顶部环位置数组
        std::vector<nous::pos_t>& new_top_ring = new_polygon_pos.emplace_back();
        // 遍历环中的每个点
        for(int j = 0; j < ring.size(); ++j)
        {
            // 获取当前点和下一个点
            SketchPointPtr curr_pt = ring[j];
            SketchPointPtr next_pt = ring[(j + 1) % ring.size()];
            // 获取当前点和下一个点的位置
            const nous::pos_t curr_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(curr_pt);
            const nous::pos_t next_pt_pos = m_pick_mesh_doc->geodata()->get_point_position(next_pt);
            // 计算新的当前点和下一个点的位置
            const nous::pos_t new_curr_pt_pos = curr_pt_pos + m_add_last_offset * m_pick_polygon_normal;
            const nous::pos_t new_next_pt_pos = next_pt_pos + m_add_last_offset * m_pick_polygon_normal;

            // 创建一个新的侧面多边形并添加到添加推断网格文档中
            std::vector<std::vector<nous::pos_t>> new_side_polygon = { { curr_pt_pos, next_pt_pos, new_next_pt_pos, new_curr_pt_pos } };
            m_add_inferring_mesh_doc->geodata()->add_polygon(new_side_polygon);

            // 将新的当前点位置添加到顶部环位置数组中
            new_top_ring.emplace_back(new_curr_pt_pos);
        }
    }

    // 将新的多边形位置数组添加到添加推断网格文档中
    m_new_add_polygon_topo_arr = m_add_inferring_mesh_doc->geodata()->add_polygon(new_polygon_pos);
    // 遍历新的添加多边形拓扑数组并将点添加到新的添加多边形拓扑集合中
    for(const auto& ring_pt_arr: m_new_add_polygon_topo_arr)
    {
        for(auto& pt: ring_pt_arr)
        {
            m_new_add_polygon_topo.insert(pt);
        }
    }

    // 标记添加推断网格文档为脏数据
    m_add_inferring_mesh_doc->dirty();
    // 标记添加推断网格文档的几何数据为脏数据
    m_add_inferring_mesh_doc->geodata()->dirty();
}

// 函数功能：准备移动模式下的推断网格
// 1. 判断是否需要反转法线方向
// 2. 根据法线方向设置移动增量
// 3. 创建新的推断网格文档和分割的旧网格文档
// 4. 查找选中面的每条线段是否有与之共面的多边形，并记录
// 5. 处理共面多边形，计算可移动限制，生成新的侧面多边形
// 6. 删除现有的底面，替换为新的顶面
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

    // 根据法线方向设置移动增量
    double move_delta = 0.1;
    if(m_valid_move_dir == ESketchExtrudeMoveValidDir::Negative)
    {
        move_delta = -move_delta;
    }
    m_move_last_offset = move_delta;

    // 获取当前聚焦的视图和场景文档
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    SceneDoc* scene = view->doc()->scene();
    if(m_move_inferring_mesh_doc)
    {
        // 删除已有的推断网格文档和分割的旧网格文档
        view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
        view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
        m_move_inferring_mesh_doc = nullptr;
        m_splited_old_mesh_doc = nullptr;
    }
    // 创建新的推断网格文档和分割的旧网格文档
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

    // add
    // 在生成新顶面之后添加检测逻辑
    m_new_move_polygon_topo_arr = m_move_inferring_mesh_doc->geodata()->add_polygon(self_positions);
    // 获取新增顶面的几何数据
    std::vector<std::vector<nous::vec3>> new_top_polygons;
    GeoDataUtils::get().get_polygon_positions(m_move_inferring_mesh_doc->geodata().get(), m_new_move_polygon_topo_arr, new_top_polygons);
    // 遍历现有的多边形检测包含关系
    bool need_remove = false;
    auto all_polygons = picked_geo->get_all_polygons();
    for (auto& existing_polygon : all_polygons)
    {
        if (GeoDataUtils::is_polygon_contained(picked_geo, new_top_polygons, existing_polygon))
        {
            need_remove = true;
            break;
        }
    }
    // 若被包含则删除
    if (need_remove)
    {
        m_move_inferring_mesh_doc->geodata()->delete_polygon(m_new_move_polygon_topo_arr);
        m_new_move_polygon_topo_arr.clear();
    }
    // add

    // 设置移动限制
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

// 函数功能：清除所有缓存的网格文档，包括隐藏对象、推断网格文档、分割的旧网格文档、复制的原始网格文档和NURBS文档
void Controller_Extrude_ByNormal::clear_all_cache_mesh_doc()
{
    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    // 清除隐藏的对象
    view->clearHideObjects();
    // 清除推断网格文档
    clear_inferring_mesh_doc();
    // 获取当前视图的场景文档
    SceneDoc* scene = view->doc()->scene();
    
    // 如果存在分割的旧网格文档，则将其合并到选中的网格文档中，并删除分割的旧网格文档
    if(m_splited_old_mesh_doc)
    {
        m_pick_mesh_doc->merge(m_splited_old_mesh_doc);
        view->delete_obj_and_try_delete_relation(m_splited_old_mesh_doc);
        m_splited_old_mesh_doc = nullptr;
    }

    // 如果存在复制的原始网格文档，则删除它
    if(m_copyed_origin_mesh_doc)
    {
        view->delete_obj_and_try_delete_relation(m_copyed_origin_mesh_doc);
        m_copyed_origin_mesh_doc = nullptr;
    }

    // 如果存在NURBS文档，则删除临时表面，并将NURBS文档标记为脏数据
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

// 函数功能：清除推断网格文档，如果当前挤出步骤为 EXTRUDE_FIXING，则删除移动和添加推断网格文档，并将指针置为空
void Controller_Extrude_ByNormal::clear_inferring_mesh_doc()
{
    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    // 检查当前的挤出步骤是否为 EXTRUDE_FIXING
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        // 获取当前视图的场景文档
        SceneDoc* pScene = view->doc()->scene();
        // 如果存在移动推断网格文档，则删除它并将指针置为空
        if(m_move_inferring_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_move_inferring_mesh_doc);
            m_move_inferring_mesh_doc = nullptr;
        }
        // 如果存在添加推断网格文档，则删除它并将指针置为空
        if(m_add_inferring_mesh_doc)
        {
            view->delete_obj_and_try_delete_relation(m_add_inferring_mesh_doc);
            m_add_inferring_mesh_doc = nullptr;
        }
    }
}

// 函数功能：清除挤出操作的限制参数，将其重置为默认值
void Controller_Extrude_ByNormal::clear_limits()
{
    // 将负向偏移限制重置为最大值
    m_negative_offset_limit = std::numeric_limits<double>::max();

    // 将正向偏移限制重置为最大值
    m_positive_offset_limit = std::numeric_limits<double>::max();

    // 将有效移动方向重置为无效
    m_valid_move_dir = ESketchExtrudeMoveValidDir::None;

    // 将移动的最后偏移量重置为0
    m_move_last_offset = 0.0;

    // 将添加的最后偏移量重置为0
    m_add_last_offset = 0.0;
}

// 函数功能：修复推断网格文档中的多边形，处理多边形的分割和合并，确保多边形的拓扑结构正确
void Controller_Extrude_ByNormal::fix_inferring_mesh_doc()
{
    if(m_move_inferring_mesh_doc)
    {
        // 获取推断网格文档的几何数据
        GeoData* inferring_geodata = m_move_inferring_mesh_doc->geodata().get();
        // 获取所有的多边形
        std::unordered_set<SketchPolygonPtr> all_sketch_polygons = inferring_geodata->get_all_polygons();

        // 存储分割后的多边形位置
        std::vector<std::vector<std::vector<nous::pos_t>>> splited_polygon_positions;
        for(SketchPolygonPtr polygon: all_sketch_polygons)
        {
            // 跳过顶面多边形
            if(polygon->ring->key == GeoData::ring_key(m_new_move_polygon_topo_arr.front()))
            {
                continue;
            }

            // 获取多边形的拓扑结构
            std::vector<std::vector<std::vector<SketchPointPtr>>> splited_polygons;
            std::vector<std::vector<SketchPointPtr>> polygon_topo = GeoDataUtils::get().get_polygon_topo(inferring_geodata, polygon);
            std::vector<std::vector<SketchPointPtr>> polygon_inner_topo;
            polygon_inner_topo.insert(polygon_inner_topo.end(), polygon_topo.begin() + 1, polygon_topo.end());
            // 计算多边形与其他环的差异
            geodata_topo::ring_difference_other_rings(inferring_geodata, polygon_topo[0], polygon_inner_topo, polygon->ring->normal,
                                                      splited_polygons);
            if(splited_polygons.empty())
            {
                // 如果没有分割，则直接获取多边形的环
                std::vector<std::vector<SketchPointPtr>>& points = splited_polygons.emplace_back();
                std::vector<SketchPointPtr>& exring_pts = points.emplace_back();
                GeoDataUtils::get().get_ring_topo(inferring_geodata, polygon->ring, exring_pts);
                for(const auto inner_ring: polygon->inner_rings)
                {
                    std::vector<SketchPointPtr>& inring_pts = points.emplace_back();
                    GeoDataUtils::get().get_ring_topo(inferring_geodata, inner_ring, inring_pts);
                }
            }

            // 遍历分割后的多边形
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
                    // 获取当前环的点位置
                    GeoDataUtils::get().get_ring_positions(inferring_geodata, splited_polygons[i][j], curr_ring_pos_arr);
                    for(int k = 0; k < splited_polygons[i][j].size(); ++k)
                    {
                        SketchPointPtr prev_pt = splited_polygons[i][j][k];
                        SketchPointPtr next_pt = splited_polygons[i][j][(k + 1) % splited_polygons[i][j].size()];
                        // 获取当前线段
                        SketchLinePtr line = inferring_geodata->get_line(GeoData::line_key(prev_pt, next_pt));

                        // 检查当前线段是否在顶面环上
                        for(auto& top_ring: m_new_move_polygon_topo_arr)
                        {
                            const bool is_shared_line = GeoDataUtils::get().is_line_on_ring(inferring_geodata, line, top_ring);
                            if(is_shared_line)
                            {
                                // 如果是共享线段，分割当前环
                                checked_ring_pos_arr = topo::split_self_cross_ring(curr_ring_pos_arr, k);
                                do_extra_process = true;
                            }
                        }
                    }

                    // 外环被切开了，这个时候要去分内环
                    if(do_extra_process && j == 0 && checked_ring_pos_arr.size() > 1)
                    {
                        // 处理共享线在外环上且外环被分成多个的情况
                        for(int m = 0; m < checked_ring_pos_arr.size(); ++m)
                        {
                            std::vector<std::vector<nous::pos_t>> spec_part_positions;
                            spec_part_positions.emplace_back(std::move(checked_ring_pos_arr[m]));
                            // 转换为 geos 多边形
                            geos::geom::Geometry* exring_polygon =
                                NousGeometryConverter::position_convert_to_geos_polygon(spec_part_positions.front(), polygon->ring->normal);

                            // 检查内环是否在外环内
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
                        // 处理共享线不在内环或者不在外环上的情况
                        std::vector<nous::pos_t>& ring_pos_arr = part_positions.emplace_back();
                        GeoDataUtils::get().get_ring_positions(inferring_geodata, splited_polygons[i][j], ring_pos_arr);
                    }
                    else
                    {
                        // 处理共享线在内外环上，且外环没有被切割，或内环被切割的情况
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

            // 删除原有的多边形
            inferring_geodata->delete_polygon(polygon, true, false);
        }

        // 添加分割后的多边形
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
                    // 处理可能的环被扣掉的情况
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

// 函数功能：处理选择信息，确定选择的元素类型并进行相应的处理
bool Controller_Extrude_ByNormal::_process_select_info(const std::unordered_map<RenderObject*, SelectInfo>& select_info_map)
{
    int valid_element_number = 0;

    // 初始化选择的表面指针为空
    m_selected_algebra_surface = nullptr;
    m_selected_closed_surface = nullptr;
    m_selected_trimmed_surface = nullptr;

    // 遍历选择信息映射
    for(auto& select_pair: select_info_map)
    {
        // 如果选择的是网格对象
        if(select_pair.first->type() == Define::ObjectType::OBJ_D5_MESH)
        {
            // 如果选择的元素数量大于1或已经有有效元素
            if(select_pair.second.elements.size() > 1 || valid_element_number > 0)
            {
                // 如果选择的元素是多边形
                if(select_pair.second.elements.begin()->get()->to_polygon())
                {
                    valid_element_number += select_pair.second.elements.size();
                    break;
                }
            }

            // 遍历选择的元素
            for(auto& ele: select_pair.second.elements)
            {
                // 如果元素是多边形
                if(ele->to_polygon())
                {
                    // 设置选择的网格文档和几何数据
                    m_pick_mesh_doc = dynamic_cast<MeshDoc*>(select_pair.first->doc());
                    m_pick_nurbs_doc = nullptr;
                    GeoData* geodata = m_pick_mesh_doc->geodata(true).get();
                    // 处理选择的多边形
                    this->processPickPolygon(geodata, ele->to_polygon()->ring);
                    valid_element_number++;
                }
            }
        }

        // 如果选择的是NURBS对象
        if(select_pair.first->type() == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* tmp_nurbs_doc = dynamic_cast<NurbsDoc*>(select_pair.first->doc());
            NurbsDataPtr picked_nurbs_data = tmp_nurbs_doc->nurbsdata(true);
            const Engine::Mat43 world_mat = tmp_nurbs_doc->world_matrix();
            const nous::mat4 trans_mat = NousConvert::convert(world_mat);
            // 遍历选择的元素
            for(auto& ele: select_pair.second.elements)
            {
                // 如果元素是代数曲面
                if(ele->type() == EElementType::AlgebraSurface)
                {
                    m_pick_nurbs_doc = tmp_nurbs_doc;
                    m_pick_mesh_doc = nullptr;
                    m_selected_algebra_surface = ele->to_nurbs_algebra_surface();
                    valid_element_number++;
                }

                // 如果元素是封闭曲面
                if(ele->type() == EElementType::ClosedSurface)
                {
                    m_pick_nurbs_doc = tmp_nurbs_doc;
                    m_pick_mesh_doc = nullptr;
                    m_selected_closed_surface = ele->to_nurbs_closed_surface();
                    valid_element_number++;
                }

                // 如果元素是修剪曲面
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

    // 如果有效元素数量不等于1，重置选择的表面指针并返回false
    if(valid_element_number != 1)
    {
        m_selected_algebra_surface = nullptr;
        m_selected_closed_surface = nullptr;
        return false;
    }

    return true;
}

// 函数功能：根据给定的值创建NURBS操作网格，分别处理代数曲面、封闭曲面和修剪曲面
void Controller_Extrude_ByNormal::_create_nurbs_operation_mesh(const nous::scalar value)
{
    // 如果选择了代数曲面，则生成代数曲面的操作网格
    if(m_selected_algebra_surface)
    {
        _generate_algebra_surface_operation_mesh(value);
    }

    // 如果选择了封闭曲面，则生成封闭曲面的操作网格
    if(m_selected_closed_surface)
    {
        _generate_closed_surface_operation_mesh(value);
    }

    // 如果选择了修剪曲面，则生成修剪曲面的操作网格
    if(m_selected_trimmed_surface)
    {
        _generate_complex_surface_operation_mesh(value);
    }
}

// 函数功能：生成代数曲面的操作网格，首先清除临时表面，然后根据给定的值生成新的表面，并将其添加到NURBS数据中
void Controller_Extrude_ByNormal::_generate_algebra_surface_operation_mesh(const nous::scalar value)
{
    // 获取NURBS数据
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    // 清除临时表面
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    // 如果值接近于0，则返回
    if(fabs(value) < 1e-5)
    {
        return;
    }

    // 根据给定的值生成新的表面
    const std::vector<std::shared_ptr<INurbsBase>> new_surfaces = nurbs::extrude_surface_by_normal_keep_distance(m_selected_algebra_surface, value);
    // 将新的表面添加到NURBS数据中
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

// 函数功能：生成封闭曲面的操作网格，首先清除临时表面，然后根据给定的值生成新的表面，并将其添加到NURBS数据中
void Controller_Extrude_ByNormal::_generate_closed_surface_operation_mesh(const nous::scalar value)
{
    // 获取NURBS数据
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    // 清除临时表面
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    // 如果值接近于0，则返回
    if(fabs(value) < 1e-5)
    {
        return;
    }

    // 根据给定的值生成新的表面
    const std::vector<std::shared_ptr<INurbsBase>> new_surfaces =
        nurbs::extrude_surface_by_normal(m_selected_closed_surface, value, m_pick_polygon_normal);

    // 将新的表面添加到NURBS数据中
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

// 函数功能：生成修剪曲面的操作网格，首先清除临时表面，然后根据给定的值生成新的表面，并将其添加到NURBS数据中
void Controller_Extrude_ByNormal::_generate_complex_surface_operation_mesh(const nous::scalar value)
{
    // 获取NURBS数据
    NurbsDataPtr nurbs_data = m_pick_nurbs_doc->nurbsdata();
    // 清除临时表面
    for(const auto& guid: m_tmp_surfaces_guid)
    {
        nurbs_data->remove_item(guid);
    }
    m_tmp_surfaces_guid.clear();
    m_pick_nurbs_doc->dirty();

    // 如果值接近于0，则返回
    if(fabs(value) < 1e-5)
    {
        return;
    }

    // 根据给定的值生成新的表面
    std::vector<std::shared_ptr<INurbsBase>> new_surfaces = nurbs::extrude_surface_by_normal_keep_distance(m_selected_trimmed_surface, value);
    for(int i = 0; i < new_surfaces.size(); ++i)
    {
        // 如果新表面是代数曲面，则设置其渲染级别为线框
        if(new_surfaces[i]->type() == EElementType::AlgebraSurface)
        {
            new_surfaces[i]->to_nurbs_algebra_surface()->set_render_level(ENurbsSurfaceRenderLevel::WireFrame);
        }
        // 将新的表面添加到NURBS数据中
        nurbs_data->add_item(new_surfaces[i]);
        new_surfaces[i]->dirty();
        m_tmp_surfaces_guid.insert(new_surfaces[i]->id());
    }
    new_surfaces.clear();

    m_pick_nurbs_doc->dirty();
}

// 函数功能：回滚到选择元素的状态，清除所有缓存的网格文档，重置相关参数和状态
void Controller_Extrude_ByNormal::_rollback_to_pick_element()
{
    // 如果当前的挤出步骤是EXTRUDE_FIXING，则清除所有缓存的网格文档
    if(m_extrude_step == ESketchExtrudeStep::EXTRUDE_FIXING)
    {
        clear_all_cache_mesh_doc(); // 清除所有缓存的网格文档
    }
    m_pick_mesh_doc = nullptr; // 重置选中的网格文档指针
    clear_limits(); // 清除挤出操作的限制参数

    // 获取当前聚焦的视图
    View* view = STCore::inst->viewMgr()->layout()->focusedView();
    InferenceHelper* inferHelper = view->inferHelper(); // 获取推断助手
    inferHelper->set_infer_filter_type(InferType::ALL_TYPE); // 设置推断过滤类型为所有类型
    m_new_move_polygon_topo.clear(); // 清除新的移动多边形拓扑
    m_new_add_polygon_topo.clear(); // 清除新的添加多边形拓扑
    m_tmp_surfaces_guid.clear(); // 清除临时表面的GUID
    view->inferHelper()->clear_ignore(); // 清除推断助手的忽略列表

    m_extrude_step = ESketchExtrudeStep::PICK_ELEMENT; // 重置挤出步骤为选择元素
}

}// namespace sketch
