#include "sketch/Command/Revertable/Scene/Revertable_ObjectParentModify.h"

#include "Engine/Util/DObject.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
OBJ_MEM_POOL_IMPL(Revertable_ObjectParentModify, 500);

Revertable_ObjectParentModify::Revertable_ObjectParentModify(long long objId, long long parBefore, long long parAfter)
    : Revertable(Define::RVT_OBJ_PARENT_MODIFY), m_objId(objId), m_parBefore(parBefore), m_parAfter(parAfter)
{
}

Revertable_ObjectParentModify::~Revertable_ObjectParentModify(void) {}

bool Revertable_ObjectParentModify::undo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }
    m_parAfter = -1LL;
    GroupDoc* current_parent = pObj->_tfObject()->parent()->_group();
    if(current_parent)
    {
        current_parent->removeChild(pObj->_tfObject());
        m_parAfter = current_parent->id();
    }

    GroupDoc* before_parent = pDoc->getObject(m_parBefore)->_group();
    if(before_parent)
    {
        pObj->_tfObject()->set_parent(before_parent->_tfObject());
        before_parent->addChild(pObj->_tfObject());
    }
    else
    {
        pObj->_tfObject()->set_parent(nullptr);
    }

    return true;
}

bool Revertable_ObjectParentModify::redo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    ObjectDoc* pObj = pDoc->getObject(m_objId);
    if(!pObj)
    {
        return false;
    }
    m_parBefore = -1LL;
    GroupDoc* current_parent = pObj->_tfObject()->parent()->_group();
    if(current_parent)
    {
        current_parent->removeChild(pObj->_tfObject());
        m_parBefore = current_parent->id();
    }

    GroupDoc* after_parent = pDoc->getObject(m_parAfter)->_group();
    if(after_parent)
    {
        pObj->_tfObject()->set_parent(after_parent->_tfObject());
        after_parent->addChild(pObj->_tfObject());
    }
    else
    {
        pObj->_tfObject()->set_parent(nullptr);
    }

    return true;
}

}// namespace sketch