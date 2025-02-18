using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{

    /// <summary>
    /// 控件参数接口
    /// </summary>
    public interface IParamCtrl
    {

        /// <summary>
        /// 参数名称
        /// </summary>
        string ParamName { get; set; }

        /// <summary>
        /// 控制参数是否启用的CheckBox名称
        /// </summary>
        string ControlCheckName { get; set; }

        /// <summary>
        /// 设置参数
        /// </summary>
        /// <param name="value"></param>
        void SetParameter(object value);

        /// <summary>
        /// 获取参数
        /// </summary>
        /// <returns></returns>
        object GetParameter();

        /// <summary>
        /// 获取参数
        /// </summary>
        /// <param name="exclude">排除的子属性控件</param>
        /// <param name="check">是否只包含启用的控件</param>
        /// <returns></returns>
        object GetParameter(List<Control> exclude, bool check);

        /// <summary>
        /// 清空参数
        /// </summary>
        void ClearParameter();
    }
}
