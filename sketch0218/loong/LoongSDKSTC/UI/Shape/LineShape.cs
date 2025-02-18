using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public partial class LineShape : UserControl
    {
        public LineShape()
        {
            InitializeComponent();
        }

        protected override void OnPaint(PaintEventArgs e) {
            //base.OnPaint(e);

            var rect = e.ClipRectangle;
            if (rect.Width >= rect.Height) { //horizontal
                Point pt0 = new Point(rect.X, (rect.Y + rect.Bottom) / 2);
                Point pt1 = new Point(rect.Right, (rect.Y + rect.Bottom) / 2);
                using (var pen = new Pen(this.ForeColor)) {
                    e.Graphics.DrawLine(pen, pt0, pt1);
                }
            }
            else { //vertical
                Point pt0 = new Point((rect.X + rect.Right) / 2, rect.Y);
                Point pt1 = new Point((rect.X + rect.Right) / 2, rect.Bottom);
                using (var pen = new Pen(this.ForeColor)) {
                    e.Graphics.DrawLine(pen, pt0, pt1);
                }
            }
        }
    }
}
