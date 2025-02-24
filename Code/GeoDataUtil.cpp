#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "CDT.h"
#include "VerifyTopology.h"
#include "geos/operation/buffer/BufferOp.h"
#include "geos/operation/polygonize/Polygonizer.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/SmoothGroup.h"
#include <execution>
#include <sketch/NousEngine/NousConvert.h>

#include "sketch/NousEngine/SketchMath.h"

#include <sketch/NousEngine/earcut.hpp>

namespace sketch
{

/**
 * @brief 获取GeoDataUtils的单例实例
 * 
 * @return GeoDataUtils& 返回GeoDataUtils的引用
 */
GeoDataUtils& GeoDataUtils::get()
{
    static GeoDataUtils _instance;
    return _instance;
}

/**
 * @brief 获取指定点集的轴对齐边界框（AABB）
 * 
 * @param geodata GeoData对象指针
 * @param points 点集，包含nous::vec3类型的点
 * @return nous::AABB3 返回计算得到的AABB
 */
nous::AABB3 GeoDataUtils::get_aabb3(GeoData* geodata, const std::vector<nous::vec3>& points) { return sketch_math::aabb3_from_points(points, 0.1f); }

/**
 * @brief 获取指定点集的轴对齐边界框（AABB）
 * 
 * @param geodata GeoData对象指针
 * @param points 点集，包含SketchPointPtr类型的点
 * @return nous::AABB3 返回计算得到的AABB
 */
nous::AABB3 GeoDataUtils::get_aabb3(GeoData* geodata, const std::vector<SketchPointPtr>& points)
{
    std::vector<nous::vec3> points_pos;
    for(auto p: points)
    {
        // 获取每个点的原始位置
        nous::vec3 point_origin = geodata->get_point_position(p);
        // 将点的位置添加到points_pos向量中
        points_pos.emplace_back(point_origin);
    }
    // 计算并返回AABB
    return sketch_math::aabb3_from_points(points_pos, 0.1f);
}

/**
 * @brief 获取指定元素的轴对齐边界框（AABB）
 * 
 * @param geodata GeoData对象指针
 * @param element SketchElement对象指针
 * @return nous::AABB3 返回计算得到的AABB
 */
nous::AABB3 GeoDataUtils::get_element_aabb3(GeoData* geodata, SketchElementPtr element)
{
    if(element->type() == EElementType::Line)
    {
        // 强制更新position handle
        geodata->m_dirty_flags |= 0x2;
        geodata->_sync_nous();

        // 获取线段的两个端点位置
        SketchLinePtr line = element->to_line();
        nous::vec3 pos0 = geodata->get_point_position(line->p0->point);
        nous::vec3 pos1 = geodata->get_point_position(line->p1->point);

        // 计算并返回AABB
        return sketch_math::aabb3_from_points({ pos0, pos1 }, 0.1f);
    }
    else if(element->type() == EElementType::Ring)
    {
        // 强制更新position handle
        geodata->m_dirty_flags |= 0x2;
        geodata->_sync_nous();

        // 获取环的所有点的位置
        SketchRingPtr ring = element->to_ring();
        std::vector<nous::vec3> points_pos = get_ring_positions(geodata, ring);

        // 计算并返回AABB
        return sketch_math::aabb3_from_points(points_pos, 0.1f);
    }
    // 返回默认的AABB
    return nous::AABB3();
}

/**
 * @brief 获取指定多边形的拓扑结构
 * 
 * @param geodata GeoData对象指针
 * @param polygon SketchPolygon对象指针
 * @param out_polygon 输出的多边形拓扑结构，包含多个点集
 */
void GeoDataUtils::get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<SketchPointPtr>>& out_polygon)
{
    // 获取外环的点集并添加到输出多边形中
    std::vector<SketchPointPtr>& exring_points_arr = out_polygon.emplace_back();
    geodata->get_element_points(polygon->ring, exring_points_arr);

    // 获取每个内环的点集并添加到输出多边形中
    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr>& inring_points_arr = out_polygon.emplace_back();
        geodata->get_element_points(inner_ring, inring_points_arr);
    }
}

/**
 * @brief 获取指定多边形的拓扑结构
 * 
 * @param geodata GeoData对象指针
 * @param polygon SketchPolygon对象指针
 * @return std::vector<std::vector<SketchPointPtr>> 返回多边形的拓扑结构，包含多个点集
 */
std::vector<std::vector<SketchPointPtr>> GeoDataUtils::get_polygon_topo(GeoData* geodata, SketchPolygonPtr polygon)
{
    // 定义结果变量
    std::vector<std::vector<SketchPointPtr>> result;
    // 调用重载函数获取多边形拓扑结构
    get_polygon_topo(geodata, polygon, result);
    // 返回结果
    return result;
}

/**
 * @brief 获取指定多边形的拓扑结构并根据焊接信息重新排序
 * 
 * @param geodata GeoData对象指针
 * @param polygon SketchPolygon对象指针
 * @param out_polygon 输出的多边形拓扑结构，包含多个点集
 */
void GeoDataUtils::get_polygon_topo_and_reorder_by_weld_info(GeoData* geodata, SketchPolygonPtr polygon,
                                                             std::vector<std::vector<SketchPointPtr>>& out_polygon)
{
    // 获取多边形的拓扑结构
    get_polygon_topo(geodata, polygon, out_polygon);

    // 遍历每个环
    for(int i = 0; i < out_polygon.size(); ++i)
    {
        auto& out_ring = out_polygon[i];
        // 保证第一段线一定是焊接线的起始，或者不是焊接线
        int offset_number = 0;
        SketchPointPtr first_point = out_ring[0];
        SketchPointPtr second_point = out_ring[1];

        // 获取第一条线段的key
        size_t first_line_key = GeoData::line_key(first_point, second_point);
        // 获取第一条线段
        SketchLinePtr first_line = geodata->get_line(first_line_key);

        size_t curr_group_idx = -1;
        // 判断第一条线段是否属于NoBranchLineGroup
        if(first_line->group() && first_line->group()->type() == EElementType::NoBranchLineGroup)
        {
            curr_group_idx = first_line->group()->id();
        }

        // 遍历环中的每条线段
        for(int j = 1; j < out_ring.size() - 1; ++j)
        {
            SketchPointPtr prev_point = out_ring[j];
            SketchPointPtr next_point = out_ring[j + 1];

            // 获取当前线段的key
            size_t behind_line_key = GeoData::line_key(prev_point, next_point);
            // 获取当前线段
            SketchLinePtr behind_line = geodata->get_line(behind_line_key);
            size_t next_group_idx = -1;
            // 判断当前线段是否属于NoBranchLineGroup
            if(behind_line->group() && behind_line->group()->type() == EElementType::NoBranchLineGroup)
            {
                next_group_idx = behind_line->group()->id();
            }

            // 如果当前线段和第一条线段不属于同一个组，则记录偏移量
            if(next_group_idx != curr_group_idx)
            {
                offset_number = j;
                break;
            }
        }

        // 如果存在偏移量，则重新排列点集
        if(offset_number != 0)
        {
            // 偏移
            std::vector<SketchPointPtr> offseted_points_arr(out_ring.size(), nullptr);
            for(int j = 0; j < out_ring.size(); ++j)
            {
                offseted_points_arr[j] = out_ring[(j + offset_number) % out_ring.size()];
            }
            out_polygon[i] = offseted_points_arr;
        }
    }
}

/**
 * @brief 获取指定多边形的顶点位置
 * 
 * @param geodata GeoData对象指针
 * @param polygon SketchPolygon对象指针
 * @param out_pos_arr 输出的多边形顶点位置数组，包含多个点集
 */
void GeoDataUtils::get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon, std::vector<std::vector<nous::pos_t>>& out_pos_arr)
{
    // 获取外环的点集并添加到输出位置数组中
    std::vector<SketchPointPtr> exring_points_arr;
    std::vector<nous::pos_t>& exring_pos_arr = out_pos_arr.emplace_back();
    geodata->get_element_points(polygon->ring, exring_points_arr);
    for(auto one_point: exring_points_arr)
    {
        // 获取每个点的原始位置并添加到外环位置数组中
        nous::vec3 point_origin = geodata->get_point_position(one_point);
        exring_pos_arr.emplace_back(point_origin);
    }

    // 获取每个内环的点集并添加到输出位置数组中
    for(const auto& inner_ring: polygon->inner_rings)
    {
        std::vector<SketchPointPtr> inring_points_arr;
        geodata->get_element_points(inner_ring, inring_points_arr);
        std::vector<nous::pos_t>& inring_pos_arr = out_pos_arr.emplace_back();
        for(auto one_point: inring_points_arr)
        {
            // 获取每个点的原始位置并添加到内环位置数组中
            nous::vec3 point_origin = geodata->get_point_position(one_point);
            inring_pos_arr.emplace_back(point_origin);
        }
    }
}

/**
 * @brief 获取多边形所有顶点的位置坐标集合
 * 
 * 该函数用于获取指定多边形（包含外环和所有内环）的顶点坐标信息。
 * 通过调用重载函数实现具体功能，返回二维坐标数组。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon SketchPolygon对象指针，需要获取顶点的多边形
 * 
 * @return std::vector<std::vector<nous::pos_t>> 
 *         返回二维位置坐标数组，结构说明：
 *         - 外层vector：包含多个环（第一个为外环，后续为内环）
 *         - 内层vector：单个环的顶点坐标集合（nous::pos_t类型）
 * 
 * @note 函数内部实际工作由重载函数get_polygon_positions(GeoData*, SketchPolygonPtr, std::vector<std::vector<nous::pos_t>>&)完成
 */
std::vector<std::vector<nous::pos_t>> GeoDataUtils::get_polygon_positions(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::vector<std::vector<nous::pos_t>> result; ///< 存储结果的二维坐标容器
    get_polygon_positions(geodata, polygon, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含多边形所有环顶点坐标的二维数组
}

/**
 * @brief 获取多边形所有环的顶点位置坐标集合
 * 
 * 该函数用于获取指定多边形（包含外环和所有内环）的顶点坐标信息。
 * 输入参数为多边形的拓扑结构（点集的集合），输出为对应的顶点位置坐标集合。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 多边形的拓扑结构，包含多个环（第一个为外环，后续为内环），每个环由SketchPointPtr类型的点集表示
 * @param[out] out_pos_arr 输出的多边形顶点位置数组，包含多个点集
 * 
 * @note 函数内部遍历每个环，并将每个点的原始位置转换为nous::pos_t类型后添加到输出数组中。
 *       如果某个点没有对应的顶点指针，则跳过该点。
 */
void GeoDataUtils::get_polygon_positions(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& polygon,
                                         std::vector<std::vector<nous::pos_t>>& out_pos_arr)
{
    for(int i = 0; i < polygon.size(); ++i)
    {
        std::vector<nous::pos_t>& ring_pos_arr = out_pos_arr.emplace_back(); ///< 为当前环创建一个新的位置数组
        for(auto one_point: polygon[i])
        {
            if(!one_point->point->vertex_ptr())
            {
                continue; ///< 如果点没有对应的顶点指针，则跳过该点
            }
            nous::vec3 point_origin = geodata->get_point_position(one_point); ///< 获取点的原始位置
            ring_pos_arr.emplace_back(point_origin); ///< 将原始位置添加到当前环的位置数组中
        }
    }
}

/**
 * @brief 获取指定环的拓扑结构
 * 
 * 该函数用于获取指定环（SketchRingPtr）的拓扑结构，即环中所有点的顺序集合。
 * 通过调用GeoData对象的get_element_points方法实现具体功能，返回一个SketchPointPtr类型的点集。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring SketchRingPtr对象指针，需要获取拓扑结构的环
 * 
 * @return std::vector<SketchPointPtr> 
 *         返回一个包含环中所有点的顺序集合的向量，每个元素为SketchPointPtr类型
 * 
 * @note 函数内部通过调用geodata->get_element_points(ring, result)来填充结果向量。
 */
std::vector<SketchPointPtr> GeoDataUtils::get_ring_topo(GeoData* geodata, SketchRingPtr ring)
{
    std::vector<SketchPointPtr> result; ///< 存储结果的点集容器
    geodata->get_element_points(ring, result); ///< 调用GeoData的方法填充结果
    return result; ///< 返回包含环中所有点的顺序集合的向量
}

/**
 * @brief 获取指定环的拓扑结构
 * 
 * 该函数用于获取指定环（SketchRingPtr）的拓扑结构，即环中所有点的顺序集合。
 * 通过调用GeoData对象的get_element_points方法实现具体功能，并将结果存储在输出参数中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring SketchRingPtr对象指针，需要获取拓扑结构的环
 * @param[out] out_topo 输出的点集，包含环中所有点的顺序集合
 * 
 * @note 函数内部通过调用geodata->get_element_points(ring, out_topo)来填充结果向量。
 */
void GeoDataUtils::get_ring_topo(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& out_topo)
{
    geodata->get_element_points(ring, out_topo); ///< 调用GeoData的方法填充结果向量
}

/**
 * @brief 获取指定环的顶点位置坐标集合
 * 
 * 该函数用于获取指定环（SketchRingPtr）的顶点位置坐标集合。
 * 通过调用重载函数get_ring_positions(GeoData*, const SketchRingPtr&, std::vector<nous::pos_t>&)实现具体功能，并将结果返回。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring SketchRingPtr对象指针，需要获取顶点位置的环
 * 
 * @return std::vector<nous::pos_t> 返回包含环中所有顶点位置坐标的向量
 * 
 * @note 函数内部实际工作由重载函数get_ring_positions(GeoData*, const SketchRingPtr&, std::vector<nous::pos_t>&)完成
 */
std::vector<nous::pos_t> GeoDataUtils::get_ring_positions(GeoData* geodata, SketchRingPtr ring)
{
    std::vector<nous::pos_t> result; ///< 存储结果的向量
    get_ring_positions(geodata, ring, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含环中所有顶点位置坐标的向量
}

/**
 * @brief 获取指定环的顶点位置坐标集合
 * 
 * 该函数用于获取指定环（由SketchPointPtr类型的点集表示）的顶点位置坐标集合。
 * 通过遍历环中的每个点，获取其原始位置，并将其添加到输出数组中。
 * 如果某个点没有对应的顶点指针，则跳过该点。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 包含SketchPointPtr类型的点集，表示需要获取顶点位置的环
 * @param[out] out_pos_arr 输出的顶点位置数组，包含环中所有顶点的位置坐标（nous::pos_t类型）
 * 
 * @note 函数内部遍历每个点，并将每个点的原始位置转换为nous::pos_t类型后添加到输出数组中。
 *       如果某个点没有对应的顶点指针，则跳过该点。
 */
void GeoDataUtils::get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring, std::vector<nous::pos_t>& out_pos_arr)
{
    for(auto one_point: ring)
    {
        if(!one_point->point->vertex_ptr())
        {
            continue; ///< 如果点没有对应的顶点指针，则跳过该点
        }
        nous::vec3 point_origin = geodata->get_point_position(one_point); ///< 获取点的原始位置
        out_pos_arr.emplace_back(point_origin); ///< 将原始位置添加到输出数组中
    }
}

/**
 * @brief 获取指定环的顶点位置坐标集合
 * 
 * 该函数用于获取指定环（由SketchPointPtr类型的点集表示）的顶点位置坐标集合。
 * 通过调用重载函数get_ring_positions(GeoData*, const std::vector<SketchPointPtr>&, std::vector<nous::pos_t>&)实现具体功能，并将结果返回。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 包含SketchPointPtr类型的点集，表示需要获取顶点位置的环
 * 
 * @return std::vector<nous::pos_t> 返回包含环中所有顶点位置坐标的向量
 * 
 * @note 函数内部实际工作由重载函数get_ring_positions(GeoData*, const std::vector<SketchPointPtr>&, std::vector<nous::pos_t>&)完成
 */
std::vector<nous::pos_t> GeoDataUtils::get_ring_positions(GeoData* geodata, const std::vector<SketchPointPtr>& ring)
{
    std::vector<nous::pos_t> result; ///< 存储结果的向量
    get_ring_positions(geodata, ring, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含环中所有顶点位置坐标的向量
}

/**
 * @brief 获取指定环的顶点位置坐标集合
 * 
 * 该函数用于获取指定环（SketchRingPtr）的顶点位置坐标集合。
 * 通过调用GeoData对象的get_element_points方法获取环中的点集，然后遍历这些点，获取每个点的原始位置，并将其添加到输出数组中。
 * 如果某个点没有对应的顶点指针，则跳过该点。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring SketchRingPtr对象指针，需要获取顶点位置的环
 * @param[out] out_pos_arr 输出的顶点位置数组，包含环中所有顶点的位置坐标（nous::pos_t类型）
 * 
 * @note 函数内部遍历每个点，并将每个点的原始位置转换为nous::pos_t类型后添加到输出数组中。
 *       如果某个点没有对应的顶点指针，则跳过该点。
 */
void GeoDataUtils::get_ring_positions(GeoData* geodata, const SketchRingPtr ring, std::vector<nous::pos_t>& out_pos_arr)
{
    std::vector<SketchPointPtr> ring_pts; ///< 存储环中所有点的点集
    geodata->get_element_points(ring, ring_pts); ///< 获取环中的点集
    for(auto one_point: ring_pts)
    {
        if(!one_point->point->vertex_ptr())
        {
            continue; ///< 如果点没有对应的顶点指针，则跳过该点
        }
        nous::vec3 point_origin = geodata->get_point_position(one_point); ///< 获取点的原始位置
        out_pos_arr.emplace_back(point_origin); ///< 将原始位置添加到输出数组中
    }
}

/**
 * @brief 判断两个环是否共面
 * 
 * 该函数用于判断两个环（SketchRingPtr）是否位于同一个平面上。
 * 通过获取两个环的法向量和其中一个点的位置，使用sketch_math::plane_coplane_plane函数来判断两个平面是否共面。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring0 第一个环的指针
 * @param[in] ring1 第二个环的指针
 * 
 * @return bool 如果两个环共面，返回true；否则返回false
 * 
 * @note 函数内部首先获取两个环的法向量和其中一个点的位置，然后调用sketch_math::plane_coplane_plane函数进行共面判断。
 */
bool GeoDataUtils::is_two_ring_coplane(GeoData* geodata, const SketchRingPtr ring0, const SketchRingPtr ring1)
{
    const nous::vec3& ring0_normal = geodata->get_mesh()->prim_normal(ring0->ring_prim->idx()); ///< 获取第一个环的法向量
    const nous::vec3& ring1_normal = geodata->get_mesh()->prim_normal(ring1->ring_prim->idx()); ///< 获取第二个环的法向量
    const nous::pos_t& ring0_pos = geodata->get_point_position(geodata->m_nousptr_points.at(ring0->ring_prim->vertex_ptr()->point_ptr())); ///< 获取第一个环的一个点的位置
    const nous::pos_t& ring1_pos = geodata->get_point_position(geodata->m_nousptr_points.at(ring1->ring_prim->vertex_ptr()->point_ptr())); ///< 获取第二个环的一个点的位置

    return sketch_math::plane_coplane_plane(ring0_normal, ring0_pos, ring1_normal, ring1_pos); ///< 判断两个环是否共面并返回结果
}

/**
 * @brief 判断一条线段是否位于指定的环上
 * 
 * 该函数用于判断一条线段（SketchLinePtr）是否位于指定的环（由SketchPointPtr类型的点集表示）上。
 * 通过遍历环中的每一条线段，比较其key值与目标线段的key值来判断是否存在匹配的线段。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] line 需要判断的线段指针
 * @param[in] ring 包含SketchPointPtr类型的点集，表示需要检查的环
 * 
 * @return bool 如果线段位于环上，返回true；否则返回false
 * 
 * @note 函数内部通过遍历环中的每一条线段，使用geodata->line_key函数计算线段的key值，并与目标线段的key值进行比较。
 */
bool GeoDataUtils::is_line_on_ring(GeoData* geodata, SketchLinePtr line, const std::vector<SketchPointPtr>& ring)
{
    for(size_t i = 0; i < ring.size(); i++)
    {
        SketchPointPtr a = ring[i]; ///< 获取环中的当前点
        SketchPointPtr b = ring[(i + 1) % ring.size()]; ///< 获取环中的下一个点，使用模运算确保最后一个点与第一个点相连

        if(line->key == geodata->line_key(a, b)) ///< 计算当前线段的key值并与目标线段的key值进行比较
        {
            return true; ///< 如果key值匹配，返回true
        }
    }

    return false; ///< 如果遍历完所有线段都没有匹配的key值，返回false
}

/**
 * @brief 判断一个点是否位于指定的环上
 * 
 * 该函数用于判断一个点（SketchPointPtr）是否位于由一系列点构成的环（ring）上。
 * 函数通过遍历环中的每个点，并与目标点进行比较来确定目标点是否是环的一部分。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 需要检查的点指针
 * @param[in] ring 包含SketchPointPtr类型的点集，表示需要检查的环
 * 
 * @return bool 如果点位于环上，返回true；否则返回false
 * 
 * @note 当前实现存在逻辑错误，它将point参数与自身进行比较，而不是与环中的点进行比较。
 *       正确的实现应该比较传入的point参数和环中每一个点。
 */
bool GeoDataUtils::is_point_on_ring(GeoData* geodata, SketchPointPtr point, const std::vector<SketchPointPtr>& ring)
{
    // 遍历环中的所有点
    for(auto ring_point : ring) ///< 对于环中的每一个点
    {
        // 比较当前环中的点是否等于传入的目标点
        if(ring_point == point) ///< 如果找到相等的点，则说明目标点在环上
        {
            return true; ///< 返回true表示点在环上
        }
    }
    // 如果没有找到相等的点，返回false表示点不在环上
    return false;
}

/**
 * @brief 判断一个环是否位于目标路径内
 * 
 * 该函数用于判断一个环（ring）是否位于另一个环（target_ring）所定义的路径内。
 * 通过遍历环中的每一条线段，并使用拓扑关系计算来判断线段的起点和终点相对于目标环的位置关系。
 * 如果环中的某一段线段的起点在目标环的边界上，而终点在目标环的内部，则认为该环位于目标路径内。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] normal 环的法向量
 * @param[in] ring 需要检查的环，包含SketchPointPtr类型的点集
 * @param[in] target_ring 目标环，包含SketchPointPtr类型的点集
 * @param[out] result 输出的分割路径，包含多个点集
 * 
 * @return size_t 返回分割路径的数量
 * 
 * @note 函数内部通过遍历环中的每一条线段，计算线段起点和终点相对于目标环的位置关系，并根据位置关系判断环是否位于目标路径内。
 *       如果环中的某一段线段的起点在目标环的边界上，而终点在目标环的内部，则认为该环位于目标路径内，并将该段路径添加到结果中。
 */
bool GeoDataUtils::is_ring_in_target_path(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                          const std::vector<SketchPointPtr>& target_ring, std::vector<std::vector<SketchPointPtr>>& result)
{
    bool onPath = false; ///< 标记是否在路径上
    std::vector<SketchPointPtr> splitPath; ///< 存储分割路径
    SketchPointPtr firstPathBegin = nullptr; ///< 标记路径开始点

    size_t index = 0; ///< 当前线段索引
    while(true)
    {
        auto cp = ring[index % ring.size()]; ///< 获取当前点
        auto np = ring[(index + 1) % ring.size()]; ///< 获取下一个点
        ++index;

        if(cp == firstPathBegin) ///< 如果回到路径开始点，结束循环
        {
            break;
        }

        nous::vec3 cpp = geodata->get_point_position(cp); ///< 获取当前点的位置
        nous::vec3 npp = geodata->get_point_position(np); ///< 获取下一个点的位置

        std::vector<nous::pos_t> target_ring_positions; ///< 存储目标环的位置
        get_ring_positions(geodata, target_ring, target_ring_positions); ///< 获取目标环的位置

        topo::ESketchTopoRelation cps = topo::calc_point_and_ring_relation(normal, cpp, target_ring_positions); ///< 计算当前点与目标环的关系
        topo::ESketchTopoRelation nps = topo::calc_point_and_ring_relation(normal, npp, target_ring_positions); ///< 计算下一个点与目标环的关系

        if(onPath) ///< 如果当前在路径上
        {
            splitPath.push_back(cp); ///< 将当前点添加到分割路径中
            if(cps == topo::ESketchTopoRelation::OnBoundary) ///< 如果当前点在目标环的边界上
            {
                onPath = false; ///< 结束路径
                result.push_back(splitPath); ///< 将分割路径添加到结果中
            }
        }

        if(cps == topo::ESketchTopoRelation::OnBoundary && nps == topo::ESketchTopoRelation::Inside) ///< 如果当前点在目标环的边界上，下一个点在目标环的内部
        {
            onPath = true; ///< 开始路径
            splitPath.clear(); ///< 清空分割路径
            splitPath.push_back(cp); ///< 将当前点添加到分割路径中
            if(!firstPathBegin) ///< 如果路径开始点未设置
            {
                firstPathBegin = cp; ///< 设置路径开始点
            }
        }

        if(!firstPathBegin && index == ring.size()) ///< 如果路径开始点未设置且遍历完所有点
        {
            break; ///< 结束循环
        }
    }

    return result.size(); ///< 返回分割路径的数量
}

/**
 * @brief 判断一个环是否完全包含另一个环
 * 
 * 该函数用于判断一个环（ring）是否完全包含另一个环（target_ring）。
 * 通过遍历目标环中的每一条线段，检查该线段是否位于源环上。
 * 如果线段不在源环上，则进一步检查线段的中点相对于源环的位置关系。
 * 如果线段的中点位于源环的外部，则认为目标环不被源环完全包含。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] normal 环的法向量
 * @param[in] ring 源环，包含SketchPointPtr类型的点集
 * @param[in] target_ring 目标环，包含SketchPointPtr类型的点集
 * 
 * @return bool 如果目标环被源环完全包含，返回true；否则返回false
 * 
 * @note 函数内部通过遍历目标环中的每一条线段，首先检查线段是否位于源环上。
 *       如果线段不在源环上，则计算线段中点的位置，并使用拓扑关系计算判断中点是否位于源环的外部。
 *       如果中点位于源环的外部，则认为目标环不被源环完全包含，并返回false。
 *       如果所有线段的中点都不在源环的外部，则认为目标环被源环完全包含，并返回true。
 */
bool GeoDataUtils::is_ring_contain_target(GeoData* geodata, const nous::vec3& normal, const std::vector<SketchPointPtr>& ring,
                                          const std::vector<SketchPointPtr>& target_ring)
{
    for(int i = 0; i < target_ring.size(); i++)
    {
        auto cp = target_ring[i]; ///< 获取目标环中的当前点
        auto np = target_ring[(i + 1) % target_ring.size()]; ///< 获取目标环中的下一个点，使用模运算确保最后一个点与第一个点相连

        if(is_line_on_ring(geodata, geodata->m_key_lines.at(GeoData::line_key(cp, np)), ring))
        {
            continue; ///< 如果线段位于源环上，继续检查下一条线段
        }

        nous::vec3 cpp = geodata->get_point_position(cp); ///< 获取当前点的位置
        nous::vec3 npp = geodata->get_point_position(np); ///< 获取下一个点的位置

        std::vector<nous::pos_t> ring_positions = get_ring_positions(geodata, ring); ///< 获取源环的位置

        if(topo::calc_point_and_ring_relation(normal, cpp + (npp - cpp) * 0.5, ring_positions) == topo::ESketchTopoRelation::Outside)
        {
            return false; ///< 如果线段中点位于源环的外部，返回false
        }
    }
    return true; ///< 如果所有线段的中点都不在源环的外部，返回true
}

/**
 * @brief 获取指定环与其他环的关系
 * 
 * 该函数用于获取指定环（ring）与其他环的关系，包括被包含的环（contained）和包含的环集合（contains）。
 * 通过遍历与指定环相关的所有多边形，判断这些多边形的环是否与指定环共面。
 * 如果共面，则进一步判断这些环是否被指定环包含或包含指定环。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定环，包含SketchPointPtr类型的点集
 * @param[out] contained 输出的被包含的环指针
 * @param[out] contains 输出的包含的环集合，包含SketchRingPtr类型的指针
 * 
 * @note 函数内部首先同步几何数据，然后计算指定环的法向量。
 *       遍历与指定环相关的所有多边形，获取每个环的法向量和点集。
 *       如果两个环共面，则使用is_ring_contain_target函数判断环之间的包含关系。
 *       如果指定环包含其他环，则将这些环添加到contains集合中。
 *       如果其他环包含指定环，则更新superior_ring为包含指定环的环。
 *       最后，将superior_ring赋值给contained。
 */
void GeoDataUtils::get_ring_relation_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, SketchRingPtr& contained,
                                           std::unordered_set<SketchRingPtr>& contains)
{
    geodata->_sync_nous(); ///< 同步几何数据

    std::vector<nous::vec3> vp;
    for(auto np: ring)
    {
        vp.emplace_back(std::move(geodata->get_point_position(np))); ///< 获取每个点的位置并添加到vp向量中
    }
    nous::vec3 c_normal = sketch_math::ring_normal(vp); ///< 计算指定环的法向量

    size_t ring_key = GeoData::ring_key(ring); ///< 获取指定环的key
    SketchRingPtr superior_ring = nullptr; ///< 初始化被包含的环指针
    std::vector<SketchPointPtr> superior_ring_topo; ///< 初始化被包含的环的拓扑结构

    for(auto& ring_polygon_pair: geodata->get_ring_relation_polygons())
    {
        SketchRingPtr target_ring = ring_polygon_pair.first; ///< 获取目标环
        std::vector<SketchPointPtr> target_ring_topo = geodata->get_element_points(target_ring); ///< 获取目标环的拓扑结构
        size_t target_ring_key = GeoData::ring_key(target_ring_topo); ///< 获取目标环的key

        if(target_ring_key == ring_key)
        {
            continue; ///< 如果目标环与指定环相同，跳过
        }

        nous::vec3 t_normal = target_ring->normal; ///< 获取目标环的法向量
        bool coplane = sketch_math::plane_coplane_plane(c_normal, geodata->get_point_position(ring[0]), t_normal,
                                                        geodata->get_point_position(target_ring_topo[0])); ///< 判断两个环是否共面

        if(coplan)
        {
            // contain&contained
            if(is_ring_contain_target(geodata, t_normal, ring, target_ring_topo))
            {
                // the rings contained by input ring.
                contains.insert(target_ring); ///< 如果指定环包含目标环，将目标环添加到contains集合中
            }
            else if(is_ring_contain_target(geodata, t_normal, target_ring_topo, ring))
            {
                // the only ring smaller contain the input ring.
                if(superior_ring)
                {
                    if(is_ring_contain_target(geodata, t_normal, superior_ring_topo, target_ring_topo))
                    {
                        superior_ring = target_ring; ///< 更新被包含的环指针
                        superior_ring_topo = target_ring_topo; ///< 更新被包含的环的拓扑结构
                    }
                }
                else
                {
                    superior_ring = target_ring; ///< 初始化被包含的环指针
                    superior_ring_topo = target_ring_topo; ///< 初始化被包含的环的拓扑结构
                }
            }
        }
    }

    contained = superior_ring; ///< 将被包含的环指针赋值给contained
}

/**
 * @brief 获取指定环与其他环的关系
 * 
 * 该函数用于获取指定环（ring）与其他环的关系，包括被包含的环（contained）和包含的环集合（contains）。
 * 通过调用重载函数get_ring_relation_rings，将指定环的拓扑结构转换为点集，然后进行关系判断。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定环的指针
 * @param[out] contained 输出的被包含的环指针
 * @param[out] contains 输出的包含的环集合，包含SketchRingPtr类型的指针
 * 
 * @note 函数内部首先调用get_ring_topo函数获取指定环的拓扑结构（点集）。
 *       然后调用重载的get_ring_relation_rings函数，传入点集进行关系判断。
 */
void GeoDataUtils::get_ring_relation_rings(GeoData* geodata, SketchRingPtr ring, SketchRingPtr contained, std::unordered_set<SketchRingPtr>& contains)
{
    std::vector<SketchPointPtr> ring_points = get_ring_topo(geodata, ring); ///< 获取指定环的拓扑结构（点集）
    get_ring_relation_rings(geodata, ring_points, contained, contains); ///< 调用重载函数进行关系判断
}

/**
 * @brief 获取包含所有指定线段的几何原语集合
 * 
 * 该函数用于获取包含所有指定线段（lines）的几何原语（SketchRingPtr）集合。
 * 通过遍历几何数据中的所有几何原语，检查每个几何原语是否包含所有指定的线段。
 * 如果某个几何原语包含所有指定的线段，则将其添加到输出集合（prim_set）中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] lines 需要检查的线段集合，包含SketchLinePtr类型的指针
 * @param[out] prim_set 输出的几何原语集合，包含SketchRingPtr类型的指针
 * 
 * @note 函数内部遍历几何数据中的所有几何原语，获取每个几何原语相邻的线段集合。
 *       如果某个几何原语的相邻线段集合包含所有指定的线段，则将该几何原语添加到输出集合中。
 */
void GeoDataUtils::get_prims_contains_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines,
                                                std::unordered_set<SketchRingPtr>& prim_set)
{
    for(const auto& pm_idx: geodata->get_mesh()->prim_index_range()) ///< 遍历几何数据中的所有几何原语
    {
        SketchRingPtr curr_ring = geodata->m_nousptr_rings.at(geodata->get_mesh()->prim(pm_idx)); ///< 获取当前几何原语
        if(curr_ring) ///< 如果当前几何原语存在
        {
            const std::unordered_set<SketchLinePtr> ring_adj_lines = geodata->get_ring_adj_lines(curr_ring); ///< 获取当前几何原语相邻的线段集合
            if(!ring_adj_lines.empty()) ///< 如果相邻线段集合不为空
            {
                bool all_contain = true; ///< 标记是否包含所有指定线段
                for(auto line: lines) ///< 遍历所有指定线段
                {
                    if(!ring_adj_lines.count(line)) ///< 如果当前几何原语不包含某个指定线段
                    {
                        all_contain = false; ///< 设置标记为false
                        break; ///< 退出循环
                    }
                }

                if(all_contain) ///< 如果当前几何原语包含所有指定线段
                {
                    prim_set.insert(curr_ring); ///< 将当前几何原语添加到输出集合中
                }
            }
        }
    }
}

