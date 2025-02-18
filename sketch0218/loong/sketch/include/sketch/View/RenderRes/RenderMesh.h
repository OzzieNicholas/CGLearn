#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace sketch
{
class SketchElement;
struct SubmeshData
{
    enum class Topology : uint8_t
    {
        Points,
        Lines,
        Triangles,
        Quads,
    };

    enum class Status : uint8_t
    {
        Normal = 0,
        Selected,
        Background
    };

    SubmeshData(std::string idxname, size_t ds, size_t dc, Topology type, const std::shared_ptr<SketchElement>& prt)
        : index_name(std::move(idxname)), index_offset(ds), index_count(dc), topology(type), sketch_element(prt)
    {
    }

    std::string index_name;
    size_t index_offset = 0;
    size_t index_count = 0;
    Topology topology = Topology::Triangles;
    int material_id = 0;
    Status status = Status::Normal;

    // size_t element_id;
    std::weak_ptr<SketchElement> sketch_element;
};

struct VertexAttribute
{
    std::string name;
    size_t offset;
    size_t stride;
    size_t size;
};

struct RenderMeshData
{
    // vertex
    // 交错[not use yet]
    // std::vector<float> vertex;
    // std::vector<VertexAttribute> vertex_attributes;
    // 分离
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> colors;
    std::vector<float> uvs;
    std::vector<int> material_ids;

    std::array<float, 16> transform;

    /// index
    std::vector<unsigned int> triangles_index;
    std::vector<unsigned int> lines_index;

    /// submesh
    std::vector<SubmeshData> submeshes;
};

enum DirtyFlag : uint8_t
{
    DirtyFlag_None = 0,
    DirtyFlag_Mesh = 1 << 0,
    DirtyFlag_Normal = 1 << 1,
    DirtyFlag_Tangent = 1 << 2,
    DirtyFlag_Color = 1 << 3,
    DirtyFlag_UV = 1 << 4,
    DirtyFlag_Material = 1 << 5
};

struct RenderMesh
{
    long long id = -1;
    uint8_t dirty_flag = 0;

    std::shared_ptr<RenderMeshData> data;
};
}// namespace sketch
