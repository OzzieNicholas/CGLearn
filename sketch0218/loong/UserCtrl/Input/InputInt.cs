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
    public class InputInt : InputBase
    {
        private int m_min = Int32.MinValue;
        private int m_max = Int32.MaxValue;
        private int m_defVal = 0;

        public InputInt() {

        }

        [DefaultValue(0)]
        public int Value {
            get => this._text2Val(this.Text);
            set {
                this.Text = this._val2Text(value);
            }
        }

        [DefaultValue(0)]
        public int DefValue {
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

        protected override void _onValueValidated() {
            if (this.Text.Trim().Length > 0) {
                this.Text = _val2Text(this._text2Val(this.Text));
            }
        }

        protected string _val2Text(int v) {
            if (v == m_defVal) {
                return m_defAutoClear ? "" : "" + m_defVal;
            }

            return "" + Math.Max(m_min, Math.Min(m_max, v));
        }

        protected int _text2Val(string text) {
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

            double vald = 0.0;
            if (Double.TryParse(s, out vald)) {
                return Math.Max(m_min, Math.Min(m_max, (int)vald));
            }

            return m_defVal;
        }
    }
}
