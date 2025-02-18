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
    class FU_Sel_Cur_Objs_Redo : FU_Main
    {
        public FU_Sel_Cur_Objs_Redo()
        {
            m_name = "sel_cur_objs_redo";
            m_group = "sel";
            m_icon = "res:tb_icon_sel_cur_objs_redo";
            m_barStyle = "button";
            m_tip = "重做物体选择";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.redoCurObjs();
        }

    }
}
