#include "sketch/View/RenderView/Render.h"

#include "Engine/Asset/AssetBitmapFont.h"
#include "Engine/Asset/AssetData/FontData.h"
#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"
#include "Engine/Asset/AssetData/MeshData.h"
#include "Engine/Asset/AssetData/SdfFontData.h"
#include "Engine/Asset/AssetData/Texture2D.h"
#include "Engine/Asset/AssetFont.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetMaterial.h"
#include "Engine/Asset/AssetMesh.h"
#include "Engine/Asset/AssetSdfFont.h"
#include "Engine/Asset/AssetTexture.h"
#include "Engine/Render/Device.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"
#include "Engine/Render/Environment.h"
#include "Engine/Render/OGLRes/Program.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Render/ShaderManager.h"
#include "Engine/Scene/Light/DctLight.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
// #include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/CELL_BATCH_BLOCK.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
VertexBuffer* Render::m_cpts_buf = nullptr;

Render::Render() : m_cpts_common_ubo(nullptr), m_world_axis_batch(nullptr), m_sky_box_batch(nullptr), m_logic_canvas(nullptr), m_logic_size(1, 1)
{
    m_is_init = false;
    m_logic_fbo = new FrameBuffer();
    m_logic_fbo->initFrameBuffer();

    memset(m_cpts_inst_batches, 0, sizeof(m_cpts_inst_batches));
    memset(m_scr_uni_line_batches, 0, sizeof(m_scr_uni_line_batches));
}

Render::~Render()
{
    if(m_logic_canvas)
    {
        delete m_logic_canvas;
        m_logic_canvas = nullptr;
    }

    if(m_logic_fbo)
    {
        delete m_logic_fbo;
        m_logic_fbo = nullptr;
    }
}

void Render::init()
{
    if(m_is_init)
    {
        return;
    }
    release();

    m_light = std::make_unique<DctLight>();
    m_light->setLightDir(-1.0f, 1.0f, -1.0f);
    m_light->setColor(1.0f, 1.0f, 1.0f);
    m_light->setIntensity(1.0f);

    this->load_texture_from_file("debug_img", "E:/GitRepository/sketch/loongstc_bin/bin_debug/debug.png");
    this->load_texture_from_file("pickdebug_img", "E:/GitRepository/sketch/loongstc_bin/bin_debug/pickdebug.png");

    init_work_axis_res();
    init_sky_box_res();
    m_is_init = true;
}

void Render::clear() { m_render_meshes.clear(); }

void Render::render(ViewCamera* camera, Define::ViewRenderMode render_mode, bool dirty)
{
    pre_render(camera);
    render_scene(camera, render_mode, dirty);
    post_render();
}

