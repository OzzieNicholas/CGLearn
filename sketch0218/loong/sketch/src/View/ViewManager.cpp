#include "sketch/View/ViewManager.h"

#include <ShellScalingApi.h>
#include <Windows.h>

#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetMaterial.h"
#include "Engine/Math/Vec2i.h"
#include "Engine/Render/Device.h"
#include "Engine/STEngine.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/WinUtil.h"

#include "Engine/Math/GeoUtil.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/Controller/Controller.h"
#include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"

#pragma comment(lib, "Shcore.lib")

using namespace Engine;

namespace sketch
{

#define NUM_CTRL_SLOTS 3
#define NUM_CTRL_LAYERS 3

class ViewManagerImpl : public ViewManager
{
public:
    ViewManagerImpl(void)
        : m_mode(Define::VIEWMGR_NONE), m_monitorSize(1, 1), m_canvasPhysicSize(1, 1), m_canvasLogicSize(1, 1), m_pScene(nullptr),
          m_pCompEditScene(nullptr), m_pLayout(nullptr), m_pBackCam(nullptr), m_pResMgr(nullptr)
    // m_pLogicCanvas(nullptr)
    {
        HDC hdc = GetDC(STCore::inst->mainHandle());
        m_dpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(STCore::inst->mainHandle(), hdc);

        /*m_pLogicFbo = new FrameBuffer();
        m_pLogicFbo->initFrameBuffer();*/
    }

    virtual ~ViewManagerImpl(void)
    {
        // if (m_pLogicCanvas) {
        //	delete m_pLogicCanvas;
        //	m_pLogicCanvas = nullptr;
        // }

        if(m_pBackCam)
        {
            delete m_pBackCam;
        }
    }

    virtual bool init() override
    {
        Log::log("", "sketch::ViewManager::init >>>>>>>>>>>>>>>>>>>>>>>  \n");

        m_mode = Define::VIEWMGR_3D;

        // 初始化当前所在显示器分辨率
        auto mi = WinUtil::screen(STCore::inst->mainHandle());
        m_monitorSize.x = mi.rcMonitor.right - mi.rcMonitor.left;
        m_monitorSize.y = mi.rcMonitor.bottom - mi.rcMonitor.top;

        m_pResMgr = new ResManager();
        m_pResMgr->init();

        m_pBackCam = new ViewCamera();
        m_pBackCam->setBackColor(0.8f, 0.8f, 0.8f, 1.0f);

        m_pLayout = TileViewLayout::instance();
        m_pLayout->onResize(STE::device()->windowWidth(), STE::device()->windowHeight());

        Log::log("", "sketch::ViewManager::init <<<<<<<<<<<<<<<<<<<<<<<  \n");

        return true;
    }

    virtual const Engine::Vec2i& monitorSize() const { return m_monitorSize; }

    virtual int monitorWidth() const { return m_monitorSize.x; }

    virtual int monitorHeight() const { return m_monitorSize.y; }

    virtual const Engine::Vec2i& canvasPhysicSize() const { return m_canvasPhysicSize; }

    virtual int canvasPhysicWidth() const { return m_canvasPhysicSize.x; }

    virtual int canvasPhysicHeight() const { return m_canvasPhysicSize.y; }

    virtual const Engine::Vec2i& canvasLogicSize() const { return m_canvasLogicSize; }

    virtual int canvasLogicWidth() const { return m_canvasLogicSize.x; }

    virtual int canvasLogicHeight() const { return m_canvasLogicSize.y; }

    virtual Engine::Vec2 physic2Logic(int x, int y) const
    {
        return { x * m_canvasLogicSize.x / (double)m_canvasPhysicSize.x, y * m_canvasLogicSize.y / (double)m_canvasPhysicSize.y };
    }

    virtual Engine::Vec2 physic2Logic(const Engine::Vec2i& v) const
    {
        return { v.x * m_canvasLogicSize.x / (double)m_canvasPhysicSize.x, v.y * m_canvasLogicSize.y / (double)m_canvasPhysicSize.y };
    }

    virtual double physic2LogicX(int x) const { return x * m_canvasLogicSize.x / (double)m_canvasPhysicSize.x; }

    virtual double physic2LogicY(int y) const { return y * m_canvasLogicSize.y / (double)m_canvasPhysicSize.y; }

    virtual Engine::Vec2i logic2Physic(const Engine::Vec2i& v) const { return v * m_canvasPhysicSize / m_canvasLogicSize; }

    // virtual Engine::FrameBuffer* logicFbo() const {
    //	return m_pLogicFbo;
    // }

