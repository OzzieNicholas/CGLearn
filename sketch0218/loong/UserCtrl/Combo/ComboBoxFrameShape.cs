using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.STC;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ComboBoxFrameShape : ComboBox, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBoxFrameShape() {
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

        public void AddItem(Define.DRTextFrame shape) {
            this.Items.Add(shape);
        }

        [Description("Selected Shape"), DefaultValue(null)]
        public object SelectedShape {
            get {
                if (this.SelectedItem == null)
                    return Define.DRTextFrame.DRTEXT_FRAME0;

                return (Define.DRTextFrame)this.SelectedItem;
            }

            set {
                if (value is Define.DRTextFrame) {
                    this.SelectedItem = value;
                }
                else if (value is int) {
                    this.SelectedItem = (Define.DRTextFrame)value;
                }
                else if (value is string) {
                    if ((string)value == "frame0")
                        this.SelectedItem = Define.DRTextFrame.DRTEXT_FRAME0;
                    else if ((string)value == "frame1")
                        this.SelectedItem = Define.DRTextFrame.DRTEXT_FRAME1;
                    else if ((string)value == "frame2")
                        this.SelectedItem = Define.DRTextFrame.DRTEXT_FRAME2;
                    else if ((string)value == "frame3")
                        this.SelectedItem = Define.DRTextFrame.DRTEXT_FRAME3;
                    else if ((string)value == "frame4")
                        this.SelectedItem = Define.DRTextFrame.DRTEXT_FRAME4;
                    else
                        this.SelectedItem = null;
                }
                else {
                    this.SelectedItem = null;
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
        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index < 0) {
                e.Graphics.FillRectangle(Brushes.White, e.Bounds);
            }
            else {
                Point v0 = new Point(e.Bounds.X + 2, e.Bounds.Y + e.Bounds.Height / 2);
                Point v1 = new Point(e.Bounds.X + e.Bounds.Width - 2, e.Bounds.Y + e.Bounds.Height / 2);

                StringFormat strFmt = new StringFormat();
                strFmt.Alignment = StringAlignment.Center;
                strFmt.LineAlignment = StringAlignment.Center;

                string textTip = "123";
                Define.DRTextFrame frame = (Define.DRTextFrame)this.Items[e.Index];
                if (frame == Define.DRTextFrame.DRTEXT_FRAME0)
                {
                    e.Graphics.DrawString(textTip, e.Font, Brushes.Black, e.Bounds, strFmt);
                }
                else if (frame == Define.DRTextFrame.DRTEXT_FRAME1)
                {
                    Font font = new Font(e.Font, FontStyle.Underline);
                    e.Graphics.DrawString(textTip, font, Brushes.Black, e.Bounds, strFmt);
                    font.Dispose();
                }
                else if (frame == Define.DRTextFrame.DRTEXT_FRAME2)
                {
                    e.Graphics.DrawString(textTip, e.Font, Brushes.Black, e.Bounds, strFmt);

                    Rectangle rect = new Rectangle(e.Bounds.Left + 15, e.Bounds.Top + 1, e.Bounds.Width - 30, e.Bounds.Height - 2);
                    e.Graphics.DrawRectangle(Pens.Black, rect);
                }
                else if (frame == Define.DRTextFrame.DRTEXT_FRAME3)
                {
                    e.Graphics.DrawString(textTip, e.Font, Brushes.Black, e.Bounds, strFmt);

                    Rectangle rect = new Rectangle(e.Bounds.Left + 15, e.Bounds.Top + 1, e.Bounds.Width - 30, e.Bounds.Height - 2);
                    int arcR = rect.Height / 2;
                    Point arcOL = new Point(rect.Left + arcR, rect.Top + rect.Height / 2);
                    Point arcOR = new Point(rect.Right - arcR, rect.Top + rect.Height / 2);

                    e.Graphics.DrawLine(Pens.Black, rect.Left + rect.Height * 0.5f, rect.Top, rect.Right - rect.Height * 0.5f, rect.Top);
                    e.Graphics.DrawLine(Pens.Black, rect.Left + rect.Height * 0.5f, rect.Bottom, rect.Right - rect.Height * 0.5f, rect.Bottom);
                    e.Graphics.DrawArc(Pens.Black, new Rectangle(arcOL.X - arcR, arcOL.Y - arcR, arcR * 2, arcR * 2), 90.0f, 180.0f);
                    e.Graphics.DrawArc(Pens.Black, new Rectangle(arcOR.X - arcR, arcOR.Y - arcR, arcR * 2, arcR * 2), 90.0f, -180.0f);
                }
                else if (frame == Define.DRTextFrame.DRTEXT_FRAME4)
                {
                    e.Graphics.DrawString(textTip, e.Font, Brushes.Black, e.Bounds, strFmt);

                    Rectangle rect = new Rectangle(e.Bounds.Left + 15, e.Bounds.Top + 1, e.Bounds.Width - 30, e.Bounds.Height - 2);
                    int arrowL = rect.Height / 2;

                    e.Graphics.DrawLine(Pens.Black, rect.Left + rect.Height * 0.5f, rect.Top, rect.Right - rect.Height * 0.5f, rect.Top);
                    e.Graphics.DrawLine(Pens.Black, rect.Left + rect.Height * 0.5f, rect.Bottom, rect.Right - rect.Height * 0.5f, rect.Bottom);
                    e.Graphics.DrawLine(Pens.Black, rect.Left, rect.Top + rect.Height * 0.5f, rect.Left + arrowL, rect.Top);
                    e.Graphics.DrawLine(Pens.Black, rect.Left, rect.Top + rect.Height * 0.5f, rect.Left + arrowL, rect.Bottom);
                    e.Graphics.DrawLine(Pens.Black, rect.Right, rect.Top + rect.Height * 0.5f, rect.Right - arrowL, rect.Top);
                    e.Graphics.DrawLine(Pens.Black, rect.Right, rect.Top + rect.Height * 0.5f, rect.Right - arrowL, rect.Bottom);
                }
                else if(frame == Define.DRTextFrame.DRTEXT_FRAME5)
                {
                    Rectangle curBounds = new Rectangle(e.Bounds.Location, e.Bounds.Size);
                    curBounds.Y += 2;
                    e.Graphics.DrawString("1", e.Font, Brushes.Black, curBounds, strFmt);
                    
                    Point oriPos = new Point((e.Bounds.Left + e.Bounds.Right) / 2,
                        (e.Bounds.Top + e.Bounds.Bottom) / 2);
                    int r = e.Bounds.Height / 2 - 1;
                    Rectangle rect = new Rectangle(oriPos.X - r - 1, oriPos.Y - r, e.Bounds.Height - 2, e.Bounds.Height - 2);
                    
                    e.Graphics.DrawEllipse(Pens.Black, rect);
                }
                else if (frame == Define.DRTextFrame.DRTEXT_FRAME6)
                {
                    Rectangle curBounds = new Rectangle(e.Bounds.Location, e.Bounds.Size);
                    curBounds.Y += 2;
                    e.Graphics.DrawString("1", e.Font, Brushes.Black, curBounds, strFmt);
                    int triSize = curBounds.Height;
                    curBounds.Y -= 3;
                    //
                    Point midPos = new Point((curBounds.Left + curBounds.Right) / 2, curBounds.Bottom);

                    Point pt1 = new Point(midPos.X - triSize / 2, midPos.Y);
                    Point pt2 = new Point(midPos.X + triSize / 2, midPos.Y);
                    e.Graphics.DrawLine(Pens.Black, pt1, pt2);

                    pt1.X = midPos.X;
                    pt1.Y = midPos.Y - triSize;
                    e.Graphics.DrawLine(Pens.Black, pt1, pt2);

                    pt2.X -= triSize;
                    e.Graphics.DrawLine(Pens.Black, pt1, pt2);
                }
            }
        }
    }
    
}
