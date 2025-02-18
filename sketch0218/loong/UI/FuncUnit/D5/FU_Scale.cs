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
    class FU_Scale : FU_Main
    {
        //static FU_Create_SPPlate m_inst = new FU_Create_SPPlate();

        public FU_Scale()
        {
            m_name = "common_scale";
            m_group = "steel_part";
            m_icon = "res:tb_icon_create_sp_plate";
            m_barStyle = "button";
            m_tip = "缩放";

        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.sketch_active_controller("Common_Scale");
            MainForm.instance.viewPanel.Focus();
        }

        public override void onDoubleClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            //Form_SPPlateProps.instance.show(null);
        }
    }
}

