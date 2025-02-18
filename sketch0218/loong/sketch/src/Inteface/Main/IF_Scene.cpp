#include "sketch/STCore.h"

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
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_SceneObject.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC long long focusedScene()
{
    auto pView = STCore::inst->viewMgr()->layout()->focusedView();
    return pView ? pView->doc()->scene()->id() : -1LL;
}

CORE_DLL_DECLSPEC void getCurObjIds(long long sceneId, std::vector<long long>& ids)
{
    auto pDoc = STCore::inst->docMgr()->document();
    if(!pDoc)
        return;

    SceneDoc* pScene = pDoc->getScene(sceneId);
    if(!pScene)
        return;

    // auto& curObjs = pScene->curObjs();
    // if (!curObjs.empty())
    //	ids.reserve(curObjs.size());
    //
    // for (auto pObj : curObjs)
    //	ids.push_back(pObj->id());
}

CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids)
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& curObjs = pScene->curObjs();
    // if (!curObjs.empty())
    //	ids.reserve(curObjs.size());
    //
    // for (auto pObj : curObjs)
    //{
    //	ids.push_back(pObj->id());
    // }
}

CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids, Define::ObjectType objType)
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& curObjs = pScene->curObjs();
    // if (!curObjs.empty())
    //	ids.reserve(curObjs.size());
    //
    // for (auto pObj : curObjs)
    //{
    //	if (pObj->type() == objType)
    //		ids.push_back(pObj->id());
    // }
}

CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids, Define::ObjectType objType0, Define::ObjectType objType1)
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& curObjs = pScene->curObjs();
    // if (!curObjs.empty())
    //	ids.reserve(curObjs.size());
    //
    // for (auto pObj : curObjs)
    //{
    //	if (pObj->type() == objType0
    //		|| pObj->type() == objType1)
    //		ids.push_back(pObj->id());
    // }
}

CORE_DLL_DECLSPEC void getCurObjIdsByBaseType(std::vector<long long>& ids, Define::ObjectType typeBegin, Define::ObjectType typeEnd)
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& curObjs = pScene->curObjs();
    // if (!curObjs.empty())
    //	ids.reserve(curObjs.size());
    //
    // for (auto pObj : curObjs)
    //{
    //	if (pObj->type() >= typeBegin && pObj->type() <= typeEnd)
    //		ids.push_back(pObj->id());
    // }
}

extern "C" CORE_DLL_DECLSPEC sketch::Define::ObjectType getCurObjsUnifiedType()
{
    sketch::View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return sketch::Define::OBJ_UNKNOWN;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& curObjs = pScene->curObjs();
    // if (curObjs.empty())
    //	return sketch::Define::OBJ_UNKNOWN;
    //
    // Define::ObjectType type = (*curObjs.begin())->type();
    // for (auto pObj : curObjs)
    //{
    //	if (type != pObj->type())
    //		return Define::OBJ_UNKNOWN;
    // }
    return Define::OBJ_UNKNOWN;
}

extern "C" CORE_DLL_DECLSPEC void setSceneCur(long long id)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    SceneDoc* pScene = pDoc->getSceneByObj(id);
    if(!pScene)
        return;

    // pScene->setCur(id);
    pScene->updateUISelInfo();
    pLayout->dirtyCells();
}

extern "C" CORE_DLL_DECLSPEC void setSceneCurs(long long* ids, int nIds)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();

    if(nIds <= 0)
        return;

    SceneDoc* pScene = pDoc->getSceneByObj(ids[0]);
    if(!pScene)
        return;

    // pScene->setCur(std::unordered_set<long long>{ids, ids + nIds});
    pScene->updateUISelInfo();
    pLayout->dirtyCells();
}