/**
 * @brief 获取包含所有指定线段的多边形集合
 * 
 * 该函数用于获取包含所有指定线段（lines）的多边形（SketchPolygonPtr）集合。
 * 通过遍历几何数据中的所有几何原语，检查每个几何原语是否包含所有指定的线段。
 * 如果某个几何原语包含所有指定的线段，则将其对应的多边形添加到输出集合（result）中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] lines 需要检查的线段集合，包含SketchLinePtr类型的指针
 * @param[out] result 输出的多边形集合，包含SketchPolygonPtr类型的指针
 * 
 * @note 函数内部遍历几何数据中的所有几何原语，获取每个几何原语相邻的线段集合。
 *       如果某个几何原语的相邻线段集合包含所有指定的线段，则将该几何原语对应的多边形添加到输出集合中。
 */
void GeoDataUtils::get_polygons_contained_by_all_lines(GeoData* geodata, const std::unordered_set<SketchLinePtr>& lines,
                                                       std::unordered_set<SketchPolygonPtr>& result)
{
    for(const auto& pm_idx: geodata->get_mesh()->prim_index_range()) ///< 遍历几何数据中的所有几何原语
    {
        nous::Prim* ele_prim = geodata->get_mesh()->prim(pm_idx); ///< 获取当前几何原语
        if(ele_prim && !geodata->is_line(ele_prim)) ///< 如果当前几何原语存在且不是线段
        {
            SketchRingPtr curr_prim = geodata->m_nousptr_rings.at(ele_prim); ///< 获取当前几何原语对应的环
            const std::unordered_set<SketchLinePtr> ring_adj_lines = geodata->get_ring_adj_lines(curr_prim); ///< 获取当前环相邻的线段集合
            if(!ring_adj_lines.empty()) ///< 如果相邻线段集合不为空
            {
                bool all_contain = true; ///< 标记是否包含所有指定线段
                for(auto line: ring_adj_lines) ///< 遍历当前环的所有相邻线段
                {
                    if(lines.find(line) == lines.end()) ///< 如果指定线段集合中不包含当前相邻线段
                    {
                        all_contain = false; ///< 设置标记为false
                        break; ///< 退出循环
                    }
                }

                if(all_contain) ///< 如果当前环包含所有指定线段
                {
                    SketchPolygonPtr skp = geodata->get_polygon(curr_prim); ///< 获取当前环对应的多边形
                    if(skp) ///< 如果多边形存在
                    {
                        result.insert(skp); ///< 将当前多边形添加到输出集合中
                    }
                }
            }
        }
    }
}

/**
 * @brief 获取指定多边形相邻的线段集合
 * 
 * 该函数用于获取指定多边形（polygon）相邻的所有线段（SketchLinePtr）集合。
 * 通过调用重载函数get_polygon_adj_lines(GeoData*, SketchPolygonPtr, std::unordered_set<SketchLinePtr>&)实现具体功能，并将结果返回。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * 
 * @return std::unordered_set<SketchLinePtr> 返回包含指定多边形相邻线段的集合
 * 
 * @note 函数内部实际工作由重载函数get_polygon_adj_lines(GeoData*, SketchPolygonPtr, std::unordered_set<SketchLinePtr>&)完成
 */
std::unordered_set<SketchLinePtr> GeoDataUtils::get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::unordered_set<SketchLinePtr> result; ///< 存储结果的线段集合
    get_polygon_adj_lines(geodata, polygon, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含指定多边形相邻线段的集合
}

/**
 * @brief 获取指定多边形相邻的线段集合
 * 
 * 该函数用于获取指定多边形（polygon）相邻的所有线段（SketchLinePtr）集合，并将结果存储在输出参数result中。
 * 通过遍历多边形的外环和所有内环，获取每条线段的两个端点，计算线段的key值，并从geodata的m_key_lines中获取对应的线段指针，最后将其插入到result集合中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * @param[out] result 输出的线段集合，包含指定多边形相邻的所有线段
 * 
 * @note 函数内部首先获取多边形的外环点集，然后遍历这些点，计算每条线段的key值，并从geodata的m_key_lines中获取对应的线段指针，插入到result集合中。
 *       接着遍历多边形的每个内环，重复上述过程，确保所有相邻线段都被添加到result集合中。
 */
void GeoDataUtils::get_polygon_adj_lines(GeoData* geodata, SketchPolygonPtr polygon, std::unordered_set<SketchLinePtr>& result)
{
    std::vector<SketchPointPtr> ring_points;
    geodata->get_element_points(polygon->ring, ring_points); ///< 获取多边形外环的点集
    for(int i = 0; i < ring_points.size(); ++i)
    {
        SketchPointPtr a = ring_points[i]; ///< 获取当前点
        SketchPointPtr b = ring_points[(i + 1) % ring_points.size()]; ///< 获取下一个点，使用模运算确保最后一个点与第一个点相连
        size_t line_key = geodata->line_key(a, b); ///< 计算当前线段的key值
        result.insert(geodata->m_key_lines.at(line_key)); ///< 从geodata的m_key_lines中获取对应的线段指针，并插入到result集合中
    }

    for(const auto& inner_ring: polygon->inner_rings) ///< 遍历多边形的每个内环
    {
        std::vector<SketchPointPtr> inner_ring_points;
        geodata->get_element_points(inner_ring, inner_ring_points); ///< 获取内环的点集
        for(int i = 0; i < inner_ring_points.size(); ++i)
        {
            SketchPointPtr a = inner_ring_points[i]; ///< 获取当前点
            SketchPointPtr b = inner_ring_points[(i + 1) % inner_ring_points.size()]; ///< 获取下一个点，使用模运算确保最后一个点与第一个点相连
            size_t line_key = geodata->line_key(a, b); ///< 计算当前线段的key值
            result.insert(geodata->m_key_lines.at(line_key)); ///< 从geodata的m_key_lines中获取对应的线段指针，并插入到result集合中
        }
    }
}

/**
 * @brief 获取指定环相邻的多边形集合
 * 
 * 该函数用于获取指定环（ring）相邻的所有多边形（SketchPolygonPtr）集合，并将结果返回。
 * 通过调用重载函数get_ring_adj_polygons(GeoData*, SketchRingPtr, std::unordered_set<SketchPolygonPtr>&)实现具体功能。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定的环对象指针
 * 
 * @return std::unordered_set<SketchPolygonPtr> 返回包含指定环相邻多边形的集合
 * 
 * @note 函数内部实际工作由重载函数get_ring_adj_polygons(GeoData*, SketchRingPtr, std::unordered_set<SketchPolygonPtr>&)完成
 */
