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
    public class DGridCellBool : DataGridViewCheckBoxCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;

        public DGridCellBool() : base()
        {
            this.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            this.Style.SelectionBackColor = Color.White;
            this.Style.SelectionForeColor = Color.Black;

            m_mergePivot = this;
        }

        ~DGridCellBool()
        {

        }

        public DGrid.CellType type
        {
            get
            {
                return DGrid.CellType.CELL_BOOL;
            }
        }

        protected override void OnMouseClick(DataGridViewCellMouseEventArgs e)
        {
            //base.OnMouseDown(e);

            this.data = !(bool)this.data;
            this.DataGridView.NotifyCurrentCellDirty(true);
            this.grid.onCellValidated(this.ColumnIndex, this.RowIndex);
        }

        protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e)
        {
            //base.OnMouseDoubleClick(e);
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style)
        {
            //base.InitializeEditingControl(row, val, style);
        }

        public override Type EditType
        {
            get
            {
                return typeof(TextBox);
            }
        }

        public override Type ValueType
        {
            get
            {
                return typeof(bool);
            }
        }

        public object data
        {
            get
            {
                return this.Value != null;
            }

            set
            {
                if (value == null || (bool)value == false)
                    this.Value = null;
                else
                    this.Value = true;
            }
        }

        public object items {
            get
            {
                return null;
            }

            set
            {

            }
        }

        protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, TypeConverter valueTypeConverter, TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
        {
            return value;
            //return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
        }

        public bool clickEdit
        {
            get
            {
                return m_clickEdit;
            }

            set
            {
                m_clickEdit = value;
            }
        }

        public bool doubleClickEdit
        {
            get
            {
                return m_doubleClickEdit;
            }

            set
            {
                m_doubleClickEdit = value;
            }
        }

        public DataGridViewCell origin
        {
            get
            {
                return this;
            }
        }

        public IDGridCell mergePivot {
            get
            {
                return m_mergePivot;
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
                m_mergeRange = value;
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

        public void setMergePivot(IDGridCell cell)
        {
            m_mergePivot = cell;
        }

        public void setMergeRange(Point range)
        {
            m_mergeRange = range;
        }

        public DGrid grid {
            get
            {
                return this.DataGridView as DGrid;
            }
        }
    }

}
