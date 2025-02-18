#include "sketch/Document/Scene/ObjectDoc.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/Revertable.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
ObjectDoc::ObjectDoc(SceneDoc* pScene, Define::ObjectType type, long long id)
    : m_pScene(pScene), m_type(type), m_flags(Define::OBJ_FLAG_ENABLE | Define::OBJ_FLAG_SAVE_ENABLE | Define::OBJ_FLAG_USER_COPY_ENABLE |
                                              Define::OBJ_FLAG_USER_MOVE_ENABLE | Define::OBJ_FLAG_REVERTABLE),
      m_dirtyFlags(0xffffu)
{
    if(pScene == nullptr)// dummy
    {
        return;
    }

    if(id >= 0)
    {
        m_id = id;
        pScene->reserveObjId(id);
    }
    else
    {
        m_id = pScene->genObjId();
    }
}
ObjectDoc::~ObjectDoc()
{
    // do nothing
}
void ObjectDoc::dispose() { this->invokeListener(Define::MSG_OBJ_DISPOSE, nullptr); }

void ObjectDoc::record() {}

void ObjectDoc::get_props(SketchParamMap& props)
{
    props.set_value("id", m_id);
    props.set_value("type", static_cast<int>(m_type));
    props.set_value("obj_flags", static_cast<unsigned int>(m_flags));
}

void ObjectDoc::set_props(const SketchParamMap& props)
{
    props.get_value("id", m_id);
    int saved_type;
    props.get_value("type", saved_type);
    m_type = Define::ObjectType(static_cast<short>(saved_type));
    unsigned int saved_obj_flags;
    props.get_value("obj_flags", saved_obj_flags);
    m_flags = static_cast<uint16_t>(saved_obj_flags);
}

bool ObjectDoc::isValid() { return true; }

bool ObjectDoc::isTemporary() { return false; }

bool ObjectDoc::isSelected(bool bSelfOnly) const { return m_rflags & RFLAG_BIT_SELECTED; }

bool ObjectDoc::isHovered(bool bSelfOnly) const { return m_rflags & RFLAG_BIT_HOVERED; }

bool ObjectDoc::isTopHovered() const { return m_rflags & RFLAG_BIT_HOVERED; }

void ObjectDoc::setSelected(bool b)
{
    if(bool(m_rflags & RFLAG_BIT_SELECTED) == b)
        return;

    if(b)
        m_rflags |= RFLAG_BIT_SELECTED;
    else
        m_rflags &= ~RFLAG_BIT_SELECTED;

    this->_onSelectChanged();
}

void ObjectDoc::setDebug(bool b)
{
    if(b)
        m_rflags |= RFLAG_BIT_DEBUG;
    else
        m_rflags &= ~RFLAG_BIT_DEBUG;
}

void ObjectDoc::setHovered(bool b)
{
    if(b)
        m_rflags |= RFLAG_BIT_HOVERED;
    else
        m_rflags &= ~RFLAG_BIT_HOVERED;
}

void ObjectDoc::setMag(bool b)
{
    if(b)
        m_flags |= Define::OBJ_FLAG_MAG;
    else
        m_flags &= ~Define::OBJ_FLAG_MAG;

    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

bool ObjectDoc::isGroup() const { return dynamic_cast<GroupDoc*>(const_cast<ObjectDoc*>(this)); }

GroupDoc* ObjectDoc::_group() const { return dynamic_cast<GroupDoc*>(const_cast<ObjectDoc*>(this)); }

void ObjectDoc::setDisposed(bool b)
{
    if(b)
    {
        m_rflags |= RFLAG_BIT_DISPOSED;
    }
    else
    {
        m_rflags &= ~RFLAG_BIT_DISPOSED;
    }
}
bool ObjectDoc::package() { return true; }

void ObjectDoc::onLateUpdate() {}

bool ObjectDoc::getWorkProps(const Engine::Mat43& workAxis, Engine::DObject& workProps, bool bCpts)
{
    // this->getProps(workProps, bCpts);
    workProps.set("mag", this->isMag());
    workProps.set("lock", this->lockFlag());
    return true;
}

bool ObjectDoc::_parseProp(Engine::DObject& flags, const std::string& pname, const Engine::DValue& pval,
                           const std::unordered_map<long long, long long>* idMap)
{
    if(pname == "obj_flags")
    {
        m_flags = pval._short();
    }
    else if(pname == "enable")
    {
        if(pval._bool())
        {
            m_flags |= Define::OBJ_FLAG_ENABLE;
        }
        else
        {
            m_flags &= ~Define::OBJ_FLAG_ENABLE;
        }
        return true;
    }
    else if(pname == "mag")
    {
        if(pval._bool())
        {
            m_flags |= Define::OBJ_FLAG_MAG;
        }
        else
        {
            m_flags &= ~Define::OBJ_FLAG_MAG;
        }
        return true;
    }
    else if(pname == "lock")
    {
        if(pval._bool())
        {
            m_flags |= Define::OBJ_FLAG_LOCK;
        }
        else
        {
            m_flags &= ~Define::OBJ_FLAG_LOCK;
        }
        return true;
    }
    else if(pname == "comp_temp")
    {
        if(pval._bool())
        {
            m_flags |= Define::OBJ_FLAG_ENABLE;
        }
        else
        {
            m_flags &= ~Define::OBJ_FLAG_ENABLE;
        }
        return true;
    }
    return false;
}
void ObjectDoc::_onPostParseProps(const Engine::DObject& props, const std::unordered_map<long long, long long>* idMap)
{
    // do nothing
}
void ObjectDoc::_onUpdateModifyDirtyFlags()
{
    // do nothing
}
void ObjectDoc::_onUpdateTransformDirtyFlags()
{
    // do nothing
}
void ObjectDoc::_onSelectChanged() {}
TFObjectDoc* ObjectDoc::_tfObject() const { return dynamic_cast<TFObjectDoc*>(const_cast<ObjectDoc*>(this)); }

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, bool val, bool def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, double val, double def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, long long val, long long def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, int val, int def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, char val, char def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, uint16_t val, uint16_t def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, uint32_t val, uint32_t def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, const std::wstring& val, const std::wstring& def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, const Engine::Vec2& val, const Engine::Vec2& def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, const Engine::Vec3& val, const Engine::Vec3& def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_saveProp(Engine::DObject& res, const std::string& pname, const Engine::Mat43& val, const Engine::Mat43& def)
{
    if(val != def)
        res[pname] = val;
}

void ObjectDoc::_savePropDoubleArray(Engine::DObject& res, Engine::DObject& src, const std::string& pname)
{
    auto it = src.find(pname);
    if(it != src.end() && it->second.type() == DValue::TYPE_DOUBLE_ARRAY && it->second.len() > 0)
    {
        res[pname].set(it->second._doubleArr(), it->second.len(), false);
    }
}

}// namespace sketch
