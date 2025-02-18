#pragma once

#include "Engine/Math/Vec2i.h"

#include "../Define.h"
#include "sketch/Document/DocListener.h"
#include "sketch/Object/SketchParamMap.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace Engine
{
class Texture2D;
class FrameBuffer;
}// namespace Engine

namespace sketch
{
class View;
class ViewLayout;
class LayoutCell;
class ViewCamera;
class TileViewLayout;
class Controller;
class ResManager;
class RenderScene;
class Drawing;
class Symbol;
class CompEditor;

class ViewManager : public DocListener
{
public:
    static ViewManager* create();
    virtual ~ViewManager(void);

    virtual bool init() = 0;

    virtual bool onDocPrepare(const Engine::Path& projPath) = 0;                                              // 场景初始化/加载前调用
    virtual bool onDocNew(const Engine::Path& projPath) = 0;                                                  // 初始化新场景
    virtual bool onDocLoad(const Engine::Path& projPath, Engine::DObject* pDocObj) = 0;                       // 加载场景
    virtual bool onDocLoaded(const Engine::Path& projPath, const std::wstring& progName, float progRange) = 0;// 场景初始化/加载完成后调用
    virtual bool onDocWillClose(void) = 0;
    virtual bool onDocClose(void) = 0;
    virtual bool onAppClose(void) = 0;
    virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override = 0;

    virtual void save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange) = 0;
    virtual void save(SketchSerializer& serializer, const std::wstring& progName, float progRange) = 0;
    virtual void load(SketchSerializer& serializer, const Engine::Path& projPath) = 0;

    virtual void setMode(Define::ViewManagerMode mode) = 0;
    virtual Define::ViewManagerMode mode() const = 0;

    virtual const Engine::Vec2i& monitorSize() const = 0;
    virtual int monitorWidth() const = 0;
    virtual int monitorHeight() const = 0;
    virtual const Engine::Vec2i& canvasPhysicSize() const = 0;
    virtual int canvasPhysicWidth() const = 0;
    virtual int canvasPhysicHeight() const = 0;
    virtual const Engine::Vec2i& canvasLogicSize() const = 0;
    virtual int canvasLogicWidth() const = 0;
    virtual int canvasLogicHeight() const = 0;
    virtual Engine::Vec2 physic2Logic(int x, int y) const = 0;
    virtual Engine::Vec2 physic2Logic(const Engine::Vec2i& v) const = 0;
    virtual double physic2LogicX(int x) const = 0;
    virtual double physic2LogicY(int y) const = 0;
    virtual Engine::Vec2i logic2Physic(const Engine::Vec2i& v) const = 0;
    // virtual Engine::FrameBuffer* logicFbo() const = 0;
    // virtual Engine::Texture2D* logicCanvas() const = 0;

    virtual void update(unsigned long long tm) = 0;
    virtual void onMonitorSizeChanged(int width, int height) = 0;
    virtual void onCanvasResize(int width, int height) = 0;
    virtual void onRender(long long lElapsedTm) = 0;
    virtual void onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onKeyUp(int key, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift) = 0;
    virtual void onFocusedCellChanged() = 0;
    virtual void onViewEvent(const std::string& event, const SketchParamMap& props) = 0;

    virtual ViewCamera* backCam() const = 0;

    virtual const std::unordered_map<long long, View*>& views() const = 0;

    virtual LayoutCell* getCell(long long id) const = 0;
    virtual View* getView(long long id) const = 0;
    virtual View* getView(const std::wstring& name) = 0;

    virtual TileViewLayout* layout() const = 0;
    virtual RenderScene* scene() const = 0;
    virtual RenderScene* compEditScene() const = 0;
    virtual ResManager* resMgr() const = 0;
};

}// namespace sketch
