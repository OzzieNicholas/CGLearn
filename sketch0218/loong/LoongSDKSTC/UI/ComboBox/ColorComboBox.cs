
using System;
using System.Drawing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class ColorComboBox : System.Windows.Forms.ComboBox, IParamCtrl, ITabOrderCtrl
    {
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ColorComboBox() {
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)DPI.Scale(15.0f);
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [Browsable(false), DefaultValue(ComboBoxStyle.DropDownList)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            //set => base.DropDownStyle = value;
        }

        [Browsable(false), DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [Browsable(false), DefaultValue(FlatStyle.System)]
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
            set => base.ItemHeight = value;
        }

        [Browsable(false)]
        public new ObjectCollection Items {
            get => base.Items;
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

        public void ClearItems() {
            this.Items.Clear();
        }

        public void AddItem(uint color) {
            this.Items.Add(color);
        }

        public void InitDefaultColors() {
            this.ClearItems();
            this.AddItem(0x000000);
            this.AddItem(0xF0F0F0);
            this.AddItem(0xFF0000);
            this.AddItem(0x00FF00);
            this.AddItem(0x0000FF);
            this.AddItem(0x00FFFF);
            this.AddItem(0xF0F000);
            this.AddItem(0xFF00FF);
            this.AddItem(0x804040);
            this.AddItem(0x00A000);
            this.AddItem(0x333399);
            this.AddItem(0x008080);
            this.AddItem(0xff6600);
            this.AddItem(0x969696);
        }

        public void InitDrawingColors(bool bWithWhite = true) {
            this.ClearItems();
            if (bWithWhite) {
                this.AddItem(0xFFFFFF);
            }
            this.AddItem(0x000000);
            this.AddItem(0xFF0000);
            this.AddItem(0x00E700);
            this.AddItem(0x0000FF);
            this.AddItem(0x00FFFF);
            this.AddItem(0x808000);
            this.AddItem(0xFF00FF);
            this.AddItem(0x804040);
            this.AddItem(0x008000);
            this.AddItem(0x333399);
            this.AddItem(0x008080);
            this.AddItem(0xFF6600);
            this.AddItem(0x969696);
        }

        [Description("Selected Color"), DefaultValue(0u)]
        public uint SelectedColor {
            get {
                if (this.SelectedItem == null)
                    return 0;

                return (uint)this.SelectedItem;
            }

            set {
                this.SelectedItem = value;
            }
        }

        public string SelectedStrColor {
            set {
                if (value == null) {
                    this.SelectedItem = 0u;
                    return;
                }

                string[] colorStr = value.Trim().Split(',');
                uint color = 0u;
                if (colorStr.Length == 3) {
                    for (int i = 0; i < 3; ++i) {
                        uint v = 0;
                        if (UInt32.TryParse(colorStr[2 - i], out v)) {
                            color |= (v << (8 * i));
                        }
                    }
                }

                this.SelectedItem = color;
            }
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            uint color = e.Index < 0 ? 0xFF0000 : (uint)this.Items[e.Index];
            if (color != 0xFFFFFF) {
                using (var br = new SolidBrush(Color.FromArgb((int)(color & 0xFF0000) >> 16, (int)(color & 0xFF00) >> 8, (int)color & 0xFF))) {
                    e.Graphics.FillRectangle(br, e.Bounds.Left + 2, e.Bounds.Top + 2, e.Bounds.Width - 4, e.Bounds.Height - 4);
                }
            }
            else { // 白色需绘制成灰色并添加边框线
                using (var br = new SolidBrush(Color.FromArgb(0xE6, 0xE6, 0xE6))) {
                    e.Graphics.FillRectangle(br, e.Bounds.Left + 2, e.Bounds.Top + 2, e.Bounds.Width - 4, e.Bounds.Height - 4);
                }

                // 绘制边框虚线
                using (var pen = new Pen(Color.Black, 2.0f)) {
                    float[] dashPatterns = { 1, 1 };
                    pen.DashPattern = dashPatterns;

                    e.Graphics.DrawRectangle(pen,
                        e.Bounds.Left + 2, e.Bounds.Top + 2, e.Bounds.Width - 4, e.Bounds.Height - 4);
                }
            }
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

        public void SetParameter(object value)
        {
            if (value is uint) {
                this.SelectedColor = (uint)value;
            }
            else if (value is int) {
                this.SelectedColor = (uint)(int)value;
            }
            else if(value is string) {
                this.SelectedStrColor = (string)value;
            }
            else if (value is LoongCAD.Geometry.Vec3) {
                var vecVal = (LoongCAD.Geometry.Vec3)value;
                uint newColor = ((uint)(vecVal.X * 255) << 16)
                                        + ((uint)(vecVal.Y * 255) << 8)
                                        + (uint)(vecVal.Z * 255);
                this.SelectedColor = newColor;
            }
        }

        public object GetParameter()
        {
            return this.SelectedColor;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.SelectedColor;
        }

        public void ClearParameter()
        {
            this.SelectedColor = 0;
        }

    }
}
