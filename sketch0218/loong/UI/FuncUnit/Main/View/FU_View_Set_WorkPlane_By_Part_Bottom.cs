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
    class FU_View_Set_WorkPlane_By_Part_Bottom : FU_Main
    {
        public FU_View_Set_WorkPlane_By_Part_Bottom()
        {
            m_name = "view_set_workplane_by_part_bottom";
            m_group = "view";
            m_icon = "res:tb_icon_work_plane_bottom";
            m_barStyle = "button";
            m_tip = "设置工作面到零件底面";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Set_WorkPlane_By_Part_Side", "mode", (sbyte)Define.ObjectSide.SIDE_BOTTOM);
            
        }
    }
}
