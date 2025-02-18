namespace UI
{
    partial class Form_ViewProps
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
            this.rb_proj = new UserCtrl.RadioButtons();
            this.rb_type = new UserCtrl.RadioButtons();
            this.chb_rotationH = new UserCtrl.Check();
            this.chb_rotationV = new UserCtrl.Check();
            this.tb_name = new UserCtrl.InputText();
            this.tb_rotationH = new UserCtrl.InputDouble();
            this.chb_name = new UserCtrl.Check();
            this.chb_type = new UserCtrl.Check();
            this.tb_rotationV = new UserCtrl.InputDouble();
            this.chb_proj = new UserCtrl.Check();
            this.gp_filter = new System.Windows.Forms.GroupBox();
            this.gb_workbox_depth = new System.Windows.Forms.GroupBox();
            this.tb_workBox_depth_up = new UserCtrl.InputDouble();
            this.tb_workBox_depth_down = new UserCtrl.InputDouble();
            this.chb_workBox_depth_up = new UserCtrl.Check();
            this.chb_workBox_depth_down = new UserCtrl.Check();
            this.btn_chb = new System.Windows.Forms.Button();
            this.gb_view = new System.Windows.Forms.GroupBox();
            this.lb_name_warnings = new System.Windows.Forms.Label();
            this.bar_cfg = new UI.CommonCfgBar();
            this.gp_filter.SuspendLayout();
            this.gb_workbox_depth.SuspendLayout();
            this.gb_view.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(90, 297);
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
            this.btn_modify.Location = new System.Drawing.Point(218, 297);
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
            this.btn_pick.Location = new System.Drawing.Point(282, 297);
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
            this.btn_cancel.Location = new System.Drawing.Point(410, 297);
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
            this.btn_apply.Location = new System.Drawing.Point(154, 297);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 13;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // rb_proj
            // 
            this.rb_proj.BackColor = System.Drawing.SystemColors.Control;
            this.rb_proj.btnCount = 2;
            this.rb_proj.btnSize = new System.Drawing.Size(42, 21);
            this.rb_proj.CheckName = "chb_proj";
            this.rb_proj.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_proj.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_proj.keys = new object[] {
        ((object)(0))};
            this.rb_proj.keys1 = new object[] {
        ((object)(0))};
            this.rb_proj.labels = new string[] {
        ""};
            this.rb_proj.Location = new System.Drawing.Point(82, 75);
            this.rb_proj.Margin = new System.Windows.Forms.Padding(0);
            this.rb_proj.Name = "rb_proj";
            this.rb_proj.PropName = "proj";
            this.rb_proj.Size = new System.Drawing.Size(87, 21);
            this.rb_proj.TabIndex = 88;
            // 
            // rb_type
            // 
            this.rb_type.BackColor = System.Drawing.SystemColors.Control;
            this.rb_type.btnCount = 2;
            this.rb_type.btnSize = new System.Drawing.Size(42, 21);
            this.rb_type.CheckName = "chb_type";
            this.rb_type.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_type.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_type.keys = new object[] {
        ((object)(0))};
            this.rb_type.keys1 = new object[] {
        ((object)(0))};
            this.rb_type.labels = new string[] {
        ""};
            this.rb_type.Location = new System.Drawing.Point(82, 48);
            this.rb_type.Margin = new System.Windows.Forms.Padding(0);
            this.rb_type.Name = "rb_type";
            this.rb_type.PropName = "type";
            this.rb_type.Size = new System.Drawing.Size(87, 21);
            this.rb_type.TabIndex = 87;
            // 
            // chb_rotationH
            // 
            this.chb_rotationH.AutoSize = true;
            this.chb_rotationH.Checked = true;
            this.chb_rotationH.CheckName = null;
            this.chb_rotationH.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rotationH.Location = new System.Drawing.Point(246, 50);
            this.chb_rotationH.Name = "chb_rotationH";
            this.chb_rotationH.PropName = null;
            this.chb_rotationH.Size = new System.Drawing.Size(72, 16);
            this.chb_rotationH.TabGroup = "right";
            this.chb_rotationH.TabIndex = 16;
            this.chb_rotationH.TabStop = false;
            this.chb_rotationH.Text = "水平角度";
            this.chb_rotationH.UseVisualStyleBackColor = true;
            // 
            // chb_rotationV
            // 
            this.chb_rotationV.AutoSize = true;
            this.chb_rotationV.Checked = true;
            this.chb_rotationV.CheckName = null;
            this.chb_rotationV.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rotationV.Location = new System.Drawing.Point(246, 76);
            this.chb_rotationV.Name = "chb_rotationV";
            this.chb_rotationV.PropName = null;
            this.chb_rotationV.Size = new System.Drawing.Size(72, 16);
            this.chb_rotationV.TabGroup = "right";
            this.chb_rotationV.TabIndex = 15;
            this.chb_rotationV.TabStop = false;
            this.chb_rotationV.Text = "垂直角度";
            this.chb_rotationV.UseVisualStyleBackColor = true;
            // 
            // tb_name
            // 
            this.tb_name.CheckName = "chb_name";
            this.tb_name.FilterChars = "*";
            this.tb_name.Location = new System.Drawing.Point(82, 20);
            this.tb_name.Name = "tb_name";
            this.tb_name.PropName = "name";
            this.tb_name.Size = new System.Drawing.Size(148, 21);
            this.tb_name.TabGroup = "left";
            this.tb_name.TabIndex = 10;
            // 
            // tb_rotationH
            // 
            this.tb_rotationH.CheckName = "chb_rotationH";
            this.tb_rotationH.DefValue = 0D;
            this.tb_rotationH.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_rotationH.Location = new System.Drawing.Point(330, 48);
            this.tb_rotationH.Name = "tb_rotationH";
            this.tb_rotationH.PropName = "rotH";
            this.tb_rotationH.Size = new System.Drawing.Size(113, 21);
            this.tb_rotationH.TabGroup = "right";
            this.tb_rotationH.TabIndex = 40;
            this.tb_rotationH.Text = "0";
            this.tb_rotationH.unitData = "deg";
            this.tb_rotationH.unitDisp = "deg";
            this.tb_rotationH.unitEnobType = "model_angl";
            this.tb_rotationH.Value = 0D;
            // 
            // chb_name
            // 
            this.chb_name.AutoSize = true;
            this.chb_name.Checked = true;
            this.chb_name.CheckName = null;
            this.chb_name.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_name.Location = new System.Drawing.Point(25, 22);
            this.chb_name.Name = "chb_name";
            this.chb_name.PropName = null;
            this.chb_name.Size = new System.Drawing.Size(48, 16);
            this.chb_name.TabGroup = "left";
            this.chb_name.TabIndex = 10;
            this.chb_name.TabStop = false;
            this.chb_name.Text = "名称";
            this.chb_name.UseVisualStyleBackColor = true;
            // 
            // chb_type
            // 
            this.chb_type.AutoSize = true;
            this.chb_type.Checked = true;
            this.chb_type.CheckName = null;
            this.chb_type.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_type.Location = new System.Drawing.Point(25, 50);
            this.chb_type.Name = "chb_type";
            this.chb_type.PropName = null;
            this.chb_type.Size = new System.Drawing.Size(48, 16);
            this.chb_type.TabGroup = "left";
            this.chb_type.TabIndex = 8;
            this.chb_type.TabStop = false;
            this.chb_type.Text = "视角";
            this.chb_type.UseVisualStyleBackColor = true;
            // 
            // tb_rotationV
            // 
            this.tb_rotationV.CheckName = "chb_rotationV";
            this.tb_rotationV.DefValue = 0D;
            this.tb_rotationV.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_rotationV.Location = new System.Drawing.Point(330, 75);
            this.tb_rotationV.Name = "tb_rotationV";
            this.tb_rotationV.PropName = "rotV";
            this.tb_rotationV.Size = new System.Drawing.Size(113, 21);
            this.tb_rotationV.TabGroup = "right";
            this.tb_rotationV.TabIndex = 50;
            this.tb_rotationV.Text = "0";
            this.tb_rotationV.unitData = "deg";
            this.tb_rotationV.unitDisp = "deg";
            this.tb_rotationV.unitEnobType = "model_angl";
            this.tb_rotationV.Value = 0D;
            // 
            // chb_proj
            // 
            this.chb_proj.AutoSize = true;
            this.chb_proj.Checked = true;
            this.chb_proj.CheckName = null;
            this.chb_proj.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_proj.Location = new System.Drawing.Point(25, 77);
            this.chb_proj.Name = "chb_proj";
            this.chb_proj.PropName = null;
            this.chb_proj.Size = new System.Drawing.Size(48, 16);
            this.chb_proj.TabGroup = "left";
            this.chb_proj.TabIndex = 6;
            this.chb_proj.TabStop = false;
            this.chb_proj.Text = "投影";
            this.chb_proj.UseVisualStyleBackColor = true;
            // 
            // gp_filter
            // 
            this.gp_filter.Controls.Add(this.gb_workbox_depth);
            this.gp_filter.Location = new System.Drawing.Point(12, 164);
            this.gp_filter.Name = "gp_filter";
            this.gp_filter.Size = new System.Drawing.Size(457, 124);
            this.gp_filter.TabIndex = 16;
            this.gp_filter.TabStop = false;
            this.gp_filter.Text = "可见性";
            // 
            // gb_workbox_depth
            // 
            this.gb_workbox_depth.Controls.Add(this.tb_workBox_depth_up);
            this.gb_workbox_depth.Controls.Add(this.tb_workBox_depth_down);
            this.gb_workbox_depth.Controls.Add(this.chb_workBox_depth_up);
            this.gb_workbox_depth.Controls.Add(this.chb_workBox_depth_down);
            this.gb_workbox_depth.Location = new System.Drawing.Point(13, 29);
            this.gb_workbox_depth.Name = "gb_workbox_depth";
            this.gb_workbox_depth.Size = new System.Drawing.Size(233, 76);
            this.gb_workbox_depth.TabIndex = 28;
            this.gb_workbox_depth.TabStop = false;
            this.gb_workbox_depth.Text = "显示深度";
            // 
            // tb_workBox_depth_up
            // 
            this.tb_workBox_depth_up.CheckName = "chb_workBox_depth_up";
            this.tb_workBox_depth_up.DefValue = 0D;
            this.tb_workBox_depth_up.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_workBox_depth_up.Location = new System.Drawing.Point(70, 20);
            this.tb_workBox_depth_up.Name = "tb_workBox_depth_up";
            this.tb_workBox_depth_up.PropName = "depth_up";
            this.tb_workBox_depth_up.Size = new System.Drawing.Size(148, 21);
            this.tb_workBox_depth_up.TabIndex = 60;
            this.tb_workBox_depth_up.Text = "0";
            this.tb_workBox_depth_up.unitEnobType = "model_dist";
            this.tb_workBox_depth_up.Value = 0D;
            // 
            // tb_workBox_depth_down
            // 
            this.tb_workBox_depth_down.CheckName = "chb_workBox_depth_down";
            this.tb_workBox_depth_down.DefValue = 0D;
            this.tb_workBox_depth_down.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_workBox_depth_down.Location = new System.Drawing.Point(70, 47);
            this.tb_workBox_depth_down.Name = "tb_workBox_depth_down";
            this.tb_workBox_depth_down.PropName = "depth_down";
            this.tb_workBox_depth_down.Size = new System.Drawing.Size(148, 21);
            this.tb_workBox_depth_down.TabIndex = 70;
            this.tb_workBox_depth_down.Text = "0";
            this.tb_workBox_depth_down.unitEnobType = "model_dist";
            this.tb_workBox_depth_down.Value = 0D;
            // 
            // chb_workBox_depth_up
            // 
            this.chb_workBox_depth_up.AutoSize = true;
            this.chb_workBox_depth_up.Checked = true;
            this.chb_workBox_depth_up.CheckName = null;
            this.chb_workBox_depth_up.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_workBox_depth_up.Location = new System.Drawing.Point(12, 24);
            this.chb_workBox_depth_up.Name = "chb_workBox_depth_up";
            this.chb_workBox_depth_up.PropName = null;
            this.chb_workBox_depth_up.Size = new System.Drawing.Size(48, 16);
            this.chb_workBox_depth_up.TabIndex = 22;
            this.chb_workBox_depth_up.TabStop = false;
            this.chb_workBox_depth_up.Text = "向上";
            this.chb_workBox_depth_up.UseVisualStyleBackColor = true;
            // 
            // chb_workBox_depth_down
            // 
            this.chb_workBox_depth_down.AutoSize = true;
            this.chb_workBox_depth_down.Checked = true;
            this.chb_workBox_depth_down.CheckName = null;
            this.chb_workBox_depth_down.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_workBox_depth_down.Location = new System.Drawing.Point(12, 49);
            this.chb_workBox_depth_down.Name = "chb_workBox_depth_down";
            this.chb_workBox_depth_down.PropName = null;
            this.chb_workBox_depth_down.Size = new System.Drawing.Size(48, 16);
            this.chb_workBox_depth_down.TabIndex = 23;
            this.chb_workBox_depth_down.TabStop = false;
            this.chb_workBox_depth_down.Text = "向下";
            this.chb_workBox_depth_down.UseVisualStyleBackColor = true;
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(346, 297);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 32;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.Btn_chb_Click);
            // 
            // gb_view
            // 
            this.gb_view.Controls.Add(this.lb_name_warnings);
            this.gb_view.Controls.Add(this.rb_proj);
            this.gb_view.Controls.Add(this.rb_type);
            this.gb_view.Controls.Add(this.chb_rotationH);
            this.gb_view.Controls.Add(this.chb_rotationV);
            this.gb_view.Controls.Add(this.tb_name);
            this.gb_view.Controls.Add(this.tb_rotationH);
            this.gb_view.Controls.Add(this.chb_name);
            this.gb_view.Controls.Add(this.chb_type);
            this.gb_view.Controls.Add(this.tb_rotationV);
            this.gb_view.Controls.Add(this.chb_proj);
            this.gb_view.Location = new System.Drawing.Point(12, 51);
            this.gb_view.Name = "gb_view";
            this.gb_view.Size = new System.Drawing.Size(457, 107);
            this.gb_view.TabIndex = 14;
            this.gb_view.TabStop = false;
            // 
            // lb_name_warnings
            // 
            this.lb_name_warnings.AutoSize = true;
            this.lb_name_warnings.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel, ((byte)(134)));
            this.lb_name_warnings.ForeColor = System.Drawing.Color.Red;
            this.lb_name_warnings.Location = new System.Drawing.Point(243, 22);
            this.lb_name_warnings.Name = "lb_name_warnings";
            this.lb_name_warnings.Size = new System.Drawing.Size(49, 13);
            this.lb_name_warnings.TabIndex = 89;
            this.lb_name_warnings.Text = "label1";
            // 
            // bar_cfg
            // 
            this.bar_cfg.AutoSize = true;
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(12, 12);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(455, 33);
            this.bar_cfg.TabIndex = 554;
            this.bar_cfg.TabStop = false;
            // 
            // Form_ViewProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(480, 337);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.gp_filter);
            this.Controls.Add(this.gb_view);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_ViewProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 视图属性";
            this.gp_filter.ResumeLayout(false);
            this.gb_workbox_depth.ResumeLayout(false);
            this.gb_workbox_depth.PerformLayout();
            this.gb_view.ResumeLayout(false);
            this.gb_view.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private UserCtrl.InputDouble tb_rotationH;
        private UserCtrl.Check chb_name;
        private UserCtrl.Check chb_type;
        private UserCtrl.InputDouble tb_rotationV;
        private UserCtrl.Check chb_proj;
        private UserCtrl.InputText tb_name;
        private UserCtrl.Check chb_rotationH;
        private UserCtrl.Check chb_rotationV;
        private System.Windows.Forms.GroupBox gp_filter;
        private UserCtrl.InputDouble tb_workBox_depth_up;
        private UserCtrl.InputDouble tb_workBox_depth_down;
        private UserCtrl.Check chb_workBox_depth_up;
        private UserCtrl.Check chb_workBox_depth_down;
        private System.Windows.Forms.GroupBox gb_workbox_depth;
        private System.Windows.Forms.Button btn_chb;
        private CommonCfgBar bar_cfg;
        private UserCtrl.RadioButtons rb_type;
        private UserCtrl.RadioButtons rb_proj;
        private System.Windows.Forms.GroupBox gb_view;
        private System.Windows.Forms.Label lb_name_warnings;
    }
}