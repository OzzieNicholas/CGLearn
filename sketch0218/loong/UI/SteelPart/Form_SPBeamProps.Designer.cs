namespace UI
{
    partial class Form_SPBeamProps
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tc_props = new System.Windows.Forms.TabControl();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.gb_drag = new System.Windows.Forms.GroupBox();
            this.tb_dy0 = new UserCtrl.InputDouble();
            this.tb_dz0 = new UserCtrl.InputDouble();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tb_dx0 = new UserCtrl.InputDouble();
            this.tb_dz1 = new UserCtrl.InputDouble();
            this.tb_dy1 = new UserCtrl.InputDouble();
            this.chb_dz1 = new UserCtrl.Check();
            this.chb_dy1 = new UserCtrl.Check();
            this.chb_dz0 = new UserCtrl.Check();
            this.tb_dx1 = new UserCtrl.InputDouble();
            this.chb_dx0 = new UserCtrl.Check();
            this.chb_dy0 = new UserCtrl.Check();
            this.chb_dx1 = new UserCtrl.Check();
            this.gb_pos = new System.Windows.Forms.GroupBox();
            this.rb_offset_h = new UserCtrl.RadioButtons();
            this.rb_rot = new UserCtrl.RadioButtons();
            this.rb_offset_v = new UserCtrl.RadioButtons();
            this.chb_offset_h = new UserCtrl.Check();
            this.chb_rot = new UserCtrl.Check();
            this.tb_rot_angl = new UserCtrl.InputDouble();
            this.chb_offset_v = new UserCtrl.Check();
            this.tb_offset_h = new UserCtrl.InputDouble();
            this.tb_offset_v = new UserCtrl.InputDouble();
            this.gb_props = new System.Windows.Forms.GroupBox();
            this.cbo_class = new UserCtrl.Combo();
            this.chb_class = new UserCtrl.Check();
            this.tb_finish = new UserCtrl.InputText();
            this.chb_polish = new UserCtrl.Check();
            this.btn_mtrl = new System.Windows.Forms.Button();
            this.tb_mtrl = new UserCtrl.InputText();
            this.chb_mtrl = new UserCtrl.Check();
            this.btn_profile = new System.Windows.Forms.Button();
            this.tb_profile = new UserCtrl.InputText();
            this.tb_name = new UserCtrl.InputText();
            this.chb_profile = new UserCtrl.Check();
            this.chb_name = new UserCtrl.Check();
            this.gb_no = new System.Windows.Forms.GroupBox();
            this.tb_assembly_no_start = new UserCtrl.InputText();
            this.tb_no_start = new UserCtrl.InputText();
            this.tb_assembly_no_prefix = new UserCtrl.InputText();
            this.tb_no_prefix = new UserCtrl.InputText();
            this.chb_assembly_noStart = new UserCtrl.Check();
            this.chb_part_noStart = new UserCtrl.Check();
            this.chb_assembly_no = new UserCtrl.Check();
            this.chb_part_no = new UserCtrl.Check();
            this.tp_2 = new System.Windows.Forms.TabPage();
            this.gb_deform = new System.Windows.Forms.GroupBox();
            this.tb_deform_exc_y = new UserCtrl.InputDouble();
            this.tb_deform_end = new UserCtrl.InputDouble();
            this.chb_deform_exc_y = new UserCtrl.Check();
            this.chb_deform_end = new UserCtrl.Check();
            this.tb_deform_exc_x = new UserCtrl.InputDouble();
            this.chb_deform_exc_x = new UserCtrl.Check();
            this.tb_deform_start = new UserCtrl.InputDouble();
            this.chb_deform_start = new UserCtrl.Check();
            this.gb_curve = new System.Windows.Forms.GroupBox();
            this.tb_curve_segs = new UserCtrl.InputInt();
            this.lb_curve_segs = new System.Windows.Forms.Label();
            this.tb_curve_r = new UserCtrl.InputDouble();
            this.rb_curve_plane = new UserCtrl.RadioButtons();
            this.chb_curve = new UserCtrl.Check();
            this.tp_3 = new System.Windows.Forms.TabPage();
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.radio1 = new UserCtrl.Radio();
            this.radio2 = new UserCtrl.Radio();
            this.radio3 = new UserCtrl.Radio();
            this.radio4 = new UserCtrl.Radio();
            this.radio5 = new UserCtrl.Radio();
            this.radio6 = new UserCtrl.Radio();
            this.radio7 = new UserCtrl.Radio();
            this.radio8 = new UserCtrl.Radio();
            this.radio9 = new UserCtrl.Radio();
            this.radio10 = new UserCtrl.Radio();
            this.inputDouble1 = new UserCtrl.InputDouble();
            this.inputDouble2 = new UserCtrl.InputDouble();
            this.check1 = new UserCtrl.Check();
            this.check2 = new UserCtrl.Check();
            this.inputDouble3 = new UserCtrl.InputDouble();
            this.check3 = new UserCtrl.Check();
            this.bar_cfg = new UI.CommonCfgBar();
            this.tc_props.SuspendLayout();
            this.tp_1.SuspendLayout();
            this.gb_drag.SuspendLayout();
            this.gb_pos.SuspendLayout();
            this.gb_props.SuspendLayout();
            this.gb_no.SuspendLayout();
            this.tp_2.SuspendLayout();
            this.gb_deform.SuspendLayout();
            this.gb_curve.SuspendLayout();
            this.SuspendLayout();
            // 
            // tc_props
            // 
            this.tc_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_props.Controls.Add(this.tp_1);
            this.tc_props.Controls.Add(this.tp_2);
            this.tc_props.Controls.Add(this.tp_3);
            this.tc_props.Location = new System.Drawing.Point(6, 42);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(395, 503);
            this.tc_props.TabIndex = 1;
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.gb_drag);
            this.tp_1.Controls.Add(this.gb_pos);
            this.tp_1.Controls.Add(this.gb_props);
            this.tp_1.Controls.Add(this.gb_no);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(387, 477);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "属性";
            this.tp_1.UseVisualStyleBackColor = true;
            // 
            // gb_drag
            // 
            this.gb_drag.Controls.Add(this.tb_dy0);
            this.gb_drag.Controls.Add(this.tb_dz0);
            this.gb_drag.Controls.Add(this.label3);
            this.gb_drag.Controls.Add(this.label2);
            this.gb_drag.Controls.Add(this.label1);
            this.gb_drag.Controls.Add(this.tb_dx0);
            this.gb_drag.Controls.Add(this.tb_dz1);
            this.gb_drag.Controls.Add(this.tb_dy1);
            this.gb_drag.Controls.Add(this.chb_dz1);
            this.gb_drag.Controls.Add(this.chb_dy1);
            this.gb_drag.Controls.Add(this.chb_dz0);
            this.gb_drag.Controls.Add(this.tb_dx1);
            this.gb_drag.Controls.Add(this.chb_dx0);
            this.gb_drag.Controls.Add(this.chb_dy0);
            this.gb_drag.Controls.Add(this.chb_dx1);
            this.gb_drag.Location = new System.Drawing.Point(6, 370);
            this.gb_drag.Name = "gb_drag";
            this.gb_drag.Size = new System.Drawing.Size(372, 102);
            this.gb_drag.TabIndex = 84;
            this.gb_drag.TabStop = false;
            this.gb_drag.Text = "偏移";
            // 
            // tb_dy0
            // 
            this.tb_dy0.CheckName = "chb_dy0";
            this.tb_dy0.DefValue = 0D;
            this.tb_dy0.Enob = 5;
            this.tb_dy0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dy0.Location = new System.Drawing.Point(111, 74);
            this.tb_dy0.Name = "tb_dy0";
            this.tb_dy0.PropName = "work_transform.dy0";
            this.tb_dy0.Size = new System.Drawing.Size(100, 21);
            this.tb_dy0.TabIndex = 110;
            this.tb_dy0.Text = "0";
            this.tb_dy0.unitEnobType = "model_dist";
            this.tb_dy0.Value = 0D;
            // 
            // tb_dz0
            // 
            this.tb_dz0.CheckName = "chb_dz0";
            this.tb_dz0.DefValue = 0D;
            this.tb_dz0.Enob = 5;
            this.tb_dz0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dz0.Location = new System.Drawing.Point(111, 20);
            this.tb_dz0.Name = "tb_dz0";
            this.tb_dz0.PropName = "work_transform.dz0";
            this.tb_dz0.Size = new System.Drawing.Size(100, 21);
            this.tb_dz0.TabIndex = 70;
            this.tb_dz0.Text = "0";
            this.tb_dz0.unitEnobType = "model_dist";
            this.tb_dz0.Value = 0D;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(14, 77);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 15;
            this.label3.Text = "纵向";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(14, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 15;
            this.label2.Text = "横向";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 15;
            this.label1.Text = "延伸";
            // 
            // tb_dx0
            // 
            this.tb_dx0.CheckName = "chb_dx0";
            this.tb_dx0.DefValue = 0D;
            this.tb_dx0.Enob = 5;
            this.tb_dx0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dx0.Location = new System.Drawing.Point(111, 47);
            this.tb_dx0.Name = "tb_dx0";
            this.tb_dx0.PropName = "work_transform.dx0";
            this.tb_dx0.Size = new System.Drawing.Size(100, 21);
            this.tb_dx0.TabIndex = 90;
            this.tb_dx0.Text = "0";
            this.tb_dx0.unitEnobType = "model_dist";
            this.tb_dx0.Value = 0D;
            // 
            // tb_dz1
            // 
            this.tb_dz1.CheckName = "chb_dz1";
            this.tb_dz1.DefValue = 0D;
            this.tb_dz1.Enob = 5;
            this.tb_dz1.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dz1.Location = new System.Drawing.Point(259, 20);
            this.tb_dz1.Name = "tb_dz1";
            this.tb_dz1.PropName = "-work_transform.dz1";
            this.tb_dz1.Size = new System.Drawing.Size(100, 21);
            this.tb_dz1.TabIndex = 80;
            this.tb_dz1.Text = "0";
            this.tb_dz1.unitEnobType = "model_dist";
            this.tb_dz1.Value = 0D;
            // 
            // tb_dy1
            // 
            this.tb_dy1.CheckName = "chb_dy1";
            this.tb_dy1.DefValue = 0D;
            this.tb_dy1.Enob = 5;
            this.tb_dy1.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dy1.Location = new System.Drawing.Point(259, 74);
            this.tb_dy1.Name = "tb_dy1";
            this.tb_dy1.PropName = "work_transform.dy1";
            this.tb_dy1.Size = new System.Drawing.Size(100, 21);
            this.tb_dy1.TabIndex = 120;
            this.tb_dy1.Text = "0";
            this.tb_dy1.unitEnobType = "model_dist";
            this.tb_dy1.Value = 0D;
            // 
            // chb_dz1
            // 
            this.chb_dz1.AutoSize = true;
            this.chb_dz1.Checked = true;
            this.chb_dz1.CheckName = null;
            this.chb_dz1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dz1.Location = new System.Drawing.Point(218, 22);
            this.chb_dz1.Name = "chb_dz1";
            this.chb_dz1.PropName = null;
            this.chb_dz1.Size = new System.Drawing.Size(36, 16);
            this.chb_dz1.TabIndex = 14;
            this.chb_dz1.TabStop = false;
            this.chb_dz1.Text = "尾";
            this.chb_dz1.UseVisualStyleBackColor = true;
            // 
            // chb_dy1
            // 
            this.chb_dy1.AutoSize = true;
            this.chb_dy1.Checked = true;
            this.chb_dy1.CheckName = null;
            this.chb_dy1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dy1.Location = new System.Drawing.Point(218, 76);
            this.chb_dy1.Name = "chb_dy1";
            this.chb_dy1.PropName = null;
            this.chb_dy1.Size = new System.Drawing.Size(36, 16);
            this.chb_dy1.TabIndex = 12;
            this.chb_dy1.TabStop = false;
            this.chb_dy1.Text = "尾";
            this.chb_dy1.UseVisualStyleBackColor = true;
            // 
            // chb_dz0
            // 
            this.chb_dz0.AutoSize = true;
            this.chb_dz0.Checked = true;
            this.chb_dz0.CheckName = null;
            this.chb_dz0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dz0.Location = new System.Drawing.Point(69, 22);
            this.chb_dz0.Name = "chb_dz0";
            this.chb_dz0.PropName = null;
            this.chb_dz0.Size = new System.Drawing.Size(36, 16);
            this.chb_dz0.TabIndex = 8;
            this.chb_dz0.TabStop = false;
            this.chb_dz0.Text = "首";
            this.chb_dz0.UseVisualStyleBackColor = true;
            // 
            // tb_dx1
            // 
            this.tb_dx1.CheckName = "chb_dx1";
            this.tb_dx1.DefValue = 0D;
            this.tb_dx1.Enob = 5;
            this.tb_dx1.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dx1.Location = new System.Drawing.Point(259, 47);
            this.tb_dx1.Name = "tb_dx1";
            this.tb_dx1.PropName = "work_transform.dx1";
            this.tb_dx1.Size = new System.Drawing.Size(100, 21);
            this.tb_dx1.TabIndex = 100;
            this.tb_dx1.Text = "0";
            this.tb_dx1.unitEnobType = "model_dist";
            this.tb_dx1.Value = 0D;
            // 
            // chb_dx0
            // 
            this.chb_dx0.AutoSize = true;
            this.chb_dx0.Checked = true;
            this.chb_dx0.CheckName = null;
            this.chb_dx0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dx0.Location = new System.Drawing.Point(69, 49);
            this.chb_dx0.Name = "chb_dx0";
            this.chb_dx0.PropName = null;
            this.chb_dx0.Size = new System.Drawing.Size(36, 16);
            this.chb_dx0.TabIndex = 10;
            this.chb_dx0.TabStop = false;
            this.chb_dx0.Text = "首";
            this.chb_dx0.UseVisualStyleBackColor = true;
            // 
            // chb_dy0
            // 
            this.chb_dy0.AutoSize = true;
            this.chb_dy0.Checked = true;
            this.chb_dy0.CheckName = null;
            this.chb_dy0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dy0.Location = new System.Drawing.Point(69, 76);
            this.chb_dy0.Name = "chb_dy0";
            this.chb_dy0.PropName = null;
            this.chb_dy0.Size = new System.Drawing.Size(36, 16);
            this.chb_dy0.TabIndex = 6;
            this.chb_dy0.TabStop = false;
            this.chb_dy0.Text = "首";
            this.chb_dy0.UseVisualStyleBackColor = true;
            // 
            // chb_dx1
            // 
            this.chb_dx1.AutoSize = true;
            this.chb_dx1.Checked = true;
            this.chb_dx1.CheckName = null;
            this.chb_dx1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dx1.Location = new System.Drawing.Point(218, 49);
            this.chb_dx1.Name = "chb_dx1";
            this.chb_dx1.PropName = null;
            this.chb_dx1.Size = new System.Drawing.Size(36, 16);
            this.chb_dx1.TabIndex = 16;
            this.chb_dx1.TabStop = false;
            this.chb_dx1.Text = "尾";
            this.chb_dx1.UseVisualStyleBackColor = true;
            // 
            // gb_pos
            // 
            this.gb_pos.Controls.Add(this.rb_offset_h);
            this.gb_pos.Controls.Add(this.rb_rot);
            this.gb_pos.Controls.Add(this.rb_offset_v);
            this.gb_pos.Controls.Add(this.chb_offset_h);
            this.gb_pos.Controls.Add(this.chb_rot);
            this.gb_pos.Controls.Add(this.tb_rot_angl);
            this.gb_pos.Controls.Add(this.chb_offset_v);
            this.gb_pos.Controls.Add(this.tb_offset_h);
            this.gb_pos.Controls.Add(this.tb_offset_v);
            this.gb_pos.Location = new System.Drawing.Point(6, 260);
            this.gb_pos.Name = "gb_pos";
            this.gb_pos.Size = new System.Drawing.Size(372, 104);
            this.gb_pos.TabIndex = 83;
            this.gb_pos.TabStop = false;
            this.gb_pos.Text = "位置";
            // 
            // rb_offset_h
            // 
            this.rb_offset_h.BackColor = System.Drawing.SystemColors.Control;
            this.rb_offset_h.btnCount = 3;
            this.rb_offset_h.btnSize = new System.Drawing.Size(42, 21);
            this.rb_offset_h.CheckName = "chb_offset_h";
            this.rb_offset_h.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_offset_h.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_offset_h.keys = new object[] {
        ((object)(0))};
            this.rb_offset_h.keys1 = new object[] {
        ((object)(0))};
            this.rb_offset_h.labels = new string[] {
        ""};
            this.rb_offset_h.Location = new System.Drawing.Point(59, 20);
            this.rb_offset_h.Margin = new System.Windows.Forms.Padding(0);
            this.rb_offset_h.Name = "rb_offset_h";
            this.rb_offset_h.PropName = "work_transform.offset_h_base";
            this.rb_offset_h.Size = new System.Drawing.Size(132, 21);
            this.rb_offset_h.TabIndex = 86;
            // 
            // rb_rot
            // 
            this.rb_rot.BackColor = System.Drawing.SystemColors.Control;
            this.rb_rot.btnCount = 4;
            this.rb_rot.btnSize = new System.Drawing.Size(42, 21);
            this.rb_rot.CheckName = "chb_rot";
            this.rb_rot.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_rot.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_rot.keys = new object[] {
        ((object)(0))};
            this.rb_rot.keys1 = new object[] {
        ((object)(0))};
            this.rb_rot.labels = new string[] {
        ""};
            this.rb_rot.Location = new System.Drawing.Point(59, 47);
            this.rb_rot.Margin = new System.Windows.Forms.Padding(0);
            this.rb_rot.Name = "rb_rot";
            this.rb_rot.PropName = "work_transform.rot_base";
            this.rb_rot.Size = new System.Drawing.Size(177, 21);
            this.rb_rot.TabIndex = 86;
            // 
            // rb_offset_v
            // 
            this.rb_offset_v.BackColor = System.Drawing.SystemColors.Control;
            this.rb_offset_v.btnCount = 3;
            this.rb_offset_v.btnSize = new System.Drawing.Size(42, 21);
            this.rb_offset_v.CheckName = "chb_offset_v";
            this.rb_offset_v.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_offset_v.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_offset_v.keys = new object[] {
        ((object)(0))};
            this.rb_offset_v.keys1 = new object[] {
        ((object)(0))};
            this.rb_offset_v.labels = new string[] {
        ""};
            this.rb_offset_v.Location = new System.Drawing.Point(59, 74);
            this.rb_offset_v.Margin = new System.Windows.Forms.Padding(0);
            this.rb_offset_v.Name = "rb_offset_v";
            this.rb_offset_v.PropName = "work_transform.offset_v_base";
            this.rb_offset_v.Size = new System.Drawing.Size(132, 21);
            this.rb_offset_v.TabIndex = 86;
            // 
            // chb_offset_h
            // 
            this.chb_offset_h.AutoSize = true;
            this.chb_offset_h.Checked = true;
            this.chb_offset_h.CheckName = null;
            this.chb_offset_h.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_offset_h.Location = new System.Drawing.Point(8, 23);
            this.chb_offset_h.Name = "chb_offset_h";
            this.chb_offset_h.PropName = null;
            this.chb_offset_h.Size = new System.Drawing.Size(48, 16);
            this.chb_offset_h.TabIndex = 10;
            this.chb_offset_h.TabStop = false;
            this.chb_offset_h.Text = "横向";
            this.chb_offset_h.UseVisualStyleBackColor = true;
            // 
            // chb_rot
            // 
            this.chb_rot.AutoSize = true;
            this.chb_rot.Checked = true;
            this.chb_rot.CheckName = null;
            this.chb_rot.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rot.Location = new System.Drawing.Point(8, 49);
            this.chb_rot.Name = "chb_rot";
            this.chb_rot.PropName = null;
            this.chb_rot.Size = new System.Drawing.Size(48, 16);
            this.chb_rot.TabIndex = 8;
            this.chb_rot.TabStop = false;
            this.chb_rot.Text = "旋转";
            this.chb_rot.UseVisualStyleBackColor = true;
            // 
            // tb_rot_angl
            // 
            this.tb_rot_angl.CheckName = "chb_rot";
            this.tb_rot_angl.DefValue = 0D;
            this.tb_rot_angl.Enob = 5;
            this.tb_rot_angl.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_rot_angl.Location = new System.Drawing.Point(246, 47);
            this.tb_rot_angl.Name = "tb_rot_angl";
            this.tb_rot_angl.PropName = "work_transform.rot_angl";
            this.tb_rot_angl.Size = new System.Drawing.Size(113, 21);
            this.tb_rot_angl.TabIndex = 40;
            this.tb_rot_angl.Text = "0";
            this.tb_rot_angl.unitData = "deg";
            this.tb_rot_angl.unitDisp = "deg";
            this.tb_rot_angl.unitEnobType = "model_angl";
            this.tb_rot_angl.Value = 0D;
            // 
            // chb_offset_v
            // 
            this.chb_offset_v.AutoSize = true;
            this.chb_offset_v.Checked = true;
            this.chb_offset_v.CheckName = null;
            this.chb_offset_v.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_offset_v.Location = new System.Drawing.Point(8, 77);
            this.chb_offset_v.Name = "chb_offset_v";
            this.chb_offset_v.PropName = null;
            this.chb_offset_v.Size = new System.Drawing.Size(48, 16);
            this.chb_offset_v.TabIndex = 6;
            this.chb_offset_v.TabStop = false;
            this.chb_offset_v.Text = "竖向";
            this.chb_offset_v.UseVisualStyleBackColor = true;
            // 
            // tb_offset_h
            // 
            this.tb_offset_h.CheckName = "chb_offset_h";
            this.tb_offset_h.DefValue = 0D;
            this.tb_offset_h.Enob = 5;
            this.tb_offset_h.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_offset_h.Location = new System.Drawing.Point(246, 20);
            this.tb_offset_h.Name = "tb_offset_h";
            this.tb_offset_h.PropName = "work_transform.offset_h";
            this.tb_offset_h.Size = new System.Drawing.Size(113, 21);
            this.tb_offset_h.TabIndex = 20;
            this.tb_offset_h.Text = "0";
            this.tb_offset_h.unitEnobType = "model_dist";
            this.tb_offset_h.Value = 0D;
            // 
            // tb_offset_v
            // 
            this.tb_offset_v.CheckName = "chb_offset_v";
            this.tb_offset_v.DefValue = 0D;
            this.tb_offset_v.Enob = 5;
            this.tb_offset_v.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_offset_v.Location = new System.Drawing.Point(246, 74);
            this.tb_offset_v.Name = "tb_offset_v";
            this.tb_offset_v.PropName = "work_transform.offset_v";
            this.tb_offset_v.Size = new System.Drawing.Size(113, 21);
            this.tb_offset_v.TabIndex = 60;
            this.tb_offset_v.Text = "0";
            this.tb_offset_v.unitEnobType = "model_dist";
            this.tb_offset_v.Value = 0D;
            // 
            // gb_props
            // 
            this.gb_props.Controls.Add(this.cbo_class);
            this.gb_props.Controls.Add(this.chb_class);
            this.gb_props.Controls.Add(this.tb_finish);
            this.gb_props.Controls.Add(this.chb_polish);
            this.gb_props.Controls.Add(this.btn_mtrl);
            this.gb_props.Controls.Add(this.tb_mtrl);
            this.gb_props.Controls.Add(this.chb_mtrl);
            this.gb_props.Controls.Add(this.btn_profile);
            this.gb_props.Controls.Add(this.tb_profile);
            this.gb_props.Controls.Add(this.tb_name);
            this.gb_props.Controls.Add(this.chb_profile);
            this.gb_props.Controls.Add(this.chb_name);
            this.gb_props.Location = new System.Drawing.Point(6, 94);
            this.gb_props.Name = "gb_props";
            this.gb_props.Size = new System.Drawing.Size(372, 160);
            this.gb_props.TabIndex = 1;
            this.gb_props.TabStop = false;
            this.gb_props.Text = " 信息";
            // 
            // cbo_class
            // 
            this.cbo_class.CheckName = "chb_class";
            this.cbo_class.DisplayMember = "";
            this.cbo_class.focusColor = System.Drawing.Color.FromArgb(((int)(((byte)(225)))), ((int)(((byte)(225)))), ((int)(((byte)(235)))));
            this.cbo_class.Font = new System.Drawing.Font("宋体", 9F);
            this.cbo_class.FormattingEnabled = true;
            this.cbo_class.ImageBorder = new System.Windows.Forms.Padding(0);
            this.cbo_class.ImageItemSize = new System.Drawing.Size(0, 0);
            this.cbo_class.KeyBrowse = true;
            this.cbo_class.Location = new System.Drawing.Point(87, 128);
            this.cbo_class.Name = "cbo_class";
            this.cbo_class.PropName = "class";
            
            this.cbo_class.Size = new System.Drawing.Size(266, 21);
            this.cbo_class.TabIndex = 6;
            this.cbo_class.Text = "-1";
            this.cbo_class.ValueMember = "";
            // 
            // chb_class
            // 
            this.chb_class.AutoSize = true;
            this.chb_class.Checked = true;
            this.chb_class.CheckName = null;
            this.chb_class.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_class.Location = new System.Drawing.Point(11, 130);
            this.chb_class.Name = "chb_class";
            this.chb_class.PropName = null;
            this.chb_class.Size = new System.Drawing.Size(48, 16);
            this.chb_class.TabIndex = 110;
            this.chb_class.TabStop = false;
            this.chb_class.Text = "等级";
            this.chb_class.UseVisualStyleBackColor = true;
            // 
            // tb_finish
            // 
            this.tb_finish.CheckName = "chb_polish";
            this.tb_finish.Location = new System.Drawing.Point(87, 101);
            this.tb_finish.Name = "tb_finish";
            this.tb_finish.PropName = "finish";
            this.tb_finish.Size = new System.Drawing.Size(266, 21);
            this.tb_finish.TabIndex = 5;
            this.tb_finish.Trim = false;
            // 
            // chb_polish
            // 
            this.chb_polish.AutoSize = true;
            this.chb_polish.Checked = true;
            this.chb_polish.CheckName = null;
            this.chb_polish.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_polish.Location = new System.Drawing.Point(11, 103);
            this.chb_polish.Name = "chb_polish";
            this.chb_polish.PropName = null;
            this.chb_polish.Size = new System.Drawing.Size(48, 16);
            this.chb_polish.TabIndex = 90;
            this.chb_polish.TabStop = false;
            this.chb_polish.Text = "抛光";
            this.chb_polish.UseVisualStyleBackColor = true;
            // 
            // btn_mtrl
            // 
            this.btn_mtrl.Location = new System.Drawing.Point(301, 72);
            this.btn_mtrl.Name = "btn_mtrl";
            this.btn_mtrl.Size = new System.Drawing.Size(54, 23);
            this.btn_mtrl.TabIndex = 4;
            this.btn_mtrl.TabStop = false;
            this.btn_mtrl.Text = "选择";
            this.btn_mtrl.UseVisualStyleBackColor = true;
            this.btn_mtrl.Click += new System.EventHandler(this.btn_mtrl_Click);
            // 
            // tb_mtrl
            // 
            this.tb_mtrl.CheckName = "chb_mtrl";
            this.tb_mtrl.FilterChars = "$";
            this.tb_mtrl.Location = new System.Drawing.Point(87, 74);
            this.tb_mtrl.Name = "tb_mtrl";
            this.tb_mtrl.PropName = "mtrl";
            this.tb_mtrl.Size = new System.Drawing.Size(206, 21);
            this.tb_mtrl.TabIndex = 3;
            this.tb_mtrl.Trim = false;
            // 
            // chb_mtrl
            // 
            this.chb_mtrl.AutoSize = true;
            this.chb_mtrl.Checked = true;
            this.chb_mtrl.CheckName = null;
            this.chb_mtrl.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_mtrl.Location = new System.Drawing.Point(11, 76);
            this.chb_mtrl.Name = "chb_mtrl";
            this.chb_mtrl.PropName = null;
            this.chb_mtrl.Size = new System.Drawing.Size(48, 16);
            this.chb_mtrl.TabIndex = 60;
            this.chb_mtrl.TabStop = false;
            this.chb_mtrl.Text = "材质";
            this.chb_mtrl.UseVisualStyleBackColor = true;
            // 
            // btn_profile
            // 
            this.btn_profile.Location = new System.Drawing.Point(301, 45);
            this.btn_profile.Name = "btn_profile";
            this.btn_profile.Size = new System.Drawing.Size(54, 23);
            this.btn_profile.TabIndex = 2;
            this.btn_profile.TabStop = false;
            this.btn_profile.Text = "选择";
            this.btn_profile.UseVisualStyleBackColor = true;
            this.btn_profile.Click += new System.EventHandler(this.btn_profile_Click);
            // 
            // tb_profile
            // 
            this.tb_profile.CheckName = "chb_profile";
            this.tb_profile.FilterChars = "$";
            this.tb_profile.Location = new System.Drawing.Point(87, 47);
            this.tb_profile.Name = "tb_profile";
            this.tb_profile.PropName = "profile";
            this.tb_profile.Size = new System.Drawing.Size(206, 21);
            this.tb_profile.TabIndex = 1;
            this.tb_profile.Trim = false;
            // 
            // tb_name
            // 
            this.tb_name.CheckName = "chb_name";
            this.tb_name.Location = new System.Drawing.Point(87, 20);
            this.tb_name.Name = "tb_name";
            this.tb_name.PropName = "name";
            this.tb_name.Size = new System.Drawing.Size(266, 21);
            this.tb_name.TabIndex = 0;
            this.tb_name.Trim = false;
            // 
            // chb_profile
            // 
            this.chb_profile.AutoSize = true;
            this.chb_profile.Checked = true;
            this.chb_profile.CheckName = null;
            this.chb_profile.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_profile.Location = new System.Drawing.Point(11, 49);
            this.chb_profile.Name = "chb_profile";
            this.chb_profile.PropName = null;
            this.chb_profile.Size = new System.Drawing.Size(72, 16);
            this.chb_profile.TabIndex = 30;
            this.chb_profile.TabStop = false;
            this.chb_profile.Text = "截面型材";
            this.chb_profile.UseVisualStyleBackColor = true;
            // 
            // chb_name
            // 
            this.chb_name.AutoSize = true;
            this.chb_name.Checked = true;
            this.chb_name.CheckName = null;
            this.chb_name.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_name.Location = new System.Drawing.Point(11, 22);
            this.chb_name.Name = "chb_name";
            this.chb_name.PropName = null;
            this.chb_name.Size = new System.Drawing.Size(48, 16);
            this.chb_name.TabIndex = 10;
            this.chb_name.TabStop = false;
            this.chb_name.Text = "名称";
            this.chb_name.UseVisualStyleBackColor = true;
            // 
            // gb_no
            // 
            this.gb_no.Controls.Add(this.tb_assembly_no_start);
            this.gb_no.Controls.Add(this.tb_no_start);
            this.gb_no.Controls.Add(this.tb_assembly_no_prefix);
            this.gb_no.Controls.Add(this.tb_no_prefix);
            this.gb_no.Controls.Add(this.chb_assembly_noStart);
            this.gb_no.Controls.Add(this.chb_part_noStart);
            this.gb_no.Controls.Add(this.chb_assembly_no);
            this.gb_no.Controls.Add(this.chb_part_no);
            this.gb_no.Location = new System.Drawing.Point(6, 6);
            this.gb_no.Name = "gb_no";
            this.gb_no.Size = new System.Drawing.Size(372, 82);
            this.gb_no.TabIndex = 0;
            this.gb_no.TabStop = false;
            this.gb_no.Text = "编号";
            // 
            // tb_assembly_no_start
            // 
            this.tb_assembly_no_start.CheckName = "chb_assembly_noStart";
            this.tb_assembly_no_start.Location = new System.Drawing.Point(239, 49);
            this.tb_assembly_no_start.Name = "tb_assembly_no_start";
            this.tb_assembly_no_start.PropName = "ass_no_start_postfix";
            this.tb_assembly_no_start.Size = new System.Drawing.Size(115, 21);
            this.tb_assembly_no_start.TabIndex = 3;
            this.tb_assembly_no_start.Text = "0";
            this.tb_assembly_no_start.Trim = false;
            this.tb_assembly_no_start.Value = "0";
            this.tb_assembly_no_start.placeholder = "  起始编号";
            // 
            // tb_no_start
            // 
            this.tb_no_start.CheckName = "chb_part_noStart";
            this.tb_no_start.Location = new System.Drawing.Point(239, 20);
            this.tb_no_start.Name = "tb_no_start";
            this.tb_no_start.PropName = "no_start_postfix";
            this.tb_no_start.Size = new System.Drawing.Size(115, 21);
            this.tb_no_start.TabIndex = 1;
            this.tb_no_start.Text = "0";
            this.tb_no_start.Trim = false;
            this.tb_no_start.Value = "0";
            this.tb_no_start.placeholder = "  起始编号";
            // 
            // tb_assembly_no_prefix
            // 
            this.tb_assembly_no_prefix.CheckName = "chb_assembly_no";
            this.tb_assembly_no_prefix.Location = new System.Drawing.Point(87, 49);
            this.tb_assembly_no_prefix.Name = "tb_assembly_no_prefix";
            this.tb_assembly_no_prefix.PropName = "ass_no_prefix";
            this.tb_assembly_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_assembly_no_prefix.TabIndex = 2;
            this.tb_assembly_no_prefix.Trim = false;
            this.tb_assembly_no_prefix.placeholder = "  前缀";
            // 
            // tb_no_prefix
            // 
            this.tb_no_prefix.CheckName = "chb_part_no";
            this.tb_no_prefix.Location = new System.Drawing.Point(87, 20);
            this.tb_no_prefix.Name = "tb_no_prefix";
            this.tb_no_prefix.PropName = "no_prefix";
            this.tb_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_no_prefix.TabIndex = 0;
            this.tb_no_prefix.Trim = false;
            this.tb_no_prefix.placeholder = "  前缀";
            // 
            // chb_assembly_noStart
            // 
            this.chb_assembly_noStart.AutoSize = true;
            this.chb_assembly_noStart.Checked = true;
            this.chb_assembly_noStart.CheckName = null;
            this.chb_assembly_noStart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_assembly_noStart.Location = new System.Drawing.Point(215, 52);
            this.chb_assembly_noStart.Name = "chb_assembly_noStart";
            this.chb_assembly_noStart.PropName = null;
            this.chb_assembly_noStart.Size = new System.Drawing.Size(15, 14);
            this.chb_assembly_noStart.TabIndex = 14;
            this.chb_assembly_noStart.TabStop = false;
            this.chb_assembly_noStart.UseVisualStyleBackColor = true;
            // 
            // chb_part_noStart
            // 
            this.chb_part_noStart.AutoSize = true;
            this.chb_part_noStart.Checked = true;
            this.chb_part_noStart.CheckName = null;
            this.chb_part_noStart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_noStart.Location = new System.Drawing.Point(215, 25);
            this.chb_part_noStart.Name = "chb_part_noStart";
            this.chb_part_noStart.PropName = null;
            this.chb_part_noStart.Size = new System.Drawing.Size(15, 14);
            this.chb_part_noStart.TabIndex = 12;
            this.chb_part_noStart.TabStop = false;
            this.chb_part_noStart.UseVisualStyleBackColor = true;
            // 
            // chb_assembly_no
            // 
            this.chb_assembly_no.AutoSize = true;
            this.chb_assembly_no.Checked = true;
            this.chb_assembly_no.CheckName = null;
            this.chb_assembly_no.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_assembly_no.Location = new System.Drawing.Point(11, 49);
            this.chb_assembly_no.Name = "chb_assembly_no";
            this.chb_assembly_no.PropName = null;
            this.chb_assembly_no.Size = new System.Drawing.Size(48, 16);
            this.chb_assembly_no.TabIndex = 40;
            this.chb_assembly_no.TabStop = false;
            this.chb_assembly_no.Text = "构件";
            this.chb_assembly_no.UseVisualStyleBackColor = true;
            // 
            // chb_part_no
            // 
            this.chb_part_no.AutoSize = true;
            this.chb_part_no.Checked = true;
            this.chb_part_no.CheckName = null;
            this.chb_part_no.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_no.Location = new System.Drawing.Point(11, 22);
            this.chb_part_no.Name = "chb_part_no";
            this.chb_part_no.PropName = null;
            this.chb_part_no.Size = new System.Drawing.Size(48, 16);
            this.chb_part_no.TabIndex = 10;
            this.chb_part_no.TabStop = false;
            this.chb_part_no.Text = "零件";
            this.chb_part_no.UseVisualStyleBackColor = true;
            // 
            // tp_2
            // 
            this.tp_2.Controls.Add(this.gb_deform);
            this.tp_2.Controls.Add(this.gb_curve);
            this.tp_2.Location = new System.Drawing.Point(4, 22);
            this.tp_2.Name = "tp_2";
            this.tp_2.Padding = new System.Windows.Forms.Padding(3);
            this.tp_2.Size = new System.Drawing.Size(387, 477);
            this.tp_2.TabIndex = 1;
            this.tp_2.Text = "变形";
            this.tp_2.UseVisualStyleBackColor = true;
            // 
            // gb_deform
            // 
            this.gb_deform.Controls.Add(this.tb_deform_exc_y);
            this.gb_deform.Controls.Add(this.tb_deform_end);
            this.gb_deform.Controls.Add(this.chb_deform_exc_y);
            this.gb_deform.Controls.Add(this.chb_deform_end);
            this.gb_deform.Controls.Add(this.tb_deform_exc_x);
            this.gb_deform.Controls.Add(this.chb_deform_exc_x);
            this.gb_deform.Controls.Add(this.tb_deform_start);
            this.gb_deform.Controls.Add(this.chb_deform_start);
            this.gb_deform.Location = new System.Drawing.Point(6, 95);
            this.gb_deform.Name = "gb_deform";
            this.gb_deform.Size = new System.Drawing.Size(316, 86);
            this.gb_deform.TabIndex = 8;
            this.gb_deform.TabStop = false;
            this.gb_deform.Text = "扭转";
            // 
            // tb_deform_exc_y
            // 
            this.tb_deform_exc_y.CheckName = "chb_deform_exc_y";
            this.tb_deform_exc_y.DefValue = 0D;
            this.tb_deform_exc_y.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_exc_y.Location = new System.Drawing.Point(217, 47);
            this.tb_deform_exc_y.Max = 10000D;
            this.tb_deform_exc_y.Min = -10000D;
            this.tb_deform_exc_y.Name = "tb_deform_exc_y";
            this.tb_deform_exc_y.PropName = "deform_exc_y";
            this.tb_deform_exc_y.Size = new System.Drawing.Size(79, 21);
            this.tb_deform_exc_y.TabGroup = "deform_exc";
            this.tb_deform_exc_y.TabIndex = 26;
            this.tb_deform_exc_y.Text = "0";
            this.tb_deform_exc_y.unitEnobType = "model_dist";
            this.tb_deform_exc_y.Value = 0D;
            // 
            // tb_deform_end
            // 
            this.tb_deform_end.CheckName = "chb_deform_end";
            this.tb_deform_end.DefValue = 0D;
            this.tb_deform_end.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_end.Location = new System.Drawing.Point(60, 47);
            this.tb_deform_end.Max = 10000D;
            this.tb_deform_end.Min = -10000D;
            this.tb_deform_end.Name = "tb_deform_end";
            this.tb_deform_end.PropName = "work_transform.deform_end";
            this.tb_deform_end.Size = new System.Drawing.Size(79, 21);
            this.tb_deform_end.TabGroup = "deform_angl";
            this.tb_deform_end.TabIndex = 26;
            this.tb_deform_end.Text = "0";
            this.tb_deform_end.unitData = "deg";
            this.tb_deform_end.unitDisp = "deg";
            this.tb_deform_end.unitEnobType = "model_angl";
            this.tb_deform_end.Value = 0D;
            // 
            // chb_deform_exc_y
            // 
            this.chb_deform_exc_y.AutoSize = true;
            this.chb_deform_exc_y.Checked = true;
            this.chb_deform_exc_y.CheckName = null;
            this.chb_deform_exc_y.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_exc_y.Location = new System.Drawing.Point(157, 49);
            this.chb_deform_exc_y.Name = "chb_deform_exc_y";
            this.chb_deform_exc_y.PropName = null;
            this.chb_deform_exc_y.Size = new System.Drawing.Size(54, 16);
            this.chb_deform_exc_y.TabGroup = "deform_exc";
            this.chb_deform_exc_y.TabIndex = 28;
            this.chb_deform_exc_y.TabStop = false;
            this.chb_deform_exc_y.Text = "Y偏心";
            this.chb_deform_exc_y.UseVisualStyleBackColor = true;
            // 
            // chb_deform_end
            // 
            this.chb_deform_end.AutoSize = true;
            this.chb_deform_end.Checked = true;
            this.chb_deform_end.CheckName = null;
            this.chb_deform_end.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_end.Location = new System.Drawing.Point(18, 49);
            this.chb_deform_end.Name = "chb_deform_end";
            this.chb_deform_end.PropName = null;
            this.chb_deform_end.Size = new System.Drawing.Size(36, 16);
            this.chb_deform_end.TabGroup = "deform_angl";
            this.chb_deform_end.TabIndex = 28;
            this.chb_deform_end.TabStop = false;
            this.chb_deform_end.Text = "尾";
            this.chb_deform_end.UseVisualStyleBackColor = true;
            // 
            // tb_deform_exc_x
            // 
            this.tb_deform_exc_x.CheckName = "chb_deform_exc_x";
            this.tb_deform_exc_x.DefValue = 0D;
            this.tb_deform_exc_x.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_exc_x.Location = new System.Drawing.Point(217, 20);
            this.tb_deform_exc_x.Max = 10000D;
            this.tb_deform_exc_x.Min = -10000D;
            this.tb_deform_exc_x.Name = "tb_deform_exc_x";
            this.tb_deform_exc_x.PropName = "deform_exc_x";
            this.tb_deform_exc_x.Size = new System.Drawing.Size(79, 21);
            this.tb_deform_exc_x.TabGroup = "deform_exc";
            this.tb_deform_exc_x.TabIndex = 25;
            this.tb_deform_exc_x.Text = "0";
            this.tb_deform_exc_x.unitEnobType = "model_dist";
            this.tb_deform_exc_x.Value = 0D;
            // 
            // chb_deform_exc_x
            // 
            this.chb_deform_exc_x.AutoSize = true;
            this.chb_deform_exc_x.Checked = true;
            this.chb_deform_exc_x.CheckName = null;
            this.chb_deform_exc_x.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_exc_x.Location = new System.Drawing.Point(157, 22);
            this.chb_deform_exc_x.Name = "chb_deform_exc_x";
            this.chb_deform_exc_x.PropName = null;
            this.chb_deform_exc_x.Size = new System.Drawing.Size(54, 16);
            this.chb_deform_exc_x.TabGroup = "deform_exc";
            this.chb_deform_exc_x.TabIndex = 27;
            this.chb_deform_exc_x.TabStop = false;
            this.chb_deform_exc_x.Text = "X偏心";
            this.chb_deform_exc_x.UseVisualStyleBackColor = true;
            // 
            // tb_deform_start
            // 
            this.tb_deform_start.CheckName = "chb_deform_start";
            this.tb_deform_start.DefValue = 0D;
            this.tb_deform_start.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_start.Location = new System.Drawing.Point(60, 20);
            this.tb_deform_start.Max = 10000D;
            this.tb_deform_start.Min = -10000D;
            this.tb_deform_start.Name = "tb_deform_start";
            this.tb_deform_start.PropName = "work_transform.deform_start";
            this.tb_deform_start.Size = new System.Drawing.Size(79, 21);
            this.tb_deform_start.TabGroup = "deform_angl";
            this.tb_deform_start.TabIndex = 25;
            this.tb_deform_start.Text = "0";
            this.tb_deform_start.unitData = "deg";
            this.tb_deform_start.unitDisp = "deg";
            this.tb_deform_start.unitEnobType = "model_angl";
            this.tb_deform_start.Value = 0D;
            // 
            // chb_deform_start
            // 
            this.chb_deform_start.AutoSize = true;
            this.chb_deform_start.Checked = true;
            this.chb_deform_start.CheckName = null;
            this.chb_deform_start.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_start.Location = new System.Drawing.Point(18, 22);
            this.chb_deform_start.Name = "chb_deform_start";
            this.chb_deform_start.PropName = null;
            this.chb_deform_start.Size = new System.Drawing.Size(36, 16);
            this.chb_deform_start.TabGroup = "deform_angl";
            this.chb_deform_start.TabIndex = 27;
            this.chb_deform_start.TabStop = false;
            this.chb_deform_start.Text = "首";
            this.chb_deform_start.UseVisualStyleBackColor = true;
            // 
            // gb_curve
            // 
            this.gb_curve.Controls.Add(this.tb_curve_segs);
            this.gb_curve.Controls.Add(this.lb_curve_segs);
            this.gb_curve.Controls.Add(this.tb_curve_r);
            this.gb_curve.Controls.Add(this.rb_curve_plane);
            this.gb_curve.Controls.Add(this.chb_curve);
            this.gb_curve.Location = new System.Drawing.Point(6, 6);
            this.gb_curve.Name = "gb_curve";
            this.gb_curve.Size = new System.Drawing.Size(316, 83);
            this.gb_curve.TabIndex = 7;
            this.gb_curve.TabStop = false;
            this.gb_curve.Text = "曲梁";
            // 
            // tb_curve_segs
            // 
            this.tb_curve_segs.CheckName = "chb_curve";
            this.tb_curve_segs.Location = new System.Drawing.Point(60, 47);
            this.tb_curve_segs.Name = "tb_curve_segs";
            this.tb_curve_segs.PropName = "curve_segs";
            this.tb_curve_segs.Size = new System.Drawing.Size(79, 21);
            this.tb_curve_segs.TabIndex = 140;
            this.tb_curve_segs.Text = "0";
            // 
            // lb_curve_segs
            // 
            this.lb_curve_segs.AutoSize = true;
            this.lb_curve_segs.Location = new System.Drawing.Point(25, 50);
            this.lb_curve_segs.Name = "lb_curve_segs";
            this.lb_curve_segs.Size = new System.Drawing.Size(29, 12);
            this.lb_curve_segs.TabIndex = 15;
            this.lb_curve_segs.Text = "分段";
            // 
            // tb_curve_r
            // 
            this.tb_curve_r.CheckName = "chb_curve";
            this.tb_curve_r.DefValue = 0D;
            this.tb_curve_r.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_curve_r.Location = new System.Drawing.Point(60, 20);
            this.tb_curve_r.Max = 99999999D;
            this.tb_curve_r.Min = -99999999D;
            this.tb_curve_r.Name = "tb_curve_r";
            this.tb_curve_r.PropName = "work_transform.curve_r";
            this.tb_curve_r.Size = new System.Drawing.Size(79, 21);
            this.tb_curve_r.TabIndex = 140;
            this.tb_curve_r.Text = "0";
            this.tb_curve_r.unitEnobType = "model_dist";
            this.tb_curve_r.Value = 0D;
            // 
            // rb_curve_plane
            // 
            this.rb_curve_plane.BackColor = System.Drawing.SystemColors.Control;
            this.rb_curve_plane.btnCount = 2;
            this.rb_curve_plane.btnSize = new System.Drawing.Size(42, 21);
            this.rb_curve_plane.CheckName = "chb_curve";
            this.rb_curve_plane.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_curve_plane.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_curve_plane.keys = new object[] {
        ((object)(0))};
            this.rb_curve_plane.keys1 = new object[] {
        ((object)(0))};
            this.rb_curve_plane.labels = new string[] {
        ""};
            this.rb_curve_plane.Location = new System.Drawing.Point(165, 20);
            this.rb_curve_plane.Margin = new System.Windows.Forms.Padding(0);
            this.rb_curve_plane.Name = "rb_curve_plane";
            this.rb_curve_plane.PropName = "work_transform.curve_plane";
            this.rb_curve_plane.Size = new System.Drawing.Size(87, 21);
            this.rb_curve_plane.TabIndex = 86;
            this.rb_curve_plane.TabStop = false;
            // 
            // chb_curve
            // 
            this.chb_curve.AutoSize = true;
            this.chb_curve.Checked = true;
            this.chb_curve.CheckName = null;
            this.chb_curve.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_curve.Location = new System.Drawing.Point(9, 22);
            this.chb_curve.Name = "chb_curve";
            this.chb_curve.PropName = null;
            this.chb_curve.Size = new System.Drawing.Size(48, 16);
            this.chb_curve.TabIndex = 10;
            this.chb_curve.TabStop = false;
            this.chb_curve.Text = "半径";
            this.chb_curve.UseVisualStyleBackColor = true;
            // 
            // tp_3
            // 
            this.tp_3.Location = new System.Drawing.Point(4, 22);
            this.tp_3.Name = "tp_3";
            this.tp_3.Size = new System.Drawing.Size(387, 477);
            this.tp_3.TabIndex = 3;
            this.tp_3.Text = "用户属性";
            this.tp_3.UseVisualStyleBackColor = true;
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(17, 551);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 2;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(146, 551);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 4;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(210, 551);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(58, 28);
            this.btn_pick.TabIndex = 5;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "获 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(338, 551);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 7;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(81, 551);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 3;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(274, 551);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 6;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.Btn_chb_Click);
            // 
            // radio1
            // 
            this.radio1.AutoSize = true;
            this.radio1.CheckName = "chb_rot";
            this.radio1.Location = new System.Drawing.Point(3, 2);
            this.radio1.Name = "radio1";
            this.radio1.PropName = "work_transform.rot_base";
            this.radio1.PropTarget = 0;
            this.radio1.Size = new System.Drawing.Size(35, 16);
            this.radio1.TabIndex = 23;
            this.radio1.TabStop = true;
            this.radio1.Text = "前";
            this.radio1.UseVisualStyleBackColor = true;
            // 
            // radio2
            // 
            this.radio2.AutoSize = true;
            this.radio2.CheckName = "chb_rot";
            this.radio2.Location = new System.Drawing.Point(44, 2);
            this.radio2.Name = "radio2";
            this.radio2.PropName = "work_transform.rot_base";
            this.radio2.PropTarget = 0;
            this.radio2.Size = new System.Drawing.Size(35, 16);
            this.radio2.TabIndex = 23;
            this.radio2.TabStop = true;
            this.radio2.Text = "顶";
            this.radio2.UseVisualStyleBackColor = true;
            // 
            // radio3
            // 
            this.radio3.AutoSize = true;
            this.radio3.CheckName = "chb_rot";
            this.radio3.Location = new System.Drawing.Point(85, 2);
            this.radio3.Name = "radio3";
            this.radio3.PropName = "work_transform.rot_base";
            this.radio3.PropTarget = 0;
            this.radio3.Size = new System.Drawing.Size(35, 16);
            this.radio3.TabIndex = 23;
            this.radio3.TabStop = true;
            this.radio3.Text = "后";
            this.radio3.UseVisualStyleBackColor = true;
            // 
            // radio4
            // 
            this.radio4.AutoSize = true;
            this.radio4.CheckName = "chb_rot";
            this.radio4.Location = new System.Drawing.Point(126, 2);
            this.radio4.Name = "radio4";
            this.radio4.PropName = "work_transform.rot_base";
            this.radio4.PropTarget = 0;
            this.radio4.Size = new System.Drawing.Size(35, 16);
            this.radio4.TabIndex = 23;
            this.radio4.TabStop = true;
            this.radio4.Text = "底";
            this.radio4.UseVisualStyleBackColor = true;
            // 
            // radio5
            // 
            this.radio5.AutoSize = true;
            this.radio5.CheckName = "chb_offset_v";
            this.radio5.Location = new System.Drawing.Point(3, 3);
            this.radio5.Name = "radio5";
            this.radio5.PropName = "work_transform.offset_v_base";
            this.radio5.PropTarget = 0;
            this.radio5.Size = new System.Drawing.Size(35, 16);
            this.radio5.TabIndex = 23;
            this.radio5.TabStop = true;
            this.radio5.Text = "前";
            this.radio5.UseVisualStyleBackColor = true;
            // 
            // radio6
            // 
            this.radio6.AutoSize = true;
            this.radio6.CheckName = "chb_offset_v";
            this.radio6.Location = new System.Drawing.Point(44, 3);
            this.radio6.Name = "radio6";
            this.radio6.PropName = "work_transform.offset_v_base";
            this.radio6.PropTarget = 0;
            this.radio6.Size = new System.Drawing.Size(35, 16);
            this.radio6.TabIndex = 23;
            this.radio6.TabStop = true;
            this.radio6.Text = "中";
            this.radio6.UseVisualStyleBackColor = true;
            // 
            // radio7
            // 
            this.radio7.AutoSize = true;
            this.radio7.CheckName = "chb_offset_v";
            this.radio7.Location = new System.Drawing.Point(85, 3);
            this.radio7.Name = "radio7";
            this.radio7.PropName = "work_transform.offset_v_base";
            this.radio7.PropTarget = 0;
            this.radio7.Size = new System.Drawing.Size(35, 16);
            this.radio7.TabIndex = 23;
            this.radio7.TabStop = true;
            this.radio7.Text = "后";
            this.radio7.UseVisualStyleBackColor = true;
            // 
            // radio8
            // 
            this.radio8.AutoSize = true;
            this.radio8.CheckName = "chb_offset_h";
            this.radio8.Location = new System.Drawing.Point(44, 3);
            this.radio8.Name = "radio8";
            this.radio8.PropName = "work_transform.offset_h_base";
            this.radio8.PropTarget = 0;
            this.radio8.Size = new System.Drawing.Size(35, 16);
            this.radio8.TabIndex = 23;
            this.radio8.TabStop = true;
            this.radio8.Text = "中";
            this.radio8.UseVisualStyleBackColor = true;
            // 
            // radio9
            // 
            this.radio9.AutoSize = true;
            this.radio9.CheckName = "chb_offset_h";
            this.radio9.Location = new System.Drawing.Point(3, 3);
            this.radio9.Name = "radio9";
            this.radio9.PropName = "work_transform.offset_h_base";
            this.radio9.PropTarget = 0;
            this.radio9.Size = new System.Drawing.Size(35, 16);
            this.radio9.TabIndex = 23;
            this.radio9.TabStop = true;
            this.radio9.Text = "左";
            this.radio9.UseVisualStyleBackColor = true;
            // 
            // radio10
            // 
            this.radio10.AutoSize = true;
            this.radio10.CheckName = "chb_offset_h";
            this.radio10.Location = new System.Drawing.Point(85, 3);
            this.radio10.Name = "radio10";
            this.radio10.PropName = "work_transform.offset_h_base";
            this.radio10.PropTarget = 0;
            this.radio10.Size = new System.Drawing.Size(35, 16);
            this.radio10.TabIndex = 23;
            this.radio10.TabStop = true;
            this.radio10.Text = "右";
            this.radio10.UseVisualStyleBackColor = true;
            // 
            // inputDouble1
            // 
            this.inputDouble1.CheckName = "chb_rot";
            this.inputDouble1.DefValue = 0D;
            this.inputDouble1.Enob = 5;
            this.inputDouble1.Font = new System.Drawing.Font("宋体", 9F);
            this.inputDouble1.Location = new System.Drawing.Point(255, 47);
            this.inputDouble1.Name = "inputDouble1";
            this.inputDouble1.PropName = "work_transform.rot_angl";
            this.inputDouble1.Size = new System.Drawing.Size(102, 21);
            this.inputDouble1.TabIndex = 40;
            this.inputDouble1.Text = "0";
            this.inputDouble1.unitData = "deg";
            this.inputDouble1.unitDisp = "deg";
            this.inputDouble1.unitEnobType = "model_angl";
            this.inputDouble1.Value = 0D;
            // 
            // inputDouble2
            // 
            this.inputDouble2.CheckName = "chb_offset_h";
            this.inputDouble2.DefValue = 0D;
            this.inputDouble2.Enob = 5;
            this.inputDouble2.Font = new System.Drawing.Font("宋体", 9F);
            this.inputDouble2.Location = new System.Drawing.Point(255, 20);
            this.inputDouble2.Name = "inputDouble2";
            this.inputDouble2.PropName = "work_transform.offset_h";
            this.inputDouble2.Size = new System.Drawing.Size(102, 21);
            this.inputDouble2.TabIndex = 20;
            this.inputDouble2.Text = "0";
            this.inputDouble2.unitEnobType = "model_dist";
            this.inputDouble2.Value = 0D;
            // 
            // check1
            // 
            this.check1.AutoSize = true;
            this.check1.Checked = true;
            this.check1.CheckName = null;
            this.check1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.check1.Location = new System.Drawing.Point(9, 24);
            this.check1.Name = "check1";
            this.check1.PropName = null;
            this.check1.Size = new System.Drawing.Size(48, 16);
            this.check1.TabIndex = 10;
            this.check1.TabStop = false;
            this.check1.Text = "横向";
            this.check1.UseVisualStyleBackColor = true;
            // 
            // check2
            // 
            this.check2.AutoSize = true;
            this.check2.Checked = true;
            this.check2.CheckName = null;
            this.check2.CheckState = System.Windows.Forms.CheckState.Checked;
            this.check2.Location = new System.Drawing.Point(9, 50);
            this.check2.Name = "check2";
            this.check2.PropName = null;
            this.check2.Size = new System.Drawing.Size(48, 16);
            this.check2.TabIndex = 8;
            this.check2.TabStop = false;
            this.check2.Text = "旋转";
            this.check2.UseVisualStyleBackColor = true;
            // 
            // inputDouble3
            // 
            this.inputDouble3.CheckName = "chb_offset_v";
            this.inputDouble3.DefValue = 0D;
            this.inputDouble3.Enob = 5;
            this.inputDouble3.Font = new System.Drawing.Font("宋体", 9F);
            this.inputDouble3.Location = new System.Drawing.Point(255, 74);
            this.inputDouble3.Name = "inputDouble3";
            this.inputDouble3.PropName = "work_transform.offset_v";
            this.inputDouble3.Size = new System.Drawing.Size(102, 21);
            this.inputDouble3.TabIndex = 60;
            this.inputDouble3.Text = "0";
            this.inputDouble3.unitEnobType = "model_dist";
            this.inputDouble3.Value = 0D;
            // 
            // check3
            // 
            this.check3.AutoSize = true;
            this.check3.Checked = true;
            this.check3.CheckName = null;
            this.check3.CheckState = System.Windows.Forms.CheckState.Checked;
            this.check3.Location = new System.Drawing.Point(9, 76);
            this.check3.Name = "check3";
            this.check3.PropName = null;
            this.check3.Size = new System.Drawing.Size(48, 16);
            this.check3.TabIndex = 6;
            this.check3.TabStop = false;
            this.check3.Text = "竖向";
            this.check3.UseVisualStyleBackColor = true;
            // 
            // bar_cfg
            // 
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(6, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(390, 33);
            this.bar_cfg.TabIndex = 0;
            // 
            // Form_SPBeamProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(410, 600);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.tc_props);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(426, 9999);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(426, 422);
            this.Name = "Form_SPBeamProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 梁属性";
            this.tc_props.ResumeLayout(false);
            this.tp_1.ResumeLayout(false);
            this.gb_drag.ResumeLayout(false);
            this.gb_drag.PerformLayout();
            this.gb_pos.ResumeLayout(false);
            this.gb_pos.PerformLayout();
            this.gb_props.ResumeLayout(false);
            this.gb_props.PerformLayout();
            this.gb_no.ResumeLayout(false);
            this.gb_no.PerformLayout();
            this.tp_2.ResumeLayout(false);
            this.gb_deform.ResumeLayout(false);
            this.gb_deform.PerformLayout();
            this.gb_curve.ResumeLayout(false);
            this.gb_curve.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.TabPage tp_2;
        private System.Windows.Forms.GroupBox gb_no;
        private UserCtrl.Check chb_assembly_noStart;
        private UserCtrl.Check chb_part_noStart;
        private UserCtrl.Check chb_assembly_no;
        private UserCtrl.Check chb_part_no;
        private UserCtrl.InputText tb_assembly_no_prefix;
        private UserCtrl.InputText tb_no_prefix;
        private UserCtrl.InputText tb_assembly_no_start;
        private UserCtrl.InputText tb_no_start;
        private System.Windows.Forms.GroupBox gb_props;
        private UserCtrl.InputText tb_profile;
        private UserCtrl.InputText tb_name;
        private UserCtrl.Check chb_profile;
        private UserCtrl.Check chb_name;
        private System.Windows.Forms.Button btn_profile;
        private System.Windows.Forms.Button btn_mtrl;
        private UserCtrl.InputText tb_mtrl;
        private UserCtrl.Check chb_mtrl;
        private UserCtrl.InputText tb_finish;
        private UserCtrl.Check chb_polish;
        private UserCtrl.Check chb_class;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private System.Windows.Forms.GroupBox gb_curve;
        private System.Windows.Forms.Label lb_curve_segs;
        private UserCtrl.InputDouble tb_curve_r;
        private UserCtrl.Check chb_curve;
        private UserCtrl.InputInt tb_curve_segs;
        private System.Windows.Forms.TabPage tp_3;
        private System.Windows.Forms.Button btn_chb;
        private CommonCfgBar bar_cfg;
        private System.Windows.Forms.GroupBox gb_deform;
        private UserCtrl.InputDouble tb_deform_end;
        private UserCtrl.InputDouble tb_deform_start;
        private UserCtrl.Check chb_deform_start;
        private UserCtrl.Combo cbo_class;
        private UserCtrl.Radio radio1;
        private UserCtrl.Radio radio2;
        private UserCtrl.Radio radio3;
        private UserCtrl.Radio radio4;
        private UserCtrl.Radio radio5;
        private UserCtrl.Radio radio6;
        private UserCtrl.Radio radio7;
        private UserCtrl.Radio radio8;
        private UserCtrl.Radio radio9;
        private UserCtrl.Radio radio10;
        private UserCtrl.InputDouble inputDouble1;
        private UserCtrl.InputDouble inputDouble2;
        private UserCtrl.Check check1;
        private UserCtrl.Check check2;
        private UserCtrl.InputDouble inputDouble3;
        private UserCtrl.Check check3;
        private UserCtrl.RadioButtons rb_curve_plane;
        private System.Windows.Forms.GroupBox gb_pos;
        private UserCtrl.RadioButtons rb_offset_h;
        private UserCtrl.RadioButtons rb_rot;
        private UserCtrl.RadioButtons rb_offset_v;
        private UserCtrl.Check chb_offset_h;
        private UserCtrl.Check chb_rot;
        private UserCtrl.InputDouble tb_rot_angl;
        private UserCtrl.Check chb_offset_v;
        private UserCtrl.InputDouble tb_offset_h;
        private UserCtrl.InputDouble tb_offset_v;
        private System.Windows.Forms.GroupBox gb_drag;
        private UserCtrl.InputDouble tb_dy0;
        private UserCtrl.InputDouble tb_dz0;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private UserCtrl.InputDouble tb_dx0;
        private UserCtrl.InputDouble tb_dz1;
        private UserCtrl.InputDouble tb_dy1;
        private UserCtrl.Check chb_dz1;
        private UserCtrl.Check chb_dy1;
        private UserCtrl.Check chb_dz0;
        private UserCtrl.InputDouble tb_dx1;
        private UserCtrl.Check chb_dx0;
        private UserCtrl.Check chb_dy0;
        private UserCtrl.Check chb_dx1;
        private UserCtrl.InputDouble tb_deform_exc_y;
        private UserCtrl.Check chb_deform_exc_y;
        private UserCtrl.Check chb_deform_end;
        private UserCtrl.InputDouble tb_deform_exc_x;
        private UserCtrl.Check chb_deform_exc_x;
    }
}