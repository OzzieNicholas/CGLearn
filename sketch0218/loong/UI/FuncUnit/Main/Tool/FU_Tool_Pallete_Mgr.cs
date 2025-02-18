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
    class FU_Tool_Pallete_Mgr : FuncUnit
    {
        public FU_Tool_Pallete_Mgr()
        {
            m_name = "tool_pallete_mgr";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_pallete_mgr";
            m_barStyle = "button";
            m_tip = "系统配色方案";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_PalleteManager.instance.show();
        }

        public override bool isValid() {
            return true;
        } 
    }
}
