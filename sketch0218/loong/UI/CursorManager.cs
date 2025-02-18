using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;


namespace UI
{
    public class CursorManager
    {
        private static CursorManager m_instance = new CursorManager();
        private Dictionary<Define.MouseCursor, Cursor> m_cursorMap = new Dictionary<Define.MouseCursor, Cursor>() {
            { Define.MouseCursor.CURSOR_NONE, null},
            { Define.MouseCursor.CURSOR_ARROW, Cursors.Arrow},
            { Define.MouseCursor.CURSOR_CROSS, Cursors.Cross},
            { Define.MouseCursor.CURSOR_HAND, Cursors.Hand},
            { Define.MouseCursor.CURSOR_MOVE, Cursors.SizeAll},
            { Define.MouseCursor.CURSOR_SIZE_ALL, Cursors.SizeAll},
            { Define.MouseCursor.CURSOR_SIZE_NS, Cursors.SizeNS},
            { Define.MouseCursor.CURSOR_SIZE_WE, Cursors.SizeWE},
            { Define.MouseCursor.CURSOR_SIZE_NWSE, Cursors.SizeNWSE},
            { Define.MouseCursor.CURSOR_SIZE_NESW, Cursors.SizeNESW}
        };

        private Cursor[] m_cursors = new Cursor[2] { null, null };

        public static CursorManager instance {
            get {
                return m_instance;
            }
        }

        public CursorManager() {

        }

        public void setCursor(int idx, Define.MouseCursor cursor) {
            Cursor orgCursor = m_cursorMap[cursor];
            m_cursors[idx] = orgCursor;

            for (int i = m_cursors.Length - 1; i >= 0; --i) {
                if (m_cursors[i] != null) {
                    MainForm.instance.viewPanel.Cursor = m_cursors[i];
                    break;
                }
            }
        }
    }
}