std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring)
{
    std::unordered_set<SketchPolygonPtr> result; ///< 存储结果的多边形集合
    get_ring_adj_polygons(geodata, ring, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含指定环相邻多边形的集合
}

/**
 * @brief 获取指定环相邻的多边形集合
 * 
 * 该函数用于获取指定环（ring）相邻的所有多边形（SketchPolygonPtr）集合，并将结果存储在输出参数result中。
 * 通过调用GeoData对象的方法获取环所属的多边形及其父多边形，并将它们添加到结果集合中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定的环对象指针
 * @param[out] result 输出的多边形集合，包含SketchPolygonPtr类型的指针
 * 
 * @note 函数内部首先通过geodata->get_polygon(ring)获取环所属的多边形，并将其添加到结果集合中。
 *       然后通过geodata->get_parent_polygon(ring)获取环的父多边形，并将其添加到结果集合中。
 */
void GeoDataUtils::get_ring_adj_polygons(GeoData* geodata, SketchRingPtr ring, std::unordered_set<SketchPolygonPtr>& result)
{
    SketchPolygonPtr skp = geodata->get_polygon(ring); ///< 获取环所属的多边形
    if(skp)
    {
        result.insert(skp); ///< 如果多边形存在，则将其添加到结果集合中
    }
    SketchPolygonPtr skp_p = geodata->get_parent_polygon(ring); ///< 获取环的父多边形
    if(skp_p)
    {
        result.insert(skp_p); ///< 如果父多边形存在，则将其添加到结果集合中
    }
}

/**
 * @brief 获取指定线段相邻的多边形集合
 * 
 * 该函数用于获取指定线段（line）相邻的所有多边形（SketchPolygonPtr）集合，并将结果返回。
 * 通过调用重载函数get_line_adj_polygons(GeoData*, SketchLinePtr, std::unordered_set<SketchPolygonPtr>&)实现具体功能。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] line 指定的线段对象指针
 * 
 * @return std::unordered_set<SketchPolygonPtr> 返回包含指定线段相邻多边形的集合
 * 
 * @note 函数内部实际工作由重载函数get_line_adj_polygons(GeoData*, SketchLinePtr, std::unordered_set<SketchPolygonPtr>&)完成
 */
std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_line_adj_polygons(GeoData* geodata, SketchLinePtr line)
{
    std::unordered_set<SketchPolygonPtr> result; ///< 存储结果的多边形集合
    get_line_adj_polygons(geodata, line, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含指定线段相邻多边形的集合
}

/**
 * @brief 获取指定线段相邻的多边形集合
 * 
 * 该函数用于获取指定线段（line）相邻的所有多边形（SketchPolygonPtr）集合，并将结果存储在输出参数result中。
 * 通过调用GeoData对象的get_line_adj_rings方法获取线段相邻的环集合，然后遍历每个环，调用get_ring_adj_polygons方法获取每个环相邻的多边形集合，最后将这些多边形添加到结果集合中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] line 指定的线段对象指针
 * @param[out] result 输出的多边形集合，包含SketchPolygonPtr类型的指针
 * 
 * @note 函数内部首先通过geodata->get_line_adj_rings(line)获取线段相邻的环集合。
 *       然后遍历每个环，调用get_ring_adj_polygons(geodata, ring)获取每个环相邻的多边形集合。
 *       最后，将这些多边形添加到结果集合result中。
 */
void GeoDataUtils::get_line_adj_polygons(GeoData* geodata, SketchLinePtr line, std::unordered_set<SketchPolygonPtr>& result)
{
    std::unordered_set<SketchRingPtr> rings = geodata->get_line_adj_rings(line); ///< 获取线段相邻的环集合
    for(auto ring: rings) ///< 遍历每个环
    {
        std::unordered_set<SketchPolygonPtr> ring_adj_polygons = get_ring_adj_polygons(geodata, ring); ///< 获取环相邻的多边形集合
        result.insert(ring_adj_polygons.begin(), ring_adj_polygons.end()); ///< 将多边形添加到结果集合中
    }
}

/**
 * @brief 获取指定点相邻的多边形集合
 * 
 * 该函数用于获取指定点（point）相邻的所有多边形（SketchPolygonPtr）集合，并将结果返回。
 * 通过调用重载函数get_point_adj_polygons(GeoData*, SketchPointPtr, std::unordered_set<SketchPolygonPtr>&)实现具体功能，并将结果返回。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 指定点对象指针
 * 
 * @return std::unordered_set<SketchPolygonPtr> 返回包含指定点相邻多边形的集合
 * 
 * @note 函数内部首先定义一个结果集合result。
 *       然后调用重载的get_point_adj_polygons函数，传入点和结果集合进行关系判断。
 *       最后，返回结果集合result。
 */
std::unordered_set<SketchPolygonPtr> GeoDataUtils::get_point_adj_polygons(GeoData* geodata, SketchPointPtr point)
{
    std::unordered_set<SketchPolygonPtr> result; ///< 存储结果的多边形集合
    get_point_adj_polygons(geodata, point, result); ///< 调用重载函数填充结果
    return result; ///< 返回包含指定点相邻多边形的集合
}

/**
 * @brief 获取指定点相邻的多边形集合
 * 
 * 该函数用于获取指定点（point）相邻的所有多边形（SketchPolygonPtr）集合，并将结果存储在输出参数result中。
 * 通过调用GeoData对象的get_point_adj_rings方法获取点相邻的环集合，然后遍历每个环，调用get_ring_adj_polygons方法获取每个环相邻的多边形集合，最后将这些多边形添加到结果集合中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 指定点对象指针
 * @param[out] result 输出的多边形集合，包含SketchPolygonPtr类型的指针
 * 
 * @note 函数内部首先通过geodata->get_point_adj_rings(point)获取点相邻的环集合。
 *       然后遍历每个环，调用get_ring_adj_polygons(geodata, ring)获取每个环相邻的多边形集合。
 *       最后，将这些多边形添加到结果集合result中。
 */
void GeoDataUtils::get_point_adj_polygons(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPolygonPtr>& result)
{
    std::unordered_set<SketchRingPtr> rings = geodata->get_point_adj_rings(point); ///< 获取点相邻的环集合
    for(auto ring: rings) ///< 遍历每个环
    {
        std::unordered_set<SketchPolygonPtr> ring_adj_polygons = get_ring_adj_polygons(geodata, ring); ///< 获取环相邻的多边形集合
        result.insert(ring_adj_polygons.begin(), ring_adj_polygons.end()); ///< 将多边形添加到结果集合中
    }
}

/**
 * @brief 获取指定点相邻的线段集合
 * 
 * 该函数用于获取指定点（point）相邻的所有线段（SketchLinePtr）集合，并将结果返回。
 * 通过调用重载函数get_point_adj_lines(GeoData*, SketchPointPtr, std::unordered_set<SketchLinePtr>&)实现具体功能。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 指定点对象指针
 * 
 * @return std::unordered_set<SketchLinePtr> 返回包含指定点相邻线段的集合
 * 
 * @note 函数内部首先定义一个空的线段集合adj_lines。
 *       然后调用重载的get_point_adj_lines函数，传入geodata、point和adj_lines，填充adj_lines。
 *       最后，返回adj_lines集合。
 */
std::unordered_set<SketchLinePtr> GeoDataUtils::get_point_adj_lines(GeoData* geodata, SketchPointPtr point)
{
    std::unordered_set<SketchLinePtr> adj_lines; ///< 定义一个空的线段集合
    get_point_adj_lines(geodata, point, adj_lines); ///< 调用重载函数填充线段集合
    return adj_lines; ///< 返回包含指定点相邻线段的集合
}

/**
 * @brief 获取指定点相邻的线段集合
 * 
 * 该函数用于获取指定点（point）相邻的所有线段（SketchLinePtr）集合，并将结果存储在输出参数result中。
 * 通过调用GeoData对象的get_point_adj_lines方法实现具体功能。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 指定点对象指针
 * @param[out] result 输出的线段集合，包含指定点相邻的所有线段
 * 
 * @note 函数内部通过调用geodata->get_point_adj_lines(point, result)来填充result集合。
 */
void GeoDataUtils::get_point_adj_lines(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchLinePtr>& result)
{
    geodata->get_point_adj_lines(point, result); ///< 调用GeoData的方法填充结果集合
}

/**
 * @brief 将多边形转换为相对点索引
 * 
 * 该函数用于将指定多边形（polygon）转换为相对点索引，并将结果存储在输出参数result中。
 * 同时，将点与索引的映射关系存储在输出参数id_point_map中。
 * 通过遍历多边形的外环和所有内环，为每个点分配一个唯一的索引，并将这些索引存储在结果中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * @param[out] id_point_map 输出的点与索引的映射关系数组
 * 
 * @return std::vector<std::vector<size_t>> 返回包含多边形所有环相对点索引的二维数组
 * 
 * @note 函数内部首先获取多边形的外环点集，并为每个点分配一个唯一的索引，存储在外环的相对点索引数组中。
 *       然后遍历多边形的每个内环，重复上述过程，为每个内环的点分配索引，并存储在对应的内环相对点索引数组中。
 *       最后，将点与索引的映射关系存储在id_point_map中。
 */
std::vector<std::vector<size_t>> GeoDataUtils::convert_polygon_to_rel_point_idx(GeoData* geodata, SketchPolygonPtr polygon,
                                                                                std::vector<SketchPointPtr>& id_point_map)
{
    std::vector<std::vector<size_t>> result; ///< 存储结果的二维数组
    std::vector<SketchPointPtr> ring_pt_arr; ///< 存储当前环的点集
    geodata->get_element_points(polygon->ring, ring_pt_arr); ///< 获取多边形外环的点集
    std::map<SketchPointPtr, size_t> point_id_map; ///< 存储点与索引的映射关系
    std::vector<size_t>& exring_rel_idx_arr = result.emplace_back(); ///< 为外环创建一个新的相对点索引数组
    for(int i = 0; i < ring_pt_arr.size(); ++i)
    {
        if(point_id_map.find(ring_pt_arr[i]) == point_id_map.end()) ///< 如果点没有分配索引
        {
            point_id_map.insert({ ring_pt_arr[i], point_id_map.size() }); ///< 分配一个新的索引
        }
        exring_rel_idx_arr.emplace_back(point_id_map[ring_pt_arr[i]]); ///< 将点的索引添加到外环相对点索引数组中
    }

    for(auto& inring: polygon->inner_rings) ///< 遍历多边形的每个内环
    {
        ring_pt_arr.clear(); ///< 清空当前环的点集
        geodata->get_element_points(inring, ring_pt_arr); ///< 获取内环的点集
        std::vector<size_t>& inring_rel_idx_arr = result.emplace_back(); ///< 为内环创建一个新的相对点索引数组
        for(int i = 0; i < ring_pt_arr.size(); ++i)
        {
            if(point_id_map.find(ring_pt_arr[i]) == point_id_map.end()) ///< 如果点没有分配索引
            {
                point_id_map.insert({ ring_pt_arr[i], point_id_map.size() }); ///< 分配一个新的索引
            }
            inring_rel_idx_arr.emplace_back(point_id_map[ring_pt_arr[i]]); ///< 将点的索引添加到内环相对点索引数组中
        }
    }

    id_point_map.resize(point_id_map.size()); ///< 调整id_point_map的大小以匹配点的数量
    std::for_each(std::execution::par, point_id_map.begin(), point_id_map.end(), [&](auto& pair) { id_point_map[pair.second] = pair.first; }); ///< 将点与索引的映射关系存储在id_point_map中

    return result; ///< 返回包含多边形所有环相对点索引的二维数组
}

// add
// 检测目标多边形是否完全被另一个多边形包含
bool GeoDataUtils::is_polygon_contained(GeoData* geodata, const std::vector<std::vector<nous::vec3>>& target_polygon,
                                        SketchPolygonPtr container_polygon)
{
    std::vector<std::vector<nous::vec3>> container_positions;
    get_polygon_positions(geodata, container_polygon, container_positions);
    // 顶点包含检测（需要完善代码）
    for (auto& pt_ring : target_polygon)
    {
        for (auto& pt : pt_ring)
        {
            if (!is_point_inside_polygon(pt, container_positions))
            {
                return false;
            }
        }
    }
    // 边界交叉检测（需要完善代码）
    if (!do_polygons_intersect(target_polygon, container_positions))
    {
        return false;
    }
    return true;
}
// add

/**
 * @brief 获取GeoDataTopoOperations的单例实例
 * 
 * 该函数用于获取GeoDataTopoOperations类的单例实例。
 * 通过静态局部变量_instance实现单例模式，确保在整个程序运行期间只有一个GeoDataTopoOperations实例存在。
 * 
 * @return GeoDataTopoOperations& 返回GeoDataTopoOperations的引用
 * 
 * @note 使用静态局部变量_instance来实现单例模式，确保线程安全（C++11及以后的标准保证了静态局部变量的线程安全初始化）。
 */
GeoDataTopoOperations& GeoDataTopoOperations::get()
{
    static GeoDataTopoOperations _instance; ///< 静态局部变量，确保单例实例
    return _instance; ///< 返回单例实例的引用
}

/**
 * @brief 合并多边形三角形
 * 
 * 该函数用于合并一组多边形三角形（SketchPolygonTriangle），并将合并后的结果存储在输出参数topos中。
 * 通过计算每个三角形的法向量，并将共面的三角形分组，然后使用geos_util::union_all_rings函数对每个共面组进行合并。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] vp 多边形三角形集合，包含SketchPolygonTriangle类型的三角形
 * @param[out] topos 输出的合并后的多边形拓扑结构，包含多个点集
 * @param[in] update_sketch_trangle 是否更新三角形的顶点位置
 * 
 * @note 函数内部首先定义一个映射plane_coplane_plane，用于存储共面三角形组。
 *       遍历每个三角形，如果update_sketch_trangle为true，则更新三角形顶点的位置。
 *       计算每个三角形的法向量，并检查该三角形是否与已有的共面三角形组共面。
 *       如果共面，则将三角形添加到对应的共面三角形组中；否则，创建一个新的共面三角形组。
 *       最后，对每个共面三角形组调用geos_util::union_all_rings函数进行合并，并将结果存储在topos中。
 */
void GeoDataTopoOperations::merge_polygon_triangle(GeoData* geodata, std::vector<SketchPolygonTriangle>& vp,
                                                   std::vector<std::vector<std::vector<nous::vec3>>>& topos, bool update_sketch_trangle)
{
    std::unordered_map<nous::vec3, std::vector<std::vector<nous::vec3>>> plane_coplane_plane; ///< 存储共面三角形组的映射

    for(auto& tri: vp)
    {
        if(update_sketch_trangle)
        {
            if(tri[0].first)
            {
                tri[0].second = geodata->get_point_position(tri[0].first); ///< 更新第一个顶点的位置
            }
            if(tri[1].first)
            {
                tri[1].second = geodata->get_point_position(tri[1].first); ///< 更新第二个顶点的位置
            }
            if(tri[2].first)
            {
                tri[2].second = geodata->get_point_position(tri[2].first); ///< 更新第三个顶点的位置
            }
        }
        std::vector<nous::vec3> triData; ///< 存储当前三角形的顶点位置
        triData.push_back(tri[0].second);
        triData.push_back(tri[1].second);
        triData.push_back(tri[2].second);
        nous::vec3 normal = sketch_math::ring_normal(triData); ///< 计算当前三角形的法向量
        bool find = false;
        for(auto& plane: plane_coplane_plane)
        {
            if(sketch_math::plane_coplane_plane(plane.first, plane.second[0][0], normal, triData[0])) ///< 检查当前三角形是否与已有的共面三角形组共面
            {
                plane.second.emplace_back(std::move(triData)); ///< 将当前三角形添加到对应的共面三角形组中
                find = true;
                break;
            }
        }
        if(!find)
        {
            plane_coplane_plane[normal].emplace_back(std::move(triData)); ///< 创建一个新的共面三角形组
        }
    }

    for(auto& plane: plane_coplane_plane)
    {
        geos_util::union_all_rings(plane.second, plane.first, topos); ///< 对每个共面三角形组进行合并，并将结果存储在topos中
    }
}

/**
 * @brief 合并两个环
 * 
 * 该函数用于合并两个指定的环（SketchRingPtr），并将合并后的结果作为一个新的环返回。
 * 如果两个环不属于同一个多边形，则返回nullptr。
 * 合并过程中，会更新几何数据中的环和多边形关系，并根据需要删除原始环。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring0 第一个环的指针
 * @param[in] ring1 第二个环的指针
 * @param[in] delete_org_rings 是否删除原始环
 * 
 * @return SketchRingPtr 合并后的新环指针，如果无法合并则返回nullptr
 * 
 * @note 函数内部首先检查两个环是否属于同一个多边形。
 *       如果不属于，则返回nullptr。
 *       获取两个环的点集，并调用geodata_topo::ring_union_other_rings函数进行合并。
 *       合并后的结果存储在一个新的环中，并更新几何数据中的环和多边形关系。
 *       如果delete_org_rings为true，则删除原始环。
 */
SketchRingPtr GeoDataTopoOperations::merge_ring(GeoData* geodata, SketchRingPtr ring0, SketchRingPtr ring1, bool delete_org_rings)
{
    if(geodata->get_parent_polygon(ring0) != geodata->get_parent_polygon(ring1))
    {
        return nullptr; ///< 如果两个环不属于同一个多边形，返回nullptr
    }
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(ring0); ///< 获取两个环共同的父多边形

    std::vector<SketchPointPtr> ring0_pos; ///< 存储第一个环的点集
    geodata->get_element_points(ring0, ring0_pos); ///< 获取第一个环的点集
    std::vector<SketchPointPtr> ring1_pos; ///< 存储第二个环的点集
    geodata->get_element_points(ring1, ring1_pos); ///< 获取第二个环的点集

    std::vector<std::vector<std::vector<SketchPointPtr>>> ring_union_result; ///< 存储合并后的环结果
    geodata_topo::ring_union_other_rings(geodata, ring0_pos, std::vector<std::vector<SketchPointPtr>>{ ring1_pos }, ring0->normal, ring_union_result); ///< 合并两个环

    // 约定情况下必然只有一个环生成
    std::vector<std::vector<SketchPointPtr>> polygon_edge; ///< 存储合并后的环的点集
    polygon_edge.emplace_back(std::move(ring_union_result[0][0])); ///< 将合并后的环的点集移动到polygon_edge中
    SketchRingPtr new_ring = geodata->_add_nous_ring(polygon_edge[0]); ///< 创建一个新的环

    geodata->set_ring_parent_polygon(new_ring, parent_skp); ///< 设置新环的父多边形
    geodata->set_ring_parent_polygon(ring0, nullptr); ///< 清除第一个环的父多边形关系
    geodata->set_ring_parent_polygon(ring1, nullptr); ///< 清除第二个环的父多边形关系

    if(parent_skp)
    {
        parent_skp->inner_rings.insert(new_ring); ///< 将新环添加到父多边形的内环集合中
        parent_skp->inner_rings.erase(ring0); ///< 从父多边形的内环集合中移除第一个环
        parent_skp->inner_rings.erase(ring1); ///< 从父多边形的内环集合中移除第二个环
        parent_skp->m_changed = true; ///< 标记父多边形已更改
    }
    if(delete_org_rings)
    {
        geodata->_delete_nous_ring(ring0); ///< 删除第一个环
        geodata->_delete_nous_ring(ring1); ///< 删除第二个环
    }

    return new_ring; ///< 返回合并后的新环
}

/**
 * @brief 合并两个多边形
 * 
 * 该函数用于合并两个指定的多边形（SketchPolygonPtr），并将合并后的结果作为一个或多个新的多边形返回。
 * 如果两个多边形不属于同一个父多边形，则返回。
 * 合并过程中，会更新几何数据中的多边形和环关系，并根据需要删除原始多边形。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon0 第一个多边形的指针
 * @param[in] polygon1 第二个多边形的指针
 * @param[in] delete_org_poly 是否删除原始多边形
 * 
 * @note 函数内部首先检查两个多边形是否属于同一个父多边形。
 *       如果不属于，则返回。
 *       获取两个多边形的拓扑结构，并调用geodata_topo::polygon_union函数进行合并。
 *       合并后的结果存储在一个或多个新的多边形中，并更新几何数据中的多边形和环关系。
 *       如果两个多边形属于同一个平滑组，则将它们的平滑组合并。
 *       如果delete_org_poly为true，则删除原始多边形。
 */
void GeoDataTopoOperations::merge_polygon(GeoData* geodata, SketchPolygonPtr polygon0, SketchPolygonPtr polygon1, bool delete_org_poly)
{
    // 拥有相同上一级可合并
    if(geodata->get_parent_polygon(polygon0->ring) != geodata->get_parent_polygon(polygon1->ring))
    {
        return; ///< 如果两个多边形不属于同一个父多边形，返回
    }

    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon0->ring); ///< 获取两个多边形共同的父多边形

    std::vector<std::vector<SketchPointPtr>> polygon0_topo; ///< 存储第一个多边形的拓扑结构
    geodata_topo::get_polygon_topo(geodata, polygon0, polygon0_topo); ///< 获取第一个多边形的拓扑结构

    std::vector<std::vector<SketchPointPtr>> polygon1_topo; ///< 存储第二个多边形的拓扑结构
    geodata_topo::get_polygon_topo(geodata, polygon1, polygon1_topo); ///< 获取第二个多边形的拓扑结构

    std::vector<std::vector<std::vector<SketchPointPtr>>> result; ///< 存储合并后的多边形拓扑结构
    geodata_topo::polygon_union(geodata, polygon0_topo, polygon1_topo, polygon0->ring->normal, result); ///< 合并两个多边形

    // merge smooth group
    if(polygon0->group() && polygon1->group())
    {
        if(polygon0->group() != polygon1->group())
        {
            polygon0->group()->merge(polygon1->group()); ///< 如果两个多边形属于不同的平滑组，则合并它们的平滑组
        }
    }

    if(delete_org_poly)
    {
        geodata->_delete_polygon(polygon0); ///< 删除第一个多边形
        if(geodata->get_parent_polygon(polygon0->ring))
        {
            // 带有父节点的环需要手动删除
            geodata->_delete_nous_ring(polygon0->ring); ///< 删除第一个多边形的环
        }
        geodata->_delete_polygon(polygon1); ///< 删除第二个多边形
        if(geodata->get_parent_polygon(polygon1->ring))
        {
            // 带有父节点的环需要手动删除
            geodata->_delete_nous_ring(polygon1->ring); ///< 删除第二个多边形的环
        }
    }

    for(auto& polygon_topo: result)
    {
        SketchPolygonPtr new_polygon = geodata->add_polygon_without_check(polygon_topo); ///< 创建一个新的多边形
        if(parent_skp)
        {
            parent_skp->inner_rings.insert(new_polygon->ring); ///< 将新多边形的环添加到父多边形的内环集合中
            geodata->set_ring_parent_polygon(new_polygon->ring, parent_skp); ///< 设置新多边形的环的父多边形
        }
    }
}

/**
 * @brief 验证并修复多边形拓扑结构
 * 
 * 该函数用于验证并修复指定的多边形（SketchPolygonPtr）的拓扑结构。
 * 如果多边形的外环被其内环完全包含，则删除该多边形。
 * 否则，使用外环与内环的差异计算（difference2D）来修复多边形的拓扑结构。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 需要验证和修复的多边形对象指针
 * @param[in] destory_topo 是否销毁拓扑结构（该参数在当前实现中未被使用）
 * 
 * @note 函数内部首先检查多边形是否存在。
 *       如果不存在，则返回。
 *       标记多边形为已更改，并获取多边形的外环拓扑结构。
 *       获取多边形的所有内环拓扑结构，并存储在inner_ring_topos中。
 *       如果内环拓扑结构不为空，则进行外环与内环的差异计算。
 *       如果差异计算没有结果，说明有内环完全包含了外环，删除该多边形。
 *       如果差异计算的结果边界一致，说明无需额外处理，直接返回。
 *       否则，将差异计算的结果作为新的外环，并调用replace_polygon_equal_or_smaller函数替换多边形。
 */
void GeoDataTopoOperations::verify_sketch_polygon(GeoData* geodata, SketchPolygonPtr polygon, const bool destory_topo)
{
    if(!polygon)
    {
        return; ///< 如果多边形不存在，返回
    }

    polygon->m_changed = true; ///< 标记多边形为已更改
    SketchRingPtr ring = polygon->ring; ///< 获取多边形的外环

    std::vector<SketchPointPtr> current_ring_topo; ///< 存储外环的拓扑结构
    geodata->get_element_points(ring, current_ring_topo); ///< 获取外环的点集

    std::unordered_map<size_t, SketchRingPtr> inner_ring_keys; ///< 存储内环的key与内环指针的映射关系
    std::vector<std::vector<SketchPointPtr>> inner_ring_topos; ///< 存储所有内环的拓扑结构
    for(auto inner: polygon->inner_rings) ///< 遍历多边形的所有内环
    {
        std::vector<SketchPointPtr>& inner_ring_topo = inner_ring_topos.emplace_back(); ///< 为当前内环创建一个新的拓扑结构数组
        geodata->get_element_points(inner, inner_ring_topo); ///< 获取当前内环的点集
        inner_ring_keys[GeoData::ring_key(inner_ring_topo)] = inner; ///< 将内环的key与内环指针存储在映射中
    }

    if(!inner_ring_topos.empty()) ///< 如果内环拓扑结构不为空
    {
        std::vector<std::vector<std::vector<SketchPointPtr>>> valid_polygon_topos; ///< 存储修复后的多边形拓扑结构

        // 外环与全部内环进行 difference2D 计算，得到修复后拓扑
        // valid_polygon_topos 用了一个 Multipolygon 结构，依次处理里面的每个 Polygon
        geodata_topo::ring_difference_other_rings(geodata, current_ring_topo, inner_ring_topos, polygon->ring->normal, valid_polygon_topos); ///< 进行外环与内环的差异计算

        // 如果 Difference 的计算没有结果，说明有内环完全包含了这个 polygon 的外环，所以删除这个 polygon
        if(valid_polygon_topos.empty())
        {
            geodata->_delete_polygon(polygon); ///< 删除多边形
            if(geodata->get_parent_polygon(ring))
            {
                geodata->_delete_nous_ring(ring); ///< 删除多边形的外环
            }
        }
        else
        {
            // 如果 difference 的边界一致，说明无需额外处理
            if(GeoData::ring_key(valid_polygon_topos[0][0]) == polygon->ring->key)
            {
                return; ///< 如果差异计算的结果边界一致，直接返回
            }

            // 否则需要将环进行替换
            std::vector<std::vector<SketchPointPtr>> all_outer_ring; ///< 存储所有新的外环拓扑结构
            for(auto& polygon_topo: valid_polygon_topos) ///< 遍历修复后的多边形拓扑结构
            {
                all_outer_ring.emplace_back(std::move(polygon_topo[0])); ///< 将修复后的外环拓扑结构移动到all_outer_ring中
            }

            std::vector<SketchRingPtr> resultRings; ///< 存储替换后的环指针
            replace_polygon_equal_or_smaller(geodata, geodata->get_polygon(ring), all_outer_ring, resultRings); ///< 替换多边形
        }
    }
}

/**
 * @brief 重建多边形
 * 
 * 该函数用于根据指定的环（SketchRingPtr）重建一个多边形（SketchPolygonPtr）。
 * 通过获取环与其他环的关系，确定哪些环被包含在当前环内，并将这些环作为新多边形的内环。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定的环对象指针，作为新多边形的外环
 * 
 * @return SketchPolygonPtr 重建后的新多边形对象指针
 * 
 * @note 函数内部首先创建一个新的多边形对象，并设置其外环和法向量。
 *       然后通过调用GeoDataUtils::get().get_ring_relation_rings获取指定环与其他环的关系。
 *       遍历所有被包含的环，检查这些环是否被其他环包含。
 *       如果某个环不被其他环包含，则将其设置为新多边形的内环，并更新几何数据中的环与多边形的关系。
 *       最后，返回重建后的新多边形对象指针。
 */
SketchPolygonPtr GeoDataTopoOperations::rebuild_sketch_polygon(GeoData* geodata, SketchRingPtr ring)
{
    SketchPolygonPtr new_skp = std::make_shared<SketchPolygon>(); ///< 创建一个新的多边形对象
    new_skp->ring = ring; ///< 设置新多边形的外环
    new_skp->normal = ring->normal; ///< 设置新多边形的法向量

    SketchRingPtr contained = nullptr; ///< 存储被包含的环指针
    std::unordered_set<SketchRingPtr> contains; ///< 存储包含的环集合
    GeoDataUtils::get().get_ring_relation_rings(geodata, ring, contained, contains); ///< 获取指定环与其他环的关系

    geodata->set_ring_polygon(ring, new_skp); ///< 设置环所属的多边形为新多边形

    for(auto& inner_ring: contains) ///< 遍历所有被包含的环
    {
        SketchRingPtr inner_ring_contained = nullptr; ///< 存储内环被包含的环指针
        std::unordered_set<SketchRingPtr> inner_ring_contains; ///< 存储内环包含的环集合
        GeoDataUtils::get().get_ring_relation_rings(geodata, inner_ring, inner_ring_contained, inner_ring_contains); ///< 获取内环与其他环的关系

        // inner_ring_contained 不等于当前 ring 说明这个 inner_ring 被其余 inner_ring 包含，跳过
        if(inner_ring_contained != new_skp->ring) ///< 如果内环被其他环包含，跳过
        {
            continue;
        }

        SketchPolygonPtr parent_polygon = geodata->get_polygon(contained); ///< 获取被包含的环所属的父多边形
        if(contained && parent_polygon) ///< 如果被包含的环存在且有父多边形
        {
            parent_polygon->inner_rings.erase(inner_ring); ///< 从父多边形的内环集合中移除当前内环
        }

        geodata->set_ring_parent_polygon(inner_ring, new_skp); ///< 设置内环的父多边形为新多边形
        new_skp->inner_rings.insert(inner_ring); ///< 将内环添加到新多边形的内环集合中
    }

    return new_skp; ///< 返回重建后的新多边形对象指针
}

/**
 * @brief 替换或拆分多边形
 * 
 * 该函数用于将一个指定的多边形（SketchPolygonPtr）根据提供的替换拓扑结构（replace_topos）拆分成多个新的多边形。
 * 通过遍历替换拓扑结构中的每个外环，创建新的多边形，并将原多边形的内环根据包含关系分配给新的多边形。
 * 最后，删除原多边形并更新几何数据中的拓扑关系。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 需要替换或拆分的原多边形对象指针
 * @param[in] replace_topos 替换拓扑结构，包含多个外环的点集
 * @param[out] resultRings 输出的新生成的环对象集合，包含SketchRingPtr类型的指针
 * 
 * @note 函数内部首先检查替换拓扑结构和原多边形是否存在。
 *       然后遍历替换拓扑结构中的每个外环，创建新的多边形，并设置其法向量。
 *       对于每个新的多边形，遍历原多边形的内环，判断内环是否被新的外环包含。
 *       如果内环被包含，则将其添加到新的多边形中，并更新几何数据中的环与多边形的关系。
 *       将被处理的内环从原多边形中移除。
 *       更新新生成的多边形的拓扑关系，如果原多边形有父多边形，则将新生成的多边形添加到父多边形的内环集合中。
 *       处理完替换拓扑结构后，将原多边形中剩余的内环分配给父多边形。
 *       最后，删除原多边形并更新父多边形的内环集合。
 */
void GeoDataTopoOperations::replace_polygon_equal_or_smaller(GeoData* geodata, SketchPolygonPtr polygon,
                                                             const std::vector<std::vector<SketchPointPtr>>& replace_topos,
                                                             std::vector<SketchRingPtr>& resultRings)
{
    if(!replace_topos.size() || !polygon) ///< 如果替换拓扑结构为空或原多边形不存在，直接返回
    {
        return;
    }

    // 由于这个 case 是将一个 sketch polygon 拆分成多个 sketch polygon
    // 所以涉及到环与环间的相互关系
    // 基本逻辑是，使用 difference 计算完毕后的几个 ring 作为外环，依次查询原 polygon 中的内环是否被其包含
    // 若包含，则将这些内环归属于 ring
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon->ring); ///< 获取原多边形的父多边形

    for(auto& ex_ring: replace_topos) ///< 遍历替换拓扑结构中的每个外环
    {
        SketchPolygonPtr new_skp = geodata->_create_polygon(std::vector<std::vector<SketchPointPtr>>{ ex_ring }); ///< 创建新的多边形

        // 记录新生成的 ring 对象，返回进行 verify
        resultRings.emplace_back(new_skp->ring); ///< 将新生成的环添加到结果集合中

        new_skp->ring->normal = polygon->ring->normal; ///< 设置新多边形的法向量
        std::unordered_set<SketchRingPtr> processed_ring; ///< 存储已处理的内环

        for(auto inner: polygon->inner_rings) ///< 遍历原多边形的每个内环
        {
            std::vector<SketchPointPtr> inner_ring_pts;
            geodata->get_element_points(inner, inner_ring_pts); ///< 获取内环的点集
            if(GeoDataUtils::get().is_ring_contain_target(geodata, polygon->ring->normal, ex_ring, inner_ring_pts)) ///< 判断内环是否被新的外环包含
            {
                new_skp->inner_rings.insert(inner); ///< 将内环添加到新多边形的内环集合中
                processed_ring.insert(inner); ///< 将内环标记为已处理
                geodata->set_ring_parent_polygon(inner, new_skp); ///< 更新内环的父多边形为新多边形
            }
        }

        // 将被获取的环从原 polygon 中移除
        for(const auto pr: processed_ring) ///< 遍历已处理的内环
        {
            polygon->inner_rings.erase(pr); ///< 从原多边形的内环集合中移除已处理的内环
        }

        // 更改外挂的 ring polygon 拓扑关系
        // 若当前 polygon 不是最外层，即 polygon 的 ring 是别的 polygon 的内环
        // 则需要更新，包含 polygon 的外层 polygon 需要包含新的 polygon
        geodata->set_ring_polygon(new_skp->ring, new_skp); ///< 设置新环所属的多边形为新多边形
        geodata->set_ring_parent_polygon(new_skp->ring, parent_skp); ///< 设置新环的父多边形为原多边形的父多边形
        if(parent_skp) ///< 如果原多边形有父多边形
        {
            parent_skp->inner_rings.insert(new_skp->ring); ///< 将新环添加到父多边形的内环集合中
            parent_skp->m_changed = true; ///< 标记父多边形已更改
        }
    }

    // 残留子环
    for(auto inner_ring: polygon->inner_rings) ///< 遍历原多边形中剩余的内环
    {
        geodata->set_ring_parent_polygon(inner_ring, parent_skp); ///< 更新内环的父多边形为原多边形的父多边形
        if(parent_skp) ///< 如果原多边形有父多边形
        {
            parent_skp->inner_rings.insert(inner_ring); ///< 将内环添加到父多边形的内环集合中
        }
    }

    // 置空父关系
    if(parent_skp) ///< 如果原多边形有父多边形
    {
        parent_skp->inner_rings.erase(polygon->ring); ///< 从父多边形的内环集合中移除原多边形的外环
        geodata->set_ring_parent_polygon(polygon->ring, nullptr); ///< 清除原多边形外环的父多边形关系
    }

    // 删除原 polygon
    geodata->_delete_polygon(polygon); ///< 删除原多边形
}

/**
 * @brief 替换多边形
 * 
 * 该函数用于根据提供的替换拓扑结构（replace_topos）替换指定的多边形（polygon）。
 * 通过删除原多边形并创建新的多边形来实现替换。新的多边形根据替换拓扑结构中的每个外环创建，并设置其法向量。
 * 对于每个新的多边形，如果其外环不是其他多边形的内环，则将其父多边形设置为原多边形的父多边形，并将其添加到父多边形的内环集合中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 需要替换的原多边形对象指针
 * @param[in] replace_topos 替换拓扑结构，包含多个外环及其内环的点集
 * 
 * @note 函数内部首先获取原多边形的父多边形。
 *       然后删除原多边形，并根据需要删除其外环。
 *       遍历替换拓扑结构中的每个外环，创建新的多边形，并设置其法向量。
 *       如果新的多边形有内环，则将内环的key存储在inner_rings_key集合中。
 *       将新创建的多边形存储在new_polygons映射中，键为外环的key。
 *       最后，遍历new_polygons映射，如果某个新的多边形的外环不是其他多边形的内环，则将其父多边形设置为原多边形的父多边形，并将其添加到父多边形的内环集合中。
 */
