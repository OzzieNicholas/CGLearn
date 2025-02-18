#pragma once

#include "nous_mesh/mesh.h"
#include <vector>

namespace sketch
{
namespace uv
{

struct DecomposedSubMesh
{
    std::vector<nous::pos_t> vertices;
    std::vector<nous::vec3i> triangles;
    std::vector<nous::vec2> uvs;
};

std::vector<DecomposedSubMesh> auto_mapping_trimesh_uv(std::vector<nous::pos_t>& vertices, const std::vector<nous::vec3i>& triangles,
                                                       const bool use_LSCM, const nous::scalar angle, const nous::scalar margin);

};// namespace uv
};// namespace sketch