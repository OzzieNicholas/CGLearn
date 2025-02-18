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
    class CGridCellLabel : CGridCell
    {
        public CGridCellLabel(CGrid grid) : base(grid) {
            m_type = CGridCellType.LABEL;

            m_ctrl = new Label();
            m_ctrl.Dock = DockStyle.Fill;
            m_ctrl.AutoSize = true;
            m_ctrl.Anchor = AnchorStyles.None;
            m_ctrl.Margin = new Padding(3, 3, 3, 3);
        }

        public override object value {
            get => m_ctrl.Text;
            set => m_ctrl.Text = value as string;
        }
    }

}