void GeoDataTopoOperations::replace_polygon(GeoData* geodata, SketchPolygonPtr polygon,
                                            const std::vector<std::vector<std::vector<SketchPointPtr>>>& replace_topos)
{
    SketchPolygonPtr parent_skp = geodata->get_parent_polygon(polygon->ring); ///< 获取原多边形的父多边形
    geodata->_delete_polygon(polygon); ///< 删除原多边形
    if(parent_skp) ///< 如果原多边形有父多边形
    {
        // 带有父节点的环需要手动删除
        geodata->_delete_nous_ring(polygon->ring); ///< 删除原多边形的外环
    }

    std::unordered_set<size_t> inner_rings_key; ///< 存储内环的key集合
    std::unordered_map<size_t, SketchPolygonPtr> new_polygons; ///< 存储新创建的多边形及其外环key的映射

    for(auto& polygon_topo: replace_topos) ///< 遍历替换拓扑结构中的每个外环及其内环
    {
        if(polygon_topo.size() > 1) ///< 如果当前外环有内环
        {
            for(int i = 1; i < polygon_topo.size(); ++i) ///< 遍历每个内环
            {
                inner_rings_key.insert(GeoData::ring_key(polygon_topo[i])); ///< 将内环的key存储在inner_rings_key集合中
            }
        }

        SketchPolygonPtr new_polygon = geodata->_create_polygon(polygon_topo); ///< 创建新的多边形
        new_polygon->ring->normal = polygon->ring->normal; ///< 设置新多边形的法向量
        new_polygons[GeoData::ring_key(polygon_topo[0])] = new_polygon; ///< 将新多边形存储在new_polygons映射中，键为外环的key
    }

    // 处理最高层级多边形的父节点
    for(auto pos: new_polygons) ///< 遍历新创建的多边形
    {
        if(inner_rings_key.find(pos.first) == inner_rings_key.end()) ///< 如果当前多边形的外环不是其他多边形的内环
        {
            geodata->set_ring_parent_polygon(pos.second->ring, parent_skp); ///< 设置新多边形的外环的父多边形为原多边形的父多边形
            if(parent_skp) ///< 如果原多边形有父多边形
            {
                parent_skp->inner_rings.insert(pos.second->ring); ///< 将新多边形的外环添加到父多边形的内环集合中
            }
        }
    }
}

/**
 * @brief 检查路径并分割多边形
 * 
 * 该函数用于检查指定的线段（line）是否可以作为路径分割与之相邻的多边形（SketchPolygonPtr）。
 * 如果线段可以分割多边形，则将多边形分割成多个新的多边形，并更新几何数据中的拓扑关系。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] line 指定的线段对象指针，作为分割路径
 * @param[in] gen_polygon 是否生成新的多边形（该参数在当前实现中未被使用）
 * 
 * @return size_t 返回分割后的多边形数量
 * 
 * @note 函数内部首先定义一个映射splitOuterRing，用于存储分割后的外环及其法向量和点集。
 *       获取线段的两个端点位置。
 *       遍历与线段相邻的所有多边形，获取每个多边形的外环及其法向量。
 *       如果线段与外环共面且线段的中点位于外环内部，则进行路径分割。
 *       通过调用geodata->_find_split_ring_path函数找到分割路径，并将分割路径添加到splitOuterRing中。
 *       对于每个分割后的外环，如果外环对应的多边形不存在，则通过rebuild_sketch_polygon函数重建多边形。
 *       调用replace_polygon_equal_or_smaller函数将分割后的外环替换或拆分原多边形，并验证新生成的多边形。
 *       最后，标记渲染网格和网格为脏，表示需要更新，并返回分割后的多边形数量。
 */
bool GeoDataTopoOperations::check_path_and_split_sketch_polygon(GeoData* geodata, SketchLinePtr line, const bool gen_polygon)
{
    // geodata->_sync_nous();
    std::unordered_map<SketchRingPtr, std::pair<nous::vec3, std::vector<std::vector<SketchPointPtr>>>> splitOuterRing; ///< 存储分割后的外环及其法向量和点集
    nous::vec3 p0pos = geodata->get_point_position(line->p0); ///< 获取线段起点的位置
    nous::vec3 p1pos = geodata->get_point_position(line->p1); ///< 获取线段终点的位置

    for(auto& polygonPair: geodata->m_ring_adj_polygons) ///< 遍历与线段相邻的所有多边形
    {
        SketchRingPtr target_ring = polygonPair.first; ///< 获取目标环

        if(geodata->get_ring_adj_lines(target_ring).count(line)) ///< 如果目标环与线段相邻，跳过
        {
            continue;
        }

        nous::vec3 tNormal = target_ring->normal; ///< 获取目标环的法向量
        nous::vec3 primPointPos = geodata->get_point_position(geodata->m_nousptr_points.at(target_ring->ring_prim->vertex_ptr()->point_ptr())); ///< 获取目标环的一个点的位置
        nous::vec3 uDir = (p1pos - p0pos).normalized(); ///< 计算线段的方向向量

        float dp = tNormal.dot(uDir); ///< 计算法向量与方向向量的点积
        if(std::abs(dp) < sketch_math::sketch_epsilon) ///< 如果线段与目标环共面
        {
            std::vector<nous::pos_t> target_ring_pos_arr;
            GeoDataUtils::get().get_ring_positions(geodata, target_ring, target_ring_pos_arr); ///< 获取目标环的位置
            if(topo::calc_point_and_ring_relation(tNormal, p0pos + (p1pos - p0pos) * 0.5, target_ring_pos_arr) == topo::ESketchTopoRelation::Outside) ///< 如果线段的中点位于目标环外部，跳过
            {
                continue;
            }
            std::unordered_set<SketchPointPtr> visited; ///< 存储已访问的点
            std::vector<SketchPointPtr> path0; ///< 存储路径0
            std::vector<SketchPointPtr> path1; ///< 存储路径1
            std::vector<SketchPointPtr> result0; ///< 存储结果路径0
            std::vector<SketchPointPtr> result1; ///< 存储结果路径1

            visited.insert(line->p1); ///< 将线段终点标记为已访问
            geodata->_find_split_ring_path(line->p0, target_ring, visited, tNormal, path0, result0); ///< 找到从起点到目标环的分割路径
            visited.insert(line->p0); ///< 将线段起点标记为已访问
            visited.erase(line->p1); ///< 将线段终点从已访问集合中移除
            geodata->_find_split_ring_path(line->p1, target_ring, visited, tNormal, path1, result1); ///< 找到从终点到目标环的分割路径

            if(!result0.empty() && !result1.empty()) ///< 如果找到两个有效的分割路径
            {
                std::vector<SketchPointPtr> splitPath; ///< 存储分割路径
                if(result0.back() == result1.back()) ///< 如果两个路径的最后一个点相同，跳过
                {
                    continue;
                }
                splitPath.insert(splitPath.end(), result1.rbegin(), result1.rend()); ///< 将路径1反转后添加到分割路径中
                splitPath.insert(splitPath.end(), result0.begin(), result0.end()); ///< 将路径0添加到分割路径中
                path0.clear(); ///< 清空路径0
                path1.clear(); ///< 清空路径1

                nous::Vertex* beginVertex = target_ring->ring_prim->vertex_ptr(); ///< 获取目标环的起始顶点
                nous::Vertex* endVertex = beginVertex; ///< 获取目标环的结束顶点
                bool switchPath = true; ///< 标记是否切换路径
                bool throughPath = false; ///< 标记是否通过分割路径

                while(true) ///< 遍历目标环的每个顶点
                {
                    SketchPointPtr pp = geodata->m_nousptr_points.at(beginVertex->pre_vertex_ptr()->point_ptr()); ///< 获取前一个点
                    SketchPointPtr cp = geodata->m_nousptr_points.at(beginVertex->point_ptr()); ///< 获取当前点
                    SketchPointPtr np = geodata->m_nousptr_points.at(beginVertex->next_vertex_ptr()->point_ptr()); ///< 获取下一个点

                    if(cp == splitPath.front()) ///< 如果当前点是分割路径的起点
                    {
                        if(!throughPath) ///< 如果未通过分割路径
                        {
                            path0.insert(path0.end(), splitPath.begin(), splitPath.end()); ///< 将分割路径添加到路径0中
                            path1.insert(path1.end(), splitPath.rbegin(), splitPath.rend()); ///< 将分割路径反转后添加到路径1中
                            throughPath = true; ///< 标记已通过分割路径
                        }
                        switchPath = !switchPath; ///< 切换路径
                    }
                    else if(cp == splitPath.back()) ///< 如果当前点是分割路径的终点
                    {
                        if(!throughPath) ///< 如果未通过分割路径
                        {
                            path0.insert(path0.end(), splitPath.rbegin(), splitPath.rend()); ///< 将分割路径反转后添加到路径0中
                            path1.insert(path1.end(), splitPath.begin(), splitPath.end()); ///< 将分割路径添加到路径1中
                            throughPath = true; ///< 标记已通过分割路径
                        }
                        switchPath = !switchPath; ///< 切换路径
                    }
                    else
                    {
                        if(switchPath) ///< 如果当前路径是路径0
                        {
                            path0.push_back(cp); ///< 将当前点添加到路径0中
                        }
                        else ///< 如果当前路径是路径1
                        {
                            path1.push_back(cp); ///< 将当前点添加到路径1中
                        }
                    }

                    beginVertex = beginVertex->next_vertex_ptr(); ///< 移动到下一个顶点
                    if(beginVertex == endVertex) ///< 如果回到起始顶点，结束循环
                    {
                        break;
                    }
                }

                if(path0.size() > 2 && path1.size() > 2) ///< 如果路径0和路径1都有效
                {
                    std::vector<std::vector<SketchPointPtr>> splitRing; ///< 存储分割后的外环
                    splitRing.emplace_back(std::move(path0)); ///< 将路径0移动到分割后的外环中
                    splitRing.emplace_back(std::move(path1)); ///< 将路径1移动到分割后的外环中
                    splitOuterRing[target_ring] = { tNormal, splitRing }; ///< 将分割后的外环及其法向量添加到映射中
                }
            }
        }
    }

    for(auto pair: splitOuterRing) ///< 遍历分割后的外环
    {
        auto& v = pair.second.second; ///< 获取分割后的外环点集
        SketchPolygonPtr delete_polygon = geodata->get_polygon(pair.first); ///< 获取目标环对应的多边形

        if(!delete_polygon) ///< 如果目标环对应的多边形不存在
        {
            delete_polygon = rebuild_sketch_polygon(geodata, pair.first); ///< 重建目标环对应的多边形
        }

        std::vector<SketchRingPtr> resultRings; ///< 存储新生成的环
        replace_polygon_equal_or_smaller(geodata, delete_polygon, v, resultRings); ///< 替换或拆分多边形
        for(auto ring: resultRings) ///< 遍历新生成的环
        {
            verify_sketch_polygon(geodata, geodata->get_polygon(ring)); ///< 验证并修复新生成的多边形
        }
    }

    geodata->mark_render_mesh_as_dirty(); ///< 标记渲染网格为脏
    geodata->mark_mesh_as_dirty(); ///< 标记网格为脏
    return splitOuterRing.size(); ///< 返回分割后的多边形数量
}

/**
 * @brief 检查点并合并线段
 * 
 * 该函数用于检查指定的点（point）是否可以与其相邻的两个点（pp 和 pn）合并成一条线段。
 * 如果点的相邻点集合大小为2，则尝试合并这两条线段。
 * 如果提供了限制点集合（limit_point_set），则检查这三个点是否都在限制点集合中，如果都在则不进行合并。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 指定点对象指针，作为可能被合并的点
 * @param[in] limit_point_set 限制点集合指针，如果提供，则检查点是否在该集合中
 * 
 * @note 函数内部首先获取指定点的相邻点集合。
 *       如果相邻点集合的大小为2，则获取这两个相邻点（pp 和 pn）。
 *       如果提供了限制点集合，则检查这三个点（pp、point 和 pn）是否都在限制点集合中。
 *       如果都在限制点集合中，则不进行合并，直接返回。
 *       否则，调用merge_line函数合并这两条线段。
 */
void GeoDataTopoOperations::check_point_and_merge_line(GeoData* geodata, SketchPointPtr point, std::unordered_set<SketchPointPtr>* limit_point_set)
{
    auto&& adjPoints = geodata->get_point_adj_points(point); ///< 获取指定点的相邻点集合
    if(adjPoints.size() == 2) ///< 如果相邻点集合的大小为2
    {
        auto it = adjPoints.begin();
        SketchPointPtr pp = *it; ///< 获取第一个相邻点
        ++it;
        SketchPointPtr pn = *it; ///< 获取第二个相邻点

        if(limit_point_set) ///< 如果提供了限制点集合
        {
            const bool in_set1 = limit_point_set->find(pp) != limit_point_set->end(); ///< 检查第一个相邻点是否在限制点集合中
            const bool in_set2 = limit_point_set->find(point) != limit_point_set->end(); ///< 检查指定点是否在限制点集合中
            const bool in_set3 = limit_point_set->find(pn) != limit_point_set->end(); ///< 检查第二个相邻点是否在限制点集合中

            if(in_set1 && in_set2 && in_set3) ///< 如果三个点都在限制点集合中
            {
                return; ///< 直接返回，不进行合并
            }
        }
        merge_line(geodata, pp, point, pn); ///< 合并两条线段
    }
}

/**
 * @brief 合并线段
 * 
 * 该函数用于合并两条相邻的线段（由点pre、center和next定义）。
 * 如果这两条线段共线且属于同一个线段组，则将它们合并成一条新的线段。
 * 合并过程中，会更新几何数据中的线段和环关系，并根据需要删除原始线段。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] pre 第一个线段的起点指针
 * @param[in] center 第一个线段的终点和第二个线段的起点指针
 * @param[in] next 第二个线段的终点指针
 * 
 * @note 函数内部首先定义一个lambda函数calculateScalePositon，用于计算调整后的下一个点位置。
 *       获取pre、center和next点的位置。
 *       计算pre到center的线段方向向量cDir，并使用calculateScalePositon调整next点的位置。
 *       计算调整后的next点到center的线段方向向量nDir。
 *       如果cDir和nDir的方向相同（即两条线段共线），则继续合并。
 *       获取两条线段（line0和line1）的指针，并检查它们是否属于同一个线段组。
 *       如果不属于同一个线段组，则不进行合并，直接返回。
 *       获取两条线段相邻的环集合，并为每个环创建一个新的拓扑结构，排除center点。
 *       对于每个新的拓扑结构，调用retopo_ring函数更新环的拓扑结构。
 *       创建一条新的线段（new_line），并将其添加到线段组中。
 *       删除原始的两条线段（line0和line1）。
 */
void GeoDataTopoOperations::merge_line(GeoData* geodata, SketchPointPtr pre, SketchPointPtr center, SketchPointPtr next)
{
    auto calculateScalePositon = [](const nous::vec3& baseDir, const nous::vec3& basePos, const nous::vec3& cpos) {
        float scale = 0.01f;
        nous::vec3 bc = cpos - basePos;
        float k = -bc.dot(baseDir) / baseDir.dot(baseDir);
        return cpos + baseDir * k * (1.0f - scale);
    };
    // geodata->_sync_nous();

    nous::vec3 prePos = geodata->get_point_position(pre); ///< 获取pre点的位置
    nous::vec3 currentPos = geodata->get_point_position(center); ///< 获取center点的位置
    nous::vec3 cDir = (currentPos - prePos).normalized(); ///< 计算pre到center的线段方向向量
    nous::vec3 nextPos = geodata->get_point_position(next); ///< 获取next点的位置
    nextPos = calculateScalePositon(cDir, currentPos, nextPos); ///< 调整next点的位置
    nous::vec3 nDir = (nextPos - currentPos).normalized(); ///< 计算调整后的next点到center的线段方向向量

    float dp = cDir.dot(nDir); ///< 计算cDir和nDir的方向相同性
    if(std::abs(dp - 1.0f) < sketch_math::sketch_epsilon) ///< 如果两条线段共线
    {
        SketchLinePtr line0 = geodata->m_key_lines.at(GeoData::line_key(pre, center)); ///< 获取第一条线段的指针
        SketchLinePtr line1 = geodata->m_key_lines.at(GeoData::line_key(center, next)); ///< 获取第二条线段的指针
        if(line0->group() != line1->group()) ///< 如果两条线段不属于同一个线段组
        {
            return; ///< 直接返回，不进行合并
        }
        auto& adjRings = geodata->get_line_adj_rings(line0); ///< 获取第一条线段相邻的环集合
        std::map<SketchRingPtr, std::vector<SketchPointPtr>> newTopo; ///< 存储新的拓扑结构

        for(auto ring: adjRings) ///< 遍历每个相邻的环
        {
            for(auto prim_vert: geodata->get_mesh()->prim_vertices_range(ring->ring_prim->idx())) ///< 遍历环中的每个顶点
            {
                SketchPointPtr cp = geodata->m_nousptr_points.at(geodata->get_mesh()->vertex(prim_vert)->point_ptr()); ///< 获取当前顶点对应的点
                if(cp != center) ///< 如果当前点不是center点
                {
                    newTopo[ring].push_back(cp); ///< 将当前点添加到新的拓扑结构中
                }
            }
        }

        for(auto newRing: newTopo) ///< 遍历每个新的拓扑结构
        {
            retopo_ring(geodata, newRing.first, newRing.second); ///< 更新环的拓扑结构
        }

        SketchLinePtr new_line = geodata->_add_nous_line(pre, next); ///< 创建一条新的线段
        if(line0->group()) ///< 如果第一条线段属于某个线段组
        {
            ElementGroupPtr group = line0->group(); ///< 获取线段组指针
            group->erase_element(line0); ///< 从线段组中移除第一条线段
            group->erase_element(line1); ///< 从线段组中移除第二条线段
            group->add_element(new_line); ///< 将新的线段添加到线段组中
        }
        geodata->_delete_nous_line(line0); ///< 删除第一条线段
        geodata->_delete_nous_line(line1); ///< 删除第二条线段
    }
}

/**
 * @brief 合并共面环和多边形
 * 
 * 该函数用于合并共面的环（SketchRingPtr）及其对应的多边形（SketchPolygonPtr）。
 * 如果两个环共面且属于同一个多边形，则合并这两个环。
 * 如果两个环共面但不属于同一个多边形，则根据参数决定是否合并对应的多边形。
 * 如果提供了限制点集合（limit_point_set），则检查这三个点是否都在限制点集合中，如果都在则不进行合并。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] coplane_rings 共面环对的集合，每个元素是一个包含两个SketchRingPtr的pair
 * @param[in] merge_adj_polygon 是否合并相邻的多边形
 * 
 * @note 函数内部首先遍历共面环对的集合。
 *       对于每一对共面环，检查它们是否至少有一个存在。
 *       如果其中一个环不存在，则删除另一个环。
 *       如果两个环都存在，则获取它们对应的多边形。
 *       如果两个环对应的多边形都存在且merge_adj_polygon为true，则合并这两个多边形。
 *       如果merge_adj_polygon为false，则删除这两个多边形。
 *       如果两个环对应的多边形不存在，则检查它们是否属于同一个父多边形。
 *       如果属于同一个父多边形，则合并这两个环。
 */
void GeoDataTopoOperations::merge_coplane_ring_and_polygon(GeoData* geodata, std::vector<std::pair<SketchRingPtr, SketchRingPtr>>& coplane_rings,
                                                           bool merge_adj_polygon)
{
    bool ring_deleted = false; ///< 标记是否有环被删除
    for(auto coplane_rings: coplane_rings) ///< 遍历共面环对的集合
    {
        SketchRingPtr ring0 = coplane_rings.first; ///< 获取第一个环
        SketchRingPtr ring1 = coplane_rings.second; ///< 获取第二个环
        if(!ring0) ///< 如果第一个环不存在
        {
            geodata->_delete_nous_ring(ring1); ///< 删除第二个环
        }
        else if(!ring1) ///< 如果第二个环不存在
        {
            geodata->_delete_nous_ring(ring0); ///< 删除第一个环
        }
        else ///< 如果两个环都存在
        {
            SketchPolygonPtr polygon0 = geodata->get_polygon(ring0); ///< 获取第一个环对应的多边形
            SketchPolygonPtr polygon1 = geodata->get_polygon(ring1); ///< 获取第二个环对应的多边形
            if(polygon0 && polygon1) ///< 如果两个环对应的多边形都存在
            {
                if(merge_adj_polygon) ///< 如果需要合并相邻的多边形
                {
                    merge_polygon(geodata, polygon0, polygon1, true); ///< 合并这两个多边形
                }
                else ///< 如果不需要合并相邻的多边形
                {
                    geodata->_delete_polygon(polygon0); ///< 删除第一个多边形
                    geodata->_delete_polygon(polygon1); ///< 删除第二个多边形
                }
            }
            else ///< 如果两个环对应的多边形不存在
            {
                SketchPolygonPtr ring0_parent_skp = geodata->get_parent_polygon(ring0); ///< 获取第一个环的父多边形
                SketchPolygonPtr ring1_parent_skp = geodata->get_parent_polygon(ring1); ///< 获取第二个环的父多边形
                if(ring0_parent_skp && ring1_parent_skp && ring0_parent_skp == ring1_parent_skp) ///< 如果两个环属于同一个父多边形
                {
                    merge_ring(geodata, ring0, ring1, true); ///< 合并这两个环
                }
            }
        }
    }
}

/**
 * @brief 重新拓扑化环
 * 
 * 该函数用于重新拓扑化指定的环（SketchRingPtr），并根据新的拓扑结构（newTopo）创建一个新的环。
 * 重新拓扑化过程中，会更新几何数据中的环和多边形关系，并根据需要删除原始环。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 需要重新拓扑化的原始环对象指针
 * @param[in] newTopo 新的拓扑结构，包含SketchPointPtr类型的点集
 * 
 * @note 函数内部首先获取原始环所属的多边形和父多边形。
 *       然后根据新的拓扑结构创建一个新的环，并设置其所属的多边形和父多边形。
 *       如果原始环所属的多边形存在，则将多边形的外环更新为新的环，并标记多边形已更改。
 *       如果原始环的父多边形存在，则将原始环从父多边形的内环集合中移除，并将新的环添加到父多边形的内环集合中。
 *       最后，清除原始环的多边形和父多边形关系，并删除原始环。
 */
void GeoDataTopoOperations::retopo_ring(GeoData* geodata, SketchRingPtr ring, std::vector<SketchPointPtr>& newTopo)
{
    SketchPolygonPtr polygon = geodata->get_polygon(ring); ///< 获取原始环所属的多边形
    SketchPolygonPtr parent_polygon = geodata->get_parent_polygon(ring); ///< 获取原始环的父多边形

    SketchRingPtr new_ring = geodata->_add_nous_ring(newTopo); ///< 根据新的拓扑结构创建一个新的环
    geodata->set_ring_polygon(new_ring, polygon); ///< 设置新环所属的多边形
    geodata->set_ring_parent_polygon(new_ring, parent_polygon); ///< 设置新环的父多边形

    if(polygon) ///< 如果原始环所属的多边形存在
    {
        polygon->ring = new_ring; ///< 将多边形的外环更新为新的环
        polygon->m_changed = true; ///< 标记多边形已更改
    }

    if(parent_polygon) ///< 如果原始环的父多边形存在
    {
        parent_polygon->inner_rings.erase(ring); ///< 将原始环从父多边形的内环集合中移除
        parent_polygon->inner_rings.insert(new_ring); ///< 将新的环添加到父多边形的内环集合中
    }

    geodata->set_ring_polygon(ring, nullptr); ///< 清除原始环的多边形关系
    geodata->set_ring_parent_polygon(ring, nullptr); ///< 清除原始环的父多边形关系
    geodata->_delete_nous_ring(ring); ///< 删除原始环
}

/**
 * @brief 替换线段
 * 
 * 该函数用于根据提供的新点集（new_line_points）替换指定的线段（line）。
 * 替换过程中，会更新几何数据中的环和线段关系，并根据需要删除原始线段。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] line 需要替换的原始线段对象指针
 * @param[in] new_line_points 新的线段点集，包含SketchPointPtr类型的点
 * 
 * @note 函数内部首先获取原始线段相邻的所有环。
 *       如果相邻环不为空，则遍历每个环，根据新点集更新环的拓扑结构。
 *       对于每个环，遍历环中的每个顶点，根据新点集调整环的点集。
 *       如果当前顶点和下一个顶点分别匹配新点集的最后一个点和第一个点，则反转新点集。
 *       如果当前顶点和下一个顶点分别匹配新点集的第一个点和最后一个点，则将新点集（除最后一个点外）添加到环的点集中。
 *       否则，将当前顶点添加到环的点集中。
 *       更新每个环的拓扑结构后，删除原始线段。
 *       如果原始线段属于某个线段组，则将原始线段从线段组中移除。
 *       根据新点集创建新的线段，并将这些新的线段添加到原始线段所属的线段组中。
 *       最后，删除原始线段。
 */
