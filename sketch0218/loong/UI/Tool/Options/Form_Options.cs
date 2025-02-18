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
using LoongCAD.Localization;
using IFWrapper;

namespace UI
{
    public partial class Form_Options : UI.ParaForm {
        public static Form_Options m_instance = new Form_Options();
        protected DObject m_props = new DObject();

        public static Form_Options instance => m_instance;

        public Form_Options() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            bar_cfg.init(this, "CFG_OPTIONS");

           
            //unit
            cbo_unit_model_dist.ClearItems();
            cbo_unit_model_dist.AddItem("mm", "mm");
            cbo_unit_model_dist.AddItem("cm", "cm");
            cbo_unit_model_dist.AddItem("m", "m");
            cbo_unit_model_dist.AddItem("in", "in");
            cbo_unit_model_dist.AddItem("ft", "ft");
            cbo_unit_model_dist.AddItem("ft-in", "ft-in");
            cbo_unit_model_dist.UpdateItems();

            cbo_unit_model_angl.ClearItems();
            cbo_unit_model_angl.AddItem("deg", Lang.Local("deg", "unit"));
            cbo_unit_model_angl.AddItem("rad", Lang.Local("rad", "unit"));
            cbo_unit_model_angl.UpdateItems();

            cbo_unit_model_spconst.ClearItems();
            cbo_unit_model_spconst.AddItem("kg/m", "kg/m");
            cbo_unit_model_spconst.AddItem("kg/cm", "kg/cm");
            cbo_unit_model_spconst.AddItem("kg/mm", "kg/mm");
            cbo_unit_model_spconst.AddItem("t/m", "T/m");
            cbo_unit_model_spconst.AddItem("t/cm", "T/cm");
            cbo_unit_model_spconst.AddItem("t/mm", "T/mm");
            cbo_unit_model_spconst.AddItem("n/m", "N/m");
            cbo_unit_model_spconst.AddItem("n/cm", "N/cm");
            cbo_unit_model_spconst.AddItem("n/mm", "N/mm");
            cbo_unit_model_spconst.AddItem("dan/m", "daN/m");
            cbo_unit_model_spconst.AddItem("dan/cm", "daN/cm");
            cbo_unit_model_spconst.AddItem("dan/mm", "daN/mm");
            cbo_unit_model_spconst.AddItem("kn/m", "kN/m");
            cbo_unit_model_spconst.AddItem("kn/cm", "kN/cm");
            cbo_unit_model_spconst.AddItem("kn/mm", "kN/mm");
            cbo_unit_model_spconst.AddItem("lbf/in", "lbf/in");
            cbo_unit_model_spconst.AddItem("lbf/ft", "lbf/ft");
            cbo_unit_model_spconst.AddItem("kip/in", "kip/in");
            cbo_unit_model_spconst.AddItem("kip/ft", "kip/ft");
            cbo_unit_model_spconst.UpdateItems();

            cbo_unit_model_rot_spconst.ClearItems();
            cbo_unit_model_rot_spconst.AddItem("kgm/rad", "kgm/rad");
            cbo_unit_model_rot_spconst.AddItem("kgm/angl", "kgm/angle");
            cbo_unit_model_rot_spconst.AddItem("tm/rad", "Tm/rad");
            cbo_unit_model_rot_spconst.AddItem("tm/angl", "Tm/angle");
            cbo_unit_model_rot_spconst.AddItem("nm/rad", "Nm/rad");
            cbo_unit_model_rot_spconst.AddItem("nm/angl", "Nm/angle");
            cbo_unit_model_rot_spconst.AddItem("danm/rad", "daNm/rad");
            cbo_unit_model_rot_spconst.AddItem("danm/angl", "daNm/angle");
            cbo_unit_model_rot_spconst.AddItem("knm/rad", "kNm/rad");
            cbo_unit_model_rot_spconst.AddItem("knm/angl", "kNm/angle");
            cbo_unit_model_rot_spconst.AddItem("lbf-in/rad", "lbf-in/rad");
            cbo_unit_model_rot_spconst.AddItem("lbf-in/angl", "lbf-in/angle");
            cbo_unit_model_rot_spconst.AddItem("lbf-ft/rad", "lbf-ft/rad");
            cbo_unit_model_rot_spconst.AddItem("lbf-ft/angl", "lbf-ft/angle");
            cbo_unit_model_rot_spconst.AddItem("kip-in/rad", "kip-in/rad");
            cbo_unit_model_rot_spconst.AddItem("kip-in/angl", "kip-in/angle");
            cbo_unit_model_rot_spconst.AddItem("kip-ft/rad", "kip-ft/rad");
            cbo_unit_model_rot_spconst.AddItem("kip-ft/angl", "kip-ft/angle");
            cbo_unit_model_rot_spconst.UpdateItems();

