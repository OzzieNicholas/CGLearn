#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace sketch
{
NurbsData::NurbsData(const NurbsData& other)
{
    this->set_type(other.get_type());
    std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remap_map;
    for(const auto& pair: other.get_all_nurbs())
    {
        add_item(pair.second->clone(remap_map));
    }

    m_dirty_flags = true;
}

void NurbsData::save(SketchSerializer& serializer)
{
    serializer.begin_write_array("items");
    for(const auto& pair: m_nurbs_items)
    {
        serializer.write_sketch_nurbs_item_ref(pair.second);
    }
    serializer.end_write_array();
}

void NurbsData::load(SketchSerializer& serializer)
{
    size_t item_number;
    serializer.begin_read_array("items", item_number);
    for(int i = 0; i < item_number; ++i)
    {
        auto& nurbs_item = serializer.read_sketch_nurbs_item_ref();
        add_item(nurbs_item);
    }
    serializer.end_read_array();
}

nous::AABB3 NurbsData::get_bounding_box() const
{
    nous::AABB3 bounding_box;
    for(const auto& pair: m_nurbs_items)
    {
        bounding_box.extend(pair.second->get_bounding_box());
    }
    return bounding_box;
}

bool NurbsData::_update_render_resource()
{
    IRenderable::clear();
    for(auto& item: m_nurbs_items)
    {
        item.second->generate_render_mesh(this);
    }
    return true;
}

};// namespace sketch