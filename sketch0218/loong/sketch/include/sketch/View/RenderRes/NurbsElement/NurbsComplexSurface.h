#pragma once

#include "INurbsBase.h"
#include "INurbsSurfaceBase.h"

namespace sketch
{

class NurbsAlgebraSurface;

class NurbsTrimmedSurface : public INurbsSurfaceBase
{
public:
    NurbsTrimmedSurface();

    NurbsTrimmedSurface(std::shared_ptr<INurbsSurfaceBase> exterior_ring);

    NurbsTrimmedSurface(std::shared_ptr<INurbsSurfaceBase> exterior_ring, const std::vector<std::shared_ptr<INurbsSurfaceBase>>& interior_rings);

    ~NurbsTrimmedSurface();

    void finalization();

    virtual std::shared_ptr<INurbsSurfaceBase> get_exterior_ring() const;

    virtual std::shared_ptr<INurbsSurfaceBase> get_interior_ring_at(const unsigned int idx) const;

    virtual std::vector<std::shared_ptr<INurbsSurfaceBase>> get_interior_rings() const;

    virtual void set_exterior_ring(std::shared_ptr<INurbsSurfaceBase> surface);

    virtual void add_interior_ring(std::shared_ptr<INurbsSurfaceBase> surface);

    virtual void remove_interior_ring_at(const unsigned int idx);

    virtual void generate_render_mesh(IRenderable* render_data) override;

    virtual nous::AABB3 get_bounding_box() override;

    virtual void transform(std::unordered_set<SketchNurbsPointPtr>& transformed_set, const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    virtual void save(SketchSerializer& serializer) override;

    virtual void load(SketchSerializer& serializer) override;

    virtual bool is_valid() override;

    virtual std::shared_ptr<INurbsBase> clone(std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr>& remaped_map,
                                              const nous::mat4& trans_mat = nous::mat4::Identity()) override;

    std::vector<std::vector<SketchNurbsPointPtr>> get_surface_control_points() override;

    std::vector<SketchNurbsPointPtr> get_surface_control_points_1D() override;

    std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> get_complex_surface_elements();

    void get_surface_sampled_triangles(std::vector<nous::vec3>& sampled_point_arr, std::vector<nous::vec3i>& triangle_idx_arr);

    void get_surface_sampled_triangles_without_uv_split(std::vector<nous::vec3>& sampled_point_arr, std::vector<nous::vec3i>& triangle_idx_arr);

    void set_parent_as_exring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    void set_parent_as_inring(std::shared_ptr<NurbsTrimmedSurface> parent) override;

    void set_basic_algebra_surface(std::shared_ptr<NurbsAlgebraSurface> algebra_surface);

    std::shared_ptr<NurbsAlgebraSurface> get_basic_algebra_surface() const;

    size_t get_inring_number() const;

    EElementType get_surface_exring_type() const;

    virtual bool is_surface_coplane(nous::vec3& coplane_normal) override;

    void get_sampled_triangles_low_resolution_when_exring_algebra_surface(unsigned int rows, unsigned int cols,
                                                                          std::vector<nous::vec3>& sampled_point_arr,
                                                                          std::vector<nous::vec3i>& triangle_idx_arr);

private:
    void _sample_surface();

    void _generate_sub_elements();

    void _generate_render_resources();

    void _generate_element_by_subsurface(std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>>& complex_surface_elements,
                                         INurbsSurfaceBasePtr ring);

private:
    std::shared_ptr<NurbsAlgebraSurface> m_basic_algebra_surface = nullptr;
    std::shared_ptr<INurbsSurfaceBase> m_exterior_ring = nullptr;
    std::vector<std::shared_ptr<INurbsSurfaceBase>> m_interior_rings;

    // 主要是边，所以记录一下各个 Element 在 m_sampled_edge_ref_idx_arr 的区间范围
    std::unordered_map<SketchElementPtr, std::vector<nous::pos_t>> m_complex_surface_elements;
    std::vector<nous::pos_t> m_unique_sampled_point_arr;
    std::vector<nous::vec2> m_unique_sampled_uv_arr;
    std::vector<nous::vec3> m_unique_sampled_normal_arr;
    std::vector<nous::vec3i> m_sampled_triangle_ref_idx_arr;

    std::vector<nous::pos_t> m_unique_sampled_point_arr_without_uv_split;
    std::vector<nous::vec3i> m_sampled_triangle_ref_idx_arr_without_uv_split;
};

using NurbsTrimmedSurfacePtr = std::shared_ptr<NurbsTrimmedSurface>;

};// namespace sketch