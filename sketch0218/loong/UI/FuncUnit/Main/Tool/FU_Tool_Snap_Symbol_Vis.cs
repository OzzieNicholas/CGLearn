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
    class FU_Tool_Snap_Symbol_Vis : FuncUnit
    {
        public FU_Tool_Snap_Symbol_Vis() {
            m_name = "tool_snap_symbol_vis";
            m_group = "tool";
            m_icon = "res:tb_icon_snap_symbol_vis";
            m_barStyle = "button";
            m_tip = "显示捕捉符号";

        }

        public override string shortCutInvokeType => "check";

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setSettings("snap_symbol_vis", check);
        }

        public override bool isChecked {
            get {
                var qry = Native.settings("snap_symbol_vis");
                return (qry != null) && (bool)qry;
            }
        }
    }
}
