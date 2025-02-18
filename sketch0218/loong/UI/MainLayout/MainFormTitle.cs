using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(false)]
    public class MainFormTitle : System.Windows.Forms.Panel
    {
        public static readonly int ORG_HEIGHT = 32;

        private UCLabel m_title = null;
        private UCPanel m_panBtns = null;
        private ImageButton m_btnMax = null;
        private ImageButton m_btnMin = null;
        private ImageButton m_btnClose = null;
        public readonly int TITLE_HEIGHT = 32;

        public MainFormTitle() {
            this.DoubleBuffered = true;
            base.BorderStyle = BorderStyle.None;
            this.SetStyle(ControlStyles.ResizeRedraw, true);

            this._initCtrls();
        }

        protected void _initCtrls() {
            this.SuspendLayout();

            m_title = new UCLabel();
            m_title.ignoreNCHit = true;
            m_title.Font = new Font("微软雅黑", 12F, FontStyle.Regular);
            m_title.AutoSize = false;
            m_title.Dock = DockStyle.Fill;
            m_title.Padding = new Padding(10, 0, 0, 0);
            m_title.BackColor = Color.Transparent;
            m_title.TextAlign = ContentAlignment.MiddleLeft;
            
            int btnW = DPI.Scale(42);
            int btnH = DPI.Scale(28);

            m_panBtns = new UCPanel();
            m_panBtns.SetBounds(Math.Max(0, this.Width - (btnW + 1) * 3), 0, (btnW + 1) * 3, this.Height);
            m_panBtns.Dock = DockStyle.Right;

            m_btnMin = new ImageButton();
            m_btnMin.SetBounds(0, 0, btnW, btnH);
            m_btnMin.image = Res.dpiImage("btn_min");
            m_btnMin.backColor = Color.Transparent;
            m_btnMin.backColorHL = Color.LightGray;
            m_btnMin.Click += btnMin_Click;

            m_btnMax = new ImageButton();
            m_btnMax.SetBounds(btnW + 1, 0, btnW, btnH);
            m_btnMax.image = Res.dpiImage("btn_max");
            m_btnMax.backColor = Color.Transparent;
            m_btnMax.backColorHL = Color.LightGray;
            m_btnMax.Click += btnMax_Click;

            m_btnClose = new ImageButton();
            m_btnClose.SetBounds((btnW + 1) * 2, 0, btnW, btnH);
            m_btnClose.image = Res.dpiImage("btn_close_black");
            m_btnClose.imageHL = Res.dpiImage("btn_close_white");
            m_btnClose.backColor = Color.Transparent;
            m_btnClose.backColorHL = Color.FromArgb(232, 17, 35);
            m_btnClose.Click += btnClose_Click;
            
            m_panBtns.Controls.AddRange(new Control[] { m_btnMin, m_btnMax, m_btnClose});

            this.Controls.AddRange(new Control[] { m_title, m_panBtns });

            this.ResumeLayout();
        }

        protected override void OnSizeChanged(EventArgs e) {
            base.OnSizeChanged(e);
        
            if (MainForm.instance != null) {
                if (MainForm.instance.WindowState == FormWindowState.Maximized) {
                    m_btnMax.image = Res.dpiImage("btn_restore");
                }
                else if (MainForm.instance.WindowState == FormWindowState.Normal) {
                    m_btnMax.image = Res.dpiImage("btn_max");
                }
            }
        }

        public string title {
            get => m_title.Text;
            set => m_title.Text = value;
        }

        private void btnMax_Click(object sender, EventArgs e) {
            if (MainForm.instance.WindowState == FormWindowState.Normal)
                MainForm.instance.WindowState = FormWindowState.Maximized;
            else if (MainForm.instance.WindowState == FormWindowState.Maximized)
                MainForm.instance.WindowState = FormWindowState.Normal;
        }

        private void btnMin_Click(object sender, EventArgs e) {
            MainForm.instance.WindowState = FormWindowState.Minimized;
        }

        private void btnClose_Click(object sender, EventArgs e) {
            FuncUnit.invokeClick("file_exit", this, e, Keys.None, null);
        }

        protected override void WndProc(ref Message message) {
            base.WndProc(ref message);

            if (this.DesignMode)
                return;

            if (message.Msg == Win32.WM_NCHITTEST && (int)message.Result == Win32.HTCLIENT) {
                message.Result = (IntPtr)Win32.HTTRANSPARENT;
            }
                
        }
    }
}
