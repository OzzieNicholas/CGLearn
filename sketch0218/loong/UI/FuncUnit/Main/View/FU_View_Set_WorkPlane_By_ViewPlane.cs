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
    class FU_View_Set_WorkPlane_By_ViewPlane : FU_Main
    {
        public FU_View_Set_WorkPlane_By_ViewPlane()
        {
            m_name = "view_set_workplane_by_viewplane";
            m_group = "view";
            m_icon = "res:tb_icon_set_work_plane_view";
            m_barStyle = "button";
            m_tip = "设置工作面到视图平面";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            DObject qryView = Native.focusedViewProps();
            if (qryView == null)
                return;

            Plane viewPlane = qryView.GetPlane("viewplane");
            Native.setFocusedWorkAxis(viewPlane.Matrix);
        }
    }
}
