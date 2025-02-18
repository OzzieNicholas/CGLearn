#pragma once

#include "sketch/Core.h"
#include "sketch/Define.h"
#include "sketch/Document/DocElement.h"
#include "sketch/Document/DocListener.h"
#include "sketch/Object/SketchParamMap.h"
#include "sketch/Serializer/SketchSerializer.h"

#include <memory>

namespace sketch
{
class SceneDoc;
class Revertable;
class Revertable_ObjectModify;
class TFObjectDoc;
class GroupDoc;
class GeoData;
class ObjectDoc : public DocElement, public DocListener
{
    friend class SceneDoc;

public:
    ObjectDoc(SceneDoc* pScene, Define::ObjectType type, long long id);
    virtual ~ObjectDoc();

    virtual void dispose();
    virtual void record();

    virtual void get_props(SketchParamMap& props);
    virtual void set_props(const SketchParamMap& props);

    virtual bool isValid();

    virtual bool isTemporary();

    inline bool isRevertable() const { return m_flags & Define::OBJ_FLAG_REVERTABLE; }
    inline bool isUserCopyable() const { return m_flags & Define::OBJ_FLAG_USER_COPY_ENABLE; }

    inline bool isUserMovable() const { return m_flags & Define::OBJ_FLAG_USER_MOVE_ENABLE; }

    inline bool isEnable() const { return m_flags & Define::OBJ_FLAG_ENABLE; }

    virtual bool isSelected(bool bSelfOnly = false) const;
    virtual bool isHovered(bool bSelfOnly) const;
    virtual bool isTopHovered() const;

    virtual void setSelected(bool b);
    virtual void setHovered(bool b);
    virtual void setDebug(bool b);
    virtual void setDisposed(bool b);

    inline long long id() const { return m_id; }

    inline bool isTF() const { return m_type > Define::OBJ_TF_BEGIN && m_type < Define::OBJ_TYPE_END; }

    inline bool isDisposed() const { return m_rflags & RFLAG_BIT_DISPOSED; }

    virtual bool package();

    virtual void onLateUpdate();

    virtual bool getWorkProps(const Engine::Mat43& workAxis, Engine::DObject& workProps, bool bCpts);

    inline Define::ObjectType type() const { return m_type; }

    virtual void setMag(bool b);

    bool isGroup() const;

    GroupDoc* _group() const;

    inline bool isSavable() const { return m_flags & Define::OBJ_FLAG_SAVE_ENABLE; }

    TFObjectDoc* _tfObject() const;

    inline SceneDoc* scene() const { return m_pScene; }

    inline long long modifyCounter() const { return m_modifyCounter; }

    inline uint32_t flags() const { return m_flags; }

    inline bool isMag() const { return m_flags & Define::OBJ_FLAG_MAG; }

    inline bool lockFlag() const { return m_flags & Define::OBJ_FLAG_LOCK; }

protected:
    static long long _replaceId(long long id, const std::unordered_map<long long, long long>* idMap)
    {
        if(idMap == nullptr)
            return id;

        auto it = idMap->find(id);
        if(it != idMap->end())
        {
            return it->second;
        }

        return id;
    }

    virtual bool _parseProp(Engine::DObject& flags, const std::string& name, const Engine::DValue& pval,
                            const std::unordered_map<long long, long long>* idMap);
    virtual void _onPostParseProps(const Engine::DObject& props, const std::unordered_map<long long, long long>* idMap);

    virtual void _onUpdateModifyDirtyFlags();
    virtual void _onUpdateTransformDirtyFlags();
    virtual void _onSelectChanged();

    static void _saveProp(Engine::DObject& res, const std::string& pname, bool val, bool def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, char val, char def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, double val, double def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, long long val, long long def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, int val, int def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, uint16_t val, uint16_t def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, uint32_t val, uint32_t def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, const std::wstring& val, const std::wstring& def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, const Engine::Vec2& val, const Engine::Vec2& def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, const Engine::Vec3& val, const Engine::Vec3& def);
    static void _saveProp(Engine::DObject& res, const std::string& pname, const Engine::Mat43& val, const Engine::Mat43& def);
    static void _savePropDoubleArray(Engine::DObject& res, Engine::DObject& src, const std::string& pname);

protected:
    SceneDoc* m_pScene;

    long long m_id;
    Define::ObjectType m_type;

    // common flags
    uint16_t m_flags;

    long long m_modifyCounter;// for script sync props

