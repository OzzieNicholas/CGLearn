
using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;
using LoongCAD.Util;
using System.Collections.Generic;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class ComboBox : System.Windows.Forms.ComboBox, IParamCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBox() {
            base.DoubleBuffered = true;
            base.DrawMode = DrawMode.Normal;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)(15.0f * DPI.ScaleFactor);
            base.Font = new Font(this.Font.FontFamily, 13F * DPI.ScaleFactor, this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [DefaultValue(ComboBoxStyle.DropDown)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            set => base.DropDownStyle = value;
        }

        [DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            set => base.DrawMode = value;
        }

        [DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            set => base.FlatStyle = value;
        }

        [DefaultValue(typeof(Font), "ËÎÌו, 13pt")]
        public new Font Font {
            get => base.Font;
            set {
            }
        }

        [DefaultValue(15), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
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

        [DefaultValue(true)]
        public bool CheckOnChange {
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
            if (this.Items.Contains(value)) {
                this.SelectedItem = value;
            }
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

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            var key = (Keys)e.KeyChar;
            if (key == Keys.Enter
                || key == Keys.Escape
                || key == Keys.Tab) //prevent beep sound
                e.Handled = true;
        }

    }
}
