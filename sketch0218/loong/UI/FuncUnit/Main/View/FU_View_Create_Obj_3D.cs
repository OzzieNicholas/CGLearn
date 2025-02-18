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
    class FU_View_Create_Obj_3D : FU_Main
    {
        public FU_View_Create_Obj_3D() {
            m_name = "view_create_obj_3d";
            m_group = "view";
            m_icon = "res:tb_icon_view_part_3d";
            m_barStyle = "button";
            m_tip = "创建3D对象视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            if (paras != null && paras.ContainsKey("pick_part")) {
                IFWrapper.Native.activateController("Create_View_Part", "mode", "3d");
            }
            else if (paras != null && paras.ContainsKey("pick_comp")) {
                IFWrapper.Native.activateController("Create_View_Comp", "mode", "3d");
            }
            else {
                bool bTemp = (paras != null && paras.ContainsKey("temp")) ? paras.GetBool("temp") : true;
                string name = (paras != null && paras.ContainsKey("name")) ? paras.GetString("name") : "";
                IFWrapper.Native.createView3DForCurObjs(name, bTemp);
            }
        }
    }
}
