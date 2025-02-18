#pragma once

#include "Engine/Util/Path.h"
#include "Engine/Util/StrUtil.h"
#include "IRender.h"

namespace Engine
{
class DctLight;
class VertexBuffer;
class UniformBuffer;
class Texture2D;
class DrawBatchBlinnPhong;
class MeshData;
class FrameBuffer;
class MaterialBlinnPhong;
class AssetTexture;
class Asset;
}// namespace Engine

namespace sketch
{
class ViewDoc;
class CELL_BATCH_BLOCK;

class Render : public IRender
{
public:
    Render();
    ~Render() override;

    void init() override;
    void clear();

    void resize(uint32_t width, uint32_t height) override
    {
        m_logic_size.x = width;
        m_logic_size.y = height;
    }

    void render(ViewCamera* camera, Define::ViewRenderMode render_mode, bool dirty) override;

    void append_mesh(const RenderMesh& mesh) override;
    void append_render_point(int idx_mode, bool is_sel, bool is_hover, const std::array<float, 3>& pos_w) override;
    void append_render_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                            uint16_t pattern, uint8_t factor, bool clip, bool occlude) override;
    void append_render_screen_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                   const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool occlude) override;
    void append_render_triangle(const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                                const std::array<float, 3>& v2, bool occlude, bool twoSide, bool clip, float alpha) override;
    void append_delete_mesh(long long id) override;

protected:
    union BATCH_LINE_KEY {
        uint64_t key;

        struct
        {
            uint16_t pattern;
            uint8_t factor;
            uint8_t width;
            uint8_t bits;// 0:clip planes, 1:screen space
        };
    };

private:
    void render_scene(ViewCamera* camera, Define::ViewRenderMode render_mode, bool dirty);

    void prepare_render_target(ViewCamera* camera);
    void present(ViewCamera* camera);

    void prepare_batch_blocks();
    void reset_batch_blocks();

    void render_ctrl_point(ViewCamera* camera);

    void init_world_axis_res();
    void init_work_axis_res();
    void init_sky_box_res();

    void prepare_logic_canvas();
    void pre_render(ViewCamera* camera);
    void post_render();
    void present_physics();

    void release();

    Engine::MeshData* viewWorkAxisMesh();
    Engine::MeshData* viewWorldAxisMesh();
    Engine::MeshData* skyBoxMesh();

    Engine::MaterialBlinnPhong* getMtrl(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool cull, bool vcolor,
                                        uint8_t lineWidth, const std::string& key = Engine::SU::NULL_STR);
    Engine::MaterialBlinnPhong* getLineMaskMtrl(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool cull,
                                                uint8_t lineWidth, const std::string& mtrlKey, const std::string& maskKey);

    void load_texture_from_file(std::string texture_name, std::string img_path);

    Engine::MaterialBlinnPhong* get_texture_material(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool cull,
                                                     bool vcolor, uint8_t lineWidth, std::string& texture_name);

    Engine::AssetTexture* getLineMaskTex(const std::string& key);
    Engine::AssetTexture* getLineColorStampTex(const std::string& key);

    std::unique_ptr<Engine::Texture2D> m_render_target;

    std::unique_ptr<Engine::DctLight> m_light;
    std::unordered_map<long long, RenderMesh> m_render_meshes;

    Engine::DrawBatchBlinnPhong* m_world_axis_batch;
    Engine::DrawBatchBlinnPhong* m_sky_box_batch;

    std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_line_batches;
    std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_blend_line_batches;
    std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_solid_tri_batches;
    std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_trans_tri_batches;
    CELL_BATCH_BLOCK* m_scr_uni_line_batches[8];
    // ctrl point batches
    CELL_BATCH_BLOCK* m_cpts_inst_batches[32];
    static Engine::VertexBuffer* m_cpts_buf;
    Engine::UniformBuffer* m_cpts_common_ubo;

    std::unordered_set<std::shared_ptr<Engine::DrawBatchBlinnPhong>> m_drawBatches;
    std::unordered_map<long long, std::unique_ptr<Engine::MeshData>> m_render_data;

    std::map<std::string, Engine::Asset*> m_loaded_texture_asset;

    Engine::FrameBuffer* m_logic_fbo;
    Engine::Texture2D* m_logic_canvas;

    Engine::Vec2i m_logic_size;

    std::unordered_map<unsigned long long, std::map<std::string, Engine::MaterialBlinnPhong*>> m_mtrlMap;
    bool m_is_init;
};
}// namespace sketch
