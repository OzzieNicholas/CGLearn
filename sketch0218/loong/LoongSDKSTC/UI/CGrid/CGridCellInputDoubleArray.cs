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
    class CGridCellInputDoubleArray : CGridCell
    {
        public CGridCellInputDoubleArray(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_DOUBLE_ARRAY;

            var tb = new DoubleArrayInput();
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
            get => (m_ctrl as DoubleArrayInput).Value;
            set {
                var tb = m_ctrl as DoubleArrayInput;
                if (value is double[])
                    tb.Value = value as double[];
                else if (value is double) 
                    tb.Value = new double[] { (double)value };
            }
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as DoubleArrayInput).ReadOnly = value;
                (m_ctrl as DoubleArrayInput).CaretVisible = !value;
            }
        }

        public void UpdateUnitEnob(DObject opts) {
            (m_ctrl as DoubleArrayInput).UpdateDispUnitEnob(opts);
        }

        public string UnitData {
            get => (m_ctrl as DoubleArrayInput).UnitData;
            set => (m_ctrl as DoubleArrayInput).UnitData = value;
        }

        public string UnitDisplay {
            get => (m_ctrl as DoubleArrayInput).UnitDisplay;
            set => (m_ctrl as DoubleArrayInput).UnitDisplay = value;
        }

        public string UnitEnobType {
            get => (m_ctrl as DoubleArrayInput).UnitEnobType;
            set => (m_ctrl as DoubleArrayInput).UnitEnobType = value;
        }

    }

    
}
