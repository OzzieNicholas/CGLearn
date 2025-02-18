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
    class FU_View_Rdquality_High : FU_Main
    {
        public FU_View_Rdquality_High() {
            m_name = "view_rdquality_high";
            m_group = "view";
            m_icon = "res:tb_icon_view_high_quality";
            m_barStyle = "button";
            m_tip = "打开/关闭零件高精确显示";
        }

        public override bool userUnit => false;
        //public override bool isChecked => Native.isCurSPPartsRdModeAllDetail();
        public override bool isChecked => true;
        //public override string shortCutInvokeType => "check";

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            //bool check = !Native.isCurSPPartsRdModeAllDetail();
            //Native.modifyCurObjectsByBaseType(new DObject { { "render_quality", check ? Define.ViewRenderQuality.RENDERQUALITY_DETAIL : Define.ViewRenderQuality.RENDERQUALITY_NONE } },
            //                                             Define.ObjectType.OBJ_STEEL_PART_BEGIN, Define.ObjectType.OBJ_STEEL_PART_END);
        }

        public override void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            //Native.modifyCurObjectsByBaseType(new DObject { { "render_quality", check ? Define.ViewRenderQuality.RENDERQUALITY_DETAIL : Define.ViewRenderQuality.RENDERQUALITY_NONE } },
            //                                    Define.ObjectType.OBJ_STEEL_PART_BEGIN, Define.ObjectType.OBJ_STEEL_PART_END);
        }
    }
}
