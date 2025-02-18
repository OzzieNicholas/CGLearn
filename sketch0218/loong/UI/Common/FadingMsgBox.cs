using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using System.Threading;

namespace UI
{
    public partial class FadingMsgBox : Form
    {
        private System.Windows.Forms.Timer m_tm = null;
        private int m_fadingTm = 0;
        private int m_tickTm = 0;
        private double m_opacity = 1.0;

        public FadingMsgBox() {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls() {
            this.TopMost = true;

            m_tm = new System.Windows.Forms.Timer();
            m_tm.Interval = 33;
            m_tm.Tick += timer_Tick;
        }

        private void timer_Tick(object sender, EventArgs e) {
            m_tickTm -= m_tm.Interval;
            if (m_tickTm <= 0) {
                m_tm.Stop();
                this.Close();
                this.Dispose();
                return;
            }

            this.Opacity = m_opacity * m_tickTm / m_fadingTm;
        }

        private void _setup(string caption, string msg) {
            if (caption.Length == 0)
                this.FormBorderStyle = FormBorderStyle.None;
            else {
                this.FormBorderStyle = FormBorderStyle.FixedSingle;
                this.Text = caption;
            }

            Size msgSize = TextRenderer.MeasureText(msg, this.lb_message.Font);
            Rectangle scrClient = this.RectangleToScreen(this.ClientRectangle);
            int border = (this.Width - scrClient.Width) / 2;
            int titleHeight = scrClient.Top - this.Top;

            this.Width = Math.Max(200, msgSize.Width + 20) + border * 2;
            this.Height = Math.Max(100, msgSize.Height + 20) + titleHeight + border;
            scrClient = this.RectangleToScreen(this.ClientRectangle);

            this.lb_message.Text = msg;
            this.lb_message.Location = new Point((scrClient.Width - msgSize.Width) / 2, (scrClient.Height - msgSize.Height) / 2);
        }

        public static void show(string caption, string msg, int fading, double opacity, Form owner) {
            FadingMsgBox dlg = new FadingMsgBox();

            dlg._setup(caption, msg);
            dlg.m_fadingTm = fading;
            dlg.m_tickTm = fading;
            dlg.m_opacity = opacity;
            dlg.Opacity = opacity;

            Point pos = new Point(owner.Width / 2 - dlg.Width / 2, owner.Height / 2 - dlg.Height / 2);
            dlg.Location = owner.PointToScreen(pos);

            owner.BeginInvoke(new Action(() => {
                dlg.Show(owner);
                dlg.m_tm.Start();
            }));
        }
    }
}
