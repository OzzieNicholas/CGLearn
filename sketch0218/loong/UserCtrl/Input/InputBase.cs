using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Drawing2D;
using LoongCAD.Util;

namespace UserCtrl
{
    public class InputBase : TextBox, IUserCtrl {
        protected string m_propName = "";
        protected string m_checkName = "";
        protected bool m_checkOnChange = true;
        protected string m_tabGroup = "";
        protected string m_unitData = "mm";
        protected string m_unitDisp = "mm";
        protected int m_enob = 2; //有效位数
        protected string m_unitEnobType = "";

        protected int m_lock = 0;

        protected bool m_defAutoClear = false;
        protected bool m_caretVisible = true;
        protected string m_waterMark = "";
        protected string m_placeholder = "";

        public InputBase() {
            SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.DoubleBuffer | ControlStyles.ResizeRedraw, true);
        }

        protected override void CreateHandle() {
            if (!IsHandleCreated) {
                try {
                    base.CreateHandle();
                }
                catch { }
                finally {
                    if (!IsHandleCreated) {
                        base.RecreateHandle();
                    }
                }
            }
        }

        [DefaultValue("mm")]
        public string unitData {
            get => m_unitData;
            set => m_unitData = value;
        }

        [DefaultValue("mm")]
        public string unitDisp {
            get => m_unitDisp;
            set => m_unitDisp = value;
        }

        [DefaultValue(2)]
        public int Enob {
            get => m_enob;
            set => m_enob = Math.Max(0, value);
        }

        [DefaultValue("")]
        public string unitEnobType {
            get => m_unitEnobType;
            set => m_unitEnobType = value;
        }

