#include "sketch/View/RenderView/View.h"

#include <list>
#include <ppl.h>
#include <stack>

#include "Engine/Asset/AssetData/FontData.h"
#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Render/Device.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"
#include "Engine/Render/DrawBatch/DrawBatchFlatText.h"
#include "Engine/Render/Environment.h"
#include "Engine/Render/OGLRes/Program.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Scene/Light/DctLight.h"
#include "Engine/Scene/TextTagArray.h"
#include "Engine/Util/Log.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDisplaySetting/ViewDisplaySetting.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/Controller/Controller.h"
#include "sketch/View/ControllerManager.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/NurbsDataUtil.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"
#include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/RenderRes/SmoothGroup.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/Render.h"
#include "sketch/View/Select/ElementSelector.h"
#include "sketch/View/Select/InferenceHelper.h"
#include "sketch/View/ViewLayout/CELL_BATCH_BLOCK.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Command/Revertable/RvtBlock.h"
// using namespace std;
using namespace Engine;

namespace sketch
{

View::View()
    : LayoutCell(Define::CELL_VIEW, DIRTY_BIT_RT | DIRTY_BIT_CAM_POSE | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_VIEW_BBOX),
      m_pDoc(nullptr), m_pScene(nullptr), m_pAxisTipCam(nullptr), m_pAxisTip(nullptr), m_pLight(nullptr), m_group_area(nullptr),
      m_redrawFrame(STCore::inst->frame()), m_camFrame(-1), m_render(STCore::inst->render())
{
    m_recorder.setFrameTimeCount(10);
}

View::~View(void) { this->release(); }

bool View::init(ViewDoc* pViewDoc)
{
    this->release();

    m_pDoc = pViewDoc;
    m_pDoc->addListener(this);

    m_selector = new ElementSelector();
    m_inferenceHelper = new InferenceHelper(m_pCam);

    m_pScene = m_pDoc->scene()->renderScene();
    m_pScene->setObjChangeCallback(std::bind(&View::sceneChangedCallback, this, std::placeholders::_1, std::placeholders::_2));

    m_pCam->setProjType(Camera::PROJ_PERSPECTIVE);
    m_pCam->setBackColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_pCam->setFov(30.0f);
    m_pCam->setZNear(0.01f);
    m_pCam->setZFar(1000.0f);
    m_pCam->setDocToViewMatrix(STCore::inst->docMgr()->document()->d2vMatrix());

    m_pLight = new DctLight();
    m_pLight->setLightDir(-1.0f, 1.0f, -1.0f);
    m_pLight->setColor(1.0f, 1.0f, 1.0f);
    m_pLight->setIntensity(1.0f);

    m_pAxisTipCam = new ViewCamera();
    m_pAxisTipCam->setProjType(Camera::PROJ_PERSPECTIVE);
    m_pAxisTipCam->setBackColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_pAxisTipCam->setFov(30.0f);
    m_pAxisTipCam->setZNear(0.1f);
    m_pAxisTipCam->setZFar(50.0f);
    m_pAxisTipCam->setPos(0.0f, 0.0f, 6.0f);

    m_camFrame = STCore::inst->frame();
    m_redrawFrame = STCore::inst->frame();

    m_dirtyFlags = DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_RT | DIRTY_BIT_CAM_POSE | DIRTY_BIT_CAM_DEPTH;

    auto height = STCore::inst->viewMgr()->canvasLogicHeight();
    auto width = STCore::inst->viewMgr()->canvasLogicWidth();
    if(m_render)
    {
        m_render->init();
        m_render->resize(width, height);
    }

    return true;
}

void View::release()
{
    if(m_render)
    {
        m_render->clear();
    }

    if(m_pLight)
    {
        delete m_pLight;
        m_pLight = nullptr;
    }

    if(m_pAxisTipCam)
    {
        delete m_pAxisTipCam;
        m_pAxisTipCam = nullptr;
    }

    if(m_pAxisTip)
    {
        delete m_pAxisTip;
        m_pAxisTip = nullptr;
    }

    m_objects.clear();
    m_visObjects.clear();

    m_camFrame = STCore::inst->frame();

    m_dirtyFlags = DIRTY_BIT_RT | DIRTY_BIT_CAM_POSE | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_VIEW_BBOX;

    m_fullScr = false;

    __super::release();
}

void View::redraw(void)
{
    if(!m_pDoc)
        return;

    m_redrawFrame = STCore::inst->frame();

    m_hideObjects.clear();

    m_dirtyFlags |= (DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_RT | DIRTY_BIT_CAM_DEPTH);
}

void View::_updateCellProps()
{
    __super::_updateCellProps();
    m_cellProps.set_value("cam_mat", NousConvert::convert(m_pCam->matrix().mat43()));
    m_cellProps.set_value("ortho_view", NousConvert::convert(m_pCam->orthoView().vec4()));
}

void View::modifyCellProps(const SketchParamMap& props)
{
    if(props.has_field("rect") && props.get_value_type("rect") == ESketchParamValueType::Vector4)
    {
        nous::vec4 rect_vec4;
        props.get_value("rect", rect_vec4);
        Engine::Vec4i rect(rect_vec4.x(), rect_vec4.y(), rect_vec4.z(), rect_vec4.w());
        rect.z = STMIN(rect.z, STE::device()->windowWidth());
        rect.w = STMIN(rect.w, STE::device()->windowHeight());
        this->setLayoutRect(rect);
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
        this->setTopMost(is_top_most);
    }

    if(this->layoutRect().z > 0.0)
    {
        this->updateCameraPose();

        if(props.has_field("cam_mat") && props.get_value_type("cam_mat") == ESketchParamValueType::Matrix4x4)
        {
            nous::mat4 cam_mat_4x4;
            props.get_value("cam_mat", cam_mat_4x4);
            m_pCam->setMatrix(NousConvert::convert(cam_mat_4x4));
        }

        if(props.has_field("ortho_view") && props.get_value_type("ortho_view") == ESketchParamValueType::Vector4)
        {
            nous::vec4 ortho_view_4;
            props.get_value("ortho_view", ortho_view_4);
            m_pCam->setOrthoView(NousConvert::convert(ortho_view_4));
        }

        this->onCameraTransform();
    }
    else
    {// uninitialized view
        this->resetCameraPose();
    }

    this->_updateCamOrtho();
}

void View::onCellAdded(int renderId)
{
    __super::onCellAdded(renderId);

    m_dirtyFlags |= (DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_RT);
}

void View::onCellRemoved()
{
    STCore::inst->viewMgr()->resMgr()->diposeInstBatches(m_renderId);

    __super::onCellRemoved();
}

void View::onLayoutResize(int width, int height)
{
    LayoutCell::onLayoutResize(width, height);
    if(m_render)
    {
        m_render->resize(width, height);
    }
}

long long View::cellId() const { return this->id(); }

const std::wstring View::cellName() const
{
    if(m_pDoc)
        return m_pDoc->name();

    return StrUtil::NULL_WSTR;
}

long long View::id() const
{
    if(m_pDoc)
        return m_pDoc->id();

    return -1LL;
}

void View::updateRedrawFrame() { m_redrawFrame = STCore::inst->frame(); }

void View::_resetBatchBlocks() { __super::_resetBatchBlocks(); }

void View::_prepareBatchBlocks() { __super::_prepareBatchBlocks(); }

void View::sceneChangedCallback(Define::Message method, RenderObject* pObj)
{
    if(m_group_area == pObj->doc())
    {
        m_group_area = pObj->doc()->parent();
    }
    m_selector->render_obj_changed(method, pObj);
    m_inferenceHelper->render_obj_changed(method, pObj);
}

void View::_refreshRT()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    ControllerManager* pCtrlMgr = STCore::inst->ctrlMgr();
    ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewLayout* pLayout = pViewMgr->layout();
    PalleteManager* pPalMgr = pCfgMgr->palMgr();
    Device* pDev = STE::renderer()->device();
    ViewCamera* pCam = this->camera();
    auto pDispSets = m_pDoc->dispSetting();
    ObjectFilter* pFilter = m_pDoc->dispFilter();
    Pallete* pPal = pPalMgr->activePallete();
    Settings* pSets = pCfgMgr->settings();

