#pragma once
#include "./Element.h"

namespace sketch
{
class SketchElement;
class NoBranchLineGroup : public ElementGroup
{
public:
    NoBranchLineGroup();
    virtual ~NoBranchLineGroup();

    void clear();
    void merge(ElementGroupPtr group);
    void add_element(std::shared_ptr<SketchElement> element);
    void add_elements(std::unordered_set<std::shared_ptr<SketchElement>> elements);
    void erase_element(std::shared_ptr<SketchElement> element);
    std::unordered_set<std::shared_ptr<SketchElement>> elements();

    void add_force_separate_point(std::shared_ptr<SketchElement> element);
    void add_force_separate_points(std::unordered_set<std::shared_ptr<SketchElement>> elements);
    void erase_force_separate_points(std::shared_ptr<SketchElement> element);
    void clear_force_separate_points() { m_force_separate_points.clear(); }
    std::unordered_set<std::shared_ptr<SketchElement>> force_separate_points() { return m_force_separate_points; }

private:
    std::unordered_set<std::shared_ptr<SketchElement>> m_line_elements;
    std::unordered_set<std::shared_ptr<SketchElement>> m_force_separate_points;
};
using NoBranchLineGroupPtr = std::shared_ptr<NoBranchLineGroup>;

}// namespace sketch
