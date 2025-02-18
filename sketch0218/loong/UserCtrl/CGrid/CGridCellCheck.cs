using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;


namespace UserCtrl
{
    public class CGridCellCheck : CGridCell
    {
        public CGridCellCheck(CGrid grid) : base(grid)
        {
            m_type = CGridCellType.CHECK;

            m_ctrl = new Check();
            (m_ctrl as Check).AutoSize = true;
            m_ctrl.Dock = DockStyle.Fill;
            m_ctrl.Anchor = AnchorStyles.Left;
            (m_ctrl as Check).MouseClick += _onCellValidated;
        }

        public override object value
        {
            get
            {
                return (m_ctrl as Check).Checked;
            }

            set
            {
                (m_ctrl as Check).Checked = (bool)value;
            }
        }

        public override bool readOnly
        {
            get => base.readOnly;
            
            set
            {
                m_readOnly = value;
                (m_ctrl as Check).Enabled = !value;
            }
        }

    }

    
}
