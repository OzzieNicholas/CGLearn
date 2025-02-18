using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Geometry;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputObject : TextBox, IUserCtrl
    {

        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private DObject m_props = new DObject();

        public InputObject() {

        }

        [DefaultValue(null)]
        public DObject Value {
            get {
                return m_props;
            }

            set {
                if (value == null) {
                    m_props.Clear();
                }
                else {
                    m_props.CopyFrom(value);
                }
            }

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

    }
}
