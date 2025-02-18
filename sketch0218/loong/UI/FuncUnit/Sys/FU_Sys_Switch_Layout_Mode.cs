using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UI
{
    class FU_Sys_Switch_Layout_Mode: FuncUnit
    {

        public FU_Sys_Switch_Layout_Mode()
        {
            m_name = "sys_switch_layout_mode";
            m_group = "sys";
            m_icon = "";
            m_barStyle = "button";
            m_tip = "切换布局模式";
        }

        public override bool userUnit => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            if (paras == null || !paras.ContainsKey("layout"))
                return;
            string layoutMode = paras.GetString("layout");
            if (LayoutManager.instance.curLayout == layoutMode)
                return;
            CustomBarsProvision.instance.confirm();

            Define.ViewManagerMode viewMode;
            string ctrl0;
            string ctrl1;
            string ctrl2;
            string ctrl3;
            switch (layoutMode)
            {
                case "main":
                    {
                        viewMode = Define.ViewManagerMode.VIEWMGR_3D;
                        ctrl0 = "Common_Select";
                        ctrl1 = "Camera_TRS";
                        ctrl2 = "Common_Operate";
                    }
                    break;
                case "drawing":
                    {
                        viewMode = Define.ViewManagerMode.VIEWMGR_DRAWING;
                        ctrl0 = "DR_Common_Select";
                        ctrl1 = "DR_Camera_TRS";
                        ctrl2 = "Common_Operate";
                    }
                    break;
                case "drawing_template":
                    {
                        viewMode = Define.ViewManagerMode.VIEWMGR_DRAWING_TEMPLATE;
                        ctrl0 = "DR_Tmpl_Common_Select";
                        ctrl1 = "DR_Tmpl_Camera_TRS";
                        ctrl2 = "Common_Operate";
                    }
                    break;
                default:
                    return;
            }
            
            LayoutManager.instance.curLayout = layoutMode;
            IFWrapper.Native.setViewMgrMode(viewMode);
            IFWrapper.Native.activateController(ctrl0);
            IFWrapper.Native.activateController(ctrl1);
            IFWrapper.Native.activateController(ctrl2);
        }
    }
}
