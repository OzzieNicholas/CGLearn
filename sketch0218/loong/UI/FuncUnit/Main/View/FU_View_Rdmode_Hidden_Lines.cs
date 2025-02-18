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
    class FU_View_Rdmode_Hidden_Lines : FU_Main
    {
        public FU_View_Rdmode_Hidden_Lines()
        {
            m_name = "view_rdmode_hidden_lines";
            m_group = "view";
            m_icon = "res:tb_icon_part_represent_cover";
            m_barStyle = "button";
            m_tip = "隐藏线模式";
            this.initShortCut( Keys.D3, true, false, false);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.modifyPartRenderModeInFocusedViews(Define.ViewRenderMode.RENDERMODE_HIDDEN_LINES);
        }
    }
}
