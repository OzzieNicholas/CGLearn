#pragma once
#include "../../../../../View/RenderRes/NurbsData.h"
#include "../../../../../View/Select/ElementSelector.h"
#include "../../Controller_Main.h"

namespace sketch
{
class RenderObject;
class NurbsDoc;

class Controller_Extrude_ByPath : public Controller_Main
{
public:
    Controller_Extrude_ByPath(const std::wstring& name);
    ~Controller_Extrude_ByPath() override;

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& props) override;
    virtual void deactive() override;

    virtual void escape(const SketchParamMap& props) override;
    virtual void enter(const SketchParamMap& props);
    virtual void key_down(int key, bool ctrl, bool alt, bool shift);
    virtual void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    virtual void mouse_click(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    virtual void prepare_render(View* view) override;

protected:
    bool process_select_elements(std::unordered_map<RenderObject*, SelectInfo>& select_infos);

    bool process_path_element(std::unordered_map<RenderObject*, SelectInfo>& select_infos);

    bool generate_extrude_mesh_or_surface();

    void rollback_to_pick_route_element();

    void rollback_to_pick_shape_element();

private:
    enum class EExtrudePathStep : uint8_t
    {
        PickShapeElement,
        PickRouteElement,
        CreateSurface
    };

    enum class EExtrudePathType : uint8_t
    {
        Polyline,
        Curve
    };

    enum class EExtrudePathShapeType : uint8_t
    {
        LineOrWeldLine,
        Polygon,
        Surface,
        CurveOrCurveGroup,
    };

    EExtrudePathStep m_extrude_path_step = EExtrudePathStep::PickRouteElement;

    EExtrudePathType m_extrude_path_type = EExtrudePathType::Curve;

    EExtrudePathShapeType m_extrude_path_shape_type = EExtrudePathShapeType::LineOrWeldLine;

    // 选中的所有线
    std::vector<std::pair<nous::pos_t, nous::pos_t>> m_selected_lines;

    // 多边形的点信息
    std::vector<std::vector<nous::pos_t>> m_selected_polygon_positions;
    // 多边形线的焊接信息，各算各的，内外环互不干扰
    std::vector<std::vector<std::pair<size_t, size_t>>> m_selected_polygon_boundary_weld_info;
    std::map<int, unsigned int> m_polygon_boundary_side_surfaces;

    // 选中的曲线集合
    std::vector<NurbsCurvePtr> m_selected_curves;
    NurbsAlgebraSurfacePtr m_selected_algebra_surface;
    NurbsClosedSurfacePtr m_selected_closed_surface;
    NurbsTrimmedSurfacePtr m_selected_trimmed_surface;

    NurbsCurvePtr m_curve_path = nullptr;
    NurbsCurvePtr m_curve_path_in_doc_transform = nullptr;

    bool m_is_polygon = false;

    NurbsDoc* m_picked_nurbs_doc = nullptr;
    std::unordered_set<uint32_t> m_cached_tmp_nurbs_items;

    std::vector<NurbsCurvePtr> m_prepared_extrude_curve;

    std::vector<nous::pos_t> m_path_inference_line;

    nous::pos_t m_hover_pos;
    nous::pos_t m_cross_pos;

    float m_last_u = 0.0;
};

};// namespace sketch