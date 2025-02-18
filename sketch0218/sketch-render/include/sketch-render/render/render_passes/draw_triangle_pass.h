#pragma once

#include "sketch_render_pass.h"

namespace sketch::render
{

class SKETCH_RENDER_API DrawTrianglePass : public ISketchRenderPass<DrawTrianglePass>
{
public:
    ~DrawTrianglePass() override = default;

public:
    void initialize(const SketchRenderContext& context) override;

    void destroy() override;

    void on_record_command(const SketchRenderContext& context, WGPUCommandEncoder encoder) override;

private:
    WGPURenderPipeline m_pipeline = nullptr;
};

}// namespace sketch::render
