using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    class CGridCell
    {
        protected CGrid m_grid = null;
        protected CGridCellType m_type = CGridCellType.NONE;
        protected int m_col = -1;
        protected int m_row = -1;

        protected Control m_ctrl = null;
        protected bool m_readOnly = false;
        protected Action<CGridCell> m_onCellValidated = null;

        protected object m_attchedData = null; //附加数据（用户自定义使用）

        protected int m_lock = 0;

        public CGridCell(CGrid grid) {
            m_grid = grid;
        }

        public CGrid grid => m_grid;
        public CGridCellType type => m_type;
        public virtual object value {
            get => null;
            set {

            }
        }

        //插入Grid
        public virtual void _insert(int col, int row) {
            if (m_col == col && m_row == row)
                return;

            if (m_col >= 0 && m_row >= 0)
                m_grid.Controls.Remove(m_ctrl);

            m_grid.Controls.Add(m_ctrl, col, row);

            m_col = col;
            m_row = row;
        }

        //从Grid移除
        public virtual void _remove() {
            m_lock += 1; //锁定确保Controls.Remove不会触发onCellValidated回调

            if (m_col >= 0 && m_row >= 0)
                m_grid.Controls.Remove(m_ctrl);

            m_col = -1;
            m_row = -1;

            m_lock -= 1;

            if (m_grid.selectedCell == this)
                m_grid.selectedCell = null;
        }

        public long key {
            get {
                if (m_col < 0 || m_row < 0)
                    return -1;

                return ((long)col << 32) + row;
            }
        }

        public int col => m_col;
        public int row => m_row;

        public object attchedData {
            get => m_attchedData;
            set => m_attchedData = value;
        }

        public virtual bool readOnly {
            get => m_readOnly;
            set => m_readOnly = value;
        }

        public Control ctrl => m_ctrl;

        public Action<CGridCell> onCellValidated {
            get => m_onCellValidated;
            set => m_onCellValidated = value;
        }

        public void invokeValidated() {
            this._onCellValidated(null, null);
        }

        protected void _onCellGotFocus(object sender, EventArgs e) {
            m_grid.selectedCell = this;
        }

        protected void _onCellLostFocus(object sender, EventArgs e) {
            if (m_grid.selectedCell == this)
                m_grid.selectedCell = null;
        }

        protected void _onCellValidated(object sender, EventArgs e) {
            if (m_onCellValidated == null || m_lock > 0)
                return;

            m_onCellValidated(this);
        }

    }

    
}
