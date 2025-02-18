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
    class FU_Tool_Settings : FuncUnit
    {
        public FU_Tool_Settings()
        {
            m_name = "tool_settings";
            m_group = "tool";
            m_icon = "res:tb_icon_system_setting";
            m_barStyle = "button";
            m_tip = "系统设置";
            
        }

        public override bool isValid() {
            return true;
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_Settings.instance.show();
        }
    }
}
