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
    class FU_View_Trans_By_Hand : FU_Main
    {
        public FU_View_Trans_By_Hand()
        {
            m_name = "view_trans_by_hand";
            m_group = "view";
            m_icon = "res:tb_icon_view_move";
            m_barStyle = "button";
            m_tip = "移动";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("View_Trans_Hand");
            
        }
    }
}
