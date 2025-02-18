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
    class FU_View_Create_Comp_Def : FU_Main
    {
        public FU_View_Create_Comp_Def()
        {
            m_name = "view_create_comp_def";
            m_group = "view";
            m_icon = "res:tb_icon_view_comp_def";
            m_barStyle = "button";
            m_tip = "创建默认组件视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Create_View_Comp", "mode", "default");

        }
    }
}
