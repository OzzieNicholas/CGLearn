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
    class FU_File_Open : FU_Main
    {
        //static FU_File_Open m_inst = new FU_File_Open();

        public FU_File_Open() {
            m_name = "file_open";
            m_group = "file";
            m_icon = "res:tb_icon_file_open";
            m_barStyle = "button";
            m_tip = "打开模型";
            //this.initShortCut( Keys.O, true, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            Form_OpenProject.instance.show();
        }

        public override bool isValid() {
            return true;
        }
    }
}
