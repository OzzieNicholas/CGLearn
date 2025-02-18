#include "sketch/View/ViewLayout/LayoutCell.h"

#include <list>
#include <ppl.h>
#include <set>

#include "Engine/Asset/AssetData/FontData.h"
#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"
#include "Engine/Asset/AssetFont.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Render/Device.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"
#include "Engine/Render/Environment.h"
#include "Engine/Render/OGLRes/Program.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Scene/Light/DctLight.h"
#include "Engine/Util/Log.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/STCore.h"
#include "sketch/View//RenderView/View.h"
#include "sketch/View/ViewLayout/CELL_BATCH_BLOCK.h"
#include "sketch/View/ViewLayout/CELL_TEXT_BLOCK.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

// using namespace std;
using namespace Engine;

namespace sketch
{

const float LayoutCell::SYS_BUTTON_SIZE = 42.0f;
const float LayoutCell::SYS_BUTTON_ICON_SIZE = 18.0f;
const float LayoutCell::TITLE_BAR_HEIGHT = 30.0f;

LayoutCell::LayoutCell(Define::LayoutCellType type, uint32_t dirtyFlags)
    : m_cellType(type), m_renderId(-1), m_pCam(new ViewCamera()), m_fullScr(false), m_topMost(false), m_dirtyFlags(dirtyFlags)
{
    memset(m_circleBatches, 0, sizeof(m_circleBatches));
}

LayoutCell::~LayoutCell(void)
{
    this->release();

    if(m_pCam)
    {
        delete m_pCam;
        m_pCam = nullptr;
    }
}

void LayoutCell::release()
{
    for(auto& it: m_scrUniTriBatches)
    {
        delete it.second;
    }
    m_scrUniTriBatches.clear();

    for(auto& it: m_colorStampLineBatches)
    {
        delete it.second;
    }
    m_colorStampLineBatches.clear();

    for(int i = 0; i < sizeof(m_circleBatches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_circleBatches[i])
            delete m_circleBatches[i];
    }
    memset(m_circleBatches, 0, sizeof(m_circleBatches));

    for(auto& it: m_textBatches)
    {
        delete it.second;
    }
    m_textBatches.clear();
}

long long LayoutCell::cellId() const { return -1LL; }

const std::wstring LayoutCell::cellName() const { return StrUtil::NULL_WSTR; }

View* LayoutCell::_view() const { return dynamic_cast<View*>(const_cast<LayoutCell*>(this)); }

void LayoutCell::redraw(void) {}

void LayoutCell::save(SketchSerializer& serializer)
{
    _updateCellProps();
    serializer.begin_write_object("props");
    m_cellProps.save(serializer);
    serializer.end_write_object();

    // serializer.advance_write(NousConvert::convert(m_layoutRect), "rect");
    // serializer.write(m_fullScr, "full_scr");
    // serializer.write(m_topMost, "top_most");
}

void LayoutCell::load(SketchSerializer& serializer)
{
    serializer.begin_read_object("props");
    m_cellProps.load(serializer);
    serializer.end_read_object();
    modifyCellProps(m_cellProps);

    // nous::vec4 layout_rect;
    // serializer.advance_read(layout_rect, "rect");
    // m_layoutRect = Engine::Vec4i(layout_rect.x(), layout_rect.y(), layout_rect.z(), layout_rect.w());
    // serializer.read(m_fullScr, "full_scr");
    // serializer.read(m_topMost, "top_most");
}

void LayoutCell::_updateCellProps()
{
    m_cellProps.set_value("rect", NousConvert::convert(m_layoutRect));
    m_cellProps.set_value("full_scr", m_fullScr);
    m_cellProps.set_value("top_most", m_topMost);
}

void LayoutCell::modifyCellProps(const SketchParamMap& props)
{
    if(props.has_field("rect") && props.get_value_type("rect") == ESketchParamValueType::Vector4)
    {
        nous::vec4 nous_rect_vec4;
        props.get_value("rect", nous_rect_vec4);
        this->setLayoutRect(Engine::Vec4i(nous_rect_vec4.x(), nous_rect_vec4.y(), nous_rect_vec4.z(), nous_rect_vec4.w()));
    }
    if(props.has_field("full_scr") && props.get_value_type("full_scr") == ESketchParamValueType::Boolean)
    {
        bool is_full_screen;
        props.get_value("full_scr", is_full_screen);
        this->setFullScr(is_full_screen);
    }
    if(props.has_field("top_most") && props.get_value_type("top_most") == ESketchParamValueType::Boolean)
    {
        bool is_top_most;
        props.get_value("top_most", is_top_most);
        this->setFullScr(is_top_most);
    }

    this->_updateCamOrtho();
}

void LayoutCell::_updateCamOrtho()
{
    float width = m_pCam->viewportWidth();
    float height = m_pCam->viewportHeight();
    if(width > 0 && height > 0)
    {// viewport ratio changed, ortho view need to be updated
        float centerX = m_pCam->orthoLeft() + m_pCam->orthoWidth() * 0.5f;
        float centerY = m_pCam->orthoBottom() + m_pCam->orthoHeight() * 0.5f;
        float newWidth = m_pCam->orthoHeight() * width / height;
        m_pCam->setOrthoView(centerX - newWidth * 0.5f, centerX + newWidth * 0.5f, m_pCam->orthoBottom(), m_pCam->orthoTop());
        this->dirtyRT();
    }
}

void LayoutCell::_prepareBatchBlocks()
{
    for(auto& it: m_scrUniTriBatches)
    {
        it.second->prepareDraw();
    }

    for(auto& it: m_colorStampLineBatches)
    {
        it.second->prepareDraw();
    }

    for(int i = 0; i < sizeof(m_circleBatches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(m_circleBatches[i])
            m_circleBatches[i]->prepareDraw();
    }
}

void LayoutCell::_resetBatchBlocks()
{
    long long frame = STCore::inst->frame();

    // ObjectMemPool::setMultiThread(true);
    // concurrency::parallel_for(0, 2, [&](int idx) {
    //	if (idx == 0) {

    for(auto it = m_scrUniTriBatches.begin(); it != m_scrUniTriBatches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {
            // 长时间不用，释放
            delete it->second;
            it = m_scrUniTriBatches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(auto it = m_colorStampLineBatches.begin(); it != m_colorStampLineBatches.end();)
    {
        if(it->second->size == 0 && frame - it->second->frame > 30000)
        {// 长时间不用，释放
            delete it->second;
            it = m_colorStampLineBatches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }

    for(int i = 0; i < sizeof(m_circleBatches) / sizeof(CELL_BATCH_BLOCK*); ++i)
    {
        if(!m_circleBatches[i])
            continue;

        auto pBlock = m_circleBatches[i];
        if(pBlock->size == 0 && frame - pBlock->frame > 30000)
        {// 长时间不用，释放
            delete pBlock;
            m_circleBatches[i] = nullptr;
        }
        else
            pBlock->reset();
    }

    for(auto it = m_textBatches.begin(); it != m_textBatches.end();)
    {
        if(it->second->data.empty() && frame - it->second->frame > 30000)
        {
            // 长时间不用，释放
            delete it->second;
            it = m_textBatches.erase(it);
        }
        else
        {
            it->second->reset();
            ++it;
        }
    }
    //	}
    //});
    // ObjectMemPool::setMultiThread(false);
}

void LayoutCell::_renderShapeCircles(double width, CELL_BATCH_BLOCK* pBlock, bool clip)
{
    // Device* pDev = STE::renderer()->device();
    // Environment* pEnv = STE::renderer()->env();

    // pDev->enableDepthTest(true);
    // pDev->setDepthFunc(GL_LEQUAL);
    // pDev->enableDepthMask(true);
    // pDev->enableCullFace(false);
    // pDev->enableBlend(true);
    // pDev->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // std::unordered_map<std::string, std::string> defs = { {"DEF_CIRCLE", "1"} };
    // if (clip)
    //	defs["DEF_CLIP"] = "1";

    // Program* pProg = STE::renderer()->shaderMgr()->getProgram(ShaderManager::SHAPE, pEnv, defs);
    // if (!pProg)
    //	return;

    // pDev->setProgram(pProg);

    // if (pBlock->pVBuf) {
    //	pProg->setAttribute("a_VertexColor", pBlock->pVBuf, 0, 4, 8);
    //	pProg->setAttribute("a_OR", pBlock->pVBuf, 4, 4, 8);
    // }
    // else {
    //	pProg->setAttribute("a_VertexColor", pBlock->pData, 4, 8);
    //	pProg->setAttribute("a_OR", pBlock->pData + 4, 4, 8);
    // }

    // pProg->setUniform("u_MatW", pEnv->camera()->d2vMatrix());
    // pProg->setUniform("u_MatWVP", pEnv->camera()->dvpMatrix());
    // pProg->setUniform("u_VPWidth", Vec3f(pEnv->camera()->viewport().zw, (float)width));

    // if (clip && pEnv && pEnv->clips().size() > 0) {
    //	pProg->setUniformBuffer("ub_Clips", pEnv->clipsUbo());
    // }

    // pDev->drawVertices(GL_TRIANGLES, 0, (int)pBlock->size / 8);
}

void LayoutCell::setLayoutRect(const Engine::Vec4i& rect)
{
    TileViewLayout* pLayout = (TileViewLayout*)STCore::inst->viewMgr()->layout();

    m_layoutRect = rect;
    if(!m_fullScr)
    {
        auto vp = pLayout->canvas2Window(m_layoutRect);
        m_pCam->setViewport((float)vp.x, (float)vp.y, (float)vp.z, (float)vp.w);
        this->_updateCamOrtho();
        this->onCellResize();
        this->dirtyRT();
    }
}

void LayoutCell::setLayoutRect(int x, int y, int width, int height)
{
    TileViewLayout* pLayout = (TileViewLayout*)STCore::inst->viewMgr()->layout();

    m_layoutRect.set(x, y, width, height);
    if(!m_fullScr)
    {
        auto vp = pLayout->canvas2Window(m_layoutRect);
        m_pCam->setViewport((float)vp.x, (float)vp.y, (float)vp.z, (float)vp.w);
        this->_updateCamOrtho();
        this->onCellResize();
        this->dirtyRT();
    }
}

void LayoutCell::setLayoutPos(int x, int y)
{
    TileViewLayout* pLayout = (TileViewLayout*)STCore::inst->viewMgr()->layout();

    m_layoutRect.xy.set(x, y);
    if(!m_fullScr)
    {
        auto vp = pLayout->canvas2Window(m_layoutRect);
        m_pCam->setViewport((float)vp.x, (float)vp.y, (float)vp.z, (float)vp.w);
    }
}

void LayoutCell::setLayoutSize(int width, int height)
{
    TileViewLayout* pLayout = (TileViewLayout*)STCore::inst->viewMgr()->layout();

    m_layoutRect.zw.set(width, height);
    if(!m_fullScr)
    {
        auto vp = pLayout->canvas2Window(m_layoutRect);
        m_pCam->setViewport((float)vp.x, (float)vp.y, (float)vp.z, (float)vp.w);
        this->_updateCamOrtho();
        this->onCellResize();
        this->dirtyRT();
    }
}

void LayoutCell::resetCameraPose() {}

void LayoutCell::onCameraTransform() {}

void LayoutCell::onLayoutResize(int width, int height)
{
    auto pViewMgr = STCore::inst->viewMgr();

    if(m_fullScr)
    {
        m_pCam->setViewport(0.0f, 0.0f, (float)pViewMgr->canvasLogicWidth(), (float)pViewMgr->canvasLogicHeight());
        this->_updateCamOrtho();
        this->dirtyRT();
    }
}

void LayoutCell::onCellResize() {}

void LayoutCell::onCellAdded(int renderId)
{
    m_renderId = renderId;

    this->dirtyRT();
}

bool LayoutCell::onCellRemoving() { return false; }

void LayoutCell::onCellRemoved() { m_renderId = -1; }

bool LayoutCell::isWindowPosIn(int x, int y) const
{
    if(m_pCam == nullptr)
        return false;

    auto& vp = m_pCam->viewport();

    return x > vp.x && x < vp.x + vp.z - 1.0f && y > vp.y && y < vp.y + vp.w - 1.0f;
}

bool LayoutCell::isCover(LayoutCell* pCell) const
{
    if(m_pCam == nullptr || pCell->m_pCam == nullptr)
        return false;

    auto& vp0 = m_pCam->viewport();
    auto& vp1 = pCell->m_pCam->viewport();

    return (vp1.x >= vp0.x && vp1.x <= vp0.x + vp0.z - 1.0f && vp1.y >= vp0.y && vp1.y <= vp0.y + vp0.w - 1.0f && (vp1.x + vp1.z - 1.0f) >= vp0.x &&
            (vp1.x + vp1.z - 1.0f) <= vp0.x + vp0.z - 1.0f && (vp1.y + vp1.w - 1.0f) >= vp0.y && (vp1.y + vp1.w - 1.0f) <= vp0.y + vp0.w - 1.0f);
}

bool LayoutCell::isIntersect(LayoutCell* pCell) const
{
    if(m_pCam == nullptr || pCell->m_pCam == nullptr)
        return false;

    auto& vp0 = m_pCam->viewport();
    auto& vp1 = pCell->m_pCam->viewport();

    return STMIN(vp0.x + vp0.z, vp1.x + vp1.z) - STMAX(vp0.x, vp1.x) > 1.0f && STMIN(vp0.y + vp0.w, vp1.y + vp1.w) - STMAX(vp0.y, vp1.y) > 1.0f;
}

void LayoutCell::setFullScr(bool b)
{
    if(m_fullScr == b)
        return;

    auto pViewMgr = STCore::inst->viewMgr();

    m_fullScr = b;

    if(m_fullScr)
        m_pCam->setViewport(0.0f, 0.0f, (float)pViewMgr->canvasLogicWidth(), (float)pViewMgr->canvasLogicHeight());
    else
    {
        auto vp = STCore::inst->viewMgr()->layout()->canvas2Window(m_layoutRect);
        m_pCam->setViewport((float)vp.x, (float)vp.y, (float)vp.z, (float)vp.w);
    }

    this->_updateCamOrtho();
    this->onCellResize();
}

void LayoutCell::setTopMost(bool b) { m_topMost = b; }

bool LayoutCell::isMouseOnTitleBar(int x, int y) const
{
    if(m_fullScr)
        return false;

    ViewCamera* pCam = this->camera();

    auto& vp = pCam->viewport();
    float vpright = vp.x + vp.z;
    float vptop = vp.y + vp.w;

    return (x >= vp.x && x <= vp.x + vp.z && y >= vptop - TITLE_BAR_HEIGHT && y <= vptop);
}

bool LayoutCell::isMouseOnControlBtn(int x, int y, int idx /*right to left*/) const
{
    ViewCamera* pCam = this->camera();

    auto& vp = pCam->viewport();
    float vpright = vp.x + vp.z;
    float vptop = vp.y + vp.w;

    return (x >= vpright - 1.0f - SYS_BUTTON_SIZE * float(idx + 1) && x <= vpright - 1.0f - SYS_BUTTON_SIZE * float(idx) &&
            y >= vptop - TITLE_BAR_HEIGHT && y <= vptop);
}

void LayoutCell::_renderPost(const std::wstring& title, const Engine::Vec4f& titleColor)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewLayout* pLayout = pViewMgr->layout();
    Engine::Renderer* pRenderer = STE::renderer();
    Environment* pEnv = pRenderer->env();
    Device* pDev = STE::renderer()->device();
    ViewCamera* pCam = this->camera();
    Pallete* pPal = STCore::inst->docMgr()->cfgMgr()->palMgr()->activePallete();

    bool bFocus = pLayout->isFocusedCell(this->cellId());

    // draw view post-UI
    // pDev->setFrameBuffer(pViewMgr->logicFbo());
    // pDev->setViewport(pCam->viewport());

    Mat4f matUI = Mat4f::ortho(0.0f, pCam->viewportWidth(), pCam->viewportHeight(), 0.0f, -1.0f, 1.0f);

    // title bar
    if(!m_fullScr)
        STE::renderer()->renderRect(matUI, 0.0f, 0.0f, pCam->viewportWidth(), TITLE_BAR_HEIGHT,
                                    bFocus ? Vec4f(0.25f, 0.2f, 0.2f, 0.9f) : Vec4f(0.6f, 0.5f, 0.5f, 0.6f));

    // title
    Mat4f matTitle = Mat4f::ortho(0.0f, pCam->viewportWidth(), 0.0f, pCam->viewportHeight(), -1.0f, 1.0f);
    STE::renderer()->renderText(matTitle, { 10.0f, pCam->viewportHeight() - (TITLE_BAR_HEIGHT - 20) * 0.5f, 0.0f }, title,
                                "fonts\\shs.ttc?fontSize=20", 20, titleColor, { 0.0f, 1.0f }, pCam->viewport().zw, false);

    // close button
    if(this->isMouseOnControlBtn(STCore::inst->mouseLogicX(), STCore::inst->mouseLogicY(), 0))
    {
        // highlight background
        STE::renderer()->renderRect(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE, 0.0f, SYS_BUTTON_SIZE, TITLE_BAR_HEIGHT,
                                    Vec4f(0.91f, 0.066f, 0.137f, 1.0f));

        STE::renderer()->renderTex(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE + (SYS_BUTTON_SIZE - SYS_BUTTON_ICON_SIZE) * 0.5f,
                                   (TITLE_BAR_HEIGHT - SYS_BUTTON_ICON_SIZE) * 0.5f, SYS_BUTTON_ICON_SIZE, SYS_BUTTON_ICON_SIZE,
                                   "Icon\\view_close.png", GL_LINEAR, Vec4f::WHITE);
    }
    else
    {
        STE::renderer()->renderTex(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE + (SYS_BUTTON_SIZE - SYS_BUTTON_ICON_SIZE) * 0.5f,
                                   (TITLE_BAR_HEIGHT - SYS_BUTTON_ICON_SIZE) * 0.5f, SYS_BUTTON_ICON_SIZE, SYS_BUTTON_ICON_SIZE,
                                   "Icon\\view_close.png", GL_LINEAR, titleColor);
    }

    // max button
    if(this->isMouseOnControlBtn(STCore::inst->mouseLogicX(), STCore::inst->mouseLogicY(), 1))
    {
        // highlight background
        STE::renderer()->renderRect(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE * 2.0f, 0.0f, SYS_BUTTON_SIZE, TITLE_BAR_HEIGHT,
                                    Vec4f(0.0f, 0.4f, 0.7f, 1.0f));

        STE::renderer()->renderTex(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE * 2.0f + (SYS_BUTTON_SIZE - SYS_BUTTON_ICON_SIZE) * 0.5f,
                                   (TITLE_BAR_HEIGHT - SYS_BUTTON_ICON_SIZE) * 0.5f, SYS_BUTTON_ICON_SIZE, SYS_BUTTON_ICON_SIZE,
                                   m_fullScr ? "Icon\\view_min.png" : "Icon\\view_max.png", GL_LINEAR, Vec4f::WHITE);
    }
    else
    {
        STE::renderer()->renderTex(matUI, pCam->viewportWidth() - 1.0f - SYS_BUTTON_SIZE * 2.0f + (SYS_BUTTON_SIZE - SYS_BUTTON_ICON_SIZE) * 0.5f,
                                   (TITLE_BAR_HEIGHT - SYS_BUTTON_ICON_SIZE) * 0.5f, SYS_BUTTON_ICON_SIZE, SYS_BUTTON_ICON_SIZE,
                                   m_fullScr ? "Icon\\view_min.png" : "Icon\\view_max.png", GL_LINEAR, titleColor);
    }

    // border
    float offx = 1.0f / pCam->viewportWidth();
    float offy = 1.0f / pCam->viewportHeight();
    if(bFocus)
    {
        STE::renderer()->renderRectLine2D(Mat4f::IDENTITY, -1.0f + offx, -1.0f + offy, 1.0f - offx, 1.0f - offy,
                                          Engine::Vec4f(pPal->cellPickBorder(), 1.0f), m_fullScr ? 2.0f : 4.0f);
    }
    else
    {
        STE::renderer()->renderRectLine2D(Mat4f::IDENTITY, -1.0f + offx, -1.0f + offy, 1.0f - offx, 1.0f - offy,
                                          Engine::Vec4f(pPal->cellBorder(), 1.0f), 1.0f);
    }
}
}// namespace sketch