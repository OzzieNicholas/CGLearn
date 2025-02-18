#pragma once
#include "../Element.h"
#include "NurbsAlgebraSurface.h"
#include "NurbsCurve.h"

namespace sketch
{

class NurbsAlgebraSurfaceEdgeElement : public SketchElement
{
public:
    NurbsAlgebraSurfaceEdgeElement() : SketchElement(EElementType::AlgebraSurfaceEdge)
    {
        m_edge_type = EAlgebraSurfaceBoundaryType::U0;
        m_edge_curve = nullptr;
    }

    std::weak_ptr<NurbsAlgebraSurface> m_parent;

    EAlgebraSurfaceBoundaryType m_edge_type;

    NurbsCurvePtr m_edge_curve;
};

using NurbsAlgebraSurfaceEdgeElementPtr = std::shared_ptr<NurbsAlgebraSurfaceEdgeElement>;

};// namespace sketch
