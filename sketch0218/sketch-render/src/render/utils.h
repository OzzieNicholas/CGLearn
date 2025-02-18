#pragma once

#include "sketch-render/types.h"
#include "sketch/types.h"
#include <filesystem>
#include <webgpu.h>
#include <wgpu.h>

namespace sketch::render
{

WGPUShaderModule read_shader(WGPUDevice device, const std::filesystem::path& path);

string get_feature_name(WGPUNativeFeature feature);

mat4c look_at(const vec3& eye, const vec3& center, const vec3& up);

mat4c projection(float fov, float aspect, float z_near, float z_far);

}// namespace sketch::render
