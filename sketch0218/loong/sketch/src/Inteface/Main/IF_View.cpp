#include "sketch/STCore.h"

#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include <Windows.h>

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/View/Revertable_View_Hide.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC BOOL isViewNameValid(const WCHAR* name, long long id)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    ViewDoc* pView = pDoc->getView(name);
    return !pView || pView->id() == id;
}

extern "C" CORE_DLL_DECLSPEC BOOL isTempView(long long id)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    ViewDoc* pView = pDoc->getView(id);
    if(pView == nullptr)
        return false;

    return pView->isTemp();
}

extern "C" CORE_DLL_DECLSPEC long long focusedViewId()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return -1LL;

    return pView->doc()->id();
}

CORE_DLL_DECLSPEC BOOL getAllViewIdAndNames(std::vector<long long>& ids, std::vector<std::wstring>& names, BOOL incTemp)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;
    for(auto it: pDoc->views())
    {
        if(!it.second->isTemp() || incTemp)
        {
            ids.push_back(it.first);
            names.push_back(it.second->name());
        }
    }

    return true;
}

CORE_DLL_DECLSPEC BOOL getActiveViewIdAndNames(std::vector<long long>& ids, std::vector<std::wstring>& names, BOOL incTemp)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    for(auto id: pDoc->activeViews())
    {
        ViewDoc* pView = pDoc->getView(id);
        ids.push_back(id);
        names.push_back(pView->name());
    }

    if(incTemp)
    {
        for(auto it: pDoc->views())
        {
            if(it.second->isTemp())
            {
                ids.push_back(it.first);
                names.push_back(it.second->name());
            }
        }
    }

    return true;
}

CORE_DLL_DECLSPEC std::vector<long long> focusedViewIds()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return {};

    auto& focusedCells = pViewMgr->layout()->focusedCells();

    std::vector<long long> viewIds;
    if(!focusedCells.empty())
        viewIds.reserve(focusedCells.size());

    for(auto id: focusedCells)
    {
        View* pView = pViewMgr->getView(id);
        if(!pView)
            continue;

        viewIds.push_back(id);
    }
    return viewIds;
}

CORE_DLL_DECLSPEC const Engine::DObject* focusedViewProps() { return nullptr; }

CORE_DLL_DECLSPEC const Engine::DObject* viewProps(long long id) { return nullptr; }

CORE_DLL_DECLSPEC BOOL modifyView(long long viewId, const Engine::DObject& props)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewDoc* pView = pDoc->getView(viewId);
    if(!pView)
        return false;

    const auto param_map = SketchParamMap::alloc(&props);
    pView->modify(param_map);
    if(props.has("save_temp"))
    {
        pDoc->addActiveView(viewId);
    }

    if(UIFuncs::onCellNameUpdate)
    {
        UIFuncs::onCellNameUpdate({ viewId }, { pView->name() });
    }
    return true;
}

CORE_DLL_DECLSPEC BOOL modifyViews(long long* ids, int nIds, const Engine::DObject& props)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    for(int i = 0; i < nIds; ++i)
    {
        long long viewId = ids[i];
        ViewDoc* pView = pDoc->getView(viewId);
        if(pView)
        {
            SketchParamMap param_map = SketchParamMap::alloc(&props);
            pView->modify(param_map);
            if(props.has("save_temp"))
            {
                pDoc->addActiveView(viewId);
            }

            viewIds.push_back(viewId);
            viewNames.push_back(pView->name());
        }
    }

    if(!viewIds.empty())
    {
        if(UIFuncs::onCellNameUpdate)
        {
            UIFuncs::onCellNameUpdate(viewIds, viewNames);
        }
    }

    return true;
}

CORE_DLL_DECLSPEC BOOL modifyFocusedViews(const Engine::DObject& props)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    auto& ids = pLayout->focusedCells();
    for(auto id: ids)
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = pCell->_view();

        pView->updateRedrawFrame();
        SketchParamMap param_map = SketchParamMap::alloc(&props);
        pView->doc()->modify(param_map);

        if(props.has("save_temp"))
        {
            pDoc->addActiveView(pView->doc()->id());
        }

        viewIds.push_back(id);
        viewNames.push_back(pView->doc()->name());
    }

    if(UIFuncs::onCellNameUpdate)
    {
        UIFuncs::onCellNameUpdate(viewIds, viewNames);
    }
    return true;
}

CORE_DLL_DECLSPEC BOOL modifyDisplaySettingInFocusedViews(const Engine::DObject& props)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    DObject viewProps;
    viewProps.set("disp_setting", &props, true);// NOTE: 注意，这里直接作为引用设入

    auto& ids = pLayout->focusedCells();
    for(auto id: ids)
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = pCell->_view();

        pView->updateRedrawFrame();
        SketchParamMap param_map = SketchParamMap::alloc(&props);
        pView->doc()->modify(param_map);

        viewIds.push_back(id);
        viewNames.push_back(pView->doc()->name());
    }

    viewProps.detach("disp_setting");// NOTE: 解绑引用，否则props会随viewProps的析构被删除

    return true;
}

CORE_DLL_DECLSPEC BOOL modifyDisplayFilterInFocusedViews(const Engine::DObject& props)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    DObject viewProps;
    viewProps.set("disp_filter", &props, true);// NOTE: 注意，这里直接作为引用设入

    auto& ids = pLayout->focusedCells();
    for(auto id: ids)
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = pCell->_view();

        pView->updateRedrawFrame();
        SketchParamMap param_map = SketchParamMap::alloc(&props);
        pView->doc()->modify(param_map);

        viewIds.push_back(id);
        viewNames.push_back(pView->doc()->name());
    }

    viewProps.detach("disp_filter");// NOTE: 解绑引用，否则props会随viewProps的析构被删除

    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL modifyPartRenderModeInFocusedViews(sketch::Define::ViewRenderMode rdMode)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    DObject viewProps = { { "disp_setting", DObject({ { "part_mode", (int)rdMode } }) } };

    auto& ids = pLayout->focusedCells();
    for(auto id: ids)
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = pCell->_view();

        pView->updateRedrawFrame();
        SketchParamMap param_map = SketchParamMap::alloc(&viewProps);
        pView->doc()->modify(param_map);

        viewIds.push_back(id);
        viewNames.push_back(pView->doc()->name());
    }

    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL modifyPartRenderModeCompInFocusedViews(sketch::Define::ViewRenderMode rdMode)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();

    std::vector<long long> viewIds;
    std::vector<std::wstring> viewNames;

    DObject viewProps = { { "disp_setting", DObject({ { "part_mode_comp", (int)rdMode } }) } };

    auto& ids = pLayout->focusedCells();
    for(auto id: ids)
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = pCell->_view();

        pView->updateRedrawFrame();
        SketchParamMap param_map = SketchParamMap::alloc(&viewProps);
        pView->doc()->modify(param_map);

        viewIds.push_back(id);
        viewNames.push_back(pView->doc()->name());
    }

    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL modifyPointSizeInAllViews(double ptSize)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();

    DObject viewProps = { { "disp_setting", DObject({ { "pt_size", ptSize } }) } };

    SketchParamMap param_map = SketchParamMap::alloc(&viewProps);
    for(auto& it: pDoc->views())
    {
        it.second->modify(param_map);
    }

    pViewMgr->layout()->dirtyCells();
    return true;
}

