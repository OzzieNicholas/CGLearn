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
    class FU_View_Focus_Objs : FU_Main
    {
        public FU_View_Focus_Objs()
        {
            m_name = "view_focus_objs";
            m_group = "view";
            m_icon = "res:tb_icon_view_focus_objs";
            m_barStyle = "button";
            m_tip = "仅显示选中物体";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.focusCurObjsInFocusedView(ctrlKeys.HasFlag(Keys.Shift) ? Define.ViewFocusMode.VIEW_FOCUS_VIS_ONLY : Define.ViewFocusMode.VIEW_FOCUS_KEEP_STYLE);
        }
    }
}
