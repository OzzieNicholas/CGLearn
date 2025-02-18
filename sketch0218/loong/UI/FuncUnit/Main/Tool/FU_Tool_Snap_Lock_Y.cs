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
    class FU_Tool_Snap_Lock_Y : FuncUnit
    {
        public FU_Tool_Snap_Lock_Y()
        {
            m_name = "tool_snap_lock_y";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_snap_lock_y";
            m_barStyle = "button";
            m_tip = "切换Y轴锁定";
            this.initShortCut( Keys.Y, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public override bool recordable => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.toggleSnapLock(this.cellType, Define.SpaceAxis.AXIS_Y);
        }
    }
}
