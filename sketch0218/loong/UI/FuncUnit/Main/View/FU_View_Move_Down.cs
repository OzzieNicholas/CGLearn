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
    class FU_View_Move_Down : FU_Main
    {
        public FU_View_Move_Down()
        {
            m_name = "view_move_down";
            m_group = "view";
            m_icon = "res:tb_icon_view_move_down";
            m_barStyle = "button";
            m_tip = "向下移动";
            //this.initShortCut( Keys.Down, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.moveCameraInFocusedCell(Define.SpaceAxis.AXIS_Y, -MainForm.instance.viewPanel.Height / 20);
        }
    }
}
