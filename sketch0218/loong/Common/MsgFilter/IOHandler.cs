using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace LoongCAD.Common
{
    public class IOHandler
    {
        private Control m_owner = null;
        
        public Func<Control, KeyEventArgs, int, bool> keyDown = null;
        public Func<Control, KeyEventArgs, bool> keyUp = null;
        public Func<Control, MouseEventArgs, bool> mouseDown = null;
        public Func<Control, MouseEventArgs, bool> mouseMove = null;
        public Func<Control, MouseEventArgs, bool> mouseUp = null;
        public Func<Control, MouseEventArgs, bool> mouseClick = null;

        public IOHandler(Control owner) {
             m_owner = owner;
        }

        public Control owner {
            get => m_owner;
        }

        public void start() {
            MsgFilterManager.add(this);
        }

        public void stop() {
            MsgFilterManager.remove(this);
        }

        public bool listen(Control sender) {
            if (m_owner == null)
                return true;

            if (m_owner.IsDisposed)
                return false;

            if (m_owner == sender)
                return true;

            var cur = sender;
            while (cur != null) {
                if (cur == m_owner)
                    return true;

                cur = cur.Parent;
            }
            
            return false;
        }

    }
}
