#pragma once

#include "../Element.h"

namespace sketch
{

class INurbsBase;

class SketchNurbsPoint : public tinynurbs::NurbsPoint<nous::scalar>
{
public:
    SketchNurbsPoint() = default;

    SketchNurbsPoint(nous::scalar x, nous::scalar y, nous::scalar z, tinynurbs::ENurbsPointType type = tinynurbs::ENurbsPointType::Common)
        : tinynurbs::NurbsPoint<nous::scalar>(x, y, z, type)
    {
    }

    SketchNurbsPoint(const nous::pos_t& point, tinynurbs::ENurbsPointType type = tinynurbs::ENurbsPointType::Common)
        : tinynurbs::NurbsPoint<nous::scalar>(point, type)
    {
    }

    void remove_parent(INurbsBase* parent);

    virtual void set_position(const nous::pos_t& pos) override;

    std::unordered_set<std::shared_ptr<INurbsBase>> get_valid_parents();

public:
    std::vector<std::weak_ptr<INurbsBase>> m_parents;
};

using SketchNurbsPointPtr = std::shared_ptr<SketchNurbsPoint>;

};// namespace sketch