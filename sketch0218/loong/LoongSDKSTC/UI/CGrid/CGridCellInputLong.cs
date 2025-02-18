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
    class CGridCellInputLong : CGridCell
    {
        public CGridCellInputLong(CGrid grid) : base(grid) {
            m_type = CGridCellType.INPUT_LONG;

            var tb = new LongInput();
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
            get => (m_ctrl as LongInput).Value;
            set => (m_ctrl as LongInput).Value = (long)value;
        }

        public long DefaultValue {
            get => (m_ctrl as LongInput).DefaultValue;
            set => (m_ctrl as LongInput).DefaultValue = value;
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as LongInput).ReadOnly = value;
                (m_ctrl as LongInput).CaretVisible = !value;
            }
        }

        public long Min {
            get => (m_ctrl as LongInput).Min;
            set => (m_ctrl as LongInput).Min = value;
        }

        public long Max {
            get => (m_ctrl as LongInput).Max;
            set => (m_ctrl as LongInput).Max = value;
        }
    }

    
}
