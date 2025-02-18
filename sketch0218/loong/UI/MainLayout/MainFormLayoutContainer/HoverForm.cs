using System;
using System.Collections.Generic;
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

namespace UI
{
    public partial class HoverForm : Form
    {
        private static HoverForm m_inst = new HoverForm();
        private CustomToolBar m_toolBar = null;
        private Point m_clientPos = Point.Empty;
        private Point m_mouseOffset = Point.Empty;
        private ToolBarPanel m_hoveredPan = null;

        public static HoverForm instance {
            get => m_inst;
        }

        private HoverForm() {
            InitializeComponent();

            this.DoubleBuffered = true;
            this.Text = "";
            this.ControlBox = false;
            this.FormBorderStyle = FormBorderStyle.None;
            this.ResizeRedraw = true;
            this.Opacity = 0.6;
        }

        public CustomToolBar toolBar {
            get => m_toolBar;
        }

        public ToolBarPanel hoveredPan {
            get => m_hoveredPan;
        }

        public Point mouseOffset {
            get => m_mouseOffset;
        }

        public Point clientPos {
            get => m_clientPos;
        }

        public Rectangle probeRect { 
            get {
                int s = Math.Min(m_toolBar.Width, m_toolBar.Height);
                var rect = new Rectangle(m_clientPos.X, m_clientPos.Y, s, s);
                rect.Inflate(DPI.Scale(3), DPI.Scale(3));
                return rect;
            }
        }

        public void beginDrag(CustomToolBar bar, ToolBarPanel pan, Point mouseScrPos) {
            var barPos = bar.screenPos;
            bar.hideFloaty();

            m_toolBar = bar;
            m_hoveredPan = pan;
            m_clientPos = barPos;
            m_mouseOffset = new Point(m_clientPos.X - mouseScrPos.X, m_clientPos.Y - mouseScrPos.Y);

            this._updatehBounds(false);
            this.Visible = true;
        }

        public void drag(ToolBarPanel pan, int line, int idx, Point mouseScrPos) {
            if (m_toolBar == null || this.Visible == false)
                return;

            m_clientPos = new Point(mouseScrPos.X + m_mouseOffset.X, mouseScrPos.Y + m_mouseOffset.Y);

            if (m_hoveredPan == pan) {
                this._updatehBounds((pan != null) && (line >= pan.lineCount));

                if (m_hoveredPan != null) 
                    m_hoveredPan.relayout(new Rectangle(line, idx, this.Width, this.Height));
            }
            else {
                if (m_hoveredPan != null) //old
                    m_hoveredPan.relayout(Rectangle.Empty);

                m_hoveredPan = pan;

                this._updatehBounds((pan != null) && (line >= pan.lineCount)); //NOTE: 需要先于relayout调用

                if (m_hoveredPan != null) //new
                    m_hoveredPan.relayout(new Rectangle(line, idx, this.Width, this.Height));
            }
        }

        public void endDrag() {
            if (m_toolBar == null || this.Visible == false)
                return;

            if (m_hoveredPan != null)
            {
                int line, idx;
                var probeRect = this.probeRect;
                if (m_hoveredPan.indexOf(probeRect, out line, out idx))
                {
                    //System.Console.WriteLine("\n\n\n\n^^^^^^HoverForm - startDrag after in: " + m_toolBar.provsion.toString());
                    m_toolBar.hideFloaty();
                    m_hoveredPan.addToolBar(m_toolBar, line, idx);
                    //System.Console.WriteLine("HoverForm - added: " + m_toolBar.provsion.toString());
                    m_hoveredPan.relayout();
                    this.Hide();
                    m_toolBar.provsion.changeLocation(m_hoveredPan.position, line);
                    //System.Console.WriteLine("HoverForm - endDrag after in: " + m_toolBar.provsion.toString());
                    //CustomBarsProvision.instance.printLayoutLocations(LayoutManager.instance.curLayout);
                    //System.Console.WriteLine("\n\n\n\n\n ");
                    return;
                }
            }

            m_toolBar.showFloaty();
            //System.Console.WriteLine("HoverForm - endDrag after: " + m_toolBar.provsion.toString());
            this.Hide();
        }

        private void _updatehBounds(bool bAddLine) {
            if (m_toolBar == null)
                return;

            m_toolBar.LayoutStyle = (m_hoveredPan == null) ? ToolStripLayoutStyle.HorizontalStackWithOverflow : m_hoveredPan.barLayout;

            Rectangle bounds = new Rectangle(m_clientPos, m_toolBar.PreferredSize);
            if (m_hoveredPan == null) {
                bounds.X -= 3;
                bounds.Width += 3 * 2;
                bounds.Y -= 32;
                bounds.Height += 32;
            }
            else if (bAddLine && m_hoveredPan.Parent != null) {
                var panRect = m_hoveredPan.Parent.RectangleToScreen(m_hoveredPan.Bounds);
                if (m_toolBar.LayoutStyle == ToolStripLayoutStyle.HorizontalStackWithOverflow) {
                    bounds.X = panRect.Left;
                    bounds.Width = panRect.Width;
                }
                else {
                    bounds.Y = panRect.Top;
                    bounds.Height = panRect.Height;
                }
            }

            this.Bounds = bounds;
        }

        protected override void OnPaint(PaintEventArgs e) {
            e.Graphics.Clear(Color.DarkGreen);
            e.Graphics.DrawRectangle(Pens.Gray, e.ClipRectangle);
        }
    }
}
