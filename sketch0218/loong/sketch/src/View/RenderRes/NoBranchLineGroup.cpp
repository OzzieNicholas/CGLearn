#pragma once
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/Element.h"

namespace sketch
{

NoBranchLineGroup::NoBranchLineGroup() : ElementGroup(EElementType::NoBranchLineGroup) {}

NoBranchLineGroup::~NoBranchLineGroup() { this->clear(); }

void NoBranchLineGroup::clear()
{
    for(auto& ele: m_line_elements)
    {
        ele->set_group(nullptr);
    }
    m_line_elements.clear();
}

void NoBranchLineGroup::merge(ElementGroupPtr group)
{
    if(group->type() != this->type())
    {
        return;
    }
}

void NoBranchLineGroup::add_element(std::shared_ptr<SketchElement> element)
{
    if(element->type() == EElementType::Line)
    {
        m_need_check = true;
        if(element->group())
        {
            element->group()->erase_element(element);
        }
        element->set_group(this->to_group());
        m_line_elements.insert(element);
    }
}

void NoBranchLineGroup::add_elements(std::unordered_set<std::shared_ptr<SketchElement>> elements)
{
    for(auto& element: elements)
    {
        if(element->type() == EElementType::Line)
        {
            m_need_check = true;
            if(element->group())
            {
                element->group()->erase_element(element);
            }
            element->set_group(this->to_group());
            m_line_elements.insert(element);
        }
    }
}

void NoBranchLineGroup::erase_element(std::shared_ptr<SketchElement> element)
{
    if(element->type() == EElementType::Line)
    {
        m_need_check = true;
        if(m_line_elements.erase(element))
        {
            element->set_group(nullptr);
        }
    }
}

std::unordered_set<std::shared_ptr<SketchElement>> NoBranchLineGroup::elements() { return m_line_elements; }

void NoBranchLineGroup::add_force_separate_point(std::shared_ptr<SketchElement> element) { m_force_separate_points.insert(element); }

void NoBranchLineGroup::add_force_separate_points(std::unordered_set<std::shared_ptr<SketchElement>> elements)
{
    m_force_separate_points.insert(elements.begin(), elements.end());
}

void NoBranchLineGroup::erase_force_separate_points(std::shared_ptr<SketchElement> element) { m_force_separate_points.erase(element); }

}// namespace sketch