extern "C" CORE_DLL_DECLSPEC void deleteViews(long long* ids, int nIds)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    for(int i = 0; i < nIds; ++i)
    {
        pDoc->deleteView(ids[i]);
    }

    // STCore::inst->updateUIActiveViewList();
    STCore::inst->dirtyView();
}

extern "C" CORE_DLL_DECLSPEC long long createObjView3D(long long objId, const WCHAR* viewName, BOOL temp)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();
    auto pDoc = STCore::inst->docMgr()->document();

    TFObjectDoc* pObj = pDoc->getTFObject(objId);
    if(pObj == nullptr)
        return -1LL;

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$part_view_3d");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    AxisPlane viewPlane = Mat43::IDENTITY;

    AABB clipBox = pObj->bbox();
    double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    clipBox.inflate(ext, ext, ext);

    AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);

    ViewDoc* pNewView = pDoc->createView(-1, pObj->scene()->id());

    DObject props = *pCfg;
    props["temp"] = bool(temp);
    props["name"] = name;
    props["type"] = (int)Define::VIEW_3D;
    props["proj"] = (int)Define::VIEWPROJ_PERSPECTIVE;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = clipBoxL.m_min.xy;
    props["work_area_max"] = clipBoxL.m_max.xy;
    props["depth_up"] = STMAX(clipBoxL.m_max.z, pCfg->getDouble("depth_up"));
    props["depth_down"] = STMAX(-clipBoxL.m_min.z, pCfg->getDouble("depth_down"));

    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pNewView->modify(param_map);

    pNewView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    pNewView->resetCameraPose();
    pViewMgr->layout()->addCell(pNewView->renderView());
    pViewMgr->layout()->setFocusedCell(pNewView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return pNewView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createView3DForCurObjs(const wchar_t* viewName, BOOL temp)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();
    auto pDoc = STCore::inst->docMgr()->document();
    auto pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return -1LL;

    SceneDoc* pScene = pView->scene()->doc();
    // auto& curObjs = pScene->curObjs();
    //
    AABB objBBox;
    // for(auto pObj: curObjs)
    //{
    //     if(!pObj->isTF())
    //         continue;
    //
    //     objBBox.add(pObj->_tfObject()->bbox());
    // }

    // if(!objBBox.isValid())
    //     return -1LL;

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$runtime");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    AxisPlane viewPlane = Mat43::IDENTITY;

    AABB clipBox = objBBox;
    double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    clipBox.inflate(ext, ext, ext);

    AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);

    ViewDoc* pNewView = pDoc->createView(-1, pScene->id());

    DObject props = *pCfg;
    props["temp"] = bool(temp);
    props["name"] = name;
    props["type"] = (int)Define::VIEW_3D;
    props["proj"] = (int)Define::VIEWPROJ_PERSPECTIVE;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = clipBoxL.m_min.xy;
    props["work_area_max"] = clipBoxL.m_max.xy;
    props["depth_up"] = clipBoxL.m_max.z;
    props["depth_down"] = -clipBoxL.m_min.z;

    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pNewView->modify(param_map);

    pNewView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    pNewView->resetCameraPose();
    pViewMgr->layout()->addCell(pNewView->renderView());
    pViewMgr->layout()->setFocusedCell(pNewView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return pNewView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createViewOrthoForCurObjs(const wchar_t* viewName, BOOL temp)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();
    auto pDoc = STCore::inst->docMgr()->document();
    auto pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return -1LL;

    SceneDoc* pScene = pView->scene()->doc();

    // auto& curObjs = pScene->curObjs();

    AABB objBBox;
    // for(auto pObj: curObjs)
    //{
    //     if(!pObj->isTF())
    //         continue;
    //
    //     objBBox.add(pObj->_tfObject()->bbox());
    // }

    if(!objBBox.isValid())
        return -1LL;

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$runtime");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    AxisPlane viewPlane = Mat43::IDENTITY;

    AABB clipBox = objBBox;
    double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    clipBox.inflate(ext, ext, ext);

    AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);

    ViewDoc* pNewView = pDoc->createView(-1, pScene->id());

    DObject props = *pCfg;
    props["temp"] = bool(temp);
    props["name"] = name;
    props["type"] = (int)Define::VIEW_3D;
    props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = clipBoxL.m_min.xy;
    props["work_area_max"] = clipBoxL.m_max.xy;
    props["depth_up"] = clipBoxL.m_max.z;
    props["depth_down"] = -clipBoxL.m_min.z;

    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pNewView->modify(param_map);

    pNewView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    pNewView->resetCameraPose();
    pViewMgr->layout()->addCell(pNewView->renderView());
    pViewMgr->layout()->setFocusedCell(pNewView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return pNewView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createObjViewOrtho(long long objId, const WCHAR* viewName, BOOL temp)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();
    auto pDoc = STCore::inst->docMgr()->document();

    TFObjectDoc* pObj = pDoc->getTFObject(objId);
    if(pObj == nullptr)
        return -1LL;

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$part_view_3d");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    AxisPlane viewPlane = Mat43::IDENTITY;

    AABB clipBox = pObj->bbox();
    double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    clipBox.inflate(ext, ext, ext);

    AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);

    ViewDoc* pNewView = pDoc->createView(-1, pObj->scene()->id());

    DObject props = *pCfg;
    props["temp"] = bool(temp);
    props["name"] = name;
    props["type"] = (int)Define::VIEW_3D;
    props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = clipBoxL.m_min.xy;
    props["work_area_max"] = clipBoxL.m_max.xy;
    props["depth_up"] = STMAX(clipBoxL.m_max.z, pCfg->getDouble("depth_up"));
    props["depth_down"] = STMAX(-clipBoxL.m_min.z, pCfg->getDouble("depth_down"));

    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pNewView->modify(param_map);

    pNewView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    pNewView->resetCameraPose();
    pViewMgr->layout()->addCell(pNewView->renderView());
    pViewMgr->layout()->setFocusedCell(pNewView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return pNewView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createObjViewFromModelSide(long long objId, const WCHAR* viewName, Define::ObjectSide objSide, BOOL temp)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();

    TFObjectDoc* pObj = pDoc->getTFObject(objId);
    if(pObj == nullptr)
        return -1LL;

    auto& matW = pObj->parent_matrix();

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$part_view_2d");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    AxisPlane sidePlane;// = pObj->sideMatrix(objSide);

    Vec3 viewY = sidePlane.projDir(Vec3::AXIS_Z);
    if(viewY.len2() < 1e-3)
        viewY = sidePlane.axisY();

    Vec3 viewX = viewY.cross(sidePlane.normal());
    Vec3 viewO = sidePlane.o();

    AxisPlane viewPlane(viewO, viewX, viewY);

    AABB clipBox = pObj->bbox();
    double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    clipBox.inflate(ext, ext, ext);

    AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);

    ViewDoc* pView = pDoc->createView(-1, pObj->scene()->id());

    DObject props = *pCfg;
    props["temp"] = bool(temp);
    props["name"] = name;
    props["type"] = (int)Define::VIEW_2D;
    props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = clipBoxL.m_min.xy;
    props["work_area_max"] = clipBoxL.m_max.xy;
    props["depth_up"] = STMAX(clipBoxL.m_max.z, pCfg->getDouble("depth_up"));
    props["depth_down"] = STMAX(-clipBoxL.m_min.z, pCfg->getDouble("depth_down"));

    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pView->modify(param_map);

    pView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    pView->resetCameraPose();
    pViewMgr->layout()->addCell(pView->renderView());
    pViewMgr->layout()->setFocusedCell(pView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return pView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createObjViewFromObjSide(long long objId, const WCHAR* viewName, Define::ObjectSide objSide, BOOL temp)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();

    TFObjectDoc* pObj = pDoc->getTFObject(objId);
    if(pObj == nullptr)
        return -1LL;

    auto& matW = pObj->parent_matrix();

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$part_view_2d");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    // auto matSide = pObj->sideMatrix(objSide);
    // if(!matSide.isRHS())
    //     matSide.x = -matSide.x;
    //
    //// 视图整体朝下了，上方向调个头
    // if(matSide.y.z < -1e-3)
    //{
    //     matSide.x = -matSide.x;
    //     matSide.y = -matSide.y;
    // }
    //
    // AxisPlane viewPlane = matSide;
    // viewPlane.setO(pObj->bbox().centerW());
    //
    // AABB clipBox = pObj->bbox();
    // double ext = STMAX(10.0, STMIN(300.0, clipBox.minSize() * 0.3));
    // clipBox.inflate(ext, ext, ext);
    //
    // AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);
    //
    // ViewDoc* pView = pDoc->createView(-1, pObj->scene()->id());
    //
    // DObject props = *pCfg;
    // props["temp"] = bool(temp);
    // props["name"] = name;
    // props["type"] = (int)Define::VIEW_2D;
    // props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    // props["viewplane"] = viewPlane;
    // props["work_area_min"] = clipBoxL.m_min.xy;
    // props["work_area_max"] = clipBoxL.m_max.xy;
    // props["depth_up"] = STMAX(clipBoxL.m_max.z, pCfg->getDouble("depth_up"));
    // props["depth_down"] = STMAX(-clipBoxL.m_min.z, pCfg->getDouble("depth_down"));
    //
    // SketchParamMap param_map = SketchParamMap::alloc(&props);
    // pView->modify(param_map);
    //
    // pView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    // pView->resetCameraPose();
    // pViewMgr->layout()->addCell(pView->renderView());
    // pViewMgr->layout()->setFocusedCell(pView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return 0;
}

