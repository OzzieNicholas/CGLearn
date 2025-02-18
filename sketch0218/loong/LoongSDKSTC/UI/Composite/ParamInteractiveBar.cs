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
    /// 属性交互按钮栏，包含以下按钮
    /// <list type="number">
    ///     <item>
    ///         <term>确定</term>
    ///         <description>获取接口为 <b>OkButton</b></description>
    ///     </item>
    ///     <item>
    ///         <term>应用</term>
    ///         <description>获取接口为 <b>ApplyButton</b></description>
    ///     </item>
    ///     <item>
    ///         <term>修改</term>
    ///         <description>获取接口为 <b>ModifyButton</b></description>
    ///     </item>
    ///     <item>
    ///         <term>获取</term>
    ///         <description>获取接口为 <b>GetButton</b></description>
    ///     </item>
    ///     <item>
    ///         <term>[勾选框]</term>
    ///         <description>获取接口为 <b>CheckBoxButton</b></description>
    ///     </item>
    ///     <item>
    ///         <term>取消</term>
    ///         <description>获取接口为 <b>CancelButton</b></description>
    ///     </item>
    /// </list>
    /// </summary>
    [ToolboxItem(true), ToolboxBitmap(typeof(Panel))]
    public partial class ParamInteractiveBar : UserControl
    {

        /// <summary>
        /// 默认构造函数
        /// </summary>
        public ParamInteractiveBar()
        {
            InitializeComponent();
            this.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
        }

        /// <summary>
        /// <b>确定</b> 按钮
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button OkButton { get => this.btn_ok; }

        /// <summary>
        /// <b>应用</b> 按钮
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button ApplyButton { get => this.btn_apply; }

        /// <summary>
        /// <b>修改</b> 按钮
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button ModifyButton { get => this.btn_modify; }

        /// <summary>
        /// <b>获取</b> 按钮
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button GetButton { get => this.btn_get; }

        /// <summary>
        /// <b>[勾选框]</b> 按钮
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button CheckBoxButton { get => this.btn_chb; }

        /// <summary>
        /// <b>取消</b> 按钮
        /// </summary>
        /// [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Button CancelButton { get => this.btn_cancel; }

    }

}
