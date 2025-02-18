#pragma once
#include "sketch/View/RenderRes/SmoothGroup.h"
#include "sketch/View/RenderRes/Element.h"

namespace sketch
{
SmoothGroup::SmoothGroup() : ElementGroup(EElementType::SmoothGroup), m_smooth_edge(false) {}

SmoothGroup::~SmoothGroup() { this->clear(); }

void SmoothGroup::clear()
{
    for(auto& ele: m_smooth_polygon_elements)
    {
        ele->set_group(nullptr);
    }
    for(auto& ele: m_soft_edge_elements)
    {
        ele->set_group(nullptr);
    }
    m_smooth_polygon_elements.clear();
    m_soft_edge_elements.clear();
}

void SmoothGroup::merge(ElementGroupPtr group)
{
    if(group->type() != this->type())
    {
        return;
    }
    auto smooth_group = std::dynamic_pointer_cast<SmoothGroup>(group);
    for(auto& ele: smooth_group->soft_edge())
    {
        this->add_element(ele);
    }
    for(auto& ele: smooth_group->soft_polygon())
    {
        this->add_element(ele);
    }
    group->clear();
    this->m_need_check = true;
}

void SmoothGroup::add_element(std::shared_ptr<SketchElement> element)
{
    if(element->type() == EElementType::Polygon)
    {
        m_need_check = true;
        if(element->group())
        {
            element->group()->erase_element(element);
        }
        element->set_group(this->to_group());
        m_smooth_polygon_elements.insert(element);
    }
    else if(element->type() == EElementType::Line)
    {
        m_need_check = true;
        if(element->group())
        {
            element->group()->erase_element(element);
        }
        element->set_group(this->to_group());
        m_soft_edge_elements.insert(element);
    }
}

void SmoothGroup::add_elements(std::unordered_set<std::shared_ptr<SketchElement>> elements)
{
    for(auto& ele: elements)
    {
        this->add_element(ele);
    }
}

void SmoothGroup::erase_element(std::shared_ptr<SketchElement> element)
{
    if(element->type() == EElementType::Polygon)
    {
        if(m_smooth_polygon_elements.erase(element))
        {
            m_need_check = true;
            element->set_group(nullptr);
        }
    }
    else if(element->type() == EElementType::Line)
    {
        if(m_soft_edge_elements.erase(element))
        {
            m_need_check = true;
            element->set_group(nullptr);
        }
    }
}

std::unordered_set<std::shared_ptr<SketchElement>> SmoothGroup::elements()
{
    std::unordered_set<std::shared_ptr<SketchElement>> elements;
    elements.insert(m_smooth_polygon_elements.begin(), m_smooth_polygon_elements.end());
    elements.insert(m_soft_edge_elements.begin(), m_soft_edge_elements.end());
    return elements;
}

}// namespace sketch
