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
    public class InputTextArray : InputBase, IUserCtrl
    {
        public InputTextArray() {
            this.Text = "";
        }

        [DefaultValue(null)]
        public string[] Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
        }

        protected string _val2Text(string[] v) {
            string str = "";
            for (int i = 0; i < v.Length; ++i) {
                if (i != 0)
                    str += ' ';

                str += v[i].Trim();
            }

            return str;
        }

        protected string[] _text2Val(string str) {
            if (String.IsNullOrEmpty(str))
                return new string[0];

            List<string> list = new List<string>();

            //split by space
            string[] strs = str.Trim().Split(' ');
            for (int i = 0; i < strs.Length; ++i) {
                string s = strs[i].Trim();
                if (s.Length > 0)
                    list.Add(s);
            }

            return list.ToArray();
        }
    }
}
