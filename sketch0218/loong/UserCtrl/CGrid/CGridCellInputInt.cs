using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class CGridCellInputInt : CGridCell
    {
        public CGridCellInputInt(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_INT;

            var tb = new InputInt();
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
            get => (m_ctrl as InputInt).Value;
            set {
                (m_ctrl as InputInt).Value = (int)value;
            }
        }

        public int defValue {
            get => (m_ctrl as InputInt).DefValue;
            set {
                (m_ctrl as InputInt).DefValue = value;
            }
        }

        public bool defAutoClear {
            get => (m_ctrl as InputInt).DefAutoClear;
            set {
                (m_ctrl as InputInt).DefAutoClear = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;

            set {
                m_readOnly = value;
                (m_ctrl as InputInt).ReadOnly = value;
                (m_ctrl as InputInt).caretVisible = !value;
            }
        }

        public int min {
            get => (m_ctrl as InputInt).Min;
            set {
                (m_ctrl as InputInt).Min = value;
            }
        }

        public int max {
            get => (m_ctrl as InputInt).Max;
            set {
                (m_ctrl as InputInt).Max = value;
            }
        }
    }
}
