using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class NumericUpDownInput : NumericUpDown, IParamCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public NumericUpDownInput()
        {
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
        }

        [DefaultValue("")]
        public string ParamName
        {
            get => m_paramName;
            set => m_paramName = value;
        }

        [DefaultValue("")]
        public string ControlCheckName
        {
            get => m_ctrlCheckName;
            set => m_ctrlCheckName = value;
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

        public void SetParameter(object value)
        {
            if (value is int) {
                this.Value = (int)value;
            }
        }

        public object GetParameter()
        {
            return this.Value;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.Value;
        }

        public void ClearParameter()
        {
            this.Value = 0;
        }

    }
}
