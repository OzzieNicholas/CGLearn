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
    class FU_Tool_Auto_Occ_Detect : FuncUnit
    {
        public FU_Tool_Auto_Occ_Detect() {
            m_name = "tool_auto_occ_detect";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_auto_occ_detect";
            m_barStyle = "button";
            m_tip = "自动碰撞检测";
            
        }

        public override string shortCutInvokeType => "check";

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setSettings("auto_occ_detect", check);
            if (check)
                MainForm.instance.statusBar.tips = "自动碰撞检测已启用";
            else
                MainForm.instance.statusBar.tips = "自动碰撞检测已禁用";
        }

        public override bool isChecked {
            get {
                var qry = Native.settings("auto_occ_detect");
                return (qry != null) && (bool)qry;
            }
        }
    }
}
