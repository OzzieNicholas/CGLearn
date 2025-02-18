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
    class FU_View_ZoomIn_Key : FU_Main
    {
        public FU_View_ZoomIn_Key()
        {
            m_name = "view_zoom_in_key";
            m_group = "view";
            m_icon = "res:tb_icon_dr_view_zoom_in_key";
            m_barStyle = "button";
            m_tip = "键盘放大";
            this.initShortCut( Keys.PageUp, false, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.scaleCameraInFocusedCell(1.1);
        }
    }
}
