using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class CGridCellTextButton : CGridCell
    {
        protected Action<CGridCell> m_onCellBtnClick = null;

        public CGridCellTextButton(CGrid grid) : base(grid) {
            m_type = CGridCellType.TEXT_BUTTON;

            var ctrl = new CGrid_Ctrl_TextButton();
            ctrl.Dock = DockStyle.Fill;
            ctrl.input.Validated += _onCellValidated;
            ctrl.button.Click += _onBtnClick;
            m_ctrl = ctrl;
        }

        public override object value {
            get => (m_ctrl as CGrid_Ctrl_TextButton).input.Value;
            set => (m_ctrl as CGrid_Ctrl_TextButton).input.Value = (string)value;
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as CGrid_Ctrl_TextButton).readOnly = value;
            }
        }

        public bool inputEnable {
            get => (m_ctrl as CGrid_Ctrl_TextButton).inputEnable;
            set => (m_ctrl as CGrid_Ctrl_TextButton).inputEnable = value;
        }

        public Action<CGridCell> onCellBtnClick {
            get => m_onCellBtnClick;
            set => m_onCellBtnClick = value;
        }

        protected void _onBtnClick(object sender, EventArgs e) {
            if (m_onCellBtnClick == null || m_lock > 0)
                return;

            m_onCellBtnClick(this);
        }
    }

    
}
