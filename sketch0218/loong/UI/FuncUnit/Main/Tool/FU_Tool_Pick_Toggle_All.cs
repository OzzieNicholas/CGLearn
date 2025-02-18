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
    class FU_Tool_Pick_Toggle_All : FuncUnit
    {
        public FU_Tool_Pick_Toggle_All()
        {
            m_name = "tool_pick_toggle_all";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_pick_toggle_all";
            m_barStyle = "button";
            m_tip = "切换所有对象拣选";
            this.initShortCut( Keys.F2, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public override bool recordable => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.togglePickAll(this.cellType, true);
        }
    }
}