extern "C" CORE_DLL_DECLSPEC long long createModelViewFromObjSide(long long objId, const WCHAR* viewName, Define::ObjectSide objSide, BOOL temp)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    DocManager* pDocMgr = STCore::inst->docMgr();
    sketch::ConfigureManager* pCfgMgr = pDocMgr->cfgMgr();

    TFObjectDoc* pObj = pDoc->getTFObject(objId);
    if(pObj == nullptr)
        return -1LL;

    auto pScene = pObj->scene();
    auto& matW = pObj->parent_matrix();

    const Engine::DObject* pCfg = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$part_view_2d");

    std::wstring name = viewName;
    name = pDoc->getValidViewName(temp ? L"*" + name : name);

    // auto matSide = pObj->sideMatrix(objSide);
    // if(!matSide.isRHS())
    //     matSide.x = -matSide.x;
    //
    //// 视图整体朝下了，上方向调个头
    // if(matSide.y.z < -1e-3)
    //{
    //     matSide.x = -matSide.x;
    //     matSide.y = -matSide.y;
    // }
    //
    // AxisPlane viewPlane = matSide;
    // viewPlane.setO(pObj->bbox().centerW());
    //
    // AABB clipBox = pScene->bbox();
    //
    // AABB clipBoxL = viewPlane.toPlaneSpace(clipBox);
    //
    // ViewDoc* pView = pDoc->createView(-1, pScene->id());
    //
    // DObject props = *pCfg;
    // props["temp"] = bool(temp);
    // props["name"] = name;
    // props["type"] = (int)Define::VIEW_2D;
    // props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    // props["viewplane"] = viewPlane;
    // props["work_area_min"] = clipBoxL.m_min.xy;
    // props["work_area_max"] = clipBoxL.m_max.xy;
    // props["depth_up"] = STMAX(clipBoxL.m_max.z, pCfg->getDouble("depth_up"));
    // props["depth_down"] = STMAX(-clipBoxL.m_min.z, pCfg->getDouble("depth_down"));
    //
    // SketchParamMap param_map = SketchParamMap::alloc(&props);
    // pView->modify(param_map);
    //
    // pView->setCamPivot(viewPlane.fromVertex2D(clipBoxL.center().xy));
    // pView->resetCameraPose();
    // pViewMgr->layout()->addCell(pView->renderView());
    // pViewMgr->layout()->setFocusedCell(pView->renderView());
    // STCore::inst->updateUIActiveViewList();

    return 0;
}

