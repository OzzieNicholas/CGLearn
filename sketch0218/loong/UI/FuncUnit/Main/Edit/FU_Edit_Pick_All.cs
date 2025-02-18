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
    class FU_Edit_Pick_All : FU_Main
    {
        //static FU_Edit_Pick_All m_inst = new FU_Edit_Pick_All();

        public FU_Edit_Pick_All()
        {
            m_name = "edit_pick_all";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_pick_all";
            m_barStyle = "button";
            m_tip = "全选";
            this.initShortCut( Keys.A, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.setCurObjsToFocusedViewVisObjs();
        }
    }
}
