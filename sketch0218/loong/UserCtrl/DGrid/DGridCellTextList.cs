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
    public class DGridCellTextList : DataGridViewButtonCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;

        private string[] m_data = null;
        private int m_buttonWidth = 0;
        private int m_buttonHeight = 0;

        public DGridCellTextList() : base()
        {
            this.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;

            this.buttonWidth = 50;
            this.buttonText = "编辑";

            m_mergePivot = this;
        }

        ~DGridCellTextList()
        {
            if (this.DataGridView != null)
            {
                this.DataGridView.ColumnWidthChanged -= DataGridView_ColumnWidthChanged;
            }
        }

        public DGrid.CellType type
        {
            get
            {
                return DGrid.CellType.CELL_TEXT_LIST;
            }
        }

        protected override void OnContentClick(DataGridViewCellEventArgs e)
        {
            base.OnContentClick(e);

            if (!this.ReadOnly)
                DGrid_EditCtrl_TextList.instance.show(this);
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style)
        {
            base.InitializeEditingControl(row, val, style);
        }

        public override Type EditType
        {
            get
            {
                return typeof(TextBox);
            }
        }

        public object data
        {
            get
            {
                return m_data;
            }

            set
            {
                if (value != null && value.GetType() == typeof(string[]))
                    m_data = (string[])((string[])value).Clone();
                else
                    m_data = null;
            }
        }

        public int buttonWidth
        {
            get
            {
                return m_buttonWidth;
            }

            set
            {
                m_buttonWidth = value;
            }
        }

        public int buttonHeight
        {
            get
            {
                return m_buttonHeight;
            }

            set
            {
                m_buttonHeight = value;
            }
        }

        public string buttonText
        {
            get
            {
                return (string)this.Value;
            }

            set
            {
                this.Value = value;
            }
        }

        protected override void OnDataGridViewChanged()
        {
            base.OnDataGridViewChanged();

            if (this.DataGridView == null)
                return;

            this.DataGridView.ColumnWidthChanged += DataGridView_ColumnWidthChanged;
        }

        private void DataGridView_ColumnWidthChanged(object sender, DataGridViewColumnEventArgs e)
        {
            this._evalButtonSize();
        }

        protected void _evalButtonSize()
        {
            int w = this.Size.Width;
            int h = this.Size.Height;

            if (m_buttonWidth > 0)
            {
                w = m_buttonWidth <= this.Size.Width ? m_buttonWidth : this.Size.Width;
            }

            if (m_buttonHeight > 0)
            {
                h = m_buttonHeight <= this.Size.Height ? m_buttonHeight : this.Size.Height;
            }

            int padH = (this.Size.Width - w) / 2;
            int padV = (this.Size.Height - h) / 2;

            this.Style.Padding = new Padding(padH, padV, padH, padV);
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

            set
            {
                m_mergePivot = value;
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
