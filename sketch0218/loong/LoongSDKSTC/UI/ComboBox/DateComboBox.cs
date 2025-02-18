using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Geometry;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class DateComboBox : DateTimePicker, IParamCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public DateComboBox() {
            this.MinDate = new DateTime(1975, 1, 1);
            this.CustomFormat = "yyyy-MM-dd";
            this.Format = DateTimePickerFormat.Custom;
            this.ShowCheckBox = false;
            this.ShowUpDown = false;
        }

        [DefaultValue("yyyy-MM-dd")]
        public new string CustomFormat {
            get => base.CustomFormat;
            set => base.CustomFormat = value;
        }

        public string ParamName {
            get => m_paramName;
            set => m_paramName = value;
        }

        public string ControlCheckName {
            get => m_ctrlCheckName;
            set => m_ctrlCheckName = value;
        }

        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        public long SelectedDate {
            get => Utils.EpochTm(this.Value);
            set => this.Value = Utils.FromEpochTm(Math.Min(Utils.EpochTm(this.MaxDate), Math.Max(Utils.EpochTm(this.MinDate), value)));
        }

        public void SetParameter(object value)
        {
            if (value is Vec3i) {
                Vec3i vecVal = (Vec3i)value;
                this.SelectedDate = 
                    LoongCAD.Util.Utils.EpochTm(new DateTime(vecVal.X, vecVal.Y, vecVal.Z));
            } else if(value is long || value is ulong) {
                this.SelectedDate = LoongCAD.Util.Utils.ToLong(value);
            }
        }

        public object GetParameter()
        {
            return this.SelectedDate;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.SelectedDate;
        }

        public void ClearParameter()
        {
            this.SelectedDate = 0;
        }

    }
    
}
