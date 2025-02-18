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
    class FU_Edit_Move : FU_Main
    {
        //static FU_Edit_Move m_inst = new FU_Edit_Move();

        public FU_Edit_Move()
        {
            m_name = "edit_move";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_move";
            m_barStyle = "button";
            m_tip = "移动";
            this.initShortCut( Keys.M, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Move");
        }
    }
}
