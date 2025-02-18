#pragma once

#include <unordered_set>

#include "../../View/RenderRes/Element.h"
#include "nous_base/nous_base.h"
#include "sketch/Define.h"

namespace nous
{
class Point;
class Prim;
}// namespace nous
namespace sketch
{
class SketchElement;
class RenderObject;
class View;
class ViewCamera;
struct SelectInfo
{
    SelectInfo() : mesh(nullptr) {}
    SelectInfo(RenderObject* mesh, const std::unordered_set<SketchElementPtr>& elements) : mesh(mesh), elements(elements) {}
    RenderObject* mesh;
    std::unordered_set<SketchElementPtr> elements;
};
class ElementSelector
{
public:
    enum SelectMode : uint8_t
    {
        NORMAL = 1 << 0,
        ADD = 1 << 1,
        SUB = 1 << 2
    };
    ElementSelector();
    virtual ~ElementSelector();

    inline void clear() { m_selectInfo.clear(); }

    void update(View* pView, int x, int y);
    void update(View* pView, int minx, int miny, int maxx, int maxy, bool containOnly);
    inline void add_select_element(RenderObject* obj, const std::unordered_set<SketchElementPtr>& elements)
    {
        m_selectInfo[obj] = SelectInfo(obj, elements);
    }
    inline const std::unordered_map<RenderObject*, SelectInfo>& get_select_info() { return m_selectInfo; }
    SelectInfo get_select_info(View* view, int x, int y);

    void render_obj_changed(Define::Message method, RenderObject* obj);

    inline void setSelectPixelRange(int val) { m_select_pixel_range = val * val; }
    inline void setSelectMode(SelectMode mode) { m_select_mode = mode; }

    void select_filter_view_group_area(View* view);

private:
    void _get_select_info(View* view, int x, int y, RenderObject** pick_obj, SketchElementPtr* pick_ele);

    int m_select_pixel_range;
    uint8_t m_select_mode;

    std::unordered_map<RenderObject*, SelectInfo> m_selectInfo;
};

}// namespace sketch