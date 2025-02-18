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
    class FU_View_Switch_To_Next : FU_Main
    {
        public FU_View_Switch_To_Next()
        {
            m_name = "view_switch_to_next";
            m_group = "view";
            m_icon = "res:tb_icon_view_next";
            m_barStyle = "button";
            m_tip = "下一窗口";
            this.initShortCut(Keys.Tab, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            MainForm.instance.headerTab.switchNext();
        }
    }
}
