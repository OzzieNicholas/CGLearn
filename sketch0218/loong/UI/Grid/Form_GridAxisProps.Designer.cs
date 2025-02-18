namespace UI
{
    partial class Form_GridAxisProps
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
            this.btn_close = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.tc_props = new System.Windows.Forms.TabControl();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.gb_mag = new System.Windows.Forms.GroupBox();
            this.chb_mag = new UserCtrl.Check();
            this.act_mag = new UserCtrl.Check();
            this.gp_common = new System.Windows.Forms.GroupBox();
            this.tb_label = new UserCtrl.InputText();
            this.chb_depth = new UserCtrl.Check();
            this.tb_depth = new UserCtrl.InputDouble();
            this.chb_label = new UserCtrl.Check();
            this.gb_other = new System.Windows.Forms.GroupBox();
            this.chb_auto_dist = new UserCtrl.Check();
            this.act_auto_dist = new UserCtrl.Check();
            this.chb_dr_vis = new UserCtrl.Check();
            this.act_dr_vis = new UserCtrl.Check();
            this.gb_extend = new System.Windows.Forms.GroupBox();
            this.tb_ext1_y = new UserCtrl.InputDouble();
            this.chb_ext1_y = new UserCtrl.Check();
            this.tb_ext0_y = new UserCtrl.InputDouble();
            this.chb_ext0_y = new UserCtrl.Check();
            this.tb_ext1_x = new UserCtrl.InputDouble();
            this.chb_ext1_x = new UserCtrl.Check();
            this.tb_ext0_x = new UserCtrl.InputDouble();
            this.chb_ext0_x = new UserCtrl.Check();
            this.tp_2 = new System.Windows.Forms.TabPage();
            this.chb_lock = new UserCtrl.Check();
            this.act_lock = new UserCtrl.Check();
            this.tc_props.SuspendLayout();
            this.tp_1.SuspendLayout();
            this.gb_mag.SuspendLayout();
            this.gp_common.SuspendLayout();
            this.gb_other.SuspendLayout();
            this.gb_extend.SuspendLayout();
            this.tp_2.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(41, 335);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 5;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 定";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(169, 335);
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
            this.btn_pick.Location = new System.Drawing.Point(233, 335);
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
            this.btn_close.Location = new System.Drawing.Point(361, 335);
            this.btn_close.Name = "btn_close";
            this.btn_close.Size = new System.Drawing.Size(58, 28);
            this.btn_close.TabIndex = 8;
            this.btn_close.TabStop = false;
            this.btn_close.Text = "关 闭";
            this.btn_close.UseVisualStyleBackColor = true;
            this.btn_close.Click += new System.EventHandler(this.gridForm_closeBtn_Click);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(297, 335);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 30;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.btn_chb_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(105, 335);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 33;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // bar_cfg
            // 
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(10, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(390, 33);
            this.bar_cfg.TabIndex = 553;
            // 
            // tc_props
            // 
            this.tc_props.Controls.Add(this.tp_1);
            this.tc_props.Controls.Add(this.tp_2);
            this.tc_props.Location = new System.Drawing.Point(4, 42);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(418, 288);
            this.tc_props.TabIndex = 556;
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.gb_mag);
            this.tp_1.Controls.Add(this.gp_common);
            this.tp_1.Controls.Add(this.gb_other);
            this.tp_1.Controls.Add(this.gb_extend);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(410, 262);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "属性";
            this.tp_1.UseVisualStyleBackColor = true;
            // 
            // gb_mag
            // 
            this.gb_mag.Controls.Add(this.chb_mag);
            this.gb_mag.Controls.Add(this.act_mag);
            this.gb_mag.Location = new System.Drawing.Point(7, 184);
            this.gb_mag.Name = "gb_mag";
            this.gb_mag.Size = new System.Drawing.Size(186, 72);
            this.gb_mag.TabIndex = 36;
            this.gb_mag.TabStop = false;
            this.gb_mag.Text = "磁性";
            // 
            // chb_mag
            // 
            this.chb_mag.AutoSize = true;
            this.chb_mag.Checked = true;
            this.chb_mag.CheckName = "act_mag";
            this.chb_mag.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_mag.Location = new System.Drawing.Point(71, 21);
            this.chb_mag.Name = "chb_mag";
            this.chb_mag.PropName = "mag";
            this.chb_mag.Size = new System.Drawing.Size(15, 14);
            this.chb_mag.TabIndex = 10;
            this.chb_mag.TabStop = false;
            this.chb_mag.UseVisualStyleBackColor = true;
            // 
            // act_mag
            // 
            this.act_mag.AutoSize = true;
            this.act_mag.Checked = true;
            this.act_mag.CheckName = null;
            this.act_mag.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_mag.Location = new System.Drawing.Point(14, 20);
            this.act_mag.Name = "act_mag";
            this.act_mag.PropName = null;
            this.act_mag.Size = new System.Drawing.Size(48, 16);
            this.act_mag.TabIndex = 9;
            this.act_mag.TabStop = false;
            this.act_mag.Text = "磁性";
            this.act_mag.UseVisualStyleBackColor = true;
            // 
            // gp_common
            // 
            this.gp_common.Controls.Add(this.tb_label);
            this.gp_common.Controls.Add(this.chb_depth);
            this.gp_common.Controls.Add(this.tb_depth);
            this.gp_common.Controls.Add(this.chb_label);
            this.gp_common.Location = new System.Drawing.Point(7, 6);
            this.gp_common.Name = "gp_common";
            this.gp_common.Size = new System.Drawing.Size(390, 84);
            this.gp_common.TabIndex = 35;
            this.gp_common.TabStop = false;
            // 
            // tb_label
            // 
            this.tb_label.CheckName = "chb_label";
            this.tb_label.Location = new System.Drawing.Point(95, 20);
            this.tb_label.Name = "tb_label";
            this.tb_label.PropName = "label";
            this.tb_label.Size = new System.Drawing.Size(151, 21);
            this.tb_label.TabIndex = 17;
            this.tb_label.Trim = false;
            // 
            // chb_depth
            // 
            this.chb_depth.AutoSize = true;
            this.chb_depth.Checked = true;
            this.chb_depth.CheckName = null;
            this.chb_depth.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_depth.Location = new System.Drawing.Point(12, 49);
            this.chb_depth.Name = "chb_depth";
            this.chb_depth.PropName = null;
            this.chb_depth.Size = new System.Drawing.Size(72, 16);
            this.chb_depth.TabIndex = 6;
            this.chb_depth.TabStop = false;
            this.chb_depth.Text = "视图深度";
            this.chb_depth.UseVisualStyleBackColor = true;
            // 
            // tb_depth
            // 
            this.tb_depth.CheckName = "chb_depth";
            this.tb_depth.DefValue = 0D;
            this.tb_depth.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_depth.Location = new System.Drawing.Point(95, 47);
            this.tb_depth.Min = 0D;
            this.tb_depth.Name = "tb_depth";
            this.tb_depth.PropName = "plane_height";
            this.tb_depth.Size = new System.Drawing.Size(151, 21);
            this.tb_depth.TabIndex = 7;
            this.tb_depth.Text = "0";
            this.tb_depth.unitEnobType = "model_dist";
            this.tb_depth.Value = 0D;
            // 
            // chb_label
            // 
            this.chb_label.AutoSize = true;
            this.chb_label.Checked = true;
            this.chb_label.CheckName = null;
            this.chb_label.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_label.Location = new System.Drawing.Point(12, 22);
            this.chb_label.Name = "chb_label";
            this.chb_label.PropName = null;
            this.chb_label.Size = new System.Drawing.Size(48, 16);
            this.chb_label.TabIndex = 0;
            this.chb_label.TabStop = false;
            this.chb_label.Text = "标签";
            this.chb_label.UseVisualStyleBackColor = true;
            // 
            // gb_other
            // 
            this.gb_other.Controls.Add(this.chb_auto_dist);
            this.gb_other.Controls.Add(this.act_auto_dist);
            this.gb_other.Controls.Add(this.chb_dr_vis);
            this.gb_other.Controls.Add(this.act_dr_vis);
            this.gb_other.Location = new System.Drawing.Point(199, 184);
            this.gb_other.Name = "gb_other";
            this.gb_other.Size = new System.Drawing.Size(198, 72);
            this.gb_other.TabIndex = 34;
            this.gb_other.TabStop = false;
            this.gb_other.Text = "其它";
            // 
            // chb_auto_dist
            // 
            this.chb_auto_dist.AutoSize = true;
            this.chb_auto_dist.Checked = true;
            this.chb_auto_dist.CheckName = "act_auto_dist";
            this.chb_auto_dist.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_auto_dist.Location = new System.Drawing.Point(138, 44);
            this.chb_auto_dist.Name = "chb_auto_dist";
            this.chb_auto_dist.PropName = "auto_dist";
            this.chb_auto_dist.Size = new System.Drawing.Size(15, 14);
            this.chb_auto_dist.TabIndex = 22;
            this.chb_auto_dist.TabStop = false;
            this.chb_auto_dist.UseVisualStyleBackColor = true;
            // 
            // act_auto_dist
            // 
            this.act_auto_dist.AutoSize = true;
            this.act_auto_dist.Checked = true;
            this.act_auto_dist.CheckName = null;
            this.act_auto_dist.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_auto_dist.Location = new System.Drawing.Point(10, 43);
            this.act_auto_dist.Name = "act_auto_dist";
            this.act_auto_dist.PropName = null;
            this.act_auto_dist.Size = new System.Drawing.Size(120, 16);
            this.act_auto_dist.TabIndex = 21;
            this.act_auto_dist.TabStop = false;
            this.act_auto_dist.Text = "自动标注轴线尺寸";
            this.act_auto_dist.UseVisualStyleBackColor = true;
            // 
            // chb_dr_vis
            // 
            this.chb_dr_vis.AutoSize = true;
            this.chb_dr_vis.Checked = true;
            this.chb_dr_vis.CheckName = "act_dr_vis";
            this.chb_dr_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dr_vis.Location = new System.Drawing.Point(138, 21);
            this.chb_dr_vis.Name = "chb_dr_vis";
            this.chb_dr_vis.PropName = "dr_vis";
            this.chb_dr_vis.Size = new System.Drawing.Size(15, 14);
            this.chb_dr_vis.TabIndex = 20;
            this.chb_dr_vis.TabStop = false;
            this.chb_dr_vis.UseVisualStyleBackColor = true;
            // 
            // act_dr_vis
            // 
            this.act_dr_vis.AutoSize = true;
            this.act_dr_vis.Checked = true;
            this.act_dr_vis.CheckName = null;
            this.act_dr_vis.CheckState = System.Windows.Forms.CheckState.Checked;
            this.act_dr_vis.Location = new System.Drawing.Point(10, 21);
            this.act_dr_vis.Name = "act_dr_vis";
            this.act_dr_vis.PropName = null;
            this.act_dr_vis.Size = new System.Drawing.Size(96, 16);
            this.act_dr_vis.TabIndex = 19;
            this.act_dr_vis.TabStop = false;
            this.act_dr_vis.Text = "在图纸中可见";
            this.act_dr_vis.UseVisualStyleBackColor = true;
            // 
            // gb_extend
            // 
            this.gb_extend.Controls.Add(this.tb_ext1_y);
            this.gb_extend.Controls.Add(this.chb_ext1_y);
            this.gb_extend.Controls.Add(this.tb_ext0_y);
            this.gb_extend.Controls.Add(this.chb_ext0_y);
            this.gb_extend.Controls.Add(this.tb_ext1_x);
            this.gb_extend.Controls.Add(this.chb_ext1_x);
            this.gb_extend.Controls.Add(this.tb_ext0_x);
            this.gb_extend.Controls.Add(this.chb_ext0_x);
            this.gb_extend.Location = new System.Drawing.Point(7, 96);
            this.gb_extend.Name = "gb_extend";
            this.gb_extend.Size = new System.Drawing.Size(390, 82);
            this.gb_extend.TabIndex = 33;
            this.gb_extend.TabStop = false;
            this.gb_extend.Text = "延伸";
            // 
            // tb_ext1_y
            // 
            this.tb_ext1_y.CheckName = "chb_ext1_y";
            this.tb_ext1_y.DefValue = 0D;
            this.tb_ext1_y.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_ext1_y.Location = new System.Drawing.Point(202, 47);
            this.tb_ext1_y.Name = "tb_ext1_y";
            this.tb_ext1_y.PropName = "ext1_y";
            this.tb_ext1_y.Size = new System.Drawing.Size(99, 21);
            this.tb_ext1_y.TabIndex = 15;
            this.tb_ext1_y.Text = "0";
            this.tb_ext1_y.unitEnobType = "model_dist";
            this.tb_ext1_y.Value = 0D;
            // 
            // chb_ext1_y
            // 
            this.chb_ext1_y.AutoSize = true;
            this.chb_ext1_y.Checked = true;
            this.chb_ext1_y.CheckName = null;
            this.chb_ext1_y.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_ext1_y.Location = new System.Drawing.Point(164, 50);
            this.chb_ext1_y.Name = "chb_ext1_y";
            this.chb_ext1_y.PropName = null;
            this.chb_ext1_y.Size = new System.Drawing.Size(36, 16);
            this.chb_ext1_y.TabIndex = 14;
            this.chb_ext1_y.TabStop = false;
            this.chb_ext1_y.Text = "上";
            this.chb_ext1_y.UseVisualStyleBackColor = true;
            // 
            // tb_ext0_y
            // 
            this.tb_ext0_y.CheckName = "chb_ext0_y";
            this.tb_ext0_y.DefValue = 0D;
            this.tb_ext0_y.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_ext0_y.Location = new System.Drawing.Point(52, 47);
            this.tb_ext0_y.Name = "tb_ext0_y";
            this.tb_ext0_y.PropName = "ext0_y";
            this.tb_ext0_y.Size = new System.Drawing.Size(99, 21);
            this.tb_ext0_y.TabIndex = 13;
            this.tb_ext0_y.Text = "0";
            this.tb_ext0_y.unitEnobType = "model_dist";
            this.tb_ext0_y.Value = 0D;
            // 
            // chb_ext0_y
            // 
            this.chb_ext0_y.AutoSize = true;
            this.chb_ext0_y.Checked = true;
            this.chb_ext0_y.CheckName = null;
            this.chb_ext0_y.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_ext0_y.Location = new System.Drawing.Point(14, 50);
            this.chb_ext0_y.Name = "chb_ext0_y";
            this.chb_ext0_y.PropName = null;
            this.chb_ext0_y.Size = new System.Drawing.Size(36, 16);
            this.chb_ext0_y.TabIndex = 12;
            this.chb_ext0_y.TabStop = false;
            this.chb_ext0_y.Text = "下";
            this.chb_ext0_y.UseVisualStyleBackColor = true;
            // 
            // tb_ext1_x
            // 
            this.tb_ext1_x.CheckName = "chb_ext1_x";
            this.tb_ext1_x.DefValue = 0D;
            this.tb_ext1_x.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_ext1_x.Location = new System.Drawing.Point(202, 20);
            this.tb_ext1_x.Name = "tb_ext1_x";
            this.tb_ext1_x.PropName = "ext1_x";
            this.tb_ext1_x.Size = new System.Drawing.Size(99, 21);
            this.tb_ext1_x.TabIndex = 11;
            this.tb_ext1_x.Text = "0";
            this.tb_ext1_x.unitEnobType = "model_dist";
            this.tb_ext1_x.Value = 0D;
            // 
            // chb_ext1_x
            // 
            this.chb_ext1_x.AutoSize = true;
            this.chb_ext1_x.Checked = true;
            this.chb_ext1_x.CheckName = null;
            this.chb_ext1_x.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_ext1_x.Location = new System.Drawing.Point(164, 23);
            this.chb_ext1_x.Name = "chb_ext1_x";
            this.chb_ext1_x.PropName = null;
            this.chb_ext1_x.Size = new System.Drawing.Size(36, 16);
            this.chb_ext1_x.TabIndex = 10;
            this.chb_ext1_x.TabStop = false;
            this.chb_ext1_x.Text = "右";
            this.chb_ext1_x.UseVisualStyleBackColor = true;
            // 
            // tb_ext0_x
            // 
            this.tb_ext0_x.CheckName = "chb_ext0_x";
            this.tb_ext0_x.DefValue = 0D;
            this.tb_ext0_x.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_ext0_x.Location = new System.Drawing.Point(52, 20);
            this.tb_ext0_x.Name = "tb_ext0_x";
            this.tb_ext0_x.PropName = "ext0_x";
            this.tb_ext0_x.Size = new System.Drawing.Size(99, 21);
            this.tb_ext0_x.TabIndex = 9;
            this.tb_ext0_x.Text = "0";
            this.tb_ext0_x.unitEnobType = "model_dist";
            this.tb_ext0_x.Value = 0D;
            // 
            // chb_ext0_x
            // 
            this.chb_ext0_x.AutoSize = true;
            this.chb_ext0_x.Checked = true;
            this.chb_ext0_x.CheckName = null;
            this.chb_ext0_x.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_ext0_x.Location = new System.Drawing.Point(14, 23);
            this.chb_ext0_x.Name = "chb_ext0_x";
            this.chb_ext0_x.PropName = null;
            this.chb_ext0_x.Size = new System.Drawing.Size(36, 16);
            this.chb_ext0_x.TabIndex = 8;
            this.chb_ext0_x.TabStop = false;
            this.chb_ext0_x.Text = "左";
            this.chb_ext0_x.UseVisualStyleBackColor = true;
            // 
            // tp_2
            // 
            this.tp_2.Controls.Add(this.chb_lock);
            this.tp_2.Controls.Add(this.act_lock);
            this.tp_2.Location = new System.Drawing.Point(4, 22);
            this.tp_2.Name = "tp_2";
            this.tp_2.Padding = new System.Windows.Forms.Padding(3);
            this.tp_2.Size = new System.Drawing.Size(410, 262);
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
            // Form_GridAxisProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(431, 377);
            this.Controls.Add(this.tc_props);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_close);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_GridAxisProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 轴线";
            this.tc_props.ResumeLayout(false);
            this.tp_1.ResumeLayout(false);
            this.gb_mag.ResumeLayout(false);
            this.gb_mag.PerformLayout();
            this.gp_common.ResumeLayout(false);
            this.gp_common.PerformLayout();
            this.gb_other.ResumeLayout(false);
            this.gb_other.PerformLayout();
            this.gb_extend.ResumeLayout(false);
            this.gb_extend.PerformLayout();
            this.tp_2.ResumeLayout(false);
            this.tp_2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_close;
        private System.Windows.Forms.Button btn_chb;
        private System.Windows.Forms.Button btn_apply;
        private CommonCfgBar bar_cfg;
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.GroupBox gb_mag;
        private UserCtrl.Check chb_mag;
        private UserCtrl.Check act_mag;
        private System.Windows.Forms.GroupBox gp_common;
        private UserCtrl.InputText tb_label;
        private UserCtrl.Check chb_depth;
        private UserCtrl.InputDouble tb_depth;
        private UserCtrl.Check chb_label;
        private System.Windows.Forms.GroupBox gb_other;
        private UserCtrl.Check chb_auto_dist;
        private UserCtrl.Check act_auto_dist;
        private UserCtrl.Check chb_dr_vis;
        private UserCtrl.Check act_dr_vis;
        private System.Windows.Forms.GroupBox gb_extend;
        private UserCtrl.InputDouble tb_ext1_y;
        private UserCtrl.Check chb_ext1_y;
        private UserCtrl.InputDouble tb_ext0_y;
        private UserCtrl.Check chb_ext0_y;
        private UserCtrl.InputDouble tb_ext1_x;
        private UserCtrl.Check chb_ext1_x;
        private UserCtrl.InputDouble tb_ext0_x;
        private UserCtrl.Check chb_ext0_x;
        private System.Windows.Forms.TabPage tp_2;
        private UserCtrl.Check chb_lock;
        private UserCtrl.Check act_lock;
    }
}