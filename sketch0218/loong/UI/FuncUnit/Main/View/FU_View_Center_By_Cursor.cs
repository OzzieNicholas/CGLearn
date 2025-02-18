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
    class FU_View_Center_By_Cursor : FU_Main
    {
        public FU_View_Center_By_Cursor()
        {
            m_name = "view_center_by_cursor";
            m_group = "view";
            m_icon = "res:tb_icon_view_center_by_cursor";
            m_barStyle = "button";
            m_tip = "光标定心";
            this.initShortCut( Keys.Insert, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.centerCameraInFocusedCell();
        }
    }
}
