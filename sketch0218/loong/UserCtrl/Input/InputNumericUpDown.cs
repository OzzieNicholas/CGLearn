using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputNumericUpDown : NumericUpDown, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public InputNumericUpDown()
        {
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
        }

        [DefaultValue("")]
        public string PropName
        {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName
        {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue(true)]
        public bool CheckOnChange
        {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }
    }
}
