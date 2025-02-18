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
    class FU_Tool_Hover_Highlight : FuncUnit
    {
        public FU_Tool_Hover_Highlight() {
            m_name = "tool_hover_highlight";
            m_group = "tool";
            m_icon = "res:tb_icon_hover_highlight";
            m_barStyle = "button";
            m_tip = "鼠标滑过高亮";
            this.initShortCut( Keys.H, false, false, false);
        }

        public override string shortCutInvokeType => "check";

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setSettings("hover_highlight", check);
            if (check)
                MainForm.instance.statusBar.tips = "鼠标悬浮高亮已启用";
            else
                MainForm.instance.statusBar.tips = "鼠标悬浮高亮已禁用";
        }

        public override bool isChecked {
            get {
                var qry = Native.settings("hover_highlight");
                return (qry != null) && (bool)qry;
            }
        }
    }
}
