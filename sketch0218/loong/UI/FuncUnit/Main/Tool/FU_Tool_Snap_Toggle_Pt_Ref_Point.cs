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
    class FU_Tool_Snap_Toggle_Pt_Ref_Point : FuncUnit
    {
        public FU_Tool_Snap_Toggle_Pt_Ref_Point()
        {
            m_name = "tool_snap_toggle_pt_ref_point";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_snap_toggle_pt_ref_point";
            m_barStyle = "button";
            m_tip = "捕捉参考点/线";
            this.initShortCut( Keys.F4, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            uint flags = IFWrapper.Native.ptSnapFlag(Define.LayoutCellType.CELL_NONE);
            IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_NONE, Define.SNAP_PT_REF_POINT, (flags & Define.SNAP_PT_REF_POINT) == 0, true);
        }
    }
}