    if((m_dirtyFlags & DIRTY_BIT_RT) == 0)
    {
        if(m_render)
        {
            m_render->render(pCam, pDispSets->partMode(), false);
        }
        return;
    }
    m_dirtyFlags &= ~DIRTY_BIT_RT;

    // set up camera
    pCam->setDocToViewMatrix(pDoc->d2vMatrix());
    if(m_dirtyFlags & DIRTY_BIT_CAM_POSE)
    {
        this->_resetCameraPose();
        m_dirtyFlags &= ~DIRTY_BIT_CAM_POSE;
    }
    if(m_dirtyFlags & DIRTY_BIT_CAM_DEPTH)
    {
        this->_updateCamDepth();
        m_dirtyFlags &= ~DIRTY_BIT_CAM_DEPTH;
    }
    pCam->setBackColor(0.8f, 0.8f, 0.8f, 1.0f);

    // prepare mesh
    auto& visObjs = this->visObjects();
    // std::vector<long long> empty_groups;
    for(auto pObj: visObjs)
    {
        // if(pObj->type() == Define ::OBJ_D5_GROUP && pObj->doc()->_group()->children().size() == 0)
        //{
        //     empty_groups.push_back(pObj->id());
        // }
        // else
        //{
        pObj->prepare_render(this);
        //}
    }

    // preapre controller
    pCtrlMgr->prepare_render(this);

    // render
    if(m_render)
    {
        m_render->render(pCam, pDispSets->partMode(), true);
    }
}

void View::onRenderCell(long long lElapsedTime) { this->_refreshRT(); }

void View::_initCamPivotVBuf(float* vbuf, float r, int segs)
{
    int vnum = segs + 8;
    float clr = STPACK_RGB(Vec3f(0.6f, 0.1f, 0.6f));

    for(int i = 0; i < vnum; ++i)
    {
        // pos/occlude
        vbuf[i * 7 + 0] = 0.0f;
        vbuf[i * 7 + 1] = 0.0f;
        vbuf[i * 7 + 2] = 0.0f;
        vbuf[i * 7 + 3] = 1.0f;

        // color
        vbuf[i * 7 + 6] = clr;
    }

    // bars
    vbuf[0 * 7 + 4] = -r * 0.6f;
    vbuf[0 * 7 + 5] = -r * 0.6f;
    vbuf[1 * 7 + 4] = -r * 0.9f;
    vbuf[1 * 7 + 5] = -r * 0.9f;
    vbuf[2 * 7 + 4] = r * 0.6f;
    vbuf[2 * 7 + 5] = -r * 0.6f;
    vbuf[3 * 7 + 4] = r * 0.9f;
    vbuf[3 * 7 + 5] = -r * 0.9f;
    vbuf[4 * 7 + 4] = r * 0.6f;
    vbuf[4 * 7 + 5] = r * 0.6f;
    vbuf[5 * 7 + 4] = r * 0.9f;
    vbuf[5 * 7 + 5] = r * 0.9f;
    vbuf[6 * 7 + 4] = -r * 0.6f;
    vbuf[6 * 7 + 5] = r * 0.6f;
    vbuf[7 * 7 + 4] = -r * 0.9f;
    vbuf[7 * 7 + 5] = r * 0.9f;

    // circle
    for(int i = 0; i < segs; i++)
    {
        vbuf[(8 + i) * 7 + 4] = cosf(i * 2.0f * FLOAT_PI / segs) * r;
        vbuf[(8 + i) * 7 + 5] = sinf(i * 2.0f * FLOAT_PI / segs) * r;
    }
}

void View::onViewTypeChanged()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();
    auto& viewBBox = this->bbox();
    auto& viewPl = m_pDoc->viewPlane();
    auto visFrustum = m_pCam->clipPlanes();
    Plane pls[6];
    for(int i = 0; i < 6; ++i)
    {
        pls[i] = Plane(pDoc->v2d(visFrustum[i].o), visFrustum[i].n);
    }

    // find all visible objects
    AABB visBBox;
    for(auto it: this->visObjects())
    {
        auto pObj = it->doc();
        if(!pObj->isValid())
            continue;

        auto& bbox = pObj->bbox();
        auto ptsW = bbox.pointsW();
        for(int i = 0; i < 6; ++i)
        {
            bool bOut = true;
            for(auto& pt: ptsW)
            {
                if(pls[i].sdist(pt) >= 0.0)
                {
                    bOut = false;
                    break;
                }
            }

            if(bOut)
                goto LINE_NO_OBJ_OUT;
        }

        visBBox.add(bbox);

LINE_NO_OBJ_OUT:
        continue;
    }

    // ��ǰ��������û�ж���ֱ����������ӽǼ���
    if(!visBBox.isValid())
    {
        if(m_pDoc->type() == Define::VIEW_3D)
            m_pCam->setByEuler((float)angl2Rad(m_pDoc->rotV()), (float)angl2Rad(m_pDoc->rotH()), 0.0);

        this->_resetCameraPose();
        return;
    }

    switch(m_pDoc->type())
    {
    case Define::VIEW_2D: {
        auto refPlane = pOpts->snapRefPlane(this);
        auto& plX = refPlane.axisX();
        auto& plY = refPlane.axisY();
        auto& plZ = refPlane.normal();

        Mat43 matCamL = refPlane.matrix().inverse() * Mat43(plX, plY, plZ, visBBox.center());

        if(m_pCam->projType() == Camera::PROJ_PERSPECTIVE)
        {// ͸��ͶӰ�£�ȷ���ڿɼ�����֮��
            double zMax = STMAX(matCamL.pos.z, 0.0);
            for(auto& pt: visBBox.points())
                zMax = STMAX(zMax, refPlane.sdist(pt));

            matCamL.pos.z = zMax + 500.0;
        }
        else// ����ͶӰ�£�������������壬��Ҫȷ�������������ͼ��Χ��֮�⼴��
            matCamL.pos.z = STMAX(500.0, pDoc->v2d(viewBBox.getMax().len(viewBBox.getMin())));

        m_pCam->setMatrix(pDoc->d2v(refPlane.matrix() * matCamL));
    }
    break;
    case Define::VIEW_3D: {
        Mat43 mat;
        mat.setByEuler(angl2Rad(m_pDoc->rotV()), angl2Rad(m_pDoc->rotH()), 0.0);
        mat.pos = visBBox.center();
        if(m_pCam->projType() == Camera::PROJ_PERSPECTIVE)
        {// ͸��ͶӰ�£�ȷ���ڿɼ�����֮��
            double d = 0.0;
            for(auto& pt: visBBox.points())
                d = STMAX(d, visBBox.center().len(pt));

            mat.pos += mat.z * (d + 500.0);
        }
        else
        {// ����ͶӰ�£�������������壬��Ҫȷ�������������ͼ��Χ��֮�⼴��
            mat.pos += mat.z * (STMAX(500.0, pDoc->v2d(viewBBox.getMax().len(viewBBox.getMin()))));
        }

        m_pCam->setMatrix(pDoc->d2v(mat));
    }
    break;
    }

    m_pCam->refineToSee(OBB(pDoc->d2v(visBBox.getMin()), pDoc->d2v(visBBox.getMax()), Mat43::IDENTITY));
    this->onCameraTransform();
}

