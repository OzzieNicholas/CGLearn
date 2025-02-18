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
    class FU_View_Rotate_VN15 : FU_Main
    {
        public FU_View_Rotate_VN15() {
            m_name = "view_rotate_vn15";
            m_group = "view";
            m_icon = "res:tb_icon_view_rotate_vn15";
            m_barStyle = "button";
            m_tip = "纵向旋转-15";
            //this.initShortCut( Keys.Down, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            IFWrapper.Native.rotateCameraInFocusedCell(0.0, 15.0);
        }
    }
}
