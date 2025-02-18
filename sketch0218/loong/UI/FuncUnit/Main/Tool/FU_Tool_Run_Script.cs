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
    class FU_Tool_Run_Script : FuncUnit
    {
        public FU_Tool_Run_Script()
        {
            m_name = "tool_run_script";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_play_script";
            m_barStyle = "button";
            m_tip = "执行脚本";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_ScriptEditor.instance.show();
        }
    }
}