void View::onCameraTransform()
{
    m_pLight->setMatrix(m_pCam->matrix());
    m_camFrame = STCore::inst->frame();
    m_dirtyFlags |= (DIRTY_BIT_RT | DIRTY_BIT_CAM_DEPTH);
}

void View::resetCameraPose()
{
    m_camFrame = STCore::inst->frame();
    m_dirtyFlags |= (DIRTY_BIT_RT | DIRTY_BIT_CAM_POSE | DIRTY_BIT_CAM_DEPTH);
}

void View::updateCameraPose()
{
    if(m_dirtyFlags & DIRTY_BIT_CAM_POSE)
    {
        this->_resetCameraPose();
        m_dirtyFlags &= ~DIRTY_BIT_CAM_POSE;
        this->dirtyRT();
    }
}

void View::_updateCamDepth()
{
    DocManager* pDocMgr = STCore::inst->docMgr();
    Document* pDoc = pDocMgr->document();
    ViewCamera* pCam = this->camera();

    auto clipBox = m_pDoc->clipBox();
    clipBox.transform(pDoc->d2vMatrix());

    auto aabb = this->bbox();
    for(auto& pt: clipBox.pointsW())
        aabb.add(pt);
    aabb.transform(m_pCam->viewMatrix());

    if(m_pDoc->projMode() == Define::VIEWPROJ_ORTHO)
    {
        // m_pCam->moveForward(-aabb.m_max.z - pDoc->d2v(1.0));
        m_pCam->setZNear(pDoc->d2v(1.0));
        m_pCam->setZFar(STMAX(-aabb.m_min.z * 1.1f, 10.0f));
        // m_pCam->setZNear(0.01);
        // m_pCam->moveForward(-pDoc->d2v(1500.0));
        // m_pCam->setZFar(3000.0);
    }
    else
    {
        m_pCam->setZNear(pDoc->d2v(1.0));
        m_pCam->setZFar(STMAX(-aabb.m_min.z * 1.1f, 10.0f));
    }

    m_camFrame = STCore::inst->frame();
    this->dirtyRT();
}

void View::_resetCameraPose()
{
    DocManager* pDocMgr = STCore::inst->docMgr();
    Document* pDoc = pDocMgr->document();
    auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();
    ViewCamera* pCam = this->camera();

    auto& viewPlane = m_pDoc->viewPlane();
    Vec3 plane_n = viewPlane.normal();

    auto& clipBox = m_pDoc->clipBox();
    auto& workAreaMin = m_pDoc->workAreaMin();
    auto& workAreaMax = m_pDoc->workAreaMax();

    switch(m_pDoc->type())
    {
    case Define::VIEW_2D: {
        auto& refPlane = pOpts->snapRefPlane(this);
        AABB bbox = pDoc->v2d(this->bbox());
        bbox.add(clipBox);

        double zMax = STMAX3(bbox.sizeX(), bbox.sizeY(), bbox.sizeZ()) + 10.0;
        Vec3 pos = viewPlane.fromVertex2D((workAreaMin + workAreaMax) * 0.5) + refPlane.normal() * zMax;
        pCam->setMatrix(Mat43f(refPlane.axisX(), refPlane.axisY(), refPlane.normal(), pDoc->d2v(pos)));
    }
    break;
    case Define::VIEW_3D: {
        Vec3f target = pDoc->d2v(viewPlane.fromVertex2D((workAreaMin + workAreaMax) * 0.5));
        Vec3f pos = target - pCam->forwardDir();
        pCam->setPos(pos);
    }
    break;
    }

    OBB box = clipBox;
    box.inflate(10.0, 10.0, 10.0);
    pCam->refineToSee(pDoc->d2v(box.aabb()));

    this->onCameraTransform();
    this->dirtyRT();
}

void View::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* paras, DocElement* pInvoker) {}

void View::_onReclipObjects()
{
    m_objects.clear();
    m_hideObjects.clear();

    auto& objs = m_pScene->objects();
    if(!objs.empty())
    {
        for(auto it: objs)
        {
            RenderObject* pObj = it.second;
            TFObjectDoc* pObjDoc = pObj->doc();

            if(!pObjDoc->isGroup() && pObjDoc->viewId() >= 0 && pObjDoc->viewId() != m_pDoc->id())// not for this view
                continue;

            if(pObj->isViewContain(this))
                m_objects.insert(pObj);
        }
    }
}

void View::_onRefilterObjs()
{
    if(!m_pDoc)
        return;

    m_visObjects.clear();

    for(auto pRdObj: m_objects)
    {
        if(m_hideObjects.find(pRdObj) != m_hideObjects.end())
        {
            continue;
        }
        auto pObj = pRdObj->doc();
        m_visObjects.insert(pRdObj);

        auto pPar = pObj->parent();
        while(pPar)
        {
            if(pPar->renderObject())
                m_visObjects.insert(pPar->renderObject());
            pPar = pPar->parent();
        }
    }
}

void View::_updateBBox()
{
    Document* pDoc = STCore::inst->docMgr()->document();

    AABB bbox;
    for(auto pObj: m_objects)
    {
        if(!pObj->doc())
            continue;

        auto& objBox = pObj->doc()->bbox();
        if(!objBox.isValid())
            continue;

        auto aabb = objBox.aabb();
        bbox.add(aabb);
    }

    m_bbox = pDoc->d2v(bbox);
}

bool View::isInDepthRange(const Engine::OBB& obb) const
{
    auto& clipBox = m_pDoc->clipBox();
    auto& viewPlane = m_pDoc->viewPlane();
    double depthDown = m_pDoc->depthDown();
    double depthUp = m_pDoc->depthUp();

    Vec3 pts[] = { obb.m_min,
                   Vec3(obb.m_max.x, obb.m_min.y, obb.m_min.z),
                   Vec3(obb.m_max.x, obb.m_max.y, obb.m_min.z),
                   Vec3(obb.m_min.x, obb.m_max.y, obb.m_min.z),
                   Vec3(obb.m_min.x, obb.m_min.y, obb.m_max.z),
                   Vec3(obb.m_max.x, obb.m_min.y, obb.m_max.z),
                   obb.m_max,
                   Vec3(obb.m_min.x, obb.m_max.y, obb.m_max.z) };

    double zmin = DBL_MAX, zmax = -DBL_MAX;
    for(int i = 0; i < 8; ++i)
    {
        auto ptw = obb.m_matrix * pts[i];
        double d = viewPlane.sdist(ptw);
        if(RANGE_IN(d, -depthDown, depthUp))
            return true;

        zmin = STMIN(zmin, d);
        zmax = STMAX(zmax, d);
    }

    return STMAX(zmin, -depthDown) <= STMIN(zmax, depthUp);
}

void View::updateRenderObjects(const std::vector<RenderObject*>& objs)
{
    if(!m_pDoc || objs.empty())
        return;

    long long viewId = m_pDoc->id();
    auto pScene = m_pDoc->scene();

    for(auto pRdObj: objs)
    {
        auto pObj = pRdObj->doc();
        if(!pObj->isTF())
        {
            continue;
        }
        m_objects.insert(pRdObj);
    }

    STCore::inst->updateUIObjList();
    m_dirtyFlags |= (DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_RT | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_VIEW_FILTER);
}

