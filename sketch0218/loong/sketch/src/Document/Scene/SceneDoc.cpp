#include "sketch/Document/Scene/SceneDoc.h"

#include <Windows.h>
#include <mutex>
#include <ppl.h>
#include <ppltasks.h>
#include <stack>

#include "Engine/Math/CCDT/CCDT.h"
#include "Engine/Util/CriticalSection.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/STLUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/LoadSceneHangUp.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/Document/View/ViewDoc.h"

using namespace Engine;

namespace sketch
{
#define SCENE_ID_BITS (0xff00000000000000LL)
#define OBJ_ID_BITS (0xffffffffffffffLL)

class SceneDocImpl : public SceneDoc
{
protected:
    typedef ObjectDoc* (*OBJ_INST_FUNC)(SceneDoc*, long long);

public:
    SceneDocImpl(long long id) : m_id(id), m_rvtId(STCore::inst->cmdMgr()->createRvtPool()), m_pRenderScene(nullptr), m_objIdCounter(0), m_state(0u)

    {
        if(!m_instFuncs)
            this->_initInstFuncs();
    }

    virtual ~SceneDocImpl() override
    {
        m_state |= 2;// set state to disposing

        STCore::inst->cmdMgr()->freezeRvt(m_rvtId);

        // invoke RenderScene to clear RenderObjects
        this->invokeListener(Define::MSG_SCENE_DELETE, DObject::alloc({ { "scene_id", m_id } }));

        // clear ObjectDocs
        for(auto& it: m_objects)
        {
            auto pObj = it.second;
            pObj->setDisposed(true);
            pObj->dispose();
            delete pObj;
        }

        STCore::inst->cmdMgr()->unfreezeRvt(m_rvtId);
        STCore::inst->cmdMgr()->deleteRvtPool(m_rvtId);
    }

    virtual bool onDocPrepare(const Engine::Path& projPath) override
    {// 场景初始化/加载前调用
        return true;
    }

    virtual bool onDocNew(const Engine::Path& projPath) override
    {// 初始化新场景
        CmdManager* pCmdMgr = STCore::inst->cmdMgr();
        pCmdMgr->freezeRvt(m_rvtId);

        pCmdMgr->unfreezeRvt(m_rvtId);

        return true;
    }

    virtual bool onDocLoaded(const Engine::Path& projPath) override
    {// 场景初始化/加载完毕后调用
        return true;
    }

    virtual bool save(SketchSerializer& serializer, const std::wstring& progName, float progRange) override
    {
        std::vector<ObjectDoc*> save_objs;
        save_objs.reserve(m_objects.size());

        for(auto it: m_objects)
        {
            ObjectDoc* obj = it.second;
            if(!obj->isSavable())
            {
                continue;
            }
            save_objs.push_back(obj);
        }

        float prog = STCore::inst->uiProgress(progName);

        serializer.begin_write_array("objs");
        // concurrency::create_task([&serializer, &save_objs] {
        for(int i = 0; i < save_objs.size(); ++i)
        {
            serializer.begin_write_object();
            SketchParamMap props;
            save_objs[i]->get_props(props);
            props.save(serializer);
            // save_objs[i]->save(serializer);
            serializer.end_write_object();
        }
        //});
        serializer.end_write_array();

        serializer.write(m_objIdCounter, "obj_id_counter");
        STCore::inst->updateUIProgress(progName, prog + progRange);

        return true;
    }

