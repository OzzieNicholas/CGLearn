using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;

namespace LoongCAD.Util
{
    public unsafe class Win32
    {
        private Win32()
        {
        }

        private static Size m_formBorderSize = new Size(-1, -1);
        public static Size formBorderSize {
            get {
                if (m_formBorderSize.Width < 0) {
                    int cxframe = Win32.GetSystemMetrics(Win32.SM_CXFRAME);
                    int cyframe = Win32.GetSystemMetrics(Win32.SM_CYFRAME);
                    int cxpad = Win32.GetSystemMetrics(Win32.SM_CXPADDEDBORDER);
                    m_formBorderSize = new Size(cxframe + cxpad, cyframe + cxpad);
                }

                return m_formBorderSize;
            }
        }


        ////////////////////////////////////////////////// Windows ///////////////////////////////////////////////////////
        public const int WS_EX_TOPMOST = 0x8;
        public const int WS_EX_TRANSPARENT = 0x20;
        public const int WS_BORDER = 0x00800000;
        public const int WS_CAPTION = 0x00C00000;
        public const int WS_CHILD = 0x40000000;
        public const int WS_CHILDWINDOW = 0x40000000;
        public const int WS_CLIPCHILDREN = 0x02000000;
        public const int WS_CLIPSIBLINGS = 0x04000000;
        public const int WS_DISABLED = 0x08000000;
        public const int WS_DLGFRAME = 0x00400000;
        public const int WS_GROUP = 0x00020000;
        public const int WS_HSCROLL = 0x00100000;
        public const int WS_OVERLAPPED = 0x00000000;
        public const uint WS_POPUP = 0x80000000;
        public const int WS_SIZEBOX = 0x00040000;
        public const int WS_THICKFRAME = 0x00040000;

        public const int CS_HREDRAW = 0x0002;
        public const int CS_VREDRAW = 0x0001;

        public const int EM_SETTABSTOPS = 0xCB;
        public const int EM_SETSCROLLPOS = 0x4DE;
        public const int EM_SETCUEBANNER = 0x1501;

        public const int MA_ACTIVATE = 1;
        public const int MA_ACTIVATEANDEAT = 2;
        public const int MA_NOACTIVATE = 3;
        public const int MA_NOACTIVATEANDEAT = 4;

        public const int HTTRANSPARENT = -1;
        public const int HTCLIENT = 1;
        public const int HTCAPTION = 2;
        public const int HTMINBUTTON = 8;
        public const int HTMAXBUTTON = 9;
        public const int HTLEFT = 10;
        public const int HTRIGHT = 11;
        public const int HTTOP = 12;
        public const int HTTOPLEFT = 13;
        public const int HTTOPRIGHT = 14;
        public const int HTBOTTOM = 15;
        public const int HTBOTTOMLEFT = 16;
        public const int HTBOTTOMRIGHT = 17;
        public const int HTBORDER = 18;
        public const int HTCLOSE = 20;

        public const int SC_SIZE = 0xF000;
        public const int SC_MOVE = 0xF010;
        public const int SC_MINIMIZE = 0xF020;
        public const int SC_MAXIMIZE = 0xF030;
        public const int SC_CLOSE = 0xF060;
        public const int SC_RESTORE = 0xF120;

