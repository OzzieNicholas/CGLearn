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
    class FU_Tool_Snap_Point_By_Input_Relative : FU_Main
    {
        public FU_Tool_Snap_Point_By_Input_Relative()
        {
            m_name = "tool_snap_point_by_input_relative";
            m_group = "tool";
            m_icon = "res:tb_icon_tool_snap_point_by_input_relative";
            m_barStyle = "button";
            m_tip = "相对数值输入";
            this.initShortCut( Keys.R, false, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public override bool recordable => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            if (!IFWrapper.Native.isFocusedCellSnappingPoint())
                return;

            if (Form_Snap_Point_By_Input.instance.Visible
                && Form_Snap_Point_By_Input.instance.Focused)
                return;

            Form_Snap_Point_By_Input.instance.show(Form_Settings.instance.customPara("model", "keyin_relative_prefix") as string);
        }
    }
}