    virtual void load(SketchSerializer& serializer)
    {
        STCore::inst->cmdMgr()->freezeRvt(m_rvtId);
        this->setLoading(true);

        const bool has_objs_field = serializer.has_field("objs");
        size_t object_count;
        serializer.begin_read_array("objs", object_count);

        std::map<long long, std::vector<long long>> group_map;

        for(int i = 0; i < object_count; ++i)
        {
            serializer.begin_read_object();
            SketchParamMap props;
            props.load(serializer);
            long long obj_id = -1;
            props.get_value("id", obj_id);
            int saved_type;
            props.get_value("type", saved_type);
            Define::ObjectType obj_type = Define::ObjectType(static_cast<short>(saved_type));

            ObjectDoc* obj = this->createObject(obj_type, obj_id);
            if(obj)
            {
                obj->set_props(props);
            }
            if(obj_type == Define::OBJ_D5_GROUP)
            {
                props.get_value("children_id", group_map[obj_id]);
            }

            // obj->load(serializer);
            this->invokeListener(Define::MSG_OBJ_CREATE, DObject::alloc({ { "obj_id", obj->id() } }));
            serializer.end_read_object();
        }
        serializer.end_read_array();

        // group
        for(auto& it: group_map)
        {
            long long group_id = it.first;
            auto& children = it.second;
            GroupDoc* parent = dynamic_cast<GroupDoc*>(this->getObject(group_id));
            if(!parent)
                continue;
            for(auto& child_id: children)
            {
                TFObjectDoc* child = dynamic_cast<TFObjectDoc*>(this->getObject(child_id));
                if(child)
                {
                    child->set_matrix_from_world(parent->world_matrix() * child->local_matrix());
                    parent->addChild((TFObjectDoc*)child);
                }
            }
        }

        this->setLoading(false);
        STCore::inst->cmdMgr()->unfreezeRvt(m_rvtId);
    }

    virtual void load_async(SketchSerializer& serializer, const std::wstring& progName, float progRange, std::function<void(bool)> onFinish)
    {
        auto pJob = new LoadSceneHangUp(this, serializer, progName, progRange, onFinish);
        STCore::inst->addHangUp(pJob);
    }

    virtual long long id() const override { return m_id; }

    virtual long long rvtId() const override { return m_rvtId; }

    virtual void setRenderScene(RenderScene* pRdScene) override { m_pRenderScene = pRdScene; }

    virtual RenderScene* renderScene() const override { return m_pRenderScene; }

    virtual void setLoading(bool b)
    {
        if(b)
            m_state |= 1u;
        else
            m_state &= ~1u;
    }

    virtual bool isLoading() const override { return m_state & 1; }

    virtual bool isDisposing() const override { return m_state & 2; }

    virtual bool isNumbering() const override { return m_state & 4; }

    virtual void setNumbering(bool b)
    {
        if(b)
            m_state |= 4;
        else
            m_state &= ~4;
    }

    virtual void setObjIdCounter(long long id) override { m_objIdCounter = id; }

    virtual long long genObjId() { return (m_id << 56) | m_objIdCounter++; }

    virtual long long genObjId(long long id) const { return (m_id << 56) | (id & OBJ_ID_BITS); }

    virtual void reserveObjId(long long id)
    {
        long long oid = (id & OBJ_ID_BITS);
        m_objIdCounter = STMAX(m_objIdCounter, oid + 1);
    }

    virtual bool checkObj(long long objId) { return m_id == (objId >> 56); }

    virtual long long objIdCounter() const { return m_objIdCounter; }

    virtual ObjectDoc* createObject(Define::ObjectType type, uint32_t flags) override
    {
        ObjectDoc* pObj = m_instFuncs[type](this, -1LL);
        m_objects[pObj->id()] = pObj;

        if(pObj->isTF())
        {
            m_objectsTF[pObj->id()] = (TFObjectDoc*)pObj;
        }

        this->invokeListener(Define::MSG_OBJ_CREATE, DObject::alloc({ { "obj_id", pObj->id() } }));

        CmdManager* pCmdMgr = STCore::inst->cmdMgr();
        if(!pCmdMgr->isRvtFreezed(m_rvtId) && pObj->isRevertable())
        {
            pCmdMgr->pushRvt(m_rvtId, new Revertable_ObjectCreate(pObj->id(), pObj->type()));
        }

        return pObj;
    }

