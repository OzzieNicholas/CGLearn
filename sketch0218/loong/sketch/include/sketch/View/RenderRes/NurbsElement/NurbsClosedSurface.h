#pragma once
#include "INurbsSurfaceBase.h"
#include "NurbsCurve.h"
#include <iostream>

namespace sketch
{

class NurbsTrimmedSurface;
class NurbsClosedSurfaceEdgeElement;

struct NurbsClosedSurfaceCurve
{
    NurbsCurvePtr m_inner_curve;
    float m_min_u;
    float m_max_u;
};

// 由一个曲线或一堆曲线的部分拼接成的边界表示对象，等效犀牛多重曲线构建的曲面
// 这玩意当且仅当里面只有一个对象的时候可编辑
class NurbsClosedSurface : public INurbsSurfaceBase
{
public:
    NurbsClosedSurface() : INurbsSurfaceBase(EElementType::ClosedSurface) {}

    ~NurbsClosedSurface();

    void generate_render_mesh(IRenderable* render_data) override;

    nous::AABB3 get_bounding_box() override;

    void save(SketchSerializer& serializer) override;

    void load(SketchSerializer& serializer) override;

    bool is_valid() override;

    std::shared_ptr<INurbsBase> clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                      const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    inline size_t get_curve_number() const { return m_inner_curve_reps.size(); }

    inline NurbsClosedSurfaceCurve get_brep_curve_range(const size_t index) { return m_inner_curve_reps[index]; }

    std::vector<std::vector<nous::pos_t>> get_sampled_triangles();

    std::vector<nous::pos_t> get_sampled_boundary();

    std::vector<std::vector<SketchNurbsPointPtr>> get_surface_control_points() override;

    std::vector<SketchNurbsPointPtr> get_surface_control_points_1D() override;

    std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> get_surface_elements();

    void set_curve_control_points(const unsigned int curve_idx, const unsigned int pt_idx, const nous::pos_t& position);

    void add_sub_curve(NurbsCurvePtr curve, double min_u, double max_u);

    void add_sub_curve_without_check(NurbsCurvePtr curve);

    virtual void transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    inline const std::vector<nous::pos_t>& get_curve_position_arr() { return m_cached_sample_points; }

    virtual void dirty() override;

    void set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    void set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    std::pair<std::shared_ptr<NurbsTrimmedSurface>, std::shared_ptr<NurbsTrimmedSurface>> get_parent() const;

    virtual bool is_surface_coplane(nous::vec3& coplane_normal) override;

private:
    void _sample_object();

private:
    std::vector<NurbsClosedSurfaceCurve> m_inner_curve_reps;

    std::vector<nous::pos_t> m_cached_sample_points;

    std::vector<std::vector<nous::pos_t>> m_cached_sample_triangles;

    std::weak_ptr<NurbsTrimmedSurface> m_parent_ex;
    std::weak_ptr<NurbsTrimmedSurface> m_parent_in;

    std::unordered_map<std::shared_ptr<SketchElement>, std::vector<nous::pos_t>> m_boundary_curve_elements;
};

using NurbsClosedSurfacePtr = std::shared_ptr<NurbsClosedSurface>;

};// namespace sketch