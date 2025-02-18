namespace UI
{
    partial class Form_Settings
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
            this.tp_common = new System.Windows.Forms.TabPage();
            this.gb_selector = new System.Windows.Forms.GroupBox();
            this.tb_snap_range_factor = new UserCtrl.InputDouble();
            this.label5 = new System.Windows.Forms.Label();
            this.gb_autosave = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tb_autosave_interval = new UserCtrl.InputInt();
            this.tp_render = new System.Windows.Forms.TabPage();
            this.lb_disp_ratio = new System.Windows.Forms.Label();
            this.cbo_disp_ratio = new UserCtrl.ComboBoxKey();
            this.chb_dr_aa = new UserCtrl.Check();
            this.chb_view_aa = new UserCtrl.Check();
            this.chb_vsync = new UserCtrl.Check();
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.tc_props.SuspendLayout();
            this.tp_common.SuspendLayout();
            this.gb_selector.SuspendLayout();
            this.gb_autosave.SuspendLayout();
            this.tp_render.SuspendLayout();
            this.SuspendLayout();
            // 
            // tc_props
            // 
            this.tc_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_props.Controls.Add(this.tp_common);
            this.tc_props.Controls.Add(this.tp_render);
            this.tc_props.Location = new System.Drawing.Point(6, 42);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(394, 309);
            this.tc_props.TabIndex = 0;
            // 
            // tp_common
            // 
            this.tp_common.BackColor = System.Drawing.SystemColors.Window;
            this.tp_common.Controls.Add(this.gb_selector);
            this.tp_common.Controls.Add(this.gb_autosave);
            this.tp_common.Location = new System.Drawing.Point(4, 22);
            this.tp_common.Name = "tp_common";
            this.tp_common.Padding = new System.Windows.Forms.Padding(3);
            this.tp_common.Size = new System.Drawing.Size(386, 283);
            this.tp_common.TabIndex = 0;
            this.tp_common.Text = "通用";
            // 
            // gb_selector
            // 
            this.gb_selector.Controls.Add(this.tb_snap_range_factor);
            this.gb_selector.Controls.Add(this.label5);
            this.gb_selector.Location = new System.Drawing.Point(7, 98);
            this.gb_selector.Name = "gb_selector";
            this.gb_selector.Size = new System.Drawing.Size(373, 100);
            this.gb_selector.TabIndex = 6;
            this.gb_selector.TabStop = false;
            this.gb_selector.Text = "选择";
            // 
            // tb_snap_range_factor
            // 
            this.tb_snap_range_factor.DefValue = 0D;
            this.tb_snap_range_factor.Enob = 5;
            this.tb_snap_range_factor.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_snap_range_factor.Location = new System.Drawing.Point(116, 26);
            this.tb_snap_range_factor.Name = "tb_snap_range_factor";
            this.tb_snap_range_factor.PropName = "snap_range_factor";
            this.tb_snap_range_factor.Size = new System.Drawing.Size(113, 21);
            this.tb_snap_range_factor.TabIndex = 112;
            this.tb_snap_range_factor.Text = "0";
            this.tb_snap_range_factor.unitEnobType = "model_dist";
            this.tb_snap_range_factor.Value = 0D;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(9, 29);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(101, 12);
            this.label5.TabIndex = 111;
            this.label5.Text = "模型捕捉范围系数";
            // 
            // gb_autosave
            // 
            this.gb_autosave.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_autosave.Controls.Add(this.label2);
            this.gb_autosave.Controls.Add(this.label1);
            this.gb_autosave.Controls.Add(this.tb_autosave_interval);
            this.gb_autosave.Location = new System.Drawing.Point(6, 6);
            this.gb_autosave.Name = "gb_autosave";
            this.gb_autosave.Size = new System.Drawing.Size(374, 85);
            this.gb_autosave.TabIndex = 5;
            this.gb_autosave.TabStop = false;
            this.gb_autosave.Text = " 自动保存";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(141, 23);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 111;
            this.label2.Text = "分钟";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 111;
            this.label1.Text = "每隔";
            // 
            // tb_autosave_interval
            // 
            this.tb_autosave_interval.Location = new System.Drawing.Point(61, 20);
            this.tb_autosave_interval.Min = 0;
            this.tb_autosave_interval.Name = "tb_autosave_interval";
            this.tb_autosave_interval.PropName = "autosave_interval";
            this.tb_autosave_interval.Size = new System.Drawing.Size(74, 21);
            this.tb_autosave_interval.TabIndex = 110;
            this.tb_autosave_interval.Text = "0";
            // 
            // tp_render
            // 
            this.tp_render.Controls.Add(this.lb_disp_ratio);
            this.tp_render.Controls.Add(this.cbo_disp_ratio);
            this.tp_render.Controls.Add(this.chb_dr_aa);
            this.tp_render.Controls.Add(this.chb_view_aa);
            this.tp_render.Controls.Add(this.chb_vsync);
            this.tp_render.Location = new System.Drawing.Point(4, 22);
            this.tp_render.Name = "tp_render";
            this.tp_render.Size = new System.Drawing.Size(386, 283);
            this.tp_render.TabIndex = 1;
            this.tp_render.Text = "绘制";
            this.tp_render.UseVisualStyleBackColor = true;
            // 
            // lb_disp_ratio
            // 
            this.lb_disp_ratio.AutoSize = true;
            this.lb_disp_ratio.Location = new System.Drawing.Point(205, 20);
            this.lb_disp_ratio.Name = "lb_disp_ratio";
            this.lb_disp_ratio.Size = new System.Drawing.Size(29, 12);
            this.lb_disp_ratio.TabIndex = 52;
            this.lb_disp_ratio.Text = "缩放";
            // 
            // cbo_disp_ratio
            // 
            this.cbo_disp_ratio.FormattingEnabled = true;
            this.cbo_disp_ratio.Location = new System.Drawing.Point(240, 16);
            this.cbo_disp_ratio.Name = "cbo_disp_ratio";
            this.cbo_disp_ratio.PropName = "disp_ratio";
            this.cbo_disp_ratio.Size = new System.Drawing.Size(106, 21);
            this.cbo_disp_ratio.TabIndex = 51;
            // 
            // chb_dr_aa
            // 
            this.chb_dr_aa.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.chb_dr_aa.Location = new System.Drawing.Point(18, 45);
            this.chb_dr_aa.Name = "chb_dr_aa";
            this.chb_dr_aa.PropName = "dr_aa";
            this.chb_dr_aa.Size = new System.Drawing.Size(93, 24);
            this.chb_dr_aa.TabIndex = 0;
            this.chb_dr_aa.TabStop = false;
            this.chb_dr_aa.Text = "图纸抗锯齿";
            this.chb_dr_aa.UseVisualStyleBackColor = true;
            // 
            // chb_view_aa
            // 
            this.chb_view_aa.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.chb_view_aa.Location = new System.Drawing.Point(18, 15);
            this.chb_view_aa.Name = "chb_view_aa";
            this.chb_view_aa.PropName = "view_aa";
            this.chb_view_aa.Size = new System.Drawing.Size(93, 24);
            this.chb_view_aa.TabIndex = 0;
            this.chb_view_aa.TabStop = false;
            this.chb_view_aa.Text = "视图抗锯齿";
            this.chb_view_aa.UseVisualStyleBackColor = true;
            // 
            // chb_vsync
            // 
            this.chb_vsync.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.chb_vsync.Location = new System.Drawing.Point(32, 75);
            this.chb_vsync.Name = "chb_vsync";
            this.chb_vsync.PropName = "vsync";
            this.chb_vsync.Size = new System.Drawing.Size(79, 24);
            this.chb_vsync.TabIndex = 0;
            this.chb_vsync.TabStop = false;
            this.chb_vsync.Text = "垂直同步";
            this.chb_vsync.UseVisualStyleBackColor = true;
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(148, 357);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 500;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(320, 357);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 550;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(234, 357);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(80, 28);
            this.btn_apply.TabIndex = 510;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // bar_cfg
            // 
            this.bar_cfg.AutoSize = true;
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(6, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(390, 33);
            this.bar_cfg.TabIndex = 552;
            // 
            // Form_Settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(409, 393);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.tc_props);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Settings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 系统设置";
            this.tc_props.ResumeLayout(false);
            this.tp_common.ResumeLayout(false);
            this.gb_selector.ResumeLayout(false);
            this.gb_selector.PerformLayout();
            this.gb_autosave.ResumeLayout(false);
            this.gb_autosave.PerformLayout();
            this.tp_render.ResumeLayout(false);
            this.tp_render.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.TabPage tp_common;
        private System.Windows.Forms.GroupBox gb_autosave;
        private UserCtrl.InputInt tb_autosave_interval;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private CommonCfgBar bar_cfg;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabPage tp_render;
        private UserCtrl.Check chb_vsync;
        private UserCtrl.Check chb_dr_aa;
        private UserCtrl.Check chb_view_aa;
        private System.Windows.Forms.Label lb_disp_ratio;
        private UserCtrl.ComboBoxKey cbo_disp_ratio;
        private System.Windows.Forms.GroupBox gb_selector;
        private System.Windows.Forms.Label label5;
        private UserCtrl.InputDouble tb_snap_range_factor;
    }
}