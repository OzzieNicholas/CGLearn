using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using UserCtrl;

namespace UserCtrl
{
    public class CGrid : TablePanel
    {
        private Dictionary<long, CGridCell> m_cells = new Dictionary<long, CGridCell>();
        private CGridCell m_selectedCell = null;
        protected SizeType m_defColSizeType = SizeType.Absolute;
        protected SizeType m_defRowSizeType = SizeType.AutoSize;
        protected float m_defColWidth = 100;
        protected float m_defRowHeight = 28;
        public CGrid() {
            this.BackColor = Color.Transparent;
            this.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;
            this.AutoScroll = false;
            this.ColumnCount = 2;
            this.RowCount = 1;
            this.DoubleBuffered = true;
            this.Size = new Size(10, 10);
            this.colResize = false;
            this.rowResize = false;
            this.TabStop = false;
        }

        public SizeType defColSizeType {
            get => m_defColSizeType;
            set => m_defColSizeType = value;
        }

        public SizeType defRowSizeType {
            get => m_defRowSizeType;
            set => m_defRowSizeType = value;
        }

        public float defColWidth {
            get => m_defColWidth;
            set => m_defColWidth = value;
        }

        public float defRowHeight {
            get => m_defRowHeight;
            set => m_defRowHeight = value;
        }

        public void setColWidths(float[] widths) {
            int n = Math.Min(widths.Length, this.ColumnStyles.Count);
            for (int i = 0; i < n; ++i)
                this.ColumnStyles[i].Width = widths[i];
        }

        public void setRowHeights(float[] heights) {
            int n = Math.Min(heights.Length, this.RowStyles.Count);
            for (int i = 0; i < n; ++i)
                this.RowStyles[i].Height = heights[i];
        }

        public int rowCount {
            get => this.RowCount;
            set {
                int old = this.RowCount;

                if (old > value) //行数变少，需清理删除行的Cell
                {
                    List<CGridCell> rmvCells = new List<CGridCell>();

                    foreach (var it in m_cells) {
                        if (it.Value.row >= value) {
                            rmvCells.Add(it.Value);
                        }
                    }

                    foreach (var cell in rmvCells) {
                        m_cells.Remove(cell.key);
                        cell._remove();
                    }
                }

                this.RowCount = value;

                while (this.RowStyles.Count < this.RowCount) {
                    this.RowStyles.Add(new RowStyle(m_defRowSizeType, m_defRowHeight));
                }
            }
        }

        public int colCount {
            get => this.ColumnCount;
            set {
                int old = this.ColumnCount;
                this.ColumnCount = value;

                if (old > value) //列数变少，需清理删除列的Cell
                {
                    List<CGridCell> rmvCells = new List<CGridCell>();

                    foreach (var it in m_cells) {
                        if (it.Value.col >= value) {
                            rmvCells.Add(it.Value);
                        }
                    }

                    foreach (var cell in rmvCells) {
                        m_cells.Remove(cell.key);
                        cell._remove();
                    }
                }

                while (this.ColumnStyles.Count < this.ColumnCount) {
                    this.ColumnStyles.Add(new ColumnStyle(m_defColSizeType, m_defColWidth));
                }
            }
        }

        public void clearCells() {
            //确保Controls.Clear时不会触发onCellValidated回调
            foreach (var it in m_cells)
                it.Value.onCellValidated = null;
            m_cells.Clear();

            this.Controls.Clear();

            m_selectedCell = null;
        }

        public void clearRow(int row) {
            List<CGridCell> rmvCells = new List<CGridCell>();

            foreach (var it in m_cells) {
                if (it.Value.row == row) {
                    rmvCells.Add(it.Value);
                }
            }

            foreach (var cell in rmvCells) {
                m_cells.Remove(cell.key);
                cell._remove();
            }
        }

        public CGridCell selectedCell {
            get => m_selectedCell;
            set => m_selectedCell = value;
        }

        public void insert(CGridCell cell, int col, int row) {
            if (cell.col == col && cell.row == row)
                return;

            cell._remove();
            cell._insert(col, row);
            m_cells[cell.key] = cell;
        }

        public void remove(CGridCell cell) {
            m_cells.Remove(cell.key);
            cell._remove();
        }

