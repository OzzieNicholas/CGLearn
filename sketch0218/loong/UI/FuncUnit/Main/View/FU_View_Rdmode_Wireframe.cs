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
    class FU_View_Rdmode_Wireframe : FU_Main
    {
        public FU_View_Rdmode_Wireframe()
        {
            m_name = "view_rdmode_wireframe";
            m_group = "view";
            m_icon = "res:tb_icon_part_represent_lineframe";
            m_barStyle = "button";
            m_tip = "线框模式";
            this.initShortCut( Keys.D1, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.modifyPartRenderModeInFocusedViews(Define.ViewRenderMode.RENDERMODE_WIREFRAME);
        }
    }
}
