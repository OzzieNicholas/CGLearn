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

namespace UserCtrl
{
    public class ParaFormBase : Form
    {
        protected bool m_disposeOnClose = false;
        protected bool m_escClose = true;
        protected bool m_syncOwnedVisibility = false;
        protected string m_cacheBoundsKey = null;

        //undo/redo
        protected Dictionary<string, RvtPool> m_rvtPools = new Dictionary<string, RvtPool>();
        protected ParaFormGrip m_sizableGrip = null;

        //tab switch
        protected List<Control> m_tabList = new List<Control>();
        protected List<Control> m_tabParList = new List<Control>();

        //bounds cache
        protected static Dictionary<string, Rectangle> m_formBounds = new Dictionary<string, Rectangle>();

        public ParaFormBase() {
            this.DoubleBuffered = true;
            this.AllowTransparency = false;
            this.ShowIcon = false;
            this.KeyPreview = true;
            //this.Icon = Properties.Resources.default_icon;
        }

        [Browsable(false), DefaultValue(false)]
        public bool disposeOnClose {
            get => m_disposeOnClose;
            set => m_disposeOnClose = value;
        }

        public virtual bool acceptEscClose {
            get => m_escClose;
            set => m_escClose = value;
        }

        public string cacheBoundsKey {
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
                        m_sizableGrip = new ParaFormGrip();
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
                    m_sizableGrip = new ParaFormGrip();
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
                this.resumeBounds();
        }

        public virtual void onAppInit() {
        
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParaFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            if (e.CloseReason == CloseReason.UserClosing && !m_disposeOnClose) {
                e.Cancel = true;
                this.hide();
            }
        }

        public void resumeBounds() {
            if (String.IsNullOrEmpty(m_cacheBoundsKey))
                return;
                
            var rc = ParaFormBase._getBound(m_cacheBoundsKey);
            if (rc.Width > 0)
                this.SetBounds(rc.X, rc.Y, rc.Width, rc.Height);
        }

        public void focus() {
            this.Focus();

            if (this.WindowState == FormWindowState.Minimized)
                this.WindowState = FormWindowState.Normal;
        }

        public void close() {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParaFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            var owner = this.Owner;
            this.Owner = null;
            
            this.Close();
        }

        public virtual void hide() {
            if (!String.IsNullOrEmpty(m_cacheBoundsKey)) {
                ParaFormBase._setBound(m_cacheBoundsKey, this.Bounds);
            }

            var owner = this.Owner;
            this.Owner = null;

            this.Visible = false;
            this.Owner = owner;
        }

        public void listenVisible(Form refForm)
        {
            if (refForm != null) {
                refForm.VisibleChanged += new EventHandler(this.referenceVisibleChanged);
            }
        }

        public virtual void performAcceptButton() {
            if (this.AcceptButton == null)
                return;

            this.AcceptButton.PerformClick();
        }

