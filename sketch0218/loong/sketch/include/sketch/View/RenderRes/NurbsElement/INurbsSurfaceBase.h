#pragma once
#include "INurbsBase.h"

namespace sketch
{

class NurbsTrimmedSurface;

// 存储 surface 共性的接口
class INurbsSurfaceBase : public INurbsBase
{
public:
    INurbsSurfaceBase(EElementType element_type) : INurbsBase(element_type) {}

    virtual std::vector<std::vector<SketchNurbsPointPtr>> get_surface_control_points() = 0;

    virtual std::vector<SketchNurbsPointPtr> get_surface_control_points_1D() = 0;

    virtual void set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent_surface) = 0;

    virtual void set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent_surface) = 0;

    virtual bool is_surface_coplane(nous::vec3& coplane_normal) = 0;
};

using INurbsSurfaceBasePtr = std::shared_ptr<INurbsSurfaceBase>;

};// namespace sketch