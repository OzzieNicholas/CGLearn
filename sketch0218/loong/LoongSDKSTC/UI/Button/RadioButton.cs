
using System;
using System.Drawing;
using System.ComponentModel;
using System.Collections.Generic;
using System.Windows.Forms;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class RadioButton : System.Windows.Forms.RadioButton, IParamCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private int m_radioTarget = -1;
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";

        public RadioButton() {

        }

        [DefaultValue("")]
        public string ParamName {
            get => m_paramName;
            set => m_paramName = value;
        }

        [DefaultValue(-1)]
        public int RadioTarget {
            get => m_radioTarget;
            set => m_radioTarget = value;
        }

        [DefaultValue("")]
        public string ControlCheckName {
            get => m_ctrlCheckName;
            set => m_ctrlCheckName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }
        
        public void SetParameter(object value)
        {
            this.Checked = this.RadioTarget == LoongCAD.Util.Utils.ToInt(value) ? true : false;
        }

        public object GetParameter()
        {
            return this.RadioTarget;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.RadioTarget;
        }

        public void ClearParameter()
        {
            this.Checked = false;
        }

    }
}
