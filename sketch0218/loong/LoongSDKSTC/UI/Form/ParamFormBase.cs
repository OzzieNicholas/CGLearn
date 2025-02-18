using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Drawing;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace LoongCAD.UI
{
    public class ParamFormBase : Form
    {
        private bool m_disposeOnClose = true;
        private bool m_escClose = true;
        private string m_cacheBoundsKey = null;

        private ParamFormGrip m_sizableGrip = null;

        //tab switch
        private List<Control> m_tabList = new List<Control>();
        private List<Control> m_tabParList = new List<Control>();

        //bounds cache
        private static Dictionary<string, Rectangle> m_formBounds = new Dictionary<string, Rectangle>();

        public ParamFormBase() {
            this.DoubleBuffered = true;
            this.AllowTransparency = false;
            this.ShowIcon = false;
            this.KeyPreview = true;
            //this.Icon = Properties.Resources.default_icon;
        }

        [Browsable(false), DefaultValue(true)]
        public bool DisposeOnClose {
            get => m_disposeOnClose;
            set => m_disposeOnClose = value;
        }

        public virtual bool EscClose {
            get => m_escClose;
            set => m_escClose = value;
        }

        public string CacheBoundsKey {
            get => m_cacheBoundsKey;
            set => m_cacheBoundsKey = value;
        }

        [DefaultValue(FormBorderStyle.FixedSingle)]
        public new FormBorderStyle FormBorderStyle {
            get => base.FormBorderStyle;
            set {
                base.FormBorderStyle = value;

                if (value == FormBorderStyle.Sizable
                    || value == FormBorderStyle.SizableToolWindow) {
                    if (m_sizableGrip == null) {
                        m_sizableGrip = new ParamFormGrip();
                        m_sizableGrip.Enabled = false;
                        this.Controls.Add(m_sizableGrip);
                    }
                }
                else {
                    if (m_sizableGrip != null) {
                        this.Controls.Remove(m_sizableGrip);
                        m_sizableGrip = null;
                    }
                }
            }
        }

        protected override void OnLayout(LayoutEventArgs levent) {
            base.OnLayout(levent);

            if (this.FormBorderStyle == FormBorderStyle.Sizable
                    || this.FormBorderStyle == FormBorderStyle.SizableToolWindow) {
                if (m_sizableGrip == null) {
                    m_sizableGrip = new ParamFormGrip();
                    m_sizableGrip.Enabled = false;
                    this.Controls.Add(m_sizableGrip);
                }

                m_sizableGrip.SendToBack();
            }
            else {
                if (m_sizableGrip != null) {
                    this.Controls.Remove(m_sizableGrip);
                    m_sizableGrip = null;
                }
            }
        }

        protected override void OnVisibleChanged(EventArgs e) {
            base.OnVisibleChanged(e);

            if (this.Visible)
                this.ResumeBounds();
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParamFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            if (e.CloseReason == CloseReason.UserClosing && !m_disposeOnClose) {
                e.Cancel = true;
                this.Hide();
            }
        }

        public void ResumeBounds() {
            if (String.IsNullOrEmpty(m_cacheBoundsKey))
                return;
                
            var rc = ParamFormBase._getBound(m_cacheBoundsKey);
            if (rc.Width > 0)
                this.SetBounds(rc.X, rc.Y, rc.Width, rc.Height);
        }

        public new void Focus() {
            base.Focus();

            if (this.WindowState == FormWindowState.Minimized)
                this.WindowState = FormWindowState.Normal;
        }

        public new void Close() {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParamFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            var owner = this.Owner;
            this.Owner = null;
            base.Close();
        }

        public new void Hide() {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParamFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            var owner = this.Owner;
            this.Owner = null;

            this.Visible = false;
            this.Owner = owner;
        }

        public virtual void EnableControlChecks(bool b, Control root = null, System.Windows.Forms.CheckBox[] userChecks = null) {
            if (userChecks != null) {
                for (int i = 0; i < userChecks.Length; ++i) {
                    userChecks[i].Checked = b;
                }
            }

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();

                foreach (Control ctrl in ctrls) {
                    if (ctrl is IParamCtrl) {
                        var chb = ParamFormBase.GetControlCheck((IParamCtrl)ctrl);
                        if (chb != null)
                            chb.Checked = b;
                    }

                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        public static System.Windows.Forms.CheckBox GetControlCheck(IParamCtrl ctrl) {
            if (ctrl == null)
                return null;

            string checkName = ctrl.ControlCheckName;
            if (string.IsNullOrEmpty(checkName))
                return null;

            var par = ((Control)ctrl).Parent;
            while (par != null) {
                var checks = par.Controls.Find(checkName, true);
                foreach (var check in checks) {
                    if (check is System.Windows.Forms.CheckBox)
                        return check as System.Windows.Forms.CheckBox;
                }

                par = par.Parent;
            }

            return null;
        }

        public virtual bool SwitchControlChecks(Control root = null, System.Windows.Forms.CheckBox[] userChecks = null) {
            List<System.Windows.Forms.CheckBox> validChecks = new List<System.Windows.Forms.CheckBox>();
            bool allOff = true;

            if (userChecks != null) {
                for (int i = 0; i < userChecks.Length; ++i) {
                    validChecks.Add(userChecks[i]);
                    allOff &= (!userChecks[i].Checked);
                }
            }

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();

                foreach (Control ctrl in ctrls) {
                    if (ctrl is IParamCtrl) {
                        var chb = ParamFormBase.GetControlCheck((IParamCtrl)ctrl);
                        if (chb != null) {
                            allOff &= (!chb.Checked);
                            validChecks.Add(chb);
                        }
                    }

                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }

            foreach (System.Windows.Forms.CheckBox chb in validChecks) {
                chb.Checked = allOff;
            }

            return allOff;
        }

        public virtual void ClearUI(Control root = null) {
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();

                foreach (Control ctrl in ctrls) {
#if false
                    if (ctrl is System.Windows.Forms.CheckBox) {
                        ((System.Windows.Forms.CheckBox)ctrl).Checked = true;
                    }
                    else if (ctrl is RadioButton) {
                        ((RadioButton)ctrl).Checked = false;
                    }
                    else if (ctrl is RadioButtonGroup) {
                        ((RadioButtonGroup)ctrl).Value = null;
                    }
                    else if (ctrl is TextInput) {
                        ((TextInput)ctrl).Value = "";
                    }
                    else if (ctrl is GuidInput) {
                        ((GuidInput)ctrl).Value = Guid.Empty;
                    }
                    else if (ctrl is DoubleInput) {
                        ((DoubleInput)ctrl).Value = 0.0;
                    }
                    else if (ctrl is IntInput) {
                        ((IntInput)ctrl).Value = 0;
                    }
                    else if (ctrl is LongInput) {
                        ((LongInput)ctrl).Value = 0;
                    }
                    else if (ctrl is IntArrayInput) {
                        ((IntArrayInput)ctrl).Value = null;
                    }
                    else if (ctrl is DoubleArrayInput) {
                        ((DoubleArrayInput)ctrl).Value = null;
                    }
                    else if (ctrl is TextArrayInput) {
                        ((TextArrayInput)ctrl).Value = null;
                    }
                    else if (ctrl is ComboBox) {
                        ((ComboBox)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is KeyValueComboBox) {
                        ((KeyValueComboBox)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ImageComboBoxBase) {
                        ((ImageComboBoxBase)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ColorComboBox) {
                        ((ColorComboBox)ctrl).SelectedColor = 0;
                    }
                    else if (ctrl is TextComboBox) {
                        ((TextComboBox)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is DateComboBox) {
                        ((DateComboBox)ctrl).SelectedDate = 0;
                    }

                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
#else
                    if (ctrl is IParamCtrl) {
                        ((IParamCtrl)ctrl).ClearParameter();
                    } else {
                        if (ctrl.Controls.Count > 0) {
                            stack.Push(ctrl.Controls);
                        }
                    }
#endif
                }
            }
        }

        public virtual void SetPlaceholders(DObject placeholderMap)
        {
            if (placeholderMap == null || placeholderMap.Count == 0)
                return;
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();

                foreach (Control ctrl in ctrls) {
                    bool bHandled = false;
                    do {
                        if (!(ctrl is IParamCtrl) || !(ctrl is IPlaceholderCtrl))
                            break;
                        string paramName = ((IParamCtrl)ctrl).ParamName;
                        if (string.IsNullOrEmpty(paramName) || !placeholderMap.ContainsKey(paramName)) {
                            break;
                        }
                        (ctrl as IPlaceholderCtrl).Placeholder = placeholderMap.Get(paramName);
                        bHandled = true;
                    } while (false);
                    if (bHandled)
                        continue;
                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        public virtual void ClearPlaceholders()
        {
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach (Control ctrl in ctrls) {
                    if (ctrl is IPlaceholderCtrl) {
                        ((IPlaceholderCtrl)ctrl).ClearPlaceholder();
                    }
                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        protected object _getProp(DObject paras, string key) {
            if (paras == null || key == null)
                return null;

            string propName = key.Trim();
            if (propName.Length == 0)
                return null;

            bool neg = (propName[0] == '-');
            if (neg)
                propName = propName.Substring(1);

            //简单属性名
            if (propName.IndexOf('.') < 0) {
                if (paras.ContainsKey(propName))
                    return paras[propName];

                return null;
            }

            //有多级关系
            string[] propNames = propName.Split('.');
            DObject curParas = paras;
            for (int i = 0; i < propNames.Length; ++i) {
                string pname = propNames[i].Trim();
                if (curParas.ContainsKey(pname)) {
                    if (i == propNames.Length - 1) {//value
                        object pval = curParas[pname];
                        if (neg) {//对数值型应用符号
                            if (pval is int)
                                pval = -(int)pval;
                            else if (pval is long)
                                pval = -(long)pval;
                            else if (pval is float)
                                pval = -(float)pval;
                            else if (pval is double)
                                pval = -(double)pval;
                            else if (pval is char)
                                pval = -(char)pval;
                            else if (pval is short)
                                pval = -(short)pval;
                            else if (pval is sbyte)
                                pval = -(sbyte)pval;
                            else if (pval is Vec2i) {
                                Vec2i vec = (Vec2i)pval;
                                pval = new Vec2i(-vec.X, -vec.Y);
                            }
                            else if (pval is Vec3i) {
                                Vec3i vec = (Vec3i)pval;
                                pval = new Vec3i(-vec.X, -vec.Y, -vec.Z);
                            }
                            else if (pval is Vec4i) {
                                Vec4i vec = (Vec4i)pval;
                                pval = new Vec4i(-vec.X, -vec.Y, -vec.Z, -vec.W);
                            }
                            else if (pval is Vec2) {
                                Vec2 vec = (Vec2)pval;
                                pval = new Vec2(-vec.X, -vec.Y);
                            }
                            else if (pval is Vec3) {
                                Vec3 vec = (Vec3)pval;
                                pval = new Vec3(-vec.X, -vec.Y, -vec.Z);
                            }
                            else if (pval is Vec4) {
                                Vec4 vec = (Vec4)pval;
                                pval = new Vec4(-vec.X, -vec.Y, -vec.Z, -vec.W);
                            }
                        }

                        return pval;
                    }
                    else if (curParas[pname] is DObject) {
                        curParas = curParas[pname] as DObject;
                    }
                    else
                        return null;
                }
            }

            return null;
        }

        protected void SetParameter(DObject paras, string key, object value) {
            if (paras == null || key == null)
                return;

            string propName = key.Trim();
            if (propName.Length == 0)
                return;

            bool neg = (propName[0] == '-');
            if (neg)
                propName = propName.Substring(1);

            //简单属性名
            if (propName.IndexOf('.') < 0) {
                paras[propName] = value;
                return;
            }

            //有多级关系
            string[] propNames = propName.Split('.');
            DObject curParas = paras;
            for (int i = 0; i < propNames.Length; ++i) {
                string pname = propNames[i].Trim();
                if (i == propNames.Length - 1) {//value
                    object pval = value;
                    if (neg) {//对数值型应用符号
                        if (pval is int)
                            pval = -(int)pval;
                        else if (pval is long)
                            pval = -(long)pval;
                        else if (pval is float)
                            pval = -(float)pval;
                        else if (pval is double)
                            pval = -(double)pval;
                        else if (pval is char)
                            pval = -(char)pval;
                        else if (pval is short)
                            pval = -(short)pval;
                        else if (pval is sbyte)
                            pval = -(sbyte)pval;
                        else if (pval is Vec2i) {
                            Vec2i vec = (Vec2i)pval;
                            pval = new Vec2i(-vec.X, -vec.Y);
                        }
                        else if (pval is Vec3i) {
                            Vec3i vec = (Vec3i)pval;
                            pval = new Vec3i(-vec.X, -vec.Y, -vec.Z);
                        }
                        else if (pval is Vec4i) {
                            Vec4i vec = (Vec4i)pval;
                            pval = new Vec4i(-vec.X, -vec.Y, -vec.Z, -vec.W);
                        }
                        else if (pval is Vec2) {
                            Vec2 vec = (Vec2)pval;
                            pval = new Vec2(-vec.X, -vec.Y);
                        }
                        else if (pval is Vec3) {
                            Vec3 vec = (Vec3)pval;
                            pval = new Vec3(-vec.X, -vec.Y, -vec.Z);
                        }
                        else if (pval is Vec4) {
                            Vec4 vec = (Vec4)pval;
                            pval = new Vec4(-vec.X, -vec.Y, -vec.Z, -vec.W);
                        }
                    }

                    curParas[pname] = pval;
                }
                else {
                    if (!curParas.ContainsKey(pname) || !(curParas[pname] is DObject))
                        curParas[pname] = new DObject();

                    curParas = curParas[pname] as DObject;
                }
            }
        }

#if false
        public virtual void SetParameters(DObject paras, Control root = null)
        {
            if (paras == null)
                return;

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach (Control ctrl in ctrls) {
                    if (ctrl is IParamCtrl) {
                        IParamCtrl inst = ctrl as IParamCtrl;
                        object pval = this._getProp(paras, inst.ParamName);
                        if (pval != null) {
                            if (ctrl is System.Windows.Forms.CheckBox)
                                ((System.Windows.Forms.CheckBox)ctrl).Checked = (bool)pval;
                            else if (ctrl is RadioButton)
                                ((RadioButton)ctrl).Checked = (((RadioButton)ctrl).RadioTarget == Utils.ToInt(pval));
                            else if (ctrl is RadioButtonGroup)
                                ((RadioButtonGroup)ctrl).Value = pval;
                            else if (ctrl is TextInput) {
                                if (pval is string)
                                    ((TextInput)ctrl).Value = pval as string;
                                else if (pval is Guid)
                                    ((TextInput)ctrl).Value = ((Guid)pval).ToString("N").ToUpper();
                                else if (pval != null)
                                    ((TextInput)ctrl).Value = pval.ToString();
                                else
                                    ((TextInput)ctrl).Value = "";
                            } else if (ctrl is DoubleInput)
                                ((DoubleInput)ctrl).Value = (pval is double) ? (double)pval : 0.0;
                            else if (ctrl is GuidInput) {
                                if (pval is string) {
                                    try {
                                        ((GuidInput)ctrl).Value = new Guid(pval as string);
                                    }
                                    catch {
                                        ((GuidInput)ctrl).Value = Guid.Empty;
                                    }
                                } else if (pval is Guid)
                                    ((GuidInput)ctrl).Value = (Guid)pval;
                            } else if (ctrl is IntInput) {
                                if (pval is int)
                                    ((IntInput)ctrl).Value = (int)pval;
                                else if (pval is double)
                                    ((IntInput)ctrl).Value = (int)(double)pval;
                                else if (pval is long)
                                    ((IntInput)ctrl).Value = (int)(long)pval;
                            } else if (ctrl is LongInput) {
                                if (pval is long)
                                    ((LongInput)ctrl).Value = (long)pval;
                                else if (pval is int)
                                    ((LongInput)ctrl).Value = (long)(int)pval;
                                else if (pval is double)
                                    ((LongInput)ctrl).Value = (long)(double)pval;
                            } else if (ctrl is DoubleArrayInput) {
                                if (pval is double[])
                                    ((DoubleArrayInput)ctrl).Value = (double[])pval;
                                else if (pval is double)
                                    ((DoubleArrayInput)ctrl).Value = new double[] { (double)pval };
                            } else if (ctrl is IntArrayInput) {
                                if (pval is int[])
                                    ((IntArrayInput)ctrl).Value = (int[])pval;
                                else if (pval is int)
                                    ((IntArrayInput)ctrl).Value = new int[] { (int)pval };
                            } else if (ctrl is LongArrayInput) {
                                if (pval is long[])
                                    ((LongArrayInput)ctrl).Value = (long[])pval;
                                else if (pval is long)
                                    ((LongArrayInput)ctrl).Value = new long[] { (long)pval };
                            } else if (ctrl is TextArrayInput) {
                                if (pval is string[])
                                    ((TextArrayInput)ctrl).Value = (string[])pval;
                                else if (pval is string)
                                    ((TextArrayInput)ctrl).Value = new string[] { (string)pval };
                            } else if (ctrl is NumericUpDownInput)
                                ((NumericUpDownInput)ctrl).Value = (int)pval;
                            else if (ctrl is ComboBox) {
                                ComboBox cbo = ctrl as ComboBox;
                                object item = pval;
                                if (cbo.Items.Contains(item))
                                    cbo.SelectedItem = item;
                            } else if (ctrl is KeyValueComboBox)
                                ((KeyValueComboBox)ctrl).SelectedKey = pval;
                            else if (ctrl is ImageComboBoxBase)
                                ((ImageComboBoxBase)ctrl).SelectedKey = pval;
                            else if (ctrl.GetType() == typeof(ColorComboBox)) {
                                if (paras[inst.ParamName] is uint)
                                    ((ColorComboBox)ctrl).SelectedColor = (uint)pval;
                                else if (paras[inst.ParamName] is int)
                                    ((ColorComboBox)ctrl).SelectedColor = (uint)(int)pval;
                                else if (paras[inst.ParamName] is string)
                                    ((ColorComboBox)ctrl).SelectedStrColor = (string)pval;
                                else if (paras[inst.ParamName] is Vec3) {
                                    var val = (Vec3)pval;
                                    uint color = ((uint)(val.X * 255) << 16)
                                                + ((uint)(val.Y * 255) << 8)
                                                + (uint)(val.Z * 255);
                                    ((ColorComboBox)ctrl).SelectedColor = color;
                                }
                            } else if (ctrl is TextComboBox)
                                ((TextComboBox)ctrl).SelectedItem = pval;
                            else if (ctrl.GetType() == typeof(DateComboBox))
                                if (pval is Vec3i) {
                                    Vec3i vec = (Vec3i)pval;
                                    ((DateComboBox)ctrl).SelectedDate = Utils.EpochTm(new DateTime(vec.X, vec.Y, vec.Z));
                                } else if (pval is long
                                      || pval is ulong)
                                    ((DateComboBox)ctrl).SelectedDate = Utils.ToLong(pval);
                        }
                    }

                    if (ctrl.Controls.Count > 0)
                        stack.Push(ctrl.Controls);
                }
            }
        }

#else

        /// <summary>
        /// 设置窗口控件的属性值
        /// </summary>
        /// <param name="paras">源属性</param>
        /// <param name="root">目标根控件，若为null则以窗口为root</param>
        public virtual void SetParameters(DObject paras, Control root = null)
        {
            if (paras == null)
                return;

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            if (root == null || !(root is IParamCtrl)) {
                stack.Push(this.Controls);
            } else {
                IParamCtrl inst = root as IParamCtrl;
                object pval = this._getProp(paras, inst.ParamName);
                if (pval != null) {
                    inst.SetParameter(pval);
                }
            }

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach(Control ctrl in ctrls) {
                    if (ctrl is IParamCtrl) {
                        IParamCtrl inst = ctrl as IParamCtrl;
                        object pval = this._getProp(paras, inst.ParamName);
                        if (pval != null) {
                            inst.SetParameter(pval);
                        }
                    }
                    else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

#endif

#if false
        public virtual DObject GetParameters(Control root = null, List<Control> exclude = null, bool check = true)
        {
            DObject retParams = new DObject();

            if (root != null)
                this.FillControlParameter(retParams, root, exclude, check);

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);
            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach (Control ctrl in ctrls) {
                    this.FillControlParameter(retParams, ctrl, exclude, check);
                    if (ctrl.Controls.Count > 0)
                        stack.Push(ctrl.Controls);
                }
            }

            return retParams;
        }

#else
        public virtual DObject GetParameters(Control root = null, List<Control> exclude = null, bool bConsiderCheck = true)
        {
            DObject retParams = new DObject();
            if (root == null || !(root is IParamCtrl)) {
                // 收集属性
                Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
                stack.Push(root == null ? this.Controls : root.Controls);
                while (stack.Count > 0) {
                    Control.ControlCollection ctrls = stack.Pop();
                    foreach (Control ctrl in ctrls) {
                        if (exclude != null && exclude.IndexOf(ctrl) >= 0)
                            continue;

                        if (ctrl is IParamCtrl) {
                            IParamCtrl inst = ctrl as IParamCtrl;
                            if (bConsiderCheck) {
                                var instCheck = GetControlCheck(inst);
                                if (instCheck != null && !instCheck.Checked) {
                                    continue;
                                }
                            }
                            object pval = inst.GetParameter(exclude, bConsiderCheck);
                            if (pval != null) {
                                retParams[inst.ParamName] = pval;
                            }
                        }
                        else if (ctrl.Controls.Count > 0) {
                            stack.Push(ctrl.Controls);
                        }
                    }
                }
            } else {
                do {
                    if (exclude != null && exclude.IndexOf(root) >= 0)
                        break;

                    IParamCtrl inst = root as IParamCtrl;
                    if (bConsiderCheck) {
                        var instCheck = GetControlCheck(inst);
                        if (instCheck != null && !instCheck.Checked) {
                            break;
                        }
                    }
                    object pval = inst.GetParameter(exclude, bConsiderCheck);
                    if (pval == null) {
                        break;
                    }

                    retParams[inst.ParamName] = pval;
                } while (false);
            }

            return retParams;
        }

#endif

        public virtual void UpdateDispUnitEnob(DObject options) {
            Stack<Control.ControlCollection> ctrls = new Stack<Control.ControlCollection>();
            ctrls.Push(this.Controls);

            while (ctrls.Count > 0) {
                var collect = ctrls.Pop();

                foreach (Control ctrl in collect) {
                    if (ctrl is InputBase) {
                        var input = ctrl as InputBase;
                        input.UpdateDispUnitEnob(options);
                        continue;
                    }

                    if (ctrl is TabControl
                        || ctrl is TabPage
                        || ctrl is GroupBox
                        || ctrl is System.Windows.Forms.Panel
                        || ctrl is UserControl) {
                        ctrls.Push(ctrl.Controls);
                    }
                }
            }
        }

        public virtual void InvokeAcceptButton() {
            if (this.AcceptButton == null)
                return;

            this.AcceptButton.PerformClick();
        }

        public void InvokeEscClose() {
            if (!m_escClose)
                return;

            if (!m_disposeOnClose)
                this.Hide();
            else
                this.Close();
        }

        protected override bool ProcessDialogKey(Keys key) {
            if (Form.ModifierKeys != Keys.None)
                return base.ProcessDialogKey(key);

            switch (key) {
                case Keys.Escape: 
                this.InvokeEscClose();
                return true;
                case Keys.Tab:
                this.MoveTabToNext();
                return true;
            }

            return base.ProcessDialogKey(key);
        }

        public void MoveTabToNext() {
            for (int i = 0; i < m_tabList.Count; ++i) {
                var ctrl = m_tabList[i];
                if (ctrl.Focused) {
                    var par = m_tabParList[i];
                    
                    //寻找下一个同TAB容器的控件
                    for (int j = i + 1; j < m_tabList.Count; ++j) {
                        var next = m_tabList[j];
                        if (!next.Visible 
                            || !next.TabStop 
                            || next is RadioButton /*Radio设置值时TabStop会自动设为true，所以忽略所有Radio控件*/
                            || next is RadioButtonGroup)
                            continue;

                        var npar = m_tabParList[j];
                        bool bSamePar = false;
                        while (npar != null) {
                            if (npar == par) {
                                bSamePar = true;
                                break;
                            }

                            npar = npar.Parent;
                        }

                        if (!bSamePar)
                            break;

                        next.Select();
                        return;
                    }

                    //已到结尾，从当前TAB容器头循环
                    int idxp = m_tabList.IndexOf(par);
                    if (idxp < 0)
                        return;

                    for (int j = idxp + 1; j < m_tabList.Count; ++j) {
                        var next = m_tabList[j];
                        if (!next.Visible || !next.TabStop)
                            continue;

                        var npar = m_tabParList[j];
                        bool bSamePar = false;
                        while (npar != null) {
                            if (npar == par) {
                                bSamePar = true;
                                break;
                            }

                            npar = npar.Parent;
                        }

                        if (!bSamePar)
                            break;

                        next.Select();
                        return;
                    }

                    return;
                }
            }

        }

        public void UpdateTabOrder() {
            m_tabList.Clear();
            m_tabParList.Clear();
            m_tabList.Add(this);
            m_tabParList.Add(null);

            _updateChildTabOrder(this, this);
        }

        protected void _updateChildTabOrder(Control root, Control tabPar) {
            if (root is TabControl) {
                var tb = root as TabControl;
                foreach (TabPage page in tb.TabPages) {
                    m_tabList.Add(page);
                    m_tabParList.Add(tb);
                    _updateChildTabOrder(page, page);
                }
                return;
            }

            //calculate group location            
            var groupRects = new Dictionary<string, Rectangle>();
            foreach (Control ctrl in root.Controls) {
                if (!(ctrl is ITabOrderCtrl))
                    continue;

                var usrCtrl = ctrl as ITabOrderCtrl;
                if (String.IsNullOrEmpty(usrCtrl.TabGroup))
                    continue;

                if (!groupRects.ContainsKey(usrCtrl.TabGroup)) {
                    groupRects[usrCtrl.TabGroup] = ctrl.Bounds;
                    continue;
                }

                var rect = ctrl.Bounds;
                var grect = groupRects[usrCtrl.TabGroup];
                var loc0 = new Point(Math.Min(grect.X, rect.X), Math.Min(grect.Y, rect.Y));
                var loc1 = new Point(Math.Max(grect.Right, rect.Right), Math.Max(grect.Bottom, rect.Bottom));
                groupRects[usrCtrl.TabGroup] = new Rectangle(loc0.X, loc0.Y, loc1.X - loc0.X, loc1.Y - loc0.Y);
            }

            var ctrls = new List<Control>();
            foreach (Control ctrl in root.Controls)
                ctrls.Add(ctrl);
            
            ctrls.Sort(delegate (Control a, Control b) {
                string ga = a is ITabOrderCtrl ? (a as ITabOrderCtrl).TabGroup : "";
                string gb = b is ITabOrderCtrl ? (b as ITabOrderCtrl).TabGroup : "";
                Rectangle recta, rectb;

                if (ga == gb) {
                    recta = a.Bounds;
                    rectb = b.Bounds;
                }
                else {
                    recta = String.IsNullOrEmpty(ga) ? a.Bounds : groupRects[ga];
                    rectb = String.IsNullOrEmpty(gb) ? b.Bounds : groupRects[gb];
                }

                bool interx = Math.Max(recta.X, rectb.X) < Math.Min(recta.Right, rectb.Right);
                bool intery = Math.Max(recta.Y, rectb.Y) < Math.Min(recta.Bottom, rectb.Bottom);

                if (interx == intery) { //true/true or false/false
                    int cx = a.Location.X - b.Location.X;
                    int cy = a.Location.Y - b.Location.Y;

                    if (cx == 0 && cy == 0)
                        return 0;

                    if (cy != 0)
                        return cy < 0 ? -1 : 1;

                    return cx < 0 ? -1 : 1;
                }
                else if (interx) {
                    int cy = a.Location.Y - b.Location.Y;
                    if (cy == 0)
                        return 0;

                    return cy < 0 ? -1 : 1;
                }
                else if (intery) {
                    int cx = a.Location.X - b.Location.X;
                    if (cx == 0)
                        return 0;

                    return cx < 0 ? -1 : 1;
                }

                return 0;
            });

            foreach (var ctrl in ctrls) {
                if (ctrl is Label)
                    continue;

                m_tabList.Add(ctrl);
                m_tabParList.Add(tabPar);

                if (ctrl.Controls.Count > 0)
                    _updateChildTabOrder(ctrl, tabPar);
            }
        }

        protected static void _setBound(string key, Rectangle rect) {
            m_formBounds[key] = rect;
        }

        protected static Rectangle _getBound(string key) {
            if (m_formBounds.ContainsKey(key))
                return m_formBounds[key];

            return new Rectangle(0, 0, 0, 0);
        }

        public void InitStartPosition(IntPtr handle, Rectangle rcWork, float posH, float posV, int rand = 25) {
            var rd = new Random();
            var rx = rd.Next(rand * 2) - rand;
            var ry = rd.Next(rand * 2) - rand;
            int x = (int)Math.Max(3, Math.Min(rcWork.Left + rcWork.Width * posH + rx, rcWork.Right - 3 - this.Width));
            int y = (int)Math.Max(3, Math.Min(rcWork.Top + rcWork.Height * posV + ry, rcWork.Bottom - 3 - this.Height));
            this.Location = new Point(x, y);
        }
    }
}
