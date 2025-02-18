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
    public class Group : GroupBox, IUserCtrl
    {
        private string m_tabGroup = "";

        public Group() {
            
        }

        [Browsable(false), DefaultValue("")]
        public string PropName {
            get => "";
            set { }
        }

        [Browsable(false), DefaultValue("")]
        public string CheckName {
            get => "";
            set { }
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }
    }
}
