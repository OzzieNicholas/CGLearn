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
    class FU_View_Switch_Type : FU_Main
    {
        public FU_View_Switch_Type()
        {
            m_name = "view_switch_type";
            m_group = "view";
            m_icon = "res:tb_icon_view_switch_type";
            m_barStyle = "button";
            m_tip = "切换3D/平面";
            //this.initShortCut( Keys.P, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.switchFocusedViewType();
            Native.resetFocusedViewCamPose();
        }
    }
}
