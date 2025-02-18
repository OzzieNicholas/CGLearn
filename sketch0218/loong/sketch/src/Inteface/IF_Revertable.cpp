#include "sketch/Interface/IF_Revertable.h"

#include "sketch/STCore.h"

#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
#include <Windows.h>

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC void undoFocusedCell()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    auto pCell = pViewMgr->layout()->focusedCell();
    if(!pCell)
        return;

    switch(pCell->cellType())
    {
    case Define::CELL_VIEW: STCore::inst->cmdMgr()->undo(((View*)pCell)->scene()->doc()->rvtId()); break;
    }
}

extern "C" CORE_DLL_DECLSPEC void undoPool(long long poolId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    STCore::inst->cmdMgr()->undo(poolId);
}

extern "C" CORE_DLL_DECLSPEC void undoScene(long long sceneId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    SceneDoc* pScene = pDoc->getScene(sceneId);
    if(pScene)
        STCore::inst->cmdMgr()->undo(pScene->rvtId());
}

extern "C" CORE_DLL_DECLSPEC void redoFocusedCell()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    auto pCell = pViewMgr->layout()->focusedCell();
    if(!pCell)
        return;

    switch(pCell->cellType())
    {
    case Define::CELL_VIEW: STCore::inst->cmdMgr()->redo(((View*)pCell)->scene()->doc()->rvtId()); break;
    }
}

extern "C" CORE_DLL_DECLSPEC void redoPool(long long poolId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    STCore::inst->cmdMgr()->redo(poolId);
}

extern "C" CORE_DLL_DECLSPEC void redoScene(long long sceneId)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    SceneDoc* pScene = pDoc->getScene(sceneId);
    if(pScene)
        STCore::inst->cmdMgr()->redo(pScene->rvtId());
}