    virtual ObjectDoc* createObject(Define::ObjectType type, long long id) override
    {
        // convert id to this scene
        if(id >= 0)
        {
            id = id & 0x00ffffffffffffffLL;
            if(id >= m_objIdCounter)
                m_objIdCounter = id + 1;
            id = id | (m_id << 56);
        }

        if(!m_instFuncs[type])
            return nullptr;

        ObjectDoc* pObj = m_instFuncs[type](this, id);
        TFObjectDoc* pTFObj = pObj->isTF() ? pObj->_tfObject() : nullptr;

#ifdef _DEBUG
        STASSERT(m_objects.find(pObj->id()) == m_objects.end());
#endif
        m_objects[pObj->id()] = pObj;

        if(pTFObj)
        {
            m_objectsTF[pObj->id()] = pTFObj;
        }

        this->invokeListener(Define::MSG_OBJ_CREATE, DObject::alloc({ { "obj_id", pObj->id() } }));

        CmdManager* pCmdMgr = STCore::inst->cmdMgr();
        if(!pCmdMgr->isRvtFreezed(m_rvtId) && pObj->isRevertable())
        {
            pCmdMgr->pushRvt(m_rvtId, new Revertable_ObjectCreate(pObj->id(), pObj->type()));
        }

        return pObj;
    }

    virtual void deleteObject(long long id) override
    {
        CmdManager* pCmdMgr = STCore::inst->cmdMgr();
        STCore::inst->docMgr()->updateModifyFrame();

        auto delIt = m_objects.find(id);
        if(delIt == m_objects.end())
            return;

        ObjectDoc* pObj = delIt->second;
        if(pObj->isDisposed())
            return;

        Define::ObjectType objType = pObj->type();
        long long objId = pObj->id();
        TFObjectDoc* pTFObj = pObj->isTF() ? pObj->_tfObject() : nullptr;

        if(!pCmdMgr->isRvtFreezed(m_rvtId) && pObj->isRevertable())
        {
            pCmdMgr->pushRvt(m_rvtId, new Revertable_ObjectDelete(objType, objId));
        }

        pObj->setDisposed(true);
        this->invokeListener(Define::MSG_OBJ_DELETE, DObject::alloc({ { "obj_id", objId } }));
        pObj->dispose();
        m_objects.erase(delIt);
        if(pTFObj)
            m_objectsTF.erase(objId);

        delete pObj;
    }

    virtual void deleteObjects(const std::unordered_set<long long>& ids) override
    {
        STCore::inst->docMgr()->updateModifyFrame();
        for(auto id: ids)
        {
            deleteObject(id);
        }
    }

    virtual void deleteObjects(const std::unordered_set<ObjectDoc*>& objs) override
    {
        STCore::inst->docMgr()->updateModifyFrame();
        for(auto pObj: objs)
        {
            deleteObject(pObj->id());
        }
    }

    virtual void deleteObjects(const std::unordered_set<TFObjectDoc*>& objs) override
    {
        STCore::inst->docMgr()->updateModifyFrame();
        for(auto pObj: objs)
        {
            deleteObject(pObj->id());
        }
    }

    virtual void clearObjects() override
    {
        while(!m_objects.empty())
        {
            this->deleteObject(m_objects.begin()->first);
        }
    }

    virtual const std::unordered_map<long long, ObjectDoc*>& objects() const override { return m_objects; }

    virtual const std::unordered_map<long long, TFObjectDoc*>& tfObjects() const override { return m_objectsTF; }

    virtual ObjectDoc* getObject(long long id) override
    {
        if(id < 0)
            return nullptr;
        auto it = m_objects.find(id);
        if(it == m_objects.end() || it->second->isDisposed())
            return nullptr;
        return it->second;
    }

    virtual TFObjectDoc* getTFObject(long long id) override
    {
        if(id < 0)
            return nullptr;
        auto it = m_objectsTF.find(id);
        if(it == m_objectsTF.end() || it->second->isDisposed())
            return nullptr;
        return it->second;
    }

    virtual void addObjListener(long long id, DocListener* pListener) override
    {
        if(pListener == nullptr)
            return;

        auto it = m_objects.find(id);
        if(it == m_objects.end())
            return;

        it->second->addListener(pListener);
    }

    virtual void removeObjListener(long long id, DocListener* pListener) override
    {
        if(pListener == nullptr)
            return;

        auto it = m_objects.find(id);
        if(it == m_objects.end())
            return;

        it->second->removeListener(pListener);
    }

