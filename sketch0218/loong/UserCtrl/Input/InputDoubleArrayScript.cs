using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputDoubleArrayScript : InputBase
    {
        private bool m_autoMerge = true;

        public InputDoubleArrayScript() {
            
        }

        [DefaultValue(null)]
        public double[] Value {
            get => this._text2Val(this.Text);
            set => this.Text = this._val2Text(value);
        }

        [DefaultValue(true)]
        public bool AutoMerge {
            get => m_autoMerge;
            set => m_autoMerge = value;
        }

        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            if (!Define.IS_RUNTIME || unitDisp == "ft-in")
                return;

            if (e.KeyCode == Keys.Enter) {
                this.Text = this._val2Text(this._text2Val(this.Text));
            }
        }

        protected override void _onValueValidated() {
            //NOTE: 用户输入后，不自动修正内容感觉更符合体验
            //double[] v = this._text2Val(this.Text);
            //this.Text = _val2Text(v);
        }

        protected string _val2Text(double[] v) {
            if (v == null 
                || v.Length == 0)
                return "";

            string str = "";
            double last = 0.0;
            int count = 0;
            for (int i = 0; i < v.Length; ++i) {
                if (m_autoMerge && v[i] == last) {
                    count += 1;
                    continue;
                }

                if (count == 1) {
                    str += _double2Text(last, double.MinValue, double.MaxValue, 0.0);
                    str += ' ';
                }
                else if (count > 1) {
                    str += count;
                    str += '*';
                    str += _double2Text(last, double.MinValue, double.MaxValue, 0.0);
                    str += ' ';
                }

                last = v[i];
                count = 1;
            }

            if (count == 1) {
                str += _double2Text(last, double.MinValue, double.MaxValue, 0.0);
            }
            else if (count > 1) {
                str += count;
                str += '*';
                str += _double2Text(last, double.MinValue, double.MaxValue, 0.0);
            }

            return str;
        }

        protected double[] _text2Val(string str) {
            str = str.Trim();

            //skip left blank
            if (String.IsNullOrEmpty(str))
                return new double[] { };

            List<double> list = new List<double>();

            try {
                int off = 0;

                //split by space
                int p = str.IndexOf(' ', off);
                while (p != -1) {
                    string s = str.Substring(off, p - off);
                    if (!m_autoMerge) {
                        list.Add(this._text2Double(s, double.MinValue, double.MaxValue, 0.0)); //非AutoMerge下，支持表达式
                    }
                    else {
                        int starOff = s.IndexOf('*');
                        if (starOff == -1)
                            starOff = s.IndexOf('X');

                        if (starOff == -1) {
                            list.Add(base._text2Double(s, double.MinValue, double.MaxValue, 0.0));
                        }
                        else {
                            string s0 = s.Substring(0, starOff);
                            string s1 = s.Substring(starOff + 1);

                            int count = 0;
                            if (Int32.TryParse(s0, out count)) {
                                double d = base._text2Double(s1, double.MinValue, double.MaxValue, 0.0);
                                for (int i = 0; i < count; ++i) {
                                    list.Add(d);
                                }
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
                    if (!m_autoMerge) {
                        list.Add(this._text2Double(s, double.MinValue, double.MaxValue, 0.0)); //非AutoMerge下，支持表达式
                    }
                    else {
                        int starOff = s.IndexOf('*');
                        if (starOff == -1)
                            starOff = s.IndexOf('X');

                        if (starOff == -1) {
                            list.Add(base._text2Double(s, double.MinValue, double.MaxValue, 0.0));
                        }
                        else {
                            string s0 = s.Substring(0, starOff);
                            string s1 = s.Substring(starOff + 1);

                            int count = 0;
                            if (Int32.TryParse(s0, out count)) {
                                double d = base._text2Double(s1, double.MinValue, double.MaxValue, 0.0);
                                for (int i = 0; i < count; ++i) {
                                    list.Add(d);
                                }
                            }
                        }
                    }
                }
            }
            catch {
                return new double[] { };
            }

            return list.ToArray();
        }

        protected override double _text2Double(string text, double min, double max, double defVal) {
            if (!Define.IS_RUNTIME)
                return base._text2Double(text, min, max, defVal);

            if (string.IsNullOrEmpty(text))
                return defVal;

            //ft-in格式，不支持表达式，直接解析
            if (unitDisp == "ft-in")
                return base._text2Double(text, min, max, defVal);

            //解析表达式
            double val = this._evalText2Double(text);
            if (double.IsNaN(val))
                return defVal;

            if (unitData != "" && unitDisp != "")
                return Math.Max(min, Math.Min(max, Unit.Cvt(val, unitDisp, unitData)));
            else
                return Math.Max(min, Math.Min(max, val));
        }
    }
}
