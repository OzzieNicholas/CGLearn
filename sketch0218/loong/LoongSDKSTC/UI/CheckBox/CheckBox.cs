
using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class CheckBox : System.Windows.Forms.CheckBox, IParamCtrl, IPlaceholderCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";

        public CheckBox() {
        }

        [DefaultValue("")]
        public string ParamName {
            get => m_paramName;
            set => m_paramName = value;
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

        [DefaultValue(true)]
        public override bool AutoSize
        {
            get => base.AutoSize;
            set => base.AutoSize = value;
        }

        /// <inheritdoc/>
        public object Placeholder 
        { 
            get => this.Checked; 
            set
            {
                if (value != null && value is bool) {
                    this.Checked = (bool)value;
                }
            } 
        }

        /// <inheritdoc/>
        public void SetParameter(object value)
        {
            if (value is bool) {
                this.Checked = (bool)value;
            }
        }

        /// <inheritdoc/>
        public object GetParameter()
        {
            return this.Checked;
        }

        /// <inheritdoc/>
        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.Checked;
        }

        /// <inheritdoc/>
        public void ClearParameter()
        {
            this.Checked = true;
        }

        /// <inheritdoc/>
        public void ClearPlaceholder()
        {
#if false
            this.Checked = true;
#endif
        }

    }
}
