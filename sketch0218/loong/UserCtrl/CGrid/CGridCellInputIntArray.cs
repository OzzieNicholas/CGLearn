using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Geometry;
using LoongCAD.Util;

namespace UserCtrl
{
    public class CGridCellInputIntArray : CGridCell
    {
        public CGridCellInputIntArray(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_INT_ARRAY;

            var tb = new InputIntArrayScript();
            tb.AutoMerge = false;
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
            get => (m_ctrl as InputIntArrayScript).Value;
            set {
                var tb = m_ctrl as InputIntArrayScript;
                if (value is int[])
                    tb.Value = value as int[];
                else if (value is int)
                    tb.Value = new int[] { (int)value };
            }
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as InputIntArrayScript).ReadOnly = value;
                (m_ctrl as InputIntArrayScript).caretVisible = !value;
            }
        }

        public void updateUnitEnob(DObject opts) {
            (m_ctrl as InputIntArrayScript).updateDispUnitEnob(opts);
        }

        public string unitData {
            get => (m_ctrl as InputIntArrayScript).unitData;
            set => (m_ctrl as InputIntArrayScript).unitData = value;
        }

        public string unitDisp {
            get => (m_ctrl as InputIntArrayScript).unitDisp;
            set => (m_ctrl as InputIntArrayScript).unitDisp = value;
        }

        public string unitEnobType {
            get => (m_ctrl as InputIntArrayScript).unitEnobType;
            set => (m_ctrl as InputIntArrayScript).unitEnobType = value;
        }

    }

    
}
