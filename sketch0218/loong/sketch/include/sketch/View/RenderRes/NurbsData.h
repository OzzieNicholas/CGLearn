#pragma once

#include "./Element.h"
#include "./IRenderable.h"
#include "NurbsElement/NurbsElements.h"
#include "sketch/Object/SketchObject.h"

namespace sketch
{

class NurbsData : public SketchObject, public IRenderable
{
public:
    NurbsData() : SketchObject(ESketchObjectType::NurbsData), IRenderable() { m_dirty_flags = true; }
    NurbsData(const NurbsData& other);
    ~NurbsData() {}

    inline bool update_render_mesh()
    {
        if(m_dirty_flags & 0x1)
        {
            this->_update_render_resource();
            m_dirty_flags &= ~0x1;
            return true;
        }
        return false;
    }

    inline void add_item(std::shared_ptr<INurbsBase> item)
    {
        if(m_nurbs_items.find(item->id()) == m_nurbs_items.end())
        {
            m_nurbs_items.insert({ item->id(), item });
        }
        else
        {
            m_nurbs_items[item->id()] = item;
        }
        dirty();
    }

    inline void clear()
    {
        m_nurbs_items.clear();
        m_dirty_flags = true;
    }

    inline bool item_exist(const uint32_t item_id) { return m_nurbs_items.find(item_id) != m_nurbs_items.end(); }

    inline std::shared_ptr<INurbsBase> get_item(const uint32_t item_id)
    {
        if(!item_exist(item_id))
        {
            return nullptr;
        }

        return m_nurbs_items[item_id];
    }

    inline bool remove_item(const uint32_t item_id)
    {
        if(m_nurbs_items.find(item_id) != m_nurbs_items.end())
        {
            m_nurbs_items.erase(item_id);
            return true;
        }
        return false;
    }

    void save(SketchSerializer& serializer) override;

    void load(SketchSerializer& serializer) override;

    inline void dirty() { m_dirty_flags = true; }

    inline std::map<uint32_t, std::shared_ptr<INurbsBase>> get_all_nurbs() const { return m_nurbs_items; }

    inline void merge(const std::shared_ptr<NurbsData> other)
    {
        if(other)
        {
            for(const auto& pair: other->get_all_nurbs())
            {
                add_item(pair.second);
            }
        }
    }

    nous::AABB3 get_bounding_box() const;

private:
    bool _update_render_resource();

private:
    uint32_t m_dirty_flags;
    std::map<uint32_t, std::shared_ptr<INurbsBase>> m_nurbs_items;
    nous::AABB3 m_bounding_box;
};

using NurbsDataPtr = std::shared_ptr<NurbsData>;

};// namespace sketch
