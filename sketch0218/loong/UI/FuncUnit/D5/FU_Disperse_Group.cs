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
    class FU_Disperse_Group : FU_Main
    {
        public FU_Disperse_Group()
        {
            m_name = "disperse_group";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_redo";
            m_barStyle = "button";
            m_tip = "解散组";
            //this.initShortCut( Keys.Y, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.sketch_disperse_group();
        }
    }
}
