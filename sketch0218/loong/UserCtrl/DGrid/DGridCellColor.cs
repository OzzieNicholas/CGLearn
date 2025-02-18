using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserCtrl
{
    public class DGridCellColor : DataGridViewTextBoxCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;
        protected Color m_color = Color.White;

        public DGridCellColor() : base() {
            this.color = Color.White;
            m_mergePivot = this;
        }

        ~DGridCellColor() {

        }

        public DGrid.CellType type {
            get {
                return DGrid.CellType.CELL_COLOR;
            }
        }

        protected override void OnMouseClick(DataGridViewCellMouseEventArgs e) {
            //base.OnMouseClick(e);

            if (this.clickEdit && !this.ReadOnly) {
                DialogResult ret = DGrid_EditCtrl_Color.instance.show(this);
                if (ret == DialogResult.OK) {
                    this.color = DGrid_EditCtrl_Color.instance.Color;
                }
            }
        }

        protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e) {
            //base.OnMouseDoubleClick(e);

            if (this.doubleClickEdit && !this.ReadOnly) {
                DialogResult ret = DGrid_EditCtrl_Color.instance.show(this);
                if (ret == DialogResult.OK) {
                    this.color = DGrid_EditCtrl_Color.instance.Color;
                }
            }
        }


        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style) {
            base.InitializeEditingControl(row, val, style);
        }

        public override Type EditType {
            get {
                return typeof(DataGridViewTextBoxEditingControl);
            }
        }


        public override Type ValueType {
            get {
                return typeof(string);
            }
        }

        public object data {
            get {
                return this.color;
            }

            set {
                if (value == null)
                    this.color = Color.White;
                else if (value.GetType() == typeof(int)) {
                    int v = (int)value;
                    this.color = Color.FromArgb((v & 0xff0000) >> 16, (v & 0xff00) >> 8, v & 0xff);
                }
                else if (value.GetType() == typeof(uint)) {
                    int v = (int)(uint)value;
                    this.color = Color.FromArgb((v & 0xff0000) >> 16, (v & 0xff00) >> 8, v & 0xff);
                }
                else
                    this.color = Color.White;
            }
        }

        [DefaultValue(typeof(Color), "0xFFFFFF")]
        public Color color {
            get => m_color;
            set {
                m_color = value;
                this.Style.SelectionBackColor = this.Style.BackColor = m_color;
            }
        }

        public object items {
            get => null;
            set { }
        }

        public bool clickEdit {
            get => m_clickEdit;
            set => m_clickEdit = value;
        }

        public bool doubleClickEdit {
            get => m_doubleClickEdit;
            set => m_doubleClickEdit = value;
        }

        public IDGridCell mergePivot {
            get => m_mergePivot;
        }

        public Point mergeRange {
            get => m_mergeRange;
            set => m_mergeRange = value;
        }

        public Rectangle bounds {
            get {
                if (m_mergePivot != this)
                    return new Rectangle(0, 0, 0, 0);

                int x0 = this.DataGridView.Width;
                int x1 = 0;
                int y0 = this.DataGridView.Height;
                int y1 = 0;

                for (int c = 0; c < m_mergeRange.X; ++c) {
                    for (int r = 0; r < m_mergeRange.Y; ++r) {
                        Rectangle cellrect = this.DataGridView.GetCellDisplayRectangle(c, r, false);
                        x0 = Math.Min(x0, cellrect.Left);
                        x1 = Math.Max(x1, cellrect.Right);
                        y0 = Math.Min(y0, cellrect.Top);
                        y1 = Math.Max(y1, cellrect.Bottom);
                    }
                }

                return new Rectangle(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            }
        }

        public void setMergePivot(IDGridCell cell) {
            m_mergePivot = cell;
        }

        public void setMergeRange(Point range) {
            m_mergeRange = range;
        }

        public DataGridViewCell origin => this;
        public DGrid grid => this.DataGridView as DGrid;

    }

}
