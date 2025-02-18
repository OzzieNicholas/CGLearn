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
    class FU_File_New : FU_Main
    {
        public bool m_isCreate = false;
        public FU_File_New()
        {
            m_name = "file_new";
            m_group = "file";
            m_icon = "res:tb_icon_file_new";
            m_barStyle = "button";
            m_tip = "新建模型";
            this.initShortCut( Keys.N, true, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            //New document

            //Form_NewProject.instance.show();

            //if (!m_isCreate)
            {
                //m_isCreate = true;
                this.creatProj();
            }
        }

        public void creatProj()
        {
            Form_NewProject.instance.force_create_proj();
        }

        public override bool isValid() {
            return true;
        }
    }

}
