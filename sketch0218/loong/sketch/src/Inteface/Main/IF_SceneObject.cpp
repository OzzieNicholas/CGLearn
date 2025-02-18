#include "sketch/Interface/Main/IF_SceneObject.h"

#include <Windows.h>
#include <stack>

#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC long long objModifyCounter(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getObject(objId);
        return pObj ? pObj->modifyCounter() : -1LL;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return -1LL;

    auto pObj = pScene->getObject(objId);
    return pObj ? pObj->modifyCounter() : -1LL;
}

extern "C" CORE_DLL_DECLSPEC BOOL objDisposed(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getObject(objId);
        return !pObj || pObj->isDisposed();
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return false;

    auto pObj = pScene->getObject(objId);
    return !pObj || pObj->isDisposed();
}

extern "C" CORE_DLL_DECLSPEC long long objScene(long long objId) { return SceneDoc::sceneId(objId); }

extern "C" CORE_DLL_DECLSPEC sketch::Define::ObjectType objType(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getObject(objId);
        return pObj ? pObj->type() : Define::OBJ_UNKNOWN;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Define::OBJ_UNKNOWN;

    auto pObj = pScene->getObject(objId);
    return pObj ? pObj->type() : Define::OBJ_UNKNOWN;
}

CORE_DLL_DECLSPEC const Engine::Guid& objGuid(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return Guid::EMPTY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Guid::EMPTY;

    auto pObj = pScene->getTFObject(objId);
    return Guid::EMPTY;
}

CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixLocal(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->local_matrix() : Mat43::IDENTITY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->local_matrix() : Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixObj(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->local_matrix() : Mat43::IDENTITY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->local_matrix() : Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC const Engine::Mat43 objMatrixObjW(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->world_matrix() : Mat43::IDENTITY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->world_matrix() : Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixWorld(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->parent_matrix() : Mat43::IDENTITY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->parent_matrix() : Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixParent(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->parent_matrix() : Mat43::IDENTITY;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->parent_matrix() : Mat43::IDENTITY;
}

extern "C" CORE_DLL_DECLSPEC uint16_t objFlags(long long sceneId, long long objId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(sceneId == 0)
    {
        auto pObj = pDoc->scene()->getTFObject(objId);
        return pObj ? pObj->flags() : 0u;
    }

    auto pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return 0u;

    auto pObj = pDoc->getTFObject(objId);
    return pObj ? pObj->flags() : 0u;
}

CORE_DLL_DECLSPEC bool objViewProps(long long objId, Engine::DObject& viewProps)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    ObjectDoc* pObj = pDoc->getObject(objId);
    if(!pObj)
        return false;

    return false;
}

CORE_DLL_DECLSPEC BOOL queryObjProps(long long objId, Engine::DObject& objProps)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ObjectDoc* pObj = pDoc->getObject(objId);
    if(!pObj)
        return false;

    // pObj->getProps(objProps, false);
    return true;
}

CORE_DLL_DECLSPEC bool curObjViewProps(Engine::DObject& viewProps)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();

    // auto curObjs = pScene->topLevelObjs(pScene->curObjs());
    // if(curObjs.empty())
    //     return false;

    //// 优先挑选非Grid物体
    // for(auto pObj: curObjs)
    //{
    //     pObj->getWorkProps(pView->doc()->scene()->workAxis(), viewProps, false);
    //     return true;
    // }

    // ObjectDoc* pObj = *curObjs.begin();
    // pObj->getWorkProps(pView->doc()->scene()->workAxis(), viewProps, false);
    return true;
}

CORE_DLL_DECLSPEC BOOL queryCurObjProps(Engine::DObject& objProps)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();

    // auto curObjs = pScene->topLevelObjs(pScene->curObjs());
    // if(curObjs.empty())
    //     return false;

    // 优先挑选非Grid物体
    // for(auto pObj: curObjs)
    //{
    //    // pObj->getProps(objProps, false);
    //    return true;
    //}

    // ObjectDoc* pObj = *curObjs.begin();
    //  pObj->getProps(objProps, false);
    return true;
}

CORE_DLL_DECLSPEC bool modifyObject(long long objId, const Engine::DObject& props)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    auto pObj = pDoc->getObject(objId);
    if(!pObj)
        return false;

    SceneDoc* pScene = pObj->scene();

    // 修改属性
    RvtBlock blk(pScene->rvtId());
    // pObj->modify(props);

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return true;
}

