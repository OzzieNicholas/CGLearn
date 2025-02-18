using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserCtrl
{
    [ToolboxItem(false)]
    public class DGrid_EditCtrl_Int: InputInt, IDataGridViewEditingControl
    {
        protected DataGridView m_dataGridView = null;
        protected int m_rowIndex = -1;
        protected bool m_changed = false;

        public DGrid_EditCtrl_Int()
        {
           
        }

        //interface
        public object EditingControlFormattedValue
        {
            get
            {
                return this.Value;
            }

            set
            {
                if (value != null && value.GetType() == typeof(int))
                    this.Value = (int)value;
                else
                    this.Value = 0;
            }
        }

        //interface
        public object GetEditingControlFormattedValue(DataGridViewDataErrorContexts context)
        {
            return EditingControlFormattedValue;
        }

        //interface
        public void ApplyCellStyleToEditingControl(DataGridViewCellStyle style)
        {
            this.Font = style.Font;
            this.ForeColor = style.ForeColor;
            this.BackColor = style.BackColor;
        }

        //interface
        public int EditingControlRowIndex
        {
            get
            {
                return m_rowIndex;
            }
            set
            {
                m_rowIndex = value;
            }
        }

        // interface
        public bool EditingControlWantsInputKey(Keys key, bool dataGridViewWantsInputKey)
        {
            return true;
        }

        // interface
        public void PrepareEditingControlForEdit(bool selectAll)
        {
            // No preparation needs to be done.
        }

        // interface
        public bool RepositionEditingControlOnValueChange
        {
            get
            {
                return false;
            }
        }

        // interface
        public DataGridView EditingControlDataGridView
        {
            get
            {
                return m_dataGridView;
            }

            set
            {
                m_dataGridView = value;
            }
        }

        // interface
        public bool EditingControlValueChanged
        {
            get
            {
                return m_changed;
            }

            set
            {
                m_changed = value;
            }
        }

        // interface
        public Cursor EditingPanelCursor
        {
            get
            {
                return base.Cursor;
            }
        }

        protected override void OnValidated(EventArgs e)
        {
            base.OnValidated(e);

            m_changed = true;

            this.EditingControlDataGridView.CurrentCell.Value = this.Value;
            this.EditingControlDataGridView.NotifyCurrentCellDirty(true);
        }

        protected override void OnLostFocus(EventArgs e)
        {
            base.OnLostFocus(e);

            m_changed = true;

            this.EditingControlDataGridView.CurrentCell.Value = this.Value;
            this.EditingControlDataGridView.NotifyCurrentCellDirty(true);
        }
    }
}
