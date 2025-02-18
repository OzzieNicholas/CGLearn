#include "sketch/Command/Revertable/View/Revertable_View_Hide.h"

#include "Engine/Util/DObject.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/ObjectDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
Revertable_View_Hide::Revertable_View_Hide(long long viewId) : Revertable(Define::RVT_VIEW_HIDE), m_viewId(viewId) {}

Revertable_View_Hide::~Revertable_View_Hide(void) {}

bool Revertable_View_Hide::undo()
{
    auto pViewMgr = STCore::inst->viewMgr();
    auto pView = pViewMgr->getView(m_viewId);
    if(!pView)
        return false;

    auto pScene = pView->scene()->doc();

    pScene->updateUISelInfo();

    std::unordered_set<ObjectDoc*> hideObjs;
    for(auto id: m_hideIdsBefore)
    {
        auto pObj = pScene->getObject(id);
        if(!pObj)
            continue;
        hideObjs.insert(pObj);
    }

    pView->setHideObjects(hideObjs);
    pView->dirtyRT();

    return true;
}

bool Revertable_View_Hide::redo()
{
    auto pViewMgr = STCore::inst->viewMgr();
    auto pView = pViewMgr->getView(m_viewId);
    if(!pView)
        return false;

    auto pScene = pView->scene()->doc();

    pScene->updateUISelInfo();

    std::unordered_set<ObjectDoc*> hideObjs;
    for(auto id: m_hideIdsAfter)
    {
        auto pObj = pScene->getObject(id);
        if(!pObj)
            continue;
        hideObjs.insert(pObj);
    }

    pView->setHideObjects(hideObjs);
    pView->dirtyRT();

    return true;
}

void Revertable_View_Hide::setHideBefore(const std::unordered_set<RenderObject*>& hideObjsBefore)
{
    m_hideIdsBefore.clear();
    for(auto pObj: hideObjsBefore)
    {
        if(!pObj)
            continue;
        m_hideIdsBefore.insert(pObj->id());
    }
}

void Revertable_View_Hide::setHideAfter(const std::unordered_set<RenderObject*>& hideObjsAfter)
{
    m_hideIdsAfter.clear();
    for(auto pObj: hideObjsAfter)
    {
        if(!pObj)
            continue;
        m_hideIdsAfter.insert(pObj->id());
    }
}

}// namespace sketch