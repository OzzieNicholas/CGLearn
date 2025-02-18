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
    public class InputGuid : InputBase
    {
        public InputGuid() {
            
        }

        [DefaultValue("")]
        public Guid Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
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
