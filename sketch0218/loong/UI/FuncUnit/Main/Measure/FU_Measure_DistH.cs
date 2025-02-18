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
    class FU_Measure_DistH : FU_Main
    {
        //static FU_Measure_DistH m_inst = new FU_Measure_DistH();

        public FU_Measure_DistH()
        {
            m_name = "measure_dist_h";
            m_group = "measure";
            m_icon = "res:tb_icon_measure_dist_h";
            m_barStyle = "button";
            m_tip = "水平距离";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            IFWrapper.Native.activateController("Measure_DistH");
            
        }
    }
}
