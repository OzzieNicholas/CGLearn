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

namespace LoongCAD.UI
{
    class CGridCellInputIntArray : CGridCell
    {
        public CGridCellInputIntArray(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_INT_ARRAY;

            var tb = new IntArrayInput();
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
            get => (m_ctrl as IntArrayInput).Value;
            set {
                var tb = m_ctrl as IntArrayInput;
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
                (m_ctrl as IntArrayInput).ReadOnly = value;
                (m_ctrl as IntArrayInput).CaretVisible = !value;
            }
        }

        public void UpdateUnitEnob(DObject opts) {
            (m_ctrl as IntArrayInput).UpdateDispUnitEnob(opts);
        }

        public string UnitData {
            get => (m_ctrl as IntArrayInput).UnitData;
            set => (m_ctrl as IntArrayInput).UnitData = value;
        }

        public string UnitDisplay {
            get => (m_ctrl as IntArrayInput).UnitDisplay;
            set => (m_ctrl as IntArrayInput).UnitDisplay = value;
        }

        public string UnitEnobType {
            get => (m_ctrl as IntArrayInput).UnitEnobType;
            set => (m_ctrl as IntArrayInput).UnitEnobType = value;
        }

    }

    
}
