#pragma once
#include "../Element.h"
#include "NurbsPoint.h"

namespace sketch
{
enum class ENurbsCloseType
{
    Open,
    SharpClose,
    SmoothClose
};

class INurbsSurfaceBase;

class INurbsBase : public SketchElement
{
public:
    INurbsBase(EElementType type) : SketchElement(type) {}
    virtual ~INurbsBase() {}

    virtual void generate_render_mesh(IRenderable* render_data) = 0;

    virtual nous::AABB3 get_bounding_box() = 0;

    virtual void transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat = nous::mat4::Identity()) = 0;

    virtual void save(SketchSerializer& serializer) = 0;

    virtual void load(SketchSerializer& serializer) = 0;

    virtual bool is_valid() = 0;

    virtual void dirty() { m_dirty = true; }

    virtual std::shared_ptr<INurbsBase> clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                              const nous::mat4& trans_mat = nous::mat4::Identity()) = 0;

public:
    bool m_dirty = false;
};

using INurbsBasePtr = std::shared_ptr<INurbsBase>;

};// namespace sketch