    // virtual Engine::Texture2D* logicCanvas() const {
    //	return m_pLogicCanvas;
    // }

    virtual void onMonitorSizeChanged(int width, int height)
    {
        m_monitorSize.x = width;
        m_monitorSize.y = height;
    }

    virtual bool onDocPrepare(const Engine::Path& projPath) override
    {// 场景初始化/加载前调用
        auto pDoc = STCore::inst->docMgr()->document();

        m_pLayout->onDocPrepare(projPath);
        m_pResMgr->onDocPrepare(projPath);

        return true;
    }

    virtual bool onDocNew(const Engine::Path& projPath) override
    {// 初始化新场景
        auto pDoc = STCore::inst->docMgr()->document();
        auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();

        // edit mode
        this->setMode(Define::VIEWMGR_3D);

        // callbacks
        m_pLayout->onDocNew(projPath);
        m_pResMgr->onDocNew(projPath);

        // focus view
        auto pDefView = pDoc->views().begin()->second;
        m_pLayout->addCell(pDefView->renderView());
        m_pLayout->setCellFullScr(pDefView->id(), true);
        m_pLayout->setFocusedCell(pDefView->renderView());

        return true;
    }

    virtual void load(SketchSerializer& serializer, const Engine::Path& projPath) override
    {
        m_pResMgr->onDocLoad(projPath);

        char mode;
        serializer.read(mode, "mode");
        this->setMode(static_cast<Define::ViewManagerMode>(mode));

        serializer.begin_read_object("layout_tiled");
        m_pLayout->load_spec(serializer);
        serializer.end_read_object();

        m_pResMgr->onDocLoad(projPath);
    }

    virtual bool onDocLoad(const Engine::Path& projPath, Engine::DObject* pRootObj) override
    {
        m_pResMgr->onDocLoad(projPath);
        // 加载场景
        this->setMode((Define::ViewManagerMode)pRootObj->get("mode")._int());

        DObject* pLayoutProps = nullptr;
        if(pRootObj->has("layout_tiled"))
            pLayoutProps = pRootObj->get("layout_tiled")._dobj();
        else if(pRootObj->has("layout_tiled_view"))// 向下兼容，将来可删除
            pLayoutProps = pRootObj->get("layout_tiled_view")._dobj();

        // callbacks
        m_pLayout->onDocLoad(projPath, pLayoutProps);
        m_pResMgr->onDocLoad(projPath);

        return true;
    }

    virtual bool onDocLoaded(const Engine::Path& projPath, const std::wstring& progName, float progRange) override
    {
        // 场景初始化/加载完成后调用
        auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();

        m_pLayout->onDocLoaded(projPath);
        m_pResMgr->onDocLoaded(projPath);
        m_pScene->onDocLoaded(projPath, progName, progRange);

        return true;
    }

    virtual bool onDocWillClose(void) override
    {
        m_pLayout->onDocWillClose();
        m_pResMgr->onDocWillClose();
        return true;
    }

    virtual bool onDocClose(void) override
    {
        Document* pDoc = STCore::inst->docMgr()->document();

        m_pLayout->onDocClose();
        m_pResMgr->onDocClose();

        // release render scene
        if(m_pScene)
        {
            delete m_pScene;
            m_pScene = nullptr;
        }

        if(m_pCompEditScene)
        {
            delete m_pCompEditScene;
            m_pScene = nullptr;
        }

        m_pScene = new RenderScene(pDoc->scene());
        m_pCompEditScene = new RenderScene(pDoc->compEditScene());

        return true;
    }

    virtual bool onAppClose(void) override
    {
        m_pLayout->onAppClose();
        m_pResMgr->onAppClose();

        // release render scene
        if(m_pScene)
        {
            delete m_pScene;
            m_pScene = nullptr;
        }

        if(m_pCompEditScene)
        {
            delete m_pCompEditScene;
            m_pCompEditScene = nullptr;
        }

        return true;
    }

    virtual void save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange) override
    {
        pSavedObj->set("mode", Define::VIEWMGR_3D);

        m_pLayout->save(pSavedObj, progName, progRange * 0.75f);

        if(!progName.empty())
        {
            STCore::inst->moveUIProgress(progName, progRange * 0.25f);
            STCore::inst->updateUIProgress(progName);
        }
    }

    virtual void save(SketchSerializer& serializer, const std::wstring& progName, float progRange) override
    {
        serializer.write(static_cast<char>(Define::VIEWMGR_3D), "mode");

        serializer.begin_write_object("layout_tiled");
        m_pLayout->save_spec(serializer, progName, progRange * 0.75f);
        serializer.end_write_object();

        if(!progName.empty())
        {
            STCore::inst->moveUIProgress(progName, progRange * 0.25f);
            STCore::inst->updateUIProgress(progName);
        }
    }

