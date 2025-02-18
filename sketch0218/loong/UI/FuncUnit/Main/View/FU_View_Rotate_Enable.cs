using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    class FU_View_Rotate_Enable : FU_Main
    {
        public FU_View_Rotate_Enable() {
            m_name = "view_rotate_enable";
            m_group = "view";
            m_icon = "res:tb_icon_view_enable_rotate";
            m_barStyle = "button";
            m_tip = "禁用旋转";
            this.initShortCut( Keys.F8, false, false, false);
        }

        public override string shortCutInvokeType => "check";

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setOptions("view_rot_enable", !check);
            if (check)
                MainForm.instance.statusBar.tips = "鼠标旋转已禁用";
            else
                MainForm.instance.statusBar.tips = "鼠标旋转已启用";
        }

        public override bool isChecked {
            get {
                var qry = Native.options("view_rot_enable");
                return (qry != null) && !(bool)qry;
            }
        }
    }
}
