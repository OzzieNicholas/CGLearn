using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    public partial class Form_CtrlPointCornerProps : UI.ParaForm
    {
        public static Form_CtrlPointCornerProps m_instance = new Form_CtrlPointCornerProps();

        public static Form_CtrlPointCornerProps instance => m_instance;

        public Form_CtrlPointCornerProps() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            btn_cutxy_swap.BackgroundImage = Res.dpiImage("config_text_swap");
            cbo_cut_type.LoadItems(typeof(Define.CornerCutType), 0);

            this.updateTabOrder();
        }

        public void show(DObject props) {
            this.setChecks(true);
            this.updateDispUnitEnob(Form_Options.instance.props);

            if (props == null) {
                props = Native.getCommonCfg("CFG_CORNER_CUT_PROPS", "$runtime");
            }

            this.updateUIByParas(props);
            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_CORNER_CUT_PROPS", "$runtime", this.getParasByUI(), true, false);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_CORNER_CUT_PROPS", "$runtime", this.getParasByUI(), true, false);
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            DObject paras = this.getParasByUI();
            //Native.modifyCurObjects(paras, Define.ObjectType.OBJ_CTRL_POINT_CHAMFER);
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            LoongCAD.Util.DObject result = Native.curObjViewProps();
            if (result == null)
                return;

            LoongCAD.STC.Define.ObjectType objType = (LoongCAD.STC.Define.ObjectType)result["obj_type"];
            if (objType != LoongCAD.STC.Define.ObjectType.OBJ_CTRL_POINT_CHAMFER)
                return;

            this.updateUIByParas(result);
            this.setChecks(true);
        }

        private void Btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks(null);
        }

        private void cbo_cut_type_SelectedIndexChanged(object sender, EventArgs e) {
            Define.CornerCutType cutType = (Define.CornerCutType)cbo_cut_type.SelectedKey;
            if (cutType == Define.CornerCutType.CORNER_CUT_NONE) {
                chb_cut_x.Text = "X";
                tb_cut_x.Enabled = false;
                tb_cut_y.Enabled = false;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_LINE) {
                chb_cut_x.Text = "X";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = true;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_ROUND) {
                chb_cut_x.Text = "R";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = false;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_INV_ROUND) {
                chb_cut_x.Text = "R";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = false;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_ARC) {
                chb_cut_x.Text = "X";
                tb_cut_x.Enabled = false;
                tb_cut_y.Enabled = false;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_ORTHO) {
                chb_cut_x.Text = "X";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = true;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_PARA) {
                chb_cut_x.Text = "X";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = true;
            }
            else if (cutType == Define.CornerCutType.CORNER_CUT_PARA_ROUND) {
                chb_cut_x.Text = "R";
                tb_cut_x.Enabled = true;
                tb_cut_y.Enabled = true;
            }
        }

        private void btn_cutxy_swap_Click(object sender, EventArgs e) {
            double cutY = tb_cut_y.Value;
            tb_cut_y.Value = tb_cut_x.Value;
            tb_cut_x.Value = cutY;
        }
    }
}
