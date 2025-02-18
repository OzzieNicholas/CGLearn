#include "sketch/STCore.h"

#include <chrono>
#include <shellapi.h>
#include <windef.h>
#include <windowsx.h>

#include "Engine/Util/Log.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/ShortCut/ShortCutManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/View/ControllerManager.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

#include "Engine/Render/Device.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/WinUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Command.h"
#include "sketch/Common/EventPool.h"
#include "sketch/NousEngine/NousEngine.h"
#include "sketch/RPC/RPCManager.h"
#include "sketch/Script/ScriptManager.h"
#include "sketch/UIFuncs.h"
#include "sketch/Util/HangUpJob.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderView/Render.h"

using namespace Engine;
// using namespace std::chrono;

namespace sketch
{

STCore* STCore::inst = new STCore();

STCore::STCore(void)
    : m_init(false), m_quit(false), m_idCounter(0L), m_moduleHandle(NULL), m_mainHandle(NULL), m_viewHandle(NULL), m_activeHandle(NULL),
      m_hoverHandle(NULL), m_topBarHandle(NULL), m_mainDefWndProc(nullptr), m_viewDefWndProc(nullptr), m_pDocMgr(nullptr), m_pViewMgr(nullptr),
      m_pCtrlMgr(nullptr), m_pCmdMgr(nullptr), m_pScriptMgr(nullptr), m_pRPCMgr(nullptr), m_render(nullptr), m_use_internal_gizmo(true),
      m_mouseButtonDown(-1), m_mousePhysicPos{ 0, 0 }, m_mouseDownPhysicPos{ 0, 0 }, m_dblClick(false), m_viewDirty(true), m_runningHUJob(nullptr)
{
    // 初始化Log文件名
    char tmstr[MAX_PATH] = { 0 };
    SYSTEMTIME tm;
    GetLocalTime(&tm);
    wsprintfA(tmstr, "%4d%02d%02d_%02d%02d%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

    m_appDataPath = SysUtil::appDataPath() / "nouveau";

    // SysUtil::setDmpPath(m_appDataPath / "dmps");

    Log::setLogPath(m_appDataPath / "logs");
    Log::initDefaultLogFile(Path(tmstr) + ".log");

    // 初始化Define映射表
    Define::init();

    m_frame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_frameBorderPen = CreatePen(PS_SOLID, 1, RGB(211, 211, 211));

    m_renderCounter = 0;
}

STCore::~STCore(void) { NousEngine::instance()->release(); }

void STCore::dispose()
{
    while(m_runningHUJob)
    {
        m_runningHUJob->updateOnMainThread();
        if(m_runningHUJob->isDone())
        {
            m_runningHUJob->onFinish();
            m_runningHUJob = nullptr;
        }

        if(!m_waitingHUJobs.empty())
        {
            m_runningHUJob = m_waitingHUJobs.front();
            m_waitingHUJobs.pop();
            m_runningHUJob->run();
        }
    }

    if(m_pCmdMgr)
        m_pCmdMgr->onAppClose();

    if(m_pDocMgr)
        m_pDocMgr->onAppClose();

    STE::dispose();
}

void STCore::init(HWND mainHandle, HWND viewHandle, HWND topBarHandle)
{
    Log::log("", "STCore::init >>>>>>>>>>>>>>>>>>>>>>>  \n");

    // 测试是否有AppData目录读写权限
    FILE* fp = fopen((m_appDataPath / "permission").str().c_str(), "w");
    if(!fp)
    {
        Log::log("", "[Error][STCore::init] Failed to write to " + m_appDataPath.str() + "\n");
        MessageBoxA(nullptr, (m_appDataPath.str() + " 没有读写权限!").c_str(), "系统错误", MB_OK);
    }
    else
        fclose(fp);

    m_mainHandle = mainHandle;
    m_viewHandle = viewHandle;
    m_topBarHandle = topBarHandle;

    // init engine
    STE::init(viewHandle);
    NousEngine::instance()->init();

    m_mainDefWndProc = (WNDPROC)GetWindowLongPtr(mainHandle, GWLP_WNDPROC);
    SetWindowLongPtr(mainHandle, GWLP_WNDPROC, (LONG_PTR)STCore::_onMainWndProc);

    m_viewDefWndProc = (WNDPROC)GetWindowLongPtr(viewHandle, GWLP_WNDPROC);
    SetWindowLongPtr(viewHandle, GWLP_WNDPROC, (LONG_PTR)STCore::_onViewWndProc);

    m_pCmdMgr = CmdManager::create();
    m_pCmdMgr->init();

    m_pDocMgr = new DocManager();
    m_pViewMgr = ViewManager::create();
    m_pCtrlMgr = ControllerManager::create();
    m_pScriptMgr = new ScriptManager();
    m_pRPCMgr = RPCManager::create();

    m_init = false;

    if(!m_pDocMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init DocManager");

        return;
    }

    if(!m_pViewMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ViewManager");

        return;
    }

    if(!m_pCtrlMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ControllerManager");

        return;
    }

    if(!m_pScriptMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ScriptManager");

        return;
    }

    if(!m_pRPCMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init RPCManager");

        return;
    }

    set_render(new Render());

    m_init = true;

    if(UIFuncs::onCoreInited)
        UIFuncs::onCoreInited();
}

void STCore::init_standalone()
{
    Log::log("", "STCore::init standalone >>>>>>>>>>>>>>>>>>>>>>>  \n");

    // 测试是否有AppData目录读写权限
    FILE* fp = fopen((m_appDataPath / "permission").str().c_str(), "w");
    if(!fp)
    {
        Log::log("", "[Error][STCore::init] Failed to write to " + m_appDataPath.str() + "\n");
        MessageBoxA(nullptr, (m_appDataPath.str() + " 没有读写权限!").c_str(), "系统错误", MB_OK);
    }
    else
        fclose(fp);

    // init engine
    STE::init(nullptr);
    NousEngine::instance()->init();

    m_pCmdMgr = CmdManager::create();
    m_pCmdMgr->init();

    m_pDocMgr = new DocManager();
    m_pViewMgr = ViewManager::create();
    m_pCtrlMgr = ControllerManager::create();
    m_pScriptMgr = new ScriptManager();
    m_pRPCMgr = RPCManager::create();

    m_init = false;

    if(!m_pDocMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init DocManager");

        return;
    }

    if(!m_pViewMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ViewManager");

        return;
    }

    if(!m_pCtrlMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ControllerManager");

        return;
    }

    if(!m_pScriptMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init ScriptManager");

        return;
    }

    if(!m_pRPCMgr->init())
    {
        if(UIFuncs::onCoreFailed)
            UIFuncs::onCoreFailed("Failed to init RPCManager");

        return;
    }

    m_init = true;

    if(UIFuncs::onCoreInited)
        UIFuncs::onCoreInited();
}

void STCore::quit() { m_quit = true; }

void STCore::set_render(IRender* render) { m_render = render; }

void STCore::messageLoop()
{
    MSG msg;
    while(TRUE)
    {
        if(m_quit)
            break;

        m_frame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        RECT viewRect;
        GetClientRect(STCore::inst->viewHandle(), &viewRect);

        POINT mp;
        GetCursorPos(&mp);// mouse pos in screen position

        m_activeHandle = GetActiveWindow();
        m_hoverHandle = WindowFromPoint(mp);
        ScreenToClient(STCore::inst->viewHandle(), &mp);// convert mousepos to view

        m_mousePhysicPos.set(mp.x, mp.y);
        m_mousePhysicPos.y = (viewRect.bottom - viewRect.top) - m_mousePhysicPos.y;// revert axis-Y

        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(!m_init)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }

            bool bResolved = false;

            if(!bResolved)
                TranslateMessage(&msg);

            bool bIOMsg = false;
            switch(msg.message)
            {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYUP:
            case WM_MOUSEMOVE:
            case WM_MOUSELEAVE:
            case WM_MOUSEHOVER:
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
                bIOMsg = true;
                if(UIFuncs::filterMsg)
                {
                    bResolved |= UIFuncs::filterMsg(&msg);
                }
                break;
            }

            // 当前有HangUpJob, 屏蔽用户IO消息
            if(bIOMsg && STCore::inst->isHangUp())
                continue;

            // 处理快捷键
            if(m_activeHandle == STCore::inst->mainHandle() || m_activeHandle == STCore::inst->viewHandle())
            {
                switch(msg.message)
                {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN: STCore::inst->docMgr()->cfgMgr()->shortCutMgr()->onKeyDown((int)msg.wParam); break;
                case WM_KEYUP: STCore::inst->docMgr()->cfgMgr()->shortCutMgr()->onKeyUp((int)msg.wParam); break;
                }
            }

            if(m_activeHandle != STCore::inst->viewHandle() && m_activeHandle != STCore::inst->mainHandle() &&
               m_hoverHandle == STCore::inst->viewHandle())
            {
                if(msg.message == WM_MOUSEMOVE || msg.message == WM_MOUSEWHEEL)
                {
                    STCore::_onViewWndProc(msg.hwnd, msg.message, msg.wParam,
                                           m_mousePhysicPos.x | (((viewRect.bottom - viewRect.top) - m_mousePhysicPos.y) << 16));
                }
            }

            if(m_activeHandle)
            {
                switch(msg.message)
                {
                case WM_KEYDOWN: {
                    if(!bResolved)
                        DispatchMessage(&msg);

                    switch(msg.wParam)
                    {
                    case VK_RETURN: UIFuncs::performAcceptButton(m_activeHandle); break;
                    case VK_ESCAPE: UIFuncs::performEscClose(m_activeHandle); break;
                    case VK_TAB: UIFuncs::performTabSwitch(m_activeHandle); break;
                    }

                    bResolved = true;
                }
                break;
                }
            }

            if(bResolved)
                continue;

            DispatchMessage(&msg);
        }
        else if(m_init)
        {
            static long long tm0 = 0;

            long long tm = GetTickCount64();
            if(tm - tm0 >= 15)
            {
                this->update();
                tm0 = tm;
            }
            else
                Sleep(1);// 空闲时释放CPU时间碎片

            if(m_viewDirty || m_activeHandle == STCore::inst->mainHandle() || m_hoverHandle == STCore::inst->viewHandle() ||
               STCore::inst->viewMgr()->layout()->hasDirtyCells())
            {
                this->onViewRender();
                m_viewDirty = false;
            }
        }
    }
}

