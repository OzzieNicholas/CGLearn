#pragma once

#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "RenderObject.h"
#include "sketch/Document/DocListener.h"

namespace sketch
{
class SceneDoc;
class Selector3D;
class RenderScene : public DocListener
{
    typedef RenderObject* (*OBJ_INST_FUNC)(RenderScene*, Define::ObjectType);

public:
    RenderScene(SceneDoc* pDoc);
    virtual ~RenderScene();

    bool onDocLoaded(const Engine::Path& projPath, const std::wstring& progName, float progRange);
    virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override;

    inline void setObjChangeCallback(std::function<void(Define::Message, RenderObject*)> callback) { m_objChangeCallback = callback; }

    inline SceneDoc* doc() const { return m_pDoc; }

    inline const std::unordered_map<long long, RenderObject*>& objects() { return m_objects; }

    inline RenderObject* getObject(long long id)
    {
        auto it = m_objects.find(id);
        if(it == m_objects.end())
            return nullptr;

        return (RenderObject*)it->second;
    }

    void updateObjInViews(RenderObject* pObj);

    void update(unsigned long long tm);

protected:
    static void _initInstFuncs();
    RenderObject* _createObject(const Define::ObjectType type, long long id);
    void _deleteObject(long long id);

protected:
    SceneDoc* m_pDoc;

    std::function<void(Define::Message, RenderObject*)> m_objChangeCallback;

    static OBJ_INST_FUNC* m_instFuncs;
    std::unordered_map<long long, RenderObject*> m_objects;
    std::unordered_set<long long> m_viewDirtyObjs;
};
}// namespace sketch