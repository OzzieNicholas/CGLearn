using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;
using LoongCAD.Util;

namespace UserCtrl
{
    public class MenuBase : MenuStrip
    {
        private bool m_clickThrough = true;

        public MenuBase() {

        }

        public bool clickThrough {
            get {
                return m_clickThrough;
            }

            set {
                m_clickThrough = value;
            }
        }

        public ToolStripMenuItem group(string text, ToolStripItem[] children = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripMenuItem item(string text, EventHandler onClick) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Enabled = (onClick != null);

            if (onClick != null)
                item.Click += onClick;

            return item;
        }

        public ToolStripMenuItem checkedItem(string text, EventHandler onChecked) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.CheckOnClick = true;
            item.CheckStateChanged += onChecked;
            return item;
        }

        public ToolStripSeparator sep() {
            return new ToolStripSeparator();
        }

        protected override void WndProc(ref Message m) {
            base.WndProc(ref m);

            if (this.clickThrough &&
                m.Msg == Win32.WM_MOUSEACTIVATE &&
                m.Result == (IntPtr)Win32.MA_ACTIVATEANDEAT) {
                m.Result = (IntPtr)Win32.MA_ACTIVATE;
            }
        }
    }

}
