using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    class CGridCellButton : CGridCell
    {
        protected Action<CGridCell> m_onCellBtnClick = null;

        public CGridCellButton(CGrid grid) : base(grid)
        {
            m_type = CGridCellType.BUTTON;

            Button btn = new Button();
            btn.Text = "...";
            btn.FlatStyle = FlatStyle.Standard;
            btn.BackColor = Color.Transparent;
            btn.AutoSize = true;
            btn.Dock = DockStyle.Fill;
            btn.Anchor = AnchorStyles.None;
            btn.Margin = new Padding(1);
            btn.Click += _onBtnClick;
            m_ctrl = btn;
        }

        public string btnText
        {
            get
            {
                return (m_ctrl as Button).Text;
            }

            set
            {
                (m_ctrl as Button).Text = value;
            }
        }

        public override object value
        {
            get
            {
                return null;
            }

            set
            {
                
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

        public Action<CGridCell> onCellBtnClick
        {
            get
            {
                return m_onCellBtnClick;
            }

            set
            {
                m_onCellBtnClick = value;
            }
        }

        protected void _onBtnClick(object sender, EventArgs e)
        {
            if (m_onCellBtnClick == null || m_lock > 0)
                return;

            m_onCellBtnClick(this);
        }

        
    }

    
}
