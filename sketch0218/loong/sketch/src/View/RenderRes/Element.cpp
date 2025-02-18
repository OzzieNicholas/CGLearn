#include "sketch/View/RenderRes/Element.h"
#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"

namespace sketch
{
uint32_t SketchElement::id_counter = 0;
SketchElement::SketchElement(EElementType type) : m_type(type), m_parent(nullptr), m_group(nullptr) { m_id = id_counter++; }

SketchElement::~SketchElement() {}

void SketchElement::save(SketchSerializer& serializer)
{
    serializer.write(m_id, "id");
    serializer.write(static_cast<uint32_t>(m_type), "type");
}

void SketchElement::load(SketchSerializer& serializer)
{
    serializer.read(m_id, "id");
    uint32_t tmp_type;
    serializer.write(tmp_type, "type");
    m_type = static_cast<EElementType>(tmp_type);
}

std::shared_ptr<NoBranchLineGroup> SketchElement::to_no_branch_line_group()
{
    return std::dynamic_pointer_cast<NoBranchLineGroup>(shared_from_this());
}

std::shared_ptr<NurbsCurve> SketchElement::to_nurbs_curve() { return std::dynamic_pointer_cast<NurbsCurve>(shared_from_this()); }

std::shared_ptr<NurbsAlgebraSurface> SketchElement::to_nurbs_algebra_surface()
{
    return std::dynamic_pointer_cast<NurbsAlgebraSurface>(shared_from_this());
}

std::shared_ptr<NurbsClosedSurface> SketchElement::to_nurbs_closed_surface()
{
    return std::dynamic_pointer_cast<NurbsClosedSurface>(shared_from_this());
}

std::shared_ptr<NurbsTrimmedSurface> SketchElement::to_nurbs_trimmed_surface()
{
    return std::dynamic_pointer_cast<NurbsTrimmedSurface>(shared_from_this());
}

std::shared_ptr<NurbsClosedSurfaceEdgeElement> SketchElement::to_nurbs_closed_surface_element()
{
    return std::dynamic_pointer_cast<NurbsClosedSurfaceEdgeElement>(shared_from_this());
}

std::shared_ptr<NurbsAlgebraSurfaceEdgeElement> SketchElement::to_nurbs_algebra_surface_element()
{
    return std::dynamic_pointer_cast<NurbsAlgebraSurfaceEdgeElement>(shared_from_this());
}

std::shared_ptr<INurbsSurfaceBase> SketchElement::to_nurbs_surface_base() { return std::dynamic_pointer_cast<INurbsSurfaceBase>(shared_from_this()); }

}// namespace sketch