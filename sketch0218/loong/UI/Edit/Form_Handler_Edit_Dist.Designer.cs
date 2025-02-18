namespace UI
{
    partial class Form_Handler_Edit_Dist
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
            this.tb_dist = new UserCtrl.InputDouble();
            this.chb_left = new UserCtrl.ImageCheckButton();
            this.chb_both = new UserCtrl.ImageCheckButton();
            this.chb_right = new UserCtrl.ImageCheckButton();
            this.SuspendLayout();
            // 
            // tb_dist
            // 
            this.tb_dist.CheckName = null;
            this.tb_dist.DefValue = 0D;
            this.tb_dist.Location = new System.Drawing.Point(1, 1);
            this.tb_dist.Margin = new System.Windows.Forms.Padding(1);
            this.tb_dist.Name = "tb_dist";
            this.tb_dist.PropName = null;
            this.tb_dist.Size = new System.Drawing.Size(133, 21);
            this.tb_dist.TabIndex = 7;
            this.tb_dist.Text = "0";
            this.tb_dist.unitEnobType = "model_dist";
            this.tb_dist.Value = 0D;
            // 
            // chb_left
            // 
            this.chb_left.autoChecked = false;
            this.chb_left.BackColor = System.Drawing.SystemColors.Window;
            this.chb_left.Checked = true;
            this.chb_left.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.chb_left.image = global::UI.Properties.Resources.handler_dist_dir_left;
            this.chb_left.imageChecked = global::UI.Properties.Resources.handler_dist_dir_left_enabled;
            this.chb_left.imageHover = global::UI.Properties.Resources.handler_dist_dir_left_hovered;
            this.chb_left.imageRotation = 0F;
            this.chb_left.Location = new System.Drawing.Point(15, 24);
            this.chb_left.Margin = new System.Windows.Forms.Padding(3, 3, 1, 3);
            this.chb_left.Name = "chb_left";
            this.chb_left.Size = new System.Drawing.Size(32, 32);
            this.chb_left.TabIndex = 8;
            // 
            // chb_both
            // 
            this.chb_both.autoChecked = false;
            this.chb_both.BackColor = System.Drawing.SystemColors.Window;
            this.chb_both.Checked = true;
            this.chb_both.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.chb_both.image = global::UI.Properties.Resources.handler_dist_dir_both;
            this.chb_both.imageChecked = global::UI.Properties.Resources.handler_dist_dir_both_enabled;
            this.chb_both.imageHover = global::UI.Properties.Resources.handler_dist_dir_both_hovered;
            this.chb_both.imageRotation = 0F;
            this.chb_both.Location = new System.Drawing.Point(53, 24);
            this.chb_both.Margin = new System.Windows.Forms.Padding(1, 3, 1, 3);
            this.chb_both.Name = "chb_both";
            this.chb_both.Size = new System.Drawing.Size(32, 32);
            this.chb_both.TabIndex = 8;
            // 
            // chb_right
            // 
            this.chb_right.autoChecked = false;
            this.chb_right.BackColor = System.Drawing.SystemColors.Window;
            this.chb_right.Checked = true;
            this.chb_right.ForeColor = System.Drawing.SystemColors.ActiveBorder;
            this.chb_right.image = global::UI.Properties.Resources.handler_dist_dir_right;
            this.chb_right.imageChecked = global::UI.Properties.Resources.handler_dist_dir_right_enabled;
            this.chb_right.imageHover = global::UI.Properties.Resources.handler_dist_dir_right_hovered;
            this.chb_right.imageRotation = 0F;
            this.chb_right.Location = new System.Drawing.Point(89, 24);
            this.chb_right.Margin = new System.Windows.Forms.Padding(1, 3, 3, 3);
            this.chb_right.Name = "chb_right";
            this.chb_right.Size = new System.Drawing.Size(32, 32);
            this.chb_right.TabIndex = 8;
            // 
            // Form_Handler_Edit_Dist
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.ClientSize = new System.Drawing.Size(136, 58);
            this.Controls.Add(this.chb_right);
            this.Controls.Add(this.chb_both);
            this.Controls.Add(this.chb_left);
            this.Controls.Add(this.tb_dist);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Handler_Edit_Dist";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private UserCtrl.InputDouble tb_dist;
        private UserCtrl.ImageCheckButton chb_left;
        private UserCtrl.ImageCheckButton chb_both;
        private UserCtrl.ImageCheckButton chb_right;
    }
}