using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputLong : InputBase
    {
        private long m_min = Int64.MinValue;
        private long m_max = Int64.MaxValue;
        private long m_defVal = 0L;
        
        public InputLong() {

        }

        [DefaultValue(0)]
        public long Value {
            get => this._text2Val(this.Text);
            set {
                base.Text = this._val2Text(value);
            }
        }

        [Browsable(false), DefaultValue("")]
        public new string Text {
            get => base.Text;
            set => base.Text = value;
        }

        [DefaultValue(0)]
        public long DefValue {
            get => m_defVal;
            set => m_defVal = value;
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

        protected override void _onValueValidated() {
            //if (this.Text.Trim().Length > 0) {
            //    base.Text = _val2Text(this._text2Val(this.Text));
            //}
        }

        protected string _val2Text(long v) {
            if (v == m_defVal) {
                return m_defAutoClear ? "" : "" + m_defVal;
            }
            
            return "" + Math.Max(m_min, Math.Min(m_max, v));
        }

        protected long _text2Val(string text) {
            if (string.IsNullOrEmpty(text))
                return m_defVal;

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
            if (Int64.TryParse(s, out val)) {
                return Math.Max(m_min, Math.Min(m_max, val));
            }

            double vald = 0.0;
            if (Double.TryParse(s, out vald)) {
                return Math.Max(m_min, Math.Min(m_max, (long)vald));
            }
            
            return m_defVal;
        }
    }
}
