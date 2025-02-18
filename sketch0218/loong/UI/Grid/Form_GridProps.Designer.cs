namespace UI
{
    partial class Form_GridProps
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
            this.btn_create = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.btn_close = new System.Windows.Forms.Button();
            this.tc_props = new System.Windows.Forms.TabControl();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.gb_mag = new System.Windows.Forms.GroupBox();
            this.act_mag_axis = new UserCtrl.Check();
            this.chb_mag_axis = new UserCtrl.Check();
            this.gb_offset = new System.Windows.Forms.GroupBox();
            this.tb_z0 = new UserCtrl.InputDouble();
            this.chb_z0 = new UserCtrl.Check();
            this.tb_y0 = new UserCtrl.InputDouble();
            this.chb_y0 = new UserCtrl.Check();
            this.tb_x0 = new UserCtrl.InputDouble();
            this.chb_x0 = new UserCtrl.Check();
            this.gb_extend = new System.Windows.Forms.GroupBox();
            this.lb_ext = new System.Windows.Forms.Label();
            this.tb_extZ = new UserCtrl.InputDouble();
            this.chb_extZ = new UserCtrl.Check();
            this.lb_extN = new System.Windows.Forms.Label();
            this.tb_extY = new UserCtrl.InputDouble();
            this.tb_extNZ = new UserCtrl.InputDouble();
            this.chb_extY = new UserCtrl.Check();
            this.chb_extNZ = new UserCtrl.Check();
            this.tb_extX = new UserCtrl.InputDouble();
            this.tb_extNY = new UserCtrl.InputDouble();
            this.chb_extX = new UserCtrl.Check();
            this.chb_extNY = new UserCtrl.Check();
            this.tb_extNX = new UserCtrl.InputDouble();
            this.chb_extNX = new UserCtrl.Check();
            this.gb_label = new System.Windows.Forms.GroupBox();
            this.lb_label_auto_tips = new System.Windows.Forms.Label();
            this.tb_labelZ = new UserCtrl.InputTextArray();
            this.chb_labelZ = new UserCtrl.Check();
            this.tb_labelY = new UserCtrl.InputTextArray();
            this.btn_labelX_auto = new System.Windows.Forms.Button();
            this.btn_labelY_auto = new System.Windows.Forms.Button();
            this.btn_labelZ_auto = new System.Windows.Forms.Button();
            this.chb_labelY = new UserCtrl.Check();
            this.tb_labelX = new UserCtrl.InputTextArray();
            this.chb_labelX = new UserCtrl.Check();
            this.gb_xyz = new System.Windows.Forms.GroupBox();
            this.tb_vecZ = new UserCtrl.InputDoubleArrayScript();
            this.chb_vecZ = new UserCtrl.Check();
            this.tb_vecY = new UserCtrl.InputDoubleArrayScript();
            this.chb_vecY = new UserCtrl.Check();
            this.tb_vecX = new UserCtrl.InputDoubleArrayScript();
            this.chb_vecX = new UserCtrl.Check();
            this.tp_2 = new System.Windows.Forms.TabPage();
            this.chb_lock = new UserCtrl.Check();
            this.act_lock = new UserCtrl.Check();
            this.btn_chb = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.act_reset_user_axis = new UserCtrl.Check();
            this.chb_reset_user_axis = new UserCtrl.Check();
            this.tc_props.SuspendLayout();
            this.tp_1.SuspendLayout();
            this.gb_mag.SuspendLayout();
            this.gb_offset.SuspendLayout();
            this.gb_extend.SuspendLayout();
            this.gb_label.SuspendLayout();
            this.gb_xyz.SuspendLayout();
            this.tp_2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_create
            // 
            this.btn_create.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_create.Location = new System.Drawing.Point(174, 524);
            this.btn_create.Name = "btn_create";
            this.btn_create.Size = new System.Drawing.Size(58, 28);
            this.btn_create.TabIndex = 5;
            this.btn_create.TabStop = false;
            this.btn_create.Text = "创 建";
            this.btn_create.UseVisualStyleBackColor = true;
            this.btn_create.Click += new System.EventHandler(this.btn_create_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(238, 524);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 6;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(302, 524);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(58, 28);
            this.btn_pick.TabIndex = 7;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "获 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // btn_close
            // 
            this.btn_close.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_close.Location = new System.Drawing.Point(430, 524);
            this.btn_close.Name = "btn_close";
            this.btn_close.Size = new System.Drawing.Size(58, 28);
            this.btn_close.TabIndex = 8;
            this.btn_close.TabStop = false;
            this.btn_close.Text = "关 闭";
            this.btn_close.UseVisualStyleBackColor = true;
            this.btn_close.Click += new System.EventHandler(this.gridForm_closeBtn_Click);
            // 
            // tc_props
            // 
            this.tc_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_props.Controls.Add(this.tp_1);
            this.tc_props.Controls.Add(this.tp_2);
            this.tc_props.Location = new System.Drawing.Point(7, 42);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(482, 476);
            this.tc_props.TabIndex = 555;
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.groupBox1);
            this.tp_1.Controls.Add(this.gb_mag);
            this.tp_1.Controls.Add(this.gb_offset);
            this.tp_1.Controls.Add(this.gb_extend);
            this.tp_1.Controls.Add(this.gb_label);
            this.tp_1.Controls.Add(this.gb_xyz);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(474, 450);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "属性";
            this.tp_1.UseVisualStyleBackColor = true;
            // 
            // gb_mag
            // 
            this.gb_mag.Controls.Add(this.act_mag_axis);
            this.gb_mag.Controls.Add(this.chb_mag_axis);
            this.gb_mag.Location = new System.Drawing.Point(6, 389);
            this.gb_mag.Name = "gb_mag";
            this.gb_mag.Size = new System.Drawing.Size(106, 51);
            this.gb_mag.TabIndex = 16;
            this.gb_mag.TabStop = false;
            this.gb_mag.Text = "磁性";
            // 
            // act_mag_axis
            // 
            this.act_mag_axis.AutoSize = true;
            this.act_mag_axis.Checked = true;
            this.act_mag_axis.CheckName = null;
            this.act_mag_axis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_mag_axis.Location = new System.Drawing.Point(6, 20);
            this.act_mag_axis.Name = "act_mag_axis";
            this.act_mag_axis.PropName = null;
            this.act_mag_axis.Size = new System.Drawing.Size(60, 16);
            this.act_mag_axis.TabIndex = 15;
            this.act_mag_axis.TabStop = false;
            this.act_mag_axis.Text = "磁性面";
            this.act_mag_axis.UseVisualStyleBackColor = true;
            // 
            // chb_mag_axis
            // 
            this.chb_mag_axis.AutoSize = true;
            this.chb_mag_axis.Checked = true;
            this.chb_mag_axis.CheckName = "act_mag_axis";
            this.chb_mag_axis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_mag_axis.Location = new System.Drawing.Point(73, 20);
            this.chb_mag_axis.Name = "chb_mag_axis";
            this.chb_mag_axis.PropName = "mag_axis";
            this.chb_mag_axis.Size = new System.Drawing.Size(15, 14);
            this.chb_mag_axis.TabIndex = 14;
            this.chb_mag_axis.TabStop = false;
            this.chb_mag_axis.UseVisualStyleBackColor = true;
            // 
            // gb_offset
            // 
            this.gb_offset.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_offset.Controls.Add(this.tb_z0);
            this.gb_offset.Controls.Add(this.chb_z0);
            this.gb_offset.Controls.Add(this.tb_y0);
            this.gb_offset.Controls.Add(this.chb_y0);
            this.gb_offset.Controls.Add(this.tb_x0);
            this.gb_offset.Controls.Add(this.chb_x0);
            this.gb_offset.Location = new System.Drawing.Point(294, 259);
            this.gb_offset.Name = "gb_offset";
            this.gb_offset.Size = new System.Drawing.Size(169, 124);
            this.gb_offset.TabIndex = 8;
            this.gb_offset.TabStop = false;
            this.gb_offset.Text = "原点";
            // 
            // tb_z0
            // 
            this.tb_z0.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_z0.CheckName = "chb_z0";
            this.tb_z0.DefValue = 0D;
            this.tb_z0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_z0.Location = new System.Drawing.Point(51, 93);
            this.tb_z0.Name = "tb_z0";
            this.tb_z0.PropName = "z0";
            this.tb_z0.Size = new System.Drawing.Size(104, 21);
            this.tb_z0.TabIndex = 17;
            this.tb_z0.Text = "0";
            this.tb_z0.unitEnobType = "model_dist";
            this.tb_z0.Value = 0D;
            // 
            // chb_z0
            // 
            this.chb_z0.AutoSize = true;
            this.chb_z0.Checked = true;
            this.chb_z0.CheckName = null;
            this.chb_z0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_z0.Location = new System.Drawing.Point(9, 96);
            this.chb_z0.Name = "chb_z0";
            this.chb_z0.PropName = null;
            this.chb_z0.Size = new System.Drawing.Size(36, 16);
            this.chb_z0.TabIndex = 16;
            this.chb_z0.TabStop = false;
            this.chb_z0.Text = "Z0";
            this.chb_z0.UseVisualStyleBackColor = true;
            // 
            // tb_y0
            // 
            this.tb_y0.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_y0.CheckName = "chb_y0";
            this.tb_y0.DefValue = 0D;
            this.tb_y0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_y0.Location = new System.Drawing.Point(51, 66);
            this.tb_y0.Name = "tb_y0";
            this.tb_y0.PropName = "y0";
            this.tb_y0.Size = new System.Drawing.Size(104, 21);
            this.tb_y0.TabIndex = 15;
            this.tb_y0.Text = "0";
            this.tb_y0.unitEnobType = "model_dist";
            this.tb_y0.Value = 0D;
            // 
            // chb_y0
            // 
            this.chb_y0.AutoSize = true;
            this.chb_y0.Checked = true;
            this.chb_y0.CheckName = null;
            this.chb_y0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_y0.Location = new System.Drawing.Point(9, 69);
            this.chb_y0.Name = "chb_y0";
            this.chb_y0.PropName = null;
            this.chb_y0.Size = new System.Drawing.Size(36, 16);
            this.chb_y0.TabIndex = 14;
            this.chb_y0.TabStop = false;
            this.chb_y0.Text = "Y0";
            this.chb_y0.UseVisualStyleBackColor = true;
            // 
            // tb_x0
            // 
            this.tb_x0.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_x0.CheckName = "chb_x0";
            this.tb_x0.DefValue = 0D;
            this.tb_x0.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_x0.Location = new System.Drawing.Point(51, 39);
            this.tb_x0.Name = "tb_x0";
            this.tb_x0.PropName = "x0";
            this.tb_x0.Size = new System.Drawing.Size(104, 21);
            this.tb_x0.TabIndex = 13;
            this.tb_x0.Text = "0";
            this.tb_x0.unitEnobType = "model_dist";
            this.tb_x0.Value = 0D;
            // 
            // chb_x0
            // 
            this.chb_x0.AutoSize = true;
            this.chb_x0.Checked = true;
            this.chb_x0.CheckName = null;
            this.chb_x0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_x0.Location = new System.Drawing.Point(9, 42);
            this.chb_x0.Name = "chb_x0";
            this.chb_x0.PropName = null;
            this.chb_x0.Size = new System.Drawing.Size(36, 16);
            this.chb_x0.TabIndex = 12;
            this.chb_x0.TabStop = false;
            this.chb_x0.Text = "X0";
            this.chb_x0.UseVisualStyleBackColor = true;
            // 
            // gb_extend
            // 
            this.gb_extend.Controls.Add(this.lb_ext);
            this.gb_extend.Controls.Add(this.tb_extZ);
            this.gb_extend.Controls.Add(this.chb_extZ);
            this.gb_extend.Controls.Add(this.lb_extN);
            this.gb_extend.Controls.Add(this.tb_extY);
            this.gb_extend.Controls.Add(this.tb_extNZ);
            this.gb_extend.Controls.Add(this.chb_extY);
            this.gb_extend.Controls.Add(this.chb_extNZ);
            this.gb_extend.Controls.Add(this.tb_extX);
            this.gb_extend.Controls.Add(this.tb_extNY);
            this.gb_extend.Controls.Add(this.chb_extX);
            this.gb_extend.Controls.Add(this.chb_extNY);
            this.gb_extend.Controls.Add(this.tb_extNX);
            this.gb_extend.Controls.Add(this.chb_extNX);
            this.gb_extend.Location = new System.Drawing.Point(6, 259);
            this.gb_extend.Name = "gb_extend";
            this.gb_extend.Size = new System.Drawing.Size(282, 124);
            this.gb_extend.TabIndex = 7;
            this.gb_extend.TabStop = false;
            this.gb_extend.Text = "延伸";
            // 
            // lb_ext
            // 
            this.lb_ext.AutoSize = true;
            this.lb_ext.Location = new System.Drawing.Point(205, 17);
            this.lb_ext.Name = "lb_ext";
            this.lb_ext.Size = new System.Drawing.Size(35, 12);
            this.lb_ext.TabIndex = 13;
            this.lb_ext.Text = "右/上";
            // 
            // tb_extZ
            // 
            this.tb_extZ.CheckName = "chb_extZ";
            this.tb_extZ.DefValue = 0D;
            this.tb_extZ.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extZ.Location = new System.Drawing.Point(176, 93);
            this.tb_extZ.Name = "tb_extZ";
            this.tb_extZ.PropName = "extZ";
            this.tb_extZ.Size = new System.Drawing.Size(99, 21);
            this.tb_extZ.TabIndex = 17;
            this.tb_extZ.Text = "0";
            this.tb_extZ.unitEnobType = "model_dist";
            this.tb_extZ.Value = 0D;
            // 
            // chb_extZ
            // 
            this.chb_extZ.AutoSize = true;
            this.chb_extZ.Checked = true;
            this.chb_extZ.CheckName = null;
            this.chb_extZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extZ.Location = new System.Drawing.Point(154, 97);
            this.chb_extZ.Name = "chb_extZ";
            this.chb_extZ.PropName = null;
            this.chb_extZ.Size = new System.Drawing.Size(15, 14);
            this.chb_extZ.TabIndex = 16;
            this.chb_extZ.TabStop = false;
            this.chb_extZ.UseVisualStyleBackColor = true;
            // 
            // lb_extN
            // 
            this.lb_extN.AutoSize = true;
            this.lb_extN.Location = new System.Drawing.Point(71, 17);
            this.lb_extN.Name = "lb_extN";
            this.lb_extN.Size = new System.Drawing.Size(35, 12);
            this.lb_extN.TabIndex = 12;
            this.lb_extN.Text = "左/下";
            // 
            // tb_extY
            // 
            this.tb_extY.CheckName = "chb_extY";
            this.tb_extY.DefValue = 0D;
            this.tb_extY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extY.Location = new System.Drawing.Point(176, 66);
            this.tb_extY.Name = "tb_extY";
            this.tb_extY.PropName = "extY";
            this.tb_extY.Size = new System.Drawing.Size(99, 21);
            this.tb_extY.TabIndex = 15;
            this.tb_extY.Text = "0";
            this.tb_extY.unitEnobType = "model_dist";
            this.tb_extY.Value = 0D;
            // 
            // tb_extNZ
            // 
            this.tb_extNZ.CheckName = "chb_extNZ";
            this.tb_extNZ.DefValue = 0D;
            this.tb_extNZ.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extNZ.Location = new System.Drawing.Point(42, 93);
            this.tb_extNZ.Name = "tb_extNZ";
            this.tb_extNZ.PropName = "extNZ";
            this.tb_extNZ.Size = new System.Drawing.Size(99, 21);
            this.tb_extNZ.TabIndex = 11;
            this.tb_extNZ.Text = "0";
            this.tb_extNZ.unitEnobType = "model_dist";
            this.tb_extNZ.Value = 0D;
            // 
            // chb_extY
            // 
            this.chb_extY.AutoSize = true;
            this.chb_extY.Checked = true;
            this.chb_extY.CheckName = null;
            this.chb_extY.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extY.Location = new System.Drawing.Point(154, 70);
            this.chb_extY.Name = "chb_extY";
            this.chb_extY.PropName = null;
            this.chb_extY.Size = new System.Drawing.Size(15, 14);
            this.chb_extY.TabIndex = 14;
            this.chb_extY.TabStop = false;
            this.chb_extY.UseVisualStyleBackColor = true;
            // 
            // chb_extNZ
            // 
            this.chb_extNZ.AutoSize = true;
            this.chb_extNZ.Checked = true;
            this.chb_extNZ.CheckName = null;
            this.chb_extNZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extNZ.Location = new System.Drawing.Point(6, 96);
            this.chb_extNZ.Name = "chb_extNZ";
            this.chb_extNZ.PropName = null;
            this.chb_extNZ.Size = new System.Drawing.Size(30, 16);
            this.chb_extNZ.TabIndex = 10;
            this.chb_extNZ.TabStop = false;
            this.chb_extNZ.Text = "Z";
            this.chb_extNZ.UseVisualStyleBackColor = true;
            // 
            // tb_extX
            // 
            this.tb_extX.CheckName = "chb_extX";
            this.tb_extX.DefValue = 0D;
            this.tb_extX.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extX.Location = new System.Drawing.Point(176, 39);
            this.tb_extX.Name = "tb_extX";
            this.tb_extX.PropName = "extX";
            this.tb_extX.Size = new System.Drawing.Size(99, 21);
            this.tb_extX.TabIndex = 13;
            this.tb_extX.Text = "0";
            this.tb_extX.unitEnobType = "model_dist";
            this.tb_extX.Value = 0D;
            // 
            // tb_extNY
            // 
            this.tb_extNY.CheckName = "chb_extNY";
            this.tb_extNY.DefValue = 0D;
            this.tb_extNY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extNY.Location = new System.Drawing.Point(42, 66);
            this.tb_extNY.Name = "tb_extNY";
            this.tb_extNY.PropName = "extNY";
            this.tb_extNY.Size = new System.Drawing.Size(99, 21);
            this.tb_extNY.TabIndex = 9;
            this.tb_extNY.Text = "0";
            this.tb_extNY.unitEnobType = "model_dist";
            this.tb_extNY.Value = 0D;
            // 
            // chb_extX
            // 
            this.chb_extX.AutoSize = true;
            this.chb_extX.Checked = true;
            this.chb_extX.CheckName = null;
            this.chb_extX.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extX.Location = new System.Drawing.Point(154, 43);
            this.chb_extX.Name = "chb_extX";
            this.chb_extX.PropName = null;
            this.chb_extX.Size = new System.Drawing.Size(15, 14);
            this.chb_extX.TabIndex = 12;
            this.chb_extX.TabStop = false;
            this.chb_extX.UseVisualStyleBackColor = true;
            // 
            // chb_extNY
            // 
            this.chb_extNY.AutoSize = true;
            this.chb_extNY.Checked = true;
            this.chb_extNY.CheckName = null;
            this.chb_extNY.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extNY.Location = new System.Drawing.Point(6, 69);
            this.chb_extNY.Name = "chb_extNY";
            this.chb_extNY.PropName = null;
            this.chb_extNY.Size = new System.Drawing.Size(30, 16);
            this.chb_extNY.TabIndex = 8;
            this.chb_extNY.TabStop = false;
            this.chb_extNY.Text = "Y";
            this.chb_extNY.UseVisualStyleBackColor = true;
            // 
            // tb_extNX
            // 
            this.tb_extNX.CheckName = "chb_extNX";
            this.tb_extNX.DefValue = 0D;
            this.tb_extNX.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_extNX.Location = new System.Drawing.Point(42, 39);
            this.tb_extNX.Name = "tb_extNX";
            this.tb_extNX.PropName = "extNX";
            this.tb_extNX.Size = new System.Drawing.Size(99, 21);
            this.tb_extNX.TabIndex = 7;
            this.tb_extNX.Text = "0";
            this.tb_extNX.unitEnobType = "model_dist";
            this.tb_extNX.Value = 0D;
            // 
            // chb_extNX
            // 
            this.chb_extNX.AutoSize = true;
            this.chb_extNX.Checked = true;
            this.chb_extNX.CheckName = null;
            this.chb_extNX.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_extNX.Location = new System.Drawing.Point(6, 42);
            this.chb_extNX.Name = "chb_extNX";
            this.chb_extNX.PropName = null;
            this.chb_extNX.Size = new System.Drawing.Size(30, 16);
            this.chb_extNX.TabIndex = 6;
            this.chb_extNX.TabStop = false;
            this.chb_extNX.Text = "X";
            this.chb_extNX.UseVisualStyleBackColor = true;
            // 
            // gb_label
            // 
            this.gb_label.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_label.Controls.Add(this.lb_label_auto_tips);
            this.gb_label.Controls.Add(this.tb_labelZ);
            this.gb_label.Controls.Add(this.chb_labelZ);
            this.gb_label.Controls.Add(this.tb_labelY);
            this.gb_label.Controls.Add(this.btn_labelX_auto);
            this.gb_label.Controls.Add(this.btn_labelY_auto);
            this.gb_label.Controls.Add(this.btn_labelZ_auto);
            this.gb_label.Controls.Add(this.chb_labelY);
            this.gb_label.Controls.Add(this.tb_labelX);
            this.gb_label.Controls.Add(this.chb_labelX);
            this.gb_label.Location = new System.Drawing.Point(6, 119);
            this.gb_label.Name = "gb_label";
            this.gb_label.Size = new System.Drawing.Size(457, 127);
            this.gb_label.TabIndex = 6;
            this.gb_label.TabStop = false;
            this.gb_label.Text = "标签";
            // 
            // lb_label_auto_tips
            // 
            this.lb_label_auto_tips.AutoSize = true;
            this.lb_label_auto_tips.Cursor = System.Windows.Forms.Cursors.Hand;
            this.lb_label_auto_tips.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lb_label_auto_tips.ForeColor = System.Drawing.SystemColors.Highlight;
            this.lb_label_auto_tips.Location = new System.Drawing.Point(391, 107);
            this.lb_label_auto_tips.Name = "lb_label_auto_tips";
            this.lb_label_auto_tips.Size = new System.Drawing.Size(53, 12);
            this.lb_label_auto_tips.TabIndex = 9;
            this.lb_label_auto_tips.Text = "自动帮助";
            this.lb_label_auto_tips.Click += new System.EventHandler(this.lb_label_auto_tips_Click);
            // 
            // tb_labelZ
            // 
            this.tb_labelZ.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_labelZ.CheckName = "chb_labelZ";
            this.tb_labelZ.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_labelZ.Location = new System.Drawing.Point(42, 74);
            this.tb_labelZ.Name = "tb_labelZ";
            this.tb_labelZ.PropName = "labelZ";
            this.tb_labelZ.Size = new System.Drawing.Size(345, 21);
            this.tb_labelZ.TabIndex = 5;
            this.tb_labelZ.Value = new string[0];
            // 
            // chb_labelZ
            // 
            this.chb_labelZ.AutoSize = true;
            this.chb_labelZ.Checked = true;
            this.chb_labelZ.CheckName = null;
            this.chb_labelZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_labelZ.Location = new System.Drawing.Point(6, 76);
            this.chb_labelZ.Name = "chb_labelZ";
            this.chb_labelZ.PropName = null;
            this.chb_labelZ.Size = new System.Drawing.Size(30, 16);
            this.chb_labelZ.TabIndex = 4;
            this.chb_labelZ.TabStop = false;
            this.chb_labelZ.Text = "Z";
            this.chb_labelZ.UseVisualStyleBackColor = true;
            // 
            // tb_labelY
            // 
            this.tb_labelY.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_labelY.CheckName = "chb_labelY";
            this.tb_labelY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_labelY.Location = new System.Drawing.Point(42, 47);
            this.tb_labelY.Name = "tb_labelY";
            this.tb_labelY.PropName = "labelY";
            this.tb_labelY.Size = new System.Drawing.Size(345, 21);
            this.tb_labelY.TabIndex = 3;
            this.tb_labelY.Value = new string[0];
            // 
            // btn_labelX_auto
            // 
            this.btn_labelX_auto.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_labelX_auto.Location = new System.Drawing.Point(393, 20);
            this.btn_labelX_auto.Name = "btn_labelX_auto";
            this.btn_labelX_auto.Size = new System.Drawing.Size(50, 21);
            this.btn_labelX_auto.TabIndex = 8;
            this.btn_labelX_auto.TabStop = false;
            this.btn_labelX_auto.Text = "自动";
            this.btn_labelX_auto.UseVisualStyleBackColor = true;
            this.btn_labelX_auto.Click += new System.EventHandler(this.btn_labelX_auto_Click);
            // 
            // btn_labelY_auto
            // 
            this.btn_labelY_auto.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_labelY_auto.Location = new System.Drawing.Point(393, 47);
            this.btn_labelY_auto.Name = "btn_labelY_auto";
            this.btn_labelY_auto.Size = new System.Drawing.Size(50, 21);
            this.btn_labelY_auto.TabIndex = 8;
            this.btn_labelY_auto.TabStop = false;
            this.btn_labelY_auto.Text = "自动";
            this.btn_labelY_auto.UseVisualStyleBackColor = true;
            this.btn_labelY_auto.Click += new System.EventHandler(this.btn_labelY_auto_Click);
            // 
            // btn_labelZ_auto
            // 
            this.btn_labelZ_auto.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_labelZ_auto.Location = new System.Drawing.Point(393, 74);
            this.btn_labelZ_auto.Name = "btn_labelZ_auto";
            this.btn_labelZ_auto.Size = new System.Drawing.Size(50, 21);
            this.btn_labelZ_auto.TabIndex = 8;
            this.btn_labelZ_auto.TabStop = false;
            this.btn_labelZ_auto.Text = "自动";
            this.btn_labelZ_auto.UseVisualStyleBackColor = true;
            this.btn_labelZ_auto.Click += new System.EventHandler(this.btn_labelZ_auto_Click);
            // 
            // chb_labelY
            // 
            this.chb_labelY.AutoSize = true;
            this.chb_labelY.Checked = true;
            this.chb_labelY.CheckName = null;
            this.chb_labelY.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_labelY.Location = new System.Drawing.Point(6, 49);
            this.chb_labelY.Name = "chb_labelY";
            this.chb_labelY.PropName = null;
            this.chb_labelY.Size = new System.Drawing.Size(30, 16);
            this.chb_labelY.TabIndex = 2;
            this.chb_labelY.TabStop = false;
            this.chb_labelY.Text = "Y";
            this.chb_labelY.UseVisualStyleBackColor = true;
            // 
            // tb_labelX
            // 
            this.tb_labelX.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_labelX.CheckName = "chb_labelX";
            this.tb_labelX.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_labelX.Location = new System.Drawing.Point(42, 20);
            this.tb_labelX.Name = "tb_labelX";
            this.tb_labelX.PropName = "labelX";
            this.tb_labelX.Size = new System.Drawing.Size(345, 21);
            this.tb_labelX.TabIndex = 1;
            this.tb_labelX.Value = new string[0];
            // 
            // chb_labelX
            // 
            this.chb_labelX.AutoSize = true;
            this.chb_labelX.Checked = true;
            this.chb_labelX.CheckName = null;
            this.chb_labelX.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_labelX.Location = new System.Drawing.Point(6, 22);
            this.chb_labelX.Name = "chb_labelX";
            this.chb_labelX.PropName = null;
            this.chb_labelX.Size = new System.Drawing.Size(30, 16);
            this.chb_labelX.TabIndex = 0;
            this.chb_labelX.TabStop = false;
            this.chb_labelX.Text = "X";
            this.chb_labelX.UseVisualStyleBackColor = true;
            // 
            // gb_xyz
            // 
            this.gb_xyz.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_xyz.Controls.Add(this.tb_vecZ);
            this.gb_xyz.Controls.Add(this.chb_vecZ);
            this.gb_xyz.Controls.Add(this.tb_vecY);
            this.gb_xyz.Controls.Add(this.chb_vecY);
            this.gb_xyz.Controls.Add(this.tb_vecX);
            this.gb_xyz.Controls.Add(this.chb_vecX);
            this.gb_xyz.Location = new System.Drawing.Point(6, 6);
            this.gb_xyz.Name = "gb_xyz";
            this.gb_xyz.Size = new System.Drawing.Size(457, 107);
            this.gb_xyz.TabIndex = 5;
            this.gb_xyz.TabStop = false;
            this.gb_xyz.Text = "坐标";
            // 
            // tb_vecZ
            // 
            this.tb_vecZ.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_vecZ.AutoMerge = false;
            this.tb_vecZ.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.tb_vecZ.CheckName = "chb_vecZ";
            this.tb_vecZ.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_vecZ.Location = new System.Drawing.Point(42, 70);
            this.tb_vecZ.Name = "tb_vecZ";
            this.tb_vecZ.PropName = "vecZ";
            this.tb_vecZ.Size = new System.Drawing.Size(401, 21);
            this.tb_vecZ.TabIndex = 5;
            this.tb_vecZ.Text = "0";
            this.tb_vecZ.unitEnobType = "model_dist";
            this.tb_vecZ.Value = new double[] {
        0D};
            // 
            // chb_vecZ
            // 
            this.chb_vecZ.AutoSize = true;
            this.chb_vecZ.Checked = true;
            this.chb_vecZ.CheckName = null;
            this.chb_vecZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_vecZ.Location = new System.Drawing.Point(6, 72);
            this.chb_vecZ.Name = "chb_vecZ";
            this.chb_vecZ.PropName = null;
            this.chb_vecZ.Size = new System.Drawing.Size(30, 16);
            this.chb_vecZ.TabIndex = 4;
            this.chb_vecZ.TabStop = false;
            this.chb_vecZ.Text = "Z";
            this.chb_vecZ.UseVisualStyleBackColor = true;
            // 
            // tb_vecY
            // 
            this.tb_vecY.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_vecY.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.tb_vecY.CheckName = "chb_vecY";
            this.tb_vecY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_vecY.Location = new System.Drawing.Point(42, 43);
            this.tb_vecY.Name = "tb_vecY";
            this.tb_vecY.PropName = "vecY";
            this.tb_vecY.Size = new System.Drawing.Size(401, 21);
            this.tb_vecY.TabIndex = 3;
            this.tb_vecY.Text = "0";
            this.tb_vecY.unitEnobType = "model_dist";
            this.tb_vecY.Value = new double[] {
        0D};
            // 
            // chb_vecY
            // 
            this.chb_vecY.AutoSize = true;
            this.chb_vecY.Checked = true;
            this.chb_vecY.CheckName = null;
            this.chb_vecY.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_vecY.Location = new System.Drawing.Point(6, 45);
            this.chb_vecY.Name = "chb_vecY";
            this.chb_vecY.PropName = null;
            this.chb_vecY.Size = new System.Drawing.Size(30, 16);
            this.chb_vecY.TabIndex = 2;
            this.chb_vecY.TabStop = false;
            this.chb_vecY.Text = "Y";
            this.chb_vecY.UseVisualStyleBackColor = true;
            // 
            // tb_vecX
            // 
            this.tb_vecX.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_vecX.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.tb_vecX.CheckName = "chb_vecX";
            this.tb_vecX.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_vecX.Location = new System.Drawing.Point(42, 16);
            this.tb_vecX.Name = "tb_vecX";
            this.tb_vecX.PropName = "vecX";
            this.tb_vecX.Size = new System.Drawing.Size(401, 21);
            this.tb_vecX.TabIndex = 1;
            this.tb_vecX.Text = "0";
            this.tb_vecX.unitEnobType = "model_dist";
            this.tb_vecX.Value = new double[] {
        0D};
            // 
            // chb_vecX
            // 
            this.chb_vecX.AutoSize = true;
            this.chb_vecX.Checked = true;
            this.chb_vecX.CheckName = null;
            this.chb_vecX.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_vecX.Location = new System.Drawing.Point(6, 18);
            this.chb_vecX.Name = "chb_vecX";
            this.chb_vecX.PropName = null;
            this.chb_vecX.Size = new System.Drawing.Size(30, 16);
            this.chb_vecX.TabIndex = 0;
            this.chb_vecX.TabStop = false;
            this.chb_vecX.Text = "X";
            this.chb_vecX.UseVisualStyleBackColor = true;
            // 
            // tp_2
            // 
            this.tp_2.Controls.Add(this.chb_lock);
            this.tp_2.Controls.Add(this.act_lock);
            this.tp_2.Location = new System.Drawing.Point(4, 22);
            this.tp_2.Name = "tp_2";
            this.tp_2.Padding = new System.Windows.Forms.Padding(3);
            this.tp_2.Size = new System.Drawing.Size(474, 450);
            this.tp_2.TabIndex = 1;
            this.tp_2.Text = "用户属性";
            this.tp_2.UseVisualStyleBackColor = true;
            // 
            // chb_lock
            // 
            this.chb_lock.AutoSize = true;
            this.chb_lock.Checked = true;
            this.chb_lock.CheckName = "act_lock";
            this.chb_lock.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_lock.Location = new System.Drawing.Point(87, 18);
            this.chb_lock.Name = "chb_lock";
            this.chb_lock.PropName = "lock";
            this.chb_lock.Size = new System.Drawing.Size(15, 14);
            this.chb_lock.TabIndex = 12;
            this.chb_lock.TabStop = false;
            this.chb_lock.UseVisualStyleBackColor = true;
            // 
            // act_lock
            // 
            this.act_lock.AutoSize = true;
            this.act_lock.Checked = true;
            this.act_lock.CheckName = null;
            this.act_lock.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_lock.Location = new System.Drawing.Point(22, 17);
            this.act_lock.Name = "act_lock";
            this.act_lock.PropName = null;
            this.act_lock.Size = new System.Drawing.Size(48, 16);
            this.act_lock.TabIndex = 13;
            this.act_lock.TabStop = false;
            this.act_lock.Text = "锁定";
            this.act_lock.UseVisualStyleBackColor = true;
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(366, 524);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 30;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.btn_chb_Click);
            // 
            // bar_cfg
            // 
            this.bar_cfg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.bar_cfg.AutoSize = true;
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(7, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(478, 33);
            this.bar_cfg.TabIndex = 553;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.act_reset_user_axis);
            this.groupBox1.Controls.Add(this.chb_reset_user_axis);
            this.groupBox1.Location = new System.Drawing.Point(118, 389);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(170, 51);
            this.groupBox1.TabIndex = 16;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "自定义轴线";
            // 
            // act_reset_user_axis
            // 
            this.act_reset_user_axis.AutoSize = true;
            this.act_reset_user_axis.Checked = true;
            this.act_reset_user_axis.CheckName = null;
            this.act_reset_user_axis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_reset_user_axis.Location = new System.Drawing.Point(6, 20);
            this.act_reset_user_axis.Name = "act_reset_user_axis";
            this.act_reset_user_axis.PropName = null;
            this.act_reset_user_axis.Size = new System.Drawing.Size(108, 16);
            this.act_reset_user_axis.TabIndex = 15;
            this.act_reset_user_axis.TabStop = false;
            this.act_reset_user_axis.Text = "删除自定义轴线";
            this.act_reset_user_axis.UseVisualStyleBackColor = true;
            // 
            // chb_reset_user_axis
            // 
            this.chb_reset_user_axis.AutoSize = true;
            this.chb_reset_user_axis.Checked = true;
            this.chb_reset_user_axis.CheckName = "act_reset_user_axis";
            this.chb_reset_user_axis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_reset_user_axis.Location = new System.Drawing.Point(120, 20);
            this.chb_reset_user_axis.Name = "chb_reset_user_axis";
            this.chb_reset_user_axis.PropName = "reset_user_axis";
            this.chb_reset_user_axis.Size = new System.Drawing.Size(15, 14);
            this.chb_reset_user_axis.TabIndex = 14;
            this.chb_reset_user_axis.TabStop = false;
            this.chb_reset_user_axis.UseVisualStyleBackColor = true;
            // 
            // Form_GridProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(499, 566);
            this.Controls.Add(this.tc_props);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_close);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_create);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(515, 9999);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(515, 547);
            this.Name = "Form_GridProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 轴网";
            this.tc_props.ResumeLayout(false);
            this.tp_1.ResumeLayout(false);
            this.gb_mag.ResumeLayout(false);
            this.gb_mag.PerformLayout();
            this.gb_offset.ResumeLayout(false);
            this.gb_offset.PerformLayout();
            this.gb_extend.ResumeLayout(false);
            this.gb_extend.PerformLayout();
            this.gb_label.ResumeLayout(false);
            this.gb_label.PerformLayout();
            this.gb_xyz.ResumeLayout(false);
            this.gb_xyz.PerformLayout();
            this.tp_2.ResumeLayout(false);
            this.tp_2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_create;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_close;
        private System.Windows.Forms.Button btn_chb;
        private CommonCfgBar bar_cfg;
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.TabPage tp_2;
        private System.Windows.Forms.GroupBox gb_offset;
        private UserCtrl.InputDouble tb_z0;
        private UserCtrl.Check chb_z0;
        private UserCtrl.InputDouble tb_y0;
        private UserCtrl.Check chb_y0;
        private UserCtrl.InputDouble tb_x0;
        private UserCtrl.Check chb_x0;
        private System.Windows.Forms.GroupBox gb_extend;
        private System.Windows.Forms.Label lb_ext;
        private UserCtrl.InputDouble tb_extZ;
        private UserCtrl.Check chb_extZ;
        private System.Windows.Forms.Label lb_extN;
        private UserCtrl.InputDouble tb_extY;
        private UserCtrl.InputDouble tb_extNZ;
        private UserCtrl.Check chb_extY;
        private UserCtrl.Check chb_extNZ;
        private UserCtrl.InputDouble tb_extX;
        private UserCtrl.InputDouble tb_extNY;
        private UserCtrl.Check chb_extX;
        private UserCtrl.Check chb_extNY;
        private UserCtrl.InputDouble tb_extNX;
        private UserCtrl.Check chb_extNX;
        private System.Windows.Forms.GroupBox gb_label;
        private UserCtrl.InputTextArray tb_labelZ;
        private UserCtrl.Check chb_labelZ;
        private UserCtrl.InputTextArray tb_labelY;
        private UserCtrl.Check chb_labelY;
        private UserCtrl.InputTextArray tb_labelX;
        private UserCtrl.Check chb_labelX;
        private System.Windows.Forms.GroupBox gb_xyz;
        private UserCtrl.InputDoubleArrayScript tb_vecZ;
        private UserCtrl.Check chb_vecZ;
        private UserCtrl.InputDoubleArrayScript tb_vecY;
        private UserCtrl.Check chb_vecY;
        private UserCtrl.InputDoubleArrayScript tb_vecX;
        private UserCtrl.Check chb_vecX;
        private UserCtrl.Check chb_lock;
        private UserCtrl.Check act_lock;
        private System.Windows.Forms.Button btn_labelZ_auto;
        private System.Windows.Forms.Button btn_labelX_auto;
        private System.Windows.Forms.Button btn_labelY_auto;
        private System.Windows.Forms.Label lb_label_auto_tips;
        private System.Windows.Forms.GroupBox gb_mag;
        private UserCtrl.Check chb_mag_axis;
        private UserCtrl.Check act_mag_axis;
        private System.Windows.Forms.GroupBox groupBox1;
        private UserCtrl.Check act_reset_user_axis;
        private UserCtrl.Check chb_reset_user_axis;
    }
}