namespace UI
{
    partial class Form_SPPlateProps
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
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.gb_no = new System.Windows.Forms.GroupBox();
            this.tb_assembly_no_start = new UserCtrl.InputText();
            this.tb_no_start = new UserCtrl.InputText();
            this.tb_assembly_no_prefix = new UserCtrl.InputText();
            this.tb_no_prefix = new UserCtrl.InputText();
            this.chb_assembly_noStart = new UserCtrl.Check();
            this.chb_part_noStart = new UserCtrl.Check();
            this.chb_assembly_no = new UserCtrl.Check();
            this.chb_part_no = new UserCtrl.Check();
            this.gb_pos = new System.Windows.Forms.GroupBox();
            this.rb_offset_h = new UserCtrl.RadioButtons();
            this.tb_offset_h = new UserCtrl.InputDouble();
            this.chb_offset_h = new UserCtrl.Check();
            this.gb_props = new System.Windows.Forms.GroupBox();
            this.cbo_class = new UserCtrl.Combo();
            this.tb_profile = new UserCtrl.InputText();
            this.chb_profile = new UserCtrl.Check();
            this.chb_class = new UserCtrl.Check();
            this.tb_finish = new UserCtrl.InputText();
            this.chb_polish = new UserCtrl.Check();
            this.btn_mtrl = new System.Windows.Forms.Button();
            this.tb_mtrl = new UserCtrl.InputText();
            this.chb_mtrl = new UserCtrl.Check();
            this.tb_name = new UserCtrl.InputText();
            this.chb_name = new UserCtrl.Check();
            this.tc_props = new System.Windows.Forms.TabControl();
            this.tp_2 = new System.Windows.Forms.TabPage();
            this.btn_chb = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.tp_1.SuspendLayout();
            this.gb_no.SuspendLayout();
            this.gb_pos.SuspendLayout();
            this.gb_props.SuspendLayout();
            this.tc_props.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(18, 389);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 9;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(146, 389);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 10;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(210, 389);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(58, 28);
            this.btn_pick.TabIndex = 11;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "获 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(338, 389);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(82, 389);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 13;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.gb_no);
            this.tp_1.Controls.Add(this.gb_pos);
            this.tp_1.Controls.Add(this.gb_props);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(386, 315);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "属性";
            this.tp_1.UseVisualStyleBackColor = true;
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
            this.gb_no.Location = new System.Drawing.Point(7, 6);
            this.gb_no.Name = "gb_no";
            this.gb_no.Size = new System.Drawing.Size(367, 82);
            this.gb_no.TabIndex = 15;
            this.gb_no.TabStop = false;
            this.gb_no.Text = "编号";
            // 
            // tb_assembly_no_start
            // 
            this.tb_assembly_no_start.CheckName = "chb_assembly_noStart";
            this.tb_assembly_no_start.Location = new System.Drawing.Point(236, 49);
            this.tb_assembly_no_start.Name = "tb_assembly_no_start";
            this.tb_assembly_no_start.PropName = "ass_no_start_postfix";
            this.tb_assembly_no_start.Size = new System.Drawing.Size(118, 21);
            this.tb_assembly_no_start.TabIndex = 40;
            this.tb_assembly_no_start.Text = "0";
            this.tb_assembly_no_start.Trim = false;
            this.tb_assembly_no_start.Value = "0";
            this.tb_assembly_no_start.placeholder = "  起始编号";
            // 
            // tb_no_start
            // 
            this.tb_no_start.CheckName = "chb_part_noStart";
            this.tb_no_start.Location = new System.Drawing.Point(236, 20);
            this.tb_no_start.Name = "tb_no_start";
            this.tb_no_start.PropName = "no_start_postfix";
            this.tb_no_start.Size = new System.Drawing.Size(118, 21);
            this.tb_no_start.TabIndex = 20;
            this.tb_no_start.Text = "0";
            this.tb_no_start.Trim = false;
            this.tb_no_start.Value = "0";
            this.tb_no_start.placeholder = "  起始编号";
            // 
            // tb_assembly_no_prefix
            // 
            this.tb_assembly_no_prefix.CheckName = "chb_assembly_no";
            this.tb_assembly_no_prefix.Location = new System.Drawing.Point(86, 49);
            this.tb_assembly_no_prefix.Name = "tb_assembly_no_prefix";
            this.tb_assembly_no_prefix.PropName = "ass_no_prefix";
            this.tb_assembly_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_assembly_no_prefix.TabIndex = 30;
            this.tb_assembly_no_prefix.Trim = false;
            this.tb_assembly_no_prefix.placeholder = "  前缀";
            // 
            // tb_no_prefix
            // 
            this.tb_no_prefix.CheckName = "chb_part_no";
            this.tb_no_prefix.Location = new System.Drawing.Point(86, 20);
            this.tb_no_prefix.Name = "tb_no_prefix";
            this.tb_no_prefix.PropName = "no_prefix";
            this.tb_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_no_prefix.TabIndex = 10;
            this.tb_no_prefix.Trim = false;
            this.tb_no_prefix.placeholder = "  前缀";
            // 
            // chb_assembly_noStart
            // 
            this.chb_assembly_noStart.AutoSize = true;
            this.chb_assembly_noStart.Checked = true;
            this.chb_assembly_noStart.CheckName = null;
            this.chb_assembly_noStart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_assembly_noStart.Location = new System.Drawing.Point(213, 52);
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
            this.chb_part_noStart.Location = new System.Drawing.Point(213, 25);
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
            this.chb_assembly_no.Location = new System.Drawing.Point(10, 49);
            this.chb_assembly_no.Name = "chb_assembly_no";
            this.chb_assembly_no.PropName = null;
            this.chb_assembly_no.Size = new System.Drawing.Size(48, 16);
            this.chb_assembly_no.TabIndex = 8;
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
            this.chb_part_no.Location = new System.Drawing.Point(10, 22);
            this.chb_part_no.Name = "chb_part_no";
            this.chb_part_no.PropName = null;
            this.chb_part_no.Size = new System.Drawing.Size(48, 16);
            this.chb_part_no.TabIndex = 6;
            this.chb_part_no.TabStop = false;
            this.chb_part_no.Text = "零件";
            this.chb_part_no.UseVisualStyleBackColor = true;
            // 
            // gb_pos
            // 
            this.gb_pos.Controls.Add(this.rb_offset_h);
            this.gb_pos.Controls.Add(this.tb_offset_h);
            this.gb_pos.Controls.Add(this.chb_offset_h);
            this.gb_pos.Location = new System.Drawing.Point(6, 257);
            this.gb_pos.Name = "gb_pos";
            this.gb_pos.Size = new System.Drawing.Size(368, 54);
            this.gb_pos.TabIndex = 14;
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
            this.rb_offset_h.Location = new System.Drawing.Point(87, 20);
            this.rb_offset_h.Margin = new System.Windows.Forms.Padding(0);
            this.rb_offset_h.Name = "rb_offset_h";
            this.rb_offset_h.PropName = "work_transform.offset_h_base";
            this.rb_offset_h.Size = new System.Drawing.Size(132, 21);
            this.rb_offset_h.TabIndex = 121;
            // 
            // tb_offset_h
            // 
            this.tb_offset_h.CheckName = "chb_offset_h";
            this.tb_offset_h.DefValue = 0D;
            this.tb_offset_h.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_offset_h.Location = new System.Drawing.Point(237, 20);
            this.tb_offset_h.Name = "tb_offset_h";
            this.tb_offset_h.PropName = "work_transform.offset_h";
            this.tb_offset_h.Size = new System.Drawing.Size(116, 21);
            this.tb_offset_h.TabIndex = 120;
            this.tb_offset_h.Text = "0";
            this.tb_offset_h.unitEnobType = "model_dist";
            this.tb_offset_h.Value = 0D;
            // 
            // chb_offset_h
            // 
            this.chb_offset_h.AutoSize = true;
            this.chb_offset_h.Checked = true;
            this.chb_offset_h.CheckName = null;
            this.chb_offset_h.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_offset_h.Location = new System.Drawing.Point(12, 23);
            this.chb_offset_h.Name = "chb_offset_h";
            this.chb_offset_h.PropName = null;
            this.chb_offset_h.Size = new System.Drawing.Size(48, 16);
            this.chb_offset_h.TabIndex = 6;
            this.chb_offset_h.TabStop = false;
            this.chb_offset_h.Text = "深度";
            this.chb_offset_h.UseVisualStyleBackColor = true;
            // 
            // gb_props
            // 
            this.gb_props.Controls.Add(this.cbo_class);
            this.gb_props.Controls.Add(this.tb_profile);
            this.gb_props.Controls.Add(this.chb_profile);
            this.gb_props.Controls.Add(this.chb_class);
            this.gb_props.Controls.Add(this.tb_finish);
            this.gb_props.Controls.Add(this.chb_polish);
            this.gb_props.Controls.Add(this.btn_mtrl);
            this.gb_props.Controls.Add(this.tb_mtrl);
            this.gb_props.Controls.Add(this.chb_mtrl);
            this.gb_props.Controls.Add(this.tb_name);
            this.gb_props.Controls.Add(this.chb_name);
            this.gb_props.Location = new System.Drawing.Point(6, 94);
            this.gb_props.Name = "gb_props";
            this.gb_props.Size = new System.Drawing.Size(368, 157);
            this.gb_props.TabIndex = 5;
            this.gb_props.TabStop = false;
            this.gb_props.Text = "信息";
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
            
