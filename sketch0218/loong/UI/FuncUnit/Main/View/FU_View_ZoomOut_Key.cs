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
    class FU_View_ZoomOut_Key : FU_Main
    {
        public FU_View_ZoomOut_Key()
        {
            m_name = "view_zoom_out_key";
            m_group = "view";
            m_icon = "res:tb_icon_dr_view_zoom_out_key";
            m_barStyle = "button";
            m_tip = "键盘缩小";
            this.initShortCut( Keys.PageDown, false, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.scaleCameraInFocusedCell(0.9);
        }
    }
}
