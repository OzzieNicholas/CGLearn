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
    class FU_Edit_Undo : FU_Main
    {
        public FU_Edit_Undo()
        {
            m_name = "edit_undo";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_undo";
            m_barStyle = "button";
            m_tip = "撤销";
            this.initShortCut( Keys.Z, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.sketch_deactive_current_controller();
            Native.sketch_undo_focused_view();
        }
    }
}