        public const int WM_DISPLAYCHANGE = 0x007E;
        public const int WM_CREATE = 0x0001;
        public const int WM_NCCREATE = 0x0081;
        public const int WM_ACTIVATE = 0x0006;
        public const int WM_NCHITTEST = 0x0084;
        public const int WM_NCCALCSIZE = 0x83;
        public const int WM_SIZE = 0x0005;
        public const int WM_GETMINMAXINFO = 0x0024;
        public const int WM_SYSCOMMAND = 0x0112;
        public const int WM_MDIACTIVATE = 0x0222;
        public const int WM_SETREDRAW = 11;
        public const int WM_PAINT = 0x000F;
        public const int WM_NCPAINT = 0x85;
        public const int WM_ERASEBKGND = 0x0014;
        public const int WM_PRINTCLIENT = 0x0318;
        public const int WM_DRAWITEM = 0x002B;
        public const int WM_KEYDOWN = 0x0100;
        public const int WM_KEYUP = 0x0101;
        public const int WM_SYSKEYDOWN = 0x0104;
        public const int WM_SYSKEYUP = 0x0105;
        public const int WM_LBUTTONDOWN = 0x0201;
        public const int WM_LBUTTONUP = 0x0202;
        public const int WM_RBUTTONDOWN = 0x0204;
        public const int WM_RBUTTONUP = 0x0205;
        public const int WM_MBUTTONDOWN = 0x0207;
        public const int WM_MBUTTONUP = 0x0208;
        public const int WM_LBUTTONDBLCLK = 0x0203;
        public const int WM_RBUTTONDBLCLK = 0x0206;
        public const int WM_MBUTTONDBLCLK = 0x0209;
        public const int WM_MOUSEACTIVATE = 0x21;
        public const int WM_MOUSEMOVE = 0x0200;
        public const int WM_MOUSEWHEEL = 0x020A;
        public const int WM_MOUSEHOVER = 0x02A1;
        public const int WM_MOUSELEAVE = 0x02A3;
        public const int WM_WINDOWPOSCHANGING = 0x0046;
        public const int WM_MOVING = 0x0216;
        public const int WM_SETFOCUS = 0x0007;
        public const int WM_KILLFOCUS = 0x0008;

        public const int GWL_STYLE = -16;

        public const int WS_MINIMIZE = 0x20000000;
        public const int WS_MAXIMIZE = 0x01000000;
        public const int WS_SYSMENU	= 0x00080000;

        public const int WVR_ALIGNTOP = 0x10;
        public const int WVR_ALIGNLEFT = 0x20;
        public const int WVR_ALIGNBOTTOM = 0x40;
        public const int WVR_ALIGNRIGHT = 0x80;
        public const int WVR_HREDRAW = 0x100;
        public const int WVR_VREDRAW = 0x200;
        public const int WVR_REDRAW = (WVR_HREDRAW | WVR_VREDRAW);
        public const int WVR_VALIDRECTS = 0x400;

        public const int DCX_WINDOW = 0x1;
        public const int DCX_CACHE = 0x2;
        public const int DCX_CLIPCHILDREN = 0x8;
        public const int DCX_CLIPSIBLINGS = 0x10;
        public const int DCX_PARENTCLIP = 0x20;
        public const int DCX_LOCKWINDOWUPDATE = 0x400;

        public const int SRCCOPY = 0xCC0020;
        public const int HWND_NOTOPMOST = -2;

        public const int SW_HIDE             = 0;
        public const int SW_SHOWNORMAL       = 1;
        public const int SW_NORMAL           = 1;
        public const int SW_SHOWMINIMIZED    = 2;
        public const int SW_SHOWMAXIMIZED    = 3;
        public const int SW_MAXIMIZE         = 3;
        public const int SW_SHOWNOACTIVATE   = 4;
        public const int SW_SHOW             = 5;
        public const int SW_MINIMIZE         = 6;
        public const int SW_SHOWMINNOACTIVE  = 7;
        public const int SW_SHOWNA           = 8;
        public const int SW_RESTORE          = 9;
        public const int SW_SHOWDEFAULT      = 10;
        public const int SW_FORCEMINIMIZE    = 11;
        public const int SW_MAX              = 11;

        public const int SWP_NOSIZE = 0x0001;
        public const int SWP_NOMOVE = 0x0002;
        public const int SWP_NOZORDER = 0x0004;
        public const int SWP_NOREDRAW = 0x0008;
        public const int SWP_NOACTIVATE = 0x0010;
        public const int SWP_FRAMECHANGED = 0x0020; // The frame changed: send WM_NCCALCSIZE
        public const int SWP_DRAWFRAME = SWP_FRAMECHANGED;
        public const int SWP_SHOWWINDOW = 0x0040;
        public const int SWP_HIDEWINDOW = 0x0080;
        public const int SWP_NOCOPYBITS = 0x0100;
        public const int SWP_NOOWNERZORDER = 0x0200; // Don't do owner Z ordering
        public const int SWP_NOREPOSITION = SWP_NOOWNERZORDER;
        public const int SWP_NOSENDCHANGING = 0x0400; // Don't send WM_WINDOWPOSCHANGING

