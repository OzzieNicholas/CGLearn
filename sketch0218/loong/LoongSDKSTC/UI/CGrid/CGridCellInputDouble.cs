using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    class CGridCellInputDouble : CGridCell
    {
        public CGridCellInputDouble(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_DOUBLE;

            var tb = new DoubleInput();
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
            get => (m_ctrl as DoubleInput).Value;
            set {
                (m_ctrl as DoubleInput).Value = (double)value;
            }
        }

        public double DefaultValue {
            get => (m_ctrl as DoubleInput).DefaultValue;
            set {
                (m_ctrl as DoubleInput).DefaultValue = value;
            }
        }

        public void UpdateDispUnitEnob(DObject opts) {
            (m_ctrl as DoubleInput).UpdateDispUnitEnob(opts);
        }

        public string UnitData {
            get => (m_ctrl as DoubleInput).UnitData;
            set {
                (m_ctrl as DoubleInput).UnitData = value;
            }
        }

        public string UnitDisplay {
            get => (m_ctrl as DoubleInput).UnitDisplay;
            set {
                (m_ctrl as DoubleInput).UnitDisplay = value;
            }
        }

        public string UnitEnobType {
            get => (m_ctrl as DoubleInput).UnitEnobType;
            set {
                (m_ctrl as DoubleInput).UnitEnobType = value;
            }
        }

        public int Enob {
            get => (m_ctrl as DoubleInput).Enob;
            set {
                (m_ctrl as DoubleInput).Enob = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as DoubleInput).ReadOnly = value;
                (m_ctrl as DoubleInput).CaretVisible = !value;
            }
        }

        public double Min {
            get => (m_ctrl as DoubleInput).Min;
            set {
                (m_ctrl as DoubleInput).Min = value;
            }
        }

        public double Max {
            get => (m_ctrl as DoubleInput).Max;
            set {
                (m_ctrl as DoubleInput).Max = value;
            }
        }
    }

    
}