void GeoDataTopoOperations::replace_line(GeoData* geodata, SketchLinePtr line, std::vector<SketchPointPtr>& new_line_points)
{
    auto line_adj_rings = geodata->get_line_adj_rings(line); ///< 获取原始线段相邻的所有环
    if(!line_adj_rings.empty()) ///< 如果相邻环不为空
    {
        std::unordered_map<SketchRingPtr, std::vector<SketchPointPtr>> replace_topo; ///< 存储每个环的新拓扑结构
        for(auto& ring: line_adj_rings) ///< 遍历每个环
        {
            for(auto prim_vert: geodata->get_mesh()->prim_vertices_range(ring->ring_prim->idx())) ///< 遍历环中的每个顶点
            {
                nous::Vertex* ver = geodata->get_mesh()->vertex(prim_vert); ///< 获取当前顶点
                nous::Vertex* next_ver = ver->next_vertex_ptr(); ///< 获取下一个顶点
                SketchPointPtr current_p = geodata->m_nousptr_points.at(ver->point_ptr()); ///< 获取当前顶点对应的SketchPointPtr
                SketchPointPtr next_p = geodata->m_nousptr_points.at(next_ver->point_ptr()); ///< 获取下一个顶点对应的SketchPointPtr

                if(current_p == new_line_points.back() && next_p == new_line_points.front()) ///< 如果当前顶点和下一个顶点分别匹配新点集的最后一个点和第一个点
                {
                    std::reverse(new_line_points.begin(), new_line_points.end()); ///< 反转新点集
                }
                if(current_p == new_line_points.front() && next_p == new_line_points.back()) ///< 如果当前顶点和下一个顶点分别匹配新点集的第一个点和最后一个点
                {
                    replace_topo[ring].insert(replace_topo[ring].end(), new_line_points.begin(), new_line_points.end() - 1); ///< 将新点集（除最后一个点外）添加到环的点集中
                }
                else
                {
                    replace_topo[ring].push_back(current_p); ///< 将当前顶点添加到环的点集中
                }
            }
        }

        // geodata->_sync_nous();
        for(auto& ring_pair: replace_topo) ///< 遍历每个环的新拓扑结构
        {
            retopo_ring(geodata, ring_pair.first, ring_pair.second); ///< 更新每个环的拓扑结构
        }
    }

    // group
    ElementGroupPtr group = line->group(); ///< 获取原始线段所属的线段组
    if(group)
    {
        group->erase_element(line); ///< 将原始线段从线段组中移除
    }
    for(int i = 1; i < new_line_points.size(); ++i) ///< 根据新点集创建新的线段
    {
        SketchLinePtr new_line = geodata->_add_nous_line(new_line_points[i - 1], new_line_points[i]); ///< 创建新的线段
        if(group)
        {
            group->add_element(new_line); ///< 将新的线段添加到线段组中
        }
    }
    geodata->_delete_nous_line(line); ///< 删除原始线段
}

/**
 * @brief 更新多个点的位置
 * 
 * 该函数用于根据提供的点和新位置对（new_pos）更新几何数据中的多个点的位置。
 * 通过遍历提供的点和新位置对，逐个更新每个点的位置。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] new_pos 包含点和新位置对的向量，每个元素是一个std::pair<SketchPointPtr, nous::vec3>
 * 
 * @note 函数内部首先遍历提供的点和新位置对。
 *       对于每个点和新位置对，调用geodata->set_point_position方法更新点的位置。
 */
void GeoDataTopoOperations::change_point_positon(GeoData* geodata, const std::vector<std::pair<SketchPointPtr, nous::vec3>>& new_pos)
{
    for(auto& pos: new_pos) ///< 遍历提供的点和新位置对
    {
        geodata->set_point_position(pos.first, pos.second); ///< 更新点的位置
    }
}

/**
 * @brief 从源几何数据复制指定元素到目标几何数据
 * 
 * 该函数用于从源几何数据（src_geodata）复制指定的几何元素（elements）到目标几何数据（geodata）。
 * 通过遍历提供的几何元素集合，逐个复制每个元素及其相关点。
 * 支持复制的元素类型包括多边形（Polygon）、线段（Line）和无分支线组（NoBranchLineGroup）。
 * 
 * @param[in] geodata 目标几何数据对象指针，提供几何数据访问接口
 * @param[in] src_geodata 源几何数据对象指针，提供几何数据访问接口
 * @param[in] elements 需要复制的几何元素集合，包含SketchElementPtr类型的指针
 * 
 * @note 函数内部首先定义一个映射point_cache，用于缓存已复制的点。
 *       然后遍历提供的几何元素集合，根据元素类型进行不同的复制操作：
 *       - 对于多边形（Polygon），获取其拓扑结构，并逐个复制每个点，然后创建新的多边形。
 *       - 对于线段（Line），复制其两个端点，然后创建新的线段。
 *       - 对于无分支线组（NoBranchLineGroup），复制其包含的所有线段及其端点，然后创建新的无分支线组。
 *       在复制过程中，使用point_cache缓存已复制的点，避免重复复制。
 */
void GeoDataTopoOperations::copy_element_without_check(GeoData* geodata, GeoData* src_geodata, const std::unordered_set<SketchElementPtr>& elements)
{
    std::unordered_map<SketchPointPtr, SketchPointPtr> point_cache; ///< 缓存已复制的点
    for(auto ele: elements) ///< 遍历提供的几何元素集合
    {
        if(ele->type() == EElementType::Polygon) ///< 如果元素是多边形
        {
            std::vector<std::vector<SketchPointPtr>> new_polygon_topo; ///< 存储新的多边形拓扑结构
            std::vector<std::vector<SketchPointPtr>> polygon_topo = GeoDataUtils::get().get_polygon_topo(src_geodata, ele->to_polygon()); ///< 获取多边形的拓扑结构
            for(auto& ring: polygon_topo) ///< 遍历多边形的每个环
            {
                std::vector<SketchPointPtr>& new_ring = new_polygon_topo.emplace_back(); ///< 为当前环创建一个新的拓扑结构数组
                for(auto& point: ring) ///< 遍历环中的每个点
                {
                    auto& pos = point_cache.find(point); ///< 查找点是否已复制
                    if(pos == point_cache.end()) ///< 如果点未复制
                    {
                        point_cache[point] = geodata->add_point_without_check(src_geodata->get_point_position(point)); ///< 复制点并缓存
                    }
                    new_ring.push_back(point_cache[point]); ///< 将复制的点添加到新的环中
                }
            }
            geodata->add_polygon_without_check(new_polygon_topo, true); ///< 创建新的多边形
        }
        else if(ele->type() == EElementType::Line) ///< 如果元素是线段
        {
            SketchLinePtr line = ele->to_line(); ///< 获取线段对象
            SketchPointPtr p0 = line->p0; ///< 获取线段的起点
            SketchPointPtr p1 = line->p1; ///< 获取线段的终点
            auto& pos0 = point_cache.find(p0); ///< 查找起点是否已复制
            if(pos0 == point_cache.end()) ///< 如果起点未复制
            {
                point_cache[p0] = geodata->add_point_without_check(src_geodata->get_point_position(p0)); ///< 复制起点并缓存
            }
            auto& pos1 = point_cache.find(p1); ///< 查找终点是否已复制
            if(pos1 == point_cache.end()) ///< 如果终点未复制
            {
                point_cache[p1] = geodata->add_point_without_check(src_geodata->get_point_position(p1)); ///< 复制终点并缓存
            }
            geodata->add_line_without_check(point_cache[p0], point_cache[p1]); ///< 创建新的线段
        }
        else if(ele->type() == EElementType::NoBranchLineGroup) ///< 如果元素是无分支线组
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>(); ///< 创建新的无分支线组
            ElementGroupPtr line_group = ele->to_group(); ///< 获取无分支线组对象
            for(auto& ele: line_group->elements()) ///< 遍历无分支线组中的每个元素
            {
                SketchLinePtr line = ele->to_line(); ///< 获取线段对象
                SketchPointPtr p0 = line->p0; ///< 获取线段的起点
                SketchPointPtr p1 = line->p1; ///< 获取线段的终点
                auto& pos0 = point_cache.find(p0); ///< 查找起点是否已复制
                if(pos0 == point_cache.end()) ///< 如果起点未复制
                {
                    point_cache[p0] = geodata->add_point_without_check(src_geodata->get_point_position(p0)); ///< 复制起点并缓存
                }
                auto& pos1 = point_cache.find(p1); ///< 查找终点是否已复制
                if(pos1 == point_cache.end()) ///< 如果终点未复制
                {
                    point_cache[p1] = geodata->add_point_without_check(src_geodata->get_point_position(p1)); ///< 复制终点并缓存
                }
                SketchLinePtr new_line = geodata->add_line_without_check(point_cache[p0], point_cache[p1]); ///< 创建新的线段
                new_group->add_element(new_line); ///< 将新的线段添加到新的无分支线组中
            }
            geodata->add_element_group(new_group); ///< 将新的无分支线组添加到目标几何数据中
        }
    }
}

/**
 * @brief 添加多边形而不拆分线段
 *
 * 该函数用于根据给定的顶点位置数组（polygon_pos）在几何数据中添加一个多边形。
 * 函数会尝试创建新的线段，但不会因为新线段的创建而拆分已有的多边形。
 * 如果指定了is_valid参数，则通过它返回多边形是否有效。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon_pos 包含多边形所有环顶点位置的二维数组，每个环由nous::pos_t类型的点集表示
 * @param[out] is_valid 指向bool类型变量的指针，用于指示生成的多边形是否有效
 * 
 * @return std::vector<std::vector<SketchPointPtr>> 返回包含多边形所有环顶点指针的二维数组
 * 
 * @note 函数内部首先计算多边形的法向量。
 *       然后遍历每个环的顶点位置，并尝试为每一对相邻的顶点创建线段。
 *       如果创建了新的线段，则只保留第一个点，并删除未使用的点。
 *       接着，检查并确保每个环中的线段都存在于geodata的m_key_lines映射中。
 *       最后，调用_check_ring_affilia_and_create_polygon方法来验证环的隶属关系并创建多边形。
 */
std::vector<std::vector<SketchPointPtr>> GeoDataTopoOperations::add_polygon_without_split_line(GeoData* geodata,
                                                                                               std::vector<std::vector<nous::pos_t>>& polygon_pos,
                                                                                               bool* is_valid)
{
    // 计算多边形的法向量
    nous::vec3 polygon_normal = sketch_math::ring_normal(polygon_pos.front());

    // 定义结果容器，存储多边形所有环的顶点指针
    std::vector<std::vector<SketchPointPtr>> polygon_pt_arr;

    // 遍历多边形的每个环
    for(const auto& ring: polygon_pos)
    {
        // 为当前环创建一个新的顶点指针数组
        std::vector<SketchPointPtr>& ring_pt_arr = polygon_pt_arr.emplace_back();

        // 遍历环中的每对相邻顶点位置
        for(int i = 0; i < ring.size(); ++i)
        {
            // 获取当前点和下一个点的位置
            nous::pos_t p0 = ring[i];
            nous::pos_t p1 = ring[(i + 1) % ring.size()];

            // 定义临时容器，用于存储新创建的线段及其相关点
            std::vector<SketchPointPtr> newLines;
            std::map<SketchLinePtr, std::vector<SketchPointPtr>> recreatLine;

            // 尝试添加线段，如果成功则更新newLines和recreatLine
            geodata->_add_line(p0, p1, recreatLine, newLines);

            if(!newLines.empty())
            {
                // 如果创建了多个点，只保留第一个点，其余未使用的点将被删除
                std::map<size_t, SketchPointPtr, std::greater<size_t>> unused_pt_idx_set;
                for(int j = 1; j < newLines.size(); ++j)
                {
                    SketchPointPtr pt = newLines[j];
                    if(!pt->point->vertex_ptr())
                    {
                        // 收集未使用的点索引
                        unused_pt_idx_set.insert({ pt->point->idx(), pt });
                    }
                }

                // 删除未使用的点
                for(auto& pair: unused_pt_idx_set)
                {
                    geodata->m_nousptr_points.erase(pair.second->point);
                    geodata->m_nous_mesh->delete_point(pair.first);
                }

                // 只保留第一个点作为环的一部分
                ring_pt_arr.emplace_back(newLines.front());
            }
        }

        // 确保环中的每条线段都存在
        for(int i = 0; i < ring_pt_arr.size(); ++i)
        {
            SketchPointPtr prev_point = ring_pt_arr[i];
            SketchPointPtr next_point = ring_pt_arr[(i + 1) % ring_pt_arr.size()];

            const size_t ring_line_key = GeoData::line_key(prev_point, next_point);

            // 如果线段不存在于m_key_lines中，则创建新的线段
            if(geodata->m_key_lines.find(ring_line_key) == geodata->m_key_lines.end())
            {
                bool is_exist = false;
                geodata->_add_nous_line(prev_point, next_point);
            }
        }
    }

    // 根据环创建多边形，只有外环可以生成多边形，内环不能生成多边形
    for(int i = 0; i < polygon_pt_arr.size(); ++i)
    {
        geodata->_check_ring_affilia_and_create_polygon(polygon_pt_arr[i], i == 0, true);
    }

    return polygon_pt_arr;
}

/**
 * @brief 尝试合并平滑组
 *
 * 该函数用于尝试合并给定元素集合中的平滑组（SmoothGroup）。
 * 如果元素是线段（SketchLinePtr），则检查其相邻的多边形（SketchPolygonPtr）。
 * 如果相邻的多边形已经属于平滑组，则将线段添加到相应的平滑组中。
 * 如果相邻的多边形不属于任何平滑组，则创建新的平滑组并将线段和多边形添加到新组中。
 * 如果元素是焊接线组（NoBranchLineGroup），则处理其中的线段，并尝试合并它们所属的平滑组。
 * 最后，将所有创建的新平滑组添加到几何数据中。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] elements 需要处理的元素集合，包含SketchElementPtr类型的指针
 * @param[in] smooth_edge 是否处理平滑边（该参数在当前实现中未被使用）
 *
 * @note 函数内部首先定义一个映射smooth_groups，用于存储待生成的平滑组及其包含的多边形和线段。
 *       然后定义一个lambda函数find_valid_group，用于处理线段及其相邻的多边形。
 *       遍历给定的元素集合，对于每个线段，获取其相邻的多边形，并调用find_valid_group进行处理。
 *       对于每个焊接线组，遍历其中的线段，处理它们及其相邻的多边形，并删除焊接线组。
 *       最后，遍历smooth_groups映射，创建新的SmoothGroup对象，并将相应的多边形和线段添加到新组中。
 *       最终将所有新创建的平滑组添加到几何数据中。
 */
void GeoDataTopoOperations::try_merge_smooth_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements, bool smooth_edge)
{
    // 定义一个映射，用于存储待生成的平滑组及其包含的多边形和线段
    std::unordered_map<int, std::pair<std::unordered_set<SketchElementPtr>, std::unordered_set<SketchElementPtr>>> smooth_groups;
    int smooth_groups_counter = 0;

    // 定义一个lambda函数，用于处理线段及其相邻的多边形
    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchPolygonPtr>& adj_polygons) {
        int affiliation_group_id_0 = -1;
        int affiliation_group_id_1 = -1;
        auto it = adj_polygons.begin();
        SketchPolygonPtr first_polygon = *it;
        SketchPolygonPtr second_polygon = *(++it);

        // 判断相邻面是否已经为平滑组
        if(first_polygon->group() && first_polygon->group()->type() == EElementType::SmoothGroup && second_polygon->group() &&
           second_polygon->group()->type() == EElementType::SmoothGroup)
        {
            first_polygon->group()->add_element(line);
            if(first_polygon->group() != second_polygon->group())
            {
                // 合并组
                first_polygon->group()->merge(second_polygon->group());
                // geodata->delete_element_group(second_polygon->group());
            }
        }
        else if(first_polygon->group() && first_polygon->group()->type() == EElementType::SmoothGroup)
        {
            first_polygon->group()->add_element(line);
            first_polygon->group()->add_element(second_polygon);
        }
        else if(second_polygon->group() && second_polygon->group()->type() == EElementType::SmoothGroup)
        {
            second_polygon->group()->add_element(line);
            second_polygon->group()->add_element(first_polygon);
        }
        else
        {
            // 待生成的平滑组
            for(auto group: smooth_groups)
            {
                if(group.second.first.find(first_polygon) != group.second.first.end())
                {
                    affiliation_group_id_0 = group.first;
                }
                if(group.second.first.find(second_polygon) != group.second.first.end())
                {
                    affiliation_group_id_1 = group.first;
                }
                if(affiliation_group_id_0 >= 0 && affiliation_group_id_1 >= 0)
                {
                    break;
                }
            }
            // 合并连接在一起的平滑组
            if(affiliation_group_id_0 >= 0 && affiliation_group_id_1 >= 0)
            {
                if(affiliation_group_id_0 != affiliation_group_id_1)
                {
                    // 合并两个组为一个组
                    smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(smooth_groups[affiliation_group_id_1].first.begin(),
                                                                       smooth_groups[affiliation_group_id_1].first.end());

                    smooth_groups[affiliation_group_id_0].second.insert(line);
                    smooth_groups[affiliation_group_id_0].second.insert(smooth_groups[affiliation_group_id_1].second.begin(),
                                                                        smooth_groups[affiliation_group_id_1].second.end());

                    smooth_groups.erase(affiliation_group_id_1);
                }
                else
                {
                    // 在同一组内
                    smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                    smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                    smooth_groups[affiliation_group_id_0].second.insert(line);
                }
            }
            else if(affiliation_group_id_0 >= 0)
            {
                smooth_groups[affiliation_group_id_0].first.insert(first_polygon);
                smooth_groups[affiliation_group_id_0].first.insert(second_polygon);
                smooth_groups[affiliation_group_id_0].second.insert(line);
            }
            else if(affiliation_group_id_1 >= 0)
            {
                smooth_groups[affiliation_group_id_1].first.insert(first_polygon);
                smooth_groups[affiliation_group_id_1].first.insert(second_polygon);
                smooth_groups[affiliation_group_id_1].second.insert(line);
            }
            else
            {
                int new_group_id = smooth_groups_counter++;
                smooth_groups[new_group_id].first.insert(first_polygon);
                smooth_groups[new_group_id].first.insert(second_polygon);
                smooth_groups[new_group_id].second.insert(line);
            }
        }
    };

    // 遍历给定的元素集合
    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            std::unordered_set<SketchPolygonPtr> adj_polygons = GeoDataUtils::get().get_line_adj_polygons(geodata, line);
            if(adj_polygons.size() == 2)
            {
                find_valid_group(line, adj_polygons);
            }
        }
        else if(ele->type() == EElementType::NoBranchLineGroup)
        {
            ElementGroupPtr weld_line_group = ele->to_group();
            std::unordered_set<SketchElementPtr> remain_lines;
            for(auto line: weld_line_group->elements())
            {
                SketchLinePtr line_ptr = line->to_line();
                std::unordered_set<SketchPolygonPtr> adj_polygons = GeoDataUtils::get().get_line_adj_polygons(geodata, line_ptr);
                if(adj_polygons.size() == 2)
                {
                    find_valid_group(line_ptr, adj_polygons);
                }
                else
                {
                    remain_lines.insert(line);
                }
            }
            geodata->delete_element_group(weld_line_group);
            try_merge_no_branch_line_group(geodata, remain_lines, {});
        }
    }

    // 遍历smooth_groups映射，创建新的SmoothGroup对象，并将相应的多边形和线段添加到新组中
    for(auto smooth_group: smooth_groups)
    {
        std::unordered_set<SketchElementPtr>& polygons = smooth_group.second.first;
        std::unordered_set<SketchElementPtr>& lines = smooth_group.second.second;

        SmoothGroupPtr new_group = std::make_shared<SmoothGroup>();
        for(auto polygon: polygons)
        {
            new_group->add_element(polygon);
        }
        for(auto line: lines)
        {
            new_group->add_element(line);
        }
        new_group->set_need_check(false);
        geodata->add_element_group(new_group);
    }
}

/**
 * @brief 尝试合并焊接线组
 *
 * 该函数用于尝试合并给定元素集合中的焊接线组（NoBranchLineGroup）。
 * 焊接线组是指一组线段（SketchLinePtr），这些线段在拓扑上是连续的且没有分支。
 * 通过遍历给定的元素集合，识别出所有连续的线段，并将它们合并成焊接线组。
 * 如果提供了强制分离点集合（force_separate_points），则在合并过程中跳过这些点。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] elements 需要处理的元素集合，包含SketchElementPtr类型的指针
 * @param[in] force_separate_points 强制分离的点集合，包含SketchElementPtr类型的指针
 *
 * @note 函数内部首先定义一个向量valid_line_groups，用于存储有效的线段组。
 *       定义一个集合visited_lines，用于记录已经访问过的线段。
 *       定义一个lambda函数find_valid_group，用于查找并合并连续的线段。
 *       遍历给定的元素集合，对于每个线段，如果它没有被访问过，则调用find_valid_group进行处理。
 *       对于每个识别出的有效线段组，如果组中的线段数量大于1，则创建一个新的NoBranchLineGroup对象，并将这些线段添加到新组中。
 *       最后，将所有创建的新焊接线组添加到几何数据中。
 */
void GeoDataTopoOperations::try_merge_no_branch_line_group(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements,
                                                           const std::unordered_set<SketchElementPtr>& force_separate_points)
{
    // 定义一个向量，用于存储有效的线段组
    std::vector<std::unordered_set<SketchLinePtr>> valid_line_groups;
    // 定义一个集合，用于记录已经访问过的线段
    std::unordered_set<SketchLinePtr> visited_lines;

    // 定义一个lambda函数，用于查找并合并连续的线段
    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchLinePtr>& group) {
        group.insert(line);

        std::stack<SketchLinePtr> line_stack;
        line_stack.push(line);
        while(line_stack.size())
        {
            SketchLinePtr current_line = line_stack.top();
            line_stack.pop();
            visited_lines.insert(current_line);

            SketchPointPtr p0 = current_line->p0;
            SketchPointPtr p1 = current_line->p1;

            // 处理线段的第一个端点
            if(force_separate_points.find(p0) == force_separate_points.end())
            {
                std::unordered_set<SketchLinePtr> p0_adj_lines = geodata->get_point_adj_lines(p0);
                p0_adj_lines.erase(current_line);
                std::unordered_set<SketchLinePtr> p0_valid_adj_lines;
                for(auto adj_line: p0_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end())
                    {
                        p0_valid_adj_lines.insert(adj_line);
                    }
                }
                if(p0_valid_adj_lines.size() == 1)
                {
                    SketchLinePtr valid_line = *p0_valid_adj_lines.begin();
                    if(elements.find(valid_line) != elements.end() && visited_lines.find(valid_line) == visited_lines.end())
                    {
                        group.insert(valid_line);
                        line_stack.push(valid_line);
                    }
                }
            }

            // 处理线段的第二个端点
            if(force_separate_points.find(p1) == force_separate_points.end())
            {
                std::unordered_set<SketchLinePtr> p1_adj_lines = geodata->get_point_adj_lines(p1);
                p1_adj_lines.erase(current_line);
                std::unordered_set<SketchLinePtr> p1_valid_adj_lines;
                for(auto adj_line: p1_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end())
                    {
                        p1_valid_adj_lines.insert(adj_line);
                    }
                }
                if(p1_valid_adj_lines.size() == 1)
                {
                    SketchLinePtr valid_line = *p1_valid_adj_lines.begin();
                    if(elements.find(valid_line) != elements.end() && visited_lines.find(valid_line) == visited_lines.end())
                    {
                        group.insert(valid_line);
                        line_stack.push(valid_line);
                    }
                }
            }
        }
    };

    // 遍历给定的元素集合
    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            if(visited_lines.find(line) == visited_lines.end())
            {
                std::unordered_set<SketchLinePtr> valid_group;
                find_valid_group(line, valid_group);
                valid_line_groups.push_back(std::move(valid_group));
            }
        }
    }

    // 遍历所有识别出的有效线段组
    for(auto& group: valid_line_groups)
    {
        if(group.size() > 1)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto line: group)
            {
                new_group->add_element(line);
            }
            new_group->set_need_check(false);
            geodata->add_element_group(new_group);
        }
    }
}

/**
 * @brief 尝试合并焊接线组（版本2）
 *
 * 该函数用于尝试合并给定元素集合中的焊接线组（NoBranchLineGroup）。
 * 焊接线组是指一组线段（SketchLinePtr），这些线段在拓扑上是连续的且没有分支。
 * 通过遍历给定的元素集合，识别出所有连续的线段，并将它们合并成焊接线组。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] elements 需要处理的元素集合，包含SketchElementPtr类型的指针
 *
 * @note 函数内部首先定义一个向量valid_line_groups，用于存储有效的线段组。
 *       定义一个集合visited_lines，用于记录已经访问过的线段。
 *       定义一个lambda函数find_valid_group，用于查找并合并连续的线段。
 *       遍历给定的元素集合，对于每个线段，如果它没有被访问过，则调用find_valid_group进行处理。
 *       对于每个识别出的有效线段组，如果组中的线段数量大于1，则创建一个新的NoBranchLineGroup对象，并将这些线段添加到新组中。
 *       最后，将所有创建的新焊接线组添加到几何数据中。
 */
void GeoDataTopoOperations::try_merge_no_branch_line_group_2(GeoData* geodata, const std::unordered_set<SketchElementPtr>& elements)
{
    // 定义一个向量，用于存储有效的线段组
    std::vector<std::unordered_set<SketchLinePtr>> valid_line_groups;
    // 定义一个集合，用于记录已经访问过的线段
    std::unordered_set<SketchLinePtr> visited_lines;

    // 定义一个lambda函数，用于查找并合并连续的线段
    auto find_valid_group = [&](SketchLinePtr line, std::unordered_set<SketchLinePtr>& group) {
        group.insert(line);

        std::stack<SketchLinePtr> line_stack;
        line_stack.push(line);
        while(line_stack.size())
        {
            SketchLinePtr current_line = line_stack.top();
            line_stack.pop();
            visited_lines.insert(current_line);

            SketchPointPtr p0 = current_line->p0;
            SketchPointPtr p1 = current_line->p1;

            // 处理线段的第一个端点
            std::unordered_set<SketchLinePtr> p0_adj_lines = geodata->get_point_adj_lines(p0);
            if(p0_adj_lines.size() == 2)
            {
                for(auto adj_line: p0_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end() && visited_lines.find(adj_line) == visited_lines.end())
                    {
                        group.insert(adj_line);
                        line_stack.push(adj_line);
                    }
                }
            }

            // 处理线段的第二个端点
            std::unordered_set<SketchLinePtr> p1_adj_lines = geodata->get_point_adj_lines(p1);
            if(p1_adj_lines.size() == 2)
            {
                for(auto adj_line: p1_adj_lines)
                {
                    if(elements.find(adj_line) != elements.end() && visited_lines.find(adj_line) == visited_lines.end())
                    {
                        group.insert(adj_line);
                        line_stack.push(adj_line);
                    }
                }
            }
        }
    };

    // 遍历给定的元素集合
    for(SketchElementPtr ele: elements)
    {
        if(ele->type() == EElementType::Line)
        {
            SketchLinePtr line = ele->to_line();
            if(visited_lines.find(line) == visited_lines.end())
            {
                std::unordered_set<SketchLinePtr> valid_group;
                find_valid_group(line, valid_group);
                valid_line_groups.push_back(std::move(valid_group));
            }
        }
    }

    // 遍历所有识别出的有效线段组
    for(auto& group: valid_line_groups)
    {
        if(group.size() > 1)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto line: group)
            {
                new_group->add_element(line);
            }
            new_group->set_need_check(false);
            geodata->add_element_group(new_group);
        }
    }
}

