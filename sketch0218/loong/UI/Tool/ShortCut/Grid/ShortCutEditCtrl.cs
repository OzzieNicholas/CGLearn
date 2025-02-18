using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class ShortCutEditCtrl : Label, IDataGridViewEditingControl
    {
        private ShortCutGrid m_grid = null;
        private int m_rowIdx = -1;
        private bool m_changed = false;
        private FuncUnit m_fu = null;
        private uint m_sc = 0;

        public ShortCutEditCtrl() {
            this.TextAlign = ContentAlignment.MiddleLeft;
        }

        //interface
        public object EditingControlFormattedValue {
            get => this.Text;
            set => this.Text = value as string;
        }

        //interface
        public object GetEditingControlFormattedValue(DataGridViewDataErrorContexts context) {
            return EditingControlFormattedValue;
        }

        //interface
        public void ApplyCellStyleToEditingControl(DataGridViewCellStyle style) {
            this.Font = style.Font;
            this.ForeColor = style.ForeColor;
            this.BackColor = style.BackColor;
        }

        //interface
        public int EditingControlRowIndex {
            get => m_rowIdx;
            set => m_rowIdx = value;
        }

        // interface
        public bool EditingControlWantsInputKey(Keys key, bool dataGridViewWantsInputKey) {
            return true;
        }

        // interface
        public void PrepareEditingControlForEdit(bool selectAll) {
            ShortCutCell cell = this.EditingControlDataGridView.CurrentCell as ShortCutCell;
            EditingControlFormattedValue = FuncUnit.shortCut2Str(cell.shortCut);
            BackColor = cell.DataGridView.DefaultCellStyle.SelectionBackColor;
            Text = cell.Value as string;
            m_fu = cell.fu;
            m_sc = 0;
        }

        // interface
        public bool RepositionEditingControlOnValueChange => false;

        // interface
        public DataGridView EditingControlDataGridView {
            get => m_grid;
            set => m_grid = value as ShortCutGrid;
        }

        // interface
        public bool EditingControlValueChanged {
            get => m_changed;
            set => m_changed = value;
        }

        // interface
        public Cursor EditingPanelCursor => base.Cursor;

        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            switch (e.KeyCode) {
                case Keys.Menu:
                    m_sc |= 1 << 24;
                break;
                case Keys.ControlKey:
                    m_sc |= 1 << 25;
                break;
                case Keys.ShiftKey:
                    m_sc |= 1 << 26;
                break;
                default: {
                    if ((m_sc & 0xff) == (int)e.KeyCode
                        || ((m_sc >> 8) & 0xff) == (int)e.KeyCode
                        || ((m_sc >> 16) & 0xff) == (int)e.KeyCode) //重复键
                        return;

                    if (Native.isValidShortCutKey((int)e.KeyCode)) {
                        m_sc = (m_sc & 0xff000000) | ((m_sc & 0xffff) << 8) | ((uint)e.KeyCode & 0xff);
                        if ((m_sc & 0xff0000) != 0) { //最多三个快捷键
                            this._onNewShortCut();
                            return;
                        }
                        
                        this.EditingControlFormattedValue = FuncUnit.shortCut2Str(m_sc);
                    }
                }break;
            }
        }

        protected override void OnKeyUp(KeyEventArgs e) {
            base.OnKeyUp(e);

            if (m_sc == 0)
                return;

            this._onNewShortCut();
        }

        protected void _onNewShortCut() {
            var cell = m_grid.CurrentCell as ShortCutCell;

            if (m_sc == cell.shortCut) {
                m_sc = 0;
                return;
            }

            if ((m_sc & 0xffffff) == 0) {
                this.EditingControlFormattedValue = FuncUnit.shortCut2Str(cell.shortCut);
                m_sc = 0;
                return;
            }

            if (Native.isBuiltInShortCut(m_sc)) {
                MessageBox.Show("[" + FuncUnit.shortCut2Str(m_sc) + "] 已被系统保留,无法用做快捷键", "提示", MessageBoxButtons.OK);
                this.EditingControlFormattedValue = FuncUnit.shortCut2Str(cell.shortCut);
                m_sc = 0;
                return;
            }

            if (m_grid.updateCell(m_grid.CurrentCell as ShortCutCell, m_sc)) 
                this.EditingControlFormattedValue = FuncUnit.shortCut2Str(m_sc);
            else
                this.EditingControlFormattedValue = FuncUnit.shortCut2Str(cell.shortCut);

            this.EditingControlValueChanged = true;
            m_grid.NotifyCurrentCellDirty(true);
            m_sc = 0;
        }

        //右键清空
        protected override void OnMouseClick(MouseEventArgs e) {
            base.OnMouseClick(e);

            if (e.Button != MouseButtons.Right)
                return;

            if (!m_grid.updateCell(m_grid.CurrentCell as ShortCutCell, 0))
                return;

            m_sc = 0u;
            this.EditingControlFormattedValue = FuncUnit.shortCut2Str(0);
            this.EditingControlValueChanged = true;
            m_grid.NotifyCurrentCellDirty(true);
        }
    }
}
