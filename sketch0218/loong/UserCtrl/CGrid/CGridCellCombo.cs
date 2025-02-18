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
    public class CGridCellCombo : CGridCell
    {
        Dictionary<object, string> m_key2Item = new Dictionary<object, string>();
        Dictionary<int, object> m_idx2Key = new Dictionary<int, object>();

        public CGridCellCombo(CGrid grid) : base(grid)
        {
            m_type = CGridCellType.COMBOBOX;

            ComboBox cbo = new ComboBox();
            cbo.Margin = new Padding(1);
            cbo.DropDownStyle = ComboBoxStyle.DropDownList;
            cbo.SelectionChangeCommitted += _onCellValidated;
            cbo.Dock = DockStyle.Fill;
            m_ctrl = cbo;
        }

        public Dictionary<object, string> items
        {
            get
            {
                return new Dictionary<object, string>(m_key2Item); 
            }

            set
            {
                if (value == null)
                    return;

                ComboBox cbo = m_ctrl as ComboBox;
                cbo.Items.Clear();

                m_key2Item = new Dictionary<object, string>(value);
                m_idx2Key.Clear();
                foreach (var it in m_key2Item)
                {
                    int idx = m_idx2Key.Count;
                    m_idx2Key[idx] = it.Key;
                    cbo.Items.Add(it.Value);
                }
            }
        }

        public override object value
        {
            get
            {
                ComboBox cbo = m_ctrl as ComboBox;

                if (cbo.SelectedIndex >= 0)
                    return m_idx2Key[cbo.SelectedIndex];

                return null;
            }

            set
            {
                ComboBox cbo = m_ctrl as ComboBox;

                string item = null;
                if (m_key2Item.TryGetValue(value, out item))
                {
                    cbo.SelectedItem = item;
                }
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

    }

    
}
