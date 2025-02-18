
using System;
using System.Drawing;
using System.ComponentModel;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class IntInput : InputBase
    {
        private int m_min = Int32.MinValue;
        private int m_max = Int32.MaxValue;
        private int m_defVal = 0;
        private int m_emptyValue = int.MaxValue;

        public IntInput() {

        }

        [DefaultValue(double.MaxValue)]
        public int Value {
            get => this._text2Val(this.Text);
            set {
                this.Text = this._val2Text(value);
            }
        }

        [DefaultValue(int.MaxValue)]
        public int DefaultValue {
            get => m_defVal;
            set {
                m_defVal = value;
            }
        }

        [DefaultValue(Int32.MaxValue)]
        public int Max {
            get => m_max;
            set {
                m_max = value;
                this.Text = _val2Text(_text2Val(this.Text));
            }
        }

        [DefaultValue(Int32.MinValue)]
        public int Min {
            get => m_min;
            set {
                m_min = value;
                this.Text = _val2Text(_text2Val(this.Text));
            }
        }

        [DefaultValue(int.MaxValue)]
        public int EmptyValue { get => m_emptyValue; set => m_emptyValue = value; }

        public override void SetParameter(object value)
        {
            if (value is int) {
                this.Value = (int)value;
            }
            else if(value is double) {
                this.Value = (int)(double)value;
            }
            else if (value is long) {
                this.Value = (int)(long)value;
            }
        }

        public override object GetParameter()
        {
            return this.Value;
        }

        public override void ClearParameter()
        {
            this.Value = m_emptyValue;
        }

        protected override void _onValueValidated() {
            if (this.Text.Trim().Length > 0) {
                this.Text = _val2Text(this._text2Val(this.Text));
            }
        }

        protected string _val2Text(int v) {
            if (v == m_emptyValue) 
                return "";

            return "" + Math.Max(m_min, Math.Min(m_max, v));
        }

        protected int _text2Val(string text) {
            if (string.IsNullOrEmpty(text))
                return m_emptyValue;

            string s = "";
            for (int i = 0; i < text.Length; ++i) {
                char c = text[i];
                if (char.IsNumber(c))
                    s += c;
                else if (c == '-' && s.Length == 0)
                    s += c;
                else if (c == '.'
                        && s.IndexOf('.') < 0)
                    s += c;
            }

            double vald = 0.0;
            if (Double.TryParse(s, out vald)) {
                return Math.Max(m_min, Math.Min(m_max, (int)vald));
            }

            return m_defVal;
        }

        protected override bool VerifyPlaceholder(object val)
        {
            if (val == null)
                return false;

            if (val is int) {
                return (int)val != this.EmptyValue;
            }
            else if (val is string) {
                if (((string)val).Length == 0) {
                    return false;
                }

                int intVal;
                if (int.TryParse(val.ToString(), out intVal))
                    return intVal != this.EmptyValue;
                return false;
            }

            return false;
        }

    }
}
