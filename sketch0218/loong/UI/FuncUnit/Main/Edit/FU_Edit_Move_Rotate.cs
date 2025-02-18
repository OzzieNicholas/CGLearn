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
    class FU_Edit_Move_Rotate : FU_Main
    {
        //static FU_Edit_Move_Rotate m_inst = new FU_Edit_Move_Rotate();

        public FU_Edit_Move_Rotate()
        {
            m_name = "edit_move_rotate";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_move_rotate";
            m_barStyle = "button";
            m_tip = "旋转移动";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Move_By_Rotate");
            
            Form_Move_ByRotate.instance.show();
            MainForm.instance.viewPanel.Focus();
        }
    }
}