/**
 * @brief 将线段添加到焊接线组
 *
 * 该函数用于将一组线段（由起点和终点坐标表示）添加到指定的焊接线组（NoBranchLineGroup）中。
 * 如果未提供焊接线组，则创建一个新的焊接线组。
 * 通过遍历每条线段，将其添加到几何数据中，并将生成的线段对象添加到焊接线组中。
 * 最后，将焊接线组添加到几何数据中，并清除强制分离的点集合。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] lines 需要添加的线段集合，包含每条线段的起点和终点坐标（nous::vec3类型）
 * @param[in] group 指定的焊接线组对象指针，如果为nullptr，则创建一个新的焊接线组
 *
 * @return ElementGroupPtr 返回包含添加线段的焊接线组对象指针
 *
 * @note 函数内部首先检查是否提供了焊接线组对象指针。
 *       如果未提供，则创建一个新的NoBranchLineGroup对象。
 *       然后遍历每条线段，调用geodata->add_line方法将其添加到几何数据中。
 *       对于每条线段，生成的线段对象会被添加到焊接线组中。
 *       最后，将焊接线组添加到几何数据中，并清除焊接线组中的强制分离点集合。
 */
ElementGroupPtr GeoDataTopoOperations::add_lines_to_no_branch_line_group(GeoData* geodata,
                                                                         const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                                                         ElementGroupPtr group)
{
    // 如果提供了焊接线组对象指针，则将其转换为NoBranchLineGroupPtr
    // 否则，创建一个新的NoBranchLineGroup对象
    NoBranchLineGroupPtr new_group = group ? std::dynamic_pointer_cast<NoBranchLineGroup>(group) : std::make_shared<NoBranchLineGroup>();

    // 遍历每条线段
    for(auto& line_pos: lines)
    {
        // 将线段添加到几何数据中，并获取生成的线段对象集合
        std::vector<SketchLinePtr> current_new_lines = geodata->add_line(line_pos.first, line_pos.second, false);
        // 将生成的每条线段对象添加到焊接线组中
        for(auto line: current_new_lines)
        {
            new_group->add_element(line);
        }
    }

    // 将焊接线组添加到几何数据中
    geodata->add_element_group(new_group);
    // 清除焊接线组中的强制分离点集合
    new_group->clear_force_separate_points();

    // 返回包含添加线段的焊接线组对象指针
    return new_group;
}

/**
 * @brief 将线段添加到平滑组
 *
 * 该函数用于将一组线段（由起点和终点坐标表示）添加到指定的平滑组（SmoothGroup）中。
 * 如果未提供平滑组，则创建一个新的平滑组。
 * 通过遍历每条线段，将其添加到几何数据中，并将生成的线段对象添加到平滑组中。
 * 最后，将平滑组添加到几何数据中。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] lines 需要添加的线段集合，包含每条线段的起点和终点坐标（nous::vec3类型）
 * @param[in] group 指定的平滑组对象指针，如果为nullptr，则创建一个新的平滑组
 *
 * @return ElementGroupPtr 返回包含添加线段的平滑组对象指针
 *
 * @note 函数内部首先检查是否提供了平滑组对象指针。
 *       如果未提供，则创建一个新的SmoothGroup对象。
 *       然后遍历每条线段，调用geodata->add_line方法将其添加到几何数据中。
 *       对于每条线段，生成的线段对象会被添加到平滑组中。
 *       最后，将平滑组添加到几何数据中。
 */
ElementGroupPtr GeoDataTopoOperations::add_lines_to_smooth_group(GeoData* geodata, const std::vector<std::pair<nous::vec3, nous::vec3>>& lines,
                                                                 ElementGroupPtr group)
{
    // 如果提供了平滑组对象指针，则将其转换为SmoothGroupPtr
    // 否则，创建一个新的SmoothGroup对象
    SmoothGroupPtr new_group = group ? std::dynamic_pointer_cast<SmoothGroup>(group) : std::make_shared<SmoothGroup>();

    // 遍历每条线段
    for(auto& line_pos: lines)
    {
        // 将线段添加到几何数据中，并获取生成的线段对象集合
        std::vector<SketchLinePtr> current_new_lines = geodata->add_line(line_pos.first, line_pos.second, false);
        // 将生成的每条线段对象添加到平滑组中
        for(auto line: current_new_lines)
        {
            new_group->add_element(line);
        }
    }

    // 将平滑组添加到几何数据中
    geodata->add_element_group(new_group);

    // 返回包含添加线段的平滑组对象指针
    return new_group;
}

/**
 * @brief 检查并重新生成无分支线组
 *
 * 该函数用于检查几何数据中的所有无分支线组（NoBranchLineGroup），并根据需要重新生成这些线组。
 * 如果无分支线组中的线段数量大于1，则尝试合并这些线段。
 * 合并过程中会考虑强制分离的点（force_separate_points），以确保这些点不会被合并。
 * 最后，清理并删除重新生成过的无分支线组。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 *
 * @note 函数内部首先遍历几何数据中的所有元素组。
 *       如果元素组的类型为无分支线组（NoBranchLineGroup）且需要检查（need_check()返回true），则进行进一步处理。
 *       获取无分支线组中的所有线段和强制分离的点。
 *       如果线段数量大于1，则调用try_merge_no_branch_line_group函数尝试合并这些线段。
 *       将需要重新生成的无分支线组添加到old_groups集合中。
 *       最后，遍历old_groups集合，删除重新生成过的无分支线组。
 */
void GeoDataTopoOperations::check_and_recreate_no_branch_line_group(GeoData* geodata)
{
    std::vector<ElementGroupPtr> old_groups; ///< 存储需要重新生成的无分支线组

    // 遍历几何数据中的所有元素组
    for(auto group: geodata->get_element_groups())
    {
        // 检查元素组是否为无分支线组且需要检查
        if(group->type() == EElementType::NoBranchLineGroup && group->need_check())
        {
            NoBranchLineGroupPtr no_branch_group = std::dynamic_pointer_cast<NoBranchLineGroup>(group); ///< 将元素组转换为无分支线组
            std::unordered_set<SketchElementPtr> lines = group->elements(); ///< 获取无分支线组中的所有线段
            std::unordered_set<SketchElementPtr> force_separate_points = no_branch_group->force_separate_points(); ///< 获取强制分离的点

            // 如果线段数量大于1，则尝试合并这些线段
            if(lines.size() > 1)
            {
                // group在此会被切断
                this->try_merge_no_branch_line_group(geodata, lines, force_separate_points); ///< 尝试合并线段
            }

            old_groups.push_back(group); ///< 将需要重新生成的无分支线组添加到old_groups集合中
        }
    }

    // 清理重新生成过的组
    for(auto group: old_groups)
    {
        // 如果组为空，则删除该组
        if(group->empty())
        {
            geodata->delete_element_group(group); ///< 删除重新生成过的无分支线组
        }
    }
}

/**
 * @brief 检查并重新生成平滑组
 *
 * 该函数用于检查几何数据中的所有平滑组（SmoothGroup），并根据需要重新生成这些平滑组。
 * 如果平滑组中的软边（soft_edge）为空，则删除该平滑组。
 * 如果平滑组需要检查（need_check()返回true），则删除该平滑组并尝试合并其中的软边。
 * 合并过程中会考虑是否强制分离的点（force_separate_points），以确保这些点不会被合并。
 *
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 *
 * @note 函数内部首先遍历几何数据中的所有元素组。
 *       如果元素组的类型为平滑组（SmoothGroup）且需要检查（need_check()返回true），则进行进一步处理。
 *       获取平滑组中的所有软边。
 *       如果软边为空，则删除该平滑组。
 *       如果软边不为空，则删除该平滑组并调用try_merge_smooth_group函数尝试合并这些软边。
 */
void GeoDataTopoOperations::check_and_recreate_smooth_group(GeoData* geodata)
{
    // 遍历几何数据中的所有元素组
    for(auto group: geodata->get_element_groups())
    {
        // 检查元素组是否为平滑组且需要检查
        if(group->type() == EElementType::SmoothGroup && group->need_check())
        {
            SmoothGroupPtr smooth_group = std::dynamic_pointer_cast<SmoothGroup>(group); ///< 将元素组转换为平滑组

            // 如果平滑组中的软边为空，则删除该平滑组
            if(smooth_group->soft_edge().empty())
            {
                geodata->delete_element_group(group); ///< 删除平滑组
            }
            else
            {
                // 获取平滑组中的所有软边
                std::unordered_set<SketchElementPtr> lines = smooth_group->soft_edge();
                geodata->delete_element_group(group); ///< 删除平滑组
                // 尝试合并平滑组中的软边
                this->try_merge_smooth_group(geodata, lines, false);
            }
        }
    }
}

/**
 * @brief 尝试对焊接线元素进行排序
 * 
 * 该函数接收一组素描元素（SketchElementPtr），并尝试将其中的线段（SketchLinePtr）按照焊接顺序排列。
 * 排序结果为一个向量，包含一对线段指针和布尔值。布尔值表示线段是否需要翻转以保持正确的焊接顺序。
 * 
 * @param line_elements 包含SketchElementPtr类型元素的无序集合，代表待排序的线段元素
 * @return std::vector<std::pair<SketchLinePtr, bool>> 返回排序后的线段及其是否需要翻转的标志
 */
std::vector<std::pair<SketchLinePtr, bool>> GeoDataTopoOperations::try_order_weld_line_elements(
    const std::unordered_set<SketchElementPtr>& line_elements)
{
    // 定义结果容器，用于存储排序后的线段及其翻转标志
    std::vector<std::pair<SketchLinePtr, bool>> result;

    // 创建剩余未处理元素的副本，以便在排序过程中逐步移除已处理的元素
    std::unordered_set<SketchElementPtr> remained_elements(line_elements.begin(), line_elements.end());

    // 从集合中选取第一条线段作为起点，并将其从剩余元素中移除
    SketchLinePtr first_line = (*remained_elements.begin())->to_line();
    remained_elements.erase(first_line);
    result.emplace_back(first_line, false); // 将第一条线段添加到结果中，初始时不需要翻转

    // 初始化左右端点标记，分别指向第一条线段的两个端点
    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    // 当仍有未处理的元素时，继续循环处理
    while (!remained_elements.empty())
    {
        SketchLinePtr picked_line = nullptr; // 用于记录本轮循环中选中的线段

        // 遍历剩余元素，寻找与当前左右端点匹配的线段
        for (auto& element : remained_elements)
        {
            SketchLinePtr line = element->to_line();
            if (line)
            {
                // 如果找到与左端点匹配的线段，则根据匹配情况决定插入位置及是否翻转
                if (line->p0 == left_pt_marker)
                {
                    result.insert(result.begin(), std::make_pair(line, true)); // 翻转后插入开头
                    left_pt_marker = line->p1; // 更新左端点标记
                    picked_line = line;
                }
                else if (line->p1 == left_pt_marker)
                {
                    result.insert(result.begin(), std::make_pair(line, false)); // 不翻转插入开头
                    left_pt_marker = line->p0; // 更新左端点标记
                    picked_line = line;
                }
                // 如果找到与右端点匹配的线段，则根据匹配情况决定插入位置及是否翻转
                else if (line->p0 == right_pt_marker)
                {
                    result.emplace_back(line, false); // 不翻转插入结尾
                    right_pt_marker = line->p1; // 更新右端点标记
                    picked_line = line;
                }
                else if (line->p1 == right_pt_marker)
                {
                    result.emplace_back(line, true); // 翻转后插入结尾
                    right_pt_marker = line->p0; // 更新右端点标记
                    picked_line = line;
                }
            }
        }

        // 如果没有选中任何线段，说明可能存在非线段元素或无法形成连续路径，退出循环
        if (!picked_line)
        {
            break;
        }

        // 从剩余元素中移除本轮选中的线段
        remained_elements.erase(picked_line);
    }

    return result; // 返回排序后的线段及其翻转标志
}

/**
 * @brief 尝试对焊接线元素进行排序
 * 
 * 该函数接收一组素描元素（SketchElementPtr），并尝试将其中的线段（SketchLinePtr）按照焊接顺序排列。
 * 排序结果为一个向量，包含一对线段指针和布尔值。布尔值表示线段是否需要翻转以保持正确的焊接顺序。
 * 
 * @param geodata GeoData对象的智能指针，提供几何数据访问接口
 * @param line_elements 包含SketchElementPtr类型元素的无序集合，代表待排序的线段元素
 * @return std::vector<std::pair<SketchLinePtr, bool>> 返回排序后的线段及其是否需要翻转的标志
 */
std::vector<std::pair<SketchLinePtr, bool>> GeoDataTopoOperations::try_order_weld_line_elements(
    const std::shared_ptr<GeoData> geodata, const std::unordered_set<SketchElementPtr>& line_elements)
{
    // 定义结果容器，用于存储排序后的线段及其翻转标志
    std::vector<std::pair<SketchLinePtr, bool>> result;

    // 创建已访问线段的集合，避免重复处理
    std::unordered_set<SketchLinePtr> visited_lines;

    // 从集合中选取第一条线段作为起点，并将其标记为已访问
    SketchLinePtr first_line = (*line_elements.begin())->to_line();
    result.emplace_back(first_line, false); // 初始时不需要翻转
    visited_lines.insert(first_line);

    // 初始化左右端点标记，分别指向第一条线段的两个端点
    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    // 处理左端点，尝试找到连接的下一条线段
    auto&& adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    while (adj_lines.size() > 1) // 当存在多于一条相邻线段时继续循环
    {
        SketchLinePtr next_line = nullptr;
        for (const SketchLinePtr& adj_line : adj_lines) // 遍历所有相邻线段
        {
            if (visited_lines.find(adj_line) != visited_lines.end()) // 如果线段已经被访问过，则跳过
            {
                continue;
            }
            if (line_elements.find(adj_line) != line_elements.end()) // 如果相邻线段属于待排序的线段集合
            {
                next_line = adj_line; // 找到下一条线段
                break;
            }
        }

        if (!next_line) // 如果没有找到新的线段，退出循环
        {
            break;
        }

        // 根据找到的线段更新左端点标记，并根据线段的方向决定是否需要翻转
        if (next_line->p0 == left_pt_marker)
        {
            result.insert(result.begin(), std::make_pair(next_line, true)); // 翻转后插入开头
            left_pt_marker = next_line->p1;
        }
        else if (next_line->p1 == left_pt_marker)
        {
            result.insert(result.begin(), std::make_pair(next_line, false)); // 不翻转插入开头
            left_pt_marker = next_line->p0;
        }

        // 更新相邻线段集合为新左端点的相邻线段
        adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    }

    // 处理右端点，尝试找到连接的下一条线段
    adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    while (adj_lines.size() > 1) // 当存在多于一条相邻线段时继续循环
    {
        SketchLinePtr next_line = nullptr;
        for (const SketchLinePtr& adj_line : adj_lines) // 遍历所有相邻线段
        {
            if (visited_lines.find(adj_line) != visited_lines.end()) // 如果线段已经被访问过，则跳过
            {
                continue;
            }
            if (line_elements.find(adj_line) != line_elements.end()) // 如果相邻线段属于待排序的线段集合
            {
                next_line = adj_line; // 找到下一条线段
                break;
            }
        }

        if (!next_line) // 如果没有找到新的线段，退出循环
        {
            break;
        }

        // 根据找到的线段更新右端点标记，并根据线段的方向决定是否需要翻转
        if (next_line->p0 == right_pt_marker)
        {
            result.emplace_back(next_line, false); // 不翻转插入结尾
            right_pt_marker = next_line->p1;
        }
        else if (next_line->p1 == right_pt_marker)
        {
            result.emplace_back(next_line, true); // 翻转后插入结尾
            right_pt_marker = next_line->p0;
        }

        // 将新找到的线段标记为已访问，并更新相邻线段集合为新右端点的相邻线段
        visited_lines.insert(next_line);
        adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    }

    return result; // 返回排序后的线段及其翻转标志
}

/**
 * @brief 尝试获取焊接线元素的位置数组
 * 
 * 该函数接收一组素描元素（SketchElementPtr），并尝试构建一个位置数组，表示这些线段在焊接时的顺序。
 * 函数会从给定的线段集合中选择一条起始线段，并沿着相邻的线段追踪，直到无法继续为止。
 * 追踪过程中，它会记录每个点的位置，并确保不会重复访问相同的线段。
 * 
 * @param geodata GeoData对象的智能指针，提供几何数据访问接口
 * @param line_elements 包含SketchElementPtr类型元素的无序集合，代表待处理的线段元素
 * @return std::vector<nous::pos_t> 返回包含所有线段端点位置的位置数组
 */
std::vector<nous::pos_t> GeoDataTopoOperations::try_get_weld_line_position_array(const std::shared_ptr<GeoData> geodata,
                                                                                 const std::unordered_set<SketchElementPtr>& line_elements)
{
    // 定义结果容器，用于存储线段端点的位置
    std::vector<nous::pos_t> result;

    // 创建已访问线段的集合，避免重复处理
    std::unordered_set<SketchLinePtr> visited_lines;

    // 从集合中选取第一条线段作为起点，并将其标记为已访问
    SketchLinePtr first_line = (*line_elements.begin())->to_line();
    visited_lines.insert(first_line);

    // 初始化左右端点标记，分别指向第一条线段的两个端点
    SketchPointPtr left_pt_marker = first_line->p0;
    SketchPointPtr right_pt_marker = first_line->p1;

    // 获取左右端点的位置，并添加到结果列表中
    nous::pos_t left_pt_pos = geodata->get_point_position(left_pt_marker);
    nous::pos_t right_pt_pos = geodata->get_point_position(right_pt_marker);
    result.emplace_back(left_pt_pos);
    result.emplace_back(right_pt_pos);

    // 处理左端点，尝试找到连接的下一条线段
    auto&& adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    while(adj_lines.size() > 1) // 当存在多于一条相邻线段时继续循环
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines) // 遍历所有相邻线段
        {
            if(visited_lines.find(adj_line) != visited_lines.end()) // 如果线段已经被访问过，则跳过
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end()) // 如果相邻线段属于待处理的线段集合
            {
                next_line = adj_line; // 找到下一条线段
                break;
            }
        }

        if(!next_line) // 如果没有找到新的线段，退出循环
        {
            break;
        }

        // 更新左端点标记，并根据新线段的方向更新位置数组
        if(next_line->p0 == left_pt_marker)
        {
            left_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == left_pt_marker)
        {
            left_pt_marker = next_line->p0;
        }
        else
        {
            break; // 如果新线段不与当前端点相连，退出循环
        }

        visited_lines.insert(next_line); // 标记新线段为已访问
        result.insert(result.begin(), geodata->get_point_position(left_pt_marker)); // 在结果列表开头插入新端点的位置

        // 更新相邻线段集合为新左端点的相邻线段
        adj_lines = geodata->get_point_adj_lines(left_pt_marker);
    }

    // 处理右端点，尝试找到连接的下一条线段
    adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    while(adj_lines.size() > 1) // 当存在多于一条相邻线段时继续循环
    {
        SketchLinePtr next_line = nullptr;
        for(const SketchLinePtr& adj_line: adj_lines) // 遍历所有相邻线段
        {
            if(visited_lines.find(adj_line) != visited_lines.end()) // 如果线段已经被访问过，则跳过
            {
                continue;
            }
            if(line_elements.find(adj_line) != line_elements.end()) // 如果相邻线段属于待处理的线段集合
            {
                next_line = adj_line; // 找到下一条线段
                break;
            }
        }

        if(!next_line) // 如果没有找到新的线段，退出循环
        {
            break;
        }

        // 更新右端点标记，并根据新线段的方向更新位置数组
        if(next_line->p0 == right_pt_marker)
        {
            right_pt_marker = next_line->p1;
        }
        else if(next_line->p1 == right_pt_marker)
        {
            right_pt_marker = next_line->p0;
        }
        else
        {
            break; // 如果新线段不与当前端点相连，退出循环
        }

        visited_lines.insert(next_line); // 标记新线段为已访问
        result.emplace_back(geodata->get_point_position(right_pt_marker)); // 在结果列表末尾添加新端点的位置

        // 更新相邻线段集合为新右端点的相邻线段
        adj_lines = geodata->get_point_adj_lines(right_pt_marker);
    }

    return result; // 返回包含所有线段端点位置的位置数组
}

namespace convert
{

/**
 * @brief 将多边形转换为GEOS几何对象，并生成逆映射表
 * 
 * 该函数将一个多边形（由SketchPointPtr类型的点集表示）转换为GEOS库中的几何对象（Polygon）。
 * 在转换过程中，多边形会被旋转到XY平面上，并生成一个逆映射表，用于将GEOS几何对象中的坐标映射回原始的SketchPointPtr。
 * 如果生成的多边形无效，则尝试通过差集操作修复多边形。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 多边形的拓扑结构，包含多个环（第一个为外环，后续为内环），每个环由SketchPointPtr类型的点集表示
 * @param[in] ref_normal 多边形的参考法向量
 * @param[out] revert_map 输出的逆映射表，将GEOS几何对象中的坐标映射回原始的SketchPointPtr
 * 
 * @return geos::geom::Geometry* 返回转换后的GEOS几何对象指针，如果转换失败则返回nullptr
 * 
 * @note 函数内部首先检查输入参数的有效性，包括GeoData对象、多边形的结构和参考法向量。
 *       然后，根据参考法向量计算旋转矩阵，将多边形旋转到XY平面上。
 *       接着，遍历多边形的外环和内环，将每个点的位置转换为GEOS坐标，并添加到相应的坐标序列中。
 *       创建外环和内环的GEOS LinearRing对象，并使用这些对象创建多边形的GEOS Polygon对象。
 *       如果生成的多边形无效，则通过差集操作修复多边形。
 *       最后，返回转换后的GEOS几何对象指针，并生成逆映射表。
 */
geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    // 创建GEOS几何工厂对象
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    // 检查输入参数的有效性
    if(!geodata || polygon.empty() || polygon.front().size() < 3)
    {
        return nullptr; // 如果输入无效，返回nullptr
    }

    // 处理参考法向量，确保其不与Z轴完全平行
    nous::vec3 act_normal = ref_normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1); // 如果参考法向量与Z轴平行，将其设置为Z轴方向
    }

    // 计算从实际法向量到XY平面的旋转矩阵
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    // 处理外环
    geos::geom::CoordinateSequence exring_coordinates; // 创建外环坐标序列
    for(auto one_point: polygon.front()) // 遍历外环中的每个点
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point); // 获取点的原始位置
        nous::vec3 p = rotate_to_xy_plane * point_origin; // 将点旋转到XY平面
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z()); // 创建GEOS坐标对象
        exring_coordinates.add(cd); // 将坐标添加到外环坐标序列中
        revert_map.insert({ cd, one_point }); // 将GEOS坐标与原始点添加到逆映射表中
    }
    exring_coordinates.add(exring_coordinates.front()); // 闭合外环
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates); // 创建外环的GEOS LinearRing对象

    // 处理内环
    std::vector<geos::geom::LinearRing*> polygon_geom_inrings; // 创建内环的GEOS LinearRing对象集合
    for(int i = 1; i < polygon.size(); ++i) // 遍历每个内环
    {
        geos::geom::CoordinateSequence inring_coordinates; // 创建内环坐标序列
        for(auto one_point: polygon[i]) // 遍历内环中的每个点
        {
            nous::vec3 point_origin = geodata->get_point_position(one_point); // 获取点的原始位置
            nous::vec3 p = rotate_to_xy_plane * point_origin; // 将点旋转到XY平面
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z()); // 创建GEOS坐标对象
            inring_coordinates.add(cd); // 将坐标添加到内环坐标序列中
            revert_map.insert({ cd, one_point }); // 将GEOS坐标与原始点添加到逆映射表中
        }
        inring_coordinates.add(inring_coordinates.front()); // 闭合内环
        geos::geom::LinearRing* inring = geom_factory->createLinearRing(inring_coordinates).release(); // 创建内环的GEOS LinearRing对象
        polygon_geom_inrings.emplace_back(inring); // 将内环的GEOS LinearRing对象添加到集合中
    }

    // 创建多边形的GEOS Polygon对象
    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(*exring.get(), polygon_geom_inrings);

    // 检查生成的多边形是否有效
    if(!geom_polygon->isValid())
    {
        // 尝试通过差集操作修复多边形
        geos::geom::Geometry* exring_polygon = geom_factory->createPolygon(*exring.get(), std::vector<geos::geom::LinearRing*>()); // 创建外环的GEOS Polygon对象
        for(int i = 0; i < polygon_geom_inrings.size(); ++i) // 遍历每个内环
        {
            geos::geom::Polygon* inring_polygon = geom_factory->createPolygon(*polygon_geom_inrings[i], std::vector<geos::geom::LinearRing*>()); // 创建内环的GEOS Polygon对象
            exring_polygon = exring_polygon->difference(inring_polygon).release(); // 通过差集操作修复多边形
        }

        return exring_polygon; // 返回修复后的多边形
    }

    return geom_polygon; // 返回转换后的多边形
}
}