        [DefaultValue(false)]
        public bool DefAutoClear {
            get => m_defAutoClear;
            set => m_defAutoClear = value;
        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue(true)]
        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(true)]
        public bool caretVisible {
            get => m_caretVisible;
            set {
                m_caretVisible = value;
                if (m_caretVisible)
                    Win32.ShowCaret(this.Handle);
                else
                    Win32.HideCaret(this.Handle);
            }
        }

        [DefaultValue("")]
        public string placeholder
        {
            get => m_placeholder;
            set {
                m_placeholder = value;
                if (String.IsNullOrEmpty(m_placeholder))
                    Win32.SendMessage(this.Handle, Win32.EM_SETCUEBANNER, 0, 0);
                else
                    Win32.SendMessage(this.Handle, Win32.EM_SETCUEBANNER, 1, m_placeholder);
            }
        }

        public void updateDispUnitEnob(DObject cfg) {
            if (cfg == null
                || String.IsNullOrEmpty(m_unitEnobType))
                return;

            object unit = null;
            if (cfg.TryGetValue("unit_" + m_unitEnobType, out unit)) {
                this.unitDisp = unit.ToString();
            }

            object enob = null;
            if (cfg.TryGetValue("enob_" + m_unitEnobType, out enob)) {
                this.Enob = (int)enob;
            }
        }

        public void lockCtrl() {
            m_lock++;
        }

        public void unlockCtrl() {
            m_lock--;
        }

        protected override void OnGotFocus(EventArgs e) {
            base.OnGotFocus(e);

            if (!m_caretVisible)
                Win32.HideCaret(this.Handle);
        }

        protected override void OnLostFocus(EventArgs e) {
            base.OnLostFocus(e);

            if (m_lock > 0)
                return;

            this._onValidated();
        }

        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            if (m_lock > 0)
                return;

            if (e.KeyCode == Keys.Enter) {
                this._onValidated();
            }
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            //检查是否需要重置关联的Check控件
            if (m_checkOnChange && !String.IsNullOrEmpty(m_checkName)) {
                var form = this.FindForm() as ParaFormBase;
                if (form != null) {
                    var chb = form.checkCtrl(this);
                    if (chb != null && !chb.Checked)
                        chb.Checked = true;
                }
            }

            var key = (Keys)e.KeyChar;
            if (key == Keys.Enter
                || key == Keys.Escape
                || key == Keys.Tab) //prevent beep sound
                e.Handled = true;
        }

        protected virtual void _onValidated() {
            if (m_lock > 0)
                return;

            m_lock += 1;
            this._onValueValidated();
            this.OnValidated(null);
            m_lock -= 1;
        }

        protected virtual void _onValueValidated() {

        }

        protected virtual string _double2Text(double v, double min, double max, double defVal) {
            if (double.IsNaN(v))
                v = defVal;

            if (v != defVal)
                v = Math.Max(min, Math.Min(max, v));

            if (m_defAutoClear && v == defVal)
                return "";

            var s = Unit.Str(v, m_unitData, m_unitDisp, m_enob);
            if (String.IsNullOrEmpty(s) && !m_defAutoClear)
                s = Unit.Str(defVal, m_unitData, m_unitDisp, m_enob);

            return s;
        }

        protected virtual double _text2Double(string text, double min, double max, double defVal) {
            if (string.IsNullOrEmpty(text))
                return defVal;

            if (unitDisp == "ft-in") {
                string[] parts = new string[] { "", "", "" };
                List<string> backups = new List<string>();
                string s = "";
                for (int i = 0; i < text.Length; ++i) {
                    char c = text[i];
                    if (char.IsNumber(c))
                        s += c;
                    else if (c == '-') {
                        if (s.Length == 0) {
                            s += c;
                        }
                        else {
                            backups.Add(s);
                            s = "";
                        }
                    }
                    else if (c == '\'' && s.Length > 0 && s != "-") { //feet symbol
                        parts[0] = s;
                        s = "";
                    }
                    else if (c == '"' && s.Length > 0 && s != "-") { //inch symbol
                        parts[1] = s;
                        s = "";
                    }
                    else if (c == '/' && s.IndexOf('/') < 0) { // a/b format
                        s += c;
                    }
                }

                if (s.IndexOf('/') >= 0)
                    parts[2] = s;
                else if (s.Length > 0)
                    backups.Add(s);

                int backpos = 0;
                for (int i = 0; i < 3; ++i) {
                    if (!String.IsNullOrEmpty(parts[i]))
                        continue;

                    while (backpos < backups.Count) {
                        string bs = backups[backpos++];
                        if (String.IsNullOrEmpty(bs)
                            || bs == "-"
                            || bs == "/")
                            continue;

                        parts[i] = bs;
                        break;
                    }
                }

                double sign = 1.0;
                for (int i = 0; i < parts.Length; ++i) {
                    if (parts[i].Length > 0) {
                        sign = parts[i].StartsWith("-") ? -1.0 : 1.0;
                        break;
                    }
                }

                long feet = 0;
                long.TryParse(parts[0], out feet);
                feet = Math.Abs(feet);

                long inch = 0;
                long.TryParse(parts[1], out inch);
                if (feet != 0)
                    inch = Math.Abs(inch);

                double dec = 0.0;
                int divpos = parts[2].IndexOf('/');
                if (divpos >= 0) {
                    long A = 0, B = 1;
                    long.TryParse(parts[2].Substring(0, divpos), out A);
                    long.TryParse(parts[2].Substring(divpos + 1), out B);
                    if (B != 0.0)
                        dec = Math.Abs((double)A / B);
                }

                if (m_unitData != "" && m_unitDisp != "")
                    return Math.Max(min, Math.Min(max, sign * Unit.Cvt(feet * 12.0 + inch + dec, m_unitDisp, m_unitData)));
                else
                    return Math.Max(min, Math.Min(max, sign * (feet * 12.0 + inch + dec)));
            }
            else {
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

                double val = 0.0;
                if (Double.TryParse(s, out val)) {
                    if (unitData != "" && unitDisp != "")
                        return Math.Max(min, Math.Min(max, Unit.Cvt(val, unitDisp, unitData)));
                    else
                        return Math.Max(min, Math.Min(max, val));
                }
            }

            return defVal;
        }

        protected virtual double _evalText2Double(string text) {
            var res = LoongCAD.Common.Callback.evalJSExpr(text);
            if (res == null) {
                string cur = "";
                for (int i = 0; i < text.Length; ++i) {
                    char c = text[i];
                    if (char.IsDigit(c)) {
                        cur += c;
                    }
                    else if (!String.IsNullOrEmpty(cur))
                        break;
                }

                return String.IsNullOrEmpty(cur) ? double.NaN : double.Parse(cur);
            }

            return Utils.ToDouble(res, double.NaN);
        }
    }
}
