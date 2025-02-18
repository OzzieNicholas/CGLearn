#pragma once
#include "./RenderMesh.h"
#include <map>

namespace sketch
{

class RenderMeshData;

class IRenderable
{
public:
    IRenderable() { m_render_mesh = std::make_shared<RenderMeshData>(); }
    virtual void clear()
    {
        m_render_mesh->positions.clear();
        m_render_mesh->normals.clear();
        m_render_mesh->tangents.clear();
        m_render_mesh->colors.clear();
        m_render_mesh->uvs.clear();
        m_render_mesh->material_ids.clear();
        m_render_mesh->transform = std::array<float, 16>();

        m_render_mesh->triangles_index.clear();
        m_render_mesh->lines_index.clear();

        m_render_mesh->submeshes.clear();
    }
    virtual bool update_render_mesh() { return false; }
    virtual std::shared_ptr<RenderMeshData> render_mesh() { return m_render_mesh; }

    virtual std::vector<float>& get_render_position() { return m_render_mesh->positions; }
    virtual std::vector<float>& get_render_normal() { return m_render_mesh->normals; }
    virtual std::vector<float>& get_render_uv() { return m_render_mesh->uvs; }
    virtual std::vector<float>& get_render_color() { return m_render_mesh->colors; }
    virtual std::vector<unsigned int>& get_render_triangles_index() { return m_render_mesh->triangles_index; }
    virtual std::vector<unsigned int>& get_render_lines_index() { return m_render_mesh->lines_index; }
    virtual std::vector<SubmeshData>& get_render_submesh() { return m_render_mesh->submeshes; }

protected:
    std::shared_ptr<RenderMeshData> m_render_mesh;
};

}// namespace sketch