void View::removeRenderObject(RenderObject* pObj)
{
    m_objects.erase(pObj);
    m_visObjects.erase(pObj);
    m_hideObjects.erase(pObj);

    STCore::inst->updateUIObjList();
    m_dirtyFlags |= (DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_RT | DIRTY_BIT_CAM_DEPTH);
}

void View::clearHideObjects()
{
    m_hideObjects.clear();
    m_dirtyFlags |= (DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_RT);
}

void View::hideObjects(const std::unordered_set<ObjectDoc*>& objs)
{
    for(auto pObj: objs)
    {
        if(!pObj->isTF())
            continue;

        std::stack<TFObjectDoc*> stack;
        stack.push(pObj->_tfObject());

        while(!stack.empty())
        {
            TFObjectDoc* pTop = stack.top();
            stack.pop();

            if(pTop->renderObject())
            {
                m_hideObjects.insert(pTop->renderObject());
            }

            if(pTop->isGroup())
            {
                for(auto pChild: pTop->_group()->children())
                {
                    if(pChild)
                        stack.push(pChild);
                }
            }
        }
    }

    m_dirtyFlags |= (DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_RT);
}

void View::setHideObjects(const std::unordered_set<ObjectDoc*>& objs)
{
    m_hideObjects.clear();

    for(auto pObj: objs)
    {
        if(!pObj || !pObj->isTF())
            continue;

        auto pTFObj = pObj->_tfObject();
        if(!pTFObj->renderObject())
            continue;

        m_hideObjects.insert(pTFObj->renderObject());
    }

    m_dirtyFlags |= (DIRTY_BIT_VIEW_BBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_CAM_DEPTH | DIRTY_BIT_RT);
}

bool View::obj_in_working_group(TFObjectDoc* obj)
{
    if(!obj)
    {
        return false;
    }
    TFObjectDoc* current = obj;
    while(current)
    {
        if(current->parent() == m_group_area)
        {
            return true;
        }
        current = current->parent();
    }

    return false;
}

TFObjectDoc* View::get_obj_highest_parent_in_working_group_area(TFObjectDoc* obj)
{
    if(!obj)
    {
        return nullptr;
    }
    TFObjectDoc* current = obj;
    while(current)
    {
        if(current->parent() == m_group_area)
        {
            return current;
        }
        current = current->parent();
    }
    return nullptr;
}

int View::get_obj_in_working_group_area_layer(TFObjectDoc* obj)
{
    if(!obj)
    {
        return -1;
    }
    int area = 0;
    TFObjectDoc* current = obj;
    while(current)
    {
        if(current->parent() == m_group_area)
        {
            return area;
        }
        area += 1;
        current = current->parent();
    }
    return -1;
}

void View::weld_lines_from_select()
{
    SceneDoc* scene = this->doc()->scene();
    ElementSelector* selector = this->selector();
    auto selectInfo = selector->get_select_info();
    std::vector<TFObjectDoc*> empty_mesh;
    RvtBlock blk(scene->rvtId());

    std::vector<MeshDoc*> cached_transfer_mesh_docs;
    // 先要把选中的曲线对象转成折线对象
    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_NURBS)
        {
            NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(objsInfo.first->doc());
            auto nurbs_data = nurbs_doc->nurbsdata(true);
            const nous::mat4 world_trans = NousConvert::convert(nurbs_doc->world_matrix());
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::Curve)
                {
                    NurbsCurvePtr curve = ele->to_nurbs_curve();
                    if(curve)
                    {
                        MeshDoc* tmp_mesh_doc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                        std::vector<nous::pos_t> transfered_lines = nurbs::convert_curve_to_polyline(curve);
                        NoBranchLineGroupPtr weld_line = std::make_shared<NoBranchLineGroup>();
                        for(int i = 0; i < transfered_lines.size() - 1; ++i)
                        {
                            const nous::pos_t prev_pos = transfered_lines[i];
                            const nous::pos_t curr_pos = transfered_lines[i + 1];
                            const nous::pos_t trans_prev_pos = NousConvert::product_v3(world_trans, prev_pos);
                            const nous::pos_t trans_curr_pos = NousConvert::product_v3(world_trans, curr_pos);
                            std::vector<SketchLinePtr> lines = tmp_mesh_doc->geodata()->add_line(prev_pos, curr_pos);
                            for(SketchLinePtr line: lines)
                            {
                                weld_line->add_element(line);
                            }
                        }
                        tmp_mesh_doc->geodata()->add_element_group(weld_line);
                        cached_transfer_mesh_docs.emplace_back(tmp_mesh_doc);
                        nurbs_data->remove_item(curve->id());
                    }
                }
            }

            if(nurbs_data->get_all_nurbs().empty())
            {
                empty_mesh.emplace_back(nurbs_doc);
            }
        }
    }

    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(objsInfo.first->doc());
            auto geo = mesh_doc->geodata(true);
            std::unordered_set<SketchElementPtr> weld_lines;
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::Line)
                {
                    weld_lines.insert(ele->to_line());
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    ElementGroupPtr line_group = ele->to_group();
                    for(auto& line: line_group->elements())
                    {
                        weld_lines.insert(line->to_line());
                    }
                    // 重新计算选中的焊接线
                    geo->delete_element_group(line_group);
                }
            }
            GeoDataTopoOperations::get().try_merge_no_branch_line_group(geo.get(), weld_lines, {});
            mesh_doc->dirty();
        }
    }

    for(auto& tmp_doc: cached_transfer_mesh_docs)
    {
        std::unordered_set<TFObjectDoc*> source_docs{ tmp_doc };
        std::unordered_set<TFObjectDoc*> target_docs;
        for(auto& obj: this->visObjects())
        {
            if(obj->doc()->type() == Define::ObjectType::OBJ_D5_MESH)
            {
                target_docs.insert(static_cast<MeshDoc*>(obj->doc()));
            }
        }
        check_intersection_and_merge_meshes(source_docs, target_docs);
    }

    for(auto& mesh: empty_mesh)
    {
        delete_obj_and_try_delete_relation(mesh);
    }

    selector->clear();
    this->dirtyRT();
}

void View::disperse_weld_line_from_select()
{
    SceneDoc* scene = this->doc()->scene();
    ElementSelector* selector = this->selector();
    auto selectInfo = selector->get_select_info();
    std::vector<TFObjectDoc*> empty_mesh;
    RvtBlock blk(scene->rvtId());
    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(objsInfo.first->doc());
            auto geo = mesh_doc->geodata(true);
            std::unordered_set<ElementGroupPtr> weld_line_group;
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    weld_line_group.insert(ele->to_group());
                }
            }
            for(auto group: weld_line_group)
            {
                geo->delete_element_group(group);
            }
            mesh_doc->dirty();
        }
    }
    selector->clear();
    this->dirtyRT();
}

void View::soft_polygon_from_select()
{
    SceneDoc* scene = this->doc()->scene();
    ElementSelector* selector = this->selector();
    auto selectInfo = selector->get_select_info();
    std::vector<TFObjectDoc*> empty_mesh;
    RvtBlock blk(scene->rvtId());
    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(objsInfo.first->doc());
            auto geo = mesh_doc->geodata(true);
            std::unordered_set<SketchElementPtr> soft_lines;
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::Line)
                {
                    soft_lines.insert(ele);
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    soft_lines.insert(ele);
                }
            }
            GeoDataTopoOperations::get().try_merge_smooth_group(geo.get(), soft_lines, false);
            mesh_doc->dirty();
        }
    }
    selector->clear();
    this->dirtyRT();
}

