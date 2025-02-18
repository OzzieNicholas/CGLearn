using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.ComponentModel;

namespace UI
{
    [ToolboxItem(false)]
    public class ToolBarPanelBottom : ToolBarPanel
    {
        public ToolBarPanelBottom() {
            m_barLayout = ToolStripLayoutStyle.HorizontalStackWithOverflow;
            m_panelPosition = CustomBarsProvision.BAR_LOCATION_BOTTOM;
        }

        public override bool indexOf(Rectangle insertRect, out int insertLine, out int insertIdx) {
            insertLine = -1;
            insertIdx = -1;

            if (insertRect.Width <= 0 || this.IsDisposed || this.Parent == null)
                return false;

            int y = 1;
            for (var l = m_bars.Count - 1; l >= 0; --l) {
                var line = m_bars[l];
                int x = 0;
                int h = 0;
                for (int b = 0; b < line.Count; b++) {
                    var bar = line[b];

                    if (bar.IsDisposed) //app is closing
                        continue;

                    var barRect = this.RectangleToScreen(new Rectangle(x, y, bar.Width, bar.Height));
                    if (barRect.IntersectsWith(insertRect) &&
                        (insertRect.Bottom - barRect.Top >= barRect.Height / 2)) {
                        if (insertRect.Left - barRect.Left < barRect.Width / 2) //bar的前面
                            insertIdx = b;
                        else //bar的后面
                            insertIdx = b + 1;

                        insertLine = l;
                        return true;
                    }

                    x += bar.Width + 1;
                    h = Math.Max(h, bar.Height);
                }

                if (line.Count > 0) {
                    var lastBar = line.Last();
                    var barRect = this.RectangleToScreen(lastBar.Bounds);
                    if (insertRect.Bottom - barRect.Top >= barRect.Height / 2) {
                        insertLine = l;
                        insertIdx = line.Count;
                        return true;
                    }
                }

                y += h + 1;
            }
           
            var panRect = this.RectangleToScreen(this.ClientRectangle);
            if (m_bars.Count == 0)
                panRect.Inflate(1, 1);

            if (panRect.IntersectsWith(insertRect)) {
                insertLine = m_bars.Count;
                insertIdx = 0;
                return true;
            }

            return false;

        }

        public override void relayout(Rectangle reserve) {
            if (this.IsDisposed)
                return;

            this.SuspendLayout();

            int y = 1;
            for (var l = m_bars.Count - 1; l >= 0; --l) {
                var line = m_bars[l];
                int x = 0;
                int h = 0;
                for (int b = 0; b < line.Count; b++) {
                    var bar = line[b];

                    if (bar.provsion != null && !bar.provsion.Provisioned)
                        continue;

                    if (bar.IsDisposed) //app is closing
                        return;

                    if (l == reserve.X && b == reserve.Y && reserve.Width > 0) { //reserve space
                        x += reserve.Width + 1;
                    }

                    bar.Location = new Point(x, y);
                    x += bar.Width + 1;
                    h = Math.Max(h, bar.Height);
                }

                y += h + 1;
            }

            this.SetBounds(this.Location.X, this.Parent.Height - y, this.Width, y);
            this.ResumeLayout();
        }
    }
}
