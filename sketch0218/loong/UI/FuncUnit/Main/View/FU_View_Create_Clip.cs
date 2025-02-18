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
    class FU_View_Create_Clip : FU_Main
    {
        //static FU_View_Create_Clip m_inst = new FU_View_Create_Clip();

        public FU_View_Create_Clip()
        {
            m_name = "view_create_clip";
            m_group = "view";
            m_icon = "res:tb_icon_view_create_clip";
            m_barStyle = "button";
            m_tip = "创建视图裁剪面";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.setPlSnapFlag(Define.LayoutCellType.CELL_VIEW, Define.SNAP_PL_GEO, true);
            IFWrapper.Native.activateController("Create_ViewClipPlane");
            
        }
    }
}
