#pragma once

#include <string>
#include <unordered_set>

#include "..\DocElement.h"
#include "sketch/Object/SketchParamMap.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace Engine
{
class AxisPlane;
class Mat43;
class OBB;
}// namespace Engine

namespace sketch
{
class View;
class SceneDoc;
class ObjectDoc;
class ObjectFilter;
class ViewDisplaySetting;

class ViewDoc : public DocElement
{
public:
    static ViewDoc* create(long long id, SceneDoc* pScene);
    virtual ~ViewDoc(void);

    virtual void modify(const SketchParamMap& props) = 0;
    virtual const SketchParamMap& props() = 0;

    virtual void redraw() = 0;
    virtual void resetCameraPose() = 0;

    virtual void save(SketchSerializer& serializer) = 0;
    virtual void load(SketchSerializer& serializer) = 0;

    virtual void setRenderView(View* pView) = 0;
    virtual View* renderView() const = 0;
    virtual SceneDoc* scene() const = 0;
    virtual long long id() const = 0;
    virtual const ::std::wstring& name() const = 0;
    virtual bool isTemp() const = 0;
    virtual Define::ViewType type() const = 0;
    virtual Define::ViewProjMode projMode() const = 0;
    virtual double depthUp() const = 0;
    virtual double depthDown() const = 0;
    virtual double rotH() const = 0;
    virtual double rotV() const = 0;
    virtual ViewDisplaySetting* dispSetting() const = 0;
    virtual ObjectFilter* dispFilter() const = 0;
    virtual const Engine::AxisPlane& viewPlane() const = 0;
    virtual const Engine::Vec2& workAreaMin() const = 0;
    virtual const Engine::Vec2& workAreaMax() const = 0;
    virtual const Engine::OBB& clipBox() const = 0;
    virtual bool isCamPivotValid() const = 0;
    virtual const Engine::Vec3& camPivot() const = 0;
    virtual Engine::Vec3 majorAxis() const = 0;
    virtual void setViewType(Define::ViewType type) = 0;
    virtual void setCamPivot(const Engine::Vec3& pivot) = 0;
    virtual void resetCamPivot() = 0;

    virtual void setWorkArea(const Engine::Vec2& vMin, const Engine::Vec2& vMax) = 0;
    virtual void setEuler(double rotH, double rotV) = 0;

    virtual void askIfClipped(long long objId) = 0;
    virtual void askIfClipped(const ::std::unordered_set<long long>& ids) = 0;
    virtual void askIfClipped(const ::std::unordered_set<ObjectDoc*>& objs) = 0;

    virtual double pickClipBoxFacingFaces(const Engine::Vec3& vOrigin, const Engine::Vec3& vDir) = 0;
};

}// namespace sketch
