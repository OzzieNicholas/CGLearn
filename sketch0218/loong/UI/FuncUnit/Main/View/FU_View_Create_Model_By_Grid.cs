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
    class FU_View_Create_Model_By_Grid : FU_Main
    {
        public FU_View_Create_Model_By_Grid()
        {
            m_name = "view_create_model_by_grid";
            m_group = "view";
            m_icon = "res:tb_icon_view_obj_grid";
            m_barStyle = "button";
            m_tip = "沿轴线创建模型视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_CreateViewByGrid.instance.show(null);
        }
    }
}