CORE_DLL_DECLSPEC bool modifyObjects(const std::vector<long long>& objIds, const Engine::DObject& props)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(objIds.empty())
        return true;

    SceneDoc* pScene = pDoc->getSceneByObj(objIds[0]);

    // 修改属性
    RvtBlock blk(pScene->rvtId());
    std::vector<TFObjectDoc*> objWithGuids;
    for(auto id: objIds)
    {
        auto pObj = pScene->getObject(id);
        if(!pObj)
            continue;

        // pObj->modify(props);
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return true;
}

CORE_DLL_DECLSPEC BOOL setObjectGUID(long long objId, const Engine::Guid& guid)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pObj = pDoc->getTFObject(objId);
    if(!pObj)
        return false;

    // pObj->setGUID(guid);
    return true;
}

CORE_DLL_DECLSPEC BOOL setCurObjectGUID(const Engine::Guid& guid)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();
    auto pView = pLayout->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->scene()->doc();

    std::vector<TFObjectDoc*> objWithGuids;

    if(objWithGuids.size() != 1)
        return false;

    auto pObj = objWithGuids.front();

    // pObj->setGUID(guid);
    return true;
}

CORE_DLL_DECLSPEC bool modifyCurObjects(const Engine::DObject& props, Define::ObjectType objType)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();
    auto pView = pLayout->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->scene()->doc();

    // 修改属性
    RvtBlock blk(pScene->rvtId());
    /*for(auto pObj: pScene->curObjs())
    {
        if(pObj->type() != objType)
            continue;

        pObj->modify(props);
    }*/

    if(UIFuncs::setWarning)
    {
        UIFuncs::setWarning(L"");
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return true;
}

CORE_DLL_DECLSPEC bool modifyCurObjects(const Engine::DObject& props, Define::ObjectType objType0, Define::ObjectType objType1)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();
    auto pView = pLayout->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->scene()->doc();

    // 修改属性
    RvtBlock blk(pScene->rvtId());

    // for(auto pObj: pScene->curObjs())
    //{
    //     if(pObj->type() != objType0 && pObj->type() != objType1)
    //         continue;

    //    pObj->modify(props);
    //}

    if(UIFuncs::setWarning)
    {
        UIFuncs::setWarning(L"");
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return true;
}

CORE_DLL_DECLSPEC bool modifyCurObjectsByBaseType(const Engine::DObject& props, Define::ObjectType objTypeBegin, Define::ObjectType objTypeEnd)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();
    auto pView = pLayout->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->scene()->doc();

    // 修改属性
    RvtBlock blk(pScene->rvtId());

    // for(auto pObj: pScene->curObjs())
    //{
    //     if(pObj->type() < objTypeBegin || pObj->type() > objTypeEnd)
    //         continue;

    //    pObj->modify(props);
    //}

    if(UIFuncs::setWarning)
    {
        UIFuncs::setWarning(L"");
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return true;
}

CORE_DLL_DECLSPEC long long createObject(long long sceneId, sketch::Define::ObjectType objType, const Engine::DObject& props)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    SceneDoc* pSceneDoc = pDoc->getScene(sceneId);
    if(!pSceneDoc)
    {
        View* pView = pViewMgr->layout()->focusedView();
        if(pView == nullptr)
            return -1;

        pSceneDoc = pView->scene()->doc();
    }

    ObjectDoc* pObj = pSceneDoc->createObject(objType);
    if(!pObj)
        return -1;

        // pObj->modify(props);

#if 0
	if (!pObj->isValid()) {
		pSceneDoc->deleteObject(pObj->id());
		return -1;
	}
#endif

    STCore::inst->updateUIObjList();
    return pObj->id();
}

CORE_DLL_DECLSPEC const Engine::Vec3& helperPointGlobalPos(long long id) { return Vec3::ZERO; }

CORE_DLL_DECLSPEC const Engine::Vec3 helperPointWorkPos(long long id) { return Vec3::ZERO; }

CORE_DLL_DECLSPEC const Engine::Vec3& curHelperPointGlobalPos() { return Vec3::ZERO; }

CORE_DLL_DECLSPEC const Engine::Vec3 curHelperPointWorkPos() { return Vec3::ZERO; }

CORE_DLL_DECLSPEC BOOL setHelperPointWorkPos(long long id, const Engine::Vec3& pos) { return true; }

CORE_DLL_DECLSPEC BOOL setHelperPointGlobalPos(long long id, const Engine::Vec3& pos) { return true; }

CORE_DLL_DECLSPEC BOOL setCurHelperPointsWorkPos(const Engine::Vec3& pos) { return true; }

CORE_DLL_DECLSPEC BOOL setCurHelperPointsGlobalPos(const Engine::Vec3& pos) { return true; }

CORE_DLL_DECLSPEC BOOL modifyObjHandler(long long objId, long long hid, const Engine::DObject& props) { return true; }
