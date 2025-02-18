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
    class FU_Create_SPColumn_Props : FU_Main
    {
        public FU_Create_SPColumn_Props()
        {
            m_name = "sp_col_props";
            m_group = "steel_part";
            m_icon = "res:tb_icon_sp_col_prop";
            m_barStyle = "button";
            m_tip = "柱属性";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            Form_SPColumnProps.instance.show(null);
        }

    }
}
