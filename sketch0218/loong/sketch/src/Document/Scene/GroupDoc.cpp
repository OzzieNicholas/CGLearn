#include "sketch/Document/Scene/GroupDoc.h"

#include "Engine/Util/DObject.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/STCore.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"

#include "sketch/NousEngine/NousConvert.h"

using namespace Engine;

namespace sketch
{
GroupDoc::GroupDoc(SceneDoc* pScene, long long id) : TFObjectDoc(pScene, Define::OBJ_D5_GROUP, id) {}

GroupDoc::~GroupDoc() {}

void GroupDoc::dirty()
{
    this->transform_change();
    m_pScene->invokeListener(Define::MSG_OBJ_UPDATE, DObject::alloc({ { "obj_id", this->id() } }));
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void GroupDoc::record()
{
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    if(!cmd_mgr->isRvtFreezed(m_pScene->rvtId()) && this->isRevertable())
    {
        cmd_mgr->pushRvt(m_pScene->rvtId(), new Revertable_ObjectModify(this));
    }
}

void GroupDoc::get_props(SketchParamMap& props)
{
    TFObjectDoc::get_props(props);

    std::vector<long long> children_id;
    for(auto child: m_children)
    {
        children_id.push_back(child->id());
    }
    props.set_value("children_id", children_id);
}

void GroupDoc::set_props(const SketchParamMap& props)
{
    TFObjectDoc::set_props(props);

    this->dirty();
}

void GroupDoc::copy_from(TFObjectDoc* obj) { TFObjectDoc::copy_from(obj); }

void GroupDoc::addChild(TFObjectDoc* pChild)
{
    if(!pChild || m_children.find(pChild) != m_children.end())
        return;

    if(pChild->parent())
    {
        pChild->parent()->_group()->removeChild(pChild);
    }

    pChild->set_parent(this);
    m_children.insert(pChild);

    this->bbox_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void GroupDoc::addChildren(const std::unordered_set<long long>& ids)
{
    std::unordered_set<ObjectDoc*> addedChildren;

    for(auto id: ids)
    {
        TFObjectDoc* pChild = this->m_pScene->getTFObject(id);
        this->addChild(pChild);
    }
}

void GroupDoc::removeChild(TFObjectDoc* pChild)
{
    if(!pChild || m_children.find(pChild) == m_children.end())
        return;

    m_children.erase(pChild);
    pChild->set_parent(nullptr);

    this->bbox_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void GroupDoc::removeChildren(const std::unordered_set<long long>& ids)
{
    std::unordered_set<ObjectDoc*> rmvChildren;

    for(auto id: ids)
    {
        TFObjectDoc* child = (TFObjectDoc*)this->m_pScene->getTFObject(id);

        if(!child || m_children.find(child) == m_children.end())
            continue;
        m_children.erase(child);
        child->set_parent(nullptr);
        child->transform_change();
    }

    this->bbox_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void GroupDoc::removeAllChildren()
{
    for(auto child: m_children)
    {
        if(!child || m_children.find(child) == m_children.end())
            continue;
        child->set_parent(nullptr);
        child->transform_change();
    }
    m_children.clear();

    this->bbox_change();
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void GroupDoc::_updateBBox()
{
    this->m_bbox.reset();

    for(auto child: m_children)
    {
        child->bbox().setMatrix(child->local_matrix());
        this->m_bbox.add(child->bbox());
        child->bbox().setMatrix(child->world_matrix());
    }
    this->m_bbox.setMatrix(this->world_matrix());
}

}// namespace sketch