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
    class FU_View_WorkBoxVis : FU_Main
    {
        public FU_View_WorkBoxVis() {
            m_name = "view_workbox_vis";
            m_group = "view";
            m_icon = "res:tb_icon_view_workbox_vis";
            m_barStyle = "button";
            m_tip = "设置视图工作区可见性";
        }

        public override bool userUnit => false;
        public override string shortCutInvokeType => "check";
        public override bool isChecked => Native.isFocusedViewWorkBoxVis();

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setFocusedViewWorkBoxVis(check);
        }
    }
}
