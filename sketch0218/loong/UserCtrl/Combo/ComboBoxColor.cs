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
    public class ComboBoxColor : ComboBox, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBoxColor() {
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)(15.0 * DPI.ScaleFactor);
            base.Font = new Font(this.Font.FontFamily, 13F * DPI.ScaleFactor, this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
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

        [Description("Prop Name"), DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
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
#if false
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
#else
            this.AddItem(0x000000); // (0, 0, 0)
            this.AddItem(0xFF0000); // (255, 0, 0)
            this.AddItem(0x00FF00); // (0, 255, 0)
            this.AddItem(0x0000FF); // (0, 0, 255)
            this.AddItem(0x00FFFF); // (0, 255, 255)
            this.AddItem(0xFFFF00); // (255, 255, 0)
            this.AddItem(0xFF00FF); // (255, 0, 255)
            this.AddItem(0x804040); // (128, 64, 64)
            this.AddItem(0x00A000); // (0, 160, 0)
            this.AddItem(0x333399); // (51, 51, 153)
            this.AddItem(0x008080); // (0, 128, 128)
            this.AddItem(0xFF6600); // (255, 102, 0)
            this.AddItem(0x969696); // (150, 150, 150)
#endif
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
            } else { // 白色需绘制成灰色并添加边框线
                using (var br = new SolidBrush(Color.FromArgb(0xE6, 0xE6, 0xE6))) {
                    e.Graphics.FillRectangle(br, e.Bounds.Left + 2, e.Bounds.Top + 2, e.Bounds.Width - 4, e.Bounds.Height - 4);
                }

                // 绘制边框虚线
                using (var pen = new Pen(Color.Black, 2.0f)) {
                    pen.DashPattern = new float[]{1, 1};
                    e.Graphics.DrawRectangle(pen, e.Bounds.Left + 2, e.Bounds.Top + 2, e.Bounds.Width - 4, e.Bounds.Height - 4);
                }
            }
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
    }
    
}
