#pragma once
#include "../../../../../View/RenderRes/NurbsData.h"
#include "../../../../../View/Select/ElementSelector.h"
#include "../../Controller_Main.h"

namespace sketch
{
class NurbsDoc;
class GeoData;
class Controller_Extrude_Freedom : public Controller_Main
{
public:
    Controller_Extrude_Freedom(const std::wstring& name);
    ~Controller_Extrude_Freedom();

    virtual SketchParamMap info() const;

    virtual void active(const SketchParamMap& pProps);
    virtual void deactive();

    virtual void escape(const SketchParamMap& props);
    virtual void enter(const SketchParamMap& props);
    void key_down(int key, bool ctrl, bool alt, bool shift) override;
    void key_up(int key, bool ctrl, bool alt, bool shift) override;
    void mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift) override;

    void prepare_render(View* view) override;

private:
    void _process_select_info(std::unordered_map<RenderObject*, SelectInfo>& select_info);

    void _prepare_fixing_extrude();

    void _rollback_to_pick_element();

    void clear_all_cache();

private:
    enum class ESketchExtrudeStep : int8_t
    {
        PickElement,
        FixingSurface
    };

    std::map<NurbsDoc*, std::vector<SketchElementPtr>> m_selected_curves_info;
    std::map<NurbsDoc*, std::unordered_set<uint32_t>> m_cached_tmp_nurbs_items;

    ESketchExtrudeStep m_extrude_step = ESketchExtrudeStep::PickElement;

    // 存储了被选择的 line line_group polygon polygon_group 转出来的线段
    std::vector<std::pair<nous::pos_t, nous::pos_t>> m_selected_or_transfered_lines;
    // 存储了用于封顶的 polygon 对象
    std::vector<std::vector<std::vector<nous::pos_t>>> m_selected_polygons;

    nous::pos_t m_direction_origin;
    nous::pos_t m_direction;

    // NurbsDoc* m_new_surface_doc = nullptr;
    // NurbsDataPtr m_new_surface_data = nullptr;
    MeshDoc* m_new_polygon_doc = nullptr;
    std::shared_ptr<GeoData> m_new_polygon_data = nullptr;

    nous::pos_t m_hover_pos;
    nous::pos_t m_guide_pos;

    nous::pos_t m_proj_on_dir_line_pos;
};

};// namespace sketch