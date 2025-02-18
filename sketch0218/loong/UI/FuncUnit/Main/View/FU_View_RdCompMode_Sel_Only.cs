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
    class FU_View_RdCompMode_Sel_Only : FU_Main
    {
        public FU_View_RdCompMode_Sel_Only()
        {
            m_name = "view_rdcompmode_sel_only";
            m_group = "view";
            m_icon = "res:tb_icon_comp_represent_selected";
            m_barStyle = "button";
            m_tip = "组件仅显示选中";
            this.initShortCut( Keys.D5, false, false, true);
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.modifyPartRenderModeCompInFocusedViews(Define.ViewRenderMode.RENDERMODE_SEL_ONLY);
        }
    }
}
