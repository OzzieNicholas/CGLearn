namespace UI
{
    partial class Form_Move_ByRotate
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
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_move = new System.Windows.Forms.Button();
            this.tb_x0 = new UserCtrl.InputDouble();
            this.gb_origin = new System.Windows.Forms.GroupBox();
            this.lb_z0 = new System.Windows.Forms.Label();
            this.tb_z0 = new UserCtrl.InputDouble();
            this.lb_y0 = new System.Windows.Forms.Label();
            this.tb_y0 = new UserCtrl.InputDouble();
            this.lb_x0 = new System.Windows.Forms.Label();
            this.gb_props = new System.Windows.Forms.GroupBox();
            this.cbo_axis = new UserCtrl.ComboBoxKey();
            this.lb_axis = new System.Windows.Forms.Label();
            this.lb_angl = new System.Windows.Forms.Label();
            this.tb_angl = new UserCtrl.InputDouble();
            this.btn_reset = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.gb_origin.SuspendLayout();
            this.gb_props.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(60, 161);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 9;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(252, 161);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_move
            // 
            this.btn_move.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_move.Location = new System.Drawing.Point(124, 161);
            this.btn_move.Name = "btn_move";
            this.btn_move.Size = new System.Drawing.Size(58, 28);
            this.btn_move.TabIndex = 13;
            this.btn_move.TabStop = false;
            this.btn_move.Text = "移 动";
            this.btn_move.UseVisualStyleBackColor = true;
            this.btn_move.Click += new System.EventHandler(this.btn_move_Click);
            // 
            // tb_x0
            // 
            this.tb_x0.CheckName = null;
            this.tb_x0.DefValue = 0D;
            this.tb_x0.Enob = 3;
            this.tb_x0.Location = new System.Drawing.Point(26, 17);
            this.tb_x0.Name = "tb_x0";
            this.tb_x0.PropName = "x0";
            this.tb_x0.Size = new System.Drawing.Size(84, 21);
            this.tb_x0.TabIndex = 18;
            this.tb_x0.Text = "0";
            this.tb_x0.unitEnobType = "model_dist";
            this.tb_x0.Value = 0D;
            // 
            // gb_origin
            // 
            this.gb_origin.Controls.Add(this.btn_pick);
            this.gb_origin.Controls.Add(this.lb_z0);
            this.gb_origin.Controls.Add(this.tb_z0);
            this.gb_origin.Controls.Add(this.lb_y0);
            this.gb_origin.Controls.Add(this.tb_y0);
            this.gb_origin.Controls.Add(this.lb_x0);
            this.gb_origin.Controls.Add(this.tb_x0);
            this.gb_origin.Location = new System.Drawing.Point(13, 12);
            this.gb_origin.Name = "gb_origin";
            this.gb_origin.Size = new System.Drawing.Size(121, 137);
            this.gb_origin.TabIndex = 19;
            this.gb_origin.TabStop = false;
            this.gb_origin.Text = "原点";
            // 
            // lb_z0
            // 
            this.lb_z0.AutoSize = true;
            this.lb_z0.Location = new System.Drawing.Point(7, 74);
            this.lb_z0.Name = "lb_z0";
            this.lb_z0.Size = new System.Drawing.Size(11, 12);
            this.lb_z0.TabIndex = 23;
            this.lb_z0.Text = "Z";
            // 
            // tb_z0
            // 
            this.tb_z0.CheckName = null;
            this.tb_z0.DefValue = 0D;
            this.tb_z0.Enob = 3;
            this.tb_z0.Location = new System.Drawing.Point(26, 71);
            this.tb_z0.Name = "tb_z0";
            this.tb_z0.PropName = "z0";
            this.tb_z0.Size = new System.Drawing.Size(84, 21);
            this.tb_z0.TabIndex = 22;
            this.tb_z0.Text = "0";
            this.tb_z0.unitEnobType = "model_dist";
            this.tb_z0.Value = 0D;
            // 
            // lb_y0
            // 
            this.lb_y0.AutoSize = true;
            this.lb_y0.Location = new System.Drawing.Point(7, 47);
            this.lb_y0.Name = "lb_y0";
            this.lb_y0.Size = new System.Drawing.Size(11, 12);
            this.lb_y0.TabIndex = 21;
            this.lb_y0.Text = "Y";
            // 
            // tb_y0
            // 
            this.tb_y0.CheckName = null;
            this.tb_y0.DefValue = 0D;
            this.tb_y0.Enob = 3;
            this.tb_y0.Location = new System.Drawing.Point(26, 44);
            this.tb_y0.Name = "tb_y0";
            this.tb_y0.PropName = "y0";
            this.tb_y0.Size = new System.Drawing.Size(84, 21);
            this.tb_y0.TabIndex = 20;
            this.tb_y0.Text = "0";
            this.tb_y0.unitEnobType = "model_dist";
            this.tb_y0.Value = 0D;
            // 
            // lb_x0
            // 
            this.lb_x0.AutoSize = true;
            this.lb_x0.Location = new System.Drawing.Point(7, 20);
            this.lb_x0.Name = "lb_x0";
            this.lb_x0.Size = new System.Drawing.Size(11, 12);
            this.lb_x0.TabIndex = 19;
            this.lb_x0.Text = "X";
            // 
            // gb_props
            // 
            this.gb_props.Controls.Add(this.cbo_axis);
            this.gb_props.Controls.Add(this.lb_axis);
            this.gb_props.Controls.Add(this.lb_angl);
            this.gb_props.Controls.Add(this.tb_angl);
            this.gb_props.Location = new System.Drawing.Point(140, 12);
            this.gb_props.Name = "gb_props";
            this.gb_props.Size = new System.Drawing.Size(170, 137);
            this.gb_props.TabIndex = 20;
            this.gb_props.TabStop = false;
            // 
            // cbo_axis
            // 
            this.cbo_axis.CheckName = null;
            this.cbo_axis.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_axis.FormattingEnabled = true;
            this.cbo_axis.Location = new System.Drawing.Point(74, 44);
            this.cbo_axis.Name = "cbo_axis";
            this.cbo_axis.PropName = "axis";
            this.cbo_axis.Size = new System.Drawing.Size(88, 21);
            this.cbo_axis.TabIndex = 25;
            // 
            // lb_axis
            // 
            this.lb_axis.AutoSize = true;
            this.lb_axis.Location = new System.Drawing.Point(23, 47);
            this.lb_axis.Name = "lb_axis";
            this.lb_axis.Size = new System.Drawing.Size(41, 12);
            this.lb_axis.TabIndex = 24;
            this.lb_axis.Text = "旋转轴";
            // 
            // lb_angl
            // 
            this.lb_angl.AutoSize = true;
            this.lb_angl.Location = new System.Drawing.Point(14, 20);
            this.lb_angl.Name = "lb_angl";
            this.lb_angl.Size = new System.Drawing.Size(53, 12);
            this.lb_angl.TabIndex = 23;
            this.lb_angl.Text = "旋转角度";
            // 
            // tb_angl
            // 
            this.tb_angl.CheckName = null;
            this.tb_angl.DefValue = 0D;
            this.tb_angl.Enob = 3;
            this.tb_angl.Location = new System.Drawing.Point(74, 17);
            this.tb_angl.Name = "tb_angl";
            this.tb_angl.PropName = "angl";
            this.tb_angl.Size = new System.Drawing.Size(88, 21);
            this.tb_angl.TabIndex = 22;
            this.tb_angl.Text = "0";
            this.tb_angl.unitData = "deg";
            this.tb_angl.unitDisp = "deg";
            this.tb_angl.unitEnobType = "model_angl";
            this.tb_angl.Value = 0D;
            // 
            // btn_reset
            // 
            this.btn_reset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_reset.Location = new System.Drawing.Point(188, 161);
            this.btn_reset.Name = "btn_reset";
            this.btn_reset.Size = new System.Drawing.Size(58, 28);
            this.btn_reset.TabIndex = 21;
            this.btn_reset.TabStop = false;
            this.btn_reset.Text = "重 置";
            this.btn_reset.UseVisualStyleBackColor = true;
            this.btn_reset.Click += new System.EventHandler(this.btn_reset_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Location = new System.Drawing.Point(26, 98);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(84, 28);
            this.btn_pick.TabIndex = 26;
            this.btn_pick.Text = "拾 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // Form_Move_ByRotate
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(321, 196);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.gb_props);
            this.Controls.Add(this.gb_origin);
            this.Controls.Add(this.btn_move);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Move_ByRotate";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 旋转移动";
            this.gb_origin.ResumeLayout(false);
            this.gb_origin.PerformLayout();
            this.gb_props.ResumeLayout(false);
            this.gb_props.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_move;
        private UserCtrl.InputDouble tb_x0;
        private System.Windows.Forms.GroupBox gb_origin;
        private System.Windows.Forms.Label lb_z0;
        private UserCtrl.InputDouble tb_z0;
        private System.Windows.Forms.Label lb_y0;
        private UserCtrl.InputDouble tb_y0;
        private System.Windows.Forms.Label lb_x0;
        private System.Windows.Forms.GroupBox gb_props;
        private System.Windows.Forms.Label lb_axis;
        private System.Windows.Forms.Label lb_angl;
        private UserCtrl.InputDouble tb_angl;
        private UserCtrl.ComboBoxKey cbo_axis;
        private System.Windows.Forms.Button btn_reset;
        private System.Windows.Forms.Button btn_pick;
    }
}