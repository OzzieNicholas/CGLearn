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
    class FU_View_Rotate_V5 : FU_Main
    {
        public FU_View_Rotate_V5() {
            m_name = "view_rotate_v5";
            m_group = "view";
            m_icon = "res:tb_icon_view_rotate_v5";
            m_barStyle = "button";
            m_tip = "纵向旋转+5";
            //this.initShortCut( Keys.Up, false, false, true);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            IFWrapper.Native.rotateCameraInFocusedCell(0.0, -5.0);
        }
    }
}