    virtual bool equalOrContain(long long mainId, long long childId) override
    {
        if(mainId == -1 || childId == -1)
            return false;

        if(mainId == childId)
            return true;

        ObjectDoc* pMain = this->getObject(mainId);
        ObjectDoc* pChild = this->getObject(childId);
        if(!pMain || !pChild)
            return false;

        if(pMain->isTF() && pMain->_tfObject()->contains(pChild, true))
            return true;

        return false;
    }

    virtual std::unordered_set<ObjectDoc*> filterObjsByScene(const std::unordered_set<ObjectDoc*>& objs) override
    {
        std::unordered_set<ObjectDoc*> filtered;
        for(auto pObj: objs)
        {
            if(!pObj || sceneId(pObj->id()) != m_id)
                continue;

            filtered.insert(pObj);
        }
        return filtered;
    }

    virtual std::unordered_set<long long> filterObjsByScene(const std::unordered_set<long long>& ids) override
    {
        std::unordered_set<long long> filtered;
        for(auto id: ids)
        {
            if(id < 0 || sceneId(id) != m_id)
                continue;

            filtered.insert(id);
        }
        return filtered;
    }

    virtual std::unordered_set<ObjectDoc*> filterObjs(const std::unordered_set<ObjectDoc*>& objs, Define::ObjectType typeBegin,
                                                      Define::ObjectType typeEnd, bool recursive) override
    {
        std::unordered_set<ObjectDoc*> ret;

        std::vector<ObjectDoc*> pool;
        pool.insert(pool.end(), objs.begin(), objs.end());

        while(!pool.empty())
        {
            ObjectDoc* pObj = pool.back();
            pool.pop_back();

            Define::ObjectType type = pObj->type();
            if(type >= typeBegin && type <= typeEnd)
                ret.insert(pObj);
        }

        return ret;
    }

    virtual std::unordered_set<long long> filterObjs(const std::unordered_set<long long>& ids, Define::ObjectType typeBegin,
                                                     Define::ObjectType typeEnd, bool recursive) override
    {
        std::unordered_set<long long> ret;

        std::vector<ObjectDoc*> pool;
        for(auto id: ids)
        {
            auto it = m_objects.find(id);
            if(it == m_objects.end())
                continue;

            pool.push_back(it->second);
        }

        while(!pool.empty())
        {
            ObjectDoc* pObj = pool.back();
            pool.pop_back();

            Define::ObjectType type = pObj->type();
            if(type >= typeBegin && type <= typeEnd)
                ret.insert(pObj->id());
        }

        return ret;
    }

    virtual std::unordered_set<ObjectDoc*> topLevelObjs(const std::unordered_set<ObjectDoc*>& objs) override
    {
        std::unordered_set<ObjectDoc*> ret;
        for(auto pObj: objs)
        {
            TFObjectDoc* pPar = nullptr;
            if(pObj->isTF())
                pPar = pObj->_tfObject()->parent();

            bool isChild = false;
            while(pPar)
            {
                if(objs.find(pPar) != objs.end())
                {
                    isChild = true;
                    break;
                }

                pPar = pPar->parent();
            }

            if(!isChild)
                ret.insert(pObj);
        }

        return ret;
    }