void Render::render_scene(ViewCamera* camera, Define::ViewRenderMode render_mode, bool dirty)
{
    Engine::Renderer* pRenderer = STE::renderer();
    Engine::Environment* pEnv = pRenderer->env();

    ViewManager* pViewMgr = STCore::inst->viewMgr();
    ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    PalleteManager* pPalMgr = pCfgMgr->palMgr();
    Pallete* pPal = pPalMgr->activePallete();
    Document* pDoc = STCore::inst->docMgr()->document();

    prepare_render_target(camera);
    if(!m_render_target)
    {
        return;
    }

    if(!dirty)
    {
        present(camera);
        return;
    }

    pEnv->setMultiSample(pCfgMgr->settings()->viewAA() ? 4 : 1);
    // setup view back color
    pEnv->enableCamBackColor(false);
    if(render_mode == Define::ViewRenderMode::RENDERMODE_WIREFRAME)
    {
        pEnv->setBackColor(pPal->viewBackWFTopLeft(), pPal->viewBackWFTopRight(), pPal->viewBackWFBottomLeft(), pPal->viewBackWFBottomRight());
    }
    else
    {
        pEnv->setBackColor(pPal->viewBackTopLeft(), pPal->viewBackTopRight(), pPal->viewBackBottomLeft(), pPal->viewBackBottomRight());
    }
    pEnv->setBackColor(0.8f, 0.8f, 0.8f, 0.8f);

    // setup lights
    pEnv->clearDctLights();
    pEnv->addDctLight(m_light.get());

    // render sky
    pRenderer->beginDraw(camera, { 0.0f, 0.0f, camera->viewportWidth(), camera->viewportHeight() });
    {
        auto& opaqueBatches = pEnv->opaqueBatches();
        {
            const Engine::Vec3f& cp = pDoc->v2d(camera->pos());
            const Engine::Vec3f& rd = camera->rightDir();
            Engine::Mat43 skyBoxMat;
            skyBoxMat.col[0] = rd;
            skyBoxMat.col[1] = rd.cross(Engine::Vec3(0.f, 0.f, 1.f)).normalize();
            skyBoxMat.col[2] = Engine::Vec3(0.f, 0.f, 1.f);
            if(camera->projType() == Engine::Camera::PROJ_ORTHO)
            {
                // const Engine::Vec4f& ov = pCam->orthoView();
                // const Engine::Vec4f& cv = pCam->viewport();
                // Engine::Mat43 scaleMat;
                // scaleMat.scale(1000.0f * (ov.y - ov.x) / cv.w);
                // skyBoxMat = scaleMat * skyBoxMat;
                // skyBoxMat.col[3] = cp;
                // auto& workAxis = m_pDoc->scene()->workAxis();
                // m_pSkyBoxBatch->setWorldMatrix((Mat43f)(pDoc->d2vMatrix() * workAxis * skyBoxMat));
                // opaqueBatches.push_back(m_pSkyBoxBatch);
            }
            else
            {
                skyBoxMat.col[3] = cp;
                m_sky_box_batch->setWorldMatrix((Mat43f)(pDoc->d2vMatrix() * skyBoxMat));
                opaqueBatches.push_back(m_sky_box_batch);
            }
        }
    }
    pRenderer->endDraw();

    pRenderer->beginDraw(camera, { 0.0f, 0.0f, camera->viewportWidth(), camera->viewportHeight() }, false, true);
    {
        auto& opaqueBatches = pEnv->opaqueBatches();
        {// draw axis
            m_world_axis_batch->setWorldMatrix((Mat43f)(pDoc->d2vMatrix()));
            opaqueBatches.push_back(m_world_axis_batch);
        }
    }
    pRenderer->endDraw();

    // render mesh
    pRenderer->beginDraw(camera, { 0.0f, 0.0f, camera->viewportWidth(), camera->viewportHeight() }, false, false);
    {

        auto& opaqueBatches = pEnv->opaqueBatches();
        auto& transBatches = pEnv->transBatches();

        // render mesh
        for(auto& mesh: m_render_meshes)
        {
            auto mesh_date = mesh.second.data;
            auto rm = m_render_data.try_emplace(mesh.first, std::make_unique<Engine::MeshData>(STE::device())).first->second.get();
            if(mesh.second.dirty_flag & DirtyFlag::DirtyFlag_Mesh)
            {
                mesh.second.dirty_flag &= ~DirtyFlag::DirtyFlag_Mesh;
                rm->clear();
                rm->createAttributeData("Vertex", mesh_date->positions.size(), mesh_date->positions.data());
                rm->createAttributeData("Normal", mesh_date->normals.size(), mesh_date->normals.data());
                if(!mesh_date->colors.empty())
                {
                    rm->createAttributeData("EmiColor", mesh_date->colors.size(), mesh_date->colors.data());
                }
                if(!mesh_date->uvs.empty())
                {
                    rm->createAttributeData("UV0", mesh_date->uvs.size(), mesh_date->uvs.data());
                }
                rm->createIndexData("Default", mesh_date->triangles_index.size(), mesh_date->triangles_index.data(),
                                    0x1405);// GL_UNSIGNED_SHORT 0x1403 | GL_UNSIGNED_INT 0x1405
                rm->createIndexData("Edge", mesh_date->lines_index.size(), mesh_date->lines_index.data(), 0x1405);
            }
            if(mesh.second.dirty_flag & DirtyFlag::DirtyFlag_Material)
            {
                mesh.second.dirty_flag &= ~DirtyFlag::DirtyFlag_Material;
            }

            Mat43f world_transform;
            world_transform.col[0].x = mesh_date->transform[0];
            world_transform.col[0].y = mesh_date->transform[1];
            world_transform.col[0].z = mesh_date->transform[2];
            world_transform.col[1].x = mesh_date->transform[4];
            world_transform.col[1].y = mesh_date->transform[5];
            world_transform.col[1].z = mesh_date->transform[6];
            world_transform.col[2].x = mesh_date->transform[8];
            world_transform.col[2].y = mesh_date->transform[9];
            world_transform.col[2].z = mesh_date->transform[10];
            world_transform.col[3].x = mesh_date->transform[12];
            world_transform.col[3].y = mesh_date->transform[13];
            world_transform.col[3].z = mesh_date->transform[14];

            for(auto& submesh: mesh_date->submeshes)
            {
                std::shared_ptr<DrawBatchBlinnPhong> db = std::make_shared<DrawBatchBlinnPhong>();
                db->setMeshData(rm);
                db->setClip(false);
                db->setCCW(true);
                db->setWorldMatrix(world_transform);
                db->setDrawRange(submesh.index_offset, submesh.index_count, 0);
                if(submesh.topology == SubmeshData::Topology::Triangles)
                {
                    Vec3f polygonColor = Vec3f(0.6f, 0.6f, 0.6f);
                    switch(submesh.material_id)
                    {
                    case 1: polygonColor = Vec3f(0.92f, 0.93f, 0.95f); break;
                    case 2: polygonColor = Vec3f(1.0f, 1.0f, 1.0f); break;
                    default: break;
                    }

                    std::string texture_name = "";
                    switch(submesh.material_id)
                    {
                    case 0: texture_name = "debug_img"; break;
                    case 1: texture_name = "pickdebug_img"; break;
                    case 2: texture_name = "pickdebug_img"; break;
                    default: break;
                    }

                    if(texture_name != "" && m_loaded_texture_asset.find(texture_name) != m_loaded_texture_asset.end())
                    {
                        db->setMaterial(this->get_texture_material(0, STRGB(polygonColor), true, true, true, true, false, true, 1.0f, texture_name));
                    }
                    else
                    {
                        db->setMaterial(this->getMtrl(0, STRGB(polygonColor), true, true, true, false, false, true, 1.0f));
                    }
                    db->setDrawMode(GL_TRIANGLES, 0);
                    db->setIndexName("Default", 0);
                    db->setDepthOffset(1.3f, 0.0f);
                }
                else if(submesh.topology == SubmeshData::Topology::Lines)
                {
                    Vec3f edgeColor = Vec3f::BLACK;
                    switch(submesh.material_id)
                    {
                    case 1: edgeColor = Vec3f::BLUE; break;
                    case 2: edgeColor = Vec3f(0.6f, 0.6f, 0.6f); break;
                    default: break;
                    }

                    db->setMaterial(this->getMtrl(STRGB(edgeColor), 0, false, true, true, false, false, true, 1.0f));
                    db->setDrawMode(GL_LINES, 0);
                    db->setIndexName("Edge", 0);
                }
                opaqueBatches.push_back(db.get());
                m_drawBatches.insert(db);
            }
        }

        // batch render
        prepare_batch_blocks();

        // batch render lines
        for(auto& itb: m_line_batches)
        {
            CELL_BATCH_BLOCK* pBlock = itb.second;
            if(pBlock->size == 0)
                continue;

            auto& key = *(BATCH_LINE_KEY*)&itb.first;
            if((key.bits & (1 << 1)) == 0)
            {// model space
                if(pBlock->pVBuf)
                    pRenderer->renderLines(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pVBuf, (int)pBlock->size / 6, float(key.width), true,
                                           true, false, true, key.bits & 1, key.factor, key.pattern);
                else
                    pRenderer->renderLines(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pData, (int)pBlock->size / 6, float(key.width), true,
                                           true, false, true, key.bits & 1, key.factor, key.pattern);
            }
            else
            {// screen space
                if(pBlock->pVBuf)
                    pRenderer->renderLines(Mat43f::IDENTITY, Mat4f::IDENTITY, pBlock->pVBuf, (int)pBlock->size / 6, float(key.bits & 7), true, true,
                                           false, true, false, key.factor, key.pattern);
                else
                    pRenderer->renderLines(Mat43f::IDENTITY, Mat4f::IDENTITY, pBlock->pData, (int)pBlock->size / 6, float(key.bits & 7), true, true,
                                           false, true, false, key.factor, key.pattern);
            }
        }

        for(int i = 0; i < sizeof(m_scr_uni_line_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
        {
            CELL_BATCH_BLOCK* pBlock = m_scr_uni_line_batches[i];
            if(!pBlock || pBlock->size == 0)
                continue;

            if(pBlock->pVBuf)
                pRenderer->renderScrUniLines(camera->dvpMatrix(), pBlock->pVBuf, (int)pBlock->size / 7, (float)i, true, true, 0, 0u);
            else
                pRenderer->renderScrUniLines(camera->dvpMatrix(), pBlock->pData, (int)pBlock->size / 7, (float)i, true, true, 0, 0u);
        }

        // batch render point
        render_ctrl_point(camera);

        // batch render triangles
        for(auto& itb: m_solid_tri_batches)
        {
            CELL_BATCH_BLOCK* pBlock = itb.second;
            if(pBlock->size == 0)
                continue;

            bool bTwoSide = itb.first & 1;
            bool bClip = itb.first & 2;
            if(pBlock->pVBuf)
                pRenderer->renderSolidTris(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pVBuf, (int)pBlock->size / 8, bTwoSide, false, bClip);
            else
                pRenderer->renderSolidTris(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pData, (int)pBlock->size / 8, bTwoSide, false, bClip);
        }
    }
    pRenderer->endDraw();

    // render transparent objects
    pRenderer->beginDraw(camera, { 0.0f, 0.0f, camera->viewportWidth(), camera->viewportHeight() }, false, false);
    {
        // batch render trans lines
        for(auto& itb: m_blend_line_batches)
        {
            CELL_BATCH_BLOCK* pBlock = itb.second;
            if(pBlock->size == 0)
                continue;

            auto& key = *(BATCH_LINE_KEY*)&itb.first;

            if(pBlock->pVBuf)
                pRenderer->renderLines(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pVBuf, (int)pBlock->size / 6, float(key.width), true, false,
                                       true, true, key.bits & 1, key.factor, key.pattern);
            else
                pRenderer->renderLines(camera->d2vMatrix(), camera->dvpMatrix(), pBlock->pData, (int)pBlock->size / 6, float(key.width), true, false,
                                       true, true, key.bits & 1, key.factor, key.pattern);
        }
    }
    pRenderer->endDraw();

    pRenderer->present({ 0, 0, (int)camera->viewportWidth(), (int)camera->viewportHeight() },
                       { 0, 0, (int)camera->viewportWidth(), (int)camera->viewportHeight() }, m_render_target.get());

    reset_batch_blocks();
    pEnv->enableCamBackColor(true);

    present(camera);
}

void Render::append_mesh(const RenderMesh& mesh) { m_render_meshes[mesh.id] = mesh; }

void Render::append_delete_mesh(long long id)
{
    m_render_meshes.erase(id);
    // do nothing
}

void Render::append_render_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool clip, bool occlude)
{
    BATCH_LINE_KEY key;
    key.pattern = pattern;
    key.factor = factor;
    key.width = width;
    key.bits = 0;

    if(clip)
        key.bits |= 1;

    CELL_BATCH_BLOCK* pBlock = nullptr;

    if(alpha >= 1.0f)
    {
        auto it = m_line_batches.find(key.key);
        if(it == m_line_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_line_batches[key.key] = pBlock;
        }
        else
            pBlock = it->second;
    }
    else
    {
        auto it = m_blend_line_batches.find(key.key);
        if(it == m_blend_line_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_blend_line_batches[key.key] = pBlock;
        }
        else
            pBlock = it->second;
    }

    pBlock->assure(6 * 2);
    pBlock->frame = STCore::inst->frame();
    float occ = occlude ? 0.0f : 1.0f;
    float clr = STPACK_RGB({ color[0], color[1], color[2] });
    pBlock->pData[pBlock->size++] = (float)v0[0];
    pBlock->pData[pBlock->size++] = (float)v0[1];
    pBlock->pData[pBlock->size++] = (float)v0[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = clr;
    pBlock->pData[pBlock->size++] = alpha;
    pBlock->pData[pBlock->size++] = (float)v1[0];
    pBlock->pData[pBlock->size++] = (float)v1[1];
    pBlock->pData[pBlock->size++] = (float)v1[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = clr;
    pBlock->pData[pBlock->size++] = alpha;
}

void Render::append_render_point(int idx_mode, bool is_sel, bool is_hover, const std::array<float, 3>& pos_w)
{
    uint64_t key = 0u;

    if(is_hover)
        key |= 0;// pPal->objHover()
    else if(is_sel)
        key |= 1;// pPal->objPick()
    else if(idx_mode == 0)
        key |= 2;// pPal->ctrlPointFirst()
    else if(idx_mode == 1)
        key |= 3;// linear from pPal->ctrlPointFirst() & pPal->ctrlPoint()
    else
        key |= 4;// pPal->ctrlPoint()

    if(is_sel)
        key |= (1 << 3);// scale & no border
    else if(is_hover)
        key |= (1 << 4);// scale

    if(!m_cpts_inst_batches[key])
        m_cpts_inst_batches[key] = new CELL_BATCH_BLOCK();

    auto pBlock = m_cpts_inst_batches[key];
    pBlock->assure(3);
    pBlock->frame = STCore::inst->frame();
    pBlock->pData[pBlock->size++] = (float)pos_w[0];
    pBlock->pData[pBlock->size++] = (float)pos_w[1];
    pBlock->pData[pBlock->size++] = (float)pos_w[2];
}

void Render::append_render_screen_line(int width, float alpha, const std::array<float, 3>& color, const std::array<float, 3>& v0,
                                       const const std::array<float, 3>& v1, uint16_t pattern, uint8_t factor, bool occlude)
{
    BATCH_LINE_KEY key;
    key.pattern = pattern;
    key.factor = factor;
    key.width = width;
    key.bits = (1 << 1);// screen space

    CELL_BATCH_BLOCK* pBlock = nullptr;

    if(alpha >= 1.0f)
    {
        auto it = m_line_batches.find(key.key);
        if(it == m_line_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_line_batches[key.key] = pBlock;
        }
        else
            pBlock = it->second;
    }
    else
    {
        auto it = m_blend_line_batches.find(key.key);
        if(it == m_blend_line_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_blend_line_batches[key.key] = pBlock;
        }
        else
            pBlock = it->second;
    }

    pBlock->assure(6 * 2);
    pBlock->frame = STCore::inst->frame();
    float occ = occlude ? 0.0f : 1.0f;
    float clr = STPACK_RGB(Engine::Vec3f(color[0], color[1], color[2]));
    pBlock->pData[pBlock->size++] = v0[0];
    pBlock->pData[pBlock->size++] = v0[1];
    pBlock->pData[pBlock->size++] = v0[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = clr;
    pBlock->pData[pBlock->size++] = alpha;
    pBlock->pData[pBlock->size++] = v1[0];
    pBlock->pData[pBlock->size++] = v1[1];
    pBlock->pData[pBlock->size++] = v1[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = clr;
    pBlock->pData[pBlock->size++] = alpha;
}

void Render::append_render_triangle(const std::array<float, 3>& color, const std::array<float, 3>& v0, const std::array<float, 3>& v1,
                                    const std::array<float, 3>& v2, bool occlude, bool twoSide, bool clip, float alpha)
{
    unsigned long long key = 0;
    if(twoSide)
        key |= 1;
    if(clip)
        key |= 2;

    CELL_BATCH_BLOCK* pBlock = nullptr;

    if(alpha >= 1.0f)
    {
        auto it = m_solid_tri_batches.find(key);
        if(it == m_solid_tri_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_solid_tri_batches[key] = pBlock;
        }
        else
            pBlock = it->second;
    }
    else
    {
        auto it = m_trans_tri_batches.find(key);
        if(it == m_trans_tri_batches.end())
        {
            pBlock = new CELL_BATCH_BLOCK();
            m_trans_tri_batches[key] = pBlock;
        }
        else
            pBlock = it->second;
    }

    pBlock->assure(8 * 3);
    pBlock->frame = STCore::inst->frame();
    float occ = occlude ? 0.0f : 1.0f;
    pBlock->pData[pBlock->size++] = (float)v0[0];
    pBlock->pData[pBlock->size++] = (float)v0[1];
    pBlock->pData[pBlock->size++] = (float)v0[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = color[0];
    pBlock->pData[pBlock->size++] = color[1];
    pBlock->pData[pBlock->size++] = color[2];
    pBlock->pData[pBlock->size++] = alpha;
    pBlock->pData[pBlock->size++] = (float)v1[0];
    pBlock->pData[pBlock->size++] = (float)v1[1];
    pBlock->pData[pBlock->size++] = (float)v1[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = color[0];
    pBlock->pData[pBlock->size++] = color[1];
    pBlock->pData[pBlock->size++] = color[2];
    pBlock->pData[pBlock->size++] = alpha;
    pBlock->pData[pBlock->size++] = (float)v2[0];
    pBlock->pData[pBlock->size++] = (float)v2[1];
    pBlock->pData[pBlock->size++] = (float)v2[2];
    pBlock->pData[pBlock->size++] = occ;
    pBlock->pData[pBlock->size++] = color[0];
    pBlock->pData[pBlock->size++] = color[1];
    pBlock->pData[pBlock->size++] = color[2];
    pBlock->pData[pBlock->size++] = alpha;
}

void Render::prepare_render_target(ViewCamera* camera)
{
    auto& vp = camera->viewport();
    if(m_render_target && (m_render_target->width() != vp.z || m_render_target->height() != vp.w))
    {
        m_render_target = nullptr;
    }

    if(!m_render_target)
    {
        m_render_target = std::make_unique<Engine::Texture2D>(STE::device(), (int)vp.z, (int)vp.w, GL_RGBA8);
        m_render_target->initTexture(false);
    }
}

void Render::present(ViewCamera* camera)
{
    // auto pViewMgr = STCore::inst->viewMgr();
    auto pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    auto pRenderer = STE::renderer();
    auto pDev = pRenderer->device();
    auto pEnv = pRenderer->env();

    pEnv->setMultiSample(1);

    if(m_render_target)
    {
        // 缓存模式
        pDev->setFrameBuffer(m_logic_fbo);
        pDev->setViewport(camera->viewport());

        pDev->enableDepthTest(false);
        pDev->enableDepthMask(false);
        pDev->enableCullFace(false);
        pDev->enableBlend(false);

        Program* pProg = pRenderer->shaderMgr()->getProgram(ShaderManager::POST, pEnv);
        pDev->setProgram(pProg);

        pProg->setAttribute("a_Vertex", pRenderer->fullScrVB(), 0, 4, 4, 0);
        pProg->setSampler2D("s_scene", m_render_target.get(), GL_NEAREST, GL_CLAMP_TO_EDGE);
        pProg->setUniform("u_uvOffScale", 0.0f, 0.0f, 1.0f, 1.0f);
        pDev->drawVertices(GL_TRIANGLES, 0, 3);
    }
    else
    {
        // 直接绘制
        pRenderer->present({ 0, 0, (int)camera->viewportWidth(), (int)camera->viewportHeight() }, camera->viewport(), m_logic_canvas);
    }
}

void Render::prepare_batch_blocks()
{
    long long frame = STCore::inst->frame();

    for(int i = 0; i < sizeof(m_cpts_inst_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_cpts_inst_batches[i])
            m_cpts_inst_batches[i]->prepareDraw();
    }

    for(auto& it: m_line_batches)
    {
        it.second->prepareDraw();
    }

    for(auto& it: m_blend_line_batches)
    {
        it.second->prepareDraw();
    }

    for(auto& it: m_solid_tri_batches)
    {
        it.second->prepareDraw();
    }

    for(auto& it: m_trans_tri_batches)
    {
        it.second->prepareDraw();
    }

    for(int i = 0; i < sizeof(m_scr_uni_line_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_scr_uni_line_batches[i])
            m_scr_uni_line_batches[i]->prepareDraw();
    }
}

void Render::reset_batch_blocks()
{
    long long frame = STCore::inst->frame();

    // m_render_meshes.clear();
    m_drawBatches.clear();

    for(int i = 0; i < sizeof(m_cpts_inst_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(!m_cpts_inst_batches[i])
            continue;

        auto pBlock = m_cpts_inst_batches[i];
        if(pBlock->size == 0 && frame - pBlock->frame > 30000)
        {// 长时间不用，释放
            delete pBlock;
            m_cpts_inst_batches[i] = nullptr;
        }
        else
            pBlock->reset();
    }

    for(auto it = m_line_batches.begin(); it != m_line_batches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {
            delete it->second;
            it = m_line_batches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(auto it = m_blend_line_batches.begin(); it != m_blend_line_batches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {
            delete it->second;
            it = m_blend_line_batches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(auto it = m_solid_tri_batches.begin(); it != m_solid_tri_batches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {
            delete it->second;
            it = m_solid_tri_batches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(auto it = m_trans_tri_batches.begin(); it != m_trans_tri_batches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {
            delete it->second;
            it = m_trans_tri_batches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(int i = 0; i < sizeof(m_scr_uni_line_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(!m_scr_uni_line_batches[i])
            continue;

        auto pBlock = m_scr_uni_line_batches[i];
        if(pBlock->size == 0 && frame - pBlock->frame > 30000)
        {// 长时间不用，释放
            delete pBlock;
            m_scr_uni_line_batches[i] = nullptr;
        }
        else
            pBlock->reset();
    }
}

void Render::render_ctrl_point(ViewCamera* camera)
{
    auto pPal = STCore::inst->docMgr()->cfgMgr()->palMgr()->activePallete();
    Device* pDev = STE::renderer()->device();
    auto pEnv = STE::renderer()->env();
    auto& vp = camera->viewport();
    Vec3f colors[] = { pPal->objHover(), pPal->objPick(), pPal->ctrlPointFirst(), (pPal->ctrlPointFirst() + pPal->ctrlPoint()) * 0.5f,
                       pPal->ctrlPoint() };

    Program* pProg = nullptr;
    for(int i = 0; i < sizeof(m_cpts_inst_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        CELL_BATCH_BLOCK* pBlock = m_cpts_inst_batches[i];
        if(!pBlock || pBlock->size == 0 || !pBlock->pVBuf)
            continue;

        if(!pProg)
        {
            pProg = STE::renderer()->shaderMgr()->getProgram(ShaderManager::CTRL_POINT, pEnv);
            if(!pProg)
                return;

            pDev->enableDepthTest(true);
            pDev->setDepthFunc(GL_LEQUAL);
            pDev->enableDepthMask(true);
            pDev->enableCullFace(false);
            pDev->enableBlend(false);
            pDev->setProgram(pProg);

            if(!m_cpts_buf)
            {
                float rb = (float)RenderObject::CTRL_POINT_SIZE * 0.5f;               // back
                float rf = (float)RenderObject::CTRL_POINT_SIZE * 0.5f - 1.0f;        // front
                float rbh = (float)RenderObject::CTRL_POINT_SIZE * 1.5f * 0.5f;       // highlight back
                float rfh = (float)RenderObject::CTRL_POINT_SIZE * 1.5f * 0.5f - 1.0f;// highlight front

                float offs[] = { -rb,  -rb,  1.0f, 0.0f, rb,  -rb,  1.0f, 0.0f, -rb,  rb,  1.0f, 0.0f,
                                 rb,   -rb,  1.0f, 0.0f, rb,  rb,   1.0f, 0.0f, -rb,  rb,  1.0f, 0.0f,

                                 -rf,  -rf,  0.0f, 1.0f, rf,  -rf,  0.0f, 1.0f, -rf,  rf,  0.0f, 1.0f,
                                 rf,   -rf,  0.0f, 1.0f, rf,  rf,   0.0f, 1.0f, -rf,  rf,  0.0f, 1.0f,

                                 -rbh, -rbh, 1.0f, 0.0f, rbh, -rbh, 1.0f, 0.0f, -rbh, rbh, 1.0f, 0.0f,
                                 rbh,  -rbh, 1.0f, 0.0f, rbh, rbh,  1.0f, 0.0f, -rbh, rbh, 1.0f, 0.0f,

                                 -rfh, -rfh, 0.0f, 1.0f, rfh, -rfh, 0.0f, 1.0f, -rfh, rfh, 0.0f, 1.0f,
                                 rfh,  -rfh, 0.0f, 1.0f, rfh, rfh,  0.0f, 1.0f, -rfh, rfh, 0.0f, 1.0f };

                m_cpts_buf = pDev->createVertexBuffer(4 * 24, GL_STATIC_DRAW, offs);
            }

            pProg->setAttribute("a_OffAndColorMask", m_cpts_buf, 0, 4, 4, 0);

            if(!m_cpts_common_ubo)
                m_cpts_common_ubo = pDev->createUniformBuffer(sizeof(Vec4f) * 5, GL_DYNAMIC_DRAW, nullptr);

            m_cpts_common_ubo->beginWrite();
            m_cpts_common_ubo->writeMat4(camera->dvpMatrix());
            m_cpts_common_ubo->writeVec4(Vec4f(vp.zw, 1.0f / vp.zw));
            m_cpts_common_ubo->endWrite();
            pProg->setUniformBuffer("ub_Common", m_cpts_common_ubo);
        }

        int clrIdx = i & 7;
        bool isSel = i & (1 << 3);
        bool isHover = i & (1 << 4);
        auto& color = colors[clrIdx];

        pProg->setAttribute("a_InstPos", pBlock->pVBuf, 0, 3, 3, 1);

        if(isSel)
        {
            pProg->setUniform("u_Color", 1.0f - color);
            pDev->drawInstVertices(GL_TRIANGLES, (int)pBlock->size / 3, 12, 6);
        }
        else
        {
            pProg->setUniform("u_Color", color);
            pDev->drawInstVertices(GL_TRIANGLES, (int)pBlock->size / 3, isHover ? 12 : 0, 12);
        }
    }
}

void Render::init_world_axis_res()
{
    {// view world axis mesh
        const float axisR = 1.0f;
        const float axisLen = 8.0f;
        const float arrowR = 1.5f;
        const float arrowLen = 2.0f;

        Vec3f colorX = Vec3f(0.8f, 0.8f, 0.8f);
        Vec3f colorY = Vec3f(0.8f, 0.8f, 0.8f);
        Vec3f colorZ = Vec3f(0.8f, 0.8f, 0.8f);

        float vbuf[] = { -axisR,
                         -axisR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,// X
                         axisLen,
                         -axisR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisLen,
                         -arrowR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisLen + arrowLen,
                         0.0f,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisLen,
                         arrowR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisLen,
                         axisR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisR,
                         axisR,
                         0.0f,
                         colorX.x,
                         colorX.y,
                         colorX.z,
                         axisR,
                         axisR,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,// Y
                         axisR,
                         axisLen,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         arrowR,
                         axisLen,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         0.0f,
                         axisLen + arrowLen,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         -arrowR,
                         axisLen,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         -axisR,
                         axisLen,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         -axisR,
                         -axisR,
                         0.0f,
                         colorY.x,
                         colorY.y,
                         colorY.z,
                         axisR,
                         0.0f,
                         0.0f,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,// Z
                         axisR,
                         0.0f,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         arrowR,
                         0.0f,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         0.0f,
                         axisLen + arrowLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         -arrowR,
                         0.0f,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         -axisR,
                         0.0f,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         -axisR,
                         0.0f,
                         0.0f,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         axisR,
                         0.0f,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         axisR,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         arrowR,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         0.0f,
                         axisLen + arrowLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         -arrowR,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         -axisR,
                         axisLen,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z,
                         0.0f,
                         -axisR,
                         0.0f,
                         colorZ.x,
                         colorZ.y,
                         colorZ.z };

        unsigned short ibuf[] = { 0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  7,  8,  8,  9,  9,  10, 10, 11, 11, 12, 12, 13,
                                  14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27 };

        AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->createAsset(Asset::ASSET_MESH, "$view$mesh$world_axis");
        MeshData* pMD = pAst->createMeshData();
        pMD->createAttributeData("Vertex/EmiColor", sizeof(vbuf) / sizeof(float), vbuf);
        pMD->createIndexData("Default", sizeof(ibuf) / sizeof(unsigned short), ibuf, GL_UNSIGNED_SHORT);
        pMD->createAttributeBuffer();
        pMD->createIndexBuffer();
        pAst->setReady(true);
    }
}

void Render::init_work_axis_res()
{
    {
        Vec3f colorX = { 0.65f, 0.2f, 0.2f };
        Vec3f colorY = { 0.2f, 0.65f, 0.2f };
        Vec3f colorZ = { 0.2f, 0.2f, 0.65f };
        float vbuf[] = { 100000.f, 0.f,      0.f,      colorX.x, colorX.y, colorX.z, -100000.f, 0.f,       0.f,       colorX.x, colorX.y, colorX.z,
                         0.f,      100000.f, 0.f,      colorY.x, colorY.y, colorY.z, 0.f,       -100000.f, 0.f,       colorY.x, colorY.y, colorY.z,
                         0.f,      0.f,      100000.f, colorZ.x, colorZ.y, colorZ.z, 0.f,       0.f,       -100000.f, colorZ.x, colorZ.y, colorZ.z };
        unsigned short ibuf[] = { 0, 1, 2, 3, 4, 5 };

        AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->createAsset(Asset::ASSET_MESH, "$view$mesh$work_axis");
        MeshData* pMD = pAst->createMeshData();
        pMD->createAttributeData("Vertex/EmiColor", sizeof(vbuf) / sizeof(float), vbuf);
        pMD->createIndexData("Default", sizeof(ibuf) / sizeof(unsigned short), ibuf, GL_UNSIGNED_SHORT);
        pMD->createAttributeBuffer();
        pMD->createIndexBuffer();
        pAst->setReady(true);
    }

    auto pMtrl = this->getMtrl(0, 0, false, true, true, false, false, true, 1, "atest:0;");
    auto pMD = this->viewWorkAxisMesh();

    m_world_axis_batch = new DrawBatchBlinnPhong();
    m_world_axis_batch->setMeshData(pMD);
    m_world_axis_batch->setMaterial(pMtrl);
    m_world_axis_batch->setIndexName("Default");
    m_world_axis_batch->setDrawRange(0, pMD->indexCount("Default"));
    m_world_axis_batch->setDrawMode(GL_LINES);
}

void Render::init_sky_box_res()
{
    {// sky box
        Vec3f dmTopColor = { 0.05f, 0.63f, 0.85f };
        Vec3f dmFrontColor = { 0.75f, 0.9f, 0.96f };
        Vec3f dmBackColor = { 0.75f, 0.9f, 0.96f };

        float vbuf[] = { 100.f,  100.f,  0.f,   dmFrontColor.x, dmFrontColor.y, dmFrontColor.z,
                         -100.f, 100.f,  0.f,   dmFrontColor.x, dmFrontColor.y, dmFrontColor.z,
                         100.f,  0.f,    100.f, dmTopColor.x,   dmTopColor.y,   dmTopColor.z,
                         -100.f, 0.f,    100.f, dmTopColor.x,   dmTopColor.y,   dmTopColor.z,
                         100.f,  -100.f, 0.f,   dmBackColor.x,  dmBackColor.y,  dmBackColor.z,
                         -100.f, -100.f, 0.f,   dmBackColor.x,  dmBackColor.y,  dmBackColor.z };
        unsigned short ibuf[] = { 0, 1, 2, 2, 1, 3, 2, 3, 5, 2, 5, 4 };

        AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->createAsset(Asset::ASSET_MESH, "$view$mesh$sky_box");
        MeshData* pMD = pAst->createMeshData();
        pMD->createAttributeData("Vertex/EmiColor", sizeof(vbuf) / sizeof(float), vbuf);
        pMD->createIndexData("Default", sizeof(ibuf) / sizeof(unsigned short), ibuf, GL_UNSIGNED_SHORT);
        pMD->createAttributeBuffer();
        pMD->createIndexBuffer();
        pAst->setReady(true);
    }

    auto pMtrl = this->getMtrl(0, 0, false, false, false, false, false, true, 1, "atest:1;");
    auto pMD = this->skyBoxMesh();

    m_sky_box_batch = new DrawBatchBlinnPhong();
    m_sky_box_batch->setMeshData(pMD);
    m_sky_box_batch->setMaterial(pMtrl);
    m_sky_box_batch->setIndexName("Default");
    m_sky_box_batch->setDrawRange(0, pMD->indexCount("Default"));
    m_sky_box_batch->setDrawMode(GL_TRIANGLES);
}

void Render::prepare_logic_canvas()
{
    if(m_logic_canvas && (m_logic_canvas->width() != m_logic_size.x || m_logic_canvas->height() != m_logic_size.y))
    {
        delete m_logic_canvas;
        m_logic_canvas = nullptr;
    }

    if(!m_logic_canvas)
    {
        m_logic_canvas = new Texture2D(STE::device(), m_logic_size.x, m_logic_size.y, GL_RGBA8);
        m_logic_canvas->initTexture(false);
        m_logic_fbo->setRenderTarget(m_logic_canvas, 0);
    }
}

void Render::pre_render(ViewCamera* camera)
{
    Device* pDev = STE::renderer()->device();
    pDev->makeCurrent();
    pDev->enableScissor(true);

    prepare_logic_canvas();

    // render background
    pDev->setFrameBuffer(m_logic_fbo);
    pDev->setViewport(0, 0, m_logic_size.x, m_logic_size.y);
    pDev->setClearColor(camera->backColor());
    pDev->setClearDepth(1.0f);
    pDev->clear(true, true);
}

void Render::post_render()
{
    Device* pDev = STE::renderer()->device();
    pDev->makeCurrent();
    pDev->enableScissor(true);

    pDev->setFrameBuffer(m_logic_fbo);
    pDev->setViewport(0, 0, m_logic_size.x, m_logic_size.y);

    present_physics();
    pDev->swapBuffers();
}

void Render::present_physics()
{
    if(!m_logic_fbo || !m_logic_canvas)
        return;

    auto view_manager = STCore::inst->viewMgr();

    Device* pDev = STE::renderer()->device();
    pDev->setFrameBuffer(nullptr);
    pDev->setViewport(0, 0, m_logic_size.x, m_logic_size.y);

    pDev->enableDepthTest(false);
    pDev->enableDepthMask(false);
    pDev->enableCullFace(false);

    Program* pProg = STE::renderer()->shaderMgr()->getProgram(ShaderManager::POST, nullptr);
    pDev->setProgram(pProg);

    pProg->setAttribute("a_Vertex", STE::renderer()->fullScrVB(), 0, 4, 4, 0);
    pProg->setSampler2D("s_scene", m_logic_canvas, (m_logic_size == view_manager->canvasPhysicSize()) ? GL_NEAREST : GL_LINEAR, GL_CLAMP_TO_EDGE);
    pProg->setUniform("u_uvOffScale", 0.0f, 0.0f, 1.0f, 1.0f);
    pDev->drawVertices(GL_TRIANGLES, 0, 3);
}

void Render::release()
{
    m_render_target.reset();
    m_light.reset();

    if(m_cpts_common_ubo)
    {
        STE::device()->deleteUniformBuffer(m_cpts_common_ubo);
        m_cpts_common_ubo = nullptr;
    }

    for(int i = 0; i < sizeof(m_cpts_inst_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_cpts_inst_batches[i])
            delete m_cpts_inst_batches[i];
    }
    memset(m_cpts_inst_batches, 0, sizeof(m_cpts_inst_batches));

    for(auto& it: m_line_batches)
    {
        delete it.second;
    }
    m_line_batches.clear();

    for(auto& it: m_blend_line_batches)
    {
        delete it.second;
    }
    m_blend_line_batches.clear();

    for(auto& it: m_solid_tri_batches)
    {
        delete it.second;
    }
    m_solid_tri_batches.clear();

    for(auto& it: m_trans_tri_batches)
    {
        delete it.second;
    }
    m_trans_tri_batches.clear();

    for(int i = 0; i < sizeof(m_scr_uni_line_batches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_scr_uni_line_batches[i])
            delete m_scr_uni_line_batches[i];
    }
    memset(m_scr_uni_line_batches, 0, sizeof(m_scr_uni_line_batches));

    if(m_world_axis_batch)
    {
        delete m_world_axis_batch;
        m_world_axis_batch = nullptr;
    }

    if(m_sky_box_batch)
    {
        delete m_sky_box_batch;
        m_sky_box_batch = nullptr;
    }
}

Engine::MeshData* Render::viewWorkAxisMesh()
{
    AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->getAsset("$view$mesh$work_axis");
    if(!pAst)
        return nullptr;

    return pAst->meshData();
}

Engine::MeshData* Render::viewWorldAxisMesh()
{
    AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->getAsset("$view$mesh$world_axis");
    if(!pAst)
        return nullptr;

    return pAst->meshData();
}

Engine::MeshData* Render::skyBoxMesh()
{
    AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->getAsset("$view$mesh$sky_box");
    if(!pAst)
        return nullptr;

    return pAst->meshData();
}

Engine::AssetTexture* Render::getLineMaskTex(const std::string& key)
{
    std::string fullKey = std::string("$core$tex_mask$") + key;
    AssetTexture* pMaskAst = (AssetTexture*)STE::assetMgr()->getAsset(fullKey);
    if(pMaskAst)
        return pMaskAst;

    std::vector<std::string> v;
    StrUtil::split(v, key, ',');

    int len = std::stoi(v[0]);
    int pitch = align(len, 4);
    unsigned char* pBuf = new unsigned char[pitch];
    memset(pBuf, 0, pitch * sizeof(unsigned char));

    int off = 0;
    for(int i = 1; i < v.size(); i += 2)
    {
        off += std::stoi(v[i]);
        int w = std::stoi(v[i + 1]);

        memset(pBuf + off, 255, w);

        off += w;
    }

    pMaskAst = (AssetTexture*)STE::assetMgr()->createAsset(Asset::ASSET_TEXTURE, fullKey);
    Texture2D* pTex = pMaskAst->createTexture2D();
    pTex->setWidth(len);
    pTex->setHeight(1);
    pTex->setFormat(GL_ALPHA8);
    pTex->setImageData(GL_ALPHA, GL_UNSIGNED_BYTE, pBuf, pitch);
    pTex->initTexture(false);
    pMaskAst->setReady(true);

    delete[] pBuf;

    return pMaskAst;
}

Engine::AssetTexture* Render::getLineColorStampTex(const std::string& key)
{
    std::string fullKey = std::string("$core$tex_stamp$") + key;
    AssetTexture* pAst = (AssetTexture*)STE::assetMgr()->getAsset(fullKey);
    if(pAst)
        return pAst;

    static std::map<std::string, Vec4i> colorMap = {
        { "black", { 0, 0, 0, 255 } },   { "white", { 255, 255, 255, 255 } }, { "red", { 255, 0, 0, 255 } },
        { "green", { 0, 255, 0, 255 } }, { "blue", { 0, 0, 255, 255 } },      { "yellow", { 255, 255, 0, 255 } },
    };

    std::vector<std::string> v;
    StrUtil::split(v, key, '|');

    int len = 0;
    for(int i = 0; i < v.size(); ++i)
    {
        size_t p = v[i].find(':');
        if(p == v[i].npos)
            continue;

        len += std::strtol(v[i].c_str() + p + 1, nullptr, 10);
    }

    unsigned char* pBuf = new unsigned char[len * 4];

    int n = 0;
    for(int i = 0; i < v.size(); ++i)
    {
        size_t p = v[i].find(':');
        if(p == v[i].npos)
            continue;

        std::string scolor = v[i].substr(0, p);
        int len = std::strtol(v[i].c_str() + p + 1, nullptr, 10);

        auto itColor = colorMap.find(scolor);
        if(itColor != colorMap.end())
        {
            auto& color = colorMap[scolor];
            for(int i = 0; i < len; ++i)
            {
                pBuf[n++] = color.x;
                pBuf[n++] = color.y;
                pBuf[n++] = color.z;
                pBuf[n++] = color.w;
            }
        }
        else
        {
            unsigned int color = (unsigned int)std::stoll(scolor);
            for(int i = 0; i < len; ++i)
            {
                pBuf[n++] = (color & 0xff0000) >> 16;
                pBuf[n++] = (color & 0xff00) >> 8;
                pBuf[n++] = (color & 0xff);
                pBuf[n++] = (color & 0xff000000) >> 24;
            }
        }
    }

    pAst = (AssetTexture*)STE::assetMgr()->createAsset(Asset::ASSET_TEXTURE, fullKey);
    Texture2D* pTex = pAst->createTexture2D();
    pTex->setWidth(len);
    pTex->setHeight(1);
    pTex->setFormat(GL_RGBA8);
    pTex->setImageData(GL_RGBA, GL_UNSIGNED_BYTE, pBuf, len * 4);
    pTex->initTexture(false);
    pAst->setReady(true);

    delete[] pBuf;

    return pAst;
}

Engine::MaterialBlinnPhong* Render::getLineMaskMtrl(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool cull,
                                                    uint8_t lineWidth, const std::string& mtrlKey, const std::string& maskKey)
{
    std::string maskTexKey = this->getLineMaskTex(maskKey)->key();
    std::string fullMtrlKey = mtrlKey + "atest:1; amapuv:0; amapfilter:linear; amapwrap:repeat;amap:" + maskTexKey + ";";
    return this->getMtrl(emi, diff, light, ztest, zwrite, blend, cull, false, lineWidth, fullMtrlKey);
}

Engine::MaterialBlinnPhong* Render::get_texture_material(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend,
                                                         bool cull, bool vcolor, uint8_t lineWidth, std::string& texture_name)
{
    AssetMaterial* pMtrlAst = (AssetMaterial*)STE::assetMgr()->createAsset(Asset::ASSET_MATERIAL);
    MaterialBlinnPhong* pMtrl = (MaterialBlinnPhong*)pMtrlAst->createMaterial(Material::MaterialType::MTRL_BLINN_PHONG);
    pMtrl->setEmissive(ARGB_RF(emi), ARGB_GF(emi), ARGB_BF(emi));
    pMtrl->setDiffuse(ARGB_RF(diff), ARGB_GF(diff), ARGB_BF(diff));
    pMtrl->setTexture(MaterialBlinnPhong::TextureChannel::MAP_DIFFUSE0, m_loaded_texture_asset[texture_name]->key());
    pMtrl->setSpecular(0.0f, 0.0f, 0.0f);
    pMtrl->setGlossiness(32.0f);
    pMtrl->setSpecularLevel(1.0f);
    pMtrl->setAlpha(1.0f);
    pMtrl->setDynamicLighting(light);
    pMtrl->enableMtrlColor(true);
    pMtrl->enableVertexColor(vcolor);
    pMtrl->setDepthTest(ztest);
    pMtrl->setDepthWrite(zwrite);
    pMtrl->setBlend(blend);
    pMtrl->setBlendEq(GL_FUNC_ADD);
    pMtrl->setBlendSrc(GL_SRC_ALPHA);
    pMtrl->setBlendDst(GL_ONE_MINUS_SRC_ALPHA);
    pMtrl->setTwoside(true);
    pMtrl->setLineWidth(lineWidth);
    pMtrl->setLineStipple(false);
    pMtrl->setTextureWrap(MaterialBlinnPhong::TextureChannel::MAP_DIFFUSE0, GL_REPEAT);
    return pMtrl;
}

void Render::load_texture_from_file(std::string texture_name, std::string img_path)
{

    AssetTexture* pTextureAst = (AssetTexture*)STE::assetMgr()->createAssetFromFile(
        Asset::ASSET_TEXTURE, img_path,
        [&, texture_name](Engine::Asset* asset) -> void {
            m_loaded_texture_asset.insert({ texture_name, asset });
            std::cout << texture_name << "load_success" << std::endl;
        },
        [&](Engine::Asset* asset, std::string error) -> void { std::cout << error << std::endl; });
}

Engine::MaterialBlinnPhong* Render::getMtrl(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool cull,
                                            bool vcolor, uint8_t lineWidth, const std::string& key)
{
    static const uint64_t LIGHT_BIT = uint64_t(1) << 63;
    static const uint64_t ZTEST_BIT = uint64_t(1) << 62;
    static const uint64_t ZWRITE_BIT = uint64_t(1) << 61;
    static const uint64_t BLEND_BIT = uint64_t(1) << 60;
    static const uint64_t CULL_BIT = uint64_t(1) << 59;
    static const uint64_t VCOLOR_BIT = uint64_t(1) << 58;

    uint64_t bitKey = (uint64_t(emi) << 32) | diff | (uint64_t(lineWidth) << 24) | (light ? LIGHT_BIT : 0ULL) | (ztest ? ZTEST_BIT : 0ULL) |
                      (zwrite ? ZWRITE_BIT : 0ULL) | (blend ? BLEND_BIT : 0ULL) | (cull ? CULL_BIT : 0ULL) | (vcolor ? VCOLOR_BIT : 0ULL);

    auto it = m_mtrlMap.find(bitKey);
    if(it != m_mtrlMap.end())
    {
        auto itm = it->second.find(key);
        if(itm != it->second.end())
            return itm->second;
    }

    std::string astKey = std::string("$core$mtrl$") + std::to_string(bitKey) + key;
    AssetMaterial* pMtrlAst = (AssetMaterial*)STE::assetMgr()->createAsset(Asset::ASSET_MATERIAL, astKey);
    MaterialBlinnPhong* pMtrl = (MaterialBlinnPhong*)pMtrlAst->createMaterial(Material::MTRL_BLINN_PHONG);
    m_mtrlMap[bitKey][key] = pMtrl;// cache mtrl

    pMtrl->setEmissive(ARGB_RF(emi), ARGB_GF(emi), ARGB_BF(emi));
    pMtrl->setDiffuse(ARGB_RF(diff), ARGB_GF(diff), ARGB_BF(diff));
    pMtrl->setSpecular(0.0f, 0.0f, 0.0f);
    pMtrl->setGlossiness(32.0f);
    pMtrl->setSpecularLevel(1.0f);
    pMtrl->setAlpha(1.0f);
    pMtrl->setDynamicLighting(light);
    pMtrl->enableMtrlColor(true);
    pMtrl->enableVertexColor(vcolor);
    pMtrl->setDepthTest(ztest);
    pMtrl->setDepthWrite(zwrite);
    pMtrl->setBlend(blend);
    pMtrl->setBlendEq(GL_FUNC_ADD);
    pMtrl->setBlendSrc(GL_SRC_ALPHA);
    pMtrl->setBlendDst(GL_ONE_MINUS_SRC_ALPHA);
    pMtrl->setTwoside(!cull);
    pMtrl->setLineWidth(lineWidth);
    pMtrl->setLineStipple(false);

    if(key.empty())
        return pMtrl;

    size_t off = 0;
    size_t pos;
    const char* cbuf = key.c_str();
    do
    {
        pos = key.find(';', off);
        std::string attrib;
        std::string val;

        size_t end = (pos == key.npos) ? key.length() : pos;
        for(int j = (int)off; j < end; ++j)
        {
            if(cbuf[j] == ':')
            {
                attrib = key.substr(off, j - off);
                val = key.substr(j + 1, end - j - 1);
                break;
            }
        }

        attrib = StrUtil::trim(attrib);

        if(attrib == "spec")
        {
            val = StrUtil::trim(val);
            Vec3f color;
            if(val.length() > 2 && val[0] == '0' && val[1] == 'x')
            {// uint color
                unsigned int v = std::stoul(val.c_str() + 2, nullptr, 16);
                color = ARGB_VEC3F(v);
            }
            else
            {
                size_t p0 = 0;
                int idx = 0;
                for(int j = 0; j < val.length(); ++j)
                {
                    if(val[j] == ',')
                    {
                        color.v[idx++] = std::stof(val.substr(p0, j - p0)) / 255.0f;
                        p0 = j + 1;
                    }
                }

                color.v[idx++] = std::stof(val.substr(p0)) / 255.0f;
            }

            pMtrl->setSpecular(color.x, color.y, color.z);
        }
        else if(attrib == "glo")
        {
            float v = std::stof(val);
            pMtrl->setGlossiness(v);
        }
        else if(attrib == "speclevel")
        {
            float v = std::stof(val);
            pMtrl->setSpecularLevel(v);
        }
        else if(attrib == "a")
        {
            float v = std::stof(val);
            pMtrl->setAlpha(v);
        }
        else if(attrib == "atest")
        {
            bool v = std::stoi(val) == 1;
            pMtrl->setAlphaTest(v);
        }
        else if(attrib == "atestval")
        {
            float v = std::stof(val);
            pMtrl->setAlphaTestValue(v);
        }
        else if(attrib == "linew")
        {
            float v = std::stof(val);
            pMtrl->setLineWidth(v);
        }
        else if(attrib == "psize")
        {
            float v = std::stof(val);
            pMtrl->setPointSize(v);
        }
        else if(attrib == "stppat")
        {
            if(SU::startWith(val, "0x"))
            {
                uint32_t v = std::stoul(val.c_str() + 2, nullptr, 16);
                pMtrl->setLineStipplePattern(v);
            }
        }
        else if(attrib == "stpfc")
        {
            int v = std::stoi(val);
            pMtrl->setLineStipple(v > 0);
            pMtrl->setLineStippleFactor(v);
        }
        else if(attrib == "linetype")
        {
            int v = std::stoi(val);
            if(v == Define::DRLINE_TYPE0)
            {
                pMtrl->setLineStipple(false);
            }
#if 0
				else if (v == Define::DRLINE_TYPE1) {
					pMtrl->setLineStipple(true);
					pMtrl->setLineStippleFactor(1);
					pMtrl->setLineStipplePattern(0xf0f0u);
				}
				else if (v == Define::DRLINE_TYPE2) {
					pMtrl->setLineStipple(true);
					pMtrl->setLineStippleFactor(1);
					pMtrl->setLineStipplePattern(0xfff0u);
				}
				else if (v == Define::DRLINE_TYPE3) {
					pMtrl->setLineStipple(true);
					pMtrl->setLineStippleFactor(2);
					pMtrl->setLineStipplePattern(0xff18u);
				}
				else if (v == Define::DRLINE_TYPE4) {
					pMtrl->setLineStipple(true);
					pMtrl->setLineStippleFactor(1);
					pMtrl->setLineStipplePattern(0xccccu);
				}
#else
            /// 解决线段缩放到很小时虚线不可见的问题
            else if(v == Define::DRLINE_TYPE1)
            {
                pMtrl->setLineStipple(true);
                pMtrl->setLineStippleFactor(1);
                pMtrl->setLineStipplePattern(0x0F0Fu);
            }
            else if(v == Define::DRLINE_TYPE2)
            {
                pMtrl->setLineStipple(true);
                pMtrl->setLineStippleFactor(1);
                pMtrl->setLineStipplePattern(0x0FFFu);
            }
            else if(v == Define::DRLINE_TYPE3)
            {
                pMtrl->setLineStipple(true);
                pMtrl->setLineStippleFactor(2);
                pMtrl->setLineStipplePattern(0x1FE3u);
            }
            else if(v == Define::DRLINE_TYPE4)
            {
                pMtrl->setLineStipple(true);
                pMtrl->setLineStippleFactor(1);
                pMtrl->setLineStipplePattern(0x3333u);
            }
#endif
            else
            {
                STASSERT(0);
            }
        }
        else if(attrib == "filltype")
        {
            int v = std::stoi(val);
            switch(v)
            {
            case Define::DRFILL_TYPE2: {// 砖块
                pMtrl->setTileOpacity(true);
                pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
                pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_bricks.png");
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
            }
            break;
            case Define::DRFILL_TYPE3: {// 棋盘
                pMtrl->setTileOpacity(true);
                pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
                pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_chess.png");
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
            }
            break;
            case Define::DRFILL_TYPE4: {// 斜线
                pMtrl->setTileOpacity(true);
                pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
                pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_lines.png");
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
            }
            break;
            case Define::DRFILL_TYPE0:// 无
            default: break;
            }
        }
        else if(attrib == "amap")
        {
            pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, val);
        }
        else if(attrib == "amapuv")
        {
            int v = std::stoi(val);
            pMtrl->setTextureUV(MaterialBlinnPhong::MAP_OPACITY, v);
        }
        else if(attrib == "amapfilter")
        {
            val = StrUtil::trim(val);

            if(val == "linear")
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_LINEAR);
            else
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
        }
        else if(attrib == "amapwrap")
        {
            val = StrUtil::trim(val);

            if(val == "clamp")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
            else if(val == "repeat")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_REPEAT);
            else if(val == "mirror")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_MIRRORED_REPEAT);
            else
                STASSERT(0);
        }
        else if(attrib == "emimap")
        {
            pMtrl->setTexture(MaterialBlinnPhong::MAP_EMISSIVE, val);
        }
        else if(attrib == "emimapuv")
        {
            int v = std::stoi(val);
            pMtrl->setTextureUV(MaterialBlinnPhong::MAP_EMISSIVE, v);
        }
        else if(attrib == "emimapfilter")
        {
            val = StrUtil::trim(val);

            if(val == "linear")
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_EMISSIVE, GL_LINEAR);
            else
                pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_EMISSIVE, GL_NEAREST);
        }
        else if(attrib == "emimapwrap")
        {
            val = StrUtil::trim(val);

            if(val == "clamp")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_CLAMP_TO_EDGE);
            else if(val == "repeat")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_REPEAT);
            else if(val == "mirror")
                pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_MIRRORED_REPEAT);
            else
                STASSERT(0);
        }
        else if(attrib == "vertcolor")
        {
            bool v = std::stoi(val) == 1;
            pMtrl->enableVertexColor(v);
        }
        else if(attrib == "mtrlcolor")
        {
            bool v = std::stoi(val) == 1;
            pMtrl->enableMtrlColor(v);
        }

        off = pos + 1;
    } while(pos != key.npos);

    return pMtrl;
}

}// namespace sketch
