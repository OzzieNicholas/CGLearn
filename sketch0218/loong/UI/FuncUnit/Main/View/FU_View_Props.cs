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
    class FU_View_Props : FU_Main
    {
        public FU_View_Props() {
            m_name = "view_props";
            m_group = "view";
            m_icon = "res:tb_icon_view_prop";
            m_barStyle = "button";
            m_tip = "视图属性";

        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            bool useCur = false;
            if (paras != null) {
                useCur = paras.ContainsKey("cur_view") && paras.GetBool("cur_view");
            }

            if (!useCur) {
                Form_ViewProps.instance.show(paras);
            }
            else {
                var qry = Native.focusedViewProps();
                if (qry == null)
                    return;

                Form_ViewProps.instance.show(qry);
            }

        }
    }
}