void STCore::tick()
{
    // MSG msg;

    if(m_quit)
        return;

    m_frame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if(m_init)
    {
        static long long tm0 = 0;

        long long tm = GetTickCount64();
        if(tm - tm0 >= 15)
        {
            this->update();
            tm0 = tm;
        }
        else
            Sleep(1);// 空闲时释放CPU时间碎片

        if(m_viewDirty || m_activeHandle == STCore::inst->mainHandle() || m_hoverHandle == STCore::inst->viewHandle() ||
           STCore::inst->viewMgr()->layout()->hasDirtyCells())
        {
            this->onViewRender();
            m_viewDirty = false;
        }
    }
}

LRESULT STCore::_onMainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_GETMINMAXINFO: {
        // set maximize size
        // NOTE: Primary Monitor: 1920x1080(100%), Secondary Monitor: 2560x1440(100%), maximize incorrectly on secondary monitor, need SizeChanged
        // callback to fix it
        RECT rcWork = WU::screenWork(STCore::inst->m_mainHandle);
        SIZE border = WU::borderSize(STCore::inst->m_mainHandle);

        LPMINMAXINFO info = (LPMINMAXINFO)lParam;
        info->ptMaxPosition.x = -border.cx;
        info->ptMaxPosition.y = -border.cy;
        info->ptMaxSize.x = rcWork.right - rcWork.left + border.cx * 2;
        info->ptMaxSize.y = rcWork.bottom - rcWork.top + border.cy * 2;
        return 0;
    }
    break;
    case WM_SIZE: {
        SIZE border = WU::borderSize(STCore::inst->m_mainHandle);
        SIZE cs = WU::clientSize(STCore::inst->m_mainHandle);

        int x0 = border.cx;
        int y0 = border.cy;
        int x1 = x0 + cs.cx;
        int y1 = y0 + cs.cy;

        HRGN hrgn = CreateRectRgn(x0 - 1, y0 - 1, x1 + 1, y1 + 1);
        SetWindowRgn(STCore::inst->m_mainHandle, hrgn, true);
        return 0;
    }
    break;
    case WM_WINDOWPOSCHANGING: {
        LPWINDOWPOS wp = (LPWINDOWPOS)lParam;

        auto mi = WU::screen(wp->x, wp->y);
        RECT rcWork = mi.rcWork;

        wp->cx = STMAX(wp->cx, 800);
        wp->cy = STMAX(wp->cy, 600);

        if((wp->x == rcWork.left || wp->x == rcWork.right - wp->cx)// snap left/right
           && wp->y == rcWork.top && wp->cx == (rcWork.right - rcWork.left) / 2 && wp->cy == rcWork.bottom - rcWork.top)
        {
            SIZE border = WU::borderSize(STCore::inst->m_mainHandle);
            wp->x -= border.cx;
            wp->y -= border.cy;
            wp->cx += border.cx * 2;
            wp->cy += border.cy * 2;
        }
        else if(wp->y == rcWork.top && wp->cy == rcWork.bottom - rcWork.top)
        {// snap top/bottom
            SIZE border = WU::borderSize(STCore::inst->m_mainHandle);
            wp->y -= border.cy;
            wp->cy += border.cy * 2;
        }

        return 0;
    }
    break;
    case WM_WINDOWPOSCHANGED: {
        auto mi = WU::screen(STCore::inst->mainHandle());

        if(STCore::inst->m_init)// 更新当前所在显示器分辨率
            STCore::inst->m_pViewMgr->onMonitorSizeChanged(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
    }
    break;
    case WM_NCHITTEST: {
        POINT pos = { (short)(lParam & 0xffff), (short)((lParam >> 16) & 0xffff) };
        ScreenToClient(STCore::inst->m_mainHandle, &pos);

        WINDOWPLACEMENT wndpl;
        wndpl.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(STCore::inst->m_mainHandle, &wndpl);

        SIZE clientSize = WU::clientSize(STCore::inst->m_mainHandle);

        uint32_t flags = 0;
        if(wndpl.showCmd == SW_NORMAL)
        {
            if(pos.x <= Define::MAINFORM_BORDER_MARGIN)// left
                flags |= 1;
            if(clientSize.cx - pos.x <= Define::MAINFORM_BORDER_MARGIN)// right
                flags |= 2;
            if(pos.y <= Define::MAINFORM_BORDER_MARGIN)// top
                flags |= 4;
            if(clientSize.cy - pos.y <= Define::MAINFORM_BORDER_MARGIN)// bottom
                flags |= 8;
        }
        else if(wndpl.showCmd == SW_MAXIMIZE && WU::isTaskBarAutoHide() && !STCore::inst->viewMgr()->layout()->isBusy())
        {
            // 最大化时，因透明border向外扩展了一圈，因此autohide的任务栏无法被激活，需手动探测
            RECT rcWindow;
            GetWindowRect(STCore::inst->m_mainHandle, &rcWindow);

            auto scr = WU::screen(STCore::inst->m_mainHandle);
            int barSide = WU::taskBarEdge();

            if(barSide == ABE_BOTTOM && rcWindow.bottom >= scr.rcMonitor.bottom && clientSize.cy - pos.y <= Define::MAINFORM_BORDER_MARGIN)
            {// task bar at bottom
                flags |= 16;
            }
            else if(barSide == ABE_TOP && rcWindow.top <= scr.rcMonitor.top && pos.y <= Define::MAINFORM_BORDER_MARGIN)
            {// task bar at top
                flags |= 16;
            }
            else if(barSide == ABE_LEFT && rcWindow.left <= scr.rcMonitor.left && pos.x <= Define::MAINFORM_BORDER_MARGIN)
            {// task bar at left
                flags |= 16;
            }
            else if(barSide == ABE_RIGHT && rcWindow.right >= scr.rcMonitor.right && clientSize.cx - pos.x <= Define::MAINFORM_BORDER_MARGIN)
            {// task bar at right
                flags |= 16;
            }
        }

        switch(flags)
        {
        case 0: {
            if(pos.y < WU::size(STCore::inst->m_topBarHandle).cy)
            {
                return HTCAPTION;
            }
        }
        break;
        case 1: return HTLEFT;
        case 2: return HTRIGHT;
        case 4: return HTTOP;
        case 8: return HTBOTTOM;
        case 5: return HTTOPLEFT;
        case 6: return HTTOPRIGHT;
        case 9: return HTBOTTOMLEFT;
        case 10: return HTBOTTOMRIGHT;
        case 16: {
            BringWindowToTop(WU::taskBarHwnd(STCore::inst->m_mainHandle));
        }
        break;
        }

        return 0;
    }
    break;
    case WM_ACTIVATE:
    case WM_NCPAINT: {// draw non-client area
        SIZE border = WU::borderSize(STCore::inst->m_mainHandle);
        SIZE cs = WU::clientSize(STCore::inst->m_mainHandle);

        int x0 = border.cx - 1;
        int y0 = border.cy - 1;
        int x1 = x0 + cs.cx + 1;
        int y1 = y0 + cs.cy + 1;

        HDC hdc = GetWindowDC(STCore::inst->m_mainHandle);

        // set color
        SelectObject(hdc, STCore::inst->m_frameBorderPen);

        // draw border frame
        MoveToEx(hdc, x0, y0, NULL);
        LineTo(hdc, x0, y1);
        LineTo(hdc, x1, y1);
        LineTo(hdc, x1, y0);
        LineTo(hdc, x0, y0);

        ReleaseDC(STCore::inst->m_mainHandle, hdc);
        return 0;
    }
    }

    return STCore::inst->m_mainDefWndProc(hwnd, msg, wParam, lParam);
}

