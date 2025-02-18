using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public partial class Form_GridAxisProps : ParaForm
    {
        public static Form_GridAxisProps m_instance = new Form_GridAxisProps();

        public static Form_GridAxisProps instance => m_instance;

        public Form_GridAxisProps() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            bar_cfg.init(this, "CFG_GRID_AXIS_PROPS");
            this.updateTabOrder();
        }

        public void show(DObject props) {
            bar_cfg.reset();
            this.setChecks(true);
            this.updateDispUnitEnob(Form_Options.instance.props);

            if (props == null) {
                var result = Native.getCommonCfg("CFG_GRID_AXIS_PROPS", "$runtime");
                if (result != null) {
                    this.updateUIByParas(result);
                }
            }
            else {
                this.updateUIByParas(props);
            }

            this.Visible = true;
            this.focus();
        }

        private void gridForm_closeBtn_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_GRID_AXIS_PROPS", "$runtime", this.getParasByUI(), true, false);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_GRID_AXIS_PROPS", "$runtime", this.getParasByUI(), true, false);
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            var paras = this.getParasByUI();
            //Native.modifyCurObjects(paras, Define.ObjectType.OBJ_GRID_AXIS);
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            LoongCAD.Util.DObject result = Native.curObjProps();
            if (result == null)
                return;

            LoongCAD.STC.Define.ObjectType objType = (LoongCAD.STC.Define.ObjectType)result["obj_type"];
            if (objType != LoongCAD.STC.Define.ObjectType.OBJ_GRID_AXIS)
                return;

            this.updateUIByParas(result);
            this.setChecks(true);
        }

        private void btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks(this);
        }
    }
}
