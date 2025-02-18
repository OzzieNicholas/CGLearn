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
using LoongCAD.STC;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputDouble : InputBase
    {
        private double m_min = Double.MinValue;
        private double m_max = Double.MaxValue;
        private double m_defVal = 0.0;

        public InputDouble() {
            
        }

        [DefaultValue(0)]
        public double Value {
            get => this._text2Double(this.Text, m_min, m_max, m_defVal);
            set => base.Text = this._double2Text(value, m_min, m_max, m_defVal);
        }

        [Browsable(false), DefaultValue("")]
        public new string Text {
            get => base.Text;
            set => base.Text = value;
        }

        [DefaultValue(0)]
        public double DefValue {
            get => m_defVal;
            set => m_defVal = value;
        }

        [DefaultValue(Double.MaxValue)]
        public double Max {
            get => m_max;
            set => m_max = value;
        }

        [DefaultValue(Double.MinValue)]
        public double Min {
            get => m_min;
            set => m_min = value;
        }

        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            if (!Define.IS_RUNTIME || unitDisp == "ft-in")
                return;

            if (e.KeyCode == Keys.Enter) {
                double v = this._evalText2Double(this.Text);
                if (double.IsNaN(v))
                    this.Text = this.DefValue.ToString();
                else
                    this.Text = v.ToString();
            }
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

        protected override void _onValueValidated() {
            //NOTE: 用户输入后，不自动修正内容感觉更符合体验
            //if (base.Text.Trim().Length > 0) {
            //    base.Text = _double2Text(_text2Double(this.Text, m_min, m_max, m_defVal), m_defVal);
            //}
        }

       
    }
}
