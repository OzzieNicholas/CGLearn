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
    public class ComboBoxText : ComboBox, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBoxText() {
            base.DoubleBuffered = true;
            base.DrawMode = DrawMode.Normal;
            base.ItemHeight = (int)(DPI.ScaleFactor * 15.0);
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
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

        [DefaultValue(typeof(Font), "宋体, 13pt"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new Font Font {
            get => base.Font;
            set => base.Font = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
        }

        [Description("Prop Name"), DefaultValue("")]
        public string PropName {
            get => m_propName;
            set {
                m_propName = value;
            }
        }

        [Description("Check Name"), DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
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

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            base.OnSelectionChangeCommitted(e);

            if (m_checkOnChange && m_checkName != null && m_checkName.Length > 0) {
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_checkName, true);
                if (ctrls != null && ctrls.Length == 1) {
                    if (ctrls[0] is Check)
                        ((Check)ctrls[0]).Checked = true;
                    else if (ctrls[0] is CheckBox)
                        ((CheckBox)ctrls[0]).Checked = true;
                }
            }

        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index >= 0) {
                using (var br = new SolidBrush(e.ForeColor)) {
                    e.Graphics.DrawString((string)this.Items[e.Index], e.Font, br, e.Bounds);
                }
            }
            
        }
    }
    
}