LRESULT STCore::_onViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_SIZE: {
        short w = (short)(lParam & 0xffffu);
        short h = (short)((lParam >> 16) & 0xffffu);
        STCore::inst->onViewResize(w, h);
        STCore::inst->onViewRender();
        return 0;
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        int button = (msg == WM_LBUTTONDOWN) ? 0 : (msg == WM_MBUTTONDOWN) ? 1 : ((msg == WM_RBUTTONDOWN) ? 2 : -1);

        SetFocus(STCore::inst->m_viewHandle);
        SetCapture(STCore::inst->m_viewHandle);

        STCore::inst->onViewMouseDown(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_MOUSEMOVE: {
        int button = ((wParam & MK_LBUTTON) != 0) ? 0 : ((wParam & MK_MBUTTON) != 0) ? 1 : (((wParam & MK_RBUTTON) != 0) ? 2 : -1);

        STCore::inst->onViewMouseMove(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        int button = (msg == WM_LBUTTONUP) ? 0 : (msg == WM_MBUTTONUP) ? 1 : ((msg == WM_RBUTTONUP) ? 2 : -1);

        STCore::inst->onViewMouseUp(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        ReleaseCapture();
        return 0;
    }
    break;
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
        int button = (msg == WM_LBUTTONDBLCLK) ? 0 : (msg == WM_MBUTTONDBLCLK) ? 1 : ((msg == WM_RBUTTONDBLCLK) ? 2 : -1);

        STCore::inst->onViewMouseDoubleClick(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_MOUSEWHEEL: {
        int button = ((wParam & MK_LBUTTON) != 0) ? 0 : ((wParam & MK_MBUTTON) != 0) ? 1 : (((wParam & MK_RBUTTON) != 0) ? 2 : -1);

        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        STCore::inst->onViewMouseWheel(button, delta, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        STCore::inst->dirtyView();
        return 0;
    }
    break;
    case WM_KEYDOWN: {
        STCore::inst->onViewKeyDown((int)wParam, GetAsyncKeyState(VK_CONTROL) & 0x8000, GetAsyncKeyState(VK_MENU) & 0x8000,
                                    GetAsyncKeyState(VK_SHIFT) & 0x8000);
        return 0;
    }
    break;
    case WM_KEYUP: {
        STCore::inst->onViewKeyUp((int)wParam, GetAsyncKeyState(VK_CONTROL) & 0x8000, GetAsyncKeyState(VK_MENU) & 0x8000,
                                  GetAsyncKeyState(VK_SHIFT) & 0x8000);
        return 0;
    }
    break;
    case WM_MOUSELEAVE:
    case WM_KILLFOCUS: {
        // do nothing
    }
    break;
    case WM_SETFOCUS: {
        // do nothing
    }
    break;
    }

    return STCore::inst->m_viewDefWndProc(hwnd, msg, wParam, lParam);
}

LRESULT STCore::onViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_SIZE: {
        short w = (short)(lParam & 0xffffu);
        short h = (short)((lParam >> 16) & 0xffffu);
        STCore::inst->onViewResize(w, h);
        STCore::inst->onViewRender();
        return 0;
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        int button = (msg == WM_LBUTTONDOWN) ? 0 : (msg == WM_MBUTTONDOWN) ? 1 : ((msg == WM_RBUTTONDOWN) ? 2 : -1);

        // int x = GET_X_LPARAM(lParam);
        // int y = GET_Y_LPARAM(lParam);

        SetFocus(STCore::inst->m_viewHandle);
        SetCapture(STCore::inst->m_viewHandle);

        STCore::inst->onViewMouseDown(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_MOUSEMOVE: {
        int button = ((wParam & MK_LBUTTON) != 0) ? 0 : ((wParam & MK_MBUTTON) != 0) ? 1 : (((wParam & MK_RBUTTON) != 0) ? 2 : -1);

        // int x = GET_X_LPARAM(lParam);
        // int y = GET_Y_LPARAM(lParam);

        STCore::inst->onViewMouseMove(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        int button = (msg == WM_LBUTTONUP) ? 0 : (msg == WM_MBUTTONUP) ? 1 : ((msg == WM_RBUTTONUP) ? 2 : -1);

        STCore::inst->onViewMouseUp(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        ReleaseCapture();
        return 0;
    }
    break;
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
        int button = (msg == WM_LBUTTONDBLCLK) ? 0 : (msg == WM_MBUTTONDBLCLK) ? 1 : ((msg == WM_RBUTTONDBLCLK) ? 2 : -1);

        STCore::inst->onViewMouseDoubleClick(button, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        return 0;
    }
    break;
    case WM_MOUSEWHEEL: {
        int button = ((wParam & MK_LBUTTON) != 0) ? 0 : ((wParam & MK_MBUTTON) != 0) ? 1 : (((wParam & MK_RBUTTON) != 0) ? 2 : -1);

        short delta = GET_WHEEL_DELTA_WPARAM(wParam);

        STCore::inst->onViewMouseWheel(button, delta, (wParam & MK_CONTROL) != 0, GetAsyncKeyState(VK_MENU) & 0x8000, (wParam & MK_SHIFT) != 0);

        STCore::inst->dirtyView();
        return 0;
    }
    break;
    case WM_KEYDOWN: {
        STCore::inst->onViewKeyDown((int)wParam, GetAsyncKeyState(VK_CONTROL) & 0x8000, GetAsyncKeyState(VK_MENU) & 0x8000,
                                    GetAsyncKeyState(VK_SHIFT) & 0x8000);
        return 0;
    }
    break;
    case WM_MOUSELEAVE:
    case WM_KILLFOCUS: {
        // do nothing
    }
    break;
    case WM_SETFOCUS: {
        // do nothing
    }
    break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void STCore::onViewRender()
{
    static long long last_tm = 0;
    m_renderCounter++;

    if(!m_init)
        return;

    if(m_runningHUJob)
        return;

    if(m_pViewMgr)
    {
        long long dur = 0;
        // auto tm = GetTickCount64();
        // auto dur = tm - last_tm;
        // if (dur >= 15) {
        m_pViewMgr->onRender(dur);
        //	last_tm = tm;
        //}
    }
}

void STCore::resetMouseButtonDown() { m_mouseButtonDown = -1; }

bool STCore::isMouseInView(View* pView) const
{
    if(!pView)
        return false;

    auto& vp = pView->camera()->viewport();
    auto logicPos = m_pViewMgr->physic2Logic(m_mousePhysicPos);
    return RANGE_IN(logicPos.x, vp.x, vp.x + vp.z) && RANGE_IN(logicPos.y, vp.y, vp.y + vp.w);
}

Engine::Vec2 STCore::mouseDownPos(View* pView) const
{
    auto& vp = pView->camera()->viewport();
    auto logicPos = m_pViewMgr->physic2Logic(m_mouseDownPhysicPos);
    return { double(logicPos.x) - double(vp.x), double(logicPos.y) - double(vp.y) };
}

Engine::Vec2 STCore::mousePos(View* pView) const
{
    auto& vp = pView->camera()->viewport();
    auto logicPos = m_pViewMgr->physic2Logic(m_mousePhysicPos);
    return { double(logicPos.x) - double(vp.x), double(logicPos.y) - double(vp.y) };
}

void STCore::dirtyView() { m_viewDirty = true; }

void STCore::updateHangUpJob()
{
    // process hangup jobs
    if(!m_runningHUJob)
        return;

    m_runningHUJob->updateOnMainThread();

    if(m_runningHUJob->isDone())
    {
        m_runningHUJob->onFinish();
        m_runningHUJob = nullptr;
    }

    if(!m_waitingHUJobs.empty())
    {
        m_runningHUJob = m_waitingHUJobs.front();
        m_waitingHUJobs.pop();
        m_runningHUJob->run();
    }
}

void STCore::update()
{
    unsigned long long tm = GetTickCount64();

    // process hangup jobs
    this->updateHangUpJob();

    if(!m_runningHUJob)
    {
        // dispatch UI commands
        EventPool::instance()->trigger_all();

        if(m_pCmdMgr)
            m_pCmdMgr->update();

        if(m_pDocMgr)
            m_pDocMgr->update(tm);

        if(m_pViewMgr)
            m_pViewMgr->update(tm);

        if(m_pCtrlMgr)
            m_pCtrlMgr->update(tm);

        if(m_pRPCMgr)
            m_pRPCMgr->update(tm);
    }

    STE::update();
}

void STCore::onViewResize(int width, int height)
{
    if(STE::device())
        STE::device()->onWindowResize(width, height);

    if(!m_init)
        return;

    if(m_pViewMgr)
        m_pViewMgr->onCanvasResize(width, height);
}

bool STCore::isHangUp() { return m_runningHUJob; }

void STCore::addHangUp(HangUpJob* pJob)
{
    if(!m_runningHUJob)
    {
        m_runningHUJob = pJob;
        m_runningHUJob->run();
        return;
    }

    m_waitingHUJobs.push(pJob);
}

#if SIM_UI_INPUT
static std::string s_current_input = "";
void STCore::onViewKeyDown(int key, bool bCtrl, bool bAlt, bool bShift)
{
    // 模拟c#输入，并通知c#ui更新。
    if(key == 8)
    {// 退格键
        if(!s_current_input.empty())
        {
            s_current_input.pop_back();
        }
    }
    else if(key == 13)
    {// 回车键
        SketchParamMap para;
        para.set_value("value", StrUtil::s2w(s_current_input));
        m_pCtrlMgr->enter(para);
    }
    else if(std::isprint(key))
    {
        if(!m_pCtrlMgr->active_controller_is_lock())
        {
            s_current_input.clear();
        }
        s_current_input += Util::winkey_to_char(key);
        m_pCtrlMgr->event_filter("controller_input", SketchParamMap());
    }

    SketchParamMap para;
    para.set_value("value", StrUtil::s2w(s_current_input));
    para.set_value("name", "key_down");
    EventPool::instance()->push_event("sim_input", para);
#else
void STCore::onViewKeyDown(int key, bool bCtrl, bool bAlt, bool bShift)
{
#endif// SIM_UI_INPUT

    if(m_pViewMgr)
        m_pViewMgr->onKeyDown(key, bCtrl, bAlt, bShift);

    if(m_pCtrlMgr)
        m_pCtrlMgr->key_down(key, bCtrl, bAlt, bShift);
}

void STCore::onViewKeyUp(int key, bool bCtrl, bool bAlt, bool bShift)
{
    if(m_pViewMgr)
        m_pViewMgr->onKeyUp(key, bCtrl, bAlt, bShift);

    if(m_pCtrlMgr)
        m_pCtrlMgr->key_up(key, bCtrl, bAlt, bShift);
}

void STCore::onViewMouseDown(int button, bool bCtrl, bool bAlt, bool bShift)
{
    if(!m_init || m_pViewMgr == nullptr)
        return;

    m_mouseButtonDown = button;
    m_mouseDownPhysicPos = m_mousePhysicPos;
    m_dblClick = false;

    m_pViewMgr->onMouseDown(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    m_pCtrlMgr->mouse_down(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
}

void STCore::onViewMouseMove(int button, bool bCtrl, bool bAlt, bool bShift)
{
    if(!m_init || m_pViewMgr == nullptr)
        return;

    m_pViewMgr->onMouseMove(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    m_pCtrlMgr->mouse_move(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
}

void STCore::onViewMouseUp(int button, bool bCtrl, bool bAlt, bool bShift)
{
    if(!m_init || m_pViewMgr == nullptr)
        return;

    if(!m_dblClick && abs(m_mousePhysicPos.x - m_mouseDownPhysicPos.x) <= 4 && fabs(m_mousePhysicPos.y - m_mouseDownPhysicPos.y) <= 4)
    {
        m_pViewMgr->onMouseClick(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    }

    m_pViewMgr->onMouseUp(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);

    if(m_pCtrlMgr)
    {
        m_pCtrlMgr->mouse_up(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    }

    m_mouseButtonDown = -1;
    m_dblClick = false;
}

void STCore::onViewMouseDoubleClick(int button, bool bCtrl, bool bAlt, bool bShift)
{
    if(!m_init || m_pViewMgr == nullptr)
        return;

    if(abs(m_mousePhysicPos.x - m_mouseDownPhysicPos.x) > 4 || fabs(m_mousePhysicPos.y - m_mouseDownPhysicPos.y) > 4)
        return;

    m_dblClick = true;

    m_pViewMgr->onMouseDoubleClick(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    if(m_pCtrlMgr)
    {
        m_pCtrlMgr->mouse_double_click(button, m_mousePhysicPos.x, m_mousePhysicPos.y, bCtrl, bAlt, bShift);
    }
}

void STCore::onViewMouseWheel(int button, int delta, bool bCtrl, bool bAlt, bool bShift) const
{
    if(!m_init)
        return;

    if(m_pViewMgr)
        m_pViewMgr->onMouseWheel(button, m_mousePhysicPos.x, m_mousePhysicPos.y, delta, bCtrl, bAlt, bShift);

    if(m_pCtrlMgr)
    {
        m_pCtrlMgr->mouse_wheel(button, m_mousePhysicPos.x, m_mousePhysicPos.y, delta, bCtrl, bAlt, bShift);
    }
}

void STCore::setMousePos(int x, int y) { m_mousePhysicPos.set(x, y); }

void STCore::updateUIProgress(const std::wstring& name, float progress, const std::wstring& message)
{
    m_progress[name] = progress;
    if(UIFuncs::updateProgressBar)
    {
        UIFuncs::updateProgressBar(name, (int)progress, message);
    }
}

void STCore::updateUIProgress(const std::wstring& name, const std::wstring& message)
{
    if(UIFuncs::updateProgressBar)
    {
        UIFuncs::updateProgressBar(name, (int)this->uiProgress(name), message);
    }
}

float STCore::uiProgress(const std::wstring& name)
{
    auto it = m_progress.find(name);
    return (it != m_progress.end()) ? it->second : 0.0f;
}

void STCore::setUIProgress(const std::wstring& name, float progress) { m_progress[name] = progress; }

void STCore::moveUIProgress(const std::wstring& name, float movProg)
{
    auto it = m_progress.find(name);
    if(it != m_progress.end())
        it->second += movProg;
    else
        m_progress[name] = movProg;
}

void STCore::updateUIObjList() { EventPool::instance()->push_event("obj_list_changed", {}); }

void STCore::uiOnDocNew(const std::wstring& projPath)
{
    EventPool::instance()->push_event("on_doc_new", { { "proj_path", projPath } });

    // Command* pCmd = Command::alloc("on_doc_new");
    // pCmd->set("proj_path", projPath);
    // this->pushUICommand(pCmd);
}

void STCore::uiOnDocLoaded(const std::wstring& projPath)
{
    EventPool::instance()->push_event("on_doc_loaded", { { "proj_path", projPath } });

    // Command* pCmd = Command::alloc("on_doc_loaded");
    // pCmd->set("proj_path", projPath);
    // this->pushUICommand(pCmd);
}

int STCore::mouseDownPysicX() const { return m_mouseDownPhysicPos.x; }

int STCore::mouseDownPysicY() const { return m_mouseDownPhysicPos.y; }

int STCore::mousePhysicX() const { return m_mousePhysicPos.x; }

int STCore::mousePhysicY() const { return m_mousePhysicPos.y; }

int STCore::mouseDownLogicX() const { return m_pViewMgr->physic2LogicX(m_mouseDownPhysicPos.x); }

int STCore::mouseDownLogicY() const { return m_pViewMgr->physic2LogicY(m_mouseDownPhysicPos.y); }

int STCore::mouseLogicX() const { return m_pViewMgr->physic2LogicX(m_mousePhysicPos.x); }

int STCore::mouseLogicY() const { return m_pViewMgr->physic2LogicY(m_mousePhysicPos.y); }

bool STCore::dblClick() const { return m_dblClick; }

}// namespace sketch
