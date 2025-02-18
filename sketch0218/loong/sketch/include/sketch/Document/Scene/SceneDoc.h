#pragma once

#include "..\..\Core.h"
#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../../Define.h"
#include "../DocElement.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace Engine
{
class Path;
class DObject;
class AABB;
class OBB;
class AxisPlane;
class Mat43;
}// namespace Engine

namespace sketch
{
class RenderScene;
class INoable;
class Number;
class ObjectDoc;
class CtrlPointDoc;
class TFObjectDoc;
class PreBuiltDoc;
class AssemblyDoc;
class PartDoc;

class SceneDoc : public DocElement
{
    friend class LoadSceneHangUp;

public:
    static long long sceneId(long long objId);
    static long long objId(long long objId);

public:
    static SceneDoc* create(long long id);

    virtual ~SceneDoc();

    virtual bool onDocPrepare(const Engine::Path& projPath) = 0;// 场景初始化/加载前调用
    virtual bool onDocNew(const Engine::Path& projPath) = 0;    // 初始化新场景
    virtual bool onDocLoaded(const Engine::Path& projPath) = 0; // 场景初始化/加载完毕后调用

    virtual bool save(SketchSerializer& serializer, const std::wstring& progName, float progRange) = 0;
    virtual void load(SketchSerializer& serializer) = 0;
    virtual void load_async(SketchSerializer& serializer, const std::wstring& progName, float progRange, std::function<void(bool)> onFinish) = 0;

    virtual long long id() const = 0;
    virtual long long rvtId() const = 0;
    virtual void setRenderScene(RenderScene* pRdScene) = 0;
    virtual RenderScene* renderScene() const = 0;

    virtual void setLoading(bool b) = 0;
    virtual bool isLoading() const = 0;
    virtual bool isDisposing() const = 0;

    virtual bool isNumbering() const = 0;
    virtual void setNumbering(bool b) = 0;

    virtual void setObjIdCounter(long long id) = 0;
    virtual long long genObjId() = 0;
    virtual long long genObjId(long long id) const = 0;
    virtual void reserveObjId(long long id) = 0;
    virtual bool checkObj(long long objId) = 0;
    virtual long long objIdCounter() const = 0;

    virtual ObjectDoc* createObject(Define::ObjectType type, uint32_t flags = 0u) = 0;
    virtual ObjectDoc* createObject(Define::ObjectType type, long long id) = 0;
    virtual void deleteObject(long long id) = 0;
    virtual void deleteObjects(const std::unordered_set<long long>& ids) = 0;
    virtual void deleteObjects(const std::unordered_set<ObjectDoc*>& objs) = 0;
    virtual void deleteObjects(const std::unordered_set<TFObjectDoc*>& objs) = 0;
    virtual void clearObjects() = 0;

    virtual const std::unordered_map<long long, ObjectDoc*>& objects() const = 0;
    virtual const std::unordered_map<long long, TFObjectDoc*>& tfObjects() const = 0;

    virtual ObjectDoc* getObject(long long id) = 0;
    virtual TFObjectDoc* getTFObject(long long id) = 0;

    virtual void addObjListener(long long id, DocListener* pListener) = 0;
    virtual void removeObjListener(long long id, DocListener* pListener) = 0;

    virtual bool equalOrContain(long long mainId, long long childId) = 0;

    virtual std::unordered_set<ObjectDoc*> filterObjsByScene(const std::unordered_set<ObjectDoc*>& objs) = 0;
    virtual std::unordered_set<long long> filterObjsByScene(const std::unordered_set<long long>& ids) = 0;
    virtual std::unordered_set<ObjectDoc*> filterObjs(const std::unordered_set<ObjectDoc*>& objs, Define::ObjectType typeBegin,
                                                      Define::ObjectType typeEnd, bool recursive) = 0;
    virtual std::unordered_set<long long> filterObjs(const std::unordered_set<long long>& ids, Define::ObjectType typeBegin,
                                                     Define::ObjectType typeEnd, bool recursive) = 0;
    virtual std::unordered_set<ObjectDoc*> topLevelObjs(const std::unordered_set<ObjectDoc*>& objs) = 0;
    virtual std::unordered_set<long long> topLevelIds(const std::unordered_set<long long>& ids) = 0;

    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, ObjectDoc* pChild) = 0;
    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, const std::unordered_set<ObjectDoc*>& children) = 0;
    virtual bool contains(const std::unordered_set<ObjectDoc*>& parents, const std::unordered_set<TFObjectDoc*>& children) = 0;

    virtual Engine::AABB bbox() = 0;

    virtual void updateUISelInfo() = 0;
};
}// namespace sketch