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
    class FU_View_Pick_Pivot_Rotate : FU_Main
    {
        public FU_View_Pick_Pivot_Rotate()
        {
            m_name = "view_pick_pivot_rotate";
            m_group = "view";
            m_icon = "res:tb_icon_view_pick_pivot_rotate";
            m_barStyle = "button";
            m_tip = "设置相机轴心并旋转";
            this.initShortCut( Keys.R, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Pick_ViewPivot_Rotate");
            
        }
    }
}
