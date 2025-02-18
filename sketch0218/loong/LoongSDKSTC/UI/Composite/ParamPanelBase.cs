using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace LoongCAD.UI
{

    /// <summary>
    /// 带属性的复合面板控件基类
    /// </summary>
    public partial class ParamPanelBase : UserControl, IParamCtrl, IPlaceholderCtrl
    {

        private string m_paramName = "";

        private string m_controlCheckName = "";

        /// <summary>
        /// 所有子属性的占位符属性
        /// </summary>
        protected LoongCAD.Util.DObject m_placeholder;

        /// <summary>
        /// 默认构造函数
        /// </summary>
        public ParamPanelBase()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 复合参数名称
        /// </summary>
        public string ParamName { get => m_paramName; set => m_paramName = value; }

        /// <summary>
        /// 复合参数是否启用的CheckBox控件名称
        /// </summary>
        public string ControlCheckName { get => m_controlCheckName; set => m_controlCheckName = value; }

        /// <summary>
        /// 水印数据
        /// </summary>
        public virtual object Placeholder
        {
            get => m_placeholder;
            set
            {
                if (value is LoongCAD.Util.DObject) {
                    m_placeholder = (LoongCAD.Util.DObject)value;

                    // 设置子属性的Placeholder
                    Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
                    stack.Push(this.Controls);
                    while (stack.Count > 0) {
                        Control.ControlCollection subCtrls = stack.Pop();
                        foreach (Control ctrl in subCtrls) {
                            if (ctrl is IParamCtrl) {
                                IPlaceholderCtrl inst = ctrl as IPlaceholderCtrl;
                                string subParamName = (ctrl as IParamCtrl).ParamName;
                                if (inst != null && m_placeholder.ContainsKey(subParamName)) {
                                    inst.Placeholder = m_placeholder.Get(subParamName);
                                }
                            } else if (ctrl.Controls.Count > 0) {
                                stack.Push(ctrl.Controls);
                            }
                        }
                    }
                } else {
                    this.ClearPlaceholder();
                }
            }
        }

        /// <inheritdoc/>
        public void SetParameter(object value)
        {
            LoongCAD.Util.DObject newParams = value as LoongCAD.Util.DObject;
            if (newParams == null) {
                return;
            }
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection subCtrls = stack.Pop();
                foreach (Control ctrl in subCtrls) {
                    IParamCtrl inst = ctrl as IParamCtrl;
                    if (inst != null && !string.IsNullOrEmpty(inst.ParamName)) {
                        if (newParams.ContainsKey(inst.ParamName)) {
                            inst.SetParameter(newParams.Get(inst.ParamName));
                        }
                    } else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        /// <summary>
        /// 获取参数
        /// </summary>
        /// <returns></returns>
        public virtual object GetParameter()
        {
            LoongCAD.Util.DObject retParams = new LoongCAD.Util.DObject();

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection subCtrls = stack.Pop();
                foreach (Control ctrl in subCtrls) {
                    IParamCtrl inst = ctrl as IParamCtrl;
                    if (inst != null && !string.IsNullOrEmpty(inst.ParamName)) {
                        object subParam = inst.GetParameter();
                        if (subParam != null) {
                            retParams[inst.ParamName] = subParam;
                        }
                    } else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }

            return retParams;
        }

        /// <summary>
        /// 获取参数
        /// </summary>
        /// <param name="exclude">排除的子属性控件</param>
        /// <param name="check">是否只包含启用的控件</param>
        /// <returns></returns>
        public virtual object GetParameter(List<Control> exclude, bool check)
        {
            LoongCAD.Util.DObject retParams = new LoongCAD.Util.DObject();

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);

            while (stack.Count > 0) {
                Control.ControlCollection subCtrls = stack.Pop();
                foreach (Control ctrl in subCtrls) {
                    if (exclude != null && exclude.IndexOf(this) >= 0) {
                        continue;
                    }
                    IParamCtrl inst = ctrl as IParamCtrl;
                    if (check) {
                        var instCheck = ParamFormBase.GetControlCheck(inst);
                        if (instCheck != null && !instCheck.Checked)
                            continue;
                    }
                    if (inst != null && !string.IsNullOrEmpty(inst.ParamName)) {
                        object subParam = inst.GetParameter(exclude, check);
                        if (subParam != null) {
                            retParams[inst.ParamName] = subParam;
                        }
                    } else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }

            return retParams;
        }

        /// <summary>
        /// 清空参数
        /// </summary>
        public virtual void ClearParameter()
        {
            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);
            while (stack.Count > 0) {
                Control.ControlCollection subCtrls = stack.Pop();
                foreach (Control ctrl in subCtrls) {
                    IParamCtrl inst = ctrl as IParamCtrl;
                    if (inst != null) {
                        inst.ClearParameter();
                    } else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

        /// <summary>
        /// 清空占位符
        /// </summary>
        public virtual void ClearPlaceholder()
        {
            m_placeholder = null;

            Stack<Control.ControlCollection> stack = new Stack<Control.ControlCollection>();
            stack.Push(this.Controls);
            while (stack.Count > 0) {
                Control.ControlCollection subCtrls = stack.Pop();
                foreach (Control ctrl in subCtrls) {
                    if (ctrl is IParamCtrl) {
                        IPlaceholderCtrl inst = ctrl as IPlaceholderCtrl;
                        if (inst != null) {
                            inst.ClearPlaceholder();
                        }
                    } else if (ctrl.Controls.Count > 0) {
                        stack.Push(ctrl.Controls);
                    }
                }
            }
        }

    }

}