            cbo_unit_model_force.ClearItems();
            cbo_unit_model_force.AddItem("kg", "kg");
            cbo_unit_model_force.AddItem("t", "T");
            cbo_unit_model_force.AddItem("n", "N");
            cbo_unit_model_force.AddItem("dan", "daN");
            cbo_unit_model_force.AddItem("kn", "kN");
            cbo_unit_model_force.AddItem("lbf", "lbf");
            cbo_unit_model_force.AddItem("kip", "kip");
            cbo_unit_model_force.UpdateItems();

            cbo_unit_model_distributed_load.ClearItems();
            cbo_unit_model_distributed_load.AddItem("kg/m", "kg/m");
            cbo_unit_model_distributed_load.AddItem("t/m", "T/m");
            cbo_unit_model_distributed_load.AddItem("n/m", "N/m");
            cbo_unit_model_distributed_load.AddItem("dan/m", "daN/m");
            cbo_unit_model_distributed_load.AddItem("kn/m", "kN/m");
            cbo_unit_model_distributed_load.AddItem("lbf/in", "lbf/in");
            cbo_unit_model_distributed_load.AddItem("lbf/ft", "lbf/ft");
            cbo_unit_model_distributed_load.AddItem("kip/in", "kip/in");
            cbo_unit_model_distributed_load.AddItem("kip/ft", "kip/ft");
            cbo_unit_model_distributed_load.UpdateItems();

            cbo_unit_model_surface_load.ClearItems();
            cbo_unit_model_surface_load.AddItem("kg/m2", "kg/m2");
            cbo_unit_model_surface_load.AddItem("t/m2", "T/m2");
            cbo_unit_model_surface_load.AddItem("n/m2", "N/m2");
            cbo_unit_model_surface_load.AddItem("dan/m2", "daN/m2");
            cbo_unit_model_surface_load.AddItem("kn/m2", "kN/m2");
            cbo_unit_model_surface_load.AddItem("lbf/in2", "lbf/in2");
            cbo_unit_model_surface_load.AddItem("lbf/ft2", "lbf/ft2");
            cbo_unit_model_surface_load.AddItem("kip/in2", "kip/in2");
            cbo_unit_model_surface_load.AddItem("kip/ft2", "kip/ft2");
            cbo_unit_model_surface_load.UpdateItems();

            cbo_unit_model_moment.ClearItems();
            cbo_unit_model_moment.AddItem("kgm", "kgm");
            cbo_unit_model_moment.AddItem("tm", "Tm");
            cbo_unit_model_moment.AddItem("nm", "Nm");
            cbo_unit_model_moment.AddItem("danm", "daNm");
            cbo_unit_model_moment.AddItem("knm", "kNm");
            cbo_unit_model_moment.AddItem("lbf-in", "lbf-in");
            cbo_unit_model_moment.AddItem("lbf-ft", "lbf-ft");
            cbo_unit_model_moment.AddItem("kip-in", "kip-in");
            cbo_unit_model_moment.AddItem("kip-ft", "kip-ft");
            cbo_unit_model_moment.UpdateItems();

            cbo_unit_model_distributed_moment.ClearItems();
            cbo_unit_model_distributed_moment.AddItem("kgm/m", "kgm/m");
            cbo_unit_model_distributed_moment.AddItem("tm/m", "Tm/m");
            cbo_unit_model_distributed_moment.AddItem("nm/m", "Nm/m");
            cbo_unit_model_distributed_moment.AddItem("danm/m", "daNm/m");
            cbo_unit_model_distributed_moment.AddItem("knm/m", "kNm/m");
            cbo_unit_model_distributed_moment.AddItem("lbf-ft/ft", "lbf-ft/ft");
            cbo_unit_model_distributed_moment.AddItem("kip-ft/ft", "kip-ft/ft");
            cbo_unit_model_distributed_moment.UpdateItems();

