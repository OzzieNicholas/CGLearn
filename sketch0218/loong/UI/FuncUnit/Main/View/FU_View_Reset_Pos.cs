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
    class FU_View_Reset_Pos : FU_Main
    {
        public FU_View_Reset_Pos()
        {
            m_name = "view_reset_pos";
            m_group = "view";
            m_icon = "res:tb_icon_view_reset_pos";
            m_barStyle = "button";
            m_tip = "重置视图位置";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.resetFocusedCellPos();
        }
    }
}
