namespace UI
{
    partial class Form_AskOverlappedObjs
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
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_replace = new System.Windows.Forms.Button();
            this.btn_keep = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lb_message
            // 
            this.lb_message.AutoSize = true;
            this.lb_message.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lb_message.Location = new System.Drawing.Point(71, 23);
            this.lb_message.Name = "lb_message";
            this.lb_message.Size = new System.Drawing.Size(89, 12);
            this.lb_message.TabIndex = 8;
            this.lb_message.Text = "附近有相似物体";
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(154, 58);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 501;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_replace
            // 
            this.btn_replace.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_replace.Location = new System.Drawing.Point(90, 58);
            this.btn_replace.Name = "btn_replace";
            this.btn_replace.Size = new System.Drawing.Size(58, 28);
            this.btn_replace.TabIndex = 501;
            this.btn_replace.TabStop = false;
            this.btn_replace.Text = "替 换";
            this.btn_replace.UseVisualStyleBackColor = true;
            this.btn_replace.Click += new System.EventHandler(this.btn_replace_Click);
            // 
            // btn_keep
            // 
            this.btn_keep.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_keep.Location = new System.Drawing.Point(26, 58);
            this.btn_keep.Name = "btn_keep";
            this.btn_keep.Size = new System.Drawing.Size(58, 28);
            this.btn_keep.TabIndex = 501;
            this.btn_keep.TabStop = false;
            this.btn_keep.Text = "保 留";
            this.btn_keep.UseVisualStyleBackColor = true;
            this.btn_keep.Click += new System.EventHandler(this.btn_keep_Click);
            // 
            // Form_AskOverlappedObjs
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(237, 98);
            this.ControlBox = false;
            this.Controls.Add(this.btn_keep);
            this.Controls.Add(this.btn_replace);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.lb_message);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_AskOverlappedObjs";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 提示";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label lb_message;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_replace;
        private System.Windows.Forms.Button btn_keep;
    }
}