        public const int SM_CXSCREEN             = 0;
        public const int SM_CYSCREEN             = 1;
        public const int SM_CXVSCROLL            = 2;
        public const int SM_CYHSCROLL            = 3;
        public const int SM_CYCAPTION            = 4;
        public const int SM_CXBORDER             = 5;
        public const int SM_CYBORDER             = 6;
        public const int SM_CXDLGFRAME           = 7;
        public const int SM_CYDLGFRAME           = 8;
        public const int SM_CYVTHUMB             = 9;
        public const int SM_CXHTHUMB             = 10;
        public const int SM_CXICON               = 11;
        public const int SM_CYICON               = 12;
        public const int SM_CXCURSOR             = 13;
        public const int SM_CYCURSOR             = 14;
        public const int SM_CYMENU               = 15;
        public const int SM_CXFULLSCREEN         = 16;
        public const int SM_CYFULLSCREEN         = 17;
        public const int SM_CYKANJIWINDOW        = 18;
        public const int SM_MOUSEPRESENT         = 19;
        public const int SM_CYVSCROLL            = 20;
        public const int SM_CXHSCROLL            = 21;
        public const int SM_DEBUG                = 22;
        public const int SM_SWAPBUTTON           = 23;
        public const int SM_RESERVED1            = 24;
        public const int SM_RESERVED2            = 25;
        public const int SM_RESERVED3            = 26;
        public const int SM_RESERVED4            = 27;
        public const int SM_CXMIN                = 28;
        public const int SM_CYMIN                = 29;
        public const int SM_CXSIZE               = 30;
        public const int SM_CYSIZE               = 31;
        public const int SM_CXFRAME              = 32;
        public const int SM_CYFRAME              = 33;
        public const int SM_CXMINTRACK           = 34;
        public const int SM_CYMINTRACK           = 35;
        public const int SM_CXDOUBLECLK          = 36;
        public const int SM_CYDOUBLECLK          = 37;
        public const int SM_CXICONSPACING        = 38;
        public const int SM_CYICONSPACING        = 39;
        public const int SM_MENUDROPALIGNMENT    = 40;
        public const int SM_PENWINDOWS           = 41;
        public const int SM_DBCSENABLED          = 42;
        public const int SM_CMOUSEBUTTONS        = 43;

        public const int SM_MOUSEHORIZONTALWHEELPRESENT = 91;
        public const int SM_CXPADDEDBORDER = 92;

        public const int MK_LBUTTON = 0x0001;
        public const int MK_MBUTTON = 0x0010;
        public const int MK_RBUTTON = 0x0002;
        public const int MK_SHIFT = 0x0004;
        public const int MK_CONTROL = 0x0008;

        public const int VK_LBUTTON = 0x01;
        public const int VK_RBUTTON = 0x02;
        public const int VK_CANCEL = 0x03;
        public const int VK_MBUTTON = 0x04;
        public const int VK_XBUTTO1 = 0x05;
        public const int VK_XBUTTO2 = 0x06;
        public const int VK_BACK = 0x08;
        public const int VK_TAB = 0x09;
        public const int VK_CLEAR = 0x0c;
        public const int VK_RETURN = 0x0d;
        public const int VK_SHIFT = 0x10;
        public const int VK_CONTROL = 0x11;
        public const int VK_MENU = 0x12; //alt key
        public const int VK_PAUSE = 0x13;
        public const int VK_CAPITAL = 0x14;
        public const int VK_SPACE = 0x20;
        public const int VK_END = 0x23;
        public const int VK_HOME = 0x24;
        public const int VK_LEFT = 0x25;
        public const int VK_UP = 0x26;
        public const int VK_RIGHT = 0x27;
        public const int VK_DOWN = 0x28;
        public const int VK_PRINT = 0x2a;
        public const int VK_SNAPSHOT = 0x2c;
        public const int VK_INSERT = 0x2d;
        public const int VK_DELETE = 0x2e;
        public const int VK_HELP = 0x2f;
        public const int VK_ESCAPE = 0x1B;

