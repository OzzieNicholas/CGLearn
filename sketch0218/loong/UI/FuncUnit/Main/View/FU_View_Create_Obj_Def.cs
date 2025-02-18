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
    class FU_View_Create_Obj_Def : FU_Main
    {
        public FU_View_Create_Obj_Def() {
            m_name = "view_create_obj_def";
            m_group = "view";
            m_icon = "res:tb_icon_view_obj_def";
            m_barStyle = "button";
            m_tip = "创建默认对象视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            if (paras != null && paras.ContainsKey("pick_part")) {
                Native.activateController("Create_View_Part", "mode", "default");
            }
            else if (paras != null && paras.ContainsKey("pick_comp")) {
                Native.activateController("Create_View_Comp", "mode", "default");
            }
            else {
                long[] ids = Native.curObjIds();
                if (ids.Length == 0)
                    return;

                bool bTemp = (paras != null && paras.ContainsKey("temp")) ? paras.GetBool("temp") : true;

                Native.createObjViewFromModelSide(ids[0], "front", Define.ObjectSide.SIDE_FRONT, bTemp);
                Native.createObjViewFromModelSide(ids[0], "top", Define.ObjectSide.SIDE_TOP, bTemp);
                Native.createObjViewFromModelSide(ids[0], "end", Define.ObjectSide.SIDE_TAIL, bTemp);
                Native.createObjView3D(ids[0], "perspective", bTemp);
            }
        }
    }
}