            this.cbo_class.Size = new System.Drawing.Size(267, 21);
            this.cbo_class.TabIndex = 103;
            this.cbo_class.Text = "-1";
            this.cbo_class.ValueMember = "";
            // 
            // tb_profile
            // 
            this.tb_profile.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.tb_profile.CheckName = "chb_profile";
            this.tb_profile.FilterChars = "+-*/$";
            this.tb_profile.Location = new System.Drawing.Point(87, 47);
            this.tb_profile.Name = "tb_profile";
            this.tb_profile.PropName = "profile";
            this.tb_profile.Size = new System.Drawing.Size(266, 21);
            this.tb_profile.TabIndex = 102;
            this.tb_profile.Trim = false;
            // 
            // chb_profile
            // 
            this.chb_profile.AutoSize = true;
            this.chb_profile.Checked = true;
            this.chb_profile.CheckName = null;
            this.chb_profile.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_profile.Location = new System.Drawing.Point(12, 49);
            this.chb_profile.Name = "chb_profile";
            this.chb_profile.PropName = null;
            this.chb_profile.Size = new System.Drawing.Size(72, 16);
            this.chb_profile.TabIndex = 101;
            this.chb_profile.TabStop = false;
            this.chb_profile.Text = "截面型材";
            this.chb_profile.UseVisualStyleBackColor = true;
            // 
            // chb_class
            // 
            this.chb_class.AutoSize = true;
            this.chb_class.Checked = true;
            this.chb_class.CheckName = null;
            this.chb_class.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_class.Location = new System.Drawing.Point(12, 130);
            this.chb_class.Name = "chb_class";
            this.chb_class.PropName = null;
            this.chb_class.Size = new System.Drawing.Size(48, 16);
            this.chb_class.TabIndex = 24;
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
            this.tb_finish.TabIndex = 90;
            this.tb_finish.Trim = false;
            // 
            // chb_polish
            // 
            this.chb_polish.AutoSize = true;
            this.chb_polish.Checked = true;
            this.chb_polish.CheckName = null;
            this.chb_polish.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_polish.Location = new System.Drawing.Point(12, 103);
            this.chb_polish.Name = "chb_polish";
            this.chb_polish.PropName = null;
            this.chb_polish.Size = new System.Drawing.Size(48, 16);
            this.chb_polish.TabIndex = 22;
            this.chb_polish.TabStop = false;
            this.chb_polish.Text = "抛光";
            this.chb_polish.UseVisualStyleBackColor = true;
            // 
            // btn_mtrl
            // 
            this.btn_mtrl.Location = new System.Drawing.Point(301, 72);
            this.btn_mtrl.Name = "btn_mtrl";
            this.btn_mtrl.Size = new System.Drawing.Size(54, 23);
            this.btn_mtrl.TabIndex = 80;
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
            this.tb_mtrl.TabIndex = 70;
            this.tb_mtrl.Trim = false;
            // 
            // chb_mtrl
            // 
            this.chb_mtrl.AutoSize = true;
            this.chb_mtrl.Checked = true;
            this.chb_mtrl.CheckName = null;
            this.chb_mtrl.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_mtrl.Location = new System.Drawing.Point(12, 76);
            this.chb_mtrl.Name = "chb_mtrl";
            this.chb_mtrl.PropName = null;
            this.chb_mtrl.Size = new System.Drawing.Size(48, 16);
            this.chb_mtrl.TabIndex = 19;
            this.chb_mtrl.TabStop = false;
            this.chb_mtrl.Text = "材质";
            this.chb_mtrl.UseVisualStyleBackColor = true;
            // 
            // tb_name
            // 
            this.tb_name.CheckName = "chb_name";
            this.tb_name.Location = new System.Drawing.Point(87, 20);
            this.tb_name.Name = "tb_name";
            this.tb_name.PropName = "name";
            this.tb_name.Size = new System.Drawing.Size(266, 21);
            this.tb_name.TabIndex = 50;
            this.tb_name.Trim = false;
            // 
            // chb_name
            // 
            this.chb_name.AutoSize = true;
            this.chb_name.Checked = true;
            this.chb_name.CheckName = null;
            this.chb_name.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_name.Location = new System.Drawing.Point(12, 20);
            this.chb_name.Name = "chb_name";
            this.chb_name.PropName = null;
            this.chb_name.Size = new System.Drawing.Size(48, 16);
            this.chb_name.TabIndex = 6;
            this.chb_name.TabStop = false;
            this.chb_name.Text = "名称";
            this.chb_name.UseVisualStyleBackColor = true;
            // 
            // tc_props
            // 
            this.tc_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_props.Controls.Add(this.tp_1);
            this.tc_props.Controls.Add(this.tp_2);
            this.tc_props.Location = new System.Drawing.Point(6, 42);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(394, 341);
            this.tc_props.TabIndex = 0;
            // 
            // tp_2
            // 
            this.tp_2.AutoScroll = true;
            this.tp_2.Location = new System.Drawing.Point(4, 22);
            this.tp_2.Name = "tp_2";
            this.tp_2.Size = new System.Drawing.Size(386, 318);
            this.tp_2.TabIndex = 1;
            this.tp_2.Text = "用户属性";
            this.tp_2.UseVisualStyleBackColor = true;
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(274, 389);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 30;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.Btn_chb_Click);
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
            this.bar_cfg.TabIndex = 553;
            this.bar_cfg.TabStop = false;
            // 
            // Form_SPPlateProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(406, 433);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.tc_props);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(422, 9999);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(422, 472);
            this.Name = "Form_SPPlateProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 多边形板属性";
            this.tp_1.ResumeLayout(false);
            this.gb_no.ResumeLayout(false);
            this.gb_no.PerformLayout();
            this.gb_pos.ResumeLayout(false);
            this.gb_pos.PerformLayout();
            this.gb_props.ResumeLayout(false);
            this.gb_props.PerformLayout();
            this.tc_props.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.GroupBox gb_props;
        private UserCtrl.Check chb_class;
        private UserCtrl.InputText tb_finish;
        private UserCtrl.Check chb_polish;
        private System.Windows.Forms.Button btn_mtrl;
        private UserCtrl.InputText tb_mtrl;
        private UserCtrl.Check chb_mtrl;
        private UserCtrl.InputText tb_name;
        private UserCtrl.Check chb_name;
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.GroupBox gb_pos;
        private UserCtrl.InputDouble tb_offset_h;
        private UserCtrl.Check chb_offset_h;
        private System.Windows.Forms.TabPage tp_2;
        private System.Windows.Forms.GroupBox gb_no;
        private UserCtrl.InputText tb_assembly_no_start;
        private UserCtrl.InputText tb_no_start;
        private UserCtrl.InputText tb_assembly_no_prefix;
        private UserCtrl.InputText tb_no_prefix;
        private UserCtrl.Check chb_assembly_noStart;
        private UserCtrl.Check chb_part_noStart;
        private UserCtrl.Check chb_assembly_no;
        private UserCtrl.Check chb_part_no;
        private System.Windows.Forms.Button btn_chb;
        private CommonCfgBar bar_cfg;
        private UserCtrl.InputText tb_profile;
        private UserCtrl.Check chb_profile;
        private UserCtrl.Combo cbo_class;
        private UserCtrl.RadioButtons rb_offset_h;
    }
}