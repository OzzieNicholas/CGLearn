
using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace LoongCAD.UI
{
    public class LongArrayInput : InputBase
    {
        private bool m_autoMerge = true;

        public LongArrayInput() {
            this.CharacterCasing = CharacterCasing.Upper;
        }

        [DefaultValue(null)]
        public long[] Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
        }

        [DefaultValue(true)]
        public bool AutoMerge {
            get => m_autoMerge;
            set => m_autoMerge = value;
        }

        public override void SetParameter(object value)
        {
            if (value is long[]) {
                this.Value = (long[])value;
            } else if (value is long) {
                this.Value = new long[] { (long)value };
            }
        }

        public override object GetParameter()
        {
            return this.Value;
        }

        public override void ClearParameter()
        {
            this.Value = null;
        }

        protected override void _onValueValidated() {
            long[] v = this._text2Val(this.Text);
            this.Text = _val2Text(v);
        }

        protected string _val2Text(long[] v) {
            string str = "";
            long last = 0;
            int count = 0;
            for (int i = 0; i < v.Length; ++i) {
                if (m_autoMerge && v[i] == last) {
                    count += 1;
                    continue;
                }

                if (count == 1) {
                    str += last;
                    str += ' ';
                }
                else if (count > 1) {
                    str += count;
                    str += '*';
                    str += last;
                    str += ' ';
                }

                last = v[i];
                count = 1;
            }

            if (count == 1) {
                str += last;
            }
            else if (count > 1) {
                str += count;
                str += '*';
                str += last;
            }

            return str;
        }

        protected long[] _text2Val(string str) {
            str = str.Trim();

            //skip left blank
            if (str.Length == 0)
                return new long[0];

            List<long> list = new List<long>();

            try {
                int off = 0;

                //split by space
                int p = str.IndexOf(' ', off);
                while (p != -1) {
                    string sub = str.Substring(off, p - off);
                    string s = "";
                    for (int i = 0; i < sub.Length; ++i) {
                        char c = sub[i];
                        if (char.IsNumber(c))
                            s += c;
                        else if ((c == 'X' || c == '*')
                                && s.IndexOf('X') < 0
                                && s.IndexOf('*') < 0)
                            s += c;
                    }

                    int starOff = s.IndexOf('*');
                    if (starOff == -1)
                        starOff = s.IndexOf('X');

                    if (starOff == -1) {
                        long d = 0;
                        if (long.TryParse(s, out d))
                            list.Add(d);
                    }
                    else {
                        string s0 = s.Substring(0, starOff);
                        string s1 = s.Substring(starOff + 1);

                        int count = 0;
                        long d = 0;
                        if (Int32.TryParse(s0, out count)
                            && long.TryParse(s1, out d)) {
                            for (int i = 0; i < count; ++i) {
                                list.Add(d);
                            }
                        }
                    }

                    off = p + 1;

                    while (off < str.Length && str[off] == ' ')
                        off++;

                    if (off >= str.Length)
                        break;

                    p = str.IndexOf(' ', off);
                }

                if (off < str.Length) {
                    string s = str.Substring(off);

                    int starOff = s.IndexOf('*');
                    if (starOff == -1)
                        starOff = s.IndexOf('X');

                    if (starOff == -1) {
                        long d = long.Parse(s);
                        list.Add(d);
                    }
                    else {
                        string s0 = s.Substring(0, starOff);
                        string s1 = s.Substring(starOff + 1);

                        int count = Int32.Parse(s0);
                        long d = long.Parse(s1);

                        for (int i = 0; i < count; ++i) {
                            list.Add(d);
                        }
                    }
                }
            }
            catch {
                list.Clear();
            }

            return list.ToArray();
        }
    }
}
