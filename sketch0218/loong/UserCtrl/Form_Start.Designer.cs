
namespace UserCtrl
{
    partial class Form_Start
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_Start));
            this.pic_back = new System.Windows.Forms.PictureBox();
            this.pan_progress = new System.Windows.Forms.Panel();
            this.lb_tips = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pic_back)).BeginInit();
            this.SuspendLayout();
            // 
            // pic_back
            // 
            this.pic_back.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pic_back.Location = new System.Drawing.Point(0, 0);
            this.pic_back.Name = "pic_back";
            this.pic_back.Size = new System.Drawing.Size(600, 340);
            this.pic_back.TabIndex = 0;
            this.pic_back.TabStop = false;
            // 
            // pan_progress
            // 
            this.pan_progress.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_progress.BackColor = System.Drawing.Color.ForestGreen;
            this.pan_progress.ForeColor = System.Drawing.SystemColors.ControlText;
            this.pan_progress.Location = new System.Drawing.Point(0, 328);
            this.pan_progress.Name = "pan_progress";
            this.pan_progress.Size = new System.Drawing.Size(600, 12);
            this.pan_progress.TabIndex = 1;
            // 
            // lb_tips
            // 
            this.lb_tips.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lb_tips.AutoSize = true;
            this.lb_tips.BackColor = System.Drawing.Color.White;
            this.lb_tips.Location = new System.Drawing.Point(12, 310);
            this.lb_tips.Name = "lb_tips";
            this.lb_tips.Size = new System.Drawing.Size(29, 12);
            this.lb_tips.TabIndex = 2;
            this.lb_tips.Text = "Tips";
            // 
            // Form_Start
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(600, 340);
            this.ControlBox = false;
            this.Controls.Add(this.lb_tips);
            this.Controls.Add(this.pan_progress);
            this.Controls.Add(this.pic_back);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form_Start";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form_Start";
            ((System.ComponentModel.ISupportInitialize)(this.pic_back)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pic_back;
        private System.Windows.Forms.Panel pan_progress;
        private System.Windows.Forms.Label lb_tips;
    }
}