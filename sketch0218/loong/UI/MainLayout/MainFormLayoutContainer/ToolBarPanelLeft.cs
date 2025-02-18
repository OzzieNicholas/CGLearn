using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace UI
{
    [ToolboxItem(false)]
    public class ToolBarPanelLeft : ToolBarPanel
    {
        public ToolBarPanelLeft() {
            m_barLayout = ToolStripLayoutStyle.VerticalStackWithOverflow;
            m_panelPosition = CustomBarsProvision.BAR_LOCATION_LEFT;
        }

        public override bool indexOf(Rectangle insertRect, out int insertLine, out int insertIdx) {
            insertLine = -1;
            insertIdx = -1;

            if (insertRect.Width <= 0 || this.IsDisposed || this.Parent == null)
                return false;

            int x = 1;
            for (int l = 0; l < m_bars.Count; l++) {
                var line = m_bars[l];
                int y = 0;
                int w = 0;

                for (int b = 0; b < line.Count; b++) {
                    var bar = line[b];

                    if (bar.IsDisposed) //app is closing
                        continue;

                    var barRect = this.RectangleToScreen(new Rectangle(x, y, bar.Width, bar.Height));
                    if (barRect.IntersectsWith(insertRect)
                        && barRect.Right - insertRect.Left >= barRect.Width / 2) {
                        if ((float)(insertRect.Top - barRect.Top) / (float)barRect.Height < 0.5) //bar的前面
                            insertIdx = b;
                        else //bar的后面
                            insertIdx = b + 1;

                        insertLine = l;
                        return true;
                    }

                    y += bar.Height + 1;
                    w = Math.Max(w, bar.Width);
                }

                if (line.Count > 0) {
                    var lastBar = line.Last();
                    var barRect = this.RectangleToScreen(lastBar.Bounds);
                    if (barRect.Right - insertRect.Left >= barRect.Width / 2) {
                        insertLine = l;
                        insertIdx = line.Count;
                        return true;
                    }
                }

                x += w + 1;
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

            int x = 1;
            for (int l = 0; l < m_bars.Count; l++) {
                var line = m_bars[l];
                int y = 0;
                int w = 0;
                for (int b = 0; b < line.Count; b++) {
                    var bar = line[b];

                    if (bar.provsion != null && !bar.provsion.Provisioned)
                        continue;

                    if (bar.IsDisposed) //app is closing
                        return;

                    if (l == reserve.X && b == reserve.Y && reserve.Width > 0) { //reserve space
                        y += reserve.Height + 1;
                    }

                    bar.Location = new Point(x, y);
                    y += bar.Height + 1;
                    w = Math.Max(w, bar.Width);
                }

                x += w + 1;
            }

            this.Width = x;
            this.ResumeLayout();
        }
    }
}
