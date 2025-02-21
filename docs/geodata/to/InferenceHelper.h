#pragma once

#include <queue>
#include <unordered_set>

#include "../../View/RenderRes/Element.h"
#include "nous_base/nous_base.h"
#include "sketch/Define.h"

namespace nous
{
class Point;
class Prim;
}// namespace nous
namespace sketch
{
class View;
class ViewCamera;
class RenderObject;
class SketchPolygon;

enum InferType : uint32_t
{
    NONE = 0,
    POINT = 1 << 1,
    POLYGON = 1 << 2,
    LINE = 1 << 3,

    CENTER = 1 << 4,

    VERTICAL = 1 << 5,
    PARALLEL = 1 << 6,
    COLLINEAR = 1 << 7,

    PLANE_CUSTOM = 1 << 8,
    PLANE_X = 1 << 9,
    PLANE_Y = 1 << 10,
    PLANE_Z = 1 << 11,

    AXIS_X = 1 << 12,
    AXIS_Y = 1 << 13,
    AXIS_Z = 1 << 14,

    CURRENT_LINE = 1 << 15,
    SEG_LINE = 1 << 16,

    CURVE = 1 << 17,
    SURFACE = 1 << 18,

    TRIANGLE_POINT = 1 << 19,
    TRIANGLE_LINE = 1 << 20,
    TRIANGLE_FACE = 1 << 21,

    // ele type
    // line
    LINE_CENTER = LINE | CENTER,
    MESH_TYPE = POINT | LINE | POLYGON,
    NURBS_TYPE = CURVE | SURFACE,
    ELEMENT_TYPE = MESH_TYPE | NURBS_TYPE,

    // virtual type
    // plane & axis
    PLANE_XYZ_TYPE = PLANE_X | PLANE_Y | PLANE_Z,
    PLANE_TYPE = PLANE_CUSTOM | PLANE_XYZ_TYPE,
    AXIS_TYPE = AXIS_X | AXIS_Y | AXIS_Z,

    // vpc
    PARALLEL_LINE = LINE | PARALLEL,
    COLLINEAR_LINE = LINE | COLLINEAR,
    VERTICAL_LINE = LINE | VERTICAL,
    VERTICAL_PLANE = PLANE_TYPE | VERTICAL,
    PARALLEL_CURVE = CURVE | PARALLEL,
    VERTICAL_SURFACE = SURFACE | VERTICAL,
    VPC_TYPE = PARALLEL | COLLINEAR | VERTICAL,

    VPC_LINE_TYPE = PARALLEL_LINE | COLLINEAR_LINE | VERTICAL_LINE,
    VPC_PLANE_TYPE = VERTICAL_PLANE,
    VPC_CURVE_TYPE = PARALLEL_CURVE,
    VPC_SURFACE_TYPE = VERTICAL_SURFACE,

    VIRTUAL_TYPE = PLANE_TYPE | AXIS_TYPE | VPC_LINE_TYPE | VPC_PLANE_TYPE,

    //
    ALL_TYPE = ELEMENT_TYPE | VIRTUAL_TYPE
};
struct ReferAttrib
{
    ReferAttrib() {}
    ReferAttrib(const nous::vec3& para_0, const nous::vec3& para_1, uint32_t type)
        : mesh(nullptr), para_0(para_0), para_1(para_1), refer_type(InferType(type))
    {
    }

    RenderObject* mesh;
    nous::vec3 para_0;
    nous::vec3 para_1;
    std::vector<std::vector<nous::vec3>> polygon;
    InferType refer_type;
};

struct InferResult
{
    InferResult() {}
    RenderObject* mesh = nullptr;
    nous::vec3 position = nous::vec3::Zero();
    nous::vec3 normal = nous::vec3::Zero();
    nous::vec3 tangent = nous::vec3::Zero();
    SketchElementPtr element = nullptr;

    InferType infer_type = InferType::NONE;
    long long hover_time = std::numeric_limits<long long>::max();
    std::vector<std::pair<InferType, std::pair<nous::vec3, nous::vec3>>> inference_lines;

    // reference info
    std::vector<nous::vec3> reference_points;
    std::vector<std::pair<nous::vec3, nous::vec3>> reference_lines;
    std::vector<std::vector<std::vector<nous::vec3>>> reference_polygons;
    bool is_nurbs() const;
    bool is_point() const;
    bool is_world_axis() const;
};

class InferenceHelper
{
    struct IgnoreElement
    {
        IgnoreElement() {}
        IgnoreElement(const std::unordered_set<SketchElementPtr>& elements) : elements(elements) {}
        std::unordered_set<SketchElementPtr> elements;
    };

public:
    InferenceHelper(ViewCamera* cam);

    virtual ~InferenceHelper();

    virtual void clear()
    {
        m_infer_result = InferResult();
        m_infer_ignore_obj_list.clear();
        m_infer_ignore_list.clear();
    }

    // 仅支持AXIS_TYPE与PLANE_XYZ_TYPE
    virtual void update_inference(View* view, int x, int y, nous::vec3* extra_refer_pos = nullptr, InferType extra_type = InferType::AXIS_TYPE);

