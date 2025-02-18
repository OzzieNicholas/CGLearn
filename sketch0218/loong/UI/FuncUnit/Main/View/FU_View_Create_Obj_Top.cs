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
    class FU_View_Create_Obj_Top : FU_Main
    {
        public FU_View_Create_Obj_Top() {
            m_name = "view_create_obj_top";
            m_group = "view";
            m_icon = "res:tb_icon_view_part_top";
            m_barStyle = "button";
            m_tip = "创建对象顶视图";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            if (paras != null && paras.ContainsKey("pick_part")) {
                Native.activateController("Create_View_Part", "mode", "top");
            }
            else if (paras != null && paras.ContainsKey("pick_comp")) {
                Native.activateController("Create_View_Comp", "mode", "top");
            }
            else {
                long[] ids = Native.curObjIds();
                if (ids.Length == 0)
                    return;

                bool bWorld = ctrlKeys.HasFlag(Keys.Shift) || ((paras != null && paras.ContainsKey("world_space")) ? paras.GetBool("world_space") : false);
                bool bTemp = (paras != null && paras.ContainsKey("temp")) ? paras.GetBool("temp") : true;
                string name = (paras != null && paras.ContainsKey("name")) ? paras.GetString("name") : "top";

                if (bWorld)
                    Native.createObjViewFromModelSide(ids[0], name, Define.ObjectSide.SIDE_FRONT, bTemp);
                else
                    Native.createObjViewFromObjSide(ids[0], name, Define.ObjectSide.SIDE_TOP, bTemp);
            }
        }
    }
}
