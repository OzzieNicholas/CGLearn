using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Text;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class UCLabel : Label
    {
        private bool m_ignoreNCHit = true;

        public UCLabel() {
            this.SetStyle(ControlStyles.ResizeRedraw, true);
        }

        [DefaultValue(false)]
        public bool ignoreNCHit {
            get => m_ignoreNCHit;
            set => m_ignoreNCHit = value;
        }

        protected override void WndProc(ref Message message) {
            base.WndProc(ref message);

            if (this.DesignMode || m_ignoreNCHit == false)
                return;

            if (message.Msg == Win32.WM_NCHITTEST && (int)message.Result == Win32.HTCLIENT) {
                message.Result = (IntPtr)Win32.HTTRANSPARENT;
            }
        }
    }

   
}
