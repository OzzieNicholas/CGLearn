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
    public class ComboBoxArrowShape : ComboBox, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;

        public ComboBoxArrowShape() {
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

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(true)]
        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        public void ClearItems() {
            this.Items.Clear();
        }

        public void AddItem(Define.DRArrowShape shape) {
            this.Items.Add(shape);
        }

        public object SelectedShape {
            get {
                if (this.SelectedItem == null)
                    return Define.DRArrowShape.DRARROW_NONE;

                return (Define.DRArrowShape)this.SelectedItem;
            }

            set {
                if (value is Define.DRArrowShape) {
                    this.SelectedItem = value;
                }
                else if (value is int) {
                    this.SelectedItem = (Define.DRArrowShape)value;
                }
                else if (value is string) {
                    if ((string)value == "none")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_NONE;
                    else if ((string)value == "slash")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_SLASH;
                    else if ((string)value == "arrow")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_ARROW;
                    else if ((string)value == "solid_arrow")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_SOLID_ARROW;
                    else if ((string)value == "sphere")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_SPHERE;
                    else if ((string)value == "solid_sphere")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_SOLID_SPHERE;
                    else if ((string)value == "x")
                        this.SelectedItem = Define.DRArrowShape.DRARROW_X;
                    else
                        this.SelectedItem = null;
                }
                else {
                    this.SelectedItem = null;
                }
            }
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index < 0) {
                e.Graphics.FillRectangle(Brushes.White, e.Bounds);
            }
            else {
                Point v0 = new Point(e.Bounds.X + 10, e.Bounds.Y + e.Bounds.Height / 2);
                Point v1 = new Point(e.Bounds.X + Math.Min(e.Bounds.Width - 10, 100), e.Bounds.Y + e.Bounds.Height / 2);

                Define.DRArrowShape shape = (Define.DRArrowShape)this.Items[e.Index];
                //DRAW_ARROW_NONE = 0, // -
                //DRAW_ARROW_SLASH, // -/
                //DRAW_ARROW_ARROW, // ->|
                //DRAW_ARROW_SOLID_ARROW, // -|>
                //DRAW_ARROW_SPHERE, // -o
                //DRAW_ARROW_SOLID_SPHERE, // -(*)
                //DRAW_ARROW_X, // -X

                if (shape == Define.DRArrowShape.DRARROW_NONE) {
                    e.Graphics.DrawLine(Pens.Black, v0, v1);
                }
                else if (shape == Define.DRArrowShape.DRARROW_SLASH) {
                    v1 = new Point(v1.X - 5, v1.Y);
                    e.Graphics.DrawLine(Pens.Black, v0, v1);
                    e.Graphics.DrawLine(Pens.Black, new Point(v1.X - 5, v1.Y + 5), new Point(v1.X + 5, v1.Y - 5));
                }
                else if (shape == Define.DRArrowShape.DRARROW_ARROW) {
                    e.Graphics.DrawLine(Pens.Black, v0, v1);
                    e.Graphics.DrawLine(Pens.Black, new Point(v1.X - 10, v1.Y - 5), v1);
                    e.Graphics.DrawLine(Pens.Black, new Point(v1.X - 10, v1.Y + 5), v1);
                }
                else if (shape == Define.DRArrowShape.DRARROW_SOLID_ARROW) {
                    e.Graphics.DrawLine(Pens.Black, v0, v1);
                    e.Graphics.FillPolygon(Brushes.Black, new Point[] { new Point(v1.X, v1.Y + 1), new Point(v1.X, v1.Y - 1), new Point(v1.X - 10, v1.Y - 5), new Point(v1.X - 10, v1.Y + 5) });
                }
                else if (shape == Define.DRArrowShape.DRARROW_SPHERE) {
                    int r = 5;
                    v1 = new Point(v1.X - r, v1.Y);
                    e.Graphics.DrawLine(Pens.Black, v0, new Point(v1.X - r, v1.Y));
                    e.Graphics.DrawEllipse(Pens.Black, v1.X - r, v1.Y - r, 2 * r, 2 * r);
                }
                else if (shape == Define.DRArrowShape.DRARROW_SOLID_SPHERE) {
                    int r = 5;
                    v1 = new Point(v1.X - r, v1.Y);
                    e.Graphics.DrawLine(Pens.Black, v0, new Point(v1.X - r, v1.Y));
                    e.Graphics.FillEllipse(Brushes.Black, v1.X - r, v1.Y - r, 2 * r, 2 * r);
                }
                else if (shape == Define.DRArrowShape.DRARROW_X) {
                    v1 = new Point(v1.X - 5, v1.Y);
                    e.Graphics.DrawLine(Pens.Black, v0, v1);
                    e.Graphics.DrawLine(Pens.Black, new Point(v1.X - 5, v1.Y + 5), new Point(v1.X + 5, v1.Y - 5));
                    e.Graphics.DrawLine(Pens.Black, new Point(v1.X - 5, v1.Y - 5), new Point(v1.X + 5, v1.Y + 5));
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
