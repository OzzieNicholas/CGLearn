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
    class FU_View_Set_Focused_Box_By_View : FU_Main
    {
        public FU_View_Set_Focused_Box_By_View()
        {
            m_name = "view_set_focused_box_by_view";
            m_group = "view";
            m_icon = "res:tb_icon_view_set_focused_box_by_view";
            m_barStyle = "button";
            m_tip = "设置工作区域到视图全景";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.setWorkAreaToVisibleAreaInFocusedViews();
        }
    }
}
