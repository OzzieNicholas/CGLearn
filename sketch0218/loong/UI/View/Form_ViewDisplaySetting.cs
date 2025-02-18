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
using IFWrapper;

namespace UI
{
    public partial class Form_ViewDisplaySetting : UI.ParaForm
    {
        private static Form_ViewDisplaySetting m_instance = new Form_ViewDisplaySetting();
        public static Form_ViewDisplaySetting instance => m_instance;

        protected Dictionary<string, string> m_partLabel2Prop = new Dictionary<string, string>() {
                {"构件编号", "ass_no" },
                {"零件编号", "part_no" },
                //{"砼构件编号", "cast_unit_no" },
                {"材质", "mtrl" },
                {"名称", "name" },
                {"截面型材", "profile" },
                {"类别", "class" },
            };

        protected Dictionary<string, string> m_partProp2Label = new Dictionary<string, string>() {
                {"ass_no", "构件编号" },
                {"part_no", "零件编号" },
                //{"cast_unit_no", "砼构件编号" },
                {"mtrl", "材质" },
                {"name", "名称" },
                {"profile", "截面型材" },
                {"class", "类别" },
            };

        protected UserCtrl.Check[] m_visCtrls = null;
        protected UserCtrl.Check[] m_compVisCtrls = null;

        public Form_ViewDisplaySetting() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            m_visCtrls = new UserCtrl.Check[] {
                chb_pt_vis,
                chb_part_vis,
                chb_bolt_vis,
                chb_hole_vis,
                chb_weld_vis,
                chb_construct_pl_vis,
                chb_rfbar_vis,
                chb_surf_treat_vis,
                chb_cut_vis,
                chb_fitting_vis,
                chb_comp_symbol_vis,
                chb_grid_vis,
                chb_construct_line_vis,
                chb_ref_obj_vis
            };

            m_compVisCtrls = new UserCtrl.Check[] {
                chb_part_vis_comp,
                chb_bolt_vis_comp,
                chb_hole_vis_comp,
                chb_weld_vis_comp,
                chb_construct_pl_vis_comp,
                chb_rfbar_vis_comp,
                chb_cut_vis_comp,
                chb_fitting_vis_comp,
                chb_comp_symbol_vis_comp
            };

