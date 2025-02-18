using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Windows.Forms.Design;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(false)]
    [Designer(typeof(ParentControlDesigner))]
    public class FoldPanelListGrid : System.Windows.Forms.TableLayoutPanel
    {
        public FoldPanelListGrid() {
            this.DoubleBuffered = true;
            this.ColumnCount = 1;
            this.RowCount = 1;
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
