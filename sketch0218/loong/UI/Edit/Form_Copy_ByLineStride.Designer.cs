namespace UI
{
    partial class Form_Copy_ByLineStride
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
            this.btn_reset = new System.Windows.Forms.Button();
            this.tb_count = new UserCtrl.InputInt();
            this.lb_count = new System.Windows.Forms.Label();
            this.gb_common = new System.Windows.Forms.GroupBox();
            this.btn_pick = new System.Windows.Forms.Button();
            this.chb_dZ = new UserCtrl.Check();
            this.chb_dY = new UserCtrl.Check();
            this.chb_dX = new UserCtrl.Check();
            this.tb_dZ = new UserCtrl.InputDoubleArrayScript();
            this.tb_dY = new UserCtrl.InputDoubleArrayScript();
            this.tb_dX = new UserCtrl.InputDoubleArrayScript();
            this.btn_copy = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.gb_common.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_reset
            // 
            this.btn_reset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_reset.Location = new System.Drawing.Point(162, 157);
            this.btn_reset.Name = "btn_reset";
            this.btn_reset.Size = new System.Drawing.Size(58, 28);
            this.btn_reset.TabIndex = 27;
            this.btn_reset.TabStop = false;
            this.btn_reset.Text = "清 除";
            this.btn_reset.UseVisualStyleBackColor = true;
            this.btn_reset.Click += new System.EventHandler(this.btn_reset_Click);
            // 
            // tb_count
            // 
            this.tb_count.CheckName = null;
            this.tb_count.Location = new System.Drawing.Point(252, 19);
            this.tb_count.Name = "tb_count";
            this.tb_count.PropName = "count";
            this.tb_count.Size = new System.Drawing.Size(77, 21);
            this.tb_count.TabGroup = "mov_count";
            this.tb_count.TabIndex = 21;
            this.tb_count.Text = "0";
            this.tb_count.unitData = "";
            this.tb_count.unitDisp = "";
            // 
            // lb_count
            // 
            this.lb_count.AutoSize = true;
            this.lb_count.Location = new System.Drawing.Point(189, 23);
            this.lb_count.Name = "lb_count";
            this.lb_count.Size = new System.Drawing.Size(53, 12);
            this.lb_count.TabIndex = 19;
            this.lb_count.Text = "复制数量";
            // 
            // gb_common
            // 
            this.gb_common.Controls.Add(this.btn_pick);
            this.gb_common.Controls.Add(this.chb_dZ);
            this.gb_common.Controls.Add(this.chb_dY);
            this.gb_common.Controls.Add(this.chb_dX);
            this.gb_common.Controls.Add(this.tb_count);
            this.gb_common.Controls.Add(this.lb_count);
            this.gb_common.Controls.Add(this.tb_dZ);
            this.gb_common.Controls.Add(this.tb_dY);
            this.gb_common.Controls.Add(this.tb_dX);
            this.gb_common.Location = new System.Drawing.Point(13, 12);
            this.gb_common.Name = "gb_common";
            this.gb_common.Size = new System.Drawing.Size(335, 139);
            this.gb_common.TabIndex = 25;
            this.gb_common.TabStop = false;
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(87, 100);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(95, 28);
            this.btn_pick.TabIndex = 28;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "拾 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // chb_dZ
            // 
            this.chb_dZ.AutoSize = true;
            this.chb_dZ.Checked = true;
            this.chb_dZ.CheckName = null;
            this.chb_dZ.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dZ.Location = new System.Drawing.Point(10, 75);
            this.chb_dZ.Name = "chb_dZ";
            this.chb_dZ.PropName = null;
            this.chb_dZ.Size = new System.Drawing.Size(36, 16);
            this.chb_dZ.TabGroup = "mov_dist";
            this.chb_dZ.TabIndex = 26;
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
            this.chb_dY.Location = new System.Drawing.Point(10, 48);
            this.chb_dY.Name = "chb_dY";
            this.chb_dY.PropName = null;
            this.chb_dY.Size = new System.Drawing.Size(36, 16);
            this.chb_dY.TabGroup = "mov_dist";
            this.chb_dY.TabIndex = 25;
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
            this.chb_dX.Location = new System.Drawing.Point(10, 22);
            this.chb_dX.Name = "chb_dX";
            this.chb_dX.PropName = null;
            this.chb_dX.Size = new System.Drawing.Size(36, 16);
            this.chb_dX.TabGroup = "mov_dist";
            this.chb_dX.TabIndex = 24;
            this.chb_dX.TabStop = false;
            this.chb_dX.Text = "dX";
            this.chb_dX.UseVisualStyleBackColor = true;
            // 
            // tb_dZ
            // 
            this.tb_dZ.AutoMerge = false;
            this.tb_dZ.CheckName = "chb_dZ";
            this.tb_dZ.Enob = 3;
            this.tb_dZ.Location = new System.Drawing.Point(54, 73);
            this.tb_dZ.Name = "tb_dZ";
            this.tb_dZ.PropName = "dz";
            this.tb_dZ.Size = new System.Drawing.Size(128, 21);
            this.tb_dZ.TabGroup = "mov_dist";
            this.tb_dZ.TabIndex = 22;
            this.tb_dZ.unitEnobType = "model_dist";
            this.tb_dZ.Value = new double[0];
            // 
            // tb_dY
            // 
            this.tb_dY.AutoMerge = false;
            this.tb_dY.CheckName = "chb_dY";
            this.tb_dY.Enob = 3;
            this.tb_dY.Location = new System.Drawing.Point(54, 46);
            this.tb_dY.Name = "tb_dY";
            this.tb_dY.PropName = "dy";
            this.tb_dY.Size = new System.Drawing.Size(128, 21);
            this.tb_dY.TabGroup = "mov_dist";
            this.tb_dY.TabIndex = 20;
            this.tb_dY.unitEnobType = "model_dist";
            this.tb_dY.Value = new double[0];
            // 
            // tb_dX
            // 
            this.tb_dX.AutoMerge = false;
            this.tb_dX.CheckName = "chb_dX";
            this.tb_dX.Enob = 3;
            this.tb_dX.Location = new System.Drawing.Point(54, 19);
            this.tb_dX.Name = "tb_dX";
            this.tb_dX.PropName = "dx";
            this.tb_dX.Size = new System.Drawing.Size(128, 21);
            this.tb_dX.TabGroup = "mov_dist";
            this.tb_dX.TabIndex = 18;
            this.tb_dX.unitEnobType = "model_dist";
            this.tb_dX.Value = new double[0];
            // 
            // btn_copy
            // 
            this.btn_copy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_copy.Location = new System.Drawing.Point(98, 157);
            this.btn_copy.Name = "btn_copy";
            this.btn_copy.Size = new System.Drawing.Size(58, 28);
            this.btn_copy.TabIndex = 24;
            this.btn_copy.TabStop = false;
            this.btn_copy.Text = "复 制";
            this.btn_copy.UseVisualStyleBackColor = true;
            this.btn_copy.Click += new System.EventHandler(this.btn_copy_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(290, 157);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 23;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(34, 157);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 22;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(226, 157);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 28;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.btn_chb_Click);
            // 
            // Form_Copy_ByLineStride
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(361, 197);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.gb_common);
            this.Controls.Add(this.btn_copy);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Copy_ByLineStride";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 线性复制";
            this.gb_common.ResumeLayout(false);
            this.gb_common.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btn_reset;
        private UserCtrl.InputInt tb_count;
        private System.Windows.Forms.Label lb_count;
        private System.Windows.Forms.GroupBox gb_common;
        private UserCtrl.InputDoubleArrayScript tb_dZ;
        private UserCtrl.InputDoubleArrayScript tb_dY;
        private UserCtrl.InputDoubleArrayScript tb_dX;
        private System.Windows.Forms.Button btn_copy;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_ok;
        private UserCtrl.Check chb_dZ;
        private UserCtrl.Check chb_dY;
        private UserCtrl.Check chb_dX;
        private System.Windows.Forms.Button btn_chb;
        private System.Windows.Forms.Button btn_pick;
    }
}