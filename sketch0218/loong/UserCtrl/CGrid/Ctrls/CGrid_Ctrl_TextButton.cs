using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

using UserCtrl;

namespace UserCtrl
{
    [ToolboxItem(false)]
    public partial class CGrid_Ctrl_TextButton : UserControl
    {
        private bool m_readOnly = false;
        private bool m_inputEnable = false;

        public CGrid_Ctrl_TextButton()
        {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls()
        {
            
        }

        public bool readOnly
        {
            get
            {
                return m_readOnly;
            }

            set
            {
                m_readOnly = value;
                tb_value.ReadOnly = m_readOnly || !m_inputEnable;
                btn_func.Enabled = !m_readOnly;
            }
        }

        public bool inputEnable
        {
            get
            {
                return m_inputEnable;
            }

            set
            {
                m_inputEnable = value;
                tb_value.ReadOnly = m_readOnly || !m_inputEnable;
            }
        }

        public InputText input
        {
            get
            {
                return tb_value;
            }
        }

        public Button button
        {
            get
            {
                return btn_func;
            }
        }
    }
}
