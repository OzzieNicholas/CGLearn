#pragma once
#include "../../../../../View/RenderRes/NurbsElement/NurbsElements.h"
#include "../../../../../View/Select/ElementSelector.h"
#include "../../../../RenderRes/GeoData.h"
#include "../../Controller_Main.h"
#include "../../Mesh/Controller_Edit.h"
#include "nous_base/nous_base.h"

namespace nous
{
class Point;
class Prim;
}// namespace nous

namespace sketch
{
class MeshDoc;
class Controller_Extrude_ByNormal : public Controller_Edit
{
public:
    Controller_Extrude_ByNormal(const std::wstring& name);
    virtual ~Controller_Extrude_ByNormal();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& props);
    virtual void deactive();

    // void onSnapPoint(const SnapPoint& pt) override;

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view) override;

private:
    void processPickPolygon(GeoData* picked_geo, SketchRingPtr ring);
    void switchMesh();

    void create_add_operation_mesh(double offset);
    void create_move_operation_mesh(double offset);

    void commit_extrude_mesh();

    std::unordered_set<SketchRingPtr> get_valid_coplane_rings(std::unordered_set<SketchRingPtr>& line_adj_rings,
                                                              std::unordered_set<SketchRingPtr>& self_rings, GeoData* geodata);

    std::vector<std::vector<nous::vec3>> get_offseted_polygon_positions(GeoData* picked_geo, SketchPolygonPtr polygon, SketchPointPtr prev_pt,
                                                                        SketchPointPtr next_pt, double offset);

    std::vector<nous::pos_t> offset_ring_positions(GeoData* picked_geo, SketchRingPtr ring, SketchPointPtr prev_pt, SketchPointPtr next_pt,
                                                   double offset);

    enum class ESketchExtrudeMoveValidDir : unsigned char
    {
        Both = 3,
        Positive = 1,
        Negative = 2,
        None = 0,
    };

    // 判断一个多边形的正负法线方向上是否有其余 Polygon 限制其移动
    ESketchExtrudeMoveValidDir need_flip_picked_normal(GeoData* geodata, SketchPolygonPtr pick_polygon);

    void prepare_add_mode_inferring_mesh();

    void prepare_move_mode_inferring_mesh(GeoData* picked_geo, SketchPolygonPtr pick_polygon);

    void clear_all_cache_mesh_doc();

    void clear_inferring_mesh_doc();

    void clear_limits();

    void fix_inferring_mesh_doc();

protected:
    bool _process_select_info(const std::unordered_map<RenderObject*, SelectInfo>& select_info_map);

    void _create_nurbs_operation_mesh(const nous::scalar value);

    void _generate_algebra_surface_operation_mesh(const nous::scalar value);

    void _generate_closed_surface_operation_mesh(const nous::scalar value);

    void _generate_complex_surface_operation_mesh(const nous::scalar value);

    void _rollback_to_pick_element();

protected:
    enum class ESketchExtrudeMode : unsigned char
    {
        ADD = 0,
        MOVE = 1
    };
    ESketchExtrudeMode m_extrude_mode;

    enum class ESketchExtrudeStep : unsigned char
    {
        PICK_ELEMENT = 0,
        EXTRUDE_BEGIN_FIXING = 1,
        EXTRUDE_FIXING = 2
    };

    struct MoveExtrudeLineCorrespondPrim
    {
        nous::Prim* m_ring = nullptr;
        bool m_exist_flag = false;
    };

    ESketchExtrudeStep m_extrude_step;

    MeshDoc* m_pick_mesh_doc;
    nous::vec3 m_pick_polygon_normal;
    nous::vec3 m_extrude_polygon_begin_position;
    nous::vec3 m_hover_pos;

    std::vector<std::vector<SketchPointPtr>> m_selected_polygon_topo;

    std::vector<std::vector<SketchPointPtr>> m_new_add_polygon_topo_arr;

    std::unordered_set<SketchPointPtr> m_new_add_polygon_topo;

    std::vector<std::vector<SketchPointPtr>> m_new_move_polygon_topo_arr;

    std::unordered_set<SketchPointPtr> m_new_move_polygon_topo;

    MeshDoc* m_move_inferring_mesh_doc = nullptr;

    MeshDoc* m_add_inferring_mesh_doc = nullptr;

    MeshDoc* m_splited_old_mesh_doc = nullptr;

    MeshDoc* m_copyed_origin_mesh_doc = nullptr;

    double m_move_last_offset = 0.0;
    double m_add_last_offset = 0.0;

    ESketchExtrudeMoveValidDir m_valid_move_dir = ESketchExtrudeMoveValidDir::None;

    double m_negative_offset_limit = std::numeric_limits<double>::max();

    double m_positive_offset_limit = std::numeric_limits<double>::max();

    NurbsDoc* m_pick_nurbs_doc = nullptr;

    NurbsAlgebraSurfacePtr m_selected_algebra_surface;

    NurbsClosedSurfacePtr m_selected_closed_surface;

    NurbsTrimmedSurfacePtr m_selected_trimmed_surface;

    std::unordered_set<size_t> m_tmp_surfaces_guid;

    bool m_is_extrude_nurbs = false;
};
}// namespace sketch
