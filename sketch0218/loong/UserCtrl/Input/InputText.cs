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
    public class InputText : InputBase
    {
        private bool m_trim = false;
        private string m_filter = "";

        public InputText() {
            
        }

        [DefaultValue("")]
        public string Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
        }


        [DefaultValue(true)]
        public bool Trim {
            get => m_trim;
            set => m_trim = value;
        }

        [DefaultValue("")]
        public string FilterChars {
            get => m_filter;
            set => m_filter = value.Trim();
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            if (m_filter.IndexOf(e.KeyChar) >= 0)
                e.Handled = true;

            var key = (Keys)e.KeyChar;
            if (key == Keys.Enter
                || key == Keys.Escape
                || key == Keys.Tab) //prevent beep sound
                e.Handled = true;
        }

        protected string _val2Text(string v) {
            if (v == null)
                v = "";

            if (m_trim)
                v = v.Trim();

            return v;
        }

        protected string _text2Val(string text) {
            if (m_trim)
                text = text.Trim();

            return text;
        }
    }
}
