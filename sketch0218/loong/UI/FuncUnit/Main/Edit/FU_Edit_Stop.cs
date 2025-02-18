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
    class FU_Edit_Stop : FU_Main
    {
        public FU_Edit_Stop()
        {
            m_name = "edit_stop";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_stop";
            m_barStyle = "button";
            m_tip = "中断";
            this.initShortCut( Keys.Escape, false, false, false);
        }

        public override bool recordable => false;

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.sketch_escape("{controller_escape}");
        }
    }
}