void View::disperse_smooth_group_from_select()
{
    SceneDoc* scene = this->doc()->scene();
    ElementSelector* selector = this->selector();
    auto selectInfo = selector->get_select_info();
    std::vector<TFObjectDoc*> empty_mesh;
    RvtBlock blk(scene->rvtId());
    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(objsInfo.first->doc());
            auto geo = mesh_doc->geodata(true);
            std::unordered_set<ElementGroupPtr> smooth_group;
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::SmoothGroup)
                {
                    smooth_group.insert(ele->to_group());
                }
            }
            for(auto group: smooth_group)
            {
                geo->delete_element_group(group);
            }
            mesh_doc->dirty();
        }
    }
    selector->clear();
    this->dirtyRT();
}

void View::delete_obj_from_select()
{
    SceneDoc* scene = this->doc()->scene();

    ElementSelector* selector = this->selector();
    auto selectInfo = selector->get_select_info();
    std::vector<TFObjectDoc*> empty_mesh;

    RvtBlock blk(scene->rvtId());
    for(auto& objsInfo: selectInfo)
    {
        if(objsInfo.first->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* group_doc = static_cast<GroupDoc*>(objsInfo.first->doc());
            this->delete_obj_and_try_delete_relation(group_doc);
        }
        else if(objsInfo.first->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(objsInfo.first->doc());
            auto geo = mesh_doc->geodata(true);
            std::unordered_set<SketchLinePtr> delete_lines;
            for(auto ele: objsInfo.second.elements)
            {
                if(ele->type() == EElementType::Polygon)
                {
                    geo->delete_polygon(ele->to_polygon());
                }
                else if(ele->type() == EElementType::Line)
                {
                    delete_lines.insert(ele->to_line());
                }
                else if(ele->type() == EElementType::NoBranchLineGroup)
                {
                    ElementGroupPtr line_group = ele->to_group();
                    for(auto& line: line_group->elements())
                    {
                        delete_lines.insert(line->to_line());
                    }
                    geo->delete_element_group(line_group);
                }
                else if(ele->type() == EElementType::SmoothGroup)
                {
                    SmoothGroupPtr group = std::dynamic_pointer_cast<SmoothGroup>(ele->to_group());
                    for(auto& line: group->soft_edge())
                    {
                        delete_lines.insert(line->to_line());
                    }
                    for(auto& polygon: group->soft_polygon())
                    {
                        geo->delete_polygon(polygon->to_polygon());
                    }
                    geo->delete_element_group(group);
                }
            }
            geo->delete_lines(delete_lines);
            if(geo->get_line_number() == 0)
            {
                empty_mesh.push_back(mesh_doc);
            }
            GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(geo.get());
            GeoDataTopoOperations::get().check_and_recreate_smooth_group(geo.get());
            mesh_doc->dirty();
        }
        else if(objsInfo.first->type() == Define::ObjectType::OBJ_D5_NURBS)
        {
            NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(objsInfo.first->doc());
            auto nurbs_data = nurbs_doc->nurbsdata(true);

            std::unordered_set<SketchElementPtr> elements;
            elements.insert(objsInfo.second.elements.begin(), objsInfo.second.elements.end());

            for(auto& ele: objsInfo.second.elements)
            {
                auto&& nearby_same_edge_elements = nurbs::find_same_edge_elements(ele);
                elements.insert(nearby_same_edge_elements.begin(), nearby_same_edge_elements.end());
            }

            std::unordered_set<SketchElementPtr> need_removed_complex_surfaces;
            std::unordered_set<SketchElementPtr> need_removed_edges;
            for(auto& ele: elements)
            {
                if(ele->type() == EElementType::AlgebraSurfaceEdge)
                {
                    auto& parent = ele->to_nurbs_algebra_surface_element()->m_parent;
                    auto& parent_complex_surface = parent.lock()->get_parent();
                    if(std::get<0>(parent_complex_surface))
                    {
                        need_removed_complex_surfaces.insert(std::get<0>(parent_complex_surface));
                    }
                    else if(std::get<1>(parent_complex_surface))
                    {
                        need_removed_complex_surfaces.insert(std::get<1>(parent_complex_surface));
                    }
                    need_removed_edges.insert(ele);
                }
                else if(ele->type() == EElementType::ClosedSurfaceEdge)
                {
                    auto& parent = ele->to_nurbs_closed_surface_element()->m_parent;
                    auto& parent_complex_surface = parent.lock()->get_parent();
                    if(std::get<0>(parent_complex_surface))
                    {
                        need_removed_complex_surfaces.insert(std::get<0>(parent_complex_surface));
                    }
                    else if(std::get<1>(parent_complex_surface))
                    {
                        need_removed_complex_surfaces.insert(std::get<1>(parent_complex_surface));
                    }
                    need_removed_edges.insert(ele);
                }
                else if(ele->type() == EElementType::TrimmedSurface)
                {
                    need_removed_complex_surfaces.insert(ele);
                }
                else
                {
                    need_removed_edges.insert(ele);
                }
            }

            // 先处理全部需要删除的复杂面对象
            for(auto& complex_surface: need_removed_complex_surfaces)
            {
                NurbsTrimmedSurfacePtr surface = complex_surface->to_nurbs_trimmed_surface();
                auto edge_curve_elements = surface->get_complex_surface_elements();
                for(auto& pair: edge_curve_elements)
                {
                    if(need_removed_edges.find(pair.first) == need_removed_edges.end())
                    {
                        // 查看是否存在重复线，如果有也不用加了
                        auto&& new_edge_same_nearby_edges = nurbs::find_same_edge_elements(pair.first);
                        bool all_deleted = true;
                        for(auto& nearby_edge: new_edge_same_nearby_edges)
                        {
                            if(need_removed_edges.find(nearby_edge) == need_removed_edges.end())
                            {
                                all_deleted = false;
                                break;
                            }
                        }

                        if(all_deleted)
                        {
                            if(pair.first->type() == EElementType::AlgebraSurfaceEdge)
                            {
                                nurbs_data->add_item(pair.first->to_nurbs_algebra_surface_element()->m_edge_curve);
                            }
                            if(pair.first->type() == EElementType::ClosedSurfaceEdge)
                            {
                                nurbs_data->add_item(pair.first->to_nurbs_closed_surface_element()->m_edge_curve.lock());
                            }
                        }
                    }
                }
                // 主动断掉和内部包含的各种 surface 对象的关系
                surface->finalization();
                nurbs_data->remove_item(surface->id());
            }

            for(auto& ele: need_removed_edges)
            {
                nurbs_data->remove_item(ele->id());
            }

            if(nurbs_data->get_all_nurbs().empty())
            {
                empty_mesh.push_back(nurbs_doc);
            }
            nurbs_doc->dirty();
        }
    }
    for(auto obj: empty_mesh)
    {
        this->delete_obj_and_try_delete_relation(obj);
    }
    selector->clear();
    this->dirtyRT();
}

void View::delete_obj_and_try_delete_relation(TFObjectDoc* obj)
{
    std::stack<TFObjectDoc*> stack;
    stack.push(obj);

    while(!stack.empty())
    {
        TFObjectDoc* current = stack.top();
        stack.pop();

        // 处理子对象
        if(current->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* group = current->_group();
            auto children = group->children();
            for(auto& child: children)
            {
                stack.push(child);
            }
            group->removeAllChildren();
        }

        // 删除父对象
        TFObjectDoc* parent = current->parent();
        if(parent && parent->_group())
        {
            parent->_group()->removeChild(current);
            if(parent->_group()->children().empty())
            {
                stack.push(parent);
            }
        }

        // 删除当前对象
        m_pScene->doc()->deleteObject(current->id());
    }
}

