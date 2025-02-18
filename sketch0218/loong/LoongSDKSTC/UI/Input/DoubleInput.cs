
using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.STC;
using LoongCAD.Util;

namespace LoongCAD.UI
{

    /// <summary>
    /// double类型数据输入框
    /// </summary>
    [ToolboxItem(true)]
    public class DoubleInput : InputBase
    {
        private double m_min = Double.MinValue;
        private double m_max = Double.MaxValue;
        private double m_defVal = 0.0;
        private double m_emptyValue = double.MaxValue;

        /// <summary>
        /// 构造函数
        /// </summary>
        public DoubleInput() {

        }

        /// <summary>
        /// 控件数值
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double Value {
            get => this._text2Double(this.Text, m_min, m_max, m_defVal, m_emptyValue);
            set => base.Text = this._double2Text(value, m_min, m_max, m_defVal, m_emptyValue);
        }

        /// <summary>
        /// 控件文本
        /// </summary>
        [Browsable(false), DefaultValue("")]
        public new string Text {
            get => base.Text;
            set => base.Text = value;
        }

        /// <summary>
        /// 控件默认值
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double DefaultValue {
            get => m_defVal;
            set => m_defVal = value;
        }

        /// <summary>
        /// 控件空值，当数值为此值时输入框显示为空
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double EmptyValue
        {
            get => m_emptyValue;
            set => m_emptyValue = value;
        }

        /// <summary>
        /// 最大有效值
        /// </summary>
        [DefaultValue(Double.MaxValue)]
        public double Max {
            get => m_max;
            set => m_max = value;
        }

        /// <summary>
        /// 最小有效值
        /// </summary>
        [DefaultValue(Double.MinValue)]
        public double Min {
            get => m_min;
            set => m_min = value;
        }

        /// <inheritdoc/>
        public override void SetParameter(object value)
        {
            this.Value = (value is double) ? (double)value : 0.0;
        }

        /// <inheritdoc/>
        public override object GetParameter()
        {
            return this.Value;
        }

        /// <inheritdoc/>
        public override void ClearParameter()
        {
            this.Value = this.EmptyValue;
        }

        /// <inheritdoc/>
        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            if (!Define.IS_RUNTIME || m_unitDisp == "ft-in")
                return;

            if (e.KeyCode == Keys.Enter
                && !String.IsNullOrEmpty(this.Text)) {
                double v = this._evalText2Double(this.Text);
                this.Text = double.IsNaN(v)? this.DefaultValue.ToString() : v.ToString();
            }
        }

        protected override double _text2Double(string text, double min, double max, double defVal, double emptyTip) {
            if (!Define.IS_RUNTIME)
                return base._text2Double(text, min, max, defVal, emptyTip);

            if (string.IsNullOrEmpty(text))
                return emptyTip;

            //ft-in格式，不支持表达式，直接解析
            if (m_unitDisp == "ft-in")
                return base._text2Double(text, min, max, defVal, emptyTip);

            //解析表达式
            double val = this._evalText2Double(text);
            if (double.IsNaN(val))
                return defVal;

            if (m_unitData != "" && m_unitDisp != "")
                return Math.Max(min, Math.Min(max, Unit.Cvt(val, m_unitDisp, m_unitData)));
            else
                return Math.Max(min, Math.Min(max, val));
        }

        protected override void _onValueValidated() {
            //NOTE: 用户输入后，不自动修正内容感觉更符合体验
            //if (base.Text.Trim().Length > 0) {
            //    base.Text = _double2Text(_text2Double(this.Text, m_min, m_max, m_defVal), m_defVal);
            //}
        }

        protected override bool VerifyPlaceholder(object val)
        {
            if (val == null)
                return false;
            if (val is double) {
                return (double)val != this.EmptyValue;
            }
            else if (val is string) {
                if (val.ToString().Length == 0)
                    return false;

                double doubleVal;
                if (double.TryParse(val.ToString(), out doubleVal)) {
                    return doubleVal != this.EmptyValue;
                }
                return false;
            }

            return false;
        }

    }
}