extern "C" CORE_DLL_DECLSPEC void focusCurObjsInFocusedView(Define::ViewFocusMode focusMode)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    SceneDoc* pScene = pView->doc()->scene();

    // pView->clearTempVisibleObjects();
    // pView->setFocusMode(focusMode);
    // pView->setFocusFrame(STCore::inst->frame());
    // pView->clearFocusObjects();
    // pView->focusObjects(pScene->curObjs());
    pView->dirtyRT();
}

extern "C" CORE_DLL_DECLSPEC void hideObjInFocusedView(long long objId)
{
    CmdManager* pCmdMgr = STCore::inst->cmdMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    SceneDoc* pScene = pView->doc()->scene();

    Revertable_View_Hide* pRvt = nullptr;
    if(!pCmdMgr->isRvtFreezed(pScene->rvtId()))
    {
        pCmdMgr->beginRvtQueue(pScene->rvtId());
        pRvt = new Revertable_View_Hide(pView->id());
        pRvt->setHideBefore(pView->getHideObjects());
    }

    auto pObj = pScene->getTFObject(objId);
    if(!pObj)
        return;

    pView->hideObjects({ pObj });
    // pScene->removeCurs({ objId });
    pScene->updateUISelInfo();

    if(pRvt)
    {
        pRvt->setHideAfter(pView->getHideObjects());
        pCmdMgr->pushRvt(pScene->rvtId(), pRvt);
        pCmdMgr->endRvtQueue(pScene->rvtId());
    }

    pView->dirtyRT();
}

extern "C" CORE_DLL_DECLSPEC void setWorkAreaToModelInFocusedViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto cid: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(cid);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        auto pView = pCell->_view();
        auto& viewPl = pView->doc()->viewPlane();
        Vec3 axis = pView->doc()->majorAxis();

        auto sceneBox = pView->doc()->scene()->bbox();
        sceneBox.inflate(100.0, 100.0, 100.0);

        auto sceneBoxL = pView->doc()->viewPlane().toPlaneSpace(sceneBox);
        pView->doc()->setWorkArea(sceneBoxL.m_min.xy, sceneBoxL.m_max.xy);
    }
}

extern "C" CORE_DLL_DECLSPEC void setWorkAreaToVisibleAreaInFocusedViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto cid: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(cid);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        auto pView = pCell->_view();
        auto& viewPl = pView->doc()->viewPlane();
        Vec3 axis = pView->doc()->majorAxis();

        AABB viewBox;
        for(auto pObj: pView->visObjects())
        {
            if(!pObj->doc())
                continue;

            viewBox.add(pObj->doc()->bbox());
        }

        if(!viewBox.isValid())
            continue;

        viewBox.inflate(100.0, 100.0, 100.0);

        auto viewBoxL = pView->doc()->viewPlane().toPlaneSpace(viewBox);
        pView->doc()->setWorkArea(viewBoxL.m_min.xy, viewBoxL.m_max.xy);
    }
}

extern "C" CORE_DLL_DECLSPEC void setWorkAreaToCurObjsInFocusedViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto cid: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(cid);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        auto pView = pCell->_view();
        auto& viewPl = pView->doc()->viewPlane();
        Vec3 axis = pView->doc()->majorAxis();

        SceneDoc* pScene = pView->doc()->scene();

        // auto& curObjs = pScene->curObjs();
        // if(curObjs.empty())
        //     continue;

        AABB objBox;
        // for(auto pObj: curObjs)
        //{
        //     if(!pObj->isTF())
        //         continue;
        //
        //     objBox.add(pObj->_tfObject()->bbox());
        // }

        if(!objBox.isValid())
            continue;

        objBox.inflate(100.0, 100.0, 100.0);

        auto objBoxL = pView->doc()->viewPlane().toPlaneSpace(objBox);
        pView->doc()->setWorkArea(objBoxL.m_min.xy, objBoxL.m_max.xy);
    }
}

extern "C" CORE_DLL_DECLSPEC void setWorkAreaToCurObjsInActiveViews()
{
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto& it: pLayout->cells())
    {
        if(it.second->cellType() != Define::CELL_VIEW)
            continue;

        View* pView = it.second->_view();
        auto& viewPl = pView->doc()->viewPlane();
        Vec3 axis = pView->doc()->majorAxis();

        SceneDoc* pScene = pView->doc()->scene();
        // auto& curObjs = pScene->curObjs();
        // if(curObjs.empty())
        //     continue;

        AABB objBox;
        // for(auto pObj: curObjs)
        //{
        //     if(!pObj->isTF())
        //         continue;
        //
        //     objBox.add(pObj->_tfObject()->bbox());
        // }

        if(!objBox.isValid())
            continue;

        objBox.inflate(100.0, 100.0, 100.0);

        auto objBoxL = pView->doc()->viewPlane().toPlaneSpace(objBox);
        pView->doc()->setWorkArea(objBoxL.m_min.xy, objBoxL.m_max.xy);
    }
}