            cbo_unit_model_temperature.ClearItems();
            cbo_unit_model_temperature.AddItem("k", Lang.Local("k", "unit"));
            cbo_unit_model_temperature.AddItem("f", Lang.Local("f", "unit"));
            cbo_unit_model_temperature.UpdateItems();

            cbo_unit_model_deform.ClearItems();
            cbo_unit_model_deform.AddItem("mm", "mm");
            cbo_unit_model_deform.AddItem("cm", "cm");
            cbo_unit_model_deform.AddItem("m", "m");
            cbo_unit_model_deform.AddItem("in", "in");
            cbo_unit_model_deform.AddItem("ft", "ft");
            cbo_unit_model_deform.AddItem("ft-in", "ft-in");
            cbo_unit_model_deform.UpdateItems();

            cbo_unit_prf_sect_dim.ClearItems();
            cbo_unit_prf_sect_dim.AddItem("mm", "mm");
            cbo_unit_prf_sect_dim.AddItem("cm", "cm");
            cbo_unit_prf_sect_dim.AddItem("m", "m");
            cbo_unit_prf_sect_dim.AddItem("in", "in");
            cbo_unit_prf_sect_dim.AddItem("ft", "ft");
            cbo_unit_prf_sect_dim.UpdateItems();

            cbo_unit_prf_angl.ClearItems();
            cbo_unit_prf_angl.AddItem("deg", Lang.Local("deg", "unit"));
            cbo_unit_prf_angl.AddItem("rad", Lang.Local("rad", "unit"));
            cbo_unit_prf_angl.UpdateItems();

            cbo_unit_prf_area.ClearItems();
            cbo_unit_prf_area.AddItem("mm2", "mm2");
            cbo_unit_prf_area.AddItem("cm2", "cm2");
            cbo_unit_prf_area.AddItem("m2", "m2");
            cbo_unit_prf_area.AddItem("in2", "in2");
            cbo_unit_prf_area.AddItem("ft2", "ft2");
            cbo_unit_prf_area.UpdateItems();

            cbo_unit_prf_sect_modulus.ClearItems();
            cbo_unit_prf_sect_modulus.AddItem("mm3", "mm3");
            cbo_unit_prf_sect_modulus.AddItem("cm3", "cm3");
            cbo_unit_prf_sect_modulus.AddItem("m3", "m3");
            cbo_unit_prf_sect_modulus.AddItem("in3", "in3");
            cbo_unit_prf_sect_modulus.AddItem("ft3", "ft3");
            cbo_unit_prf_sect_modulus.UpdateItems();

            cbo_unit_prf_moment_inertia.ClearItems();
            cbo_unit_prf_moment_inertia.AddItem("mm4", "mm4");
            cbo_unit_prf_moment_inertia.AddItem("cm4", "cm4");
            cbo_unit_prf_moment_inertia.AddItem("in4", "in4");
            cbo_unit_prf_moment_inertia.UpdateItems();

            cbo_unit_prf_radius_inertia.ClearItems();
            cbo_unit_prf_radius_inertia.AddItem("mm", "mm");
            cbo_unit_prf_radius_inertia.AddItem("cm", "cm");
            cbo_unit_prf_radius_inertia.AddItem("m", "m");
            cbo_unit_prf_radius_inertia.AddItem("in", "in");
            cbo_unit_prf_radius_inertia.AddItem("ft", "ft");
            cbo_unit_prf_radius_inertia.UpdateItems();

            cbo_unit_prf_torsion_const.ClearItems();
            cbo_unit_prf_torsion_const.AddItem("mm4", "mm4");
            cbo_unit_prf_torsion_const.AddItem("cm4", "cm4");
            cbo_unit_prf_torsion_const.AddItem("in4", "in4");
            cbo_unit_prf_torsion_const.UpdateItems();

            cbo_unit_prf_warping_const.ClearItems();
            cbo_unit_prf_warping_const.AddItem("mm6", "mm6");
            cbo_unit_prf_warping_const.AddItem("cm6", "cm6");
            cbo_unit_prf_warping_const.AddItem("in6", "in6");
            cbo_unit_prf_warping_const.UpdateItems();

