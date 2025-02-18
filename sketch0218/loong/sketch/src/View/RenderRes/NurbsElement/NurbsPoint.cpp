#include "sketch/View/RenderRes/NurbsElement/NurbsPoint.h"
#include "sketch/View/RenderRes/NurbsElement/INurbsBase.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsAlgebraSurface.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsClosedSurface.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsCurve.h"

namespace sketch
{

void SketchNurbsPoint::set_position(const nous::pos_t& pos)
{
    m_point = pos;
    std::set<size_t, std::greater<size_t>> need_delete_parent_idx;
    for(int i = 0; i < m_parents.size(); ++i)
    {
        if(!m_parents[i].expired())
        {
            m_parents[i].lock()->dirty();
        }
        else
        {
            need_delete_parent_idx.insert(i);
        }
    }

    for(auto& idx: need_delete_parent_idx)
    {
        m_parents.erase(m_parents.begin() + idx);
    }
}

std::unordered_set<std::shared_ptr<INurbsBase>> SketchNurbsPoint::get_valid_parents()
{
    std::unordered_set<std::shared_ptr<INurbsBase>> result;
    for(int i = 0; i < m_parents.size(); ++i)
    {
        if(!m_parents[i].expired())
        {
            result.insert(m_parents[i].lock());
        }
    }
    return result;
}

void SketchNurbsPoint::remove_parent(INurbsBase* parent)
{
    std::set<int, std::greater<int>> delete_idx_set;
    for(int i = 0; i < m_parents.size(); ++i)
    {
        if(m_parents[i].expired() || m_parents[i].lock().get() == parent)
        {
            delete_idx_set.insert(i);
        }
    }

    for(int idx: delete_idx_set)
    {
        m_parents.erase(m_parents.begin() + idx);
    }
}

};// namespace sketch