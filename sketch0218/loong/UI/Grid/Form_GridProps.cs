using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public partial class Form_GridProps : ParaForm
    {
        public static Form_GridProps m_instance = new Form_GridProps();
        public static Form_GridProps instance => m_instance;

        public readonly string[] m_labelUnits = new string[] { "mm", "cm", "m", "inch", "ft", "ft-in" };

        public Form_GridProps()
        {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls()
        {
            bar_cfg.init(this, "CFG_GRID_PROPS");
            this.updateTabOrder();
        }

        public void show(DObject props)
        {
            bar_cfg.reset();
            this.setChecks(true);
            this.updateDispUnitEnob(Form_Options.instance.props);

            if (props == null)
            {
                var result = Native.getCommonCfg("CFG_GRID_PROPS", "$runtime");
                if (result != null)
                {
                    this.updateUIByParas(result);
                }
            }
            else
            {
                this.updateUIByParas(props);
            }

            this.Visible = true;
            this.focus();
        }

        private void gridForm_closeBtn_Click(object sender, EventArgs e)
        {
            this.hide();
        }

        private void btn_create_Click(object sender, EventArgs e)
        {
            DialogResult ret = MessageBox.Show("创建新的轴网?", "创建轴网", MessageBoxButtons.OKCancel);
            if (ret == DialogResult.OK)
            {
                LoongCAD.Util.DObject paras = this.getParasByUI();
                paras["reset"] = true;
            }
        }

        private void btn_modify_Click(object sender, EventArgs e)
        {
            var paras = this.getParasByUI();
            paras["reset"] = true;
            //Native.modifyCurObjects(paras, Define.ObjectType.OBJ_GRID);
        }

        private void btn_pick_Click(object sender, EventArgs e)
        {
            LoongCAD.Util.DObject result = Native.curObjProps();
            if (result == null)
                return;

            LoongCAD.STC.Define.ObjectType objType = (LoongCAD.STC.Define.ObjectType)result["obj_type"];
            if (objType != LoongCAD.STC.Define.ObjectType.OBJ_GRID)
                return;

            this.updateUIByParas(result);
            this.setChecks(true);
        }

        private void btn_chb_Click(object sender, EventArgs e)
        {
            this.switchChecks(this);
        }

        private void btn_labelX_auto_Click(object sender, EventArgs e)
        {
            var labels = tb_labelX.Value;
            if (labels.Length > 3)
            {
                if (MessageBox.Show(this, "是否替换当前标签内容?", "提示", MessageBoxButtons.OKCancel) == DialogResult.Cancel)
                    return;
            }

            var s = this._evalLabel(tb_vecX.Value, tb_labelX.Value, false);
            if (!String.IsNullOrEmpty(s))
                tb_labelX.Text = s;
        }

        private void btn_labelY_auto_Click(object sender, EventArgs e)
        {
            var labels = tb_labelY.Value;
            if (labels.Length > 3)
            {
                if (MessageBox.Show(this, "是否替换当前标签内容?", "提示", MessageBoxButtons.OKCancel) == DialogResult.Cancel)
                    return;
            }

            var s = this._evalLabel(tb_vecY.Value, tb_labelY.Value, false);
            if (!String.IsNullOrEmpty(s))
                tb_labelY.Text = s;
        }

        private void btn_labelZ_auto_Click(object sender, EventArgs e)
        {
            var labels = tb_labelZ.Value;
            if (labels.Length > 3)
            {
                if (MessageBox.Show(this, "是否替换当前标签内容?", "提示", MessageBoxButtons.OKCancel) == DialogResult.Cancel)
                    return;
            }

            var s = this._evalLabel(tb_vecZ.Value, tb_labelZ.Value, true);
            if (!String.IsNullOrEmpty(s))
                tb_labelZ.Text = s;
        }

        private string _evalLabel(double[] axies, string[] labels, bool lvl)
        {
            string prefix = "";
            string postfix = "";
            object counter = null;
            string counterUnit = "";

            if (labels.Length == 1)
            {
                List<string> splits = new List<string>();
                string cur = "";
                string s = labels[0];
                for (int i = 0; i < s.Length; ++i)
                {
                    char c = s[i];

                    if (String.IsNullOrEmpty(cur))
                    {
                        cur += c;
                        continue;
                    }

                    if (!char.IsLetter(cur.Last()) && char.IsLetter(c))
                    {
                        splits.Add(cur);
                        cur = "" + c;
                        continue;
                    }

                    if (!char.IsDigit(cur.Last()) && char.IsDigit(c))
                    {
                        splits.Add(cur);
                        cur = "" + c;
                        continue;
                    }

                    if (c == '#' || cur.Last() == '#'
                                 || c == '[' || cur.Last() == '['
                                 || c == ']' || cur.Last() == ']')
                    {
                        splits.Add(cur);
                        cur = "" + c;
                        continue;
                    }

                    cur += c;
                }

                if (!String.IsNullOrEmpty(cur))
                    splits.Add(cur);

                int iCounter = -1;
                for (int i = splits.Count - 1; i >= 0; --i)
                {
                    //查找 #
                    if (splits[i] == "#")
                    {
                        iCounter = i;
                        break;
                    }
                }

                if (iCounter < 0)
                {
                    for (int i = splits.Count - 1; i >= 0; --i)
                    {
                        //查找数字
                        if (int.TryParse(splits[i], out int tmpv))
                        {
                            iCounter = i;
                            break;
                        }
                    }
                }

                if (iCounter < 0)
                {
                    for (int i = splits.Count - 1; i >= 0; --i)
                    {
                        if (splits[i].Length == 1 && char.IsLetter(splits[i].First()))
                        {
                            iCounter = i;
                            break;
                        }
                    }
                }

                if (iCounter < 0)
                {
                    prefix = s;
                    counter = null;
                }
                else
                {
                    for (int i = 0; i < iCounter; ++i)
                        prefix += splits[i];

                    for (int i = iCounter + 1; i < splits.Count; ++i)
                        postfix += splits[i];

                    if (int.TryParse(splits[iCounter], out int tmpv))
                        counter = tmpv;
                    else if (splits[iCounter].Length == 1 && char.IsLetter(splits[iCounter].First()))
                        counter = splits[iCounter].First();
                    else if (splits[iCounter] == "#")
                    {
                        counter = null;
                        counterUnit = "";
                        if (iCounter < splits.Count - 3
                            && splits[iCounter + 1] == "[")
                        {
                            string unit = "";
                            int iUnitEnd = -1;
                            for (int i = iCounter + 2; i < splits.Count; ++i)
                            {
                                if (splits[i] == "]")
                                {
                                    iUnitEnd = i;
                                    break;
                                }

                                unit += splits[i];
                            }

                            if (iUnitEnd >= 0)
                            {
                                for (int i = 0; i < m_labelUnits.Length; ++i)
                                {
                                    if (unit == m_labelUnits[i])
                                    {
                                        counterUnit = m_labelUnits[i];
                                        break;
                                    }
                                }

                                postfix = "";
                                for (int i = iUnitEnd + 1; i < splits.Count; ++i)
                                    postfix += splits[i];
                            }
                        }
                    }
                    else
                        counter = null;
                }
            }
            else if (labels.Length >= 2)
            {
                prefix = labels[0];
                postfix = labels.Length > 2 ? labels[2] : "";

                string s = labels[1];
                if (int.TryParse(s, out int v))
                    counter = v;
                else if (s.Length == 1 && char.IsLetter(s[0]))
                    counter = s[0];
                else if (s.StartsWith("#"))
                {
                    counter = null;
                    counterUnit = "";

                    var ss = s.Substring(1);
                    for (int i = 0; i < m_labelUnits.Length; ++i)
                    {
                        if (ss == "[" + m_labelUnits[i] + "]")
                        {
                            counterUnit = m_labelUnits[i];
                            break;
                        }
                    }
                }
                else
                {
                    counter = null;
                    counterUnit = "";
                }
            }

            string res = "";
            if (counter is char)
            {
                //字母
                string cur = "";
                char c = (char)counter;
                if (char.IsUpper((char)counter))
                {
                    for (int i = 0; i < axies.Length; ++i)
                    {
                        if (i > 0)
                            res += " ";

                        res += prefix + cur + c + postfix;
                        c += (char)1;
                        if (c > 'Z')
                        {
                            cur += counter;
                            c = (char)counter;
                        }
                    }
                }
                else if (char.IsLower((char)counter))
                {
                    for (int i = 0; i < axies.Length; ++i)
                    {
                        if (i > 0)
                            res += " ";

                        res += prefix + cur + c + postfix;
                        c += (char)1;
                        if (c > 'z')
                        {
                            cur += counter;
                            c = (char)counter;
                        }
                    }
                }
            }
            else if (counter is int)
            {
                int n = (int)counter;
                for (int i = 0; i < axies.Length; ++i)
                {
                    if (i > 0)
                        res += " ";

                    res += prefix + n + postfix;
                    if (n >= 0)
                        n++;
                    else
                        n--;
                }
            }
            else
            {
                //用实际值
                if (lvl)
                {
                    //标高，绝对值
                    for (int i = 0; i < axies.Length; ++i)
                    {
                        double g = axies[i];
                        if (i > 0)
                            res += " ";

                        string sv = String.IsNullOrEmpty(counterUnit)
                            ? g.ToString()
                            : Unit.Str(g, "mm", counterUnit, 3);
                        res += prefix + ((g >= 0.0) ? ("+" + sv) : sv) + postfix;
                    }
                }
                else
                {
                    //其它轴，相对值
                    double g = 0.0;
                    for (int i = 0; i < axies.Length; ++i)
                    {
                        g += axies[i];
                        if (i > 0)
                            res += " ";

                        string sv = String.IsNullOrEmpty(counterUnit)
                            ? g.ToString()
                            : Unit.Str(g, "mm", counterUnit, 3);
                        res += prefix + ((g >= 0.0) ? ("+" + sv) : sv) + postfix;
                    }
                }
            }

            return res;
        }

        private void lb_label_auto_tips_Click(object sender, EventArgs e)
        {
            if (Form_Grid_Label_Auto_Tips_Context.instance.hideFrame != Native.STCoreFrame())
            {
                Form_Grid_Label_Auto_Tips_Context.instance.show(this,
                    lb_label_auto_tips.RectangleToScreen(lb_label_auto_tips.ClientRectangle));
            }
        }
    }
}