            cbo_part_quality.ClearItems();
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_FAST, "快速");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_EXACT, "精确");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_REF_LINE, "参考线");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_DETAIL, "高精细(较慢)");
            cbo_part_quality.UpdateItems();

            cbo_part_mode.ClearItems();
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_WIREFRAME, "线框");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_SHADED_WIREFRAME, "半透明");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_HIDDEN_LINES, "隐藏线");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_EDGEFACE, "着色");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_SEL_ONLY, "仅选中");
            cbo_part_mode.UpdateItems();

            cbo_part_mode_comp.ClearItems();
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_WIREFRAME, "线框");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_SHADED_WIREFRAME, "半透明");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_HIDDEN_LINES, "隐藏线");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_EDGEFACE, "着色");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_SEL_ONLY, "仅选中");
            cbo_part_mode_comp.UpdateItems();

            rb_bolt_quality.labels = new string[] { "快速", "精确" };
            rb_bolt_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_bolt_quality.refresh();

            rb_hole_quality.labels = new string[] { "快速", "精确" };
            rb_hole_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_hole_quality.refresh();

            rb_weld_quality.labels = new string[] { "快速", "精确" };
            rb_weld_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_weld_quality.refresh();

            rb_rfbar_quality.labels = new string[] { "快速", "精确" };
            rb_rfbar_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_rfbar_quality.refresh();

            li_part_label_props.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
            li_part_label_active_props.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);

            li_part_label_props.Items.Clear();
            li_part_label_active_props.Items.Clear();
            li_part_label_props.Items.AddRange(this.partLabels());
            
            rb_pt_size_mode.labels = new string[] { "所有视图", "当前视图" };
            rb_pt_size_mode.keys = new object[] { 0, 1 };
            rb_pt_size_mode.refresh();
            
            this.updateTabOrder();
        }

        public Dictionary<string, string> partLabel2PropMap => m_partLabel2Prop;
        public Dictionary<string, string> partProp2LabelMap => m_partProp2Label;

        public void show(DObject props) {
            if (props == null)
                return;

            this.updateUIByParas(props);
            this._updateVisAll();

            if (this.Visible) {
                this.focus();
                return;
            }

            rb_pt_size_mode.Value = 0;                
            this.Visible = true;
            this.focus();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            if (Form_ViewProps.instance.Visible) {
                Form_ViewProps.instance.dispSetting = this.getParasByUI();
            }

            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            if (Form_ViewProps.instance.Visible) {
                Form_ViewProps.instance.dispSetting = this.getParasByUI();
            }
        }

        private void Btn_modify_Click(object sender, EventArgs e) {
            var paras = this.getParasByUI();

            Native.modifyDisplaySettingInFocusedViews(paras);

            var ptSizeMode = rb_pt_size_mode.Value;
            if (ptSizeMode != null && ptSizeMode.Equals(0))
                Native.modifyPointSizeInAllViews(tb_pt_size.Value);

            Native.redrawFocusedViews();
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            LoongCAD.Util.DObject qry = Native.focusedViewProps();
            if (qry == null || !qry.ContainsKey("disp_setting"))
                return;

            this.updateUIByParas(qry["disp_setting"] as DObject);
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks(this, new UserCtrl.Check[] {
                chb_vis_all, chb_vis_comp_all,
                chb_pt_vis,
                chb_part_vis, chb_part_vis_comp,
                chb_bolt_vis, chb_bolt_vis_comp,
                chb_hole_vis, chb_hole_vis_comp,
                chb_weld_vis, chb_weld_vis_comp,
                chb_construct_pl_vis, chb_construct_pl_vis_comp,
                chb_rfbar_vis, chb_rfbar_vis_comp,
                chb_surf_treat_vis,
                chb_cut_vis, chb_cut_vis_comp,
                chb_fitting_vis, chb_fitting_vis_comp,
                chb_comp_symbol_vis, chb_comp_symbol_vis_comp,
                chb_grid_vis, chb_construct_line_vis, chb_ref_obj_vis,
                chb_part_center_line_vis, chb_part_center_line_vis_comp,
                chb_part_ref_line_vis, chb_part_ref_line_vis_comp,
                chb_part_label_vis, chb_part_label_vis_comp,
                chb_connection_text_vis,
            });
        }

        public override void updateUIByParas(DObject paras, Control root = null) {
            base.updateUIByParas(paras, root);

            li_part_label_active_props.Items.Clear();
            if (paras.ContainsKey("part_label_props")) {
                string[] src = paras["part_label_props"] as string[];
                for (int i = 0; i < src.Length; ++i) {
                    if (m_partProp2Label.ContainsKey(src[i]))
                        li_part_label_active_props.Items.Add(m_partProp2Label[src[i]]);
                }
            }
        }

        public override LoongCAD.Util.DObject getParasByUI(Control root = null, List<Control> exclude = null, bool check = true) {
            DObject ret = base.getParasByUI(root, exclude, check);

            List<string> partLabel = new List<string>();
            foreach (string s in li_part_label_active_props.Items) {
                partLabel.Add(this.partLabel2Prop(s));
            }

            ret["part_label_props"] = partLabel.ToArray();
            return ret;
        }

        private void _updateVisAll() {
            //for vis all
            bool allCheck = true;
            foreach (var ctrl in m_visCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            chb_vis_all.Checked = allCheck;

            //for comp vis all
            allCheck = true;
            foreach (var ctrl in m_compVisCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            chb_vis_comp_all.Checked = allCheck;
        }

        private void chb_vis_all_Click(object sender, EventArgs e) {
            bool allCheck = true;
            foreach (var ctrl in m_visCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            foreach (var ctrl in m_visCtrls) {
                ctrl.Checked = !allCheck;
            }
        }

        private void chb_vis_comp_all_Click(object sender, EventArgs e) {
            bool allCheck = true;
            foreach (var ctrl in m_compVisCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            foreach (var ctrl in m_compVisCtrls) {
                ctrl.Checked = !allCheck;
            }
        }

        private void btn_part_lable_add_Click(object sender, EventArgs e) {
            if (li_part_label_props.SelectedItem == null)
                return;

            foreach (var item in li_part_label_props.SelectedItems) {
                if (li_part_label_active_props.Items.Contains(item))
                    continue;

                li_part_label_active_props.Items.Add(item);
            }
        }

        private void btn_part_label_remove_Click(object sender, EventArgs e) {
            if (li_part_label_active_props.SelectedItems.Count == 0)
                return;

            object[] selItems = new object[li_part_label_active_props.SelectedItems.Count];
            li_part_label_active_props.SelectedItems.CopyTo(selItems, 0);

            foreach (var item in selItems) {
                li_part_label_active_props.Items.Remove(item);
            }
        }

        public string[] partLabels() {
            return m_partLabel2Prop.Keys.ToArray();
        }

        public string partLabel2Prop(string lb) {
            m_partLabel2Prop.TryGetValue(lb, out string res);
            return res;
        }

    }
}
