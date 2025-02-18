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
    class FU_View_Create_Model_By_WorkPlane : FU_Main
    {
        public FU_View_Create_Model_By_WorkPlane()
        {
            m_name = "view_create_model_by_workplane";
            m_group = "view";
            m_icon = "res:tb_icon_view_work_plane";
            m_barStyle = "button";
            m_tip = "沿工作平面创建模型视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.createModelViewByFocusedWorkPlane();
        }
    }
}