        public CGridCell newCell(CGridCellType type, int col, int row) {
            CGridCell oldCell = this.cell(col, row);
            if (oldCell != null && oldCell.type != type) {
                this.remove(oldCell);
                oldCell = null;
            }

            if (oldCell != null)
                return oldCell;

            CGridCell cell = null;
            if (type == CGridCellType.LABEL)
                cell = new CGridCellLabel(this);
            else if (type == CGridCellType.IMAGE)
                cell = new CGridCellImage(this);
            else if (type == CGridCellType.INPUT_DOUBLE)
                cell = new CGridCellInputDouble(this);
            else if (type == CGridCellType.INPUT_DOUBLE_ARRAY_SCRIPT)
                cell = new CGridCellInputDoubleArrayScript(this);
            else if (type == CGridCellType.INPUT_INT)
                cell = new CGridCellInputInt(this);
            else if (type == CGridCellType.INPUT_INT_ARRAY)
                cell = new CGridCellInputIntArray(this);
            else if (type == CGridCellType.INPUT_LONG)
                cell = new CGridCellInputLong(this);
            else if (type == CGridCellType.INPUT_TEXT)
                cell = new CGridCellInputText(this);
            else if (type == CGridCellType.CHECK)
                cell = new CGridCellCheck(this);
            else if (type == CGridCellType.BUTTON)
                cell = new CGridCellButton(this);
            else if (type == CGridCellType.LIST_TEXT)
                cell = new CGridCellListText(this);
            else if (type == CGridCellType.COMBOBOX)
                cell = new CGridCellCombo(this);
            else if (type == CGridCellType.COMBOBOX_INPUT)
                cell = new CGridCellComboInput(this);
            else if (type == CGridCellType.COMBOBOX_IMAGE)
                cell = new CGridCellComboImage(this);
            else if (type == CGridCellType.TEXT_BUTTON)
                cell = new CGridCellTextButton(this);

            if (cell != null)
                this.insert(cell, col, row);

            return cell;
        }

        public CGridCell cell(int col, int row) {
            long key = ((long)col << 32) + row;

            CGridCell cell = null;
            if (!m_cells.TryGetValue(key, out cell))
                return null;

            return cell;
        }

        public CGridCell setLabel(int col, int row, string v) {
            var cell = this.newCell(CGridCellType.LABEL, col, row);
            cell.value = v;
            return cell;
        }

        public CGridCell setImage(int col, int row, Image img) {
            var cell = this.newCell(CGridCellType.IMAGE, col, row);
            cell.value = img;
            return cell;
        }

        public CGridCell setInputDouble(int col, int row, double v) {
            var cell = this.newCell(CGridCellType.INPUT_DOUBLE, col, row);
            cell.value = v;
            return cell;
        }

        public CGridCell setInputString(int col, int row, string v) {
            var cell = this.newCell(CGridCellType.INPUT_TEXT, col, row);
            cell.value = v;
            return cell;
        }

        public CGridCell setInputInt(int col, int row, int v) {
            var cell = this.newCell(CGridCellType.INPUT_INT, col, row);
            cell.value = v;
            return cell;
        }

        public CGridCell setButton(int col, int row, string text, Action<CGridCell> onClick) {
            CGridCellButton cell = this.newCell(CGridCellType.BUTTON, col, row) as CGridCellButton;
            cell.btnText = text;
            cell.onCellBtnClick = onClick;
            return cell;
        }

        public void updateSize() {
            int[] widths = this.GetColumnWidths();
            int[] heights = this.GetRowHeights();

            int w = 0;
            for (int i = 0; i < widths.Length; ++i)
                w += widths[i];

            int h = 0;
            if (heights.Length > 1) {
                for (int i = 0; i < heights.Length - 1; ++i)
                    h += heights[i];

                if (Math.Abs(heights[heights.Length - 1] - heights[heights.Length - 2]) < 4)
                    h += heights[heights.Length - 1];
                else
                    h += heights[heights.Length - 2]; //最后一行高度差异过大,可能为填补高度差导致,不靠谱, 改为使用上一行
            }
            else if (heights.Length == 1)
                h = heights[0];
            else
                h = 10;

            this.Size = new Size(w, h);
        }

        public void updateHeight() {
            int[] heights = this.GetRowHeights();

            int h = 0;

            if (heights.Length > 1) {
                for (int i = 0; i < heights.Length - 1; ++i)
                    h += heights[i];

                if (Math.Abs(heights[heights.Length - 1] - heights[heights.Length - 2]) < 4)
                    h += heights[heights.Length - 1];
                else
                    h += heights[heights.Length - 2]; //最后一行高度差异过大,可能为填补高度差导致,不靠谱, 改为使用上一行
            }
            else if (heights.Length == 1)
                h = heights[0];
            else
                h = 10;

            this.Height = h;
        }
    }
}
