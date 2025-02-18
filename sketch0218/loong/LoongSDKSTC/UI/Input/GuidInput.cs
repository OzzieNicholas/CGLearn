using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class GuidInput : InputBase
    {
        public GuidInput() {
            
        }

        [DefaultValue("")]
        public Guid Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
        }

        public override void SetParameter(object value)
        {
            if (value is string) {
                try {
                    this.Value = new Guid(value as string);
                } catch {
                    this.Value = Guid.Empty;
                }
            }
            else if (value is Guid) {
                this.Value = (Guid)value;
            }
        }

        public override object GetParameter()
        {
            return this.Value;
        }

        public override void ClearParameter()
        {
            this.Value = Guid.Empty;
        }

        protected string _val2Text(Guid v) {
            return v.ToString("N").ToUpper();
        }

        protected Guid _text2Val(string text) {
            try {
                return new Guid(text);
            }
            catch {
                return Guid.Empty;
            }
        }
    }
}
