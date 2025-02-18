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
    class FU_Tool_Snap_Toggle_Pt_Line_Nearest : FuncUnit
    {
        public FU_Tool_Snap_Toggle_Pt_Line_Nearest()
        {
            m_name = "tool_snap_toggle_pt_line_nearest";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_snap_toggle_pt_line_nearest";
            m_barStyle = "button";
            m_tip = "捕捉最近点";
            this.initShortCut( Keys.F6, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            uint flags = IFWrapper.Native.ptSnapFlag(Define.LayoutCellType.CELL_NONE);
            IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_NONE, Define.SNAP_PT_LINE_NEAREST, (flags & Define.SNAP_PT_LINE_NEAREST) == 0, true);
        }
    }
}
