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

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ComboBoxDate : DateTimePicker, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBoxDate() {
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


        [Description("Prop Name")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [Description("Check Name")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [Description("Check On Change")]
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


    }
    
}
