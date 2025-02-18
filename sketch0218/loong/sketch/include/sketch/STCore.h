#pragma once

#include "Core.h"
#include <functional>
#include <list>
#include <queue>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "Engine/Math/Vec2i.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/Path.h"

namespace sketch
{
class CmdManager;
class Command;
class DocManager;
class ViewManager;
class ControllerManager;
class ScriptManager;
class ScriptContext;
class RPCManager;
class View;
class HangUpJob;
class IRender;

class CORE_DLL_DECLSPEC STCore
{
public:
    static STCore* inst;

public:
    ~STCore(void);

    inline DocManager* docMgr() const { return m_pDocMgr; }

    inline ViewManager* viewMgr() const { return m_pViewMgr; }

    inline ControllerManager* ctrlMgr() const { return m_pCtrlMgr; }

    inline CmdManager* cmdMgr() const { return m_pCmdMgr; }

    inline ScriptManager* scriptMgr() const { return m_pScriptMgr; }

    inline RPCManager* rpcMgr() const { return m_pRPCMgr; }

    inline IRender* render() const { return m_render; }

    inline const Engine::Path& appDataPath() const { return m_appDataPath; }

    void init(HWND mainHandle, HWND viewHandle, HWND topBarHandle);
    void init_standalone();
    void dispose();
    void quit();

    void set_render(IRender* render);
    void use_internal_gizmo(const bool use_internal_gizmo) { m_use_internal_gizmo = use_internal_gizmo; }
    bool is_use_internal_gizmo() const { return m_use_internal_gizmo; }

    inline bool inited() const { return m_init; }

    inline void setModuleHandle(const HMODULE handle) { m_moduleHandle = handle; }

    inline HMODULE moduleHandle() const { return m_moduleHandle; }

    inline HWND mainHandle() const { return m_mainHandle; }

    inline HWND viewHandle() const { return m_viewHandle; }

    inline HWND activeHandle() const { return m_activeHandle; }

    inline HWND hoverHandle() const { return m_hoverHandle; }

    void messageLoop();
    void tick();
    void update();
    void onViewRender();
    void onViewResize(int width, int height);

    void addHangUp(HangUpJob* pJob);
    void updateHangUpJob();
    bool isHangUp();

    void uiOnDocNew(const std::wstring& projPath);
    void uiOnDocLoaded(const std::wstring& projPath);

    float uiProgress(const std::wstring& name);
    void setUIProgress(const std::wstring& name, float progress);
    void moveUIProgress(const std::wstring& name, float progress);
    void updateUIProgress(const std::wstring& name, float progress, const std::wstring& message = L"");
    void updateUIProgress(const std::wstring& name, const std::wstring& message = L"");

    void updateUIObjList();

    inline long long uniqueIdCounter() const { return m_idCounter; }

    inline void setUniqueIdCounter(const long long i) { m_idCounter = i; }

    inline long long uniqueId() { return m_idCounter++; }

    inline long long frame() const { return m_frame; }

    inline long long renderCounter() const { return m_renderCounter; }

    void dirtyView();

    void resetMouseButtonDown();

    inline int mouseButtonDown() const { return m_mouseButtonDown; }

    bool isMouseInView(View* pView) const;
    Engine::Vec2 mouseDownPos(View* pView) const;
    Engine::Vec2 mousePos(View* pView) const;

    int mouseDownPysicX() const;
    int mouseDownPysicY() const;
    int mousePhysicX() const;
    int mousePhysicY() const;
    int mouseDownLogicX() const;
    int mouseDownLogicY() const;
    int mouseLogicX() const;
    int mouseLogicY() const;
    bool dblClick() const;

protected:
    STCore(void);

    static LRESULT _onMainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);// MainForm STCore WndProc
    static LRESULT _onViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);// MainView STCore WndProc

    static LRESULT onViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);// MainView STCore WndProc

public:
    void onViewKeyDown(int key, bool bCtrl, bool bAlt, bool bShift);
    void onViewKeyUp(int key, bool bCtrl, bool bAlt, bool bShift);
    void onViewMouseDown(int button, bool bCtrl, bool bAlt, bool bShift);
    void onViewMouseMove(int button, bool bCtrl, bool bAlt, bool bShift);
    void onViewMouseUp(int button, bool bCtrl, bool bAlt, bool bShift);
    void onViewMouseDoubleClick(int button, bool bCtrl, bool bAlt, bool bShift);
    void onViewMouseWheel(int button, int delta, bool bCtrl, bool bAlt, bool bShift) const;
    void setMousePos(int x, int y);// left down is (0, 0)

protected:
    bool m_init;
    bool m_quit;

    HMODULE m_moduleHandle;// hwnd of core dll module
    HWND m_mainHandle;     // hwnd of main form
    HWND m_viewHandle;     // hwnd of main view
    HWND m_topBarHandle;   // hwnd of mainform top bar
    HWND m_activeHandle;
    HWND m_hoverHandle;

    DocManager* m_pDocMgr;
    ViewManager* m_pViewMgr;
    ControllerManager* m_pCtrlMgr;
    CmdManager* m_pCmdMgr;
    ScriptManager* m_pScriptMgr;
    RPCManager* m_pRPCMgr;

    IRender* m_render;

    bool m_use_internal_gizmo;

    // root path in appdata
    Engine::Path m_appDataPath;

    // unique id manager
    long long m_idCounter;

    // running frame counter
    long long m_frame;
    long long m_renderCounter;

    // mouse event
    int m_mouseButtonDown;
    Engine::Vec2i m_mousePhysicPos;
    Engine::Vec2i m_mouseDownPhysicPos;
    bool m_dblClick;

    // ui command call back
    std::list<Command*> m_uiCmds;
    std::unordered_map<std::wstring, float> m_progress;

    WNDPROC m_mainDefWndProc;// MainForm default WndProc
    WNDPROC m_viewDefWndProc;// MainView default WndProc

    // rendering
    bool m_viewDirty;

    // win32 resource
    HPEN m_frameBorderPen;

    // hangup tasks
    HangUpJob* m_runningHUJob;
    std::queue<HangUpJob*> m_waitingHUJobs;
};

}// namespace sketch