        public const int SB_HORZ = 0x0;
        public const int SB_VERT = 0x1;

        [DllImport("uxtheme.dll")]
        public static extern int SetWindowTheme(IntPtr hWnd, string pszSubAppName, string pszSubIdList);

        [DllImport("user32.dll")]
        public static extern int GetWindowLongA(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll")]
        public static extern int SetWindowLongA(IntPtr hWnd, int nIndex, long dwNewLong);

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hWnd, int msg, int wParam, [MarshalAs(UnmanagedType.LPWStr)] string lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SendMessage(IntPtr hWnd, int msg, Int32 wParam, int[] lParam);

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int msg, bool wParam, int lParam);

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, int lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int SendMessage(IntPtr hWnd, int msg, IntPtr wParam, ref Point lParam);

        [DllImport("user32.dll")]
        public static extern IntPtr SetFocus(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool HideCaret(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool ShowCaret(IntPtr hWnd);

        [DllImport("User32.dll")]
        public static extern int GetScrollPos(IntPtr hWnd, int nBar);

        [DllImport("user32.dll")]
        public static extern int SetScrollPos(IntPtr hWnd, int nBar, int nPos, bool bRedraw);

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true, CallingConvention = CallingConvention.Winapi)]
        public static extern short GetKeyState(int keyCode);

        [DllImport("user32.dll")]
        public static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

        [DllImport("user32.dll")]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);

        [DllImport("user32.dll")]
        public static extern IntPtr GetWindowDC(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

        [DllImport("user32.dll")]
        public static extern bool GetWindowPlacement(IntPtr hWnd, out WINDOWPLACEMENT placement);

        [DllImport("user32.dll")]
        public static extern int GetSystemMetrics(int nIndex);

        [DllImport("user32.dll")]
        public static extern int SetWindowRgn(IntPtr hWnd, IntPtr rgn, bool bRedraw);

        [DllImport("user32.dll")]
        public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll")]
        public static extern bool ClientToScreen(IntPtr hWnd, ref RECT lpRect);

        [DllImport("user32.dll")]
        public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll")]
        public static extern bool GetCursorPos(out Point pt);

        [DllImport("kernel32.dll")]
        public static extern ulong GetTickCount64();

        [DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hwnd);

        [DllImport("gdi32.dll")]
        public static extern IntPtr CreateSolidBrush(int color);

        [DllImport("gdi32.dll")]
        public static extern IntPtr CreateRectRgn(int x1, int y1, int x2, int y2);

        [DllImport("gdi32.dll")]
        public static extern int SetBkColor(IntPtr hdc, int color);

        [DllImport("gdi32.dll")]
        public static extern IntPtr MonitorFromWindow(IntPtr hwnd, int dwFlags);

        [DllImport("gdi32.dll")]
        public static extern bool GetMonitorInfoW(IntPtr hmon, out MONITORINFO moninfo);

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MONITORINFO
        {
            public int cbSize;
            public RECT rcMonitor;
            public RECT rcWork;
            public int dwFlags;
        }
        
        [StructLayout(LayoutKind.Sequential)]
        public struct WINDOWPOS {
            public IntPtr hwnd;
            public IntPtr hwndInsertAfter;
            public int x;
            public int y;
            public int cx;
            public int cy;
            public int flags;

        }

        [StructLayout(LayoutKind.Sequential)]
        public struct NCCALCSIZE_PARAMS {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            public RECT[] rects;
            public WINDOWPOS windowPos;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct WINDOWPLACEMENT {
            public uint length;
            public uint flags;
            public uint showCmd;
            public Point ptMinPosition;
            public Point ptMaxPosition;
            public RECT rcNormalPosition;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MINMAXINFO  {
            public Point ptReserved;
            public Point ptMaxSize;
            public Point ptMaxPosition;
            public Point ptMinTrackSize;
            public Point ptMaxTrackSize;
        }

    }

    
}
