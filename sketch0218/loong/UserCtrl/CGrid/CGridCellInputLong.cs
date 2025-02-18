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
    public class CGridCellInputLong : CGridCell
    {
        public CGridCellInputLong(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_LONG;

            var tb = new InputLong();
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
            get => (m_ctrl as InputLong).Value;
            set {
                (m_ctrl as InputLong).Value = (long)value;
            }
        }

        public long defValue {
            get => (m_ctrl as InputLong).DefValue;
            set {
                (m_ctrl as InputLong).DefValue = value;
            }
        }

        public bool defAutoClear {
            get => (m_ctrl as InputLong).DefAutoClear;
            set {
                (m_ctrl as InputLong).DefAutoClear = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;

            set {
                m_readOnly = value;
                (m_ctrl as InputLong).ReadOnly = value;
                (m_ctrl as InputLong).caretVisible = !value;
            }
        }

        public long min {
            get => (m_ctrl as InputLong).Min;
            set {
                (m_ctrl as InputLong).Min = value;
            }
        }

        public long max {
            get => (m_ctrl as InputLong).Max;
            set {
                (m_ctrl as InputLong).Max = value;
            }
        }
    }

    
}
