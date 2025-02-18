using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UI
{
    public class ShortCutCell : DataGridViewTextBoxCell
    {
        private FuncUnit m_fu = null;
        private uint m_shortCut = 0;

        public ShortCutCell() : base() {
            this.ToolTipText = "选中后按下新快捷键组合，右键单击清除";
        }

        ~ShortCutCell() {

        }

        public ShortCutGrid grid => this.DataGridView as ShortCutGrid;

        public FuncUnit fu {
            get => m_fu;
            set => m_fu = value;
        }

        public uint shortCut {
            get => m_shortCut;
            set {
                m_shortCut = value;
                this.Value = FuncUnit.shortCut2Str(m_shortCut);
            }
        }

        protected override void OnMouseClick(DataGridViewCellMouseEventArgs e) {
            base.OnMouseDown(e);

            if (m_fu == null)
                return;

            if (e.Button == MouseButtons.Left) {
                if (this.DataGridView != null && !this.ReadOnly)
                    this.DataGridView.BeginEdit(true);
            }
            else if (e.Button == MouseButtons.Right && !this.IsInEditMode) {
                this.grid.updateCell(this, 0);
            }
        }

        public override void InitializeEditingControl(int row, object val, DataGridViewCellStyle style) {
            base.InitializeEditingControl(row, val, style);
        }

        public override Type EditType => typeof(ShortCutEditCtrl);
        public override Type ValueType => typeof(string);
        public override object DefaultNewRowValue => 0.0;

        public override object ParseFormattedValue(object formattedValue, DataGridViewCellStyle cellStyle, TypeConverter formattedValueTypeConverter, TypeConverter valueTypeConverter) {
            if (formattedValue == null)
                return "";

            if (formattedValue.GetType() != typeof(string))
                return "";

            return formattedValue;
        }
    }

}
