using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    public class TabPanelPage : System.Windows.Forms.Panel
    {
        public TabPanelPage() {
            this.DoubleBuffered = true;
            this.TabStop = false;
            this.Dock = DockStyle.Fill;
        }

        protected override void WndProc(ref Message m) {
            const int HTTRANSPARENT = (-1);

            if (this.DesignMode && m.Msg == Win32.WM_NCHITTEST) {//prevent select in designer
                m.Result = (IntPtr)HTTRANSPARENT;
                return;
            }

            base.WndProc(ref m);
        }
    }

}
