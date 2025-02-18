#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"

#include "Engine/Util/DObject.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/ObjectDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"

using namespace Engine;

namespace sketch
{
OBJ_MEM_POOL_IMPL(Revertable_ObjectModify, 500);

Revertable_ObjectModify::Revertable_ObjectModify(ObjectDoc* obj_doc) : Revertable(Define::RVT_OBJ_MODIFY), m_objId(obj_doc->id())
{
    obj_doc->get_props(m_undo_props);
}

Revertable_ObjectModify::~Revertable_ObjectModify() {}

bool Revertable_ObjectModify::undo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(!pDoc)
    {
        return false;
    }

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }

    pObj->get_props(m_redo_props);
    pObj->set_props(m_undo_props);
    m_undo_props.clear();

    return true;
}

bool Revertable_ObjectModify::redo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(!pDoc)
    {
        return false;
    }

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }

    pObj->get_props(m_undo_props);
    pObj->set_props(m_redo_props);
    m_redo_props.clear();

    return true;
}
}// namespace sketch
