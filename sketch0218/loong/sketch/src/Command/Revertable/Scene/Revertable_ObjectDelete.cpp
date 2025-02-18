#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"

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
OBJ_MEM_POOL_IMPL(Revertable_ObjectDelete, 500);

Revertable_ObjectDelete::Revertable_ObjectDelete(Define::ObjectType objType, long long objId)
    : Revertable(Define::RVT_OBJ_DELETE), m_objId(objId), m_objType(objType)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ObjectDoc* pObj = pDoc->getObject(m_objId);
    pObj->get_props(m_undo_props);
}

Revertable_ObjectDelete::~Revertable_ObjectDelete(void) {}

bool Revertable_ObjectDelete::undo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    SceneDoc* pScene = pDoc->getSceneByObj(m_objId);
    if(!pScene)
        return false;

    ObjectDoc* pObj = pScene->createObject(m_objType, m_objId);
    pObj->set_props(m_undo_props);
    m_undo_props.clear();
    return true;
}

bool Revertable_ObjectDelete::redo()
{
    Document* pDoc = STCore::inst->docMgr()->document();

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }

    pObj->get_props(m_undo_props);
    pDoc->deleteObject(m_objId);
    return true;
}

}// namespace sketch