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
using LoongCAD.UI;
using IFWrapper;

namespace UI
{
    public partial class Form_SPBeamProps : UI.ParaForm
    {
        public static Form_SPBeamProps m_instance = new Form_SPBeamProps();
        private Panel_UsrData_SP m_usrData = null;

        public static Form_SPBeamProps instance => m_instance;

        public Form_SPBeamProps() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.618f, 0.2f);
        }

        private void _initControls() {
            m_usrData = new Panel_UsrData_SP();
            tp_3.Controls.Add(m_usrData);

            bar_cfg.init(this, "CFG_SP_BEAM_PROPS");

            cbo_class.DropDownStyle = ComboBoxStyle.DropDownList;
            cbo_class.dispStyle = UserCtrl.Combo.DisplayStyle.COLOR_TEXT;
            
            rb_offset_h.labels = new string[] { "左", "中", "右" };
            rb_offset_h.keys = new object[] { Define.ObjectSide.SIDE_LEFT, Define.ObjectSide.SIDE_MIDDLE, Define.ObjectSide.SIDE_RIGHT };
            rb_offset_h.keys1 = new object[] { "left", "middle", "right" };
            rb_offset_h.refresh();
            rb_offset_h.Value = Define.ObjectSide.SIDE_MIDDLE;
            rb_offset_h.onUserModified += btn_modify_Click;

            rb_offset_v.labels = new string[] { "前", "中", "后"};
            rb_offset_v.keys = new object[] { Define.ObjectSide.SIDE_FRONT, Define.ObjectSide.SIDE_MIDDLE, Define.ObjectSide.SIDE_BACK };
            rb_offset_v.keys1 = new object[] { "front", "middle", "back" };
            rb_offset_v.refresh();
            rb_offset_v.Value = Define.ObjectSide.SIDE_MIDDLE;
            rb_offset_v.onUserModified += btn_modify_Click;

            rb_rot.labels = new string[] { "前", "顶", "后", "底" };
            rb_rot.keys = new object[] { Define.ObjectSide.SIDE_FRONT, Define.ObjectSide.SIDE_TOP, Define.ObjectSide.SIDE_BACK, Define.ObjectSide.SIDE_BOTTOM };
            rb_rot.keys1 = new object[] { "front", "top", "back", "bottom" };
            rb_rot.refresh();
            rb_rot.Value = Define.ObjectSide.SIDE_TOP;
            rb_rot.onUserModified += btn_modify_Click;

            rb_curve_plane.labels = new string[] { "横", "竖" };
            rb_curve_plane.keys = new object[] { Define.SpacePlane.PLANE_ZX, Define.SpacePlane.PLANE_YZ };
            rb_curve_plane.refresh();
            rb_curve_plane.onUserModified += btn_modify_Click;

            this.updateTabOrder();
        }

        public void show(DObject props) {
            bar_cfg.reset();
            m_usrData.reset();
            this.setChecks(true);
            this.updateDispUnitEnob(Form_Options.instance.props);
            
            if (props == null) {
                var result = Native.getCommonCfg("CFG_SP_BEAM_PROPS", "$runtime");
                this.updateUIByParas(result);
            }
            else {
                this.updateUIByParas(props);
            }

            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            var paras = this.getParasByUI();
            paras["lock"] = false;
            paras.Remove("guid");
            Native.setCommonCfg("CFG_SP_BEAM_PROPS", "$runtime", paras, true, false);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            var paras = this.getParasByUI();
            paras["lock"] = false;
            paras.Remove("guid");
            Native.setCommonCfg("CFG_SP_BEAM_PROPS", "$runtime", paras, true, false);
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            Control root = null;
            if (sender != null 
                && sender != btn_modify
                && sender is UserCtrl.IUserCtrl)
                root = sender as Control;

            var paras = this.getParasByUI(root);
            //if (paras.ContainsKey("profile")) {
            //    if (!Native.isProfileNameValid(paras.GetString("profile"))) {
            //        MessageBox.Show("无效截面", "错误", MessageBoxButtons.OK);
            //        return;
            //    }
            //}

            if (paras.ContainsKey("work_transform")) {
                paras.GetDObject("work_transform")["work_axis"] = Native.focusedWorkAxis();
            }

            //Native.modifyCurObjects(paras, Define.ObjectType.OBJ_STEEL_PART_BEAM, Define.ObjectType.OBJ_STEEL_PART_POLY_BEAM);
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            LoongCAD.Util.DObject result = Native.curObjViewProps();
            if (result == null)
                return;

            LoongCAD.STC.Define.ObjectType objType = (LoongCAD.STC.Define.ObjectType)result["obj_type"];
            if (objType != LoongCAD.STC.Define.ObjectType.OBJ_STEEL_PART_BEAM
                && objType != LoongCAD.STC.Define.ObjectType.OBJ_STEEL_PART_POLY_BEAM)
                return;

            m_usrData.reset();
            this.updateUIByParas(result);
            this.setChecks(true);
        }

        private void btn_profile_Click(object sender, EventArgs e) {
            //ProfileExplorerDialog dlg = new ProfileExplorerDialog();
            //dlg.Owner = MainForm.instance;

            //var profile = Native.profileNameData2Disp(tb_profile.Value);
            //if (dlg.ShowModal(profile) == DialogResult.OK) {
            //    string newPrf = dlg.SelectedProfile;
            //    if (newPrf != null) {
            //        tb_profile.Value = newPrf;
            //    }
            //}
        }

        private void btn_mtrl_Click(object sender, EventArgs e) {
            
        }

        private void Btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks(null, new UserCtrl.Check[] { chb_offset_h, chb_offset_v, chb_rot,
                    chb_dx0, chb_dy0, chb_dz0, chb_dx1, chb_dy1, chb_dz1, chb_curve, chb_deform_start, chb_deform_end});
        }

        public override DObject getParasByUI(Control root = null, List<Control> exclude = null, bool check = true) {
            var props = base.getParasByUI(root, exclude, check);

            ////disp to data
            //if (props.ContainsKey("profile")) {
            //    props["profile"] = Native.profileNameDisp2Data(props.GetString("profile"));
            //}

            return props;
        }

        public override void updateUIByParas(DObject paras, Control root = null) {
            ////disp to data
            //if (paras.ContainsKey("profile")) {
            //    paras["profile"] = Native.profileNameData2Disp(paras.GetString("profile"));
            //}

            base.updateUIByParas(paras, root);
        }


    }
}
