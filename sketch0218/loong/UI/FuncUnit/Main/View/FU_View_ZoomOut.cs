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


namespace UI
{
    class FU_View_ZoomOut : FU_Main
    {
        public FU_View_ZoomOut()
        {
            m_name = "view_zoom_out";
            m_group = "view";
            m_icon = "res:tb_icon_dr_view_zoom_out";
            m_barStyle = "button";
            m_tip = "鼠标点击缩小";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("View_Scale_Zoom", "mode", "out");
        }
    }
}