extern "C" CORE_DLL_DECLSPEC long long createModelViewBase(sketch::Define::SpacePlane plane, double height)
{
    ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    SceneDoc* pScene = pDoc->scene();

    auto pViewProps = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$runtime");
    if(!pViewProps)
        return -1LL;

    std::wstring name;
    if(pViewProps->has("name"))
        name += pViewProps->getWStr("name");
    else if(plane == Define::PLANE_XY)
        name += L"X-Y";
    else if(plane == Define::PLANE_YZ)
        name += L"Y-Z";
    else if(plane == Define::PLANE_ZX)
        name += L"X-Z";

    if(!SU::startWith(name, L'*'))
        name = L"*" + name;

    name = pDoc->getValidViewName(name);

    AxisPlane viewPlane;
    if(plane == Define::PLANE_XY)
    {
        viewPlane.set({ 0.0, 0.0, height }, Vec3::AXIS_X, Vec3::AXIS_Y);
    }
    else if(plane == Define::PLANE_YZ)
    {
        viewPlane.set({ height, 0.0, 0.0 }, Vec3::AXIS_Y, Vec3::AXIS_Z);
    }
    else if(plane == Define::PLANE_ZX)
    {
        viewPlane.set({ 0.0, height, 0.0 }, -Vec3::AXIS_X, Vec3::AXIS_Z);
    }

    AABB sceneBox = pScene->bbox();
    auto sceneBoxL = viewPlane.toPlaneSpace(sceneBox);

    DObject props = *pViewProps;
    props["temp"] = true;
    props["name"] = name;
    props["type"] = (int)Define::VIEW_2D;
    props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    props["viewplane"] = viewPlane;
    props["work_area_min"] = sceneBoxL.m_min.xy;
    props["work_area_max"] = sceneBoxL.m_max.xy;

    ViewDoc* pView = pDoc->createView(-1, pScene->id());
    SketchParamMap param_map = SketchParamMap::alloc(&props);
    pView->modify(param_map);

    pViewMgr->layout()->addCell(pView->renderView());
    pViewMgr->layout()->setFocusedCell(pView->renderView());

    return pView->id();
}

extern "C" CORE_DLL_DECLSPEC long long createModelViewByFocusedWorkPlane()
{
    ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    auto pDoc = STCore::inst->docMgr()->document();
    auto pLayout = pViewMgr->layout();
    auto pView = pLayout->focusedView();

    SceneDoc* pScene = pView ? pView->doc()->scene() : pDoc->scene();

    auto pViewProps = pCfgMgr->getConfig(L"CFG_VIEW_PROPS", L"$runtime");

    return 0;
    // auto workAxis = pScene->workAxis();
    // AxisPlane viewPlane = workAxis;
    //
    // AABB sceneBox = pScene->bbox();
    // auto sceneBoxL = viewPlane.toPlaneSpace(sceneBox);
    //
    // std::wstring name;
    // if(pViewProps->has("name"))
    //     name += pViewProps->getWStr("name");
    // else if(pViewProps->get("type")._int() == Define::VIEW_2D)
    //     name += L"2d";
    // else
    //     name += L"3d";
    //
    // if(!SU::startWith(name, L'*'))
    //     name = L"*" + name;
    //
    // name = pDoc->getValidViewName(name);
    //
    // DObject props = *pViewProps;
    // props["temp"] = true;
    // props["name"] = name;
    // props["type"] = (int)Define::VIEW_2D;
    // props["proj"] = (int)Define::VIEWPROJ_ORTHO;
    // props["viewplane"] = viewPlane;
    // props["work_area_min"] = sceneBoxL.m_min.xy;
    // props["work_area_max"] = sceneBoxL.m_max.xy;
    // props["depth_up"] = STMAX(sceneBoxL.m_max.z, pViewProps->getDouble("depth_up"));
    // props["depth_down"] = STMAX(-sceneBoxL.m_min.z, pViewProps->getDouble("depth_down"));
    //
    // ViewDoc* pNewView = pDoc->createView(-1, pScene->id());
    // SketchParamMap* param_map = SketchParamMap::alloc(&props);
    // pNewView->modify(*param_map);
    // delete param_map;
    //
    // pViewMgr->layout()->addCell(pNewView->renderView());
    // pViewMgr->layout()->setFocusedCell(pNewView->renderView());
    //
    // return pNewView->id();
}

extern "C" CORE_DLL_DECLSPEC void addActiveViews(long long* ids, int nIds, BOOL addLayout)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    if(nIds == 0)
        return;

    pDoc->addActiveViews(ids, nIds);

    // STCore::inst->updateUIActiveViewList();

    if(addLayout)
    {
        std::unordered_set<LayoutCell*> cells;
        for(int i = 0; i < nIds; ++i)
        {
            View* pView = STCore::inst->viewMgr()->getView(ids[i]);
            if(!pView)
                continue;

            cells.insert(pView);
        }

        if(!cells.empty())
            STCore::inst->viewMgr()->layout()->addCells(cells);
    }
}

extern "C" CORE_DLL_DECLSPEC void removeActiveViews(long long* ids, int nIds, BOOL removeLayout)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    pDoc->removeActiveViews(ids, nIds);

    // STCore::inst->updateUIActiveViewList();

    if(removeLayout)
    {
        std::unordered_set<LayoutCell*> cells;
        for(int i = 0; i < nIds; ++i)
        {
            View* pView = STCore::inst->viewMgr()->getView(ids[i]);
            if(!pView)
                continue;

            cells.insert(pView);
        }

        if(!cells.empty())
            STCore::inst->viewMgr()->layout()->removeCells(cells);
    }
}

extern "C" CORE_DLL_DECLSPEC void hideCurObjsInFocusedView()
{
    CmdManager* pCmdMgr = STCore::inst->cmdMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    SceneDoc* pScene = pView->doc()->scene();

    // for(auto pObj: pScene->curObjs())
    //{
    //     if(!pObj->isTF())
    //         continue;
    // }

    std::unordered_set<ObjectDoc*> objs;
    // for(auto pObj: pScene->curObjs())
    //{
    //     if(!pObj->isTF())
    //         continue;
    //
    //     objs.insert(pObj);
    // }

    if(objs.empty())
        return;

    Revertable_View_Hide* pRvt = nullptr;
    if(!pCmdMgr->isRvtFreezed(pScene->rvtId()))
    {
        pCmdMgr->beginRvtQueue(pScene->rvtId());
        pRvt = new Revertable_View_Hide(pView->id());
        pRvt->setHideBefore(pView->getHideObjects());
    }

    pView->hideObjects(objs);
    // pScene->clearCur();
    pScene->updateUISelInfo();

    if(pRvt)
    {
        pRvt->setHideAfter(pView->getHideObjects());
        pCmdMgr->pushRvt(pScene->rvtId(), pRvt);
        pCmdMgr->endRvtQueue(pScene->rvtId());
    }

    pView->dirtyRT();
}