    virtual std::unordered_set<long long> topLevelIds(const std::unordered_set<long long>& ids) override
    {
        std::unordered_set<long long> ret;
        for(auto id: ids)
        {
            ObjectDoc* pObj = m_objects[id];

            TFObjectDoc* pPar = nullptr;
            if(pObj->isTF())
                pPar = pObj->_tfObject()->parent();

            bool isChild = false;
            while(pPar)
            {
                if(ids.find(pPar->id()) != ids.end())
                {
                    isChild = true;
                    break;
                }

                pPar = pPar->parent();
            }

            if(!isChild)
                ret.insert(pObj->id());
        }

        return ret;
    }

    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, ObjectDoc* pChild) override
    {
        if(parents.empty() || !pChild)
            return false;

        if(!pChild->isTF())
        {
            return parents.find(pChild) != parents.end();
        }

        TFObjectDoc* pPar = pChild->_tfObject();
        while(pPar)
        {
            if(parents.find(pPar) != parents.end())
                return true;

            pPar = pPar->parent();
        }

        return false;
    }

    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, const std::unordered_set<ObjectDoc*>& children) override
    {
        for(auto pChild: children)
        {
            if(!pChild)
                return false;

            if(!pChild->isTF())
            {
                if(parents.find(pChild) == parents.end())
                    return false;

                continue;
            }

            TFObjectDoc* pPar = pChild->_tfObject();
            bool isIn = false;
            while(pPar)
            {
                if(parents.find(pPar) != parents.end())
                {
                    isIn = true;
                    break;
                }

                pPar = pPar->parent();
            }

            if(!isIn)
                return false;
        }

        return true;
    }

    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, const std::unordered_set<TFObjectDoc*>& children) override
    {
        for(auto pChild: children)
        {
            if(!pChild)
                return false;

            TFObjectDoc* pPar = pChild;
            bool isIn = false;
            while(pPar)
            {
                if(parents.find(pPar) != parents.end())
                {
                    isIn = true;
                    break;
                }

                pPar = pPar->parent();
            }

            if(!isIn)
                return false;
        }

        return true;
    }

    virtual Engine::AABB bbox() override
    {
        AABB ret;
        for(auto it: m_objectsTF)
        {
            TFObjectDoc* pTFObj = it.second;
            auto& bbox = pTFObj->bbox();
            if(!bbox.isValid())
                continue;

            ret.add(bbox);
        }
        return ret;
    }

    virtual void updateUISelInfo() override
    {
        auto pLayout = STCore::inst->viewMgr()->layout();

        std::wstring selInfo;

        if(UIFuncs::setObjSelInfo)
        {
            UIFuncs::setObjSelInfo(selInfo);
        }
    }

protected:
    static void _initInstFuncs()
    {
        // init instanciate funcs
        m_instFuncs = new OBJ_INST_FUNC[Define::OBJ_TYPE_END];
        ::memset(m_instFuncs, 0, sizeof(OBJ_INST_FUNC) * Define::OBJ_TYPE_END);

        /*m_instFuncs[Define::OBJ_GRID] = GridDoc::newInstance;
        m_instFuncs[Define::OBJ_GRID_AXIS] = GridAxisDoc::newInstance;*/
        // m_instFuncs[Define::OBJ_D5_POLYGON] = PolygonDoc::newInstance;
        m_instFuncs[Define::OBJ_D5_GROUP] = GroupDoc::newInstance;
        m_instFuncs[Define::OBJ_D5_MESH] = MeshDoc::newInstance;
        m_instFuncs[Define::OBJ_D5_NURBS] = NurbsDoc::newInstance;
        // m_instFuncs[Define::OBJ_DRAW_RECTANGLE] = RectangleDoc::newInstance;
        // m_instFuncs[Define::OBJ_DRAW_CIRCLE] = CircleDoc::newInstance;
    }

protected:
    long long m_objIdCounter;

    long long m_id;
    long long m_rvtId;

    // b0: loading, b1: disposing, b2: numbering
    uint32_t m_state;

    static OBJ_INST_FUNC* m_instFuncs;
    std::unordered_map<long long, ObjectDoc*> m_objects;
    std::unordered_map<long long, TFObjectDoc*> m_objectsTF;

    RenderScene* m_pRenderScene;
};

SceneDocImpl::OBJ_INST_FUNC* SceneDocImpl::m_instFuncs = nullptr;

SceneDoc* SceneDoc::create(long long id) { return new SceneDocImpl(id); }

SceneDoc::~SceneDoc() {}

long long SceneDoc::sceneId(long long objId) { return objId < 0 ? -1LL : (objId >> 56); }

long long SceneDoc::objId(long long objId) { return objId < 0 ? -1LL : (objId & OBJ_ID_BITS); }

}// namespace sketch