void View::convert_nurbs_item_into_geodata(NurbsDoc* nurbs_doc, MeshDoc* target, const SelectInfo mesh_select, nous::mat4& group_transform)
{
    nous::mat4 nurbs_doc_transform = NousConvert::convert(nurbs_doc->world_matrix());

    for(auto& element: mesh_select.elements)
    {
        if(element->type() == EElementType::Curve)
        {
            // Curve 对象转完之后就从原来的里面删了
            if(nurbs_doc->nurbsdata()->item_exist(element->id()))
            {
                NurbsCurvePtr curve = element->to_nurbs_curve();
                std::vector<nous::pos_t> sampled_positions = nurbs::convert_curve_to_polyline(curve);
                std::unordered_set<SketchElementPtr> weld_line_group;
                MeshDoc* tmp_doc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                for(int i = 0; i < sampled_positions.size() - 1; ++i)
                {
                    const nous::vec4 pt_pos_v4(sampled_positions[i].x(), sampled_positions[i].y(), sampled_positions[i].z(), 1.0);
                    const nous::vec4 lp0 = group_transform * nurbs_doc_transform * pt_pos_v4;
                    const nous::pos_t lp1 = lp0.head<3>() / lp0.w();

                    const nous::vec4 next_pt_pos_v4(sampled_positions[i + 1].x(), sampled_positions[i + 1].y(), sampled_positions[i + 1].z(), 1.0);
                    const nous::vec4 next_lp0 = group_transform * nurbs_doc_transform * next_pt_pos_v4;
                    const nous::pos_t next_lp1 = next_lp0.head<3>() / next_lp0.w();

                    auto& lines = tmp_doc->geodata()->add_line(lp1, next_lp1);
                    for(auto& line: lines)
                    {
                        weld_line_group.insert(line);
                    }
                }
                GeoDataTopoOperations::get().try_merge_no_branch_line_group(tmp_doc->geodata().get(), weld_line_group, {});
                target->merge(tmp_doc);
                delete_obj_and_try_delete_relation(tmp_doc);
                nurbs_doc->nurbsdata()->remove_item(element->id());
                nurbs_doc->nurbsdata()->dirty();
                nurbs_doc->dirty();
            }
        }
        // Tips：经严谨讨论，当前版本不要支持对曲面边线的离散化操作，这样会多出一条线导致重线
        // else if(element->type() == EElementType::AlgebraSurfaceEdge)
        //{
        //    // 曲面的边对象，纯加一条焊接线
        //    NurbsAlgebraSurfaceEdgeElementPtr algebra_edge_curve = element->to_nurbs_algebra_surface_element();
        //    if(!algebra_edge_curve->m_parent.expired())
        //    {
        //        NurbsAlgebraSurfacePtr algebra_surface = algebra_edge_curve->m_parent.lock();
        //        std::vector<nous::pos_t> sampled_positions = nurbs::convert_curve_to_polyline(algebra_edge_curve->m_edge_curve);
        //        NoBranchLineGroupPtr weld_line_group = std::make_shared<NoBranchLineGroup>();
        //        SketchPointPtr prev_point = nullptr;
        //        MeshDoc* tmp_doc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
        //        for(int i = 0; i < sampled_positions.size(); ++i)
        //        {
        //            const nous::vec4 pt_pos_v4(sampled_positions[i].x(), sampled_positions[i].y(), sampled_positions[i].z(), 1.0);
        //            const nous::vec4 lp0 = group_transform * nurbs_doc_transform * pt_pos_v4;
        //            const nous::pos_t lp1 = lp0.head<3>() / lp0.w();
        //            SketchPointPtr curr_point = tmp_doc->geodata()->add_point_without_check(lp1);
        //            if(prev_point)
        //            {
        //                auto& line = tmp_doc->geodata()->add_line_without_check(prev_point, curr_point);
        //                weld_line_group->add_element(line);
        //            }
        //            prev_point = curr_point;
        //        }
        //        tmp_doc->geodata()->add_element_group(weld_line_group);
        //        target->merge(tmp_doc);
        //        delete_obj_and_try_delete_relation(tmp_doc);
        //    }
        //}
        // else if(element->type() == EElementType::ClosedSurfaceEdge)
        //{
        //    // 曲面的边对象，纯加一条焊接线
        //    NurbsClosedSurfaceEdgeElementPtr closed_edge_curve = element->to_nurbs_closed_surface_element();
        //    if(!closed_edge_curve->m_parent.expired())
        //    {
        //        NurbsClosedSurfacePtr closed_surface = closed_edge_curve->m_parent.lock();
        //        if(!closed_edge_curve->m_edge_curve.expired())
        //        {
        //            std::vector<nous::pos_t> sampled_positions = nurbs::convert_curve_to_polyline(closed_edge_curve->m_edge_curve.lock());
        //            NoBranchLineGroupPtr weld_line_group = std::make_shared<NoBranchLineGroup>();
        //            SketchPointPtr prev_point = nullptr;
        //            MeshDoc* tmp_doc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
        //            for(int i = 0; i < sampled_positions.size(); ++i)
        //            {
        //                const nous::vec4 pt_pos_v4(sampled_positions[i].x(), sampled_positions[i].y(), sampled_positions[i].z(), 1.0);
        //                const nous::vec4 lp0 = group_transform * nurbs_doc_transform * pt_pos_v4;
        //                const nous::pos_t lp1 = lp0.head<3>() / lp0.w();
        //                SketchPointPtr curr_point = tmp_doc->geodata()->add_point_without_check(lp1);
        //                if(prev_point)
        //                {
        //                    auto& line = tmp_doc->geodata()->add_line_without_check(prev_point, curr_point);
        //                    weld_line_group->add_element(line);
        //                }
        //                prev_point = curr_point;
        //            }
        //            tmp_doc->geodata()->add_element_group(weld_line_group);
        //            target->merge(tmp_doc);
        //            delete_obj_and_try_delete_relation(tmp_doc);
        //        }
        //    }
        //}
        else if(element->type() == EElementType::TrimmedSurface)
        {
            NurbsTrimmedSurfacePtr trimmed_surface = element->to_nurbs_trimmed_surface();
            MeshDoc* tmp_doc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
            nurbs::convert_nurbs_surface_to_mesh(tmp_doc, nurbs_doc, trimmed_surface);
            target->merge(tmp_doc);
            delete_obj_and_try_delete_relation(tmp_doc);
        }
    }
}

