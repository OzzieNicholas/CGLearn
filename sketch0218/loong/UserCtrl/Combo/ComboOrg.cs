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

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ComboOrg : ComboBox, IUserCtrl {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboOrg() {
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

        [DefaultValue(typeof(Font), "宋体, 13pt"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
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
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
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