CORE_DLL_DECLSPEC void hideObjsInFocusedView(const std::unordered_set<long long>& ids)
{
    CmdManager* pCmdMgr = STCore::inst->cmdMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    SceneDoc* pScene = pView->doc()->scene();

    for(auto id: ids)
    {
        auto pObj = pScene->getTFObject(id);
        if(!pObj)
            continue;
    }

    std::unordered_set<ObjectDoc*> objs;
    for(auto id: ids)
    {
        auto pObj = pScene->getTFObject(id);
        if(!pObj)
            continue;

        objs.insert(pObj);
    }

    if(objs.empty())
        return;

    Revertable_View_Hide* pRvt = nullptr;
    if(!pCmdMgr->isRvtFreezed(pScene->rvtId()))
    {
        pCmdMgr->beginRvtQueue(pScene->rvtId());
        pRvt = new Revertable_View_Hide(pView->id());
        pRvt->setHideBefore(pView->getHideObjects());
    }

    pView->hideObjects(objs);
    // pScene->clearCur();
    pScene->updateUISelInfo();

    if(pRvt)
    {
        pRvt->setHideAfter(pView->getHideObjects());
        pCmdMgr->pushRvt(pScene->rvtId(), pRvt);
        pCmdMgr->endRvtQueue(pScene->rvtId());
    }

    pView->dirtyRT();
}

extern "C" CORE_DLL_DECLSPEC void resetFocusedViewCamPose()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return;

    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    ViewDoc* pViewDoc = pView->doc();
    // pViewDoc->modify({ {"type", (pViewDoc->type() == Define::VIEW_2D) ? Define::VIEW_3D : Define::VIEW_2D} });
    pViewDoc->resetCameraPose();
}

extern "C" CORE_DLL_DECLSPEC void switchFocusedViewType()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return;

    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return;

    ViewDoc* pViewDoc = pView->doc();
    pViewDoc->modify({ { "type", (pViewDoc->type() == Define::VIEW_2D) ? Define::VIEW_3D : Define::VIEW_2D } });
}

extern "C" CORE_DLL_DECLSPEC BOOL isFocusedViewWorkBoxVis()
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    return false;
}

extern "C" CORE_DLL_DECLSPEC BOOL setFocusedViewWorkBoxVis(BOOL b)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return false;

    // pView->setWorkBoxVis(b);
    return true;
}

extern "C" CORE_DLL_DECLSPEC void redrawFocusedViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto id: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        pCell->_view()->doc()->redraw();
        pCell->dirtyRT();
    }
}

extern "C" CORE_DLL_DECLSPEC void redrawActiveViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto& it: pLayout->cells())
    {
        auto pCell = it.second;
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        pCell->_view()->doc()->redraw();
        pCell->dirtyRT();
    }
}

extern "C" CORE_DLL_DECLSPEC void dirtyFocusedViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto id: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(id);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        pCell->dirtyRT();
    }
}

extern "C" CORE_DLL_DECLSPEC void dirtyActiveViews()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    for(auto& it: pLayout->cells())
    {
        auto pCell = it.second;
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        pCell->dirtyRT();
    }
}

extern "C" CORE_DLL_DECLSPEC void moveCameraInFocusedCell(sketch::Define::SpaceAxis axis, int pixel)
{
    auto pCell = STCore::inst->viewMgr()->layout()->focusedCell();
    if(!pCell)
        return;

    auto pDoc = STCore::inst->docMgr()->document();
    Camera* pCam = pCell->camera();

    POINT mp;
    GetCursorPos(&mp);
    ScreenToClient(STCore::inst->viewHandle(), &mp);

    RECT rcView;
    GetClientRect(STCore::inst->viewHandle(), &rcView);
    mp.y = rcView.bottom - rcView.top - mp.y;

    Vec3f pickPos, pickDir;
    pCam->getPickRay((float)mp.x, (float)mp.y, &pickPos, &pickDir);

    Planef mvpl;

    if(pCell->cellType() == Define::CELL_VIEW)
    {
        auto pView = (View*)pCell;
        auto pViewDoc = pView->doc();
        auto& clipBox = pViewDoc->clipBox();

        Planef pl0(pDoc->d2v(pViewDoc->viewPlane().o()), pViewDoc->viewPlane().normal());

        if(pViewDoc->isCamPivotValid())
        {
            Vec3f camPivot = pDoc->d2v(pViewDoc->camPivot());
            mvpl = { camPivot, -pCam->forwardDir() };
        }
        else
        {
            float t = FLT_MAX;
            pl0.intersectRay(pickPos, pickDir, &t);

            Vec3f hitPt = pickPos + pickDir * t;

            if(clipBox.contains(pDoc->v2d(hitPt)))
            {
                mvpl = { hitPt, (pCam->pos() - hitPt).normalize() };
            }
            else
            {
                Vec3 vOrigin = pDoc->v2d(pickPos);
                Vec3 vDir = pickDir;

                double t0 = pViewDoc->pickClipBoxFacingFaces(vOrigin, vDir);
                double t1 = DBL_MAX;
                pViewDoc->viewPlane().intersectRay(vOrigin, vDir, &t1);
                double t = DBL_MAX;
                if(t0 >= 0.0 && t1 >= 0.0)
                    t = STMIN(t0, t1);
                else if(t0 >= 0.0)
                    t = t0;
                else
                    t = t1;

                if(t < DBL_MAX && t > 0.0)
                {
                    mvpl.o = pDoc->d2v(vOrigin + vDir * t);
                    mvpl.n = (pCam->pos() - mvpl.o).normalize();
                }
                else
                {
                    mvpl.o = pCam->pos() + pCam->forwardDir() * 1000.0f;
                    mvpl.n = pCam->forwardDir();
                }
            }
        }
    }
    else if(pCell->cellType() == Define::CELL_DRAWING || pCell->cellType() == Define::CELL_DR_TEMPLATE)
    {
        mvpl = { Vec3f::ZERO, pCam->forwardDir() };
    }

    Vec3f pickPos1, pickDir1;
    switch(axis)
    {
    case Define::AXIS_X: pCam->getPickRay((float)(mp.x + pixel), (float)mp.y, &pickPos1, &pickDir1); break;
    case Define::AXIS_Y: pCam->getPickRay((float)mp.x, (float)(mp.y + pixel), &pickPos1, &pickDir1); break;
    }

    float t0, t1;
    if(mvpl.intersectRay(pickPos, pickDir, &t0) && mvpl.intersectRay(pickPos1, pickDir1, &t1))
    {
        auto pos0 = pickPos + pickDir * t0;
        auto pos1 = pickPos1 + pickDir1 * t1;
        pCam->translate(pos1 - pos0);
        pCell->onCameraTransform();
    }
}

