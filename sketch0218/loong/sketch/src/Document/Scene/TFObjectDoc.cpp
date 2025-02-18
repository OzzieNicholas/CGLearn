#include "sketch/Document/Scene/TFObjectDoc.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectMatrixModify.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectParentModify.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/STCore.h"

#include "sketch/NousEngine/NousConvert.h"

using namespace Engine;

namespace sketch
{
TFObjectDoc::TFObjectDoc(SceneDoc* pScene, Define::ObjectType type, long long id) : ObjectDoc(pScene, type, id) {}

TFObjectDoc::~TFObjectDoc() {}

void TFObjectDoc::dirty() {}

void TFObjectDoc::record() {}

void TFObjectDoc::get_props(SketchParamMap& props)
{
    ObjectDoc::get_props(props);

    this->_update_matrix();
    props.set_value("mat", NousConvert::convert(m_matrix));
    props.set_value("world_mat", NousConvert::convert(m_world_matrix));
    props.set_value("world_mat_inv", NousConvert::convert(m_world_matrix_inv));
    props.set_value("parent_mat", NousConvert::convert(m_parent_matrix));
    props.set_value("parent_mat_inv", NousConvert::convert(m_parent_matrix_inv));
}

void TFObjectDoc::set_props(const SketchParamMap& props)
{
    ObjectDoc::set_props(props);
    nous::mat4 matrix4x4;
    props.get_value("mat", matrix4x4);
    m_matrix = NousConvert::convert(matrix4x4);
    props.get_value("world_mat", matrix4x4);
    m_world_matrix = NousConvert::convert(matrix4x4);
    props.get_value("world_mat_inv", matrix4x4);
    m_world_matrix_inv = NousConvert::convert(matrix4x4);
    props.get_value("parent_mat", matrix4x4);
    m_parent_matrix = NousConvert::convert(matrix4x4);
    props.get_value("parent_mat_inv", matrix4x4);
    m_parent_matrix_inv = NousConvert::convert(matrix4x4);

    this->transform_change();
}

inline void TFObjectDoc::copy_from(TFObjectDoc* obj)
{
    m_matrix = obj->m_matrix;
    m_world_matrix = obj->m_world_matrix;
    m_world_matrix_inv = obj->m_world_matrix_inv;
    m_parent_matrix = obj->m_parent_matrix;
    m_parent_matrix_inv = obj->m_parent_matrix_inv;

    this->transform_change();
}

void TFObjectDoc::set_matrix_from_local(const Engine::Mat43& m)
{
    m_matrix = m;

    m_dirtyFlags |= DIRTY_BIT_MATLW_BBOX;

    this->transform_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void TFObjectDoc::set_matrix_from_world(const Engine::Mat43& m)
{
    m_matrix = ((this->parent() == nullptr) ? m : this->parent()->world_matrix_inv() * m);

    m_dirtyFlags |= DIRTY_BIT_MATLW_BBOX;

    this->transform_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

Engine::Mat43 TFObjectDoc::world_matrix()
{
    if(m_dirtyFlags & DIRTY_BIT_MATW)
    {
        this->_update_matrix();
        m_dirtyFlags &= ~DIRTY_BIT_MATW;
    }

    return m_world_matrix;
}

Engine::Mat43 TFObjectDoc::world_matrix_inv()
{
    if(m_dirtyFlags & DIRTY_BIT_MATW)
    {
        this->_update_matrix();
        m_dirtyFlags &= ~DIRTY_BIT_MATW;
    }

    return m_world_matrix_inv;
}

const Engine::Mat43& TFObjectDoc::parent_matrix()
{
    if(m_dirtyFlags & DIRTY_BIT_MATW)
    {
        this->_update_matrix();
        m_dirtyFlags &= ~DIRTY_BIT_MATW;
    }

    return m_parent_matrix;
}

const Engine::Mat43& TFObjectDoc::parent_matrix_inv()
{
    if(m_dirtyFlags & DIRTY_BIT_MATW)
    {
        this->_update_matrix();
        m_dirtyFlags &= ~DIRTY_BIT_MATW;
    }

    return m_parent_matrix_inv;
}

bool TFObjectDoc::contains(long long id, bool recursive) const { return this->contains(m_pScene->getTFObject(id), recursive); }

bool TFObjectDoc::contains(ObjectDoc* pObj, bool recursive) const
{
    if(!pObj || !pObj->isTF())
        return false;

    TFObjectDoc* pCur = pObj->_tfObject();
    if(!recursive)
        return pCur->parent() == this;

    while(pCur)
    {
        if(pCur->parent() == this)
            return true;

        pCur = pCur->parent();
    }

    return false;
}

void TFObjectDoc::bbox_change()
{
    m_dirtyFlags |= DIRTY_BIT_BBOX;
    if(m_parent)
    {
        m_parent->bbox_change();
    }
}

void TFObjectDoc::transform_change()
{
    m_dirtyFlags |= DIRTY_BIT_MATLW_BBOX;

    for(auto child: m_children)
    {
        if(!child->isDisposed())
            child->transform_change();
    }
    this->bbox_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void TFObjectDoc::set_parent(TFObjectDoc* pPar)
{
    CmdManager* pCmdMgr = STCore::inst->cmdMgr();
    if(!pCmdMgr->isRvtFreezed(m_pScene->rvtId()) && this->isRevertable())
    {
        pCmdMgr->pushRvt(m_pScene->rvtId(), new Revertable_ObjectParentModify(m_id, m_parent ? m_parent->id() : -1LL, pPar ? pPar->id() : -1LL));
    }

    Engine::Mat43 world_matrix = this->world_matrix();
    m_parent = pPar;
    this->set_matrix_from_world(world_matrix);
    this->transform_change();
}

void TFObjectDoc::_updateBBox()
{
    // do nothing
}

void TFObjectDoc::_update_matrix()
{
    if(m_parent)
    {
        m_parent_matrix = m_parent->world_matrix();
        m_parent_matrix_inv = m_parent_matrix.inverse();
    }
    else
    {
        m_parent_matrix = Engine::Mat43::IDENTITY;
        m_parent_matrix_inv = Engine::Mat43::IDENTITY;
    }

    m_world_matrix = m_parent_matrix * this->local_matrix();
    m_world_matrix_inv = m_world_matrix.inverse();
}

Engine::OBB& TFObjectDoc::bbox()
{
    if(m_dirtyFlags & DIRTY_BIT_BBOX)
    {
        this->_updateBBox();
        m_dirtyFlags &= ~DIRTY_BIT_BBOX;
    }

    return m_bbox;
}

}// namespace sketch