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
    public class DGridCellText : DataGridViewTextBoxCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;

        public DGridCellText() : base()
        {
            m_mergePivot = this;
            this.Style.SelectionBackColor = this.Style.BackColor;
            this.Style.SelectionForeColor = this.Style.ForeColor;
        }

        ~DGridCellText()
        {

        }

        public DGrid.CellType type
        {
            get
            {
                return DGrid.CellType.CELL_TEXT;
            }
        }

        protected override void OnMouseClick(DataGridViewCellMouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (m_clickEdit && this.DataGridView != null && !this.ReadOnly)
            {
                this.DataGridView.CurrentCell = m_mergePivot.origin;
                this.DataGridView.BeginEdit(true);
            }
                
        }

        protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e)
        {
            base.OnMouseDoubleClick(e);

            if (m_doubleClickEdit && this.DataGridView != null && !this.ReadOnly)
            {
                this.DataGridView.CurrentCell = m_mergePivot.origin;
                this.DataGridView.BeginEdit(true);
            }
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style)
        {
            base.InitializeEditingControl(row, val, style);
        }

        public override Type EditType
        {
            get
            {
                return typeof(DataGridViewTextBoxEditingControl);
            }
        }

        public override Type ValueType
        {
            get
            {
                return typeof(string);
            }
        }

        public object data
        {
            get
            {
                return this.Value;
            }

            set
            {
                if (value != null && value.GetType() == typeof(string))
                    this.Value = value;
                else
                    this.Value = null;
            }
        }

        public object items
        {
            get
            {
                return null;
            }

            set
            {

            }
        }

        public bool clickEdit {
            get
            {
                return m_clickEdit;
            }

            set
            {
                m_clickEdit = value;
            }
        }

        public bool doubleClickEdit {
            get
            {
                return m_doubleClickEdit;
            }

            set
            {
                m_doubleClickEdit = value;
            }
        }

        public Point mergeRange
        {
            get
            {
                return m_mergeRange;
            }

            set
            {
                if (m_mergePivot == this)
                {
                    //reset old range
                    for (int col = 0; col < m_mergeRange.X; ++col)
                    {
                        for (int row = 0; row < m_mergeRange.Y; ++row)
                        {
                            IDGridCell cell = this.grid.cell(this.ColumnIndex + col, this.RowIndex + row);
                            if (cell != this)
                            {
                                cell.setMergePivot(cell);
                                cell.setMergeRange(new Point(1, 1));
                            }
                        }
                    }

                    m_mergeRange = value;

                    //set new range
                    for (int col = 0; col < m_mergeRange.X; ++col)
                    {
                        for (int row = 0; row < m_mergeRange.Y; ++row)
                        {
                            IDGridCell cell = this.grid.cell(this.ColumnIndex + col, this.RowIndex + row);
                            if (cell != this)
                            {
                                cell.setMergePivot(this);
                            }
                        }
                    }
                }
                else
                {
                    int maxCol = Math.Max(m_mergePivot.ColumnIndex + m_mergePivot.mergeRange.X - 1,
                                        this.ColumnIndex + value.X - 1);

                    int maxRow = Math.Max(m_mergePivot.RowIndex + m_mergePivot.mergeRange.Y - 1,
                                        this.RowIndex + value.Y - 1);

                    m_mergePivot.mergeRange = new Point(maxCol - m_mergePivot.ColumnIndex + 1, maxRow - m_mergePivot.RowIndex + 1);
                }
            }
        }

        public Rectangle bounds
        {
            get
            {
                if (m_mergePivot != this)
                    return new Rectangle(0, 0, 0, 0);

                int x0 = this.DataGridView.Width;
                int x1 = 0;
                int y0 = this.DataGridView.Height;
                int y1 = 0;

                for (int c = 0; c < m_mergeRange.X; ++c)
                {
                    for (int r = 0; r < m_mergeRange.Y; ++r)
                    {
                        Rectangle cellrect = this.DataGridView.GetCellDisplayRectangle(this.ColumnIndex + c, this.RowIndex + r, false);
                        x0 = Math.Min(x0, cellrect.Left);
                        x1 = Math.Max(x1, cellrect.Right);
                        y0 = Math.Min(y0, cellrect.Top);
                        y1 = Math.Max(y1, cellrect.Bottom);
                    }
                }

                return new Rectangle(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            }
        }

        public IDGridCell mergePivot
        {
            get
            {
                return m_mergePivot;
            }

            set
            {
                m_mergePivot = value;
            }
        }

        public void setMergePivot(IDGridCell cell)
        {
            m_mergePivot = cell;
        }

        public void setMergeRange(Point range)
        {
            m_mergeRange = range;
        }

        public DataGridViewCell origin
        {
            get
            {
                return this;
            }
        }

        public DGrid grid
        {
            get
            {
                return this.DataGridView as DGrid;
            }
        }
    }

}
