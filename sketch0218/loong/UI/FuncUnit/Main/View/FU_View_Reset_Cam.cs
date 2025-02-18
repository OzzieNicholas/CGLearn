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
    class FU_View_Reset_Cam : FU_Main
    {
        public FU_View_Reset_Cam()
        {
            m_name = "view_reset_cam";
            m_group = "view";
            m_icon = "res:tb_icon_dr_cam_reset";
            m_barStyle = "button";
            m_tip = "重置视角";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.resetFocusedViewCamPose();
        }
    }
}
