#pragma once

#include "../Core.h"
#include <functional>
#include <string>
#include <unordered_set>
#include <vcruntime.h>

#include "DocElement.h"
#include "Engine/Math/Mat43.h"
#include "Engine/Math/Mat43f.h"
#include "sketch/Serializer/SketchSerializer.h"

#include "nous_base/nous_base.h"

namespace Engine
{
class Path;
class Guid;
class Vec2;
class Vec2f;
class Vec3;
class Vec3f;
class Mat43;
class Mat43f;
class AABB;
class AABBf;
class AxisPlane;
class AxisPlanef;
class OBB;
}// namespace Engine

namespace sketch
{
class ViewDisplayFilter;
class ViewDoc;
class SceneDoc;
class ObjectDoc;
class TFObjectDoc;

class Document : public DocElement
{
public:
    static Document* create();
    virtual ~Document();

    virtual void init() = 0;
    virtual void save(SketchSerializer& serializer, const std::wstring& progName, float progRange) = 0;
    virtual void getProjectProps(Engine::DObject& props) = 0;
    virtual void modify(const Engine::DObject& props) = 0;

    virtual bool onDocPrepare(const Engine::Path& projPath) = 0;
    virtual bool onDocNew(const Engine::Path& projPath) = 0;
    virtual bool onDocLoaded(const Engine::Path& projPath) = 0;
    virtual bool onDocWillClose() = 0;
    virtual bool onDocClose() = 0;

    virtual void load(const Engine::Path& projPath, SketchSerializer* serializer, const std::wstring& progName, float progRange,
                      std::function<void(bool)> onFinish) = 0;

    virtual void createLockFile(const Engine::Path& projPath) = 0;
    virtual void releaseLockFile() = 0;

    virtual const Engine::Guid& guid() const = 0;
    virtual const std::wstring& env() const = 0;
    virtual void setEnv(const std::wstring& env) = 0;
    virtual void setReadOnly(bool readOnly) = 0;
    virtual bool readOnly() const = 0;
    virtual SceneDoc* createScene(long long id) = 0;
    virtual SceneDoc* getScene(long long id) const = 0;
    virtual SceneDoc* getSceneByObj(long long objId) const = 0;
    virtual ObjectDoc* getObject(long long objId) const = 0;
    virtual TFObjectDoc* getTFObject(long long objId) const = 0;
    virtual void deleteObject(long long objId) = 0;
    virtual void deleteObjects(std::unordered_set<long long> objIds) = 0;
    virtual SceneDoc* scene() const = 0;        // main scene
    virtual SceneDoc* compEditScene() const = 0;// scene for comp editor
    virtual float d2v(double val) const = 0;    // doc uit to view unit
    virtual Engine::Vec3f d2v(const Engine::Vec3& val) const = 0;
    virtual Engine::Vec2f d2v(const Engine::Vec2& val) const = 0;
    virtual Engine::Mat43f d2v(const Engine::Mat43& val) const = 0;
    virtual Engine::AABBf d2v(const Engine::AABB& val) const = 0;
    virtual Engine::AxisPlanef d2v(const Engine::AxisPlane& val) const = 0;
    virtual Engine::OBB d2v(const Engine::OBB& val) const = 0;
    virtual const Engine::Mat43& d2vMatrix() const = 0;
    virtual const Engine::Mat43& v2dMatrix() const = 0;
    virtual double v2d(float val) const = 0;
    virtual Engine::Vec3 v2d(const Engine::Vec3f& val) const = 0;
    // virtual Engine::Vec2 v2d(const Engine::Vec2f& val) const = 0;
    virtual Engine::Mat43 v2d(const Engine::Mat43f& val) const = 0;
    virtual Engine::AABB v2d(const Engine::AABBf& val) const = 0;
    // virtual Engine::AxisPlane v2d(const Engine::AxisPlanef& val) const = 0;
    virtual Engine::OBB v2d(const Engine::OBB& val) const = 0;
    virtual ViewDoc* createView(long long id, long long sceneId) = 0;
    virtual void deleteView(long long id) = 0;
    virtual std::wstring getValidViewName(const std::wstring& prefix) const = 0;
    virtual ViewDoc* getView(long long id) const = 0;
    virtual ViewDoc* getView(const std::wstring& name) const = 0;
    virtual const std::unordered_map<long long, ViewDoc*>& views() const = 0;
    virtual void addActiveView(long long id) = 0;
    virtual void removeActiveView(long long id) = 0;
    virtual const std::unordered_set<long long>& activeViews() const = 0;
    virtual void setActiveViews(const std::vector<long long>& ids) = 0;
    virtual void setActiveViews(long long* views, size_t num) = 0;
    virtual void addActiveViews(long long* views, size_t num) = 0;
    virtual void removeActiveViews(long long* views, size_t num) = 0;
    virtual const Engine::Path& path() const = 0;
    virtual void setPath(const Engine::Path& path) = 0;
    virtual const std::wstring& projNo() const = 0;
    virtual const std::wstring& projName() const = 0;
    virtual const std::wstring& author() const = 0;
    virtual const std::wstring& target() const = 0;
    virtual const std::wstring& address() const = 0;
    virtual const std::wstring& designer() const = 0;
    virtual long long dateBegin() const = 0;
    virtual long long dateComplete() const = 0;
    virtual const std::wstring& remark(int idx) const = 0;
    virtual const std::wstring& comment() const = 0;
    virtual const std::wstring& usrField(int idx) const = 0;
    virtual const std::wstring& fullDesc() const = 0;
    virtual const std::wstring& shortDesc() const = 0;
    virtual double totalWeight() const = 0;
    virtual const std::wstring& manufacturer() const = 0;
    virtual void package() = 0;
    virtual void update(unsigned long long tm) = 0;

    virtual ViewDoc* GetFirstView() const = 0;
};
}// namespace sketch