    void setMode(Define::ViewManagerMode mode) override
    {
        if(m_mode == mode)
        {
            return;
        }

        m_mode = mode;
    }

    Define::ViewManagerMode mode() const override { return m_mode; }

    void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override
    {
        switch(method)
        {
        case Define::MSG_CUR_OBJS_UPDATE: {
            m_pLayout->dirtyCells();
        }
        break;
        case Define::MSG_VIEW_CREATE: {
            ViewDoc* pDoc = (ViewDoc*)params->get("view")._ptr();

            View* pView = new View();
            pView->init(pDoc);
            pDoc->setRenderView(pView);
            m_views[pView->id()] = pView;
        }
        break;
        case Define::MSG_VIEW_DELETE: {
            ViewDoc* pDoc = (ViewDoc*)params->get("view")._ptr();

            View* pView = pDoc->renderView();
            if(pView)
            {
                m_pLayout->removeCell(pView);
                m_views.erase(pView->id());
                delete pView;
            }
        }
        break;
        default: break;
        }
    }

    virtual void update(unsigned long long tm) override
    {
        if(m_pScene)
            m_pScene->update(tm);

        if(m_pCompEditScene)
            m_pCompEditScene->update(tm);

        if(m_pResMgr)
            m_pResMgr->update(tm);

        if(!m_pLayout || m_pLayout->isBusy())
            return;
    }

    virtual void onRender(long long lElapsedTm) override
    {
        // Device* pDev = STE::renderer()->device();
        // pDev->makeCurrent();
        // pDev->enableScissor(true);

        this->_updateLogicCanvasSize();
        if(auto view = this->getView(-1))
        {
            if(auto render = view->render())
            {
                render->resize(m_canvasLogicSize.x, m_canvasLogicSize.y);
            }
        }
        // this->_prepareLogicCanvas();
        if(m_pLayout)
        {
            m_pLayout->onResize(m_canvasLogicSize.x, m_canvasLogicSize.y);
        }

        m_pBackCam->setViewport(0.0f, 0.0f, (float)m_canvasLogicSize.x, (float)m_canvasLogicSize.y);
        m_pBackCam->setProjType(Camera::PROJ_ORTHO);
        m_pBackCam->setOrthoView(0.0f, (float)m_canvasLogicSize.x, 0.0f, (float)m_canvasLogicSize.y);

        ////render background
        // pDev->setFrameBuffer(m_pLogicFbo);
        // pDev->setViewport(0, 0, m_canvasLogicSize.x, m_canvasLogicSize.y);
        // pDev->setClearColor(m_pBackCam->backColor());
        // pDev->setClearDepth(1.0f);
        // pDev->clear(true, true);

        // render layouts
        if(m_pLayout)
            m_pLayout->onRender(lElapsedTm);

        // post rendering
        // pDev->setFrameBuffer(m_pLogicFbo);
        // pDev->setViewport(0, 0, m_canvasLogicSize.x, m_canvasLogicSize.y);

        // this->_presentPhysic();

        // present
        // pDev->swapBuffers();
    }

    void _updateLogicCanvasSize()
    {
        switch(STCore::inst->docMgr()->cfgMgr()->settings()->displayRatio())
        {
        case Define::DISP_RATIO_100:
            m_canvasLogicSize.x = m_canvasPhysicSize.x;
            m_canvasLogicSize.y = m_canvasPhysicSize.y;
            break;
        case Define::DISP_RATIO_125:
            m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 125;
            m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 125;
            break;
        case Define::DISP_RATIO_150:
            m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 150;
            m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 150;
            break;
        case Define::DISP_RATIO_175:
            m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 175;
            m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 175;
            break;
        case Define::DISP_RATIO_200:
            m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 200;
            m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 200;
            break;
        default: {// adaptive by system dpi scaling
            if(m_dpi >= 192)
            {
                m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 192;
                m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 192;
            }
            else if(m_dpi >= 168)
            {
                m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 175;
                m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 175;
            }
            else if(m_dpi >= 144)
            {
                m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 150;
                m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 150;
            }
            else if(m_dpi >= 120)
            {
                m_canvasLogicSize.x = m_canvasPhysicSize.x * 100 / 125;
                m_canvasLogicSize.y = m_canvasPhysicSize.y * 100 / 125;
            }
            else
            {
                m_canvasLogicSize.x = m_canvasPhysicSize.x;
                m_canvasLogicSize.y = m_canvasPhysicSize.y;
            }
        }
        }
    }

