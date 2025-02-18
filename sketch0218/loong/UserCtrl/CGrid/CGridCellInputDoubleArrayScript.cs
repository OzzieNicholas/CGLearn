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
    public class CGridCellInputDoubleArrayScript : CGridCell
    {
        public CGridCellInputDoubleArrayScript(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_DOUBLE_ARRAY_SCRIPT;

            var tb = new InputDoubleArrayScript();
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
            get => (m_ctrl as InputDoubleArrayScript).Value;
            set {
                var tb = m_ctrl as InputDoubleArrayScript;
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
                (m_ctrl as InputDoubleArrayScript).ReadOnly = value;
                (m_ctrl as InputDoubleArrayScript).caretVisible = !value;
            }
        }

        public void updateUnitEnob(DObject opts) {
            (m_ctrl as InputDoubleArrayScript).updateDispUnitEnob(opts);
        }

        public string unitData {
            get => (m_ctrl as InputDoubleArrayScript).unitData;
            set {
                (m_ctrl as InputDoubleArrayScript).unitData = value;
            }
        }

        public string unitDisp {
            get => (m_ctrl as InputDoubleArrayScript).unitDisp;
            set {
                (m_ctrl as InputDoubleArrayScript).unitDisp = value;
            }
        }

        public string unitEnobType {
            get => (m_ctrl as InputDoubleArrayScript).unitEnobType;
            set {
                (m_ctrl as InputDoubleArrayScript).unitEnobType = value;
            }
        }

    }

    
}
