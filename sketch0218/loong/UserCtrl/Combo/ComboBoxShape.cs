using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{

    [ToolboxItem(true)]
    public class ComboBoxShape : ComboBox, IUserCtrl
    {

        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;
        private List<GraphicsPath> m_shapeList = new List<GraphicsPath>();

        public ComboBoxShape() {
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)(15.0 * DPI.ScaleFactor);
            base.Font = new Font(this.Font.FontFamily, 13F * DPI.ScaleFactor, this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [DefaultValue(ComboBoxStyle.DropDownList)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            //set => base.DropDownStyle = value;
        }

        [DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [Browsable(false), DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.DrawMode = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
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

        public void ClearItems() {
            this.Items.Clear();
            m_shapeList.Clear();
        }

        public void AddItem(int shapeKey, GraphicsPath shapeGraph) {
            if (shapeGraph == null) {
                return;
            }
            this.Items.Add(shapeKey);
            m_shapeList.Add(shapeGraph);
        }

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            base.OnSelectionChangeCommitted(e);

            if (m_checkOnChange && m_checkName != null && m_checkName.Length > 0 && this.TopLevelControl != null) {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_checkName, true);
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
            e.DrawFocusRectangle();

            if (e.Index < 0 || e.Index >= this.Items.Count)
                e.Graphics.FillRectangle(Brushes.White, e.Bounds);
            else
                e.Graphics.DrawPath(Pens.Black, m_shapeList[e.Index]);
        }

    }
}
