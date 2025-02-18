using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    public partial class MainFormContentPanel : UserControl
    {
        public MainFormContentPanel() {
            InitializeComponent();

            this._initCtrls();
        }

        protected void _initCtrls() {
        
        }

        public MainFormLayoutContainer layout => layout_container;
        public CellTab headerTab => layout_container.centerPanel.headerTab;
        public MainView viewPanel => layout_container.centerPanel.viewPanel;

        protected override void WndProc(ref Message msg) {
            if (this.DesignMode) {
                base.WndProc(ref msg);
                return;
            }

            switch (msg.Msg) {
                case Win32.WM_NCHITTEST: {
                    msg.Result = (IntPtr)Win32.HTTRANSPARENT;
                    return;
                }
            }

            base.WndProc(ref msg);
        }
    }
}
