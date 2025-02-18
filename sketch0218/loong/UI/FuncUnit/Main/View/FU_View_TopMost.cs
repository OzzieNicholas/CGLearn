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
    class FU_View_TopMost : FU_Main
    {
        public FU_View_TopMost() {
            m_name = "view_topmost";
            m_group = "view";
            m_icon = "res:tb_icon_view_topmost";
            m_barStyle = "button";
            m_tip = "视图置顶";
        }

        public override bool userUnit => false;
        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public override string shortCutInvokeType => "check";
        public override bool isChecked => Native.isFocusedCellsAllTopMost();

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Native.setFocusedCellTopMost(check);
        }
    }
}
