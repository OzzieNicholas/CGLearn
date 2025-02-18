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
    public class ComboBoxStampLine : ComboBox, IUserCtrl
    {
        private static Dictionary<int, Pen> m_pens = new Dictionary<int, Pen>();
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        static ComboBoxStampLine() {
            m_pens[(int)Define.DRLineType.DRLINE_TYPE0] = new Pen(Color.Black);
            m_pens[(int)Define.DRLineType.DRLINE_TYPE0].Width = 2.0f;

            m_pens[(int)Define.DRLineType.DRLINE_TYPE1] = new Pen(Color.Black);
            m_pens[(int)Define.DRLineType.DRLINE_TYPE1].DashPattern = new float[] { 4.0f, 2.0f };
            m_pens[(int)Define.DRLineType.DRLINE_TYPE1].Width = 2.0f;

            m_pens[(int)Define.DRLineType.DRLINE_TYPE2] = new Pen(Color.Black);
            m_pens[(int)Define.DRLineType.DRLINE_TYPE2].DashPattern = new float[] { 8.0f, 4.0f };
            m_pens[(int)Define.DRLineType.DRLINE_TYPE2].Width = 2.0f;

            m_pens[(int)Define.DRLineType.DRLINE_TYPE3] = new Pen(Color.Black);
            m_pens[(int)Define.DRLineType.DRLINE_TYPE3].DashPattern = new float[] { 8.0f, 2.0f, 4.0f, 2.0f };
            m_pens[(int)Define.DRLineType.DRLINE_TYPE3].Width = 2.0f;

            m_pens[(int)Define.DRLineType.DRLINE_TYPE4] = new Pen(Color.Black);
            m_pens[(int)Define.DRLineType.DRLINE_TYPE4].DashPattern = new float[] { 2.0f, 1.0f, 2.0f, 1.0f };
            m_pens[(int)Define.DRLineType.DRLINE_TYPE4].Width = 2.0f;
        }

        public ComboBoxStampLine()
        {
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

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
        }

        [Description("Prop Name")]
        public string PropName
        {
            get => m_propName;
            set => m_propName = value;
        }

        [Description("Check Name")]
        public string CheckName
        {
            get => m_checkName;
            set => m_checkName = value;
        }

        [Description("Check On Change")]
        public bool CheckOnChange
        {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        protected override void OnSelectionChangeCommitted(EventArgs e)
        {
            base.OnSelectionChangeCommitted(e);

            if (m_checkOnChange && m_checkName != null && m_checkName.Length > 0 && this.TopLevelControl != null)
            {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_checkName, true);
                if (ctrls != null && ctrls.Length == 1)
                {
                    if (ctrls[0] is CheckBox)
                    {
                        CheckBox chb = ctrls[0] as CheckBox;
                        if (!chb.Checked)
                            chb.Checked = true;
                    }
                }
            }

        }

        protected override void OnDrawItem(DrawItemEventArgs e)
        {
            e.DrawBackground();
            e.DrawFocusRectangle();

            if (e.Index < 0)
            {
                e.Graphics.FillRectangle(Brushes.White, e.Bounds);
            }
            else
            {
                int type = (int)this.Items[e.Index];
                if (m_pens.ContainsKey(type))
                {
                    e.Graphics.DrawLine(m_pens[type], e.Bounds.X + 2, e.Bounds.Y + e.Bounds.Height / 2,
                                        e.Bounds.X + e.Bounds.Width - 2, e.Bounds.Y + e.Bounds.Height / 2);
                }
            }
            
            base.OnDrawItem(e);
        }
    }
    
}
