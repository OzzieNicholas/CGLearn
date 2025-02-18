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
    class FU_Tool_Cut_Color : FuncUnit
    {
        public FU_Tool_Cut_Color() {
            m_name = "tool_cut_color";
            m_group = "tool";
            m_icon = "res:tb_icon_cut_color";
            m_barStyle = "button";
            m_tip = "切割面提示色";
        }

        public override string shortCutInvokeType => "check";

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setSettings("cut_color", check);
            if (check)
                MainForm.instance.statusBar.tips = "切割面提示色已启用";
            else
                MainForm.instance.statusBar.tips = "切割面提示色已禁用";
        }

        public override bool isChecked {
            get {
                var qry = Native.settings("cut_color");
                return (qry != null) && (bool)qry;
            }
        }
    }
}
