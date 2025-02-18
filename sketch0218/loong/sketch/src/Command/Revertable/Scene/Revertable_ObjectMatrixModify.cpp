#include "sketch/Command/Revertable/Scene/Revertable_ObjectMatrixModify.h"

#include "Engine/Util/DObject.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
OBJ_MEM_POOL_IMPL(Revertable_ObjectMatrixModify, 500);

Revertable_ObjectMatrixModify::Revertable_ObjectMatrixModify(long long objId) : Revertable(Define::RVT_OBJ_MATRIX_MODIFY), m_objId(objId) {}

Revertable_ObjectMatrixModify::~Revertable_ObjectMatrixModify(void) {}

bool Revertable_ObjectMatrixModify::isValid() { return m_matBefore != m_matAfter; }

bool Revertable_ObjectMatrixModify::undo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    TFObjectDoc* pObj = pDoc->getTFObject(m_objId);
    if(pObj == nullptr)
        return false;

    pObj->set_matrix_from_local(m_matBefore);

    return true;
}

bool Revertable_ObjectMatrixModify::redo()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return false;

    TFObjectDoc* pObj = pDoc->getTFObject(m_objId);
    if(pObj == nullptr)
        return false;

    pObj->set_matrix_from_local(m_matAfter);

    return true;
}

void Revertable_ObjectMatrixModify::setMatBefore(const Engine::Mat43& m) { m_matBefore = m; }

void Revertable_ObjectMatrixModify::setMatAfter(const Engine::Mat43& m) { m_matAfter = m; }
}// namespace sketch