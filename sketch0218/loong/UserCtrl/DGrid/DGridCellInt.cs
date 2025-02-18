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
    public class DGridCellInt : DataGridViewTextBoxCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;
        
        public DGridCellInt() : base()
        {
            m_mergePivot = this;
            this.Style.SelectionBackColor = this.Style.BackColor;
            this.Style.SelectionForeColor = this.Style.ForeColor;
        }

        ~DGridCellInt()
        {

        }

        public DGrid.CellType type
        {
            get
            {
                return DGrid.CellType.CELL_INT;
            }
        }

        protected override void OnMouseClick(DataGridViewCellMouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (this.clickEdit && this.DataGridView != null && !this.ReadOnly)
                this.DataGridView.BeginEdit(true);
        }

        protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e)
        {
            base.OnMouseDoubleClick(e);

            if (this.doubleClickEdit && this.DataGridView != null && !this.ReadOnly)
                this.DataGridView.BeginEdit(true);
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style)
        {
            base.InitializeEditingControl(row, val, style);

        }

        public override Type EditType
        {
            get
            {
                return typeof(DGrid_EditCtrl_Int);
            }
        }

        public override Type ValueType
        {
            get
            {
                return typeof(int);
            }
        }

        public override object DefaultNewRowValue
        {
            get
            {
                return 0;
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
                if (value != null && value.GetType() == typeof(int))
                    this.Value = value;
                else
                    this.Value = 0;
            }
        }

        public override object ParseFormattedValue(object formattedValue, DataGridViewCellStyle cellStyle, TypeConverter formattedValueTypeConverter, TypeConverter valueTypeConverter)
        {
            if (formattedValue == null)
                return 0;

            if (formattedValue.GetType() != typeof(int))
                return 0;

            return formattedValue;
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

        public IDGridCell mergePivot
        {
            get
            {
                return m_mergePivot;
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