CORE_DLL_DECLSPEC const Engine::Mat43& focusedWorkAxis()
{
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return Mat43::IDENTITY;

    SceneDoc* pScene = pView->doc()->scene();
    return Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC const Engine::Mat43& workAxis(long long sceneId)
{
    SceneDoc* pScene = STCore::inst->docMgr()->document()->getScene(sceneId);
    if(!pScene)
        return Mat43::IDENTITY;

    return Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC void setFocusedWorkAxis(const Engine::Mat43& m)
{
    auto pLayout = STCore::inst->viewMgr()->layout();
    View* pView = pLayout->focusedView();
    if(!pView)
        return;

    SceneDoc* pSceneDoc = pView->doc()->scene();
    // pSceneDoc->setWorkAxis(m);
    pLayout->dirtyCells();
}

CORE_DLL_DECLSPEC void setWorkAxis(long long sceneId, const Engine::Mat43& m)
{
    auto pLayout = STCore::inst->viewMgr()->layout();
    SceneDoc* pSceneDoc = STCore::inst->docMgr()->document()->getScene(sceneId);
    if(!pSceneDoc)
        return;

    // pSceneDoc->setWorkAxis(m);
    pLayout->dirtyCells();
}

CORE_DLL_DECLSPEC BOOL setCurObjs(const std::vector<long long>& objIds)
{
    auto pDoc = STCore::inst->docMgr()->document();
    auto pLayout = STCore::inst->viewMgr()->layout();
    auto pView = pLayout->focusedView();
    auto pScene = pView ? pView->doc()->scene() : STCore::inst->docMgr()->document()->scene();

    if(objIds.empty())
    {
        // pScene->clearCur();
        pLayout->dirtyCells();
        return true;
    }

    pScene = pDoc->getSceneByObj(objIds[0]);
    if(!pScene)
        return false;

    // pScene->pushCurToUndo();
    // pScene->setCur(std::unordered_set<long long>{objIds.begin(), objIds.end()});
    pScene->updateUISelInfo();
    pLayout->dirtyCells();
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL setCurObjsToFocusedViewVisObjs()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();

    std::unordered_set<long long> curIds;
    auto& viewObjs = pView->visObjects();
    for(auto pObj: viewObjs)
    {
        if(!pObj->doc())
            continue;

        auto pTFObj = pObj->doc()->_tfObject();
        if(!pTFObj)
            continue;

        // 忽略子部件
        if(pTFObj->parent() && viewObjs.find(pTFObj->parent()->renderObject()) != viewObjs.end())
            continue;

        curIds.insert(pTFObj->id());
    }

    // pScene->pushCurToUndo();
    // pScene->setCur(curIds);
    pScene->updateUISelInfo();
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL deleteCurObjs()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();

    std::vector<long long> curIds;
    std::unordered_set<int> objTypes;
    bool bRmvGrid = true;

    // for (auto pObj : pScene->curObjs())
    //{
    //	curIds.push_back(pObj->id());
    //
    //	bRmvGrid = false;
    //
    //	objTypes.insert(pObj->type());
    // }

    RvtBlock blk(pScene->rvtId());

    bool bAllCpt = true;
    std::unordered_set<long long> preBuilts;
    std::unordered_set<long long> owners;
    for(auto id: curIds)
    {
        ObjectDoc* pObj = pScene->getObject(id);
        if(!pObj)
            continue;

        if(pObj->isTF())
            bAllCpt = false;

        pScene->deleteObject(pObj->id());
    }

    // if (bAllCpt && !owners.empty())
    //	pScene->setCur(owners);

    pScene->updateUISelInfo();
    STCore::inst->updateUIObjList();
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL deleteObjs(const std::vector<long long>& objIds)
{
    //
    Document* pDoc = STCore::inst->docMgr()->document();

    // 根据场景分组目标对象
    std::unordered_map<SceneDoc*, std::unordered_set<long long>> objGroup;
    std::unordered_set<int> objTypes;
    for(auto id: objIds)
    {
        ObjectDoc* pObj = pDoc->getObject(id);
        if(pObj == nullptr)
            continue;
        objGroup[pObj->scene()].insert(pObj->id());
        objTypes.insert(pObj->type());
    }

    // 依次删除每个场景中的目标对象
    for(auto& itt: objGroup)
    {
        auto pScene = itt.first;
        std::unordered_set<long long> dirtyPrebuilts;

        //
        for(auto objId: itt.second)
        {
            auto pObj = pScene->getObject(objId);
            if(pObj == nullptr)
                continue;

            pScene->deleteObject(pObj->id());
        }

        pScene->updateUISelInfo();
    }

    STCore::inst->updateUIObjList();
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL undoCurObjs()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();
    // pScene->undoCur();
    STCore::inst->viewMgr()->layout()->dirtyCells();
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL redoCurObjs()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(!pView)
        return false;

    SceneDoc* pScene = pView->doc()->scene();
    // pScene->redoCur();
    STCore::inst->viewMgr()->layout()->dirtyCells();
    return true;
}
