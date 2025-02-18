namespace UI
{
    partial class ProgressMsgBox
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
            this.lb_message = new System.Windows.Forms.Label();
            this.pan_progress = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // lb_message
            // 
            this.lb_message.AutoSize = true;
            this.lb_message.Location = new System.Drawing.Point(12, 12);
            this.lb_message.Name = "lb_message";
            this.lb_message.Size = new System.Drawing.Size(47, 12);
            this.lb_message.TabIndex = 8;
            this.lb_message.Text = "message";
            // 
            // pan_progress
            // 
            this.pan_progress.BackColor = System.Drawing.Color.Green;
            this.pan_progress.Location = new System.Drawing.Point(0, 44);
            this.pan_progress.Margin = new System.Windows.Forms.Padding(0);
            this.pan_progress.Name = "pan_progress";
            this.pan_progress.Size = new System.Drawing.Size(450, 12);
            this.pan_progress.TabIndex = 9;
            // 
            // ProgressMsgBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.ClientSize = new System.Drawing.Size(675, 90);
            this.ControlBox = false;
            this.Controls.Add(this.pan_progress);
            this.Controls.Add(this.lb_message);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ProgressMsgBox";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label lb_message;
        private System.Windows.Forms.Panel pan_progress;
    }
}