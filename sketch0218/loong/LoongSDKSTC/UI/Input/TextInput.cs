
using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

namespace LoongCAD.UI
{
    public class TextInput : InputBase
    {
        private bool m_trim = false;
        private string m_ignoreChars = "";

        public TextInput() {

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
        public string IgnoreChars {
            get => m_ignoreChars;
            set => m_ignoreChars = value.Trim();
        }

        public override void SetParameter(object value)
        {
            this.Value = value != null ? value.ToString() : "";
        }

        public override object GetParameter()
        {
            return this.Value;
        }

        public override void ClearParameter()
        {
            this.Value = "";
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            if (m_ignoreChars.IndexOf(e.KeyChar) >= 0)
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