            cbo_unit_prf_cover_area.ClearItems();
            cbo_unit_prf_cover_area.AddItem("m2/m", "m2/m");
            cbo_unit_prf_cover_area.AddItem("mm2/m", "mm2/m");
            cbo_unit_prf_cover_area.AddItem("cm2/m", "cm2/m");
            cbo_unit_prf_cover_area.AddItem("ft2/ft", "ft2/ft");
            cbo_unit_prf_cover_area.AddItem("in2/ft", "in2/ft");
            cbo_unit_prf_cover_area.AddItem("in2/in", "in2/in");
            cbo_unit_prf_cover_area.UpdateItems();

            cbo_unit_mtrl_strength.ClearItems();
            cbo_unit_mtrl_strength.AddItem("kg/m2", "kg/m2");
            cbo_unit_mtrl_strength.AddItem("kg/cm2", "kg/cm2");
            cbo_unit_mtrl_strength.AddItem("kg/mm2", "kg/mm2");
            cbo_unit_mtrl_strength.AddItem("t/m2", "T/m2");
            cbo_unit_mtrl_strength.AddItem("t/cm2", "T/cm2");
            cbo_unit_mtrl_strength.AddItem("t/mm2", "T/mm2");
            cbo_unit_mtrl_strength.AddItem("n/m2", "N/m2");
            cbo_unit_mtrl_strength.AddItem("n/cm2", "N/cm2");
            cbo_unit_mtrl_strength.AddItem("n/mm2", "N/mm2");
            cbo_unit_mtrl_strength.AddItem("dan/m2", "daN/m2");
            cbo_unit_mtrl_strength.AddItem("dan/cm2", "daN/cm2");
            cbo_unit_mtrl_strength.AddItem("dan/mm2", "daN/mm2");
            cbo_unit_mtrl_strength.AddItem("kn/m2", "kN/m2");
            cbo_unit_mtrl_strength.AddItem("kn/cm2", "kN/cm2");
            cbo_unit_mtrl_strength.AddItem("kn/mm2", "kN/mm2");
            cbo_unit_mtrl_strength.AddItem("lbf/in2", "lbf/in2");
            cbo_unit_mtrl_strength.AddItem("lbf/ft2", "lbf/ft2");
            cbo_unit_mtrl_strength.AddItem("kip/in2", "kip/in2");
            cbo_unit_mtrl_strength.AddItem("kip/ft2", "kip/ft2");
            cbo_unit_mtrl_strength.UpdateItems();

            cbo_unit_mtrl_modulus.ClearItems();
            cbo_unit_mtrl_modulus.AddItem("kg/m2", "kg/m2");
            cbo_unit_mtrl_modulus.AddItem("kg/cm2", "kg/cm2");
            cbo_unit_mtrl_modulus.AddItem("kg/mm2", "kg/mm2");
            cbo_unit_mtrl_modulus.AddItem("t/m2", "T/m2");
            cbo_unit_mtrl_modulus.AddItem("t/cm2", "T/cm2");
            cbo_unit_mtrl_modulus.AddItem("t/mm2", "T/mm2");
            cbo_unit_mtrl_modulus.AddItem("n/m2", "N/m2");
            cbo_unit_mtrl_modulus.AddItem("n/cm2", "N/cm2");
            cbo_unit_mtrl_modulus.AddItem("n/mm2", "N/mm2");
            cbo_unit_mtrl_modulus.AddItem("dan/m2", "daN/m2");
            cbo_unit_mtrl_modulus.AddItem("dan/cm2", "daN/cm2");
            cbo_unit_mtrl_modulus.AddItem("dan/mm2", "daN/mm2");
            cbo_unit_mtrl_modulus.AddItem("kn/m2", "kN/m2");
            cbo_unit_mtrl_modulus.AddItem("kn/cm2", "kN/cm2");
            cbo_unit_mtrl_modulus.AddItem("kn/mm2", "kN/mm2");
            cbo_unit_mtrl_modulus.AddItem("lbf/in2", "lbf/in2");
            cbo_unit_mtrl_modulus.AddItem("lbf/ft2", "lbf/ft2");
            cbo_unit_mtrl_modulus.AddItem("kip/in2", "kip/in2");
            cbo_unit_mtrl_modulus.AddItem("kip/ft2", "kip/ft2");
            cbo_unit_mtrl_modulus.UpdateItems();

