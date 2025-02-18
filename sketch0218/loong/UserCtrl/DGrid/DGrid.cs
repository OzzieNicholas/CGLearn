using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using UserCtrl;

namespace UserCtrl
{
    //Deprecated: 做的不是很好用，不建议使用 (- -||)
    //控件队列可尝试使用CGrid
    public class DGrid : DataGridView
    {
        public enum CellType
        {
            CELL_NONE = 0,
            CELL_TEXT, CELL_TEXT_LIST,
            CELL_INT, CELL_INT_LIST,
            CELL_DOUBLE, CElL_DOUBLE_LIST,
            CELL_BOOL,
            CELL_COLOR,
            CELL_TEXT_MAP,
            CELL_INT_TEXT_MAP,
            CELL_FILE,
            CELL_IMAGE_FILE,
            CELL_DIRECTORY,
            CELL_COMBO_KEY,
            CELL_COMBO_IMAGE
        };

        protected StringFormat m_cellFmt = new StringFormat();

        public DGrid() {
            this.BackgroundColor = Color.White;
            this.ColumnHeadersVisible = false;
            this.RowHeadersVisible = false;
            this.AllowUserToAddRows = false;

            m_cellFmt.LineAlignment = StringAlignment.Center;
            m_cellFmt.Alignment = StringAlignment.Center;

            this.init(1, 1);

            this.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
        }

        public void init(int colnum, int rownum) {
            this.ColumnCount = colnum;
            this.RowCount = rownum;

            for (int col = 0; col < colnum; ++col) {
                for (int row = 0; row < rownum; ++row) {
                    DGridCellText ce = new DGridCellText();
                    this[col, row] = ce;
                    ce.ReadOnly = true;
                }
            }
        }

        public IDGridCell cell(int col, int row) {
            if (col < 0 || col > this.ColumnCount - 1)
                return null;
            if (row < 0 || row > this.RowCount - 1)
                return null;

            if (!(this[col, row] is IDGridCell)) {
                this[col, row] = new DGridCellText();
            }

            return (IDGridCell)this[col, row];
        }

        public IDGridCell setCellType(int col, int row, DGrid.CellType type) {
            IDGridCell curCell = this.cell(col, row);
            if (curCell != null && curCell.type == type)
                return curCell;

            switch (type) {
                case CellType.CELL_TEXT:
                this[col, row] = new DGridCellText();
                break;
                case CellType.CELL_TEXT_LIST:
                this[col, row] = new DGridCellTextList();
                break;
                case CellType.CELL_INT:
                this[col, row] = new DGridCellInt();
                break;
                case CellType.CELL_DOUBLE:
                this[col, row] = new DGridCellDouble();
                break;
                case CellType.CELL_COLOR:
                this[col, row] = new DGridCellColor();
                break;
                case CellType.CELL_BOOL:
                this[col, row] = new DGridCellBool();
                break;
                case CellType.CELL_TEXT_MAP:
                this[col, row] = new DGridCellTextMap();
                break;
                case CellType.CELL_INT_TEXT_MAP:
                this[col, row] = new DGridCellIntTextMap();
                break;
                case CellType.CELL_FILE:
                this[col, row] = new DGridCellFile();
                break;
                case CellType.CELL_IMAGE_FILE:
                this[col, row] = new DGridCellImageFile();
                break;
            }

            return (IDGridCell)this[col, row];
        }

        public void clearCell(int col, int row) {
            if (col < 0 || col > this.ColumnCount - 1)
                return;
            if (row < 0 || row > this.RowCount - 1)
                return;

            IDGridCell curCell = this.cell(col, row);
            if (curCell == null || curCell.type != CellType.CELL_TEXT)
                curCell = this.setCellType(col, row, CellType.CELL_TEXT);

            curCell.data = null;
            curCell.ReadOnly = true;
        }

