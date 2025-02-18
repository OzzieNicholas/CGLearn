#pragma once
#include "../Element.h"
#include "NurbsClosedSurface.h"

namespace sketch
{

class NurbsClosedSurfaceEdgeElement : public SketchElement
{
public:
    NurbsClosedSurfaceEdgeElement() : SketchElement(EElementType::ClosedSurfaceEdge) {}

    std::weak_ptr<NurbsClosedSurface> m_parent;
    std::weak_ptr<NurbsCurve> m_edge_curve;
};

using NurbsClosedSurfaceEdgeElementPtr = std::shared_ptr<NurbsClosedSurfaceEdgeElement>;

};// namespace sketch