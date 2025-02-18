#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"

#include "Engine/Util/DObject.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/ObjectDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
OBJ_MEM_POOL_IMPL(Revertable_ObjectCreate, 500);

Revertable_ObjectCreate::Revertable_ObjectCreate(long long objId, Define::ObjectType objType)
    : Revertable(Define::RVT_OBJ_CREATE), m_objId(objId), m_objType(objType)
{
}

Revertable_ObjectCreate::~Revertable_ObjectCreate(void) {}

bool Revertable_ObjectCreate::undo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }

    pObj->get_props(m_redo_props);
    pDoc->deleteObject(m_objId);
    return true;
}

bool Revertable_ObjectCreate::redo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    SceneDoc* pScene = pDoc->getSceneByObj(m_objId);
    if(!pScene)
        return false;

    // if (pScene->getObject(m_objId))
    //	return false;

    auto pObj = pScene->createObject(m_objType, m_objId);
    pObj->set_props(m_redo_props);
    m_redo_props.clear();

    return true;
}

}// namespace sketch