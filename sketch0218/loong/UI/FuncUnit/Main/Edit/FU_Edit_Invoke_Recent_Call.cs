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
    class FU_Edit_Invoke_Recent_Call : FU_Main
    {
        public FU_Edit_Invoke_Recent_Call()
        {
            m_name = "edit_invoke_recent_call";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_invoke_recent_call";
            m_barStyle = "button";
            m_tip = "重复最后一步操作";

            this.initShortCut( Keys.Enter, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public override bool recordable => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            FuncUnit.invokeRecentCall();
        }
    }
}