extern "C" CORE_DLL_DECLSPEC void centerCameraInFocusedCell()
{
    auto pCell = STCore::inst->viewMgr()->layout()->focusedCell();
    if(!pCell)
        return;

    auto pDoc = STCore::inst->docMgr()->document();
    auto pCam = pCell->camera();
    auto& vp = pCam->viewport();

    POINT mp;
    GetCursorPos(&mp);
    ScreenToClient(STCore::inst->viewHandle(), &mp);

    RECT rcView;
    GetClientRect(STCore::inst->viewHandle(), &rcView);
    mp.y = rcView.bottom - rcView.top - mp.y;

    Vec3f pickPos, pickDir;
    pCam->getPickRay((float)mp.x, (float)mp.y, &pickPos, &pickDir);

    if(pCell->cellType() == Define::CELL_VIEW)
    {
        auto pView = (View*)pCell;
        auto pViewDoc = pView->doc();
        auto& clipBox = pViewDoc->clipBox();

        if(pCam->projType() != Camera::PROJ_PERSPECTIVE)
        {
            Planef pl = { pCam->pos(), pCam->forwardDir() };

            Vec3f midPos, midDir;// Orhto下Camera不一定处于屏幕中心
            pCam->getPickRay(vp.x + vp.z * 0.5f, vp.y + vp.w * 0.5f, &midPos, &midDir);

            float t0, t1;
            if(pl.intersectRay(pickPos, pickDir, &t0) && pl.intersectRay(midPos, midDir, &t1))
            {
                auto pos0 = pickPos + pickDir * t0;
                auto pos1 = midPos + midDir * t1;
                pCam->translate(pos0 - pos1);
                pCell->onCameraTransform();
            }

            return;
        }
        else
        {
            double tmin = DBL_MAX;
            Vec3 pickPosDoc = pDoc->v2d(pickPos);
            Vec3 pickDirDoc = pickDir;

            if(tmin < DBL_MAX)
            {
                Planef pl = { pDoc->d2v(pickPosDoc + pickDirDoc * tmin), pCam->forwardDir() };
                float t;
                if(pl.intersectRay(pCam->pos(), pCam->forwardDir(), &t))
                {
                    auto cpos = pCam->pos() + pCam->forwardDir() * t;
                    pCam->translate(pl.o - cpos);
                    pCell->onCameraTransform();
                    return;
                }
            }

            {
                Planef pl(pDoc->d2v(pViewDoc->viewPlane().o()), pViewDoc->viewPlane().normal());
                float t0, t1;
                if(pl.intersectRay(pCam->pos(), pCam->forwardDir(), &t0) && pl.intersectRay(pickPos, pickDir, &t1))
                {
                    auto pos0 = pCam->pos() + pCam->forwardDir() * t0;
                    auto pos1 = pickPos + pickDir * t1;
                    pCam->translate(pos1 - pos0);
                    pCell->onCameraTransform();
                    return;
                }
            }

            tmin = pViewDoc->pickClipBoxFacingFaces(pickPosDoc, pickDirDoc);
            if(tmin < DBL_MAX)
            {
                Planef pl = { pickPos + pickDir * (float)tmin, pCam->forwardDir() };
                float t;
                if(pl.intersectRay(pCam->pos(), pCam->forwardDir(), &t))
                {
                    auto cpos = pCam->pos() + pCam->forwardDir() * t;
                    pCam->translate(pl.o - cpos);
                    pCell->onCameraTransform();
                    return;
                }
            }
        }
    }
    else if(pCell->cellType() == Define::CELL_DRAWING || pCell->cellType() == Define::CELL_DR_TEMPLATE)
    {
        Planef pl = { pCam->pos(), pCam->forwardDir() };

        Vec3f midPos, midDir;// Orhto下Camera不一定处于屏幕中心
        pCam->getPickRay(vp.x + vp.z * 0.5f, vp.y + vp.w * 0.5f, &midPos, &midDir);

        float t0, t1;
        if(pl.intersectRay(pickPos, pickDir, &t0) && pl.intersectRay(midPos, midDir, &t1))
        {
            auto pos0 = pickPos + pickDir * t0;
            auto pos1 = midPos + midDir * t1;
            pCam->translate(pos0 - pos1);
            pCell->onCameraTransform();
        }
    }
}

extern "C" CORE_DLL_DECLSPEC void rotateCameraInFocusedCell(double anglH, double anglV)
{
    auto pCell = STCore::inst->viewMgr()->layout()->focusedCell();
    if(!pCell)
        return;

    auto pDoc = STCore::inst->docMgr()->document();
    auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();
    Camera* pCam = pCell->camera();

    if(pCell->cellType() == Define::CELL_VIEW)
    {
        if(!pOpts->viewRotEnable())
            return;

        auto pView = (View*)pCell;
        auto pViewDoc = pView->doc();
        auto& viewPlane = pViewDoc->viewPlane();
        auto& clipBox = pViewDoc->clipBox();

        Planef pl0(pDoc->d2v(viewPlane.o()), viewPlane.normal());

        Vec3f rotTarget = Vec3f::MAX;
        if(pViewDoc->isCamPivotValid())
        {
            rotTarget = pDoc->d2v(pViewDoc->camPivot());
        }
        else
        {
            Vec3f pickPos, pickDir;
            pCam->getPickRay(pCam->viewportX() + pCam->viewportWidth() * 0.5f, pCam->viewportY() + pCam->viewportHeight() * 0.5f, &pickPos, &pickDir);

            float retT = FLT_MAX;
            pl0.intersectRay(pickPos, pickDir, &retT);
            Vec3f hitPt = pickPos + pickDir * retT;

            if(clipBox.contains(pDoc->v2d(hitPt)))
            {
                rotTarget = hitPt;
            }
            else
            {
                Vec3 vOrigin = pDoc->v2d(pickPos);
                Vec3 vDir = pickDir;

                double t0 = pViewDoc->pickClipBoxFacingFaces(vOrigin, vDir);
                double t1 = DBL_MAX;
                pViewDoc->viewPlane().intersectRay(vOrigin, vDir, &t1);
                double t = DBL_MAX;
                if(t0 >= 0.0 && t1 >= 0.0)
                    t = STMIN(t0, t1);
                else if(t0 >= 0.0)
                    t = t0;
                else
                    t = t1;

                if(t < DBL_MAX && t > 0.0)
                {
                    rotTarget = pDoc->d2v(vOrigin + vDir * t);
                }
                else
                {
                    rotTarget = pDoc->d2v(viewPlane.fromVertex2D((pViewDoc->workAreaMin() + pViewDoc->workAreaMax()) * 0.5));
                }
            }
        }

        if(rotTarget != Vec3f::MAX)
        {
            if(anglH)
            {
                pCam->rotateAround(rotTarget, Vec3f::AXIS_Z, (float)angl2Rad(anglH));
            }

            if(anglV)
            {
                pCam->rotateAround(rotTarget, pCam->rightDir(), (float)angl2Rad(anglV));
            }

            if(anglH || anglV)
            {
                if(pViewDoc->type() != Define::VIEW_3D)
                    pViewDoc->setViewType(Define::VIEW_3D);

                auto euler = pCam->eulerAngl();
                pViewDoc->setEuler(euler.y, euler.x);
                pView->onCameraTransform();
            }
        }
    }
    else if(pCell->cellType() == Define::CELL_DRAWING || pCell->cellType() == Define::CELL_DR_TEMPLATE)
    {
        return;
    }
}