    virtual void onCanvasResize(int width, int height) override
    {
        HDC hdc = GetDC(STCore::inst->mainHandle());
        m_dpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(STCore::inst->mainHandle(), hdc);

        m_canvasPhysicSize.x = width;
        m_canvasPhysicSize.y = height;
        this->_updateLogicCanvasSize();

        if(m_pLayout)
            m_pLayout->onResize(m_canvasLogicSize.x, m_canvasLogicSize.y);
    }

    virtual void onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift) override {}

    virtual void onKeyUp(int key, bool bCtrl, bool bAlt, bool bShift) override {}

    virtual void onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();

        if(!pLayout->isLock())
        {
            pLayout->onMouseDown(button, x, y, bCtrl, bAlt, bShift);
            if(pLayout->isBusy())
                return;
        }
    }

    virtual void onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();
        if(pLayout == nullptr)
            return;

        if(pLayout->focusedView() && pLayout->focusedView()->isDirtyRT())
        {
            return;
        }

        if(!pLayout->isLock())
        {
            pLayout->onMouseMove(button, x, y, bCtrl, bAlt, bShift);
            if(pLayout->isBusy())
                return;
        }
    }

    virtual void onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();
        if(pLayout == nullptr)
            return;

        if(!pLayout->isLock())
        {
            bool busy = pLayout->isBusy();
            pLayout->onMouseUp(button, x, y, bCtrl, bAlt, bShift);
            if(busy)
                return;
        }
    }

    virtual void onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();
        if(pLayout == nullptr)
            return;

        if(!pLayout->isLock())
        {
            pLayout->onMouseClick(button, x, y, bCtrl, bAlt, bShift);
            if(pLayout->isBusy())
                return;
        }
    }

    virtual void onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();
        if(pLayout == nullptr)
            return;

        if(!pLayout->isLock())
        {
            pLayout->onMouseDoubleClick(button, x, y, bCtrl, bAlt, bShift);
            if(pLayout->isBusy())
                return;
        }
    }

    virtual void onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift) override
    {
        x = (int)this->physic2LogicX(x);
        y = (int)this->physic2LogicX(y);

        ViewLayout* pLayout = this->layout();
        if(pLayout == nullptr)
            return;

        if(!pLayout->isLock())
        {
            pLayout->onMouseWheel(button, x, y, delta, bCtrl, bAlt, bShift);
            if(pLayout->isBusy())
                return;
        }
    }

    virtual void onFocusedCellChanged() override {}

    virtual void onViewEvent(const std::string& event, const SketchParamMap& props) override
    {
        // do nothing
    }

    virtual ViewCamera* backCam() const override { return m_pBackCam; }

    virtual const std::unordered_map<long long, View*>& views() const override { return m_views; }

    virtual LayoutCell* getCell(long long id) const override
    {
        auto itv = m_views.find(id);
        if(itv != m_views.end())
            return itv->second;

        return nullptr;
    }

    virtual View* getView(long long id) const
    {
        auto it = m_views.find(id);
        if(it == m_views.end())
            return nullptr;

        return it->second;
    }

    virtual View* getView(const std::wstring& name) override
    {
        for(auto& it: m_views)
        {
            if(it.second->doc()->name() == name)
                return it.second;
        }

        return nullptr;
    }

    virtual TileViewLayout* layout() const override { return m_pLayout; }

    virtual RenderScene* scene() const override { return m_pScene; }

    virtual RenderScene* compEditScene() const override { return m_pCompEditScene; }

    virtual ResManager* resMgr() const override { return m_pResMgr; }

protected:
    Define::ViewManagerMode m_mode;

    int m_dpi;

    // monitor size
    Engine::Vec2i m_monitorSize;

    // view panel size
    Engine::Vec2i m_canvasPhysicSize;
    Engine::Vec2i m_canvasLogicSize;

    // render scene
    RenderScene* m_pScene;
    RenderScene* m_pCompEditScene;

    // views
    std::unordered_map<long long, View*> m_views;

    // symbols
    std::unordered_map<long long, Symbol*> m_symbols;

    // cell layout
    TileViewLayout* m_pLayout;

    // logic canvas
    // Engine::FrameBuffer* m_pLogicFbo;
    // Engine::Texture2D* m_pLogicCanvas;

    ViewCamera* m_pBackCam;

    // resource manager
    ResManager* m_pResMgr;
};

ViewManager* ViewManager::create() { return new ViewManagerImpl(); }

ViewManager::~ViewManager(void) {}
}// namespace sketch
