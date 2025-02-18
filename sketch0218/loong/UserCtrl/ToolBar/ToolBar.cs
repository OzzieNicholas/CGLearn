using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using LoongCAD.Util;

namespace UserCtrl
{
    public class ToolBar : ToolStrip
    {
        private bool m_clickThrough = true;
        private Pen m_gridPen = null;

        public ToolBar() {
            this.DoubleBuffered = true;
            this.GripMargin = new Padding(2);

            m_gridPen = new Pen(Color.DarkGray);
            m_gridPen.DashPattern = new float[] { 2.0f, 1.0f };
            m_gridPen.Width = 1.0f;
        }

        [DefaultValue(true)]
        public bool clickThrough {
            get => m_clickThrough;
            set => m_clickThrough = value;
        }

        protected override void WndProc(ref Message m) {
            base.WndProc(ref m);

            if (this.clickThrough &&
                m.Msg == Win32.WM_MOUSEACTIVATE &&
                m.Result == (IntPtr)Win32.MA_ACTIVATEANDEAT) {
                m.Result = (IntPtr)Win32.MA_ACTIVATE;
            }
        }

        protected override void OnPaintGrip(PaintEventArgs ev) {
            var rect = ev.ClipRectangle;

            if (this.LayoutStyle == ToolStripLayoutStyle.HorizontalStackWithOverflow
                || this.LayoutStyle == ToolStripLayoutStyle.Flow) {
                var x = (rect.X + rect.Right) / 2;
                var top = rect.Top + 5;
                var bottom = rect.Bottom - 5;
                ev.Graphics.DrawLine(m_gridPen, x, top, x, bottom);
                ev.Graphics.DrawLine(m_gridPen, x + 2, top, x + 2, bottom);
            }
            else {
                var y = (rect.Top + rect.Bottom) / 2;
                var left = rect.Left + 5;
                var right = rect.Right - 5;
                ev.Graphics.DrawLine(m_gridPen, left, y, right, y);
                ev.Graphics.DrawLine(m_gridPen, left, y + 2, right, y + 2);
            }
        }

        protected override void OnPaint(PaintEventArgs e) {
            base.OnPaint(e);

            if (this.Items.Count == 0)
                return;

            var rect = this.ClientRectangle;
            if (this.LayoutStyle == ToolStripLayoutStyle.HorizontalStackWithOverflow
                || this.LayoutStyle == ToolStripLayoutStyle.Flow) {
                int clearSize = this.Width - this.Items[this.Items.Count - 1].Bounds.Right;
                using (var br = new SolidBrush(this.BackColor)) {
                    e.Graphics.FillRectangle(br, rect.Right - clearSize, rect.Top, clearSize, rect.Height);
                }
            }
            else {
                int clearSize = this.Height - this.Items[this.Items.Count - 1].Bounds.Bottom;
                using (var br = new SolidBrush(this.BackColor)) {
                    e.Graphics.FillRectangle(br, rect.Left, rect.Bottom - clearSize, rect.Width, clearSize);
                }
            }
        }
    }

}