extern "C" CORE_DLL_DECLSPEC void scaleCameraInFocusedCell(double ratio)
{
    auto pCell = STCore::inst->viewMgr()->layout()->focusedCell();
    if(!pCell)
        return;

    auto pDoc = STCore::inst->docMgr()->document();
    auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();
    auto pCam = pCell->camera();

    POINT mp;
    GetCursorPos(&mp);
    ScreenToClient(STCore::inst->viewHandle(), &mp);

    RECT rcView;
    GetClientRect(STCore::inst->viewHandle(), &rcView);
    mp.y = rcView.bottom - rcView.top - mp.y;

    if(pCam->projType() == Camera::PROJ_PERSPECTIVE)
    {
        if(pCell->cellType() != Define::CELL_VIEW)
            return;

        Vec3 pickPos, pickDir;
        pCam->getDocPickRay(pDoc, (float)mp.x, (float)mp.y, &pickPos, &pickDir);

        auto pView = (View*)pCell;
        ratio -= 1.0;
        double tmin = DBL_MAX;

        if(pView->doc()->viewPlane().intersectRay(pickPos, pickDir, &tmin))
        {
            Vec3 scaleRay = pickPos + pickDir * tmin - pDoc->v2d(pCam->pos());
            pCam->setPos(pCam->pos() + pDoc->d2v(scaleRay * ratio));
            pCell->onCameraTransform();
            return;
        }

        tmin = pView->doc()->pickClipBoxFacingFaces(pickPos, pickDir);
        if(tmin < DBL_MAX)
        {
            Vec3 scaleRay = pickPos + pickDir * tmin - pDoc->v2d(pCam->pos());
            pCam->setPos(pCam->pos() + pDoc->d2v(scaleRay * ratio));
            pCell->onCameraTransform();
            return;
        }
    }
    else
    {
        float xOrtho = pCam->orthoLeft() + (mp.x - pCam->viewportX()) / pCam->viewportWidth() * pCam->orthoWidth();
        float yOrtho = pCam->orthoBottom() + (mp.y - pCam->viewportY()) / pCam->viewportHeight() * pCam->orthoHeight();

        // 防止精度不足导致放大之后宽高比错误的问题
        if((pCam->orthoWidth() < 1e-4 || pCam->orthoHeight() < 1e-4) && ratio < 1.0f)
        {
            return;
        }

        ratio = 2.0 - ratio;
        float left = (pCam->orthoLeft() - xOrtho) * (float)ratio + xOrtho;
        float right = (pCam->orthoRight() - xOrtho) * (float)ratio + xOrtho;
        float bottom = (pCam->orthoBottom() - yOrtho) * (float)ratio + yOrtho;
        float top = (pCam->orthoTop() - yOrtho) * (float)ratio + yOrtho;
        float w = right - left;
        float h = top - bottom;
        float cx = (left + right) * 0.5f;
        float cy = (bottom + top) * 0.5f;
        pCam->translate(pCam->rightDir() * cx + pCam->upDir() * cy);
        pCam->setOrthoView(-w * 0.5f, w * 0.5f, -h * 0.5f, h * 0.5f);
        pCell->onCameraTransform();
        return;
    }
}

extern "C" CORE_DLL_DECLSPEC void scaleViewByWorkBox()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    ViewCamera* pCam = pView->camera();

    auto& bbox = pView->doc()->clipBox();
    AABBf bboxView = pDoc->d2v(bbox);

    pCam->setPos(bboxView.center() - pCam->forwardDir());
    pCam->refineToSee(bboxView);
    pView->onCameraTransform();
}

extern "C" CORE_DLL_DECLSPEC void scaleViewByCurObjs()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    ViewCamera* pCam = pView->camera();

    // auto& curObjs = pScene->curObjs();
    // if(curObjs.empty())
    //     return;

    // for(auto pObj: curObjs)
    //{
    //     if(!pObj->isTF())
    //         continue;
    // }

    AABB bbox;
    // for(auto pObj: curObjs)
    //{
    //     if(!pObj->isTF())
    //         continue;
    //
    //     bbox.add(pObj->_tfObject()->bbox());
    // }

    if(!bbox.isValid())
        return;

    bbox.inflate(STMAX(0.0, (300.0 - bbox.sizeX()) * 0.5), STMAX(0.0, (300.0 - bbox.sizeY()) * 0.5), STMAX(0.0, (300.0 - bbox.sizeZ()) * 0.5));

    AABBf bboxView = pDoc->d2v(bbox);

    pCam->setPos(bboxView.center() - pCam->forwardDir());
    pCam->refineToSee(bboxView);
    pView->onCameraTransform();
}

extern "C" CORE_DLL_DECLSPEC void sketch_set_camera(const std::array<float, 16>& matrix, float fov)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    auto view = pDoc->GetFirstView()->renderView();// pDoc->getView(0)->renderView();
    auto camera = view->camera();
    camera->setMatrix({ matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10],
                        matrix[11], matrix[12], matrix[13], matrix[14], matrix[15] });
    camera->setFov(fov);
}
