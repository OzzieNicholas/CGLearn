namespace UI
{
    partial class Form_Move_ByMirror
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
            this.gb_props = new System.Windows.Forms.GroupBox();
            this.lb_angl = new System.Windows.Forms.Label();
            this.tb_angl = new UserCtrl.InputDouble();
            this.lb_y0 = new System.Windows.Forms.Label();
            this.tb_y0 = new UserCtrl.InputDouble();
            this.lb_x0 = new System.Windows.Forms.Label();
            this.btn_reset = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.gb_props.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(25, 158);
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
            this.btn_cancel.Location = new System.Drawing.Point(217, 158);
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
            this.btn_move.Location = new System.Drawing.Point(89, 158);
            this.btn_move.Name = "btn_move";
            this.btn_move.Size = new System.Drawing.Size(58, 28);
            this.btn_move.TabIndex = 13;
            this.btn_move.TabStop = false;
            this.btn_move.Text = "镜 像";
            this.btn_move.UseVisualStyleBackColor = true;
            this.btn_move.Click += new System.EventHandler(this.btn_move_Click);
            // 
            // tb_x0
            // 
            this.tb_x0.CheckName = null;
            this.tb_x0.DefValue = 0D;
            this.tb_x0.Enob = 3;
            this.tb_x0.Location = new System.Drawing.Point(46, 17);
            this.tb_x0.Name = "tb_x0";
            this.tb_x0.PropName = "x0";
            this.tb_x0.Size = new System.Drawing.Size(198, 21);
            this.tb_x0.TabIndex = 18;
            this.tb_x0.Text = "0";
            this.tb_x0.unitEnobType = "model_dist";
            this.tb_x0.Value = 0D;
            // 
            // gb_props
            // 
            this.gb_props.Controls.Add(this.btn_pick);
            this.gb_props.Controls.Add(this.lb_angl);
            this.gb_props.Controls.Add(this.tb_angl);
            this.gb_props.Controls.Add(this.lb_y0);
            this.gb_props.Controls.Add(this.tb_y0);
            this.gb_props.Controls.Add(this.lb_x0);
            this.gb_props.Controls.Add(this.tb_x0);
            this.gb_props.Location = new System.Drawing.Point(13, 12);
            this.gb_props.Name = "gb_props";
            this.gb_props.Size = new System.Drawing.Size(262, 136);
            this.gb_props.TabIndex = 19;
            this.gb_props.TabStop = false;
            // 
            // lb_angl
            // 
            this.lb_angl.AutoSize = true;
            this.lb_angl.Location = new System.Drawing.Point(7, 74);
            this.lb_angl.Name = "lb_angl";
            this.lb_angl.Size = new System.Drawing.Size(29, 12);
            this.lb_angl.TabIndex = 23;
            this.lb_angl.Text = "角度";
            // 
            // tb_angl
            // 
            this.tb_angl.CheckName = null;
            this.tb_angl.DefValue = 0D;
            this.tb_angl.Enob = 3;
            this.tb_angl.Location = new System.Drawing.Point(46, 71);
            this.tb_angl.Max = 10000D;
            this.tb_angl.Min = -10000D;
            this.tb_angl.Name = "tb_angl";
            this.tb_angl.PropName = "angl";
            this.tb_angl.Size = new System.Drawing.Size(198, 21);
            this.tb_angl.TabIndex = 22;
            this.tb_angl.Text = "0";
            this.tb_angl.unitData = "deg";
            this.tb_angl.unitDisp = "deg";
            this.tb_angl.unitEnobType = "model_angl";
            this.tb_angl.Value = 0D;
            // 
            // lb_y0
            // 
            this.lb_y0.AutoSize = true;
            this.lb_y0.Location = new System.Drawing.Point(20, 47);
            this.lb_y0.Name = "lb_y0";
            this.lb_y0.Size = new System.Drawing.Size(17, 12);
            this.lb_y0.TabIndex = 21;
            this.lb_y0.Text = "Y0";
            // 
            // tb_y0
            // 
            this.tb_y0.CheckName = null;
            this.tb_y0.DefValue = 0D;
            this.tb_y0.Enob = 3;
            this.tb_y0.Location = new System.Drawing.Point(46, 44);
            this.tb_y0.Name = "tb_y0";
            this.tb_y0.PropName = "y0";
            this.tb_y0.Size = new System.Drawing.Size(198, 21);
            this.tb_y0.TabIndex = 20;
            this.tb_y0.Text = "0";
            this.tb_y0.unitEnobType = "model_dist";
            this.tb_y0.Value = 0D;
            // 
            // lb_x0
            // 
            this.lb_x0.AutoSize = true;
            this.lb_x0.Location = new System.Drawing.Point(20, 20);
            this.lb_x0.Name = "lb_x0";
            this.lb_x0.Size = new System.Drawing.Size(17, 12);
            this.lb_x0.TabIndex = 19;
            this.lb_x0.Text = "X0";
            // 
            // btn_reset
            // 
            this.btn_reset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_reset.Location = new System.Drawing.Point(153, 158);
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
            this.btn_pick.Location = new System.Drawing.Point(149, 98);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(95, 28);
            this.btn_pick.TabIndex = 23;
            this.btn_pick.Text = "拾 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // Form_Move_ByMirror
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(287, 196);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.gb_props);
            this.Controls.Add(this.btn_move);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Move_ByMirror";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 镜像移动";
            this.gb_props.ResumeLayout(false);
            this.gb_props.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_move;
        private UserCtrl.InputDouble tb_x0;
        private System.Windows.Forms.GroupBox gb_props;
        private System.Windows.Forms.Label lb_angl;
        private UserCtrl.InputDouble tb_angl;
        private System.Windows.Forms.Label lb_y0;
        private UserCtrl.InputDouble tb_y0;
        private System.Windows.Forms.Label lb_x0;
        private System.Windows.Forms.Button btn_reset;
        private System.Windows.Forms.Button btn_pick;
    }
}