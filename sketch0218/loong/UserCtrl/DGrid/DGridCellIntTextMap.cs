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
    public class DGridCellIntTextMap : DataGridViewComboBoxCell, IDGridCell
    {
        private bool m_clickEdit = true;
        private bool m_doubleClickEdit = false;
        private Point m_mergeRange = new Point(1, 1);
        private IDGridCell m_mergePivot = null;

        object[] m_items = null;
        Dictionary<string, int> m_s2imap = new Dictionary<string, int>();
        Dictionary<int, string> m_i2smap = new Dictionary<int, string>();

        public DGridCellIntTextMap() : base()
        {
            m_mergePivot = this;
        }

        ~DGridCellIntTextMap()
        {

        }

        public DGrid.CellType type
        {
            get
            {
                return DGrid.CellType.CELL_INT_TEXT_MAP;
            }
        }

        protected override void OnMouseDown(DataGridViewCellMouseEventArgs e)
        {
            base.OnMouseDown(e);

            //if (this.DataGridView != null && !this.ReadOnly)
            //{
            //    this.DataGridView.CurrentCell = this;
            //    this.DataGridView.BeginEdit(true);
            //}
        }

        //protected override void OnMouseClick(DataGridViewCellMouseEventArgs e)
        //{
        //    base.OnMouseDown(e);

        //    if (this.DataGridView != null && !this.ReadOnly)
        //        this.DataGridView.BeginEdit(true);
        //}

        protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e)
        {
            //base.OnMouseDoubleClick(e);
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style)
        {
            base.InitializeEditingControl(row, val, style);

            
        }

        public override Type EditType
        {
            get
            {
                return typeof(DataGridViewComboBoxEditingControl);
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
                if (this.Value == null)
                    return null;

                return this.m_s2imap[this.Value as string];
            }

            set
            {
                if (value != null)
                    this.Value = m_i2smap[(int)value];
                else
                    this.Value = null;
            }
        }

        public object items
        {
            get
            {
                return m_items;
            }

            set
            {
                if (value != null && value is object[])
                {
                    m_items = value as object[];
                    m_i2smap.Clear();
                    m_s2imap.Clear();
                    this.Items.Clear();

                    for (int i = 0; i < m_items.Length; i += 2)
                    {
                        int key = (int)m_items[i];
                        string val = m_items[i + 1] as string;
                        m_i2smap[key] = val;
                        m_s2imap[val] = key;
                        this.Items.Add(val);
                    }
                }
                else
                {
                    m_items = null;
                    m_i2smap.Clear();
                    m_s2imap.Clear();
                    this.Items.Clear();
                }
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
