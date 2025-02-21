#pragma once

#include "../Controller.h"

#include "nous_base/nous_base.h"

namespace sketch
{
class MeshDoc;
class SceneDoc;
class InferenceHelper;
class InferResult;
class Controller_Main : public Controller
{
public:
    Controller_Main(const std::wstring& name);
    ~Controller_Main() override;

    void active(const SketchParamMap& props) override;
    void deactive() override;

    void mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift) override;
    void event_filter(const std::string& event, const SketchParamMap& props) override;

    virtual void _lock_mouse_move();
    virtual void _unlock_mouse_move();
    virtual void _update_helper_mesh(InferenceHelper* infer_helper);
    virtual void _render_helper_mesh(View* view);
    void _check_intersection_and_merge_mesh(std::unordered_set<MeshDoc*>& meshes, std::unordered_set<MeshDoc*>& ignore_meshes, SceneDoc* scene);

protected:
    void escape(const SketchParamMap& props) override;
    void enter(const SketchParamMap& props) override;

protected:
    // input
    int m_lock_mouse_x;
    int m_lock_mouse_y;
    // helper mesh
    std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>> m_infer_point_tri;
    std::vector<std::pair<nous::vec4, std::pair<nous::vec3, nous::vec3>>> m_infer_point_edge;
    std::vector<std::pair<nous::vec4, std::pair<nous::vec3, nous::vec3>>> m_infer_lines;

    std::vector<std::pair<nous::vec4, std::vector<nous::vec3>>> m_refer_point_tri;
    std::vector<std::pair<nous::vec4, std::pair<nous::vec3, nous::vec3>>> m_refer_point_edge;
    std::vector<std::pair<nous::vec4, std::pair<nous::vec3, nous::vec3>>> m_refer_lines;
    std::vector<std::pair<nous::vec4, std::vector<std::pair<nous::vec3, nous::vec3>>>> m_refer_polygons_edge;
};
}// namespace sketch