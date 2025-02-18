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
    class FU_Edit_Del : FU_Main
    {
        public FU_Edit_Del()
        {
            m_name = "edit_del";
            m_group = "edit";
            m_icon = "res:tb_icon_sc_delete";
            m_barStyle = "button";
            m_tip = "删除";
            this.initShortCut( Keys.Delete, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.sketch_delete_select();
        }
    }
}
