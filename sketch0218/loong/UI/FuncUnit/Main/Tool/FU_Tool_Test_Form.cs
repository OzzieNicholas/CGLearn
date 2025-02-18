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
    class FU_Tool_Test_Form : FuncUnit
    {
        public FU_Tool_Test_Form()
        {
            m_name = "tool_test_form";
            m_group = "tool";
            m_icon = "res:tb_icon_view_set_visible";
            m_barStyle = "button";
            m_tip = "测试窗口";
            
        }

        public override bool isValid() {
            return true;
        }

        public override bool userUnit => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            var form = new Form_Test();
            form.Show();
        }
    }
}
