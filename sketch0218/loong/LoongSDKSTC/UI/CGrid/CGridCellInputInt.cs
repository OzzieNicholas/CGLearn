using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    class CGridCellInputInt : CGridCell
    {
        public CGridCellInputInt(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_INT;

            var tb = new IntInput();
            tb.Dock = DockStyle.Fill;
            tb.BorderStyle = BorderStyle.None;
            tb.Height = (int)tb.Font.Size + 2;
            tb.BackColor = Color.White;

            m_ctrl = tb;
            m_ctrl.GotFocus += _onCellGotFocus;
            m_ctrl.LostFocus += _onCellLostFocus;
            m_ctrl.Validated += _onCellValidated;
        }

        public override object value {
            get => (m_ctrl as IntInput).Value;
            set {
                (m_ctrl as IntInput).Value = (int)value;
            }
        }

        public int DefaultValue {
            get => (m_ctrl as IntInput).DefaultValue;
            set {
                (m_ctrl as IntInput).DefaultValue = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;

            set {
                m_readOnly = value;
                (m_ctrl as IntInput).ReadOnly = value;
                (m_ctrl as IntInput).CaretVisible = !value;
            }
        }

        public int Min {
            get => (m_ctrl as IntInput).Min;
            set {
                (m_ctrl as IntInput).Min = value;
            }
        }

        public int Max {
            get => (m_ctrl as IntInput).Max;
            set {
                (m_ctrl as IntInput).Max = value;
            }
        }
    }
}
