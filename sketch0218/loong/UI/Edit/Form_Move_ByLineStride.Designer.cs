namespace UI
{
    partial class Form_Move_ByLineStride
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
            this.gb_origin = new System.Windows.Forms.GroupBox();
            this.chb_dZ = new UserCtrl.Check();
            this.chb_dY = new UserCtrl.Check();
            this.chb_dX = new UserCtrl.Check();
            this.btn_pick = new System.Windows.Forms.Button();
            this.tb_dz = new UserCtrl.InputDouble();
            this.tb_dy = new UserCtrl.InputDouble();
            this.tb_dx = new UserCtrl.InputDouble();
            this.btn_reset = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.gb_origin.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(22, 157);
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
            this.btn_cancel.Location = new System.Drawing.Point(278, 157);
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
            this.btn_move.Location = new System.Drawing.Point(86, 157);
            this.btn_move.Name = "btn_move";
            this.btn_move.Size = new System.Drawing.Size(58, 28);
            this.btn_move.TabIndex = 13;
            this.btn_move.TabStop = false;
            this.btn_move.Text = "移 动";
            this.btn_move.UseVisualStyleBackColor = true;
            this.btn_move.Click += new System.EventHandler(this.btn_move_Click);
            // 
            // gb_origin
            // 
            this.gb_origin.Controls.Add(this.chb_dZ);
            this.gb_origin.Controls.Add(this.chb_dY);
            this.gb_origin.Controls.Add(this.chb_dX);
            this.gb_origin.Controls.Add(this.btn_pick);
            this.gb_origin.Controls.Add(this.tb_dz);
            this.gb_origin.Controls.Add(this.tb_dy);
            this.gb_origin.Controls.Add(this.tb_dx);
            this.gb_origin.Location = new System.Drawing.Point(13, 12);
            this.gb_origin.Name = "gb_origin";
            this.gb_origin.Size = new System.Drawing.Size(322, 136);
            this.gb_origin.TabIndex = 19;
            this.gb_origin.TabStop = false;
            // 
            // chb_dZ
            // 
            this.chb_dZ.AutoSize = true;
            this.chb_dZ.Checked = true;
            this.chb_dZ.CheckName = null;
            this.chb_dZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dZ.Location = new System.Drawing.Point(6, 73);
            this.chb_dZ.Name = "chb_dZ";
            this.chb_dZ.PropName = null;
            this.chb_dZ.Size = new System.Drawing.Size(36, 16);
            this.chb_dZ.TabIndex = 29;
            this.chb_dZ.TabStop = false;
            this.chb_dZ.Text = "dZ";
            this.chb_dZ.UseVisualStyleBackColor = true;
            // 
            // chb_dY
            // 
            this.chb_dY.AutoSize = true;
            this.chb_dY.Checked = true;
            this.chb_dY.CheckName = null;
            this.chb_dY.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dY.Location = new System.Drawing.Point(6, 46);
            this.chb_dY.Name = "chb_dY";
            this.chb_dY.PropName = null;
            this.chb_dY.Size = new System.Drawing.Size(36, 16);
            this.chb_dY.TabIndex = 28;
            this.chb_dY.TabStop = false;
            this.chb_dY.Text = "dY";
            this.chb_dY.UseVisualStyleBackColor = true;
            // 
            // chb_dX
            // 
            this.chb_dX.AutoSize = true;
            this.chb_dX.Checked = true;
            this.chb_dX.CheckName = null;
            this.chb_dX.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dX.Location = new System.Drawing.Point(6, 20);
            this.chb_dX.Name = "chb_dX";
            this.chb_dX.PropName = null;
            this.chb_dX.Size = new System.Drawing.Size(36, 16);
            this.chb_dX.TabIndex = 27;
            this.chb_dX.TabStop = false;
            this.chb_dX.Text = "dX";
            this.chb_dX.UseVisualStyleBackColor = true;
            // 
            // btn_pick
            // 
            this.btn_pick.Location = new System.Drawing.Point(220, 98);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(95, 28);
            this.btn_pick.TabIndex = 22;
            this.btn_pick.Text = "拾 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // tb_dz
            // 
            this.tb_dz.CheckName = "chb_dZ";
            this.tb_dz.DefValue = 0D;
            this.tb_dz.Enob = 3;
            this.tb_dz.Location = new System.Drawing.Point(51, 71);
            this.tb_dz.Name = "tb_dz";
            this.tb_dz.PropName = "dz";
            this.tb_dz.Size = new System.Drawing.Size(264, 21);
            this.tb_dz.TabIndex = 22;
            this.tb_dz.Text = "0";
            this.tb_dz.unitEnobType = "model_dist";
            this.tb_dz.Value = 0D;
            // 
            // tb_dy
            // 
            this.tb_dy.CheckName = "chb_dY";
            this.tb_dy.DefValue = 0D;
            this.tb_dy.Enob = 3;
            this.tb_dy.Location = new System.Drawing.Point(51, 44);
            this.tb_dy.Name = "tb_dy";
            this.tb_dy.PropName = "dy";
            this.tb_dy.Size = new System.Drawing.Size(264, 21);
            this.tb_dy.TabIndex = 20;
            this.tb_dy.Text = "0";
            this.tb_dy.unitEnobType = "model_dist";
            this.tb_dy.Value = 0D;
            // 
            // tb_dx
            // 
            this.tb_dx.CheckName = "chb_dX";
            this.tb_dx.DefValue = 0D;
            this.tb_dx.Enob = 3;
            this.tb_dx.Location = new System.Drawing.Point(51, 17);
            this.tb_dx.Name = "tb_dx";
            this.tb_dx.PropName = "dx";
            this.tb_dx.Size = new System.Drawing.Size(264, 21);
            this.tb_dx.TabIndex = 18;
            this.tb_dx.Text = "0";
            this.tb_dx.unitEnobType = "model_dist";
            this.tb_dx.Value = 0D;
            // 
            // btn_reset
            // 
            this.btn_reset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_reset.Location = new System.Drawing.Point(150, 157);
            this.btn_reset.Name = "btn_reset";
            this.btn_reset.Size = new System.Drawing.Size(58, 28);
            this.btn_reset.TabIndex = 21;
            this.btn_reset.TabStop = false;
            this.btn_reset.Text = "清 除";
            this.btn_reset.UseVisualStyleBackColor = true;
            this.btn_reset.Click += new System.EventHandler(this.btn_reset_Click);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(214, 157);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 29;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.btn_chb_Click);
            // 
            // Form_Move_ByLineStride
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(348, 197);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.gb_origin);
            this.Controls.Add(this.btn_move);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Move_ByLineStride";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 线性移动";
            this.gb_origin.ResumeLayout(false);
            this.gb_origin.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_move;
        private System.Windows.Forms.GroupBox gb_origin;
        private UserCtrl.InputDouble tb_dx;
        private UserCtrl.InputDouble tb_dz;
        private UserCtrl.InputDouble tb_dy;
        private System.Windows.Forms.Button btn_reset;
        private System.Windows.Forms.Button btn_pick;
        private UserCtrl.Check chb_dZ;
        private UserCtrl.Check chb_dY;
        private UserCtrl.Check chb_dX;
        private System.Windows.Forms.Button btn_chb;
    }
}