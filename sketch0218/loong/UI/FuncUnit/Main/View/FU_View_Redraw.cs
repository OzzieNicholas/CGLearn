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
    class FU_View_Redraw : FU_Main
    {
        public FU_View_Redraw()
        {
            m_name = "view_redraw";
            m_group = "view";
            m_icon = "res:tb_icon_view_redraw";
            m_barStyle = "button";
            m_tip = "重绘";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.redrawFocusedViews();
        }
    }
}
