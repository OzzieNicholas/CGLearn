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
    class FU_Edit_Props : FU_Main
    {
        public FU_Edit_Props() {
            m_name = "edit_props";
            m_group = "edit";
            m_icon = "res:tb_icon_edit_props";
            m_barStyle = "button";
            m_tip = "对象/视图属性窗口";
            this.initShortCut( Keys.Return, false, true, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            switch (Native.focusedCellType()) {
                case Define.LayoutCellType.CELL_VIEW:
                this._onClickView(paras);
                break;
                default:
                break;
            }
        }

        private void _onClickView(DObject paras) {
            var props = (paras != null) ? paras : Native.curObjViewProps();
            if (props == null) {
                props = Native.focusedViewProps();
                if (props == null)
                    return;
            }

            if (props.ContainsKey("obj_type")) {
                Define.ObjectType objType = (Define.ObjectType)props.GetInt("obj_type");
                switch (objType) {
                    case Define.ObjectType.OBJ_CTRL_POINT_CHAMFER: {
                        Form_CtrlPointCornerProps.instance.show(props);
                    }
                    break;
                    case Define.ObjectType.OBJ_STEEL_PART_BEAM:
                    case Define.ObjectType.OBJ_STEEL_PART_POLY_BEAM: {
                        if (props.ContainsKey("column") && (bool)props["column"] == true)
                            Form_SPColumnProps.instance.show(props);
                        else
                            Form_SPBeamProps.instance.show(props);
                    }
                    break;
                    case Define.ObjectType.OBJ_STEEL_PART_PLATE: {
                        Form_SPPlateProps.instance.show(props);
                    }
                    break;
                    case Define.ObjectType.OBJ_GRID: {
                        Form_GridProps.instance.show(props);
                    }
                    break;
                    case Define.ObjectType.OBJ_GRID_AXIS: {
                        Form_GridAxisProps.instance.show(props);
                    }
                    break;
                }
            }
            else if (props.ContainsKey("disp_filter")) { //view
                Form_ViewProps.instance.show(props);
            }
        }

        

    }
}
