#pragma once

#include "../../../../View/RenderRes/NurbsData.h"
#include "../../../../View/Select/ElementSelector.h"
#include "..\Controller_Main.h"

namespace nous
{
class Point;
}// namespace nous

namespace sketch
{
class TFObjectDoc;
class MeshDoc;
class NurbsDoc;
class View;
class SketchPolygon;
class ElementSelector;
class RenderObject;

class Controller_Edit : public Controller_Main
{
public:
    Controller_Edit(const std::wstring& name);
    virtual ~Controller_Edit();

public:
    virtual void active(const SketchParamMap& props) override;
    virtual void deactive() override;

protected:
    virtual void _clear_affected_meshes();
    virtual void _prepar_affect_meshes_elements(const std::unordered_map<RenderObject*, SelectInfo>& select_info);
    virtual void _update_new_meshes();
    virtual void _render_new_meshes(View* view);
    virtual void _add_affected_element_to_infer_ignore();

protected:
    bool m_keep_connection;
    std::unordered_map<TFObjectDoc*, nous::mat4> m_affected_bundles;
    std::unordered_map<NurbsDoc*, std::vector<std::pair<SketchNurbsPointPtr, nous::pos_t>>> m_affected_nurbs_points;
    std::unordered_map<MeshDoc*, std::unordered_map<SketchPointPtr, nous::vec3>> m_affected_mesh_local_position;
    std::unordered_map<MeshDoc*, std::unordered_set<SketchLinePtr>> m_affected_mesh_local_line;
    std::unordered_map<MeshDoc*, std::unordered_set<std::shared_ptr<SketchPolygon>>> m_affected_mesh_polygons;
    std::unordered_map<std::shared_ptr<SketchPolygon>, std::vector<std::vector<SketchPointPtr>>> m_affected_polygon_topo;
    std::unordered_map<std::shared_ptr<SketchPolygon>, std::vector<std::vector<std::pair<SketchPointPtr, nous::vec3>>>> m_affected_polygon_triangles;

    std::unordered_set<std::shared_ptr<SketchPolygon>> m_selected_polygons;
    std::unordered_map<MeshDoc*, std::unordered_map<std::shared_ptr<SketchPolygon>, std::vector<std::vector<std::vector<nous::vec3>>>>>
        m_mesh_new_polygons;

    // std::unordered_map<NurbsDoc*, std::vector<std::tuple<size_t, ENurbsDataType, std::vector<nous::pos_t>>>> m_selected_nurbs_item;
};
}// namespace sketch