    inline const InferResult& get_infer_result() { return m_infer_result; }

    void render_obj_changed(Define::Message method, RenderObject* obj);

    inline void set_infer_range(int val) { m_infer_pixel_range_pow2 = val * val; }

    inline void set_infer_filter_type(InferType infer_type) { m_infer_filter_type = infer_type; }

    inline void add_ignore(RenderObject* obj) { m_infer_ignore_obj_list.insert(obj); }

    inline void add_ignore(RenderObject* obj, const std::unordered_set<SketchElementPtr>& ele)
    {
        m_infer_ignore_list[obj].elements.insert(ele.begin(), ele.end());
    }

    inline void add_ignore(RenderObject* obj, SketchElementPtr val) { m_infer_ignore_list[obj].elements.insert(val); }

    inline bool is_ignore(RenderObject* obj, SketchElementPtr val)
    {
        auto& pos = m_infer_ignore_list.find(obj);
        if(pos == m_infer_ignore_list.end())
        {
            return false;
        }

        return pos->second.elements.find(val) != pos->second.elements.end();
    }

    inline void clear_ignore()
    {
        m_infer_ignore_obj_list.clear();
        m_infer_ignore_list.clear();
    }

    inline void force_reference(const ReferAttrib& refer, InferType force_infer_refer_type)
    {
        m_enalble_force_reference = true;
        m_force_reference = refer;
        m_force_reference_type = force_infer_refer_type;
    }

    inline void clear_force_reference()
    {
        m_enalble_force_reference = false;
        m_force_reference = ReferAttrib();
        m_force_reference_type = InferType::NONE;
    }

    inline void attach_reference_triangle(const std::vector<float>& triangles, bool point = true, bool line = true, bool face = true)
    {
        m_refer_triangle_points = point;
        m_refer_triangle_lines = line;
        m_refer_triangle_faces = face;
        m_reference_triangle = triangles;
    }

    inline void clear_reference_triangle()
    {
        m_refer_triangle_points = false;
        m_refer_triangle_lines = false;
        m_refer_triangle_faces = false;
        m_reference_triangle.clear();
    }

    inline void attach_reference(const ReferAttrib& refer) {}

    inline void attach_reference(const InferResult& refer) { _gen_reference_element(const_cast<InferResult&>(refer), true); }

    inline void clear_refer_points()
    {
        while(!m_reference_points_queue.empty())
        {
            m_reference_points_queue.pop();
        }
    }

    inline void clear_refer_lines()
    {
        while(!m_reference_lines_queue.empty())
        {
            m_reference_lines_queue.pop();
        }
    }

    inline void clear_refer_polygons()
    {
        while(!m_reference_polygons_queue.empty())
        {
            m_reference_polygons_queue.pop();
        }
    }

    inline void clear_refer_curves()
    {
        while(!m_reference_curve_queue.empty())
        {
            m_reference_curve_queue.pop();
        }
    }

    inline void clear_refer_surfaces()
    {
        while(!m_reference_surface_queue.empty())
        {
            m_reference_surface_queue.pop();
        }
    }

    inline void clear_refer()
    {
        clear_refer_points();
        clear_refer_lines();
        clear_refer_polygons();
        clear_refer_curves();
        clear_refer_surfaces();
    }

private:
    void _gen_reference_element(InferResult& infer, bool force = false);

    void _intersect_with_xyz_plan(View* view, int x, int y, nous::vec3& pos, nous::vec3& normal, const nous::vec3 axis_center = nous::vec3::Zero());

    void _inference_with_refer(InferResult& current_infer, View* view, int x, int y, nous::vec3* extra_refer_pos, InferType extra_type);

    void _inference_with_force_refer(InferResult& infer, const nous::vec3& cam_pos, const nous::vec3& cam_dir);

    bool _check_line_intersection_with_inference(InferResult& current_infer);

    bool _check_polygon_intersection_with_inference(InferResult& current_infer, ViewCamera* cam);

private:
    ViewCamera* m_camera;
    int m_infer_pixel_range_pow2;

    InferType m_infer_filter_type;
    InferResult m_infer_result;

    std::unordered_map<RenderObject*, IgnoreElement> m_infer_ignore_list;
    std::unordered_set<RenderObject*> m_infer_ignore_obj_list;
    // reference
    bool m_enalble_force_reference;
    ReferAttrib m_force_reference;
    InferType m_force_reference_type;

    bool m_refer_triangle_points;
    bool m_refer_triangle_lines;
    bool m_refer_triangle_faces;
    std::vector<float> m_reference_triangle;

    std::queue<ReferAttrib> m_reference_points_queue;
    std::queue<ReferAttrib> m_reference_lines_queue;
    std::queue<ReferAttrib> m_reference_polygons_queue;
    std::queue<ReferAttrib> m_reference_curve_queue;
    std::queue<ReferAttrib> m_reference_surface_queue;
};

}// namespace sketch