            cbo_unit_mtrl_density.ClearItems();
            cbo_unit_mtrl_density.AddItem("kg/m3", "kg/m3");
            cbo_unit_mtrl_density.AddItem("t/m3", "T/m3");
            cbo_unit_mtrl_density.AddItem("n/m3", "N/m3");
            cbo_unit_mtrl_density.AddItem("kn/m3", "kN/m3");
            cbo_unit_mtrl_density.AddItem("lbf/ft3", "lbf/ft3");
            cbo_unit_mtrl_density.UpdateItems();

            cbo_unit_mtrl_weight.ClearItems();
            cbo_unit_mtrl_weight.AddItem("kg", "kg");
            cbo_unit_mtrl_weight.AddItem("t", "T");
            cbo_unit_mtrl_weight.AddItem("n", "N");
            cbo_unit_mtrl_weight.AddItem("lbf", "lbf");
            cbo_unit_mtrl_weight.AddItem("kip", "kip");
            cbo_unit_mtrl_weight.UpdateItems();

            cbo_unit_mtrl_strain.ClearItems();
            cbo_unit_mtrl_strain.AddItem("%", "%");
            cbo_unit_mtrl_strain.AddItem("‰", "‰");
            cbo_unit_mtrl_strain.UpdateItems();

            cbo_unit_mtrl_thermal_coeff.ClearItems();
            cbo_unit_mtrl_thermal_coeff.AddItem("1/k", "1/K");
            cbo_unit_mtrl_thermal_coeff.AddItem("1/f", "1/F");
            cbo_unit_mtrl_thermal_coeff.UpdateItems();

            cbo_unit_mtrl_ratio.ClearItems();
            cbo_unit_mtrl_ratio.AddItem("%", "%");
            cbo_unit_mtrl_ratio.AddItem("‰", "‰");
            cbo_unit_mtrl_ratio.UpdateItems();

            cbo_unit_mtrl_volume.ClearItems();
            cbo_unit_mtrl_volume.AddItem("mm3", "mm3");
            cbo_unit_mtrl_volume.AddItem("cm3", "cm3");
            cbo_unit_mtrl_volume.AddItem("m3", "m3");
            cbo_unit_mtrl_volume.AddItem("in3", "in3");
            cbo_unit_mtrl_volume.AddItem("ft3", "ft3");
            cbo_unit_mtrl_volume.UpdateItems();

            Callback.regUICmdListener(onCommand, "on_doc_new");
            Callback.regUICmdListener(onCommand, "on_doc_loaded");

            this.updateTabOrder();
        }

        public DObject onCommand(string cmd, DObject paras) {
            switch (cmd) {
                case "on_doc_new":
                case "on_doc_loaded": {
                    this.reset();
                    this.hide();
                }
                break;
            }

            return null;
        }

        public DObject props {
            get => m_props;
        }

        public void reset() {
            bar_cfg.reset();

            m_props = Native.options();
            this.updateUIByParas(m_props);
        }

        public void show() {
            this.reset();

            if (this.Visible) {
                this.focus();
                return;
            }

            this.Visible = true;
            this.focus();
        }

        public string str(double v, string unitData, string unitEnobType) {
            if (m_props == null
                || String.IsNullOrEmpty(unitEnobType))
                return v.ToString();

            object unitDisp = unitData;
            object enob = 2;
            m_props.TryGetValue("unit_" + unitEnobType, out unitDisp);
            m_props.TryGetValue("enob_" + unitEnobType, out enob);

            return Unit.Str(v, unitData, unitDisp as string, (int)enob);
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            if (Native.isDocumentReadOnly()) {
                MessageBox.Show("无法修改只读模型", "提示", MessageBoxButtons.OK);
                return;
            }

            m_props.CopyFrom(this.getParasByUI());
            Native.setOptions(m_props);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            if (Native.isDocumentReadOnly()) {
                MessageBox.Show("无法修改只读模型", "提示", MessageBoxButtons.OK);
                return;
            }

            m_props.CopyFrom(this.getParasByUI());
            Native.setOptions(m_props);
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        public override DObject getParasByUI(Control root = null, List<Control> exclude = null, bool check = true) {
            var res = base.getParasByUI(root, exclude, check);
            return res;
        }

        public override void updateUIByParas(DObject paras, Control root = null) {
            if (paras == null)
                return;

            base.updateUIByParas(paras, root);
        }



    }
}
