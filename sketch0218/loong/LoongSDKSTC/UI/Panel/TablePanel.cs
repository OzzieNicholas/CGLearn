using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class TablePanel : TableLayoutPanel, ITabOrderCtrl
    {
        private string m_tabGroup = "";

        private bool m_colResize = true;
        private bool m_rowResize = false;
        private bool m_resizing = false;
        private int m_resizeColIdx = -1;
        private int m_resizeRowIdx = -1;

        public TablePanel() {
            this.DoubleBuffered = true;
            //this.SetStyle(ControlStyles.UserPaint, true);
            //this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [Description("Col Resize")]
        public bool colResize {
            get => m_colResize;
            set => m_colResize = value;
        }

        [Description("Row Resize")]
        public bool rowResize {
            get => m_rowResize;
            set => m_rowResize = value;
        }

        protected override void OnMouseDown(MouseEventArgs e) {
            base.OnMouseDown(e);

            if (e.Button == MouseButtons.Left) {
                if (!m_resizing && (m_resizeColIdx >= 0 || m_resizeRowIdx >= 0)) {
                    m_resizing = true;
                }
            }
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (!m_resizing) {
                m_resizeColIdx = -1;
                m_resizeRowIdx = -1;

                //for columns
                if (m_colResize) {
                    int[] colWidths = this.GetColumnWidths();
                    int width = 0;
                    for (int i = 0; i < this.ColumnStyles.Count - 1; i++) {
                        width += colWidths[i];
                        if (e.X > width - 3 && e.X < width + 3) {
                            m_resizeColIdx = i;
                            break;
                        }
                    }
                }

                //for rows
                if (m_rowResize) {
                    int[] rowHeights = this.GetRowHeights();
                    int height = 0;
                    for (int i = 0; i < this.RowStyles.Count - 1; i++) {
                        height += rowHeights[i];
                        if (e.Y > height - 3 && e.Y < height + 3) {
                            m_resizeRowIdx = i;
                            break;
                        }
                    }
                }

                if (m_resizeColIdx >= 0 && m_resizeRowIdx >= 0)
                    this.Cursor = Cursors.Cross;
                else if (m_resizeColIdx >= 0)
                    this.Cursor = Cursors.VSplit;
                else if (m_resizeRowIdx >= 0)
                    this.Cursor = Cursors.HSplit;
                else
                    this.Cursor = Cursors.Default;
            }
            else {
                this.SuspendLayout();

                if (m_resizeColIdx >= 0) {
                    int[] colWidths = this.GetColumnWidths();
                    int offset = 0;
                    for (int i = 0; i < m_resizeColIdx; i++)
                        offset += colWidths[i];

                    int width = Math.Max(e.X - offset, 2);
                    if (this.ColumnStyles[m_resizeColIdx].SizeType == SizeType.Percent)
                        this.ColumnStyles[m_resizeColIdx].Width = (float)width / (float)this.Width;
                    else if (this.ColumnStyles[m_resizeColIdx].SizeType == SizeType.Absolute)
                        this.ColumnStyles[m_resizeColIdx].Width = (float)width;
                }

                if (m_resizeRowIdx >= 0) {
                    int[] rowHeights = this.GetRowHeights();
                    int offset = 0;
                    for (int i = 0; i < m_resizeRowIdx; i++)
                        offset += rowHeights[i];

                    int height = Math.Max(e.X - offset, 2);
                    if (this.RowStyles[m_resizeColIdx].SizeType == SizeType.Percent)
                        this.RowStyles[m_resizeColIdx].Height = (float)height / (float)this.Height;
                    else if (this.RowStyles[m_resizeColIdx].SizeType == SizeType.Absolute)
                        this.RowStyles[m_resizeColIdx].Height = (float)height;
                }

                this.ResumeLayout();
            }
        }

        protected override void OnMouseUp(MouseEventArgs e) {
            base.OnMouseUp(e);

            if (m_resizing) {
                m_resizing = false;
                this.Cursor = Cursors.Default;
            }
        }
    }
}
