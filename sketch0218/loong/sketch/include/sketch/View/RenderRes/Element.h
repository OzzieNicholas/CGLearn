#pragma once

#include "../../Nurbs/tinynurbs.h"
#include "nous_mesh/mesh.h"
#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace sketch
{
class IRenderable;
class SketchSerializer;

enum class EElementType : uint32_t
{
    None = 0,
    Point,
    Line,
    Ring,
    Polygon,
    // 曲线
    Curve = 5,
    // 代数曲面
    AlgebraSurface,
    // 围合曲面
    ClosedSurface,
    // 复杂曲面（带洞曲面）
    TrimmedSurface,
    // 代数曲面的边界
    AlgebraSurfaceEdge,
    // 围合曲面的曲线
    ClosedSurfaceEdge,
    // 焊接线
    NoBranchLineGroup,
    // 柔化面
    SmoothGroup
};

class SketchElement;
using SketchElementPtr = std::shared_ptr<SketchElement>;
class ElementGroup;
using ElementGroupPtr = std::shared_ptr<ElementGroup>;

class SketchPoint;
class SketchLine;
class SketchRing;
class SketchPolygon;
class NoBranchLineGroup;
using SketchPointPtr = std::shared_ptr<SketchPoint>;
using SketchLinePtr = std::shared_ptr<SketchLine>;
using SketchRingPtr = std::shared_ptr<SketchRing>;
using SketchPolygonPtr = std::shared_ptr<SketchPolygon>;

class SketchNurbsPoint;
class INurbsBase;
class INurbsSurfaceBase;
class NurbsAlgebraSurface;
class NurbsCurve;
class NurbsClosedSurface;
class NurbsTrimmedSurface;
class NurbsClosedSurfaceEdgeElement;
class NurbsAlgebraSurfaceEdgeElement;

class SketchElement : public std::enable_shared_from_this<SketchElement>
{
public:
    SketchElement() = default;
    SketchElement(EElementType type);
    virtual ~SketchElement();

    inline uint32_t id() const { return m_id; }
    inline void set_id(uint32_t id) { m_id = id; }
    inline EElementType type() const { return m_type; }
    inline void set_type(EElementType type) { m_type = type; }
    inline SketchElementPtr parent() const { return m_parent; }
    inline void set_parent(SketchElementPtr parent) { m_parent = parent; }
    inline ElementGroupPtr group() const { return m_group; }
    inline void set_group(ElementGroupPtr group) { m_group = group; }

    virtual void save(SketchSerializer& serializer);
    virtual void load(SketchSerializer& serializer);

    ElementGroupPtr to_group() { return std::dynamic_pointer_cast<ElementGroup>(shared_from_this()); }
    SketchPointPtr to_point() { return std::dynamic_pointer_cast<SketchPoint>(shared_from_this()); }
    SketchLinePtr to_line() { return std::dynamic_pointer_cast<SketchLine>(shared_from_this()); }
    SketchRingPtr to_ring() { return std::dynamic_pointer_cast<SketchRing>(shared_from_this()); }
    SketchPolygonPtr to_polygon() { return std::dynamic_pointer_cast<SketchPolygon>(shared_from_this()); }
    std::shared_ptr<NoBranchLineGroup> to_no_branch_line_group();
    std::shared_ptr<NurbsCurve> to_nurbs_curve();
    std::shared_ptr<NurbsAlgebraSurface> to_nurbs_algebra_surface();
    std::shared_ptr<NurbsClosedSurface> to_nurbs_closed_surface();
    std::shared_ptr<NurbsTrimmedSurface> to_nurbs_trimmed_surface();
    std::shared_ptr<NurbsClosedSurfaceEdgeElement> to_nurbs_closed_surface_element();
    std::shared_ptr<NurbsAlgebraSurfaceEdgeElement> to_nurbs_algebra_surface_element();
    std::shared_ptr<INurbsSurfaceBase> to_nurbs_surface_base();

private:
    static uint32_t id_counter;
    // static std::unordered_map<uint32_t, SketchElementPtr> id_map;
    uint32_t m_id;
    EElementType m_type;
    SketchElementPtr m_parent;
    ElementGroupPtr m_group;
};

class ElementGroup : public SketchElement
{
public:
    ElementGroup(EElementType type) : SketchElement(type), m_need_check(true) {}
    virtual ~ElementGroup() {}
    bool empty() { return elements().empty(); }
    bool need_check() { return m_need_check; }
    void set_need_check(bool b) { m_need_check = b; }

    virtual void clear() = 0;
    // ele只允许有一个group，merge会自动从目标对象中删除ele
    virtual void merge(ElementGroupPtr group) = 0;
    virtual void add_element(std::shared_ptr<SketchElement> element) = 0;
    virtual void add_elements(std::unordered_set<std::shared_ptr<SketchElement>> elements) = 0;
    virtual void erase_element(std::shared_ptr<SketchElement> element) = 0;
    virtual std::unordered_set<std::shared_ptr<SketchElement>> elements() = 0;

protected:
    bool m_need_check;
};

// point
class SketchPoint : public SketchElement
{
public:
    SketchPoint() = default;
    SketchPoint(nous::Point* point) : SketchElement(EElementType::Point), point(point) {}
    ~SketchPoint() {}

    nous::Point* point;
};

// line
class SketchLine : public SketchElement
{
public:
    SketchLine() = default;
    SketchLine(nous::Prim* prim, size_t key) : SketchElement(EElementType::Line), line_prim(prim), key(key) {}
    ~SketchLine() {}

    SketchPointPtr p0;
    SketchPointPtr p1;

    nous::Prim* line_prim;
    size_t key = 0;
};

// ring
class SketchRing : public SketchElement
{
public:
    SketchRing() = default;
    SketchRing(nous::Prim* prim, size_t key) : SketchElement(EElementType::Ring), ring_prim(prim), key(key) {}
    ~SketchRing() {}

    nous::Prim* ring_prim;

    nous::vec3 normal = nous::vec3::Identity();
    size_t key = 0;
};

// polygon
// 当 Point* 不为 nullptr 时选用 Point 的位置，否则选用 vec3 的位置
using SketchPolygonTriangle = std::vector<std::pair<SketchPointPtr, nous::vec3>>;
class SketchPolygon : public SketchElement
{
public:
    SketchPolygon() = default;
    SketchPolygon(SketchRingPtr ring) : SketchElement(EElementType::Polygon), ring(ring) {}
    SketchPolygon(SketchPolygon& skp) : SketchElement(EElementType::Polygon)
    {
        normal = skp.normal;
        ring = skp.ring;
        inner_rings = skp.inner_rings;
        triangles = skp.triangles;
    }
    ~SketchPolygon() {}

    bool m_changed = true;

    nous::vec3 normal = nous::vec3::Identity();
    SketchRingPtr ring = nullptr;
    std::unordered_set<SketchRingPtr> inner_rings;
    std::vector<SketchPolygonTriangle> triangles;
};

}// namespace sketch
