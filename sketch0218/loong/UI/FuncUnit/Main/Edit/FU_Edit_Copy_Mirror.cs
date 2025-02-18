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
    class FU_Edit_Copy_Mirror : FU_Main
    {
        //static FU_Edit_Copy_Mirror m_inst = new FU_Edit_Copy_Mirror();

        public FU_Edit_Copy_Mirror()
        {
            m_name = "edit_copy_mirror";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_copy_mirror";
            m_barStyle = "button";
            m_tip = "镜像复制";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Copy_By_Mirror");
            
            Form_Copy_ByMirror.instance.show(null);
            MainForm.instance.viewPanel.Focus();
        }
    }
}
