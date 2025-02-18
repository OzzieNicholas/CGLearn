#pragma once
#include "./Element.h"

namespace sketch
{
class SketchElement;
class SmoothGroup : public ElementGroup
{
public:
    SmoothGroup();
    virtual ~SmoothGroup();

    bool smooth_edge() { return m_smooth_edge; }
    void set_smooth_edge(bool smooth_edge) { m_smooth_edge = smooth_edge; }

    void clear();
    void merge(ElementGroupPtr group);
    void add_element(std::shared_ptr<SketchElement> element) override;
    void add_elements(std::unordered_set<std::shared_ptr<SketchElement>> elements) override;
    void erase_element(std::shared_ptr<SketchElement> element) override;
    virtual std::unordered_set<std::shared_ptr<SketchElement>> elements() override;

    std::unordered_set<std::shared_ptr<SketchElement>> soft_polygon() { return m_smooth_polygon_elements; }
    std::unordered_set<std::shared_ptr<SketchElement>> soft_edge() { return m_soft_edge_elements; }

private:
    bool m_smooth_edge;// 平滑法线
    std::unordered_set<std::shared_ptr<SketchElement>> m_smooth_polygon_elements;
    std::unordered_set<std::shared_ptr<SketchElement>> m_soft_edge_elements;// weld_line & line
};
using SmoothGroupPtr = std::shared_ptr<SmoothGroup>;

}// namespace sketch
