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
    class FU_View_Hide_Objs : FU_Main
    {
        public FU_View_Hide_Objs()
        {
            m_name = "view_hide_objs";
            m_group = "view";
            m_icon = "res:tb_icon_view_hide_objs";
            m_barStyle = "button";
            m_tip = "隐藏物体";
            this.initShortCut( Keys.H, false, false, true);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            bool useCur = false;
            if (paras != null) {
                useCur = paras.ContainsKey("cur_objs") && paras.GetBool("cur_objs");
            }

            if (useCur)
                Native.hideCurObjsInFocusedView();
            else
                Native.activateController("View_Hide_Objs", null);
        }
    }
}
