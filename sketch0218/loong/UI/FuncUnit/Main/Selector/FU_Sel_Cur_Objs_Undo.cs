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
    class FU_Sel_Cur_Objs_Undo : FU_Main
    {
        public FU_Sel_Cur_Objs_Undo()
        {
            m_name = "sel_cur_objs_undo";
            m_group = "sel";
            m_icon = "res:tb_icon_sel_cur_objs_undo";
            m_barStyle = "button";
            m_tip = "回退物体选择";
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Native.undoCurObjs();
        }

    }
}
