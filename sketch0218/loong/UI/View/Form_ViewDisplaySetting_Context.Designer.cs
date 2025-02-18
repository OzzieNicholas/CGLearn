namespace UI
{
    partial class Form_ViewDisplaySetting_Context
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.tc_root = new System.Windows.Forms.TabControl();
            this.tp_props = new System.Windows.Forms.TabPage();
            this.rb_rfbar_quality = new UserCtrl.RadioButtons();
            this.rb_weld_quality = new UserCtrl.RadioButtons();
            this.rb_hole_quality = new UserCtrl.RadioButtons();
            this.rb_bolt_quality = new UserCtrl.RadioButtons();
            this.sepLine6 = new UserCtrl.SepLine();
            this.sepLine5 = new UserCtrl.SepLine();
            this.sepLine4 = new UserCtrl.SepLine();
            this.sepLine3 = new UserCtrl.SepLine();
            this.sepLine2 = new UserCtrl.SepLine();
            this.sepLine1 = new UserCtrl.SepLine();
            this.sep = new UserCtrl.SepLine();
            this.cbo_part_mode_comp = new UserCtrl.ComboBoxKey();
            this.label1 = new System.Windows.Forms.Label();
            this.cbo_part_mode = new UserCtrl.ComboBoxKey();
            this.lb_mode = new System.Windows.Forms.Label();
            this.lb_quality = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.chb_comp_symbol_vis_comp = new UserCtrl.Check();
            this.label5 = new System.Windows.Forms.Label();
            this.chb_fitting_vis_comp = new UserCtrl.Check();
            this.chb_cut_vis_comp = new UserCtrl.Check();
            this.label4 = new System.Windows.Forms.Label();
            this.chb_ref_obj_vis = new UserCtrl.Check();
            this.label3 = new System.Windows.Forms.Label();
            this.chb_construct_line_vis = new UserCtrl.Check();
            this.chb_rfbar_vis_comp = new UserCtrl.Check();
            this.chb_grid_vis = new UserCtrl.Check();
            this.cbo_part_quality = new UserCtrl.ComboBoxKey();
            this.chb_comp_symbol_vis = new UserCtrl.Check();
            this.chb_construct_pl_vis_comp = new UserCtrl.Check();
            this.chb_fitting_vis = new UserCtrl.Check();
            this.label2 = new System.Windows.Forms.Label();
            this.chb_cut_vis = new UserCtrl.Check();
            this.chb_weld_vis_comp = new UserCtrl.Check();
            this.lb_pt_vis = new System.Windows.Forms.Label();
            this.chb_surf_treat_vis = new UserCtrl.Check();
            this.chb_hole_vis_comp = new UserCtrl.Check();
            this.chb_rfbar_vis = new UserCtrl.Check();
            this.lb_part_vis = new System.Windows.Forms.Label();
            this.chb_construct_pl_vis = new UserCtrl.Check();
            this.chb_bolt_vis_comp = new UserCtrl.Check();
            this.chb_weld_vis = new UserCtrl.Check();
            this.lb_all_vis = new System.Windows.Forms.Label();
            this.chb_hole_vis = new UserCtrl.Check();
            this.chb_part_vis_comp = new UserCtrl.Check();
            this.chb_bolt_vis = new UserCtrl.Check();
            this.chb_vis_comp_all = new UserCtrl.Check();
            this.chb_part_vis = new UserCtrl.Check();
            this.chb_pt_vis = new UserCtrl.Check();
            this.chb_vis_all = new UserCtrl.Check();
            this.lb_vis_model = new System.Windows.Forms.Label();
            this.lb_vis_comp = new System.Windows.Forms.Label();
            this.tp_advance = new System.Windows.Forms.TabPage();
            this.gb_connect = new System.Windows.Forms.GroupBox();
            this.chb_connection_text_vis = new UserCtrl.Check();
            this.gb_point = new System.Windows.Forms.GroupBox();
            this.rb_pt_size_mode = new UserCtrl.RadioButtons();
            this.tb_pt_size = new UserCtrl.InputDouble();
            this.label17 = new System.Windows.Forms.Label();
            this.gb_part = new System.Windows.Forms.GroupBox();
            this.gb_part_vis = new System.Windows.Forms.GroupBox();
            this.chb_part_label_vis = new UserCtrl.Check();
            this.chb_part_label_vis_comp = new UserCtrl.Check();
            this.chb_part_label_vis_ass_main = new UserCtrl.Check();
            this.btn_part_label_remove = new System.Windows.Forms.Button();
            this.sepLine7 = new UserCtrl.SepLine();
            this.label8 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.chb_part_ref_line_vis_comp = new UserCtrl.Check();
            this.chb_part_ref_line_vis = new UserCtrl.Check();
            this.label15 = new System.Windows.Forms.Label();
            this.chb_part_center_line_vis_comp = new UserCtrl.Check();
            this.li_part_label_active_props = new System.Windows.Forms.ListBox();
            this.chb_part_center_line_vis = new UserCtrl.Check();
            this.li_part_label_props = new System.Windows.Forms.ListBox();
            this.btn_part_lable_add = new System.Windows.Forms.Button();
            this.tc_root.SuspendLayout();
            this.tp_props.SuspendLayout();
            this.tp_advance.SuspendLayout();
            this.gb_connect.SuspendLayout();
            this.gb_point.SuspendLayout();
            this.gb_part.SuspendLayout();
            this.gb_part_vis.SuspendLayout();
            this.SuspendLayout();
            // 
            // tc_root
            // 
            this.tc_root.Controls.Add(this.tp_props);
            this.tc_root.Controls.Add(this.tp_advance);
            this.tc_root.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tc_root.Location = new System.Drawing.Point(0, 0);
            this.tc_root.Name = "tc_root";
            this.tc_root.SelectedIndex = 0;
            this.tc_root.Size = new System.Drawing.Size(488, 574);
            this.tc_root.TabIndex = 544;
            // 
            // tp_props
            // 
            this.tp_props.Controls.Add(this.rb_rfbar_quality);
            this.tp_props.Controls.Add(this.rb_weld_quality);
            this.tp_props.Controls.Add(this.rb_hole_quality);
            this.tp_props.Controls.Add(this.rb_bolt_quality);
            this.tp_props.Controls.Add(this.sepLine6);
            this.tp_props.Controls.Add(this.sepLine5);
            this.tp_props.Controls.Add(this.sepLine4);
            this.tp_props.Controls.Add(this.sepLine3);
            this.tp_props.Controls.Add(this.sepLine2);
            this.tp_props.Controls.Add(this.sepLine1);
            this.tp_props.Controls.Add(this.sep);
            this.tp_props.Controls.Add(this.cbo_part_mode_comp);
            this.tp_props.Controls.Add(this.label1);
            this.tp_props.Controls.Add(this.cbo_part_mode);
            this.tp_props.Controls.Add(this.lb_mode);
            this.tp_props.Controls.Add(this.lb_quality);
            this.tp_props.Controls.Add(this.label14);
            this.tp_props.Controls.Add(this.label13);
            this.tp_props.Controls.Add(this.label12);
            this.tp_props.Controls.Add(this.label11);
            this.tp_props.Controls.Add(this.label10);
            this.tp_props.Controls.Add(this.label9);
            this.tp_props.Controls.Add(this.label7);
            this.tp_props.Controls.Add(this.label6);
            this.tp_props.Controls.Add(this.chb_comp_symbol_vis_comp);
            this.tp_props.Controls.Add(this.label5);
            this.tp_props.Controls.Add(this.chb_fitting_vis_comp);
            this.tp_props.Controls.Add(this.chb_cut_vis_comp);
            this.tp_props.Controls.Add(this.label4);
            this.tp_props.Controls.Add(this.chb_ref_obj_vis);
            this.tp_props.Controls.Add(this.label3);
            this.tp_props.Controls.Add(this.chb_construct_line_vis);
            this.tp_props.Controls.Add(this.chb_rfbar_vis_comp);
            this.tp_props.Controls.Add(this.chb_grid_vis);
            this.tp_props.Controls.Add(this.cbo_part_quality);
            this.tp_props.Controls.Add(this.chb_comp_symbol_vis);
            this.tp_props.Controls.Add(this.chb_construct_pl_vis_comp);
            this.tp_props.Controls.Add(this.chb_fitting_vis);
            this.tp_props.Controls.Add(this.label2);
            this.tp_props.Controls.Add(this.chb_cut_vis);
            this.tp_props.Controls.Add(this.chb_weld_vis_comp);
            this.tp_props.Controls.Add(this.lb_pt_vis);
            this.tp_props.Controls.Add(this.chb_surf_treat_vis);
            this.tp_props.Controls.Add(this.chb_hole_vis_comp);
            this.tp_props.Controls.Add(this.chb_rfbar_vis);
            this.tp_props.Controls.Add(this.lb_part_vis);
            this.tp_props.Controls.Add(this.chb_construct_pl_vis);
            this.tp_props.Controls.Add(this.chb_bolt_vis_comp);
            this.tp_props.Controls.Add(this.chb_weld_vis);
            this.tp_props.Controls.Add(this.lb_all_vis);
            this.tp_props.Controls.Add(this.chb_hole_vis);
            this.tp_props.Controls.Add(this.chb_part_vis_comp);
            this.tp_props.Controls.Add(this.chb_bolt_vis);
            this.tp_props.Controls.Add(this.chb_vis_comp_all);
            this.tp_props.Controls.Add(this.chb_part_vis);
            this.tp_props.Controls.Add(this.chb_pt_vis);
            this.tp_props.Controls.Add(this.chb_vis_all);
            this.tp_props.Controls.Add(this.lb_vis_model);
            this.tp_props.Controls.Add(this.lb_vis_comp);
            this.tp_props.Location = new System.Drawing.Point(4, 22);
            this.tp_props.Name = "tp_props";
            this.tp_props.Padding = new System.Windows.Forms.Padding(3);
            this.tp_props.Size = new System.Drawing.Size(480, 548);
            this.tp_props.TabIndex = 0;
            this.tp_props.Text = "一般";
            this.tp_props.UseVisualStyleBackColor = true;
            // 
            // rb_rfbar_quality
            // 
            this.rb_rfbar_quality.BackColor = System.Drawing.SystemColors.Control;
            this.rb_rfbar_quality.btnCount = 2;
            this.rb_rfbar_quality.btnSize = new System.Drawing.Size(42, 21);
            this.rb_rfbar_quality.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_rfbar_quality.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_rfbar_quality.keys = new object[] {
        ((object)(0))};
            this.rb_rfbar_quality.keys1 = new object[] {
        ((object)(0))};
            this.rb_rfbar_quality.labels = new string[] {
        ""};
            this.rb_rfbar_quality.Location = new System.Drawing.Point(206, 268);
            this.rb_rfbar_quality.Margin = new System.Windows.Forms.Padding(0);
            this.rb_rfbar_quality.Name = "rb_rfbar_quality";
            this.rb_rfbar_quality.PropName = "rfbar_quality";
            this.rb_rfbar_quality.Size = new System.Drawing.Size(87, 21);
            this.rb_rfbar_quality.TabIndex = 91;
            // 
            // rb_weld_quality
            // 
            this.rb_weld_quality.BackColor = System.Drawing.SystemColors.Control;
            this.rb_weld_quality.btnCount = 2;
            this.rb_weld_quality.btnSize = new System.Drawing.Size(42, 21);
            this.rb_weld_quality.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_weld_quality.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_weld_quality.keys = new object[] {
        ((object)(0))};
            this.rb_weld_quality.keys1 = new object[] {
        ((object)(0))};
            this.rb_weld_quality.labels = new string[] {
        ""};
            this.rb_weld_quality.Location = new System.Drawing.Point(206, 225);
            this.rb_weld_quality.Margin = new System.Windows.Forms.Padding(0);
            this.rb_weld_quality.Name = "rb_weld_quality";
            this.rb_weld_quality.PropName = "weld_quality";
            this.rb_weld_quality.Size = new System.Drawing.Size(87, 21);
            this.rb_weld_quality.TabIndex = 90;
            // 
            // rb_hole_quality
            // 
            this.rb_hole_quality.BackColor = System.Drawing.SystemColors.Control;
            this.rb_hole_quality.btnCount = 2;
            this.rb_hole_quality.btnSize = new System.Drawing.Size(42, 21);
            this.rb_hole_quality.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_hole_quality.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_hole_quality.keys = new object[] {
        ((object)(0))};
            this.rb_hole_quality.keys1 = new object[] {
        ((object)(0))};
            this.rb_hole_quality.labels = new string[] {
        ""};
            this.rb_hole_quality.Location = new System.Drawing.Point(206, 197);
            this.rb_hole_quality.Margin = new System.Windows.Forms.Padding(0);
            this.rb_hole_quality.Name = "rb_hole_quality";
            this.rb_hole_quality.PropName = "hole_quality";
            this.rb_hole_quality.Size = new System.Drawing.Size(87, 21);
            this.rb_hole_quality.TabIndex = 89;
            // 
            // rb_bolt_quality
            // 
            this.rb_bolt_quality.BackColor = System.Drawing.SystemColors.Control;
            this.rb_bolt_quality.btnCount = 2;
            this.rb_bolt_quality.btnSize = new System.Drawing.Size(42, 21);
            this.rb_bolt_quality.CheckName = "chb_offset_h";
            this.rb_bolt_quality.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_bolt_quality.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_bolt_quality.keys = new object[] {
        ((object)(0))};
            this.rb_bolt_quality.keys1 = new object[] {
        ((object)(0))};
            this.rb_bolt_quality.labels = new string[] {
        ""};
            this.rb_bolt_quality.Location = new System.Drawing.Point(206, 169);
            this.rb_bolt_quality.Margin = new System.Windows.Forms.Padding(0);
            this.rb_bolt_quality.Name = "rb_bolt_quality";
            this.rb_bolt_quality.PropName = "bolt_quality";
            this.rb_bolt_quality.Size = new System.Drawing.Size(87, 21);
            this.rb_bolt_quality.TabIndex = 88;
            // 
            // sepLine6
            // 
            this.sepLine6.BackColor = System.Drawing.Color.Transparent;
            this.sepLine6.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine6.Location = new System.Drawing.Point(11, 421);
            this.sepLine6.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine6.Name = "sepLine6";
            this.sepLine6.Size = new System.Drawing.Size(454, 8);
            this.sepLine6.TabIndex = 23;
            // 
            // sepLine5
            // 
            this.sepLine5.BackColor = System.Drawing.Color.Transparent;
            this.sepLine5.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine5.Location = new System.Drawing.Point(11, 385);
            this.sepLine5.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine5.Name = "sepLine5";
            this.sepLine5.Size = new System.Drawing.Size(454, 8);
            this.sepLine5.TabIndex = 23;
            // 
            // sepLine4
            // 
            this.sepLine4.BackColor = System.Drawing.Color.Transparent;
            this.sepLine4.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine4.Location = new System.Drawing.Point(11, 321);
            this.sepLine4.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine4.Name = "sepLine4";
            this.sepLine4.Size = new System.Drawing.Size(454, 8);
            this.sepLine4.TabIndex = 23;
            // 
            // sepLine3
            // 
            this.sepLine3.BackColor = System.Drawing.Color.Transparent;
            this.sepLine3.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine3.Location = new System.Drawing.Point(11, 253);
            this.sepLine3.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine3.Name = "sepLine3";
            this.sepLine3.Size = new System.Drawing.Size(454, 8);
            this.sepLine3.TabIndex = 23;
            // 
            // sepLine2
            // 
            this.sepLine2.BackColor = System.Drawing.Color.Transparent;
            this.sepLine2.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine2.Location = new System.Drawing.Point(11, 154);
            this.sepLine2.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine2.Name = "sepLine2";
            this.sepLine2.Size = new System.Drawing.Size(454, 8);
            this.sepLine2.TabIndex = 23;
            // 
            // sepLine1
            // 
            this.sepLine1.BackColor = System.Drawing.Color.Transparent;
            this.sepLine1.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine1.Location = new System.Drawing.Point(11, 97);
            this.sepLine1.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine1.Name = "sepLine1";
            this.sepLine1.Size = new System.Drawing.Size(454, 8);
            this.sepLine1.TabIndex = 23;
            // 
            // sep
            // 
            this.sep.BackColor = System.Drawing.Color.Transparent;
            this.sep.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sep.Location = new System.Drawing.Point(11, 61);
            this.sep.Margin = new System.Windows.Forms.Padding(4);
            this.sep.Name = "sep";
            this.sep.Size = new System.Drawing.Size(454, 8);
            this.sep.TabIndex = 23;
            // 
            // cbo_part_mode_comp
            // 
            this.cbo_part_mode_comp.CheckName = "chb_offsetH";
            this.cbo_part_mode_comp.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_part_mode_comp.FormattingEnabled = true;
            this.cbo_part_mode_comp.Location = new System.Drawing.Point(391, 126);
            this.cbo_part_mode_comp.Name = "cbo_part_mode_comp";
            this.cbo_part_mode_comp.PropName = "part_mode_comp";
            this.cbo_part_mode_comp.Size = new System.Drawing.Size(77, 21);
            this.cbo_part_mode_comp.TabIndex = 18;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(398, 110);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 17;
            this.label1.Text = "节点表示法";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cbo_part_mode
            // 
            this.cbo_part_mode.CheckName = "chb_offsetH";
            this.cbo_part_mode.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_part_mode.FormattingEnabled = true;
            this.cbo_part_mode.Location = new System.Drawing.Point(308, 126);
            this.cbo_part_mode.Name = "cbo_part_mode";
            this.cbo_part_mode.PropName = "part_mode";
            this.cbo_part_mode.Size = new System.Drawing.Size(77, 21);
            this.cbo_part_mode.TabIndex = 16;
            // 
            // lb_mode
            // 
            this.lb_mode.AutoSize = true;
            this.lb_mode.Location = new System.Drawing.Point(325, 110);
            this.lb_mode.Name = "lb_mode";
            this.lb_mode.Size = new System.Drawing.Size(41, 12);
            this.lb_mode.TabIndex = 15;
            this.lb_mode.Text = "表示法";
            this.lb_mode.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lb_quality
            // 
            this.lb_quality.AutoSize = true;
            this.lb_quality.Location = new System.Drawing.Point(226, 15);
            this.lb_quality.Name = "lb_quality";
            this.lb_quality.Size = new System.Drawing.Size(53, 12);
            this.lb_quality.TabIndex = 15;
            this.lb_quality.Text = "显示模式";
            this.lb_quality.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(11, 520);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(53, 12);
            this.label14.TabIndex = 3;
            this.label14.Text = "参考对象";
            this.label14.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(11, 463);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(41, 12);
            this.label13.TabIndex = 3;
            this.label13.Text = "辅助线";
            this.label13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(11, 435);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(29, 12);
            this.label12.TabIndex = 3;
            this.label12.Text = "轴线";
            this.label12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(11, 400);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(53, 12);
            this.label11.TabIndex = 3;
            this.label11.Text = "组件符号";
            this.label11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(12, 364);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(29, 12);
            this.label10.TabIndex = 3;
            this.label10.Text = "接合";
            this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(12, 336);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(29, 12);
            this.label9.TabIndex = 3;
            this.label9.Text = "切割";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(11, 299);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 3;
            this.label7.Text = "表面处理";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 271);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 12);
            this.label6.TabIndex = 3;
            this.label6.Text = "钢筋";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_comp_symbol_vis_comp
            // 
            this.chb_comp_symbol_vis_comp.AutoSize = true;
            this.chb_comp_symbol_vis_comp.Checked = true;
            this.chb_comp_symbol_vis_comp.CheckName = null;
            this.chb_comp_symbol_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_comp_symbol_vis_comp.Location = new System.Drawing.Point(148, 400);
            this.chb_comp_symbol_vis_comp.Name = "chb_comp_symbol_vis_comp";
            this.chb_comp_symbol_vis_comp.PropName = "comp_symbol_vis_comp";
            this.chb_comp_symbol_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_comp_symbol_vis_comp.TabIndex = 13;
            this.chb_comp_symbol_vis_comp.TabStop = false;
            this.chb_comp_symbol_vis_comp.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(11, 492);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 3;
            this.label5.Text = "辅助面";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_fitting_vis_comp
            // 
            this.chb_fitting_vis_comp.AutoSize = true;
            this.chb_fitting_vis_comp.Checked = true;
            this.chb_fitting_vis_comp.CheckName = null;
            this.chb_fitting_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_fitting_vis_comp.Location = new System.Drawing.Point(148, 364);
            this.chb_fitting_vis_comp.Name = "chb_fitting_vis_comp";
            this.chb_fitting_vis_comp.PropName = "fitting_vis_comp";
            this.chb_fitting_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_fitting_vis_comp.TabIndex = 13;
            this.chb_fitting_vis_comp.TabStop = false;
            this.chb_fitting_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_cut_vis_comp
            // 
            this.chb_cut_vis_comp.AutoSize = true;
            this.chb_cut_vis_comp.Checked = true;
            this.chb_cut_vis_comp.CheckName = null;
            this.chb_cut_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_vis_comp.Location = new System.Drawing.Point(148, 336);
            this.chb_cut_vis_comp.Name = "chb_cut_vis_comp";
            this.chb_cut_vis_comp.PropName = "cut_vis_comp";
            this.chb_cut_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_cut_vis_comp.TabIndex = 13;
            this.chb_cut_vis_comp.TabStop = false;
            this.chb_cut_vis_comp.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(11, 227);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 12);
            this.label4.TabIndex = 3;
            this.label4.Text = "焊接";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_ref_obj_vis
            // 
            this.chb_ref_obj_vis.AutoSize = true;
            this.chb_ref_obj_vis.Checked = true;
            this.chb_ref_obj_vis.CheckName = null;
            this.chb_ref_obj_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_ref_obj_vis.Location = new System.Drawing.Point(76, 520);
            this.chb_ref_obj_vis.Name = "chb_ref_obj_vis";
            this.chb_ref_obj_vis.PropName = "ref_obj_vis";
            this.chb_ref_obj_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_ref_obj_vis.TabIndex = 13;
            this.chb_ref_obj_vis.TabStop = false;
            this.chb_ref_obj_vis.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 199);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(17, 12);
            this.label3.TabIndex = 3;
            this.label3.Text = "孔";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_construct_line_vis
            // 
            this.chb_construct_line_vis.AutoSize = true;
            this.chb_construct_line_vis.Checked = true;
            this.chb_construct_line_vis.CheckName = null;
            this.chb_construct_line_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_construct_line_vis.Location = new System.Drawing.Point(76, 464);
            this.chb_construct_line_vis.Name = "chb_construct_line_vis";
            this.chb_construct_line_vis.PropName = "construct_line_vis";
            this.chb_construct_line_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_construct_line_vis.TabIndex = 13;
            this.chb_construct_line_vis.TabStop = false;
            this.chb_construct_line_vis.UseVisualStyleBackColor = true;
            // 
            // chb_rfbar_vis_comp
            // 
            this.chb_rfbar_vis_comp.AutoSize = true;
            this.chb_rfbar_vis_comp.Checked = true;
            this.chb_rfbar_vis_comp.CheckName = null;
            this.chb_rfbar_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rfbar_vis_comp.Location = new System.Drawing.Point(148, 272);
            this.chb_rfbar_vis_comp.Name = "chb_rfbar_vis_comp";
            this.chb_rfbar_vis_comp.PropName = "rfbar_vis_comp";
            this.chb_rfbar_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_rfbar_vis_comp.TabIndex = 13;
            this.chb_rfbar_vis_comp.TabStop = false;
            this.chb_rfbar_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_grid_vis
            // 
            this.chb_grid_vis.AutoSize = true;
            this.chb_grid_vis.Checked = true;
            this.chb_grid_vis.CheckName = null;
            this.chb_grid_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_grid_vis.Location = new System.Drawing.Point(76, 436);
            this.chb_grid_vis.Name = "chb_grid_vis";
            this.chb_grid_vis.PropName = "grid_vis";
            this.chb_grid_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_grid_vis.TabIndex = 13;
            this.chb_grid_vis.TabStop = false;
            this.chb_grid_vis.UseVisualStyleBackColor = true;
            // 
            // cbo_part_quality
            // 
            this.cbo_part_quality.CheckName = "chb_offsetH";
            this.cbo_part_quality.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_part_quality.FormattingEnabled = true;
            this.cbo_part_quality.Location = new System.Drawing.Point(206, 126);
            this.cbo_part_quality.Name = "cbo_part_quality";
            this.cbo_part_quality.PropName = "part_quality";
            this.cbo_part_quality.Size = new System.Drawing.Size(96, 21);
            this.cbo_part_quality.TabIndex = 14;
            // 
            // chb_comp_symbol_vis
            // 
            this.chb_comp_symbol_vis.AutoSize = true;
            this.chb_comp_symbol_vis.Checked = true;
            this.chb_comp_symbol_vis.CheckName = null;
            this.chb_comp_symbol_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_comp_symbol_vis.Location = new System.Drawing.Point(76, 400);
            this.chb_comp_symbol_vis.Name = "chb_comp_symbol_vis";
            this.chb_comp_symbol_vis.PropName = "comp_symbol_vis";
            this.chb_comp_symbol_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_comp_symbol_vis.TabIndex = 13;
            this.chb_comp_symbol_vis.TabStop = false;
            this.chb_comp_symbol_vis.UseVisualStyleBackColor = true;
            // 
            // chb_construct_pl_vis_comp
            // 
            this.chb_construct_pl_vis_comp.AutoSize = true;
            this.chb_construct_pl_vis_comp.Checked = true;
            this.chb_construct_pl_vis_comp.CheckName = null;
            this.chb_construct_pl_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_construct_pl_vis_comp.Location = new System.Drawing.Point(148, 492);
            this.chb_construct_pl_vis_comp.Name = "chb_construct_pl_vis_comp";
            this.chb_construct_pl_vis_comp.PropName = "construct_pl_vis_comp";
            this.chb_construct_pl_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_construct_pl_vis_comp.TabIndex = 13;
            this.chb_construct_pl_vis_comp.TabStop = false;
            this.chb_construct_pl_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_fitting_vis
            // 
            this.chb_fitting_vis.AutoSize = true;
            this.chb_fitting_vis.Checked = true;
            this.chb_fitting_vis.CheckName = null;
            this.chb_fitting_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_fitting_vis.Location = new System.Drawing.Point(76, 364);
            this.chb_fitting_vis.Name = "chb_fitting_vis";
            this.chb_fitting_vis.PropName = "fitting_vis";
            this.chb_fitting_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_fitting_vis.TabIndex = 13;
            this.chb_fitting_vis.TabStop = false;
            this.chb_fitting_vis.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 171);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "螺栓";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_cut_vis
            // 
            this.chb_cut_vis.AutoSize = true;
            this.chb_cut_vis.Checked = true;
            this.chb_cut_vis.CheckName = null;
            this.chb_cut_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_vis.Location = new System.Drawing.Point(76, 336);
            this.chb_cut_vis.Name = "chb_cut_vis";
            this.chb_cut_vis.PropName = "cut_vis";
            this.chb_cut_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_cut_vis.TabIndex = 13;
            this.chb_cut_vis.TabStop = false;
            this.chb_cut_vis.UseVisualStyleBackColor = true;
            // 
            // chb_weld_vis_comp
            // 
            this.chb_weld_vis_comp.AutoSize = true;
            this.chb_weld_vis_comp.Checked = true;
            this.chb_weld_vis_comp.CheckName = null;
            this.chb_weld_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_weld_vis_comp.Location = new System.Drawing.Point(148, 228);
            this.chb_weld_vis_comp.Name = "chb_weld_vis_comp";
            this.chb_weld_vis_comp.PropName = "weld_vis_comp";
            this.chb_weld_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_weld_vis_comp.TabIndex = 13;
            this.chb_weld_vis_comp.TabStop = false;
            this.chb_weld_vis_comp.UseVisualStyleBackColor = true;
            // 
            // lb_pt_vis
            // 
            this.lb_pt_vis.AutoSize = true;
            this.lb_pt_vis.Location = new System.Drawing.Point(11, 76);
            this.lb_pt_vis.Name = "lb_pt_vis";
            this.lb_pt_vis.Size = new System.Drawing.Size(41, 12);
            this.lb_pt_vis.TabIndex = 3;
            this.lb_pt_vis.Text = "辅助点";
            this.lb_pt_vis.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_surf_treat_vis
            // 
            this.chb_surf_treat_vis.AutoSize = true;
            this.chb_surf_treat_vis.Checked = true;
            this.chb_surf_treat_vis.CheckName = null;
            this.chb_surf_treat_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_surf_treat_vis.Location = new System.Drawing.Point(76, 300);
            this.chb_surf_treat_vis.Name = "chb_surf_treat_vis";
            this.chb_surf_treat_vis.PropName = "surf_treat_vis";
            this.chb_surf_treat_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_surf_treat_vis.TabIndex = 13;
            this.chb_surf_treat_vis.TabStop = false;
            this.chb_surf_treat_vis.UseVisualStyleBackColor = true;
            // 
            // chb_hole_vis_comp
            // 
            this.chb_hole_vis_comp.AutoSize = true;
            this.chb_hole_vis_comp.Checked = true;
            this.chb_hole_vis_comp.CheckName = null;
            this.chb_hole_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_hole_vis_comp.Location = new System.Drawing.Point(148, 200);
            this.chb_hole_vis_comp.Name = "chb_hole_vis_comp";
            this.chb_hole_vis_comp.PropName = "hole_vis_comp";
            this.chb_hole_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_hole_vis_comp.TabIndex = 13;
            this.chb_hole_vis_comp.TabStop = false;
            this.chb_hole_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_rfbar_vis
            // 
            this.chb_rfbar_vis.AutoSize = true;
            this.chb_rfbar_vis.Checked = true;
            this.chb_rfbar_vis.CheckName = null;
            this.chb_rfbar_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rfbar_vis.Location = new System.Drawing.Point(76, 272);
            this.chb_rfbar_vis.Name = "chb_rfbar_vis";
            this.chb_rfbar_vis.PropName = "rfbar_vis";
            this.chb_rfbar_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_rfbar_vis.TabIndex = 13;
            this.chb_rfbar_vis.TabStop = false;
            this.chb_rfbar_vis.UseVisualStyleBackColor = true;
            // 
            // lb_part_vis
            // 
            this.lb_part_vis.AutoSize = true;
            this.lb_part_vis.Location = new System.Drawing.Point(11, 128);
            this.lb_part_vis.Name = "lb_part_vis";
            this.lb_part_vis.Size = new System.Drawing.Size(29, 12);
            this.lb_part_vis.TabIndex = 3;
            this.lb_part_vis.Text = "零件";
            this.lb_part_vis.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_construct_pl_vis
            // 
            this.chb_construct_pl_vis.AutoSize = true;
            this.chb_construct_pl_vis.Checked = true;
            this.chb_construct_pl_vis.CheckName = null;
            this.chb_construct_pl_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_construct_pl_vis.Location = new System.Drawing.Point(76, 492);
            this.chb_construct_pl_vis.Name = "chb_construct_pl_vis";
            this.chb_construct_pl_vis.PropName = "construct_pl_vis";
            this.chb_construct_pl_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_construct_pl_vis.TabIndex = 13;
            this.chb_construct_pl_vis.TabStop = false;
            this.chb_construct_pl_vis.UseVisualStyleBackColor = true;
            // 
            // chb_bolt_vis_comp
            // 
            this.chb_bolt_vis_comp.AutoSize = true;
            this.chb_bolt_vis_comp.Checked = true;
            this.chb_bolt_vis_comp.CheckName = null;
            this.chb_bolt_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_bolt_vis_comp.Location = new System.Drawing.Point(148, 172);
            this.chb_bolt_vis_comp.Name = "chb_bolt_vis_comp";
            this.chb_bolt_vis_comp.PropName = "bolt_vis_comp";
            this.chb_bolt_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_bolt_vis_comp.TabIndex = 13;
            this.chb_bolt_vis_comp.TabStop = false;
            this.chb_bolt_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_weld_vis
            // 
            this.chb_weld_vis.AutoSize = true;
            this.chb_weld_vis.Checked = true;
            this.chb_weld_vis.CheckName = null;
            this.chb_weld_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_weld_vis.Location = new System.Drawing.Point(76, 228);
            this.chb_weld_vis.Name = "chb_weld_vis";
            this.chb_weld_vis.PropName = "weld_vis";
            this.chb_weld_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_weld_vis.TabIndex = 13;
            this.chb_weld_vis.TabStop = false;
            this.chb_weld_vis.UseVisualStyleBackColor = true;
            // 
            // lb_all_vis
            // 
            this.lb_all_vis.AutoSize = true;
            this.lb_all_vis.Location = new System.Drawing.Point(11, 39);
            this.lb_all_vis.Name = "lb_all_vis";
            this.lb_all_vis.Size = new System.Drawing.Size(29, 12);
            this.lb_all_vis.TabIndex = 3;
            this.lb_all_vis.Text = "全部";
            this.lb_all_vis.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_hole_vis
            // 
            this.chb_hole_vis.AutoSize = true;
            this.chb_hole_vis.Checked = true;
            this.chb_hole_vis.CheckName = null;
            this.chb_hole_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_hole_vis.Location = new System.Drawing.Point(76, 200);
            this.chb_hole_vis.Name = "chb_hole_vis";
            this.chb_hole_vis.PropName = "hole_vis";
            this.chb_hole_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_hole_vis.TabIndex = 13;
            this.chb_hole_vis.TabStop = false;
            this.chb_hole_vis.UseVisualStyleBackColor = true;
            // 
            // chb_part_vis_comp
            // 
            this.chb_part_vis_comp.AutoSize = true;
            this.chb_part_vis_comp.Checked = true;
            this.chb_part_vis_comp.CheckName = null;
            this.chb_part_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_vis_comp.Location = new System.Drawing.Point(148, 129);
            this.chb_part_vis_comp.Name = "chb_part_vis_comp";
            this.chb_part_vis_comp.PropName = "part_vis_comp";
            this.chb_part_vis_comp.Size = new System.Drawing.Size(15, 14);
            this.chb_part_vis_comp.TabIndex = 13;
            this.chb_part_vis_comp.TabStop = false;
            this.chb_part_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_bolt_vis
            // 
            this.chb_bolt_vis.AutoSize = true;
            this.chb_bolt_vis.Checked = true;
            this.chb_bolt_vis.CheckName = null;
            this.chb_bolt_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_bolt_vis.Location = new System.Drawing.Point(76, 172);
            this.chb_bolt_vis.Name = "chb_bolt_vis";
            this.chb_bolt_vis.PropName = "bolt_vis";
            this.chb_bolt_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_bolt_vis.TabIndex = 13;
            this.chb_bolt_vis.TabStop = false;
            this.chb_bolt_vis.UseVisualStyleBackColor = true;
            // 
            // chb_vis_comp_all
            // 
            this.chb_vis_comp_all.AutoSize = true;
            this.chb_vis_comp_all.Checked = true;
            this.chb_vis_comp_all.CheckName = null;
            this.chb_vis_comp_all.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_vis_comp_all.Location = new System.Drawing.Point(148, 40);
            this.chb_vis_comp_all.Name = "chb_vis_comp_all";
            this.chb_vis_comp_all.PropName = null;
            this.chb_vis_comp_all.Size = new System.Drawing.Size(15, 14);
            this.chb_vis_comp_all.TabIndex = 13;
            this.chb_vis_comp_all.TabStop = false;
            this.chb_vis_comp_all.UseVisualStyleBackColor = true;
            // 
            // chb_part_vis
            // 
            this.chb_part_vis.AutoSize = true;
            this.chb_part_vis.Checked = true;
            this.chb_part_vis.CheckName = null;
            this.chb_part_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_vis.Location = new System.Drawing.Point(76, 129);
            this.chb_part_vis.Name = "chb_part_vis";
            this.chb_part_vis.PropName = "part_vis";
            this.chb_part_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_part_vis.TabIndex = 13;
            this.chb_part_vis.TabStop = false;
            this.chb_part_vis.UseVisualStyleBackColor = true;
            // 
            // chb_pt_vis
            // 
            this.chb_pt_vis.AutoSize = true;
            this.chb_pt_vis.Checked = true;
            this.chb_pt_vis.CheckName = null;
            this.chb_pt_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_pt_vis.Location = new System.Drawing.Point(76, 76);
            this.chb_pt_vis.Name = "chb_pt_vis";
            this.chb_pt_vis.PropName = "pt_vis";
            this.chb_pt_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_pt_vis.TabIndex = 13;
            this.chb_pt_vis.TabStop = false;
            this.chb_pt_vis.UseVisualStyleBackColor = true;
            // 
            // chb_vis_all
            // 
            this.chb_vis_all.AutoSize = true;
            this.chb_vis_all.Checked = true;
            this.chb_vis_all.CheckName = null;
            this.chb_vis_all.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_vis_all.Location = new System.Drawing.Point(76, 40);
            this.chb_vis_all.Name = "chb_vis_all";
            this.chb_vis_all.PropName = null;
            this.chb_vis_all.Size = new System.Drawing.Size(15, 14);
            this.chb_vis_all.TabIndex = 13;
            this.chb_vis_all.TabStop = false;
            this.chb_vis_all.UseVisualStyleBackColor = true;
            // 
            // lb_vis_model
            // 
            this.lb_vis_model.AutoSize = true;
            this.lb_vis_model.Location = new System.Drawing.Point(57, 15);
            this.lb_vis_model.Name = "lb_vis_model";
            this.lb_vis_model.Size = new System.Drawing.Size(53, 12);
            this.lb_vis_model.TabIndex = 0;
            this.lb_vis_model.Text = "模型可见";
            this.lb_vis_model.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lb_vis_comp
            // 
            this.lb_vis_comp.AutoSize = true;
            this.lb_vis_comp.Location = new System.Drawing.Point(128, 15);
            this.lb_vis_comp.Name = "lb_vis_comp";
            this.lb_vis_comp.Size = new System.Drawing.Size(53, 12);
            this.lb_vis_comp.TabIndex = 2;
            this.lb_vis_comp.Text = "节点可见";
            this.lb_vis_comp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // tp_advance
            // 
            this.tp_advance.Controls.Add(this.gb_connect);
            this.tp_advance.Controls.Add(this.gb_point);
            this.tp_advance.Controls.Add(this.gb_part);
            this.tp_advance.Location = new System.Drawing.Point(4, 22);
            this.tp_advance.Name = "tp_advance";
            this.tp_advance.Padding = new System.Windows.Forms.Padding(3);
            this.tp_advance.Size = new System.Drawing.Size(480, 548);
            this.tp_advance.TabIndex = 1;
            this.tp_advance.Text = "高级";
            this.tp_advance.UseVisualStyleBackColor = true;
            // 
            // gb_connect
            // 
            this.gb_connect.Controls.Add(this.chb_connection_text_vis);
            this.gb_connect.Location = new System.Drawing.Point(7, 346);
            this.gb_connect.Name = "gb_connect";
            this.gb_connect.Size = new System.Drawing.Size(121, 90);
            this.gb_connect.TabIndex = 17;
            this.gb_connect.TabStop = false;
            this.gb_connect.Text = "节点";
            // 
            // chb_connection_text_vis
            // 
            this.chb_connection_text_vis.AutoSize = true;
            this.chb_connection_text_vis.Checked = true;
            this.chb_connection_text_vis.CheckName = null;
            this.chb_connection_text_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_connection_text_vis.Location = new System.Drawing.Point(13, 27);
            this.chb_connection_text_vis.Name = "chb_connection_text_vis";
            this.chb_connection_text_vis.PropName = "connection_text_vis";
            this.chb_connection_text_vis.Size = new System.Drawing.Size(72, 16);
            this.chb_connection_text_vis.TabIndex = 15;
            this.chb_connection_text_vis.TabStop = false;
            this.chb_connection_text_vis.Text = "节点文本";
            this.chb_connection_text_vis.UseVisualStyleBackColor = true;
            // 
            // gb_point
            // 
            this.gb_point.Controls.Add(this.rb_pt_size_mode);
            this.gb_point.Controls.Add(this.tb_pt_size);
            this.gb_point.Controls.Add(this.label17);
            this.gb_point.Location = new System.Drawing.Point(134, 346);
            this.gb_point.Name = "gb_point";
            this.gb_point.Size = new System.Drawing.Size(187, 90);
            this.gb_point.TabIndex = 16;
            this.gb_point.TabStop = false;
            this.gb_point.Text = "辅助点";
            // 
            // rb_pt_size_mode
            // 
            this.rb_pt_size_mode.BackColor = System.Drawing.SystemColors.Control;
            this.rb_pt_size_mode.btnCount = 2;
            this.rb_pt_size_mode.btnSize = new System.Drawing.Size(75, 21);
            this.rb_pt_size_mode.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_pt_size_mode.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_pt_size_mode.keys = new object[] {
        ((object)(0))};
            this.rb_pt_size_mode.keys1 = new object[] {
        ((object)(0))};
            this.rb_pt_size_mode.labels = new string[] {
        ""};
            this.rb_pt_size_mode.Location = new System.Drawing.Point(17, 52);
            this.rb_pt_size_mode.Margin = new System.Windows.Forms.Padding(0);
            this.rb_pt_size_mode.Name = "rb_pt_size_mode";
            this.rb_pt_size_mode.PropName = "pt_size_mode";
            this.rb_pt_size_mode.Size = new System.Drawing.Size(153, 21);
            this.rb_pt_size_mode.TabIndex = 87;
            // 
            // tb_pt_size
            // 
            this.tb_pt_size.CheckName = "chb_offsetH";
            this.tb_pt_size.DefValue = 0D;
            this.tb_pt_size.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_pt_size.Location = new System.Drawing.Point(47, 25);
            this.tb_pt_size.Min = 0D;
            this.tb_pt_size.Name = "tb_pt_size";
            this.tb_pt_size.PropName = "pt_size";
            this.tb_pt_size.Size = new System.Drawing.Size(89, 21);
            this.tb_pt_size.TabIndex = 21;
            this.tb_pt_size.Text = "0";
            this.tb_pt_size.Value = 0D;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(11, 28);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(29, 12);
            this.label17.TabIndex = 14;
            this.label17.Text = "尺寸";
            this.label17.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // gb_part
            // 
            this.gb_part.Controls.Add(this.gb_part_vis);
            this.gb_part.Controls.Add(this.btn_part_label_remove);
            this.gb_part.Controls.Add(this.sepLine7);
            this.gb_part.Controls.Add(this.label8);
            this.gb_part.Controls.Add(this.label18);
            this.gb_part.Controls.Add(this.chb_part_ref_line_vis_comp);
            this.gb_part.Controls.Add(this.chb_part_ref_line_vis);
            this.gb_part.Controls.Add(this.label15);
            this.gb_part.Controls.Add(this.chb_part_center_line_vis_comp);
            this.gb_part.Controls.Add(this.li_part_label_active_props);
            this.gb_part.Controls.Add(this.chb_part_center_line_vis);
            this.gb_part.Controls.Add(this.li_part_label_props);
            this.gb_part.Controls.Add(this.btn_part_lable_add);
            this.gb_part.Location = new System.Drawing.Point(7, 7);
            this.gb_part.Name = "gb_part";
            this.gb_part.Size = new System.Drawing.Size(465, 333);
            this.gb_part.TabIndex = 0;
            this.gb_part.TabStop = false;
            this.gb_part.Text = "零件";
            // 
            // gb_part_vis
            // 
            this.gb_part_vis.Controls.Add(this.chb_part_label_vis);
            this.gb_part_vis.Controls.Add(this.chb_part_label_vis_comp);
            this.gb_part_vis.Controls.Add(this.chb_part_label_vis_ass_main);
            this.gb_part_vis.Location = new System.Drawing.Point(10, 112);
            this.gb_part_vis.Name = "gb_part_vis";
            this.gb_part_vis.Size = new System.Drawing.Size(111, 106);
            this.gb_part_vis.TabIndex = 25;
            this.gb_part_vis.TabStop = false;
            this.gb_part_vis.Text = "显示标签";
            // 
            // chb_part_label_vis
            // 
            this.chb_part_label_vis.AutoSize = true;
            this.chb_part_label_vis.Checked = true;
            this.chb_part_label_vis.CheckName = null;
            this.chb_part_label_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_label_vis.Location = new System.Drawing.Point(13, 30);
            this.chb_part_label_vis.Name = "chb_part_label_vis";
            this.chb_part_label_vis.PropName = "part_label_vis";
            this.chb_part_label_vis.Size = new System.Drawing.Size(60, 16);
            this.chb_part_label_vis.TabIndex = 15;
            this.chb_part_label_vis.TabStop = false;
            this.chb_part_label_vis.Text = "模型中";
            this.chb_part_label_vis.UseVisualStyleBackColor = true;
            // 
            // chb_part_label_vis_comp
            // 
            this.chb_part_label_vis_comp.AutoSize = true;
            this.chb_part_label_vis_comp.Checked = true;
            this.chb_part_label_vis_comp.CheckName = null;
            this.chb_part_label_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_label_vis_comp.Location = new System.Drawing.Point(13, 52);
            this.chb_part_label_vis_comp.Name = "chb_part_label_vis_comp";
            this.chb_part_label_vis_comp.PropName = "part_label_vis_comp";
            this.chb_part_label_vis_comp.Size = new System.Drawing.Size(60, 16);
            this.chb_part_label_vis_comp.TabIndex = 15;
            this.chb_part_label_vis_comp.TabStop = false;
            this.chb_part_label_vis_comp.Text = "节点中";
            this.chb_part_label_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_part_label_vis_ass_main
            // 
            this.chb_part_label_vis_ass_main.AutoSize = true;
            this.chb_part_label_vis_ass_main.Checked = true;
            this.chb_part_label_vis_ass_main.CheckName = null;
            this.chb_part_label_vis_ass_main.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_label_vis_ass_main.Location = new System.Drawing.Point(13, 74);
            this.chb_part_label_vis_ass_main.Name = "chb_part_label_vis_ass_main";
            this.chb_part_label_vis_ass_main.PropName = "part_label_vis_ass_main";
            this.chb_part_label_vis_ass_main.Size = new System.Drawing.Size(60, 16);
            this.chb_part_label_vis_ass_main.TabIndex = 15;
            this.chb_part_label_vis_ass_main.TabStop = false;
            this.chb_part_label_vis_ass_main.Text = "主零件";
            this.chb_part_label_vis_ass_main.UseVisualStyleBackColor = true;
            // 
            // btn_part_label_remove
            // 
            this.btn_part_label_remove.Location = new System.Drawing.Point(270, 213);
            this.btn_part_label_remove.Name = "btn_part_label_remove";
            this.btn_part_label_remove.Size = new System.Drawing.Size(55, 28);
            this.btn_part_label_remove.TabIndex = 19;
            this.btn_part_label_remove.Text = "<<";
            this.btn_part_label_remove.UseVisualStyleBackColor = true;
            // 
            // sepLine7
            // 
            this.sepLine7.BackColor = System.Drawing.Color.Transparent;
            this.sepLine7.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.sepLine7.Location = new System.Drawing.Point(71, 89);
            this.sepLine7.Margin = new System.Windows.Forms.Padding(4);
            this.sepLine7.Name = "sepLine7";
            this.sepLine7.Size = new System.Drawing.Size(380, 10);
            this.sepLine7.TabIndex = 24;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(8, 88);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(53, 12);
            this.label8.TabIndex = 14;
            this.label8.Text = "零件标签";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(337, 103);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(53, 12);
            this.label18.TabIndex = 14;
            this.label18.Text = "零件标签";
            this.label18.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_part_ref_line_vis_comp
            // 
            this.chb_part_ref_line_vis_comp.AutoSize = true;
            this.chb_part_ref_line_vis_comp.Checked = true;
            this.chb_part_ref_line_vis_comp.CheckName = null;
            this.chb_part_ref_line_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_ref_line_vis_comp.Location = new System.Drawing.Point(146, 54);
            this.chb_part_ref_line_vis_comp.Name = "chb_part_ref_line_vis_comp";
            this.chb_part_ref_line_vis_comp.PropName = "part_ref_line_vis_comp";
            this.chb_part_ref_line_vis_comp.Size = new System.Drawing.Size(108, 16);
            this.chb_part_ref_line_vis_comp.TabIndex = 15;
            this.chb_part_ref_line_vis_comp.TabStop = false;
            this.chb_part_ref_line_vis_comp.Text = "参考线(节点中)";
            this.chb_part_ref_line_vis_comp.UseVisualStyleBackColor = true;
            // 
            // chb_part_ref_line_vis
            // 
            this.chb_part_ref_line_vis.AutoSize = true;
            this.chb_part_ref_line_vis.Checked = true;
            this.chb_part_ref_line_vis.CheckName = null;
            this.chb_part_ref_line_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_ref_line_vis.Location = new System.Drawing.Point(146, 32);
            this.chb_part_ref_line_vis.Name = "chb_part_ref_line_vis";
            this.chb_part_ref_line_vis.PropName = "part_ref_line_vis";
            this.chb_part_ref_line_vis.Size = new System.Drawing.Size(60, 16);
            this.chb_part_ref_line_vis.TabIndex = 15;
            this.chb_part_ref_line_vis.TabStop = false;
            this.chb_part_ref_line_vis.Text = "参考线";
            this.chb_part_ref_line_vis.UseVisualStyleBackColor = true;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(149, 103);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(29, 12);
            this.label15.TabIndex = 14;
            this.label15.Text = "属性";
            this.label15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // chb_part_center_line_vis_comp
            // 
            this.chb_part_center_line_vis_comp.AutoSize = true;
            this.chb_part_center_line_vis_comp.Checked = true;
            this.chb_part_center_line_vis_comp.CheckName = null;
            this.chb_part_center_line_vis_comp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_center_line_vis_comp.Location = new System.Drawing.Point(13, 54);
            this.chb_part_center_line_vis_comp.Name = "chb_part_center_line_vis_comp";
            this.chb_part_center_line_vis_comp.PropName = "part_center_line_vis_comp";
            this.chb_part_center_line_vis_comp.Size = new System.Drawing.Size(108, 16);
            this.chb_part_center_line_vis_comp.TabIndex = 15;
            this.chb_part_center_line_vis_comp.TabStop = false;
            this.chb_part_center_line_vis_comp.Text = "中心线(节点中)";
            this.chb_part_center_line_vis_comp.UseVisualStyleBackColor = true;
            // 
            // li_part_label_active_props
            // 
            this.li_part_label_active_props.FormattingEnabled = true;
            this.li_part_label_active_props.ItemHeight = 12;
            this.li_part_label_active_props.Location = new System.Drawing.Point(331, 120);
            this.li_part_label_active_props.Name = "li_part_label_active_props";
            this.li_part_label_active_props.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.li_part_label_active_props.Size = new System.Drawing.Size(120, 196);
            this.li_part_label_active_props.TabIndex = 17;
            // 
            // chb_part_center_line_vis
            // 
            this.chb_part_center_line_vis.AutoSize = true;
            this.chb_part_center_line_vis.Checked = true;
            this.chb_part_center_line_vis.CheckName = null;
            this.chb_part_center_line_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_center_line_vis.Location = new System.Drawing.Point(13, 32);
            this.chb_part_center_line_vis.Name = "chb_part_center_line_vis";
            this.chb_part_center_line_vis.PropName = "part_center_line_vis";
            this.chb_part_center_line_vis.Size = new System.Drawing.Size(60, 16);
            this.chb_part_center_line_vis.TabIndex = 15;
            this.chb_part_center_line_vis.TabStop = false;
            this.chb_part_center_line_vis.Text = "中心线";
            this.chb_part_center_line_vis.UseVisualStyleBackColor = true;
            // 
            // li_part_label_props
            // 
            this.li_part_label_props.FormattingEnabled = true;
            this.li_part_label_props.ItemHeight = 12;
            this.li_part_label_props.Location = new System.Drawing.Point(146, 120);
            this.li_part_label_props.Name = "li_part_label_props";
            this.li_part_label_props.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.li_part_label_props.Size = new System.Drawing.Size(120, 196);
            this.li_part_label_props.TabIndex = 16;
            // 
            // btn_part_lable_add
            // 
            this.btn_part_lable_add.Location = new System.Drawing.Point(270, 179);
            this.btn_part_lable_add.Name = "btn_part_lable_add";
            this.btn_part_lable_add.Size = new System.Drawing.Size(55, 28);
            this.btn_part_lable_add.TabIndex = 18;
            this.btn_part_lable_add.Text = ">>";
            this.btn_part_lable_add.UseVisualStyleBackColor = true;
            // 
            // Form_ViewDisplaySetting_Context
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(488, 574);
            this.ControlBox = false;
            this.Controls.Add(this.tc_root);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_ViewDisplaySetting_Context";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.tc_root.ResumeLayout(false);
            this.tp_props.ResumeLayout(false);
            this.tp_props.PerformLayout();
            this.tp_advance.ResumeLayout(false);
            this.gb_connect.ResumeLayout(false);
            this.gb_connect.PerformLayout();
            this.gb_point.ResumeLayout(false);
            this.gb_point.PerformLayout();
            this.gb_part.ResumeLayout(false);
            this.gb_part.PerformLayout();
            this.gb_part_vis.ResumeLayout(false);
            this.gb_part_vis.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tc_root;
        private System.Windows.Forms.TabPage tp_props;
        private UserCtrl.RadioButtons rb_rfbar_quality;
        private UserCtrl.RadioButtons rb_weld_quality;
        private UserCtrl.RadioButtons rb_hole_quality;
        private UserCtrl.RadioButtons rb_bolt_quality;
        private UserCtrl.SepLine sepLine6;
        private UserCtrl.SepLine sepLine5;
        private UserCtrl.SepLine sepLine4;
        private UserCtrl.SepLine sepLine3;
        private UserCtrl.SepLine sepLine2;
        private UserCtrl.SepLine sepLine1;
        private UserCtrl.SepLine sep;
        private UserCtrl.ComboBoxKey cbo_part_mode_comp;
        private System.Windows.Forms.Label label1;
        private UserCtrl.ComboBoxKey cbo_part_mode;
        private System.Windows.Forms.Label lb_mode;
        private System.Windows.Forms.Label lb_quality;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private UserCtrl.Check chb_comp_symbol_vis_comp;
        private System.Windows.Forms.Label label5;
        private UserCtrl.Check chb_fitting_vis_comp;
        private UserCtrl.Check chb_cut_vis_comp;
        private System.Windows.Forms.Label label4;
        private UserCtrl.Check chb_ref_obj_vis;
        private System.Windows.Forms.Label label3;
        private UserCtrl.Check chb_construct_line_vis;
        private UserCtrl.Check chb_rfbar_vis_comp;
        private UserCtrl.Check chb_grid_vis;
        private UserCtrl.ComboBoxKey cbo_part_quality;
        private UserCtrl.Check chb_comp_symbol_vis;
        private UserCtrl.Check chb_construct_pl_vis_comp;
        private UserCtrl.Check chb_fitting_vis;
        private System.Windows.Forms.Label label2;
        private UserCtrl.Check chb_cut_vis;
        private UserCtrl.Check chb_weld_vis_comp;
        private System.Windows.Forms.Label lb_pt_vis;
        private UserCtrl.Check chb_surf_treat_vis;
        private UserCtrl.Check chb_hole_vis_comp;
        private UserCtrl.Check chb_rfbar_vis;
        private System.Windows.Forms.Label lb_part_vis;
        private UserCtrl.Check chb_construct_pl_vis;
        private UserCtrl.Check chb_bolt_vis_comp;
        private UserCtrl.Check chb_weld_vis;
        private System.Windows.Forms.Label lb_all_vis;
        private UserCtrl.Check chb_hole_vis;
        private UserCtrl.Check chb_part_vis_comp;
        private UserCtrl.Check chb_bolt_vis;
        private UserCtrl.Check chb_vis_comp_all;
        private UserCtrl.Check chb_part_vis;
        private UserCtrl.Check chb_pt_vis;
        private UserCtrl.Check chb_vis_all;
        private System.Windows.Forms.Label lb_vis_model;
        private System.Windows.Forms.Label lb_vis_comp;
        private System.Windows.Forms.TabPage tp_advance;
        private System.Windows.Forms.GroupBox gb_connect;
        private UserCtrl.Check chb_connection_text_vis;
        private System.Windows.Forms.GroupBox gb_point;
        private UserCtrl.RadioButtons rb_pt_size_mode;
        private UserCtrl.InputDouble tb_pt_size;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox gb_part;
        private System.Windows.Forms.GroupBox gb_part_vis;
        private UserCtrl.Check chb_part_label_vis;
        private UserCtrl.Check chb_part_label_vis_comp;
        private UserCtrl.Check chb_part_label_vis_ass_main;
        private System.Windows.Forms.Button btn_part_label_remove;
        private UserCtrl.SepLine sepLine7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label18;
        private UserCtrl.Check chb_part_ref_line_vis_comp;
        private UserCtrl.Check chb_part_ref_line_vis;
        private System.Windows.Forms.Label label15;
        private UserCtrl.Check chb_part_center_line_vis_comp;
        private System.Windows.Forms.ListBox li_part_label_active_props;
        private UserCtrl.Check chb_part_center_line_vis;
        private System.Windows.Forms.ListBox li_part_label_props;
        private System.Windows.Forms.Button btn_part_lable_add;
    }
}