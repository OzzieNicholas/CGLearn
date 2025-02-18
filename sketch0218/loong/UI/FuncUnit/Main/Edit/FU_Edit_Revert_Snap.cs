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
    class FU_Edit_Revert_Snap : FU_Main
    {
        public FU_Edit_Revert_Snap()
        {
            m_name = "edit_revert_snap";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_revert_snap";
            m_barStyle = "button";
            m_tip = "取消上一次捕捉";

            this.initShortCut( Keys.Back, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.revertSnap(this.cellType);
        }


    }
}