        public virtual void setChecks(bool b, Control root = null, UserCtrl.Check[] userChecks = null) {
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
                    if (ctrl is IUserCtrl) {
                        var chb = this.checkCtrl((IUserCtrl)ctrl);
                        if (chb != null)
                            chb.Checked = b;
                    }

                    if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        public CheckBox checkCtrl(IUserCtrl ctrl) {
            if (ctrl == null)
                return null;

            string checkName = ctrl.CheckName;
            if (string.IsNullOrEmpty(checkName))
                return null;

            var par = ((Control)ctrl).Parent;
            while (par != null) {
                var checks = par.Controls.Find(checkName, true);
                foreach (var check in checks) {
                    if (check is CheckBox)
                        return check as CheckBox;
                }

                par = par.Parent;
            }

            return null;
        }

        public virtual bool switchChecks(Control root = null, UserCtrl.Check[] userChecks = null) {
            List<CheckBox> validChecks = new List<CheckBox>();
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
                    if (ctrl is IUserCtrl) {
                        var chb = checkCtrl((IUserCtrl)ctrl);
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

            foreach (CheckBox chb in validChecks) {
                chb.Checked = allOff;
            }

            return allOff;
        }

        public void clearUI(Control root = null) {
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();

                foreach (Control ctrl in ctrls) {
                    if (ctrl is Check) {
                        ((Check)ctrl).Checked = true;
                    }
                    else if (ctrl is Radio) {
                        ((Radio)ctrl).Checked = false;
                    }
                    else if (ctrl is RadioButtons) {
                        ((RadioButtons)ctrl).Value = null;
                    }
                    else if (ctrl is InputText) {
                        ((InputText)ctrl).Value = "";
                    }
                    else if (ctrl is InputGuid) {
                        ((InputGuid)ctrl).Value = Guid.Empty;
                    }
                    else if (ctrl is InputDouble) {
                        ((InputDouble)ctrl).Value = 0.0;
                    }
                    else if (ctrl is InputInt) {
                        ((InputInt)ctrl).Value = 0;
                    }
                    else if (ctrl is InputLong) {
                        ((InputLong)ctrl).Value = 0;
                    }
                    else if (ctrl is InputMark) {
                        ((InputMark)ctrl).Value = null;
                    }
                    else if (ctrl is InputObject) {
                        ((InputObject)ctrl).Value = null;
                    }
                    else if (ctrl is InputIntArrayScript) {
                        ((InputIntArrayScript)ctrl).Value = null;
                    }
                    else if (ctrl is InputDoubleArrayScript) {
                        ((InputDoubleArrayScript)ctrl).Value = null;
                    }
                    else if (ctrl is InputTextArray) {
                        ((InputTextArray)ctrl).Value = null;
                    }
                    else if (ctrl is Combo) {
                        ((Combo)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ComboBoxText) {
                        ((ComboBoxText)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ComboBoxKey) {
                        ((ComboBoxKey)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ComboBoxImage) {
                        ((ComboBoxImage)ctrl).SelectedItem = null;
                    }
                    else if (ctrl is ComboBoxColor) {
                        ((ComboBoxColor)ctrl).SelectedColor = 0;
                    }
                    else if (ctrl is ComboBoxStampLine) {
                        ((ComboBoxStampLine)ctrl).SelectedItem = Define.DRLineType.DRLINE_TYPE0;
                    }
                    else if (ctrl is ComboBoxArrowShape) {
                        ((ComboBoxArrowShape)ctrl).SelectedItem = Define.DRArrowShape.DRARROW_NONE;
                    }
                    else if (ctrl is ComboBoxFrameShape) {
                        ((ComboBoxFrameShape)ctrl).SelectedItem = Define.DRTextFrame.DRTEXT_FRAME0;
                    }
                    else if (ctrl is ComboBoxDate) {
                        ((ComboBoxDate)ctrl).SelectedDate = 0;
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

        protected void _saveProp(DObject paras, string key, object value) {
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

        public virtual void updateUIByParas(DObject paras, Control root = null) {
            if (paras == null)
                return;

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach (Control ctrl in ctrls) {
                    if (ctrl is IUserCtrl) {
                        IUserCtrl inst = ctrl as IUserCtrl;
                        object pval = this._getProp(paras, inst.PropName);
                        if (pval != null) {
                            if (ctrl is Check)
                                ((Check)ctrl).Checked = (bool)pval;
                            else if (ctrl is Radio)
                                ((Radio)ctrl).Checked = (((Radio)ctrl).PropTarget == Utils.ToInt(pval));
                            else if (ctrl is RadioButtons)
                                ((RadioButtons)ctrl).Value = pval;
                            else if (ctrl is InputText) {
                                if (pval is string)
                                    ((InputText)ctrl).Value = pval as string;
                                else if (pval is Guid)
                                    ((InputText)ctrl).Value = ((Guid)pval).ToString("N").ToUpper();
                                else if (pval != null)
                                    ((InputText)ctrl).Value = pval.ToString();
                                else
                                    ((InputText)ctrl).Value = "";
                            }
                            else if (ctrl is InputDouble)
                                ((InputDouble)ctrl).Value = (pval is double) ? (double)pval : 0.0;
                            else if (ctrl is InputGuid) {
                                if (pval is string) {
                                    try {
                                        ((InputGuid)ctrl).Value = new Guid(pval as string);
                                    }
                                    catch {
                                        ((InputGuid)ctrl).Value = Guid.Empty;
                                    }
                                }
                                else if (pval is Guid)
                                    ((InputGuid)ctrl).Value = (Guid)pval;
                            }
                            else if (ctrl is InputInt) {
                                if (pval is int)
                                    ((InputInt)ctrl).Value = (int)pval;
                                else if (pval is double)
                                    ((InputInt)ctrl).Value = (int)(double)pval;
                                else if (pval is long)
                                    ((InputInt)ctrl).Value = (int)(long)pval;
                            }
                            else if (ctrl is InputLong) {
                                if (pval is long)
                                    ((InputLong)ctrl).Value = (long)pval;
                                else if (pval is int)
                                    ((InputLong)ctrl).Value = (long)(int)pval;
                                else if (pval is double)
                                    ((InputLong)ctrl).Value = (long)(double)pval;
                            }
                            else if (ctrl is InputMark)
                                ((InputMark)ctrl).Value = pval as DObject;
                            else if (ctrl is InputObject)
                                ((InputObject)ctrl).Value = pval as DObject;
                            else if (ctrl is InputDoubleArrayScript) {
                                if (pval is double[])
                                    ((InputDoubleArrayScript)ctrl).Value = (double[])pval;
                                else if (pval is double)
                                    ((InputDoubleArrayScript)ctrl).Value = new double[] { (double)pval };
                            }
                            else if (ctrl is InputIntArrayScript) {
                                if (pval is int[])
                                    ((InputIntArrayScript)ctrl).Value = (int[])pval;
                                else if (pval is int)
                                    ((InputIntArrayScript)ctrl).Value = new int[] { (int)pval };
                            }
                            else if (ctrl is InputLongArrayScript) {
                                if (pval is long[])
                                    ((InputLongArrayScript)ctrl).Value = (long[])pval;
                                else if (pval is long)
                                    ((InputLongArrayScript)ctrl).Value = new long[] { (long)pval };
                            }
                            else if (ctrl is InputTextArray) {
                                if (pval is string[])
                                    ((InputTextArray)ctrl).Value = (string[])pval;
                                else if (pval is string)
                                    ((InputTextArray)ctrl).Value = new string[] { (string)pval };
                            }
                            else if (ctrl is InputNumericUpDown)
                                ((InputNumericUpDown)ctrl).Value = (int)pval;
                            else if (ctrl is Combo) {
                                Combo cbo = ctrl as Combo;
                                cbo.SelectedKey = pval;
                            }
                            else if (ctrl is ComboBoxText) {
                                ComboBoxText cbo = ctrl as ComboBoxText;
                                object item = pval;
                                if (cbo.Items.Contains(item))
                                    cbo.SelectedItem = item;
                            }
                            else if (ctrl is ComboBoxKey)
                                ((ComboBoxKey)ctrl).SelectedKey = pval;
                            else if (ctrl.GetType() == typeof(ComboBoxImage))
                                ((ComboBoxImage)ctrl).SelectedKey = pval;
                            else if (ctrl.GetType() == typeof(ComboBoxColor)) {
                                if (paras[inst.PropName] is uint)
                                    ((ComboBoxColor)ctrl).SelectedColor = (uint)pval;
                                else if (paras[inst.PropName] is int)
                                    ((ComboBoxColor)ctrl).SelectedColor = (uint)(int)pval;
                                else if (paras[inst.PropName] is string)
                                    ((ComboBoxColor)ctrl).SelectedStrColor = (string)pval;
                                else if (paras[inst.PropName] is Vec3) {
                                    var val = (Vec3)pval;
                                    uint color = ((uint)(val.X * 255) << 16)
                                                + ((uint)(val.Y * 255) << 8)
                                                + (uint)(val.Z * 255);
                                    ((ComboBoxColor)ctrl).SelectedColor = color;
                                }
                            }
                            else if (ctrl.GetType() == typeof(ComboBoxStampLine))
                                ((ComboBoxStampLine)ctrl).SelectedItem = (Define.DRLineType)pval;
                            else if (ctrl.GetType() == typeof(ComboBoxArrowShape))
                                ((ComboBoxArrowShape)ctrl).SelectedShape = pval;
                            else if (ctrl.GetType() == typeof(ComboBoxFrameShape))
                                ((ComboBoxFrameShape)ctrl).SelectedShape = pval;
                            else if (ctrl.GetType() == typeof(ComboBoxDate))
                                if (pval is Vec3i) {
                                    Vec3i vec = (Vec3i)pval;
                                    ((ComboBoxDate)ctrl).SelectedDate = Utils.EpochTm(new DateTime(vec.X, vec.Y, vec.Z));
                                }
                                else if (pval is long
                                    || pval is ulong)
                                    ((ComboBoxDate)ctrl).SelectedDate = Utils.ToLong(pval);
                        }
                    }

                    if (ctrl.Controls.Count > 0)
                        stack.Push(ctrl.Controls);
                }
            }
        }

        protected virtual bool _saveCtrlProp(DObject paras, Control ctrl, List<Control> exclude, bool check) {
            if (!ctrl.Enabled)
                return false;

            if (exclude != null && exclude.IndexOf(ctrl) >= 0)
                return false;

            if (!(ctrl is IUserCtrl))
                return false;
                
            IUserCtrl inst = ctrl as IUserCtrl;

            if (check) {
                var chb = this.checkCtrl(inst);
                if (chb != null && chb.Checked == false)
                    return false;
            }

            if (inst.PropName != null && inst.PropName != "") {
                if (ctrl is Check)
                    _saveProp(paras, inst.PropName, ((Check)ctrl).Checked);
                else if (ctrl is Radio && ((Radio)ctrl).Checked)
                    _saveProp(paras, inst.PropName, ((Radio)ctrl).PropTarget);
                else if (ctrl is RadioButtons)
                    _saveProp(paras, inst.PropName, ((RadioButtons)ctrl).Value);
                else if (ctrl is InputText)
                    _saveProp(paras, inst.PropName, ((InputText)ctrl).Value);
                else if (ctrl is InputDouble)
                    _saveProp(paras, inst.PropName, ((InputDouble)ctrl).Value);
                else if (ctrl is InputInt)
                    _saveProp(paras, inst.PropName, ((InputInt)ctrl).Value);
                else if (ctrl is InputLong)
                    _saveProp(paras, inst.PropName, ((InputLong)ctrl).Value);
                else if (ctrl is InputGuid)
                    _saveProp(paras, inst.PropName, ((InputGuid)ctrl).Value);
                else if (ctrl is InputMark)
                    _saveProp(paras, inst.PropName, ((InputMark)ctrl).Value);
                else if (ctrl is InputObject)
                    _saveProp(paras, inst.PropName, ((InputObject)ctrl).Value.Clone());
                else if (ctrl is InputIntArrayScript)
                    _saveProp(paras, inst.PropName, ((InputIntArrayScript)ctrl).Value);
                else if (ctrl is InputLongArrayScript)
                    _saveProp(paras, inst.PropName, ((InputLongArrayScript)ctrl).Value);
                else if (ctrl is InputDoubleArrayScript)
                    _saveProp(paras, inst.PropName, ((InputDoubleArrayScript)ctrl).Value);
                else if (ctrl is InputTextArray)
                    _saveProp(paras, inst.PropName, ((InputTextArray)ctrl).Value);
                else if (ctrl is InputNumericUpDown)
                    _saveProp(paras, inst.PropName, (int)((InputNumericUpDown)ctrl).Value);
                else if (ctrl is Combo && ((Combo)ctrl).SelectedKey != null)
                    _saveProp(paras, inst.PropName, ((Combo)ctrl).SelectedKey);
                else if (ctrl is ComboBoxText && ((ComboBoxText)ctrl).SelectedItem != null)
                    _saveProp(paras, inst.PropName, ((ComboBoxText)ctrl).SelectedItem);
                else if (ctrl is ComboBoxKey && ((ComboBoxKey)ctrl).SelectedKey != null)
                    _saveProp(paras, inst.PropName, ((ComboBoxKey)ctrl).SelectedKey);
                else if (ctrl.GetType() == typeof(ComboBoxImage))
                    _saveProp(paras, inst.PropName, ((ComboBoxImage)ctrl).SelectedKey);
                else if (ctrl.GetType() == typeof(ComboBoxColor))
                    _saveProp(paras, inst.PropName, ((ComboBoxColor)ctrl).SelectedColor);
                else if (ctrl.GetType() == typeof(ComboBoxStampLine))
                    _saveProp(paras, inst.PropName, ((ComboBoxStampLine)ctrl).SelectedItem);
                else if (ctrl.GetType() == typeof(ComboBoxArrowShape))
                    _saveProp(paras, inst.PropName, ((ComboBoxArrowShape)ctrl).SelectedShape);
                else if (ctrl.GetType() == typeof(ComboBoxFrameShape))
                    _saveProp(paras, inst.PropName, ((ComboBoxFrameShape)ctrl).SelectedShape);
                else if (ctrl.GetType() == typeof(ComboBoxDate))
                    _saveProp(paras, inst.PropName, ((ComboBoxDate)ctrl).SelectedDate);
            }

            return true;
        }

        public virtual DObject getParasByUI(Control root = null, List<Control> exclude = null, bool check = true) {
            DObject paras = new DObject();

            if (root != null) 
                this._saveCtrlProp(paras, root, exclude, check);

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(root == null ? this.Controls : root.Controls);
            while (stack.Count > 0) {
                Control.ControlCollection ctrls = stack.Pop();
                foreach (Control ctrl in ctrls) {
                    this._saveCtrlProp(paras, ctrl, exclude, check);
                    if (ctrl.Controls.Count > 0)
                        stack.Push(ctrl.Controls);
                }
            }

            return paras;
        }

        public bool isRvtFreezed(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return false;

            return pool.isRvtFreezed();
        }

        public void freezeRvt(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.freezeRvt();
        }

        public void unfreezeRvt(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.unfreezeRvt();
        }

        public void newRvtPool(string name) {
            if (m_rvtPools.ContainsKey(name))
                return;

            m_rvtPools[name] = new RvtPool();
        }

        public void clearRvts() {
            foreach (var it in m_rvtPools)
                it.Value.clearRvts();
        }

        public void clearRvt(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.clearRvts();
        }

        public void beginRvtQueue(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.beginRvtQueue();
        }
        public void pushRvt(string name, Revertable pRvt) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.pushRvt(pRvt);
        }

        public void endRvtQueue(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.endRvtQueue();
        }

        public virtual void undo(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.undo();
        }

        public virtual void redo(string name) {
            RvtPool pool;
            if (!m_rvtPools.TryGetValue(name, out pool))
                return;

            pool.redo();
        }

        private void InitializeComponent() {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ParaFormBase));
            this.SuspendLayout();
            // 
            // ParaForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(226, 202);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ParaFormBase";
            this.ResumeLayout(false);

        }

        public virtual void updateDispUnitEnob(DObject options) {
            Stack<Control.ControlCollection> ctrls = new Stack<Control.ControlCollection>();
            ctrls.Push(this.Controls);

            while (ctrls.Count > 0) {
                var collect = ctrls.Pop();

                foreach (Control ctrl in collect) {
                    if (ctrl is InputBase) {
                        var input = ctrl as InputBase;
                        input.updateDispUnitEnob(options);
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

        public void performEscClose() {
            if (!m_escClose)
                return;

            if (!m_disposeOnClose)
                this.hide();
            else
                this.close();
        }

        protected override bool ProcessDialogKey(Keys key) {
            if (Form.ModifierKeys != Keys.None)
                return base.ProcessDialogKey(key);

            switch (key) {
                case Keys.Escape:
                this.performEscClose();
                return true;
                case Keys.Tab:
                this.performTabSwitch();
                return true;
            }

            return base.ProcessDialogKey(key);
        }

        public void performTabSwitch() {
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
                            || next is RadioButtons)
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

        public void updateTabOrder() {
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
                if (!(ctrl is IUserCtrl))
                    continue;

                var usrCtrl = ctrl as IUserCtrl;
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
                string ga = a is IUserCtrl ? (a as IUserCtrl).TabGroup : "";
                string gb = b is IUserCtrl ? (b as IUserCtrl).TabGroup : "";
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

        public void referenceVisibleChanged(object sender, EventArgs e)
        {
            if (sender == null || !(sender is Form)) {
                return;
            }

            if (!((Form)sender).Visible) 
                this.hide();
        }

        protected static void _setBound(string key, Rectangle rect) {
            m_formBounds[key] = rect;
        }

        protected static Rectangle _getBound(string key) {
            if (m_formBounds.ContainsKey(key))
                return m_formBounds[key];

            return new Rectangle(0, 0, 0, 0);
        }

        public void initStartPosition(IntPtr handle, Rectangle rcWork, float posH, float posV, int rand = 25) {
            var rd = new Random();
            var rx = rd.Next(rand * 2) - rand;
            var ry = rd.Next(rand * 2) - rand;
            int x = (int)Math.Max(3, Math.Min(rcWork.Left + rcWork.Width * posH + rx, rcWork.Right - 3 - this.Width));
            int y = (int)Math.Max(3, Math.Min(rcWork.Top + rcWork.Height * posV + ry, rcWork.Bottom - 3 - this.Height));
            this.Location = new Point(x, y);
            
        }
    }
}
