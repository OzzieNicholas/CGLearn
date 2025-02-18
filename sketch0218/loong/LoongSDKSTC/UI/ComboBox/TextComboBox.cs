using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class TextComboBox : System.Windows.Forms.ComboBox, IParamCtrl, IPlaceholderCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private string m_placeholder = "";
        private bool m_checkOnChange = true;

        public TextComboBox() {
            base.DoubleBuffered = true;
            base.DrawMode = DrawMode.Normal;
            base.ItemHeight = (int)(DPI.ScaleFactor * 15.0);
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.Font = new Font(this.Font.FontFamily, 13F * DPI.ScaleFactor, this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [DefaultValue(ComboBoxStyle.DropDownList)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            set => base.DropDownStyle = value;
        }

        [DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.DrawMode = value;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set => base.Font = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
        }

        [DefaultValue("")]
        public string ParamName {
            get => m_paramName;
            set {
                m_paramName = value;
            }
        }

        [DefaultValue("")]
        public string ControlCheckName {
            get => m_ctrlCheckName;
            set => m_ctrlCheckName = value;
        }

        [Description("Check On Change"), DefaultValue(true)]
        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        public object Placeholder
        {
            get => m_placeholder ;
            set => m_placeholder = value != null ? value.ToString() : null;
        }

        public void SetParameter(object value)
        {
            this.SelectedItem = value != null ? value.ToString() : null;
        }

        public object GetParameter()
        {
            return this.SelectedItem;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.SelectedItem;
        }

        public void ClearParameter()
        {
            this.SelectedItem = null;
        }

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            base.OnSelectionChangeCommitted(e);

            if (m_checkOnChange && m_ctrlCheckName != null && m_ctrlCheckName.Length > 0 && this.TopLevelControl != null) {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_ctrlCheckName, true);
                if (ctrls != null && ctrls.Length == 1) {
                    if (ctrls[0] is CheckBox) {
                        CheckBox chb = ctrls[0] as CheckBox;
                        if (!chb.Checked)
                            chb.Checked = true;
                    }
                }
            }
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index >= 0) {
                using (var br = new SolidBrush(e.ForeColor)) {
                    if (string.IsNullOrEmpty(m_placeholder)) {
                        e.Graphics.DrawString((string)this.Items[e.Index], e.Font, br, e.Bounds);
                    } else {
                        e.Graphics.DrawString(
                            (string)this.Items[e.Index] + "(" + m_placeholder + ")", e.Font, br, e.Bounds);
                    }
                }
            }
        }

        public void ClearPlaceholder()
        {
            m_placeholder = "";
        }

    }
    
}
