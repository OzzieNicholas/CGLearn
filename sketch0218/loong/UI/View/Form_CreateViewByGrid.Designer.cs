namespace UI
{
    partial class Form_CreateViewByGrid
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
            this.gb_view = new System.Windows.Forms.GroupBox();
            this.btn_props_xz = new System.Windows.Forms.Button();
            this.cbo_props_xz = new UserCtrl.ComboBoxText();
            this.tb_prefix_xz = new UserCtrl.InputText();
            this.cbo_number_xz = new UserCtrl.ComboBoxKey();
            this.lb_xz = new System.Windows.Forms.Label();
            this.btn_props_zy = new System.Windows.Forms.Button();
            this.cbo_props_zy = new UserCtrl.ComboBoxText();
            this.tb_prefix_zy = new UserCtrl.InputText();
            this.cbo_number_zy = new UserCtrl.ComboBoxKey();
            this.lb_zy = new System.Windows.Forms.Label();
            this.btn_props_xy = new System.Windows.Forms.Button();
            this.cbo_props_xy = new UserCtrl.ComboBoxText();
            this.tb_prefix_xy = new UserCtrl.InputText();
            this.cbo_number_xy = new UserCtrl.ComboBoxKey();
            this.lb_xy = new System.Windows.Forms.Label();
            this.lb_props = new System.Windows.Forms.Label();
            this.lb_prefix = new System.Windows.Forms.Label();
            this.lb_number = new System.Windows.Forms.Label();
            this.lb_plane = new System.Windows.Forms.Label();
            this.btn_create = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.gb_view.SuspendLayout();
            this.SuspendLayout();
            // 
            // gb_view
            // 
            this.gb_view.Controls.Add(this.btn_props_xz);
            this.gb_view.Controls.Add(this.cbo_props_xz);
            this.gb_view.Controls.Add(this.tb_prefix_xz);
            this.gb_view.Controls.Add(this.cbo_number_xz);
            this.gb_view.Controls.Add(this.lb_xz);
            this.gb_view.Controls.Add(this.btn_props_zy);
            this.gb_view.Controls.Add(this.cbo_props_zy);
            this.gb_view.Controls.Add(this.tb_prefix_zy);
            this.gb_view.Controls.Add(this.cbo_number_zy);
            this.gb_view.Controls.Add(this.lb_zy);
            this.gb_view.Controls.Add(this.btn_props_xy);
            this.gb_view.Controls.Add(this.cbo_props_xy);
            this.gb_view.Controls.Add(this.tb_prefix_xy);
            this.gb_view.Controls.Add(this.cbo_number_xy);
            this.gb_view.Controls.Add(this.lb_xy);
            this.gb_view.Controls.Add(this.lb_props);
            this.gb_view.Controls.Add(this.lb_prefix);
            this.gb_view.Controls.Add(this.lb_number);
            this.gb_view.Controls.Add(this.lb_plane);
            this.gb_view.Location = new System.Drawing.Point(4, 42);
            this.gb_view.Name = "gb_view";
            this.gb_view.Size = new System.Drawing.Size(563, 150);
            this.gb_view.TabIndex = 2;
            this.gb_view.TabStop = false;
            // 
            // btn_props_xz
            // 
            this.btn_props_xz.Location = new System.Drawing.Point(500, 109);
            this.btn_props_xz.Name = "btn_props_xz";
            this.btn_props_xz.Size = new System.Drawing.Size(51, 27);
            this.btn_props_xz.TabIndex = 18;
            this.btn_props_xz.Text = "显示";
            this.btn_props_xz.UseVisualStyleBackColor = true;
            this.btn_props_xz.Click += new System.EventHandler(this.btn_props_xz_Click);
            // 
            // cbo_props_xz
            // 
            
            this.cbo_props_xz.FormattingEnabled = true;
            this.cbo_props_xz.Location = new System.Drawing.Point(278, 113);
            this.cbo_props_xz.Name = "cbo_props_xz";
            this.cbo_props_xz.PropName = "props_xz";
            this.cbo_props_xz.Size = new System.Drawing.Size(218, 21);
            this.cbo_props_xz.TabIndex = 17;
            // 
            // tb_prefix_xz
            // 
            this.tb_prefix_xz.Location = new System.Drawing.Point(164, 113);
            this.tb_prefix_xz.Name = "tb_prefix_xz";
            this.tb_prefix_xz.PropName = "prefix_xz";
            this.tb_prefix_xz.Size = new System.Drawing.Size(106, 21);
            this.tb_prefix_xz.TabIndex = 16;
            this.tb_prefix_xz.Text = "GRID";
            this.tb_prefix_xz.Trim = false;
            this.tb_prefix_xz.Value = "GRID";
            // 
            // cbo_number_xz
            // 
            
            this.cbo_number_xz.FormattingEnabled = true;
            this.cbo_number_xz.Items.AddRange(new object[] {
            "全部",
            "第一",
            "最后"});
            this.cbo_number_xz.Location = new System.Drawing.Point(74, 113);
            this.cbo_number_xz.Name = "cbo_number_xz";
            this.cbo_number_xz.PropName = "number_xz";
            this.cbo_number_xz.Size = new System.Drawing.Size(84, 21);
            this.cbo_number_xz.TabIndex = 15;
            // 
            // lb_xz
            // 
            this.lb_xz.AutoSize = true;
            this.lb_xz.Location = new System.Drawing.Point(26, 116);
            this.lb_xz.Name = "lb_xz";
            this.lb_xz.Size = new System.Drawing.Size(17, 12);
            this.lb_xz.TabIndex = 14;
            this.lb_xz.Text = "XZ";
            // 
            // btn_props_zy
            // 
            this.btn_props_zy.Location = new System.Drawing.Point(500, 73);
            this.btn_props_zy.Name = "btn_props_zy";
            this.btn_props_zy.Size = new System.Drawing.Size(51, 27);
            this.btn_props_zy.TabIndex = 13;
            this.btn_props_zy.Text = "显示";
            this.btn_props_zy.UseVisualStyleBackColor = true;
            this.btn_props_zy.Click += new System.EventHandler(this.btn_props_zy_Click);
            // 
            // cbo_props_zy
            // 
            
            this.cbo_props_zy.FormattingEnabled = true;
            this.cbo_props_zy.Location = new System.Drawing.Point(278, 77);
            this.cbo_props_zy.Name = "cbo_props_zy";
            this.cbo_props_zy.PropName = "props_zy";
            this.cbo_props_zy.Size = new System.Drawing.Size(218, 21);
            this.cbo_props_zy.TabIndex = 12;
            // 
            // tb_prefix_zy
            // 
            this.tb_prefix_zy.Location = new System.Drawing.Point(164, 77);
            this.tb_prefix_zy.Name = "tb_prefix_zy";
            this.tb_prefix_zy.PropName = "prefix_zy";
            this.tb_prefix_zy.Size = new System.Drawing.Size(106, 21);
            this.tb_prefix_zy.TabIndex = 11;
            this.tb_prefix_zy.Text = "GRID";
            this.tb_prefix_zy.Trim = false;
            this.tb_prefix_zy.Value = "GRID";
            // 
            // cbo_number_zy
            // 
            
            this.cbo_number_zy.FormattingEnabled = true;
            this.cbo_number_zy.Items.AddRange(new object[] {
            "全部",
            "第一",
            "最后"});
            this.cbo_number_zy.Location = new System.Drawing.Point(74, 77);
            this.cbo_number_zy.Name = "cbo_number_zy";
            this.cbo_number_zy.PropName = "number_zy";
            this.cbo_number_zy.Size = new System.Drawing.Size(84, 21);
            this.cbo_number_zy.TabIndex = 10;
            // 
            // lb_zy
            // 
            this.lb_zy.AutoSize = true;
            this.lb_zy.Location = new System.Drawing.Point(26, 80);
            this.lb_zy.Name = "lb_zy";
            this.lb_zy.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.lb_zy.Size = new System.Drawing.Size(17, 12);
            this.lb_zy.TabIndex = 9;
            this.lb_zy.Text = "ZY";
            // 
            // btn_props_xy
            // 
            this.btn_props_xy.Location = new System.Drawing.Point(500, 37);
            this.btn_props_xy.Name = "btn_props_xy";
            this.btn_props_xy.Size = new System.Drawing.Size(51, 27);
            this.btn_props_xy.TabIndex = 8;
            this.btn_props_xy.Text = "显示";
            this.btn_props_xy.UseVisualStyleBackColor = true;
            this.btn_props_xy.Click += new System.EventHandler(this.btn_props_xy_Click);
            // 
            // cbo_props_xy
            // 
            
            this.cbo_props_xy.FormattingEnabled = true;
            this.cbo_props_xy.Location = new System.Drawing.Point(278, 41);
            this.cbo_props_xy.Name = "cbo_props_xy";
            this.cbo_props_xy.PropName = "props_xy";
            this.cbo_props_xy.Size = new System.Drawing.Size(218, 21);
            this.cbo_props_xy.TabIndex = 7;
            // 
            // tb_prefix_xy
            // 
            this.tb_prefix_xy.Location = new System.Drawing.Point(164, 41);
            this.tb_prefix_xy.Name = "tb_prefix_xy";
            this.tb_prefix_xy.PropName = "prefix_xy";
            this.tb_prefix_xy.Size = new System.Drawing.Size(106, 21);
            this.tb_prefix_xy.TabIndex = 6;
            this.tb_prefix_xy.Text = "PLAN";
            this.tb_prefix_xy.Trim = false;
            this.tb_prefix_xy.Value = "PLAN";
            // 
            // cbo_number_xy
            // 
            
            this.cbo_number_xy.FormattingEnabled = true;
            this.cbo_number_xy.Items.AddRange(new object[] {
            "全部",
            "第一",
            "最后"});
            this.cbo_number_xy.Location = new System.Drawing.Point(74, 41);
            this.cbo_number_xy.Name = "cbo_number_xy";
            this.cbo_number_xy.PropName = "number_xy";
            this.cbo_number_xy.Size = new System.Drawing.Size(84, 21);
            this.cbo_number_xy.TabIndex = 5;
            // 
            // lb_xy
            // 
            this.lb_xy.AutoSize = true;
            this.lb_xy.Location = new System.Drawing.Point(26, 44);
            this.lb_xy.Name = "lb_xy";
            this.lb_xy.Size = new System.Drawing.Size(17, 12);
            this.lb_xy.TabIndex = 4;
            this.lb_xy.Text = "XY";
            // 
            // lb_props
            // 
            this.lb_props.AutoSize = true;
            this.lb_props.Location = new System.Drawing.Point(358, 17);
            this.lb_props.Name = "lb_props";
            this.lb_props.Size = new System.Drawing.Size(53, 12);
            this.lb_props.TabIndex = 3;
            this.lb_props.Text = "视图属性";
            // 
            // lb_prefix
            // 
            this.lb_prefix.AutoSize = true;
            this.lb_prefix.Location = new System.Drawing.Point(178, 17);
            this.lb_prefix.Name = "lb_prefix";
            this.lb_prefix.Size = new System.Drawing.Size(77, 12);
            this.lb_prefix.TabIndex = 2;
            this.lb_prefix.Text = "视图名称前缀";
            // 
            // lb_number
            // 
            this.lb_number.AutoSize = true;
            this.lb_number.Location = new System.Drawing.Point(89, 17);
            this.lb_number.Name = "lb_number";
            this.lb_number.Size = new System.Drawing.Size(53, 12);
            this.lb_number.TabIndex = 1;
            this.lb_number.Text = "视图编号";
            // 
            // lb_plane
            // 
            this.lb_plane.AutoSize = true;
            this.lb_plane.Location = new System.Drawing.Point(9, 17);
            this.lb_plane.Name = "lb_plane";
            this.lb_plane.Size = new System.Drawing.Size(53, 12);
            this.lb_plane.TabIndex = 0;
            this.lb_plane.Text = "视图平面";
            // 
            // btn_create
            // 
            this.btn_create.Location = new System.Drawing.Point(401, 198);
            this.btn_create.Name = "btn_create";
            this.btn_create.Size = new System.Drawing.Size(80, 28);
            this.btn_create.TabIndex = 7;
            this.btn_create.Text = "创 建";
            this.btn_create.UseVisualStyleBackColor = true;
            this.btn_create.Click += new System.EventHandler(this.btn_create_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Location = new System.Drawing.Point(487, 198);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 8;
            this.btn_cancel.Text = "取消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // bar_cfg
            // 
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(4, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(556, 33);
            this.bar_cfg.TabIndex = 552;
            // 
            // Form_CreateViewByGrid
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(573, 232);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_create);
            this.Controls.Add(this.gb_view);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_CreateViewByGrid";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 沿轴线生成视图";
            this.gb_view.ResumeLayout(false);
            this.gb_view.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.GroupBox gb_view;
        private System.Windows.Forms.Label lb_plane;
        private System.Windows.Forms.Button btn_props_xz;
        private UserCtrl.InputText tb_prefix_xz;
        private UserCtrl.ComboBoxKey cbo_number_xz;
        private System.Windows.Forms.Label lb_xz;
        private System.Windows.Forms.Button btn_props_zy;
        private UserCtrl.InputText tb_prefix_zy;
        private UserCtrl.ComboBoxKey cbo_number_zy;
        private System.Windows.Forms.Label lb_zy;
        private System.Windows.Forms.Button btn_props_xy;
        private UserCtrl.InputText tb_prefix_xy;
        private UserCtrl.ComboBoxKey cbo_number_xy;
        private System.Windows.Forms.Label lb_xy;
        private System.Windows.Forms.Label lb_props;
        private System.Windows.Forms.Label lb_prefix;
        private System.Windows.Forms.Label lb_number;
        private System.Windows.Forms.Button btn_create;
        private System.Windows.Forms.Button btn_cancel;
        private UserCtrl.ComboBoxText cbo_props_xz;
        private UserCtrl.ComboBoxText cbo_props_zy;
        private UserCtrl.ComboBoxText cbo_props_xy;
        private CommonCfgBar bar_cfg;
    }
}