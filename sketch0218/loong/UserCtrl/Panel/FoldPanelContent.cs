using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;
using System.ComponentModel.Design;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(false)]
    [Designer(typeof(ParentControlDesigner))]
    public class FoldPanelContent : System.Windows.Forms.Panel
    {
        public FoldPanelContent() {
            this.DoubleBuffered = true;
        }

        protected override void WndProc(ref Message m) {
            if (this.DesignMode && m.Msg == Win32.WM_NCHITTEST) {//prevent select in designer
                m.Result = (IntPtr)Win32.HTTRANSPARENT;
                return;
            }

            base.WndProc(ref m);
        }
    }
}
