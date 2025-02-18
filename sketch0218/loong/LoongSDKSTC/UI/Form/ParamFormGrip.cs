using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    public class ParamFormGrip : StatusStrip
    {
        private static readonly ParaFormGripRenderer m_renderer = new ParaFormGripRenderer();

        public ParamFormGrip() {
            this.Renderer = m_renderer;
            this.BackColor = Color.Transparent;
        }

        protected override void WndProc(ref Message m) {
            const int HTTRANSPARENT = (-1);

            if (this.DesignMode && m.Msg == 0x0084/*WM_NCHITTEST*/) {//prevent select in designer
                m.Result = (IntPtr)HTTRANSPARENT;
                return;
            }

            base.WndProc(ref m);
        }
    }

    class ParaFormGripRenderer : ToolStripSystemRenderer
    {
        public ParaFormGripRenderer() : base() { 
        
        }

        protected override void OnRenderToolStripBorder(ToolStripRenderEventArgs e) {
            //base.OnRenderToolStripBorder(e);
        }
    }
}
