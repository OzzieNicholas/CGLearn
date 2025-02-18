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
    class FU_View_Create_Model_3P : FU_Main
    {
        public FU_View_Create_Model_3P()
        {
            m_name = "view_create_model_3p";
            m_group = "view";
            m_icon = "res:tb_icon_view_create_model_3p";
            m_barStyle = "button";
            m_tip = "3点创建模型视图";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Create_View_Model_By_3P");
            
        }
    }
}