        protected override void OnCellPainting(DataGridViewCellPaintingEventArgs e) {
            base.OnCellPainting(e);

            if (e.ColumnIndex < 0 || e.RowIndex < 0)
                return;

            IDGridCell cell = this.cell(e.ColumnIndex, e.RowIndex);
            if (cell == null)
                return;

            int col0 = cell.mergePivot.ColumnIndex;
            int col1 = cell.mergePivot.ColumnIndex + cell.mergePivot.mergeRange.X - 1;
            int row0 = cell.mergePivot.RowIndex;
            int row1 = cell.mergePivot.RowIndex + cell.mergePivot.mergeRange.Y - 1;

            if (cell.mergePivot.origin.Selected && !cell.mergePivot.origin.IsInEditMode) {
                using (var br = new SolidBrush(e.CellStyle.SelectionBackColor)) {
                    e.Graphics.FillRectangle(br, e.CellBounds);
                }
            }
            else {
                using (var br = new SolidBrush(e.CellStyle.BackColor)) {
                    e.Graphics.FillRectangle(br, e.CellBounds);
                }
            }

            using (var pen = new Pen(this.GridColor)) {
                if (cell.ColumnIndex == col1) {
                    e.Graphics.DrawLine(pen, e.CellBounds.Right - 1, e.CellBounds.Top, e.CellBounds.Right - 1, e.CellBounds.Bottom);
                }

                if (cell.RowIndex == row1) {
                    e.Graphics.DrawLine(pen, e.CellBounds.Left, e.CellBounds.Bottom - 1, e.CellBounds.Right, e.CellBounds.Bottom - 1);
                }
            }
            
            if (cell.type == CellType.CELL_TEXT) {
                e.Handled = true;
            }
            else {
                e.AdvancedBorderStyle.All = DataGridViewAdvancedCellBorderStyle.None;
                e.CellStyle.BackColor = Color.Transparent;
                e.CellStyle.SelectionBackColor = Color.Transparent;
            }
        }

        protected override void OnPaint(PaintEventArgs e) {
            base.OnPaint(e);

            for (int c = 0; c < this.ColumnCount; ++c) {
                for (int r = 0; r < this.RowCount; ++r) {
                    IDGridCell cell = this.cell(c, r);
                    if (cell.type == CellType.CELL_TEXT && cell.mergePivot == cell && cell.data != null && !cell.mergePivot.origin.IsInEditMode) {
                        Rectangle bounds = cell.bounds;
                        e.Graphics.DrawString(cell.origin.Value as string, this.Font, Brushes.Black, bounds, m_cellFmt);
                    }
                }
            }
        }

        protected override void OnCellEndEdit(DataGridViewCellEventArgs e) {
            this.SuspendLayout();

            base.OnCellEndEdit(e);

            this.Invalidate();
            this.onCellValidated(e.ColumnIndex, e.RowIndex);

            this.ResumeLayout();
        }

        protected override void OnSelectionChanged(EventArgs e) {
            this.SuspendLayout();

            base.OnSelectionChanged(e);

            foreach (var obj in this.SelectedCells) {
                if (!(obj is IDGridCell))
                    continue;

                IDGridCell cell = obj as IDGridCell;

                int col0 = Math.Min(cell.mergePivot.ColumnIndex, this.ColumnCount - 1);
                int col1 = Math.Min(cell.mergePivot.ColumnIndex + cell.mergePivot.mergeRange.X - 1, this.ColumnCount - 1);
                int row0 = Math.Min(cell.mergePivot.RowIndex, this.RowCount - 1);
                int row1 = Math.Min(cell.mergePivot.RowIndex + cell.mergePivot.mergeRange.Y - 1, this.RowCount - 1);

                for (int c = col0; c <= col1; ++c) {
                    for (int r = row0; r <= row1; ++r) {
                        if (!this[c, r].Selected)
                            this.SetSelectedCellCore(c, r, true);
                    }
                }
            }

            this.ResumeLayout();
        }

        protected override void OnMouseClick(MouseEventArgs e) {
            this.SuspendLayout();

            base.OnMouseClick(e);

            int w = 0;
            int h = 0;

            if (this.RowHeadersVisible)
                w += this.RowHeadersWidth;

            if (this.ColumnHeadersVisible)
                h += this.ColumnHeadersHeight;

            for (int i = 0; i < this.ColumnCount; ++i)
                w += this.Columns[i].Width;
            for (int i = 0; i < this.RowCount; ++i)
                h += this.Rows[i].Height;

            if (e.X > w || e.Y > h)
                this.ClearSelection();

            this.ResumeLayout();
        }

        public virtual void onCellValidated(int col, int row) {

        }
    }

    
}