void View::convert_to_geodata_from_select()
{
    auto select_info = m_selector->get_select_info();
    // 用来存放转换完成的数据
    std::unordered_set<NurbsDoc*> deleted_doc_set;
    for(auto& mesh_select: select_info)
    {
        if(mesh_select.first->type() == Define::OBJ_D5_NURBS)
        {
            MeshDoc* converted_meshdoc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
            NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(mesh_select.first->doc());
            nous::mat4 identity_trans = nous::mat4::Identity();
            convert_nurbs_item_into_geodata(nurbs_doc, converted_meshdoc, mesh_select.second, identity_trans);
            if(nurbs_doc->nurbsdata()->get_all_nurbs().size() == 0)
            {
                deleted_doc_set.insert(nurbs_doc);
            }

            std::unordered_set<TFObjectDoc*> scene_docs;
            for(auto& obj: visObjects())
            {
                if(obj->doc() != converted_meshdoc)
                {
                    scene_docs.insert(obj->doc());
                }
            }
            std::unordered_set<TFObjectDoc*> curr_docs;
            curr_docs.insert(converted_meshdoc);
            this->check_intersection_and_merge_meshes(curr_docs, scene_docs);
        }
        else if(mesh_select.first->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* group_doc = mesh_select.first->doc()->_group();
            if(group_doc)
            {
                nous::mat4 group_trans = NousConvert::convert(group_doc->world_matrix());

                std::unordered_set<TFObjectDoc*> group_need_removed_docs;
                std::unordered_set<TFObjectDoc*> group_need_add_docs;

                for(auto& tf_doc: group_doc->children())
                {
                    if(tf_doc->type() == Define::ObjectType::OBJ_D5_NURBS)
                    {
                        MeshDoc* converted_meshdoc = (MeshDoc*)create_obj_in_working_group(Define::ObjectType::OBJ_D5_MESH);
                        NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(tf_doc);
                        // 同 Group 内的转换无需考虑 Group 自身的 Transform 了，给一个 Identity 就行
                        nous::mat4 identity_trans = nous::mat4::Identity();
                        convert_nurbs_item_into_geodata(nurbs_doc, converted_meshdoc, mesh_select.second, identity_trans);
                        if(nurbs_doc->nurbsdata()->get_all_nurbs().size() == 0)
                        {
                            group_need_add_docs.insert(nurbs_doc);
                            deleted_doc_set.insert(nurbs_doc);
                        }
                        group_need_add_docs.insert(converted_meshdoc);
                    }
                }

                for(auto& remove_doc: group_need_removed_docs)
                {
                    group_doc->removeChild(remove_doc);
                }
                for(auto& add_doc: group_need_add_docs)
                {
                    std::unordered_set<TFObjectDoc*> scene_docs = group_doc->children();
                    group_doc->addChild(add_doc);
                    this->check_intersection_and_merge_meshes(std::unordered_set<TFObjectDoc*>{ add_doc }, scene_docs);
                }
            }
        }
    }

    for(auto& delete_nurbs_doc: deleted_doc_set)
    {
        delete_obj_and_try_delete_relation(delete_nurbs_doc);
    }
}

GroupDoc* View::create_group_from_select_elemnt()
{
    GroupDoc* group = nullptr;
    auto& select_info = m_selector->get_select_info();

    int select_ele_num = 0;
    for(auto& mesh_select: select_info)
    {
        GroupDoc* group = mesh_select.first->doc()->_group();
        if(group)
        {
            select_ele_num += 1;
            if(select_ele_num >= 2)
                break;
        }
        for(auto& line: mesh_select.second.elements)
        {
            select_ele_num += 1;
            if(select_ele_num >= 2)
                break;
        }
        if(select_ele_num >= 2)
            break;
    }
    if(select_ele_num >= 2)
    {
        RvtBlock blk(m_pScene->doc()->rvtId());
        ObjectDoc* obj = this->create_obj_in_working_group(Define::OBJ_D5_GROUP);
        group = static_cast<GroupDoc*>(obj);
        if(group)
        {
            if(m_group_area)
            {
                m_group_area->_group()->addChild(group);
            }
            std::unordered_set<TFObjectDoc*> empty_mesh;
            for(auto& mesh_select: select_info)
            {
                if(mesh_select.first->type() == Define::OBJ_D5_MESH)
                {
                    MeshDoc* src_mesh = static_cast<MeshDoc*>(mesh_select.first->doc());
                    auto src_geo = src_mesh->geodata(true);
                    MeshDoc* new_mesh_doc = (MeshDoc*)m_pScene->doc()->createObject(Define::OBJ_D5_MESH);
                    auto new_geo = new_mesh_doc->geodata();
                    // copy matrix
                    new_mesh_doc->set_matrix_from_world(src_mesh->world_matrix());

                    GeoDataTopoOperations::get().copy_element_without_check(new_geo.get(), src_geo.get(), mesh_select.second.elements);
                    new_mesh_doc->dirty();
                    group->addChild(new_mesh_doc);

                    std::unordered_set<SketchLinePtr> relation_lines;
                    for(auto ele: mesh_select.second.elements)
                    {
                        if(ele->type() == EElementType::Polygon)
                        {
                            GeoDataUtils::get().get_polygon_adj_lines(src_geo.get(), ele->to_polygon(), relation_lines);
                            src_geo->delete_polygon(ele->to_polygon());
                        }
                        else if(ele->type() == EElementType::Line)
                        {
                            relation_lines.insert(ele->to_line());
                        }
                        else if(ele->type() == EElementType::NoBranchLineGroup)
                        {
                            ElementGroupPtr line_group = ele->to_group();
                            for(auto& line: line_group->elements())
                            {
                                relation_lines.insert(line->to_line());
                            }
                            src_geo->delete_element_group(line_group);
                        }
                    }
                    for(auto line_key: relation_lines)
                    {
                        if(GeoDataUtils::get().get_line_adj_polygons(src_geo.get(), line_key).size() == 0)
                        {
                            src_geo->delete_line(line_key, false, false);
                        }
                    }
                    if(src_geo->get_line_number() == 0)
                    {
                        empty_mesh.insert(mesh_select.first->doc());
                    }
                }
                else if(mesh_select.first->type() == Define::OBJ_D5_NURBS)
                {
                    group->addChild(mesh_select.first->doc());
                }
                else if(mesh_select.first->type() == Define::OBJ_D5_GROUP)
                {
                    group->addChild(mesh_select.first->doc());
                }
            }
            for(auto obj: empty_mesh)
            {
                if(obj->parent()->_group())
                {
                    obj->parent()->_group()->removeChild(obj);
                }
                m_pScene->doc()->deleteObject(obj->id());
            }
            group->dirty();
        }
    }
    return group;
}

TFObjectDoc* View::create_obj_in_working_group(Define::ObjectType type, GroupDoc* working_group)
{
    ObjectDoc* new_doc = m_pScene->doc()->createObject(type);
    if(working_group && new_doc->_tfObject())
    {
        working_group->addChild(new_doc->_tfObject());
    }
    else if(m_group_area && new_doc->_tfObject())
    {
        m_group_area->_group()->addChild(new_doc->_tfObject());
    }
    return new_doc->_tfObject();
}

void View::disperse_group_from_select()
{
    RvtBlock blk(m_pScene->doc()->rvtId());
    auto& select_info = m_selector->get_select_info();
    std::unordered_set<GroupDoc*> need_delete;
    for(auto& mesh_select: select_info)
    {
        if(mesh_select.first->type() == Define::OBJ_D5_GROUP)
        {
            GroupDoc* group = static_cast<GroupDoc*>(mesh_select.first->doc());
            // dispersed group
            std::unordered_set<TFObjectDoc*> disperse_meshes;
            for(auto& child: group->children())
            {
                if(child->type() == Define::OBJ_D5_MESH)
                {
                    disperse_meshes.insert(static_cast<MeshDoc*>(child));
                }
            }
            group->removeAllChildren();

            if(group->parent())
            {
                GroupDoc* parent_group = static_cast<GroupDoc*>(group->parent());
                std::unordered_set<TFObjectDoc*> peer_meshes;
                for(auto& child: parent_group->children())
                {
                    if(child->type() == Define::OBJ_D5_MESH)
                    {
                        peer_meshes.insert(static_cast<MeshDoc*>(child));
                    }
                }
                check_intersection_and_merge_meshes(disperse_meshes, peer_meshes);
            }
            else
            {
                std::unordered_set<TFObjectDoc*> peer_meshes;
                for(auto& root_child: this->visObjects())
                {
                    if(root_child->type() == Define::OBJ_D5_MESH)
                    {
                        peer_meshes.insert(static_cast<MeshDoc*>(root_child->doc()));
                    }
                }
                check_intersection_and_merge_meshes(disperse_meshes, peer_meshes);
            }
            need_delete.insert(group);
        }
    }
    for(auto group: need_delete)
    {
        m_pScene->doc()->deleteObject(group->id());
    }
}

