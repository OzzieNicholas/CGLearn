using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public partial class MainFormTopPanel : UserControl
    {
        public static readonly int ORG_HEIGHT = MainFormTitle.ORG_HEIGHT + MainFormMenuPanel.ORG_HEIGHT;

        public MainFormTopPanel() {
            InitializeComponent();

            this.DoubleBuffered = true;
            this.SetStyle(ControlStyles.ResizeRedraw, true);

            this._initCtrls();
        }

        protected void _initCtrls() {
            btn_logo.Size = DPI.ScaleSize(ORG_HEIGHT, ORG_HEIGHT);
            btn_logo.image = Res.dpiImage("logo_darkblue");
            btn_logo.imageHL = Res.dpiImage("logo_blue");
            btn_logo.MouseDown += logo_MouseDown;

            pan_title.SetBounds(btn_logo.Width, 0, Math.Max(0, this.Width - btn_logo.Width), DPI.Scale(MainFormTitle.ORG_HEIGHT));
            pan_title.title = "D5 Modeling";
            pan_title.Dock = DockStyle.Top;

            pan_menu.SetBounds(btn_logo.Width, pan_title.Height, Math.Max(0, this.Width - btn_logo.Width), DPI.Scale(ORG_HEIGHT) - pan_title.Height);
            pan_menu.Dock = DockStyle.Bottom;
        }

        public new Size Size {
            get => base.Size;
            set => base.Width = value.Width;
        }

        public new int Height {
            get => base.Height;
            set { }
        }

        public UserCtrl.ImageButton logoButton => btn_logo;

        public string title {
            get => pan_title.title;
            set => pan_title.title = value;
        }

        public int titleHeight => pan_title.Height;

        public MenuStrip menu {
            get => pan_menu.menu;
            set => pan_menu.menu = value;
        }

        private void logo_MouseDown(object sender, MouseEventArgs e) {
            SubMenu sb = SubMenu_Logo.instance;

            if (sb.Visible)
                sb.Close();
            else
                sb.show(MainForm.instance, MainForm.instance.PointToClient(this.PointToScreen(new Point(btn_logo.Location.X, btn_logo.Location.Y + btn_logo.Height))));
        }

        protected override void WndProc(ref Message m) {
            base.WndProc(ref m);

            if (this.DesignMode) 
                return;
                
            if (m.Msg == Win32.WM_NCHITTEST) {//prevent select in designer
                m.Result = (IntPtr)Win32.HTTRANSPARENT;
            }
        }


    }
}
