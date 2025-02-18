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
    class FU_View_Zoom_By_WorkBox : FU_Main
    {
        public FU_View_Zoom_By_WorkBox()
        {
            m_name = "view_zoom_by_workbox";
            m_group = "view";
            m_icon = "res:tb_icon_dr_view_reset_scale";
            m_barStyle = "button";
            m_tip = "恢复原始尺寸";
            this.initShortCut( Keys.Home, false, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.scaleViewByWorkBox();
        }
    }
}
