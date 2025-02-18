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
    class FU_View_Set_Active_Box_By_Part : FU_Main
    {
        public FU_View_Set_Active_Box_By_Part()
        {
            m_name = "view_set_active_box_by_part";
            m_group = "view";
            m_icon = "res:tb_icon_view_act_box_part";
            m_barStyle = "button";
            m_tip = "设置所有视图工作区到零件";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.setWorkAreaToCurObjsInActiveViews();
        }
    }
}
