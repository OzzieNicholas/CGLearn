using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Geometry;

namespace UserCtrl
{
    public class CGridCellInputText : CGridCell
    {
        public CGridCellInputText(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_TEXT;

            var tb = new InputText();
            tb.Dock = DockStyle.Fill;
            tb.BorderStyle = BorderStyle.None;
            tb.Height = (int)tb.Font.Size + 2;
            tb.BackColor = Color.White;
            tb.BackColor = Color.White;

            m_ctrl = tb;
            m_ctrl.GotFocus += _onCellGotFocus;
            m_ctrl.LostFocus += _onCellLostFocus;
            m_ctrl.Validated += _onCellValidated;
        }

        public override object value {
            get => (m_ctrl as InputText).Value;
            set {
                string s = "";
                if (value != null) {
                    if (value is Vec2i) {
                        var vec = (Vec2i)value;
                        s = "" + vec.X + ',' + vec.Y;
                    }
                    else if (value is Vec3i) {
                        var vec = (Vec3i)value;
                        s = "" + vec.X + ',' + vec.Y + ',' + vec.Z;
                    }
                    else if (value is Vec4i) {
                        var vec = (Vec4i)value;
                        s = "" + vec.X + ',' + vec.Y + ',' + vec.Z + ',' + vec.W;
                    }
                    else if (value is Vec2) {
                        var vec = (Vec2)value;
                        s = "" + vec.X + ',' + vec.Y;
                    }
                    else if (value is Vec3) {
                        var vec = (Vec3)value;
                        s = "" + vec.X + ',' + vec.Y + ',' + vec.Z;
                    }
                    else if (value is Vec4) {
                        var vec = (Vec4)value;
                        s = "" + vec.X + ',' + vec.Y + ',' + vec.Z + ',' + vec.W;
                    }
                    else
                        s = "" + value;
                }

                (m_ctrl as InputText).Value = s;
            }
        }

        public override bool readOnly {
            get => base.readOnly;

            set {
                m_readOnly = value;
                (m_ctrl as InputText).ReadOnly = value;
                (m_ctrl as InputText).caretVisible = !value;
            }
        }

    }

    
}
