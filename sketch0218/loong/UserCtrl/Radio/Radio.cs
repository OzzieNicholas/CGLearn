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
    public class Radio : RadioButton, IUserCtrl
    {
        private string m_propName = "";
        private int m_propTarget = -1;
        private string m_checkName = "";
        private string m_tabGroup = "";

        public Radio() {

        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(-1)]
        public int PropTarget {
            get => m_propTarget;
            set => m_propTarget = value;
        }
    }
}