namespace convert
{

/**
 * @brief 将多边形转换为GEOS几何对象，并生成逆映射表
 * 
 * 该函数将一个多边形（由SketchPointPtr类型的点集表示）转换为GEOS库中的几何对象（Polygon）。
 * 在转换过程中，多边形会被旋转到XY平面上，并生成一个逆映射表，用于将GEOS几何对象中的坐标映射回原始的SketchPointPtr。
 * 如果生成的多边形无效，则返回nullptr。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 多边形的拓扑结构，包含SketchPointPtr类型的点集
 * @param[in] ref_normal 多边形的参考法向量
 * @param[out] revert_map 输出的逆映射表，将GEOS几何对象中的坐标映射回原始的SketchPointPtr
 * 
 * @return geos::geom::Geometry* 返回转换后的GEOS几何对象指针，如果转换失败则返回nullptr
 * 
 * @note 函数内部首先检查输入参数的有效性，包括GeoData对象、多边形的结构和参考法向量。
 *       然后，根据参考法向量计算旋转矩阵，将多边形旋转到XY平面上。
 *       接着，遍历多边形中的每个点，将每个点的位置转换为GEOS坐标，并添加到外环坐标序列中。
 *       创建外环的GEOS LinearRing对象，并使用该对象创建多边形的GEOS Polygon对象。
 *       如果生成的多边形无效，则返回nullptr。
 *       最后，返回转换后的GEOS几何对象指针，并生成逆映射表。
 */
geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, const std::vector<SketchPointPtr> polygon,
                                                                  const nous::vec3& ref_normal,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    // 创建GEOS几何工厂对象
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    // 检查输入参数的有效性
    if(!geodata || polygon.empty() || polygon.size() < 3)
    {
        return nullptr; // 如果输入无效，返回nullptr
    }

    // 处理参考法向量，确保其不与Z轴完全平行
    nous::vec3 act_normal = ref_normal;
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1); // 如果参考法向量与Z轴平行，将其设置为Z轴方向
    }

    // 计算从实际法向量到XY平面的旋转矩阵
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    // 处理外环
    geos::geom::CoordinateSequence exring_coordinates; // 创建外环坐标序列
    for(auto one_point: polygon) // 遍历多边形中的每个点
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point); // 获取点的原始位置
        nous::vec3 p = rotate_to_xy_plane * point_origin; // 将点旋转到XY平面
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z()); // 创建GEOS坐标对象
        exring_coordinates.add(cd); // 将坐标添加到外环坐标序列中
        revert_map.insert({ cd, one_point }); // 将GEOS坐标与原始点添加到逆映射表中
    }
    exring_coordinates.add(exring_coordinates.front()); // 闭合外环
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates); // 创建外环的GEOS LinearRing对象

    // 创建多边形的GEOS Polygon对象
    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(std::move(exring)).release();

    // 检查生成的多边形是否有效
    if(!geom_polygon->isValid())
    {
        return nullptr; // 如果多边形无效，返回nullptr
    }

    return geom_polygon; // 返回转换后的多边形
}
}

/**
 * @brief 将多边形转换为GEOS几何对象，并生成逆映射表
 * 
 * 该函数将一个多边形（SketchPolygonPtr）转换为GEOS库中的几何对象（Polygon）。
 * 在转换过程中，多边形会被旋转到XY平面上，并生成一个逆映射表，用于将GEOS几何对象中的坐标映射回原始的SketchPointPtr。
 * 如果生成的多边形无效，则返回nullptr。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * @param[out] revert_map 输出的逆映射表，将GEOS几何对象中的坐标映射回原始的SketchPointPtr
 * @param[in] ref_normal 可选的参考法向量，如果提供则使用该法向量，否则使用多边形的外环法向量
 * 
 * @return geos::geom::Geometry* 返回转换后的GEOS几何对象指针，如果转换失败则返回nullptr
 * 
 * @note 函数内部首先检查输入参数的有效性，包括GeoData对象、多边形对象和外环。
 *       然后，根据参考法向量（如果提供）或多边形的外环法向量计算旋转矩阵，将多边形旋转到XY平面上。
 *       接着，遍历多边形的外环和所有内环，将每个点的位置转换为GEOS坐标，并添加到相应的坐标序列中。
 *       创建外环的GEOS LinearRing对象，并使用该对象创建多边形的GEOS Polygon对象。
 *       如果多边形有内环，则将内环的GEOS LinearRing对象添加到多边形的孔洞集合中。
 *       确保内环合并后，使用GEOS的Union操作合并所有内环。
 *       最后，创建包含合并后内环的多边形的GEOS Polygon对象，并返回该对象。
 */
geos::geom::Geometry* convert_polygon_to_geometry_with_revert_map(GeoData* geodata, SketchPolygonPtr polygon,
                                                                  std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map,
                                                                  const nous::vec3* ref_normal)
{
    // 创建GEOS几何工厂对象
    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model);

    // 检查输入参数的有效性
    if(!polygon || !polygon->ring || !geodata)
    {
        return nullptr; // 如果输入无效，返回nullptr
    }

    // 处理参考法向量，确保其不与Z轴完全平行
    nous::vec3 act_normal = polygon->ring->normal;
    if(ref_normal)
    {
        act_normal = *ref_normal; // 如果提供了参考法向量，则使用该法向量
    }
    if(act_normal.normalized().dot(nous::vec3(0, 0, 1)) > 1.0 - 1e-5 || act_normal.normalized().dot(nous::vec3(0, 0, 1)) < -1.0 + 1e-5)
    {
        act_normal = nous::vec3(0, 0, 1); // 如果参考法向量与Z轴平行，将其设置为Z轴方向
    }

    // 计算从实际法向量到XY平面的旋转矩阵
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(act_normal, nous::vec3(0, 0, 1));

    // 处理外环
    geos::geom::CoordinateSequence exring_coordinates; // 创建外环坐标序列
    std::vector<SketchPointPtr> exring_points_arr;
    geodata->get_element_points(polygon->ring, exring_points_arr); // 获取外环的点集
    for(auto one_point: exring_points_arr) // 遍历外环中的每个点
    {
        nous::vec3 point_origin = geodata->get_point_position(one_point); // 获取点的原始位置
        nous::vec3 p = rotate_to_xy_plane * point_origin; // 将点旋转到XY平面
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z()); // 创建GEOS坐标对象
        exring_coordinates.add(cd); // 将坐标添加到外环坐标序列中
        revert_map.insert({ cd, one_point }); // 将GEOS坐标与原始点添加到逆映射表中
    }
    exring_coordinates.add(exring_coordinates.front()); // 闭合外环
    std::unique_ptr<geos::geom::LinearRing> exring = geom_factory->createLinearRing(exring_coordinates); // 创建外环的GEOS LinearRing对象

    // 处理内环
    std::vector<geos::geom::LinearRing*> polygon_geom_inrings;
    for(const auto& inner_ring: polygon->inner_rings) // 遍历多边形的每个内环
    {
        std::vector<SketchPointPtr> inring_points_arr;
        geodata->get_element_points(inner_ring, inring_points_arr); // 获取内环的点集
        geos::geom::CoordinateSequence inring_coordinates;
        for(auto one_point: inring_points_arr) // 遍历内环中的每个点
        {
            nous::vec3 point_origin = geodata->get_point_position(one_point); // 获取点的原始位置
            nous::vec3 p = rotate_to_xy_plane * point_origin; // 将点旋转到XY平面
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y(), p.z()); // 创建GEOS坐标对象
            inring_coordinates.add(cd); // 将坐标添加到内环坐标序列中
            revert_map.insert({ cd, one_point }); // 将GEOS坐标与原始点添加到逆映射表中
        }
        inring_coordinates.add(inring_coordinates.front()); // 闭合内环
        geos::geom::LinearRing* inring = geom_factory->createLinearRing(inring_coordinates).release(); // 创建内环的GEOS LinearRing对象
        polygon_geom_inrings.emplace_back(inring); // 将内环的GEOS LinearRing对象添加到内环集合中
    }

    // 确保内环合并
    std::vector<std::unique_ptr<geos::geom::Geometry>> union_polygons;
    for(auto ring: polygon_geom_inrings) // 遍历内环集合
    {
        auto polygon = geom_factory->createPolygon(std::unique_ptr<geos::geom::LinearRing>(ring)); // 创建多边形的GEOS Polygon对象
        union_polygons.emplace_back(std::move(polygon)); // 将多边形的GEOS Polygon对象添加到合并集合中
    }
    auto result = geom_factory->createGeometryCollection(std::move(union_polygons))->Union(); // 使用GEOS的Union操作合并所有内环
    geos::geom::GeometryTypeId type_id = result->getGeometryTypeId(); // 获取合并结果的几何类型ID

    std::vector<std::unique_ptr<geos::geom::Geometry>> result_geoms;
    if(const auto* result_collection = dynamic_cast<const geos::geom::GeometryCollection*>(result.get())) // 如果合并结果是几何集合
    {
        for(std::size_t i = 0; i < result_collection->getNumGeometries(); ++i) // 遍历几何集合中的每个几何对象
        {
            result_geoms.emplace_back(std::unique_ptr<geos::geom::Geometry>(result_collection->getGeometryN(i)->clone())); // 克隆几何对象并添加到结果集合中
        }
    }
    else // 如果合并结果不是几何集合
    {
        result_geoms.emplace_back(std::move(result)); // 将合并结果添加到结果集合中
    }

    // 提取外环和孔洞
    std::vector<geos::geom::LinearRing*> holes;
    for(const auto& geom: result_geoms) // 遍历结果集合中的每个几何对象
    {
        if(!geom)
            continue; // 如果几何对象为空，跳过

        // 如果是Polygon，提取外环和孔洞
        if(const auto* polygon = dynamic_cast<const geos::geom::Polygon*>(geom.get()))
        {
            // 提取外环
            const geos::geom::LinearRing* exterior = dynamic_cast<const geos::geom::LinearRing*>(polygon->getExteriorRing());
            if(exterior)
            {
                holes.push_back(const_cast<geos::geom::LinearRing*>(exterior)); // 将外环添加到孔洞集合中
            }

            // 提取孔洞
            for(std::size_t i = 0; i < polygon->getNumInteriorRing(); ++i) // 遍历多边形的每个孔洞
            {
                const geos::geom::LinearRing* interior = dynamic_cast<const geos::geom::LinearRing*>(polygon->getInteriorRingN(i));
                if(interior)
                {
                    holes.push_back(const_cast<geos::geom::LinearRing*>(interior)); // 将孔洞添加到孔洞集合中
                }
            }
        }
        // 如果是LinearRing，直接添加
        else if(const auto* ring = dynamic_cast<const geos::geom::LinearRing*>(geom.get()))
        {
            holes.push_back(const_cast<geos::geom::LinearRing*>(ring)); // 将LinearRing添加到孔洞集合中
        }
    }

    // 创建包含合并后内环的多边形的GEOS Polygon对象
    geos::geom::Polygon* geom_polygon = geom_factory->createPolygon(*exring.get(), holes);

    return geom_polygon; // 返回转换后的多边形
}

/**
 * @brief 将GEOS几何对象（Polygon）转换为多边形的拓扑结构，并生成逆映射表
 * 
 * 该函数将一个GEOS几何对象（Polygon）转换为一个多边形的拓扑结构（包含外环和内环的点集），
 * 并使用提供的逆映射表（revert_map）将GEOS几何对象中的坐标映射回原始的SketchPointPtr。
 * 如果转换失败或几何对象无效，则返回空的拓扑结构。
 * 
 * @param[in] polygon GEOS几何对象指针，表示需要转换的多边形
 * @param[out] revert_map 输出的逆映射表，将GEOS几何对象中的坐标映射回原始的SketchPointPtr
 * 
 * @return std::vector<std::vector<SketchPointPtr>> 返回转换后的多边形拓扑结构，包含外环和内环的点集
 * 
 * @note 函数内部首先检查输入的GEOS几何对象是否有效。
 *       如果无效，则返回空的拓扑结构。
 *       获取多边形的外环，并遍历外环中的每个点，使用逆映射表将GEOS坐标映射回原始的SketchPointPtr。
 *       如果GEOS坐标在逆映射表中存在，则直接映射；否则，通过比较坐标距离找到最近的点进行映射。
 *       获取多边形的所有内环，并遍历每个内环中的每个点，使用逆映射表将GEOS坐标映射回原始的SketchPointPtr。
 *       最后，返回包含外环和内环点集的多边形拓扑结构。
 */
std::vector<std::vector<SketchPointPtr>> convert_geometry_to_polygon_with_revert_map(geos::geom::Polygon* polygon,
                                                                                     std::map<geos::geom::Coordinate, SketchPointPtr>& revert_map)
{
    std::vector<std::vector<SketchPointPtr>> child_skpg_rings; // 存储转换后的多边形拓扑结构

    if(!polygon) // 检查输入的GEOS几何对象是否有效
    {
        return child_skpg_rings; // 如果无效，返回空的拓扑结构
    }

    // 获取多边形的外环
    const geos::geom::LinearRing* child_exring = polygon->getExteriorRing();
    int exring_pt_number = child_exring->getNumPoints(); // 获取外环的点数
    if(exring_pt_number < 4) // 外环至少需要4个点（包括起点和终点）
    {
        return child_skpg_rings; // 如果点数不足，返回空的拓扑结构
    }

    // 处理外环
    auto& exring_pt_arr = child_skpg_rings.emplace_back(); // 为外环创建一个新的点集
    for(int i = 0; i < exring_pt_number - 1; ++i) // 遍历外环中的每个点（不包括最后一个点，因为它与第一个点相同）
    {
        geos::geom::Coordinate coord = child_exring->getCoordinateN(i); // 获取当前点的GEOS坐标
        if(revert_map.find(coord) != revert_map.end()) // 如果GEOS坐标在逆映射表中存在
        {
            exring_pt_arr.emplace_back(revert_map.at(coord)); // 直接映射到原始的SketchPointPtr
        }
        else // 如果GEOS坐标在逆映射表中不存在
        {
            for(const auto& revert_pair: revert_map) // 遍历逆映射表
            {
                if(coord.distance(revert_pair.first) < 0.0001) // 比较坐标距离，找到最近的点
                {
                    exring_pt_arr.emplace_back(revert_pair.second); // 映射到原始的SketchPointPtr
                    break; // 找到后跳出循环
                }
            }
        }
    }

    // 处理内环
    for(int i = 0; i < polygon->getNumInteriorRing(); ++i) // 遍历多边形的每个内环
    {
        const geos::geom::LinearRing* child_inring = polygon->getInteriorRingN(i); // 获取当前内环
        auto& inring_pt_arr = child_skpg_rings.emplace_back(); // 为当前内环创建一个新的点集
        for(int j = 0; j < child_inring->getNumPoints() - 1; ++j) // 遍历内环中的每个点（不包括最后一个点，因为它与第一个点相同）
        {
            geos::geom::Coordinate coord = child_inring->getCoordinateN(j); // 获取当前点的GEOS坐标
            if(revert_map.find(coord) != revert_map.end()) // 如果GEOS坐标在逆映射表中存在
            {
                inring_pt_arr.emplace_back(revert_map.at(coord)); // 直接映射到原始的SketchPointPtr
            }
        }
    }

    return child_skpg_rings; // 返回转换后的多边形拓扑结构
}

namespace geodata_topo
{
/**
 * @brief 合并两个多边形
 * 
 * 该函数用于合并两个指定的多边形（polygon0 和 polygon1），并将合并后的结果存储在输出参数 result 中。
 * 合并过程中，首先将两个多边形转换为 GEOS 几何对象，并使用逆映射表（revert_map）记录坐标与原始点的映射关系。
 * 然后使用 GEOS 库的 Union 操作合并两个几何对象，并提取有效的多边形。
 * 最后，将合并后的多边形转换回原始的 SketchPointPtr 结构，并存储在结果中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon0 第一个多边形的拓扑结构，包含多个环（第一个为外环，后续为内环），每个环由SketchPointPtr类型的点集表示
 * @param[in] polygon1 第二个多边形的拓扑结构，包含多个环（第一个为外环，后续为内环），每个环由SketchPointPtr类型的点集表示
 * @param[in] normal 多边形的法向量
 * @param[out] result 输出的合并后的多边形拓扑结构，包含多个环（第一个为外环，后续为内环），每个环由SketchPointPtr类型的点集表示
 * 
 * @return bool 如果合并成功，返回true；否则返回false
 * 
 * @note 函数内部首先定义一个逆映射表（revert_map），用于记录坐标与原始点的映射关系。
 *       然后将两个多边形转换为 GEOS 几何对象（ring0_geometry 和 ring1_geometry），并使用逆映射表记录坐标与原始点的映射关系。
 *       使用 GEOS 库的 Union 操作合并两个几何对象，并提取有效的多边形（valid_polygons）。
 *       对于每个有效的多边形，将其转换回原始的 SketchPointPtr 结构，并存储在结果中。
 *       最后，返回合并是否成功的结果。
 */
bool polygon_union(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>> polygon0, const std::vector<std::vector<SketchPointPtr>> polygon1,
                   const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map; // 定义逆映射表，记录坐标与原始点的映射关系

    // 将第一个多边形转换为 GEOS 几何对象，并使用逆映射表记录坐标与原始点的映射关系
    geos::geom::Geometry* ring0_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon0, normal, revert_map);
    // 将第二个多边形转换为 GEOS 几何对象，并使用逆映射表记录坐标与原始点的映射关系
    geos::geom::Geometry* ring1_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon1, normal, revert_map);

    // 使用 GEOS 库的 Union 操作合并两个几何对象
    auto result_geo = ring0_geometry->Union(ring1_geometry);
    // 提取合并结果中的有效多边形
    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(result_geo.get());

    // 遍历每个有效的多边形
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        // 将 GEOS 多边形转换回原始的 SketchPointPtr 结构，并使用逆映射表
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map);
        if(result_part.size()) // 如果转换后的多边形有效
        {
            result.emplace_back(std::move(result_part)); // 将转换后的多边形添加到结果中
        }
    }

    return true; // 返回合并成功的结果
}

/**
 * @brief 检查多边形是否有效
 * 
 * 该函数用于检查指定的多边形（polygon）是否有效。
 * 通过获取多边形的顶点位置坐标集合，并使用 GEOS 库的函数判断多边形的有效性。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * 
 * @return bool 如果多边形有效，返回true；否则返回false
 * 
 * @note 函数内部首先定义一个二维数组（polygon_pos_arr），用于存储多边形的顶点位置坐标集合。
 *       然后通过调用GeoDataUtils::get().get_polygon_positions获取多边形的顶点位置坐标集合，并存储在polygon_pos_arr中。
 *       最后，使用geos_util::is_polygon_valid函数判断多边形的有效性，并返回结果。
 */
bool is_polygon_valid(GeoData* geodata, SketchPolygonPtr polygon)
{
    std::vector<std::vector<nous::pos_t>> polygon_pos_arr; // 定义二维数组，存储多边形的顶点位置坐标集合
    GeoDataUtils::get().get_polygon_positions(geodata, polygon, polygon_pos_arr); // 获取多边形的顶点位置坐标集合
    return geos_util::is_polygon_valid(polygon_pos_arr); // 使用GEOS库判断多边形的有效性，并返回结果
}

/**
 * @brief 计算一个环与其余多个环的不重合部分
 * 
 * 该函数用于计算指定环（ring）与其余多个环（others）的不重合部分。
 * 通过将环转换为 GEOS 几何对象，并使用 GEOS 库的差集操作来计算不重合部分。
 * 最终将结果转换回环的点集形式，并存储在输出参数 result 中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定的环，包含SketchPointPtr类型的点集
 * @param[in] others 其余多个环的集合，每个环由SketchPointPtr类型的点集表示
 * @param[in] normal 环的法向量
 * @param[out] result 输出的不重合部分的环集合，包含多个点集
 * 
 * @return bool 如果计算成功，返回true；否则返回false
 * 
 * @note 函数内部首先定义一个映射（revert_map），用于存储点与 GEOS 坐标的映射关系。
 *       将指定环转换为 GEOS 几何对象（ring_geometry）。
 *       如果指定环的几何对象无效，则返回false。
 *       遍历其余多个环，将每个环转换为 GEOS 几何对象，并使用差集操作计算不重合部分。
 *       如果某个环的几何对象无效，则清空结果并返回false。
 *       使用 GEOS 库的函数提取有效的多边形，并将这些多边形转换回环的点集形式，存储在结果中。
 */
bool ring_difference_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                                 const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    if(!geodata)
    {
        return false; ///< 如果geodata为空，返回false
    }

    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map; ///< 定义点与GEOS坐标的映射关系
    std::vector<std::vector<SketchPointPtr>> ring_as_polygon;
    ring_as_polygon.emplace_back(ring); ///< 将指定环放入一个二维向量中
    geos::geom::Geometry* ring_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, ring_as_polygon, normal, revert_map); ///< 将指定环转换为GEOS几何对象

    if(!ring_geometry)
    {
        return false; ///< 如果指定环的几何对象为空，返回false
    }

    geos::geom::Geometry* difference_geometry = ring_geometry; ///< 初始化差集几何对象为指定环的几何对象
    for(int i = 0; i < others.size(); ++i)
    {
        std::vector<std::vector<SketchPointPtr>> other_ring_as_polygon;
        other_ring_as_polygon.emplace_back(others[i]); ///< 将当前环放入一个二维向量中
        geos::geom::Geometry* other_ring_geometry =
            convert::convert_polygon_to_geometry_with_revert_map(geodata, other_ring_as_polygon, normal, revert_map); ///< 将当前环转换为GEOS几何对象

        if(other_ring_geometry)
        {
            if(other_ring_geometry->isValid())
            {
                difference_geometry = difference_geometry->difference(other_ring_geometry).release(); ///< 计算差集
            }
            else
            {
                result.clear(); ///< 清空结果
                return false; ///< 如果当前环的几何对象无效，返回false
            }
        }
    }

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(difference_geometry); ///< 提取有效的多边形
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map); ///< 将多边形转换回环的点集形式
        if(result_part.size())
            result.emplace_back(std::move(result_part)); ///< 将结果添加到输出集合中
    }

    return true; ///< 返回true表示计算成功
}

/**
 * @brief 计算一个环与其余多个环的合并部分
 * 
 * 该函数用于计算指定环（ring）与其余多个环（others）的合并部分。
 * 通过将环转换为 GEOS 几何对象，并使用 GEOS 库的并集操作来计算合并部分。
 * 最终将结果转换回环的点集形式，并存储在输出参数 result 中。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] ring 指定的环，包含SketchPointPtr类型的点集
 * @param[in] others 其余多个环的集合，每个环由SketchPointPtr类型的点集表示
 * @param[in] normal 环的法向量
 * @param[out] result 输出的合并部分的环集合，包含多个点集
 * 
 * @return bool 如果计算成功，返回true；否则返回false
 * 
 * @note 函数内部首先定义一个映射（revert_map），用于存储点与 GEOS 坐标的映射关系。
 *       将指定环转换为 GEOS 几何对象（ring_geometry）。
 *       如果指定环的几何对象无效，则返回false。
 *       遍历其余多个环，将每个环转换为 GEOS 几何对象，并使用并集操作计算合并部分。
 *       使用 GEOS 库的函数提取有效的多边形，并将这些多边形转换回环的点集形式，存储在结果中。
 */
bool ring_union_other_rings(GeoData* geodata, const std::vector<SketchPointPtr>& ring, const std::vector<std::vector<SketchPointPtr>>& others,
                            const nous::vec3& normal, std::vector<std::vector<std::vector<SketchPointPtr>>>& result)
{
    if(!geodata)
    {
        return false; ///< 如果geodata为空，返回false
    }

    std::map<geos::geom::Coordinate, SketchPointPtr> revert_map; ///< 定义点与GEOS坐标的映射关系
    std::vector<std::vector<SketchPointPtr>> ring_as_polygon;
    ring_as_polygon.emplace_back(ring); ///< 将指定环放入一个二维向量中
    geos::geom::Geometry* ring_geometry = convert::convert_polygon_to_geometry_with_revert_map(geodata, ring_as_polygon, normal, revert_map); ///< 将指定环转换为GEOS几何对象

    if(!ring_geometry)
    {
        return false; ///< 如果指定环的几何对象为空，返回false
    }

    geos::geom::Geometry* union_geometry = ring_geometry; ///< 初始化并集几何对象为指定环的几何对象
    for(int i = 0; i < others.size(); ++i)
    {
        std::vector<std::vector<SketchPointPtr>> other_ring_as_polygon;
        other_ring_as_polygon.emplace_back(others[i]); ///< 将当前环放入一个二维向量中
        geos::geom::Geometry* other_ring_geometry =
            convert::convert_polygon_to_geometry_with_revert_map(geodata, other_ring_as_polygon, normal, revert_map); ///< 将当前环转换为GEOS几何对象

        if(other_ring_geometry)
        {
            union_geometry = union_geometry->Union(other_ring_geometry).release(); ///< 计算并集
        }
    }

    std::vector<geos::geom::Geometry*> valid_polygons = geos_util::extract_valid_polygons(union_geometry); ///< 提取有效的多边形
    for(geos::geom::Geometry* polygon: valid_polygons)
    {
        auto&& result_part = convert::convert_geometry_to_polygon_with_revert_map(static_cast<geos::geom::Polygon*>(polygon), revert_map); ///< 将多边形转换回环的点集形式
        if(result_part.size())
            result.emplace_back(std::move(result_part)); ///< 将结果添加到输出集合中
    }

    return true; ///< 返回true表示计算成功
}

