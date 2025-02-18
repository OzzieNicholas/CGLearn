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
    class FU_Tool_Snap_Toggle_Pt_Free : FuncUnit
    {
        public FU_Tool_Snap_Toggle_Pt_Free()
        {
            m_name = "tool_snap_toggle_pt_free";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_snap_toggle_pt_free";
            m_barStyle = "button";
            m_tip = "捕捉任意点";
            this.initShortCut( Keys.F7, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            uint flags = IFWrapper.Native.ptSnapFlag(Define.LayoutCellType.CELL_NONE);
            IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_NONE, Define.SNAP_PT_FREE, (flags & Define.SNAP_PT_FREE) == 0, true);
        }
    }
}