    // runtime flags (not saved in props)
    static const uint16_t RFLAG_BIT_DISPOSED = 1 << 0;
    static const uint16_t RFLAG_BIT_SELECTED = 1 << 1;
    static const uint16_t RFLAG_BIT_HOVERED = 1 << 2;
    static const uint16_t RFLAG_BIT_DEBUG = 1 << 3;
    static const uint16_t RFLAG_BIT_COMP_CHILD = 1 << 4;
    uint16_t m_rflags;

    uint32_t m_dirtyFlags;// b0: props, b1: mat local, b2: mat world, b3 bbox, b4: geo, b5: obj geo info

    // dirty flags
    static const uint32_t DIRTY_BIT_MATL = 1 << 0;
    static const uint32_t DIRTY_BIT_MATW = 1 << 1;
    static const uint32_t DIRTY_BIT_BBOX = 1 << 2;
    static const uint32_t DIRTY_BIT_GEO = 1 << 3;
    static const uint32_t DIRTY_BIT_GEO_H = 1 << 4;
    static const uint32_t DIRTY_BIT_GEO_NATIVE = 1 << 5;
    static const uint32_t DIRTY_BIT_GEO_NO_PREPARE = 1 << 6;
    static const uint32_t DIRTY_BIT_CENTER_LINE_NATIVE = 1 << 7;
    static const uint32_t DIRTY_BIT_SHAPE = 1 << 8;
    static const uint32_t DIRTY_BIT_MEASURE = 1 << 9;
    static const uint32_t DIRTY_BIT_BOLT_HOLE_DEPTH = 1 << 10;
    static const uint32_t DIRTY_BIT_BOLT_HOLE_GEO = 1 << 11;
    static const uint32_t DIRTY_BIT_BOLT_HOLE_GEO_H = 1 << 12;
    static const uint32_t DIRTY_BIT_BOLT_CFGS = 1 << 13;
    static const uint32_t DIRTY_BIT_WELD_GROOVE_GEO = 1 << 14;
    static const uint32_t DIRTY_BIT_AUTO_WELD = 1 << 15;
    static const uint32_t DIRTY_BIT_PART_SEG = 1 << 16;

    static const uint32_t DIRTY_BIT_PROPS_MATW_BBOX = DIRTY_BIT_MATW | DIRTY_BIT_BBOX;
    static const uint32_t DIRTY_BIT_PROPS_MATLW = DIRTY_BIT_MATL | DIRTY_BIT_MATW;
    static const uint32_t DIRTY_BIT_PROPS_BBOX = DIRTY_BIT_BBOX;
    static const uint32_t DIRTY_BIT_MATLW_BBOX = DIRTY_BIT_MATL | DIRTY_BIT_MATW | DIRTY_BIT_BBOX;
    static const uint32_t DIRTY_BIT_MATLW = DIRTY_BIT_MATL | DIRTY_BIT_MATW;

    static const uint32_t DIRTY_BIT_PROPS_BBOX_GEO = DIRTY_BIT_BBOX | DIRTY_BIT_GEO | DIRTY_BIT_GEO_H | DIRTY_BIT_GEO_NATIVE |
                                                     DIRTY_BIT_GEO_NO_PREPARE | DIRTY_BIT_CENTER_LINE_NATIVE | DIRTY_BIT_SHAPE |
                                                     DIRTY_BIT_BOLT_HOLE_GEO | DIRTY_BIT_BOLT_HOLE_GEO_H | DIRTY_BIT_WELD_GROOVE_GEO;

    static const uint32_t DIRTY_BIT_PROPS_MATLW_BBOX_GEO_MEASURE =
        DIRTY_BIT_MATL | DIRTY_BIT_MATW | DIRTY_BIT_BBOX | DIRTY_BIT_GEO | DIRTY_BIT_GEO_H | DIRTY_BIT_GEO_NATIVE | DIRTY_BIT_GEO_NO_PREPARE |
        DIRTY_BIT_CENTER_LINE_NATIVE | DIRTY_BIT_BOLT_HOLE_GEO | DIRTY_BIT_BOLT_HOLE_GEO_H | DIRTY_BIT_WELD_GROOVE_GEO | DIRTY_BIT_MEASURE |
        DIRTY_BIT_SHAPE;

    static const uint32_t DIRTY_BIT_PROPS_MATLW_BBOX = DIRTY_BIT_MATL | DIRTY_BIT_MATW | DIRTY_BIT_BBOX;

public:// default props
};
}// namespace sketch