/**
 * @brief 计算点与环的关系
 * 
 * 该函数用于计算指定点（point）相对于指定环（ring）的位置关系。
 * 通过将环和点投影到XY平面，并使用GEOS库来判断点是否在环内、环上或环外。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] point 需要检查的点的位置，类型为nous::pos_t
 * @param[in] ring 包含SketchPointPtr类型的点集，表示需要检查的环
 * @param[in] normal 环的法向量
 * 
 * @return topo::ESketchTopoRelation 返回点与环的关系，可能的值包括：
 *         - topo::ESketchTopoRelation::Inside：点在环内
 *         - topo::ESketchTopoRelation::OnBoundary：点在环上
 *         - topo::ESketchTopoRelation::Outside：点在环外
 * 
 * @note 函数内部首先定义一个四元数（rotate_to_xy_plane），用于将环和点投影到XY平面。
 *       然后遍历环中的每个点，获取其原始位置并进行投影，将投影后的坐标添加到环的坐标序列中。
 *       使用GEOS库创建一个环的几何对象（orgRing）和一个多边形的几何对象（orgPolygon）。
 *       将指定点投影到XY平面，并创建一个点的几何对象（testPoint）。
 *       使用GEOS库的contains和touches方法判断点与多边形的关系，并返回相应的结果。
 */
topo::ESketchTopoRelation point_ring_relation(GeoData* geodata, const nous::pos_t& point, const std::vector<SketchPointPtr>& ring,
                                              const nous::vec3& normal)
{
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0, 0, 1)); ///< 定义四元数，用于将环和点投影到XY平面
    std::vector<geos::geom::LinearRing*> holes_0; ///< 定义空洞集合

    geos::geom::CoordinateSequence ring_coord_seq; ///< 定义环的坐标序列
    for(auto np: ring)
    {
        nous::vec3 point_origin = geodata->get_point_position(np); ///< 获取环中每个点的原始位置
        nous::vec3 p = rotate_to_xy_plane * point_origin; ///< 将点投影到XY平面
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y()); ///< 创建GEOS坐标
        ring_coord_seq.add(cd); ///< 将坐标添加到环的坐标序列中
    }
    ring_coord_seq.add(ring_coord_seq.front()); ///< 将环的第一个点添加到序列末尾，形成闭合环

    geos::geom::GeometryFactory::Ptr geom_factory = geos::geom::GeometryFactory::create(sketch_geos_precision_model); ///< 创建几何工厂
    std::unique_ptr<geos::geom::LinearRing> orgRing = geom_factory->createLinearRing(std::move(ring_coord_seq)); ///< 创建环的几何对象
    geos::geom::Geometry* orgPolygon = geom_factory->createPolygon(*orgRing.get(), holes_0); ///< 创建多边形的几何对象

    nous::vec3 planePos = rotate_to_xy_plane * point; ///< 将点投影到XY平面
    geos::geom::Coordinate npcd = geos::geom::Coordinate(planePos.x(), planePos.y()); ///< 创建GEOS坐标
    std::unique_ptr<geos::geom::Point> testPoint = geom_factory->createPoint(npcd); ///< 创建点的几何对象

    if(orgPolygon->contains(testPoint.get())) ///< 判断点是否在多边形内
    {
        if(orgPolygon->touches(testPoint.get())) ///< 判断点是否在多边形边界上
        {
            return topo::ESketchTopoRelation::OnBoundary; ///< 返回点在环上
        }

        return topo::ESketchTopoRelation::Inside; ///< 返回点在环内
    }

    return topo::ESketchTopoRelation::Outside; ///< 返回点在环外
}

bool split_intersect_polygons(GeoData* geodata, const std::vector<std::vector<SketchPointPtr>>& input_polygon, const nous::vec3 input_pg_normal,
                              const std::unordered_set<SketchPolygonPtr>& coplane_polygons,
                              std::map<SketchPolygonPtr, std::vector<std::vector<std::vector<SketchPointPtr>>>>& split_pieces,
                              std::vector<std::vector<std::vector<SketchPointPtr>>>& remaining_pieces)
{
    // 存储临时生成用于拓扑运算的 Geometry 和原始 ring 对应的 skecthpolygon 之间的关系
    std::unordered_map<geos::geom::Geometry*, SketchPolygonPtr> geometry_to_prim_map;
    std::map<geos::geom::Coordinate, SketchPointPtr> ctpmap;

    // 这段代码将准备好输入的 skpolygon 和与其共面的所有 skpolygon 的 geometry
    geos::geom::Geometry* input_geom_polygon = convert::convert_polygon_to_geometry_with_revert_map(geodata, input_polygon, input_pg_normal, ctpmap);
    if(!input_geom_polygon || !input_geom_polygon->isValid())
    {
        return false;
    }

    for(const auto& polygon: coplane_polygons)
    {
        geos::geom::Geometry* geom_polygon = convert::convert_polygon_to_geometry_with_revert_map(geodata, polygon, ctpmap, &input_pg_normal);
        if(geom_polygon->isValid())
        {
            geometry_to_prim_map.insert({ geom_polygon, polygon });
        }
        else
        {
        }
    }

    // 将 input_polygon 的 geometry 对象和与其共面的所有 geometry 对象执行拓扑计算
    for(auto geom_prim_pair: geometry_to_prim_map)
    {
        geos::geom::Geometry* other_polygon = geom_prim_pair.first;

        //// 如果这个 polygon 被某个其余 polygon 完全包含，无需处理，在 addring 时会找到对应的 parent
        // if(geos_util::is_contain(other_polygon, input_geom_polygon))
        //{
        //     continue;
        // }
        //
        //// 如果这个 polygon 完全包含了其余的某个 polygon，无需处理，在其剩余部分 addring 时会找到对应的 child
        // if(geos_util::is_contain(input_geom_polygon, other_polygon))
        //{
        //     continue;
        // }

        // 若两个多边形完全不存在包含和被包含关系，则先简单判断两个多边形是否存在相交关系，若不存在相交关系，则无需进一步处理
        if(!input_geom_polygon->intersects(other_polygon))
        {
            continue;
        }

        // 这个函数会返回所有的 intersection 计算出的 polygon 的集合，将这些 polygon 转换成环，然后记录在 other_polygon 对应的外环中
        // 说明这个外环对应的 polygon 已经被切分出的一定的部分，后续需要 difference 运算进行拆分操作
        std::vector<geos::geom::Geometry*> intersection_result = geos_util::intersect_polygon(input_geom_polygon, other_polygon);

        // 为安全起见，intersects 的判断很可能包含一些【线重合】和【点重合】的情况，在 intersect_polygon 方法中会丢弃掉这些线和点
        // 所以 intersection_result 可能为空，为避免无效操作，此处需要多个判断
        if(!intersection_result.empty())
        {
            split_pieces.insert({ geom_prim_pair.second, std::vector<std::vector<std::vector<SketchPointPtr>>>() });

            // 对于每个被切分出的 polygon，将其转为 Point* 组成的 ring 对象
            // 与此同时，将这个部分与 input_geom_polygon 和 other_polygon 分别计算 difference
            for(auto child_geometry: intersection_result)
            {
                // 会内存泄露
                other_polygon = other_polygon->difference(child_geometry).release();
                input_geom_polygon = input_geom_polygon->difference(child_geometry).release();

                // 从 childpolygon 中的 Coordinate 反查 Point*
                geos::geom::Polygon* child_polygon = static_cast<geos::geom::Polygon*>(child_geometry);

                auto child_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(child_polygon, ctpmap);
                if(child_pg_pt_arr.size())
                    split_pieces[geom_prim_pair.second].emplace_back(std::move(child_pg_pt_arr));
            }

            // 全部裁切完毕后，other_polygon 的剩余部分也需要重新划分成不同的 polygon 加入返回的字典中
            std::vector<geos::geom::Geometry*> splited_polygon_parts = geos_util::extract_valid_polygons(other_polygon);
            for(auto splited_geometry: splited_polygon_parts)
            {
                geos::geom::Polygon* splited_polygon = static_cast<geos::geom::Polygon*>(splited_geometry);

                auto splited_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(splited_polygon, ctpmap);
                if(splited_pg_pt_arr.size())
                    split_pieces[geom_prim_pair.second].emplace_back(std::move(splited_pg_pt_arr));
            }
        }
    }

    // 此时 inputpolygon 已经和与其共面的 polygon 全部 intersection 解算完毕，剩余的部分作为单独的环加入其中
    std::vector<geos::geom::Geometry*> remained_geometries = geos_util::extract_valid_polygons(input_geom_polygon);
    for(auto remain_geometry: remained_geometries)
    {
        geos::geom::Polygon* remain_polygon = static_cast<geos::geom::Polygon*>(remain_geometry);

        auto remain_pg_pt_arr = convert::convert_geometry_to_polygon_with_revert_map(remain_polygon, ctpmap);
        if(remain_pg_pt_arr.size())
            remaining_pieces.emplace_back(std::move(remain_pg_pt_arr));
    }

    return true;
}

std::vector<std::vector<nous::vec3>> triangulate_polygon_with_intersect(std::vector<std::vector<nous::vec3>>& polygon)
{
    std::vector<std::vector<nous::vec3>> result;
    nous::vec3 normal = sketch_math::ring_normal(polygon.front());
    geos::geom::GeometryFactory::Ptr geos_factory = geos::geom::GeometryFactory::create();

    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f));
    float plane_z = 0.0f;

    std::map<geos::geom::Coordinate, nous::vec3> geopos_pos_map;
    std::vector<geos::geom::LinearRing*> holes_0;
    geos::geom::CoordinateSequence out_rs;
    for(auto pos: polygon.front())
    {
        nous::vec3 p = rotate_to_xy_plane * pos;
        plane_z = p.z();
        geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y());
        out_rs.add(cd);
        geopos_pos_map[cd] = pos;
    }
    out_rs.add(out_rs.front());
    std::unique_ptr<geos::geom::LinearRing> outerRing = geos_factory->createLinearRing(std::move(out_rs));
    geos::geom::Geometry* geosOuterPolygon = geos_factory->createPolygon(*outerRing.get(), holes_0);

    geos::geom::Geometry* inner_mergePolygon = geos_factory->createPolygon().release();
    for(int i = 1; i < polygon.size(); ++i)
    {
        geos::geom::CoordinateSequence in_rs;
        for(const auto& pos: polygon[i])
        {
            nous::vec3 p = rotate_to_xy_plane * pos;
            geos::geom::Coordinate cd = geos::geom::Coordinate(p.x(), p.y());
            in_rs.add(cd);
            geopos_pos_map[cd] = pos;
        }
        in_rs.add(in_rs.front());

        std::unique_ptr<geos::geom::LinearRing> exRing = geos_factory->createLinearRing(std::move(in_rs));
        geos::geom::Geometry* geosInnerPolygon = geos_factory->createPolygon(*exRing.get(), holes_0);
        inner_mergePolygon = inner_mergePolygon->Union(geosInnerPolygon).release();
    }

    geos::operation::polygonize::Polygonizer polygonizerOut;
    polygonizerOut.add(geosOuterPolygon);
    std::unique_ptr<geos::geom::GeometryCollection> outPolygons = geos_factory->createGeometryCollection(polygonizerOut.getPolygons());
    std::unique_ptr<geos::geom::Geometry> mergedOutPolys = outPolygons->Union();

    geos::operation::polygonize::Polygonizer polygonizerIn;
    polygonizerIn.add(inner_mergePolygon);
    std::unique_ptr<geos::geom::GeometryCollection> inPolygons = geos_factory->createGeometryCollection(polygonizerIn.getPolygons());
    std::unique_ptr<geos::geom::Geometry> mergedInPolys = inPolygons->Union();

    const geos::geom::Geometry* geosResult = mergedOutPolys->difference(mergedInPolys.get()).release();

    geos::geom::GeometryTypeId type_id = geosResult->getGeometryTypeId();
    std::vector<const geos::geom::Geometry*> geometrys;
    if(type_id == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION || type_id == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON)
    {
        int gnum = geosResult->getNumGeometries();
        for(int i = 0; i < gnum; ++i)
        {
            auto child_geom = geosResult->getGeometryN(i);
            if(child_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_POLYGON)
            {
                geometrys.push_back(child_geom);
            }
        }
    }
    else if(type_id == geos::geom::GeometryTypeId::GEOS_POLYGON)
    {
        geometrys.push_back(geosResult);
    }
    else
    {
    }
    for(auto geo: geometrys)
    {
        std::vector<nous::vec3> nparray;
        nous::Array<nous::Array<nous::vec2>> allPolygon;

        const geos::geom::Polygon* valid_polygon = static_cast<const geos::geom::Polygon*>(geo);

        const geos::geom::LinearRing* outerRing = valid_polygon->getExteriorRing();
        auto outsq = outerRing->getCoordinates();
        size_t outerPointNum = outsq->getSize();// outer_ring->getNumPoints();
        if(outerPointNum > 0)
        {
            nous::Array<nous::vec2>& outer = allPolygon.emplace_back();
            for(int i = 0; i < outerPointNum - 1; ++i)
            {
                // std::unique_ptr<geos::geom::Point> pt = outer_ring->getPointN(i);
                // outerPolygon.emplace_back(pt->getX(), pt->getY());
                geos::geom::Coordinate p;
                outsq->getAt(i, p);
                auto pos = geopos_pos_map.find(p);
                if(pos == geopos_pos_map.end())
                {
                    nparray.emplace_back(rotate_to_xy_plane.conjugate() * nous::vec3(p.x, p.y, plane_z));
                }
                else
                {
                    nparray.emplace_back(pos->second);
                }
                outer.emplace_back(p.x, p.y);
            }
            if(outer.size() < 3)
            {
                allPolygon.pop_back();
            }
        }

        size_t innerRingNum = valid_polygon->getNumInteriorRing();
        if(innerRingNum > 0)
        {
            for(int i = 0; i < innerRingNum; ++i)
            {
                const geos::geom::LinearRing* interRing = valid_polygon->getInteriorRingN(i);
                auto insq = interRing->getCoordinates();
                size_t interRingPointNum = insq->getSize();// interRing->getNumPoints();
                nous::Array<nous::vec2>& innerPolygon = allPolygon.emplace_back();
                for(int j = 0; j < interRingPointNum - 1; ++j)
                {
                    // std::unique_ptr<geos::geom::Point> pt = interRing->getPointN(j);
                    // innerPolygon.emplace_back(pt->getX(), pt->getY());
                    geos::geom::Coordinate p;
                    insq->getAt(j, p);
                    auto pos = geopos_pos_map.find(p);
                    if(pos == geopos_pos_map.end())
                    {
                        nparray.emplace_back(rotate_to_xy_plane.conjugate() * nous::vec3(p.x, p.y, plane_z));
                    }
                    else
                    {
                        nparray.emplace_back(pos->second);
                    }
                    innerPolygon.emplace_back(p.x, p.y);
                }
                if(innerPolygon.size() < 3)
                {
                    allPolygon.pop_back();
                }
            }
        }

        auto tris = mapbox::earcut<size_t>(allPolygon);
        for(auto tri: tris)
        {
            std::vector<nous::vec3>& nt = result.emplace_back();
            nt.emplace_back(nparray[tri.x()]);
            nt.emplace_back(nparray[tri.y()]);
            nt.emplace_back(nparray[tri.z()]);
        }
    }

    return result;
}

/**
 * @brief 对多边形进行三角化
 * 
 * 该函数用于对指定的多边形（polygon）进行三角化，并返回三角化的结果。
 * 如果多边形的拓扑结构无效或存在自交情况，函数会使用不同的方法进行退化处理。
 * 
 * @param[in] polygon 多边形的顶点位置集合，包含多个环（第一个为外环，后续为内环），每个环由nous::vec3类型的点集表示
 * @param[in] reverse_triangle_order 是否反转三角形顶点的顺序
 * 
 * @return std::vector<std::vector<nous::vec3>> 返回三角化的结果，包含多个三角形，每个三角形由三个nous::vec3类型的点表示
 * 
 * @note 函数内部首先将多边形的顶点位置转换为相对索引，并存储在id_pos_map中。
 *       然后将这些顶点投影到XY平面，并存储在cdt_positions中。
 *       接着创建约束边集合（constrained_edges），表示多边形的边。
 *       使用CDT库进行三角化，如果拓扑结构无效或存在自交情况，则进行退化处理。
 *       最后，根据reverse_triangle_order参数决定是否反转三角形顶点的顺序，并返回三角化的结果。
 */
std::vector<std::vector<nous::vec3>> triangulate_polygon(std::vector<std::vector<nous::vec3>>& polygon, bool reverse_triangle_order)
{
    std::vector<nous::vec3> id_pos_map; ///< 存储点与索引的映射关系
    const auto&& polygon_pt_rel_idx = topo::convert_polygon_position_to_relative_idx(polygon, id_pos_map); ///< 将多边形的顶点位置转换为相对索引
    std::vector<CDT::V2d<float>> cdt_positions; ///< 存储投影到XY平面的顶点位置

    nous::vec3 normal = sketch_math::ring_normal(polygon.front()); ///< 计算多边形外环的法向量
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(normal, nous::vec3(0.f, 0.f, 1.f)); ///< 定义四元数，用于将点投影到XY平面
    float plane_z = 0.0f; ///< 存储投影后的z坐标

    for(int i = 0; i < id_pos_map.size(); ++i)
    {
        const nous::pos_t xoy_pos = rotate_to_xy_plane * id_pos_map[i]; ///< 将点投影到XY平面
        plane_z = xoy_pos.z(); ///< 获取投影后的z坐标
        CDT::V2d<float> cdt_pos; ///< 定义CDT库中的顶点位置
        cdt_pos.x = xoy_pos.x(); ///< 设置x坐标
        cdt_pos.y = xoy_pos.y(); ///< 设置y坐标
        cdt_positions.emplace_back(cdt_pos); ///< 将投影后的顶点位置添加到cdt_positions中
    }

    CDT::EdgeVec constrained_edges; ///< 存储约束边集合
    for(int i = 0; i < polygon_pt_rel_idx.size(); ++i)
    {
        for(int j = 0; j < polygon_pt_rel_idx[i].size(); ++j)
        {
            const size_t prev_pt_idx = polygon_pt_rel_idx[i][j]; ///< 获取当前点的索引
            const size_t next_pt_idx = polygon_pt_rel_idx[i][(j + 1) % polygon_pt_rel_idx[i].size()]; ///< 获取下一个点的索引，使用模运算确保最后一个点与第一个点相连
            constrained_edges.push_back(CDT::Edge(prev_pt_idx, next_pt_idx)); ///< 创建约束边并添加到集合中
        }
    }

    std::vector<std::vector<nous::vec3>> result; ///< 存储三角化的结果
    try
    {
        CDT::Triangulation<float> cdt; ///< 创建CDT库中的三角化对象
        cdt.insertVertices(cdt_positions); ///< 插入顶点
        cdt.insertEdges(constrained_edges); ///< 插入约束边

        const bool is_valid = CDT::verifyTopology(cdt); ///< 验证拓扑结构是否有效
        if(!is_valid)
        {
            // 退化给 geos 和 earcut 三角化
            return triangulate_polygon_with_intersect(polygon); ///< 使用自交检测和earcut进行三角化
        }

        cdt.eraseOuterTrianglesAndHoles(); ///< 删除外环三角形和孔

        for(const auto& tri: cdt.triangles) ///< 遍历生成的三角形
        {
            auto& triangle = result.emplace_back(); ///< 创建一个新的三角形
            if(reverse_triangle_order) ///< 如果需要反转三角形顶点的顺序
            {
                triangle.emplace_back(id_pos_map[tri.vertices[0]]); ///< 添加第一个顶点
                triangle.emplace_back(id_pos_map[tri.vertices[1]]); ///< 添加第二个顶点
                triangle.emplace_back(id_pos_map[tri.vertices[2]]); ///< 添加第三个顶点
            }
            else ///< 如果不需要反转三角形顶点的顺序
            {
                triangle.emplace_back(id_pos_map[tri.vertices[0]]); ///< 添加第一个顶点
                triangle.emplace_back(id_pos_map[tri.vertices[2]]); ///< 添加第三个顶点
                triangle.emplace_back(id_pos_map[tri.vertices[1]]); ///< 添加第二个顶点
            }
        }
    }
    catch(CDT::IntersectingConstraintsError ice) ///< 捕获自交约束边错误
    {
        // 退化给自交检测
        std::vector<std::vector<std::vector<nous::pos_t>>> splited_valid_polygon = topo::rebuild_invalid_polygon_topo(polygon); ///< 重建无效多边形拓扑结构
        for(int i = 0; i < splited_valid_polygon.size(); ++i)
        {
            std::vector<std::vector<nous::vec3>> sub_polygon_triangles = triangulate_polygon(splited_valid_polygon[i]); ///< 对子多边形进行三角化
            result.insert(result.end(), sub_polygon_triangles.begin(), sub_polygon_triangles.end()); ///< 将子多边形的三角化结果添加到最终结果中
        }
        return result; ///< 返回三角化的结果
    }
    catch(CDT::DuplicateVertexError dve) ///< 捕获重复顶点错误
    {
        return triangulate_polygon_with_intersect(polygon); ///< 使用自交检测和earcut进行三角化
    }

    return result; ///< 返回三角化的结果
}

/**
 * @brief 使用CDT库对多边形进行三角化
 * 
 * 该函数用于对指定的多边形（polygon）进行三角化，并将三角化的结果存储在多边形的triangles属性中。
 * 如果多边形无效或存在自交情况，函数会清空triangles属性并返回。
 * 
 * @param[in] geodata GeoData对象指针，提供几何数据访问接口
 * @param[in] polygon 指定的多边形对象指针
 * 
 * @note 函数内部首先清空多边形的triangles属性。
 *       然后检查多边形是否有效，如果无效则直接返回。
 *       接着将多边形的顶点位置转换为相对索引，并存储在id_point_map中。
 *       将这些顶点投影到XY平面，并存储在cdt_positions中。
 *       创建约束边集合（constrained_edges），表示多边形的边。
 *       使用CDT库进行三角化，如果拓扑结构无效或存在自交情况，则清空triangles属性并返回。
 *       最后，将三角化的结果存储在多边形的triangles属性中。
 */
void triangulate_polygon_cdt(GeoData* geodata, SketchPolygonPtr polygon)
{
    polygon->triangles.clear(); ///< 清空多边形的triangles属性

    if(!is_polygon_valid(geodata, polygon)) ///< 检查多边形是否有效
    {
        return; ///< 如果多边形无效，直接返回
    }

    std::vector<SketchPointPtr> id_point_map; ///< 存储点与索引的映射关系
    const auto&& polygon_pt_rel_idx = GeoDataUtils::get().convert_polygon_to_rel_point_idx(geodata, polygon, id_point_map); ///< 将多边形的顶点位置转换为相对索引
    std::vector<CDT::V2d<float>> cdt_positions; ///< 存储投影到XY平面的顶点位置

    double record_z = 0.0; ///< 存储投影后的z坐标
    nous::quat rotate_to_xy_plane = nous::quat::FromTwoVectors(polygon->ring->normal, nous::vec3(0.f, 0.f, 1.f)); ///< 定义四元数，用于将点投影到XY平面
    for(int i = 0; i < id_point_map.size(); ++i)
    {
        const nous::pos_t pt_pos = geodata->get_point_position(id_point_map[i]); ///< 获取点的位置
        const nous::pos_t xoy_pos = rotate_to_xy_plane * pt_pos; ///< 将点投影到XY平面
        record_z = xoy_pos.z(); ///< 获取投影后的z坐标
        CDT::V2d<float> cdt_pos; ///< 定义CDT库中的顶点位置
        cdt_pos.x = xoy_pos.x(); ///< 设置x坐标
        cdt_pos.y = xoy_pos.y(); ///< 设置y坐标
        cdt_positions.emplace_back(cdt_pos); ///< 将投影后的顶点位置添加到cdt_positions中
    }

    CDT::EdgeVec constrained_edges; ///< 存储约束边集合
    for(int i = 0; i < polygon_pt_rel_idx.size(); ++i)
    {
        for(int j = 0; j < polygon_pt_rel_idx[i].size(); ++j)
        {
            const size_t prev_pt_idx = polygon_pt_rel_idx[i][j]; ///< 获取当前点的索引
            const size_t next_pt_idx = polygon_pt_rel_idx[i][(j + 1) % polygon_pt_rel_idx[i].size()]; ///< 获取下一个点的索引，使用模运算确保最后一个点与第一个点相连
            constrained_edges.push_back(CDT::Edge(prev_pt_idx, next_pt_idx)); ///< 创建约束边并添加到集合中
        }
    }

    try
    {
        CDT::Triangulation<float> cdt; ///< 创建CDT库中的三角化对象
        cdt.insertVertices(cdt_positions); ///< 插入顶点
        cdt.insertEdges(constrained_edges); ///< 插入约束边
        cdt.eraseOuterTrianglesAndHoles(); ///< 删除外环三角形和孔

        for(const auto& tri: cdt.triangles) ///< 遍历生成的三角形
        {
            auto& triangle = polygon->triangles.emplace_back(); ///< 创建一个新的三角形
            triangle.emplace_back(id_point_map[tri.vertices[0]], nous::vec3::Zero()); ///< 添加第一个顶点
            triangle.emplace_back(id_point_map[tri.vertices[1]], nous::vec3::Zero()); ///< 添加第二个顶点
            triangle.emplace_back(id_point_map[tri.vertices[2]], nous::vec3::Zero()); ///< 添加第三个顶点
        }
    }
    catch(CDT::IntersectingConstraintsError ice) ///< 捕获自交约束边错误
    {
        // 退化给自交检测
        polygon->triangles.clear(); ///< 清空triangles属性
        return; ///< 直接返回
    }
    catch(CDT::DuplicateVertexError dve) ///< 捕获重复顶点错误
    {
        polygon->triangles.clear(); ///< 清空triangles属性
        return; ///< 直接返回
    }
}

};// namespace geodata_topo
};// namespace sketch