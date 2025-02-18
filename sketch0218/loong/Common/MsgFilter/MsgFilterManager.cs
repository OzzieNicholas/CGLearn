using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using LoongCAD.Util;

namespace LoongCAD.Common
{
    public class MsgFilterManager
    {
        private static List<IOHandler> m_ioHandlers = new List<IOHandler>();
        private static int m_mouseDownTm = 0;
        private static Point m_mouseDownPos = new Point(0, 0);
        private static bool m_enable = false;

        public MsgFilterManager() {
        
        }

        public static void start() {
            m_enable = true;        
        }

        public static void stop() {
            m_enable = false;
        }

        public static void add(IOHandler io) {
            if (m_ioHandlers.IndexOf(io) >= 0)
                return;

            m_ioHandlers.Add(io);
        }

        public static void remove(IOHandler io) {
            m_ioHandlers.Remove(io);
        }

        public static bool PreFilterMessage(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam) {
            if (!m_enable)
                return false;

            Control sender = Control.FromChildHandle(hWnd);
            if (sender != null && sender.IsDisposed)
                return false;

            bool bRes = false;
            switch (msg) {
                case Win32.WM_SYSKEYDOWN: {
                    Keys keyCode = (Keys)(wParam.ToInt32() & (int)Keys.KeyCode);
                    if (Win32.GetKeyState(Win32.VK_CONTROL) < 0)
                        keyCode |= Keys.Control;
                    if (Win32.GetKeyState(Win32.VK_MENU) < 0)
                        keyCode |= Keys.Alt;
                    if (Win32.GetKeyState(Win32.VK_SHIFT) < 0)
                        keyCode |= Keys.Shift;
                
                    int repeat = (int)(lParam.ToInt32() & 0xffff);
                
                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (io.keyDown == null
                            || !io.listen(sender))
                            continue;
                
                        bRes |= io.keyDown(sender, new KeyEventArgs(keyCode), repeat);
                    }
                }
                break;
                case Win32.WM_KEYDOWN: {
                    Keys keyCode = (Keys)(wParam.ToInt32() & (int)Keys.KeyCode);
                    if (Win32.GetKeyState(Win32.VK_CONTROL) < 0)
                        keyCode |= Keys.Control;
                    if (Win32.GetKeyState(Win32.VK_MENU) < 0)
                        keyCode |= Keys.Alt;
                    if (Win32.GetKeyState(Win32.VK_SHIFT) < 0)
                        keyCode |= Keys.Shift;

                    int repeat = (int)(lParam.ToInt32() & 0xffff);

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (io.keyDown == null
                            || !io.listen(sender))
                            continue;

                        bRes |= io.keyDown(sender, new KeyEventArgs(keyCode), repeat);
                    }
                }
                break;
                case Win32.WM_SYSKEYUP: {
                    Keys keyCode = (Keys)(wParam.ToInt32() & (int)Keys.KeyCode);
                    if (Win32.GetKeyState(Win32.VK_CONTROL) < 0)
                        keyCode |= Keys.Control;
                    if (Win32.GetKeyState(Win32.VK_MENU) < 0)
                        keyCode |= Keys.Alt;
                    if (Win32.GetKeyState(Win32.VK_SHIFT) < 0)
                        keyCode |= Keys.Shift;

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (io.keyUp == null
                            || !io.listen(sender))
                            continue;

                        bRes |= io.keyUp(sender, new KeyEventArgs(keyCode));
                    }
                }
                break;
                case Win32.WM_KEYUP: {
                    Keys keyCode = (Keys)(wParam.ToInt32() & (int)Keys.KeyCode);
                    if (Win32.GetKeyState(Win32.VK_CONTROL) < 0)
                        keyCode |= Keys.Control;
                    if (Win32.GetKeyState(Win32.VK_MENU) < 0)
                        keyCode |= Keys.Alt;
                    if (Win32.GetKeyState(Win32.VK_SHIFT) < 0)
                        keyCode |= Keys.Shift;

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (io.keyUp == null
                            || !io.listen(sender))
                            continue;

                        bRes |= io.keyUp(sender, new KeyEventArgs(keyCode));
                    }
                }
                break;
                case Win32.WM_LBUTTONDOWN:
                case Win32.WM_MBUTTONDOWN:
                case Win32.WM_RBUTTONDOWN: {
                    m_mouseDownPos = Control.MousePosition;
                    m_mouseDownTm = Environment.TickCount;

                    MouseButtons button = (msg == Win32.WM_LBUTTONDOWN) ?
                        MouseButtons.Left : (msg == Win32.WM_MBUTTONDOWN) ?
                        MouseButtons.Middle : ((msg == Win32.WM_RBUTTONDOWN) ? MouseButtons.Right : MouseButtons.None);

                    Win32.GetCursorPos(out Point ptScr);

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (io.mouseDown == null
                            || !io.listen(sender))
                            continue;

                        Point pt = (io.owner != null) ? io.owner.PointToClient(ptScr) : ptScr;
                        bRes |= io.mouseDown(sender, new MouseEventArgs(button, 1, pt.X, pt.Y, 0));
                    }
                }
                break;
                case Win32.WM_MOUSEMOVE: {
                    int wpar = wParam.ToInt32();
                    MouseButtons button = ((wpar & Win32.MK_LBUTTON) != 0) ?
                            MouseButtons.Left : ((wpar & Win32.MK_MBUTTON) != 0) ?
                            MouseButtons.Middle : (((wpar & Win32.MK_RBUTTON) != 0) ? MouseButtons.Right : MouseButtons.None);

                    Win32.GetCursorPos(out Point ptScr);

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (!io.listen(sender))
                            continue;

                        if (io.mouseMove != null) {
                            Point pt = (io.owner != null) ? io.owner.PointToClient(ptScr) : ptScr;
                            bRes |= io.mouseMove(sender, new MouseEventArgs(button, 1, pt.X, pt.Y, 0));
                        }
                    }
                }
                break;
                case Win32.WM_LBUTTONUP:
                case Win32.WM_MBUTTONUP:
                case Win32.WM_RBUTTONUP: {
                    MouseButtons button = (msg == Win32.WM_LBUTTONUP) ?
                             MouseButtons.Left : (msg == Win32.WM_MBUTTONUP) ?
                             MouseButtons.Middle : ((msg == Win32.WM_RBUTTONUP) ? MouseButtons.Right : MouseButtons.None);

                    Win32.GetCursorPos(out Point ptScr);
                    bool bClicked = (Environment.TickCount - m_mouseDownTm < 500)
                            && ((m_mouseDownPos.X - ptScr.X) * (m_mouseDownPos.X - ptScr.X) + (m_mouseDownPos.Y - ptScr.Y) * (m_mouseDownPos.Y - ptScr.Y) < 32);

                    for (int i = 0; i < m_ioHandlers.Count; ++i) {
                        var io = m_ioHandlers[i];
                        if (!io.listen(sender))
                            continue;

                        Point pt = (io.owner != null) ? io.owner.PointToClient(ptScr) : ptScr;

                        if (io.mouseClick != null && bClicked) 
                            bRes |= io.mouseClick(sender, new MouseEventArgs(button, 1, pt.X, pt.Y, 0));

                        if (io.mouseUp != null) 
                            bRes |= io.mouseUp(sender, new MouseEventArgs(button, 1, pt.X, pt.Y, 0));
                    }
                }
                break;
            }
            
            return bRes;
        }
    }
}
