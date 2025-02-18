using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class CGridCellComboInput : CGridCell
    {
        public CGridCellComboInput(CGrid grid) : base(grid)
        {
            m_type = CGridCellType.COMBOBOX_INPUT;

            ComboBox cbo = new ComboBox();
            cbo.DropDownStyle = ComboBoxStyle.DropDown;
            cbo.KeyDown += Cbo_KeyDown;
            cbo.SelectionChangeCommitted += Cbo_SelectionChangeCommitted;
            cbo.GotFocus += _onCellGotFocus;
            cbo.LostFocus += _onCellLostFocus;
            cbo.Validated += _onCellValidated;
            cbo.Dock = DockStyle.Fill;
            m_ctrl = cbo;
        }

        public string[] items
        {
            get
            {
                ComboBox cbo = m_ctrl as ComboBox;

                string[] ret = new string[cbo.Items.Count];
                for (int i = 0; i < cbo.Items.Count; ++i)
                    ret[i] = cbo.Items[i] as string;
                
                return ret; 
            }

            set
            {
                ComboBox cbo = m_ctrl as ComboBox;
                cbo.Items.Clear();

                if (value == null)
                    return;

                cbo.Items.AddRange(value);
            }
        }

        public override object value
        {
            get
            {
                ComboBox cbo = m_ctrl as ComboBox;
                return cbo.Text.Trim();
            }

            set
            {
                ComboBox cbo = m_ctrl as ComboBox;
                cbo.Text = (value != null) ? value as string : "";
            }
        }

        public override bool readOnly
        {
            get => base.readOnly;
            
            set
            {
                ComboBox cbo = m_ctrl as ComboBox;
                m_readOnly = value;
                cbo.Enabled = !value;
            }
        }

        private void Cbo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                _onCellValidated(sender, e);
        }

        private void Cbo_SelectionChangeCommitted(object sender, EventArgs e)
        {
            ComboBox cbo = m_ctrl as ComboBox;
            if (cbo.SelectedItem != null)
                cbo.Text = cbo.SelectedItem as string;

            _onCellValidated(sender, e);
        }
    }

    
}
