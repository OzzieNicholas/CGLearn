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
    public class CGridCellListText : CGridCell
    {
        string[] m_value = new string[0];

        public CGridCellListText(CGrid grid) : base(grid)
        {
            m_type = CGridCellType.CHECK;

            m_ctrl = new Button();
            (m_ctrl as Button).AutoSize = true;
            (m_ctrl as Button).FlatStyle = FlatStyle.Standard;
            (m_ctrl as Button).BackColor = Color.Transparent;
            m_ctrl.Text = "...";
            m_ctrl.Dock = DockStyle.Fill;
            m_ctrl.Anchor = AnchorStyles.Left;
            (m_ctrl as Button).MouseClick += Btn_MouseClick;
        }

        public override object value
        {
            get
            {
                return m_value;
            }

            set
            {
                if (value != null)
                    m_value = (value as string[]).ToArray();
                else
                    m_value = new string[0];
            }
        }

        public override bool readOnly
        {
            get => base.readOnly;
            
            set
            {
                m_readOnly = value;
                (m_ctrl as Button).Enabled = !value;
            }
        }

        private void Btn_MouseClick(object sender, MouseEventArgs e)
        {
            CGrid_EditCtrl_ListText dlg = new CGrid_EditCtrl_ListText();
            if (dlg.show(m_value) == DialogResult.OK)
            {
                m_value = dlg.value;

                if (m_onCellValidated != null)
                    m_onCellValidated(this);
            }
        }
    }

    
}
