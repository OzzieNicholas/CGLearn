
using System;
using System.Drawing;
using System.ComponentModel;

namespace LoongCAD.UI
{
    public class LongInput : InputBase
    {
        private long m_min = Int64.MinValue;
        private long m_max = Int64.MaxValue;
        private long m_defVal = 0L;
        private long m_emptyTip = long.MaxValue;

        public LongInput() {

        }

        [DefaultValue(0)]
        public long Value {
            get => this._text2Val(this.Text);
            set => base.Text = this._val2Text(value);
        }

        [Browsable(false), DefaultValue("")]
        public new string Text {
            get => base.Text;
            set => base.Text = value;
        }

        [DefaultValue(0)]
        public long DefaultValue {
            get => m_defVal;
            set => m_defVal = value;
        }

        [DefaultValue(long.MaxValue)]
        public long EmptyTip {
            get => m_emptyTip;
            set => m_emptyTip = value;
        }

        [DefaultValue(long.MaxValue)]
        public long Max {
            get => m_max;
            set => m_max = value;
        }

        [DefaultValue(long.MinValue)]
        public long Min {
            get => m_min;
            set => m_min = value;
        }

        public override void SetParameter(object value)
        {
            if (value is int) {
                this.Value = (long)(int)value;
            } else if (value is double) {
                this.Value = (long)(double)value;
            } else if (value is long) {
                this.Value = (long)value;
            }
        }

        public override object GetParameter()
        {
            return this.Value;
        }

        public override void ClearParameter()
        {
            this.Value = this.EmptyTip;
        }

        protected override void _onValueValidated() {
            //if (this.Text.Trim().Length > 0) {
            //    base.Text = _val2Text(this._text2Val(this.Text));
            //}
        }

        protected string _val2Text(long v) {
            if (v == m_emptyTip) 
                return "";

            return "" + Math.Max(m_min, Math.Min(m_max, v));
        }

        protected long _text2Val(string text) {
            if (string.IsNullOrEmpty(text))
                return m_emptyTip;

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

            long val = 0;
            if (Int64.TryParse(s, out val)) 
                return Math.Max(m_min, Math.Min(m_max, val));

            double vald = 0.0;
            if (Double.TryParse(s, out vald)) 
                return Math.Max(m_min, Math.Min(m_max, (long)vald));

            return m_defVal;
        }
    }
}