void View::copy_group(GroupDoc* org, GroupDoc* target)
{
    org->copy_from(target);
    for(auto child: target->children())
    {
        TFObjectDoc* new_child = this->create_obj_in_working_group(child->type(), org);
        new_child->copy_from(child);
        if(child->type() == Define::OBJ_D5_GROUP)
        {
            copy_group(static_cast<GroupDoc*>(new_child), static_cast<GroupDoc*>(child));
        }
        new_child->dirty();
    }
    org->dirty();
}

void View::check_intersection_and_merge_meshes(std::unordered_set<TFObjectDoc*>& src_meshes, std::unordered_set<TFObjectDoc*>& target_meshes)
{
    std::unordered_set<MeshDoc*> need_delete_mesh;
    std::unordered_set<ObjectDoc*> already_processed_mesh;
    auto valid_merge_obj = [&already_processed_mesh](ObjectDoc* t_mesh) {
        return already_processed_mesh.find(t_mesh) == already_processed_mesh.end() && t_mesh->type() == Define::OBJ_D5_MESH;
    };
    for(auto& change_mesh: src_meshes)
    {
        if(!valid_merge_obj(change_mesh))
        {
            continue;
        }
        TFObjectDoc* current_parent = change_mesh->parent();
        MeshDoc* c_mesh = static_cast<MeshDoc*>(change_mesh);
        View* view = STCore::inst->viewMgr()->layout()->focusedView();
        std::vector<ObjectDoc*> intersect_objs;

        Engine::OBB merge_mesh_obb = c_mesh->bbox();
        merge_mesh_obb.set(merge_mesh_obb.m_min - Engine::Vec3(0.1f), merge_mesh_obb.m_max + Engine::Vec3(0.1f), merge_mesh_obb.m_matrix);

        for(auto& obj: target_meshes)
        {
            if(obj->parent() == current_parent && obj != change_mesh && valid_merge_obj(obj) &&
               static_cast<MeshDoc*>(obj)->bbox().intersect(merge_mesh_obb))
            {
                intersect_objs.push_back(obj);
                already_processed_mesh.insert(obj);
            }
        }

        if(intersect_objs.size())
        {
            MeshDoc* base_doc = static_cast<MeshDoc*>(intersect_objs[0]);
            base_doc->record();
            for(int i = 1; i < intersect_objs.size(); ++i)
            {
                MeshDoc* cd = (MeshDoc*)intersect_objs[i];
                base_doc->merge_without_check(cd);
                need_delete_mesh.insert(cd);
            }
            base_doc->merge(c_mesh);
            need_delete_mesh.insert(c_mesh);
            already_processed_mesh.insert(c_mesh);

            // new obj merge
            intersect_objs.clear();
            Engine::OBB base_mesh_obb = base_doc->bbox();
            base_mesh_obb.set(base_mesh_obb.m_min - Engine::Vec3(0.1f), base_mesh_obb.m_max + Engine::Vec3(0.1f), base_mesh_obb.m_matrix);
            for(auto obj: target_meshes)
            {
                if(obj->parent() == current_parent && obj != base_doc && valid_merge_obj(obj) &&
                   static_cast<MeshDoc*>(obj)->bbox().intersect(base_mesh_obb))
                {
                    intersect_objs.push_back(obj);
                    already_processed_mesh.insert(static_cast<MeshDoc*>(obj));
                }
            }
            for(int i = 0; i < intersect_objs.size(); ++i)
            {
                MeshDoc* cd = (MeshDoc*)intersect_objs[i];
                base_doc->merge(cd);
                need_delete_mesh.insert(cd);
            }
            base_doc->dirty();
        }
    }

    for(auto delete_mesh: need_delete_mesh)
    {
        if(delete_mesh->parent()->_group())
        {
            delete_mesh->parent()->_group()->removeChild(delete_mesh);
        }
        m_pScene->doc()->deleteObject(delete_mesh->id());
    }
}

void View::serialize_view_list(SketchSerializer* serializer)
{
    std::unordered_set<TFObjectDoc*> root_objs;
    for(auto& obj: this->visObjects())
    {
        if(obj->doc()->_tfObject())
        {
            TFObjectDoc* root_obj = obj->doc()->_tfObject();
            while(root_obj->parent())
            {
                root_obj = root_obj->parent();
            }
            root_objs.insert(root_obj);
        }
    }
    serializer->begin_write_array("View List");
    for(auto& obj: root_objs)
    {
        serialize_tfobj(obj, serializer);
    }
    serializer->end_write_array();
}

void View::serialize_tfobj(TFObjectDoc* obj, SketchSerializer* serializer)
{
    serializer->begin_write_object("Object");
    if(obj->type() == Define::OBJ_D5_GROUP)
    {
        std::string cur_name = "Group " + std::to_string(obj->id());
        serializer->begin_write_array(cur_name.c_str());
        for(auto& child: obj->_group()->children())
        {
            serialize_tfobj(child, serializer);
        }
        serializer->end_write_array();
    }
    else
    {
        std::string cur_name;
        if(obj->type() == Define::OBJ_D5_MESH)
        {
            cur_name = "Polygon " + std::to_string(obj->id());
        }
        else if(obj->type() == Define::OBJ_D5_NURBS)
        {
            cur_name = "Nurbs " + std::to_string(obj->id());
        }
        serializer->begin_write_array(cur_name.c_str());
        serializer->end_write_array();
    }
    serializer->end_write_object();
}

void View::prepare_mesh(RenderMesh mesh)
{
    if(m_render)
    {
        m_render->append_mesh(mesh);
    }
}

void View::delete_mesh(long long id) const
{
    if(m_render)
    {
        m_render->append_delete_mesh(id);
    }
}

void View::batchRenderCtrlPoints(int idxMode /*0:first 1:second 2:normal*/, bool isSel, bool isHover, const Engine::Vec3& posW)
{
    if(m_render)
    {
        m_render->append_render_point(idxMode, isSel, isHover, { float(posW.x), float(posW.y), float(posW.z) });
    }
}

void View::batchRenderScreenLine(int width, float alpha, const Engine::Vec3f& color, const Engine::Vec3f& v0, const Engine::Vec3f& v1,
                                 uint16_t pattern, uint8_t factor, bool occlude)
{
    if(m_render)
    {
        m_render->append_render_screen_line(width, alpha, { color.x, color.y, color.z }, { v0.x, v0.y, v0.z }, { v1.x, v1.y, v1.z }, pattern, factor,
                                            occlude);
    }
}

void View::batchRenderLine(int width, float alpha, const Engine::Vec3f& color, const Engine::Vec3& v0, const Engine::Vec3& v1, uint16_t pattern,
                           uint8_t factor, bool clip, bool occlude)
{
    if(m_render)
    {
        m_render->append_render_line(width, alpha, { color.x, color.y, color.z }, { float(v0.x), float(v0.y), float(v0.z) },
                                     { float(v1.x), float(v1.y), float(v1.z) }, pattern, factor, clip, occlude);
    }
}

void View::batchRenderTri(const Engine::Vec3f& color, const Engine::Vec3& v0, const Engine::Vec3& v1, const Engine::Vec3& v2, bool occlude,
                          bool twoSide, bool clip, float alpha)
{
    if(m_render)
    {
        m_render->append_render_triangle({ color.x, color.y, color.z }, { float(v0.x), float(v0.y), float(v0.z) },
                                         { float(v1.x), float(v1.y), float(v1.z) }, { float(v2.x), float(v2.y), float(v2.z) }, occlude, twoSide, clip,
                                         alpha);
    }
}
}// namespace sketch
