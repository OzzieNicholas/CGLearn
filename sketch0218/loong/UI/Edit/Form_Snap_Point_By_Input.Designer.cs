namespace UI
{
    partial class Form_Snap_Point_By_Input
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
            this.tb_offset = new UserCtrl.InputText();
            this.SuspendLayout();
            // 
            // tb_offset
            // 
            this.tb_offset.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_offset.CheckName = null;
            this.tb_offset.Location = new System.Drawing.Point(10, 10);
            this.tb_offset.Margin = new System.Windows.Forms.Padding(1);
            this.tb_offset.Name = "tb_offset";
            this.tb_offset.PropName = null;
            this.tb_offset.Size = new System.Drawing.Size(135, 21);
            this.tb_offset.TabIndex = 7;
            this.tb_offset.Trim = false;
            // 
            // Form_Snap_Point_By_Input
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(155, 38);
            this.Controls.Add(this.tb_offset);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Snap_Point_By_Input";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 输入数值";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private UserCtrl.InputText tb_offset;
    }
}