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
    class FU_Tool_Cus_Bar : FuncUnit
    {
        public FU_Tool_Cus_Bar()
        {
            m_name = "tool_cus_bar";
            m_group = "tool";
            m_icon = "res:tb_icon_custom_bar";
            m_barStyle = "button";
            m_tip = "自定义工具栏";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_CusToolBar.instance.show();
        }

        public override bool isValid() {
            return true;
        }
    }
}
