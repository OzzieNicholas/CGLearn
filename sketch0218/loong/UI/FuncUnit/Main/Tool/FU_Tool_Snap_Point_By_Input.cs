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
    class FU_Tool_Snap_Point_By_Input : FU_Main
    {
        public FU_Tool_Snap_Point_By_Input() {
            m_name = "tool_snap_point_by_input";
            m_group = "tool";
            m_icon = "res:tb_icon_";
            m_barStyle = "button";
            m_tip = "数值输入";
        }

        public override bool userUnit => false;
        public override bool recordable => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            if (!IFWrapper.Native.isFocusedCellSnappingPoint())
                return;

            if (Form_Snap_Point_By_Input.instance.Visible
                && Form_Snap_Point_By_Input.instance.Focused) {
                return;
            }

            string input = "";
            if (sender is uint) { //short cut
                Keys key0 = (Keys)((uint)sender & 0xff);
                if (key0 == Keys.Subtract
                    || key0 == Keys.OemMinus)
                    input = "-";
                else if (key0 >= Keys.D0 && key0 <= Keys.D9)
                    input = (key0 - Keys.D0).ToString();
                else if (key0 >= Keys.NumPad0 && key0 <= Keys.NumPad9)
                    input = (key0 - Keys.NumPad0).ToString();
            }

            Form_Snap_Point_By_Input.instance.show(input);
        }
    }
}
