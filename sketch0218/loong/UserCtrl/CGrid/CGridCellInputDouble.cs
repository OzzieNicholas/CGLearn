using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;

namespace UserCtrl
{
    public class CGridCellInputDouble : CGridCell
    {
        public CGridCellInputDouble(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_DOUBLE;

            var tb = new InputDouble();
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
            get => (m_ctrl as InputDouble).Value;
            set {
                (m_ctrl as InputDouble).Value = (double)value;
            }
        }

        public double defValue {
            get => (m_ctrl as InputDouble).DefValue;
            set {
                (m_ctrl as InputDouble).DefValue = value;
            }
        }

        public void updateUnitEnob(DObject opts) {
            (m_ctrl as InputDouble).updateDispUnitEnob(opts);
        }

        public string unitData {
            get => (m_ctrl as InputDouble).unitData;
            set {
                (m_ctrl as InputDouble).unitData = value;
            }
        }

        public string unitDisp {
            get => (m_ctrl as InputDouble).unitDisp;
            set {
                (m_ctrl as InputDouble).unitDisp = value;
            }
        }

        public string unitEnobType {
            get => (m_ctrl as InputDouble).unitEnobType;
            set {
                (m_ctrl as InputDouble).unitEnobType = value;
            }
        }

        public int enob {
            get => (m_ctrl as InputDouble).Enob;
            set {
                (m_ctrl as InputDouble).Enob = value;
            }
        }

        public bool defAutoClear {
            get => (m_ctrl as InputDouble).DefAutoClear;
            set {
                (m_ctrl as InputDouble).DefAutoClear = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as InputDouble).ReadOnly = value;
                (m_ctrl as InputDouble).caretVisible = !value;
            }
        }

        public double min {
            get => (m_ctrl as InputDouble).Min;
            set {
                (m_ctrl as InputDouble).Min = value;
            }
        }

        public double max {
            get => (m_ctrl as InputDouble).Max;
            set {
                (m_ctrl as InputDouble).Max = value;
            }
        }
    }

    
}
