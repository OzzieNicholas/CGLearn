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
    class FU_Edit_Move_Line : FU_Main
    {
        //static FU_Edit_Move_Line m_inst = new FU_Edit_Move_Line();

        public FU_Edit_Move_Line()
        {
            m_name = "edit_move_line";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_move_liner";
            m_barStyle = "button";
            m_tip = "线性移动";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Move_By_LineStride");
            
            Form_Move_ByLineStride.instance.show();
            MainForm.instance.viewPanel.Focus();
        }
    }
}
