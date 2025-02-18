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
    class FU_View_Set_Focused_Box_2P : FU_Main
    {
        //static FU_View_Set_Box_2P m_inst = new FU_View_Set_Box_2P();

        public FU_View_Set_Focused_Box_2P()
        {
            m_name = "view_set_focused_box_2p";
            m_group = "view";
            m_icon = "res:tb_icon_view_set_box_2p";
            m_barStyle = "button";
            m_tip = "2点设置工作区";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Set_WorkBox_By_2P");
            
        }
    }
}
