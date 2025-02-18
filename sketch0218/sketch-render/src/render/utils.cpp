#include "utils.h"
#include <fstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sketch::render
{

WGPUShaderModule read_shader(WGPUDevice device, const std::filesystem::path& path)
{
    std::ifstream file(path);
    std::string code((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    WGPUShaderModuleWGSLDescriptor shader_code_desc = {};
    shader_code_desc.chain.next = nullptr;
    shader_code_desc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shader_code_desc.code = code.data();

    WGPUShaderModuleDescriptor shader_desc = {};
    shader_desc.nextInChain = &shader_code_desc.chain;
    shader_desc.hints = nullptr;
    shader_desc.hintCount = 0;

    return wgpuDeviceCreateShaderModule(device, &shader_desc);
}

string get_feature_name(WGPUNativeFeature feature)
{
    switch(feature)
    {
    case WGPUNativeFeature_PushConstants: return "WGPUNativeFeature_PushConstants";
    case WGPUNativeFeature_TextureAdapterSpecificFormatFeatures: return "WGPUNativeFeature_TextureAdapterSpecificFormatFeatures";
    case WGPUNativeFeature_MultiDrawIndirect: return "WGPUNativeFeature_MultiDrawIndirect";
    case WGPUNativeFeature_MultiDrawIndirectCount: return "WGPUNativeFeature_MultiDrawIndirectCount";
    case WGPUNativeFeature_VertexWritableStorage: return "WGPUNativeFeature_VertexWritableStorage";
    case WGPUNativeFeature_TextureBindingArray: return "WGPUNativeFeature_TextureBindingArray";
    case WGPUNativeFeature_SampledTextureAndStorageBufferArrayNonUniformIndexing:
        return "WGPUNativeFeature_SampledTextureAndStorageBufferArrayNonUniformIndexing";
    case WGPUNativeFeature_PipelineStatisticsQuery: return "WGPUNativeFeature_PipelineStatisticsQuery";
    case WGPUNativeFeature_StorageResourceBindingArray: return "WGPUNativeFeature_StorageResourceBindingArray";
    case WGPUNativeFeature_PartiallyBoundBindingArray: return "WGPUNativeFeature_PartiallyBoundBindingArray";
    case WGPUNativeFeature_TextureFormat16bitNorm: return "WGPUNativeFeature_TextureFormat16bitNorm";
    case WGPUNativeFeature_TextureCompressionAstcHdr: return "WGPUNativeFeature_TextureCompressionAstcHdr";
    case WGPUNativeFeature_MappablePrimaryBuffers: return "WGPUNativeFeature_MappablePrimaryBuffers";
    case WGPUNativeFeature_BufferBindingArray: return "WGPUNativeFeature_BufferBindingArray";
    case WGPUNativeFeature_UniformBufferAndStorageTextureArrayNonUniformIndexing:
        return "WGPUNativeFeature_UniformBufferAndStorageTextureArrayNonUniformIndexing";
    case WGPUNativeFeature_VertexAttribute64bit: return "WGPUNativeFeature_VertexAttribute64bit";
    case WGPUNativeFeature_TextureFormatNv12: return "WGPUNativeFeature_TextureFormatNv12";
    case WGPUNativeFeature_RayTracingAccelerationStructure: return "WGPUNativeFeature_RayTracingAccelerationStructure";
    case WGPUNativeFeature_RayQuery: return "WGPUNativeFeature_RayQuery";
    case WGPUNativeFeature_ShaderF64: return "WGPUNativeFeature_ShaderF64";
    case WGPUNativeFeature_ShaderI16: return "WGPUNativeFeature_ShaderI16";
    case WGPUNativeFeature_ShaderPrimitiveIndex: return "WGPUNativeFeature_ShaderPrimitiveIndex";
    case WGPUNativeFeature_ShaderEarlyDepthTest: return "WGPUNativeFeature_ShaderEarlyDepthTest";
    case WGPUNativeFeature_Subgroup: return "WGPUNativeFeature_Subgroup";
    case WGPUNativeFeature_SubgroupVertex: return "WGPUNativeFeature_SubgroupVertex";
    case WGPUNativeFeature_SubgroupBarrier: return "WGPUNativeFeature_SubgroupBarrier";
    case WGPUNativeFeature_Force32: return "WGPUNativeFeature_Force32";
    }
}

mat4c look_at(const vec3& eye, const vec3& center, const vec3& up)
{
    auto result = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(center[0], center[1], center[2]), glm::vec3(up[0], up[1], up[2]));
    return Eigen::Map<mat4c>(glm::value_ptr(result), 4, 4);
}

mat4c projection(float fov, float aspect, float z_near, float z_far)
{
    auto result = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
    return Eigen::Map<mat4c>(glm::value_ptr(result), 4, 4);
}

}// namespace sketch::render
