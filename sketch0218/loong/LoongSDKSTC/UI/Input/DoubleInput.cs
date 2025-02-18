
using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.STC;
using LoongCAD.Util;

namespace LoongCAD.UI
{

    /// <summary>
    /// double�������������
    /// </summary>
    [ToolboxItem(true)]
    public class DoubleInput : InputBase
    {
        private double m_min = Double.MinValue;
        private double m_max = Double.MaxValue;
        private double m_defVal = 0.0;
        private double m_emptyValue = double.MaxValue;

        /// <summary>
        /// ���캯��
        /// </summary>
        public DoubleInput() {

        }

        /// <summary>
        /// �ؼ���ֵ
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double Value {
            get => this._text2Double(this.Text, m_min, m_max, m_defVal, m_emptyValue);
            set => base.Text = this._double2Text(value, m_min, m_max, m_defVal, m_emptyValue);
        }

        /// <summary>
        /// �ؼ��ı�
        /// </summary>
        [Browsable(false), DefaultValue("")]
        public new string Text {
            get => base.Text;
            set => base.Text = value;
        }

        /// <summary>
        /// �ؼ�Ĭ��ֵ
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double DefaultValue {
            get => m_defVal;
            set => m_defVal = value;
        }

        /// <summary>
        /// �ؼ���ֵ������ֵΪ��ֵʱ�������ʾΪ��
        /// </summary>
        [DefaultValue(double.MaxValue)]
        public double EmptyValue
        {
            get => m_emptyValue;
            set => m_emptyValue = value;
        }

        /// <summary>
        /// �����Чֵ
        /// </summary>
        [DefaultValue(Double.MaxValue)]
        public double Max {
            get => m_max;
            set => m_max = value;
        }

        /// <summary>
        /// ��С��Чֵ
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

            //ft-in��ʽ����֧�ֱ��ʽ��ֱ�ӽ���
            if (m_unitDisp == "ft-in")
                return base._text2Double(text, min, max, defVal, emptyTip);

            //�������ʽ
            double val = this._evalText2Double(text);
            if (double.IsNaN(val))
                return defVal;

            if (m_unitData != "" && m_unitDisp != "")
                return Math.Max(min, Math.Min(max, Unit.Cvt(val, m_unitDisp, m_unitData)));
            else
                return Math.Max(min, Math.Min(max, val));
        }

        protected override void _onValueValidated() {
            //NOTE: �û�����󣬲��Զ��������ݸо�����������
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
