namespace UI
{
    partial class Form_ShortCutConflict
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
            this.btn_overlap = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_msg = new System.Windows.Forms.Label();
            this.li_conflicts = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // btn_overlap
            // 
            this.btn_overlap.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_overlap.Location = new System.Drawing.Point(111, 149);
            this.btn_overlap.Name = "btn_overlap";
            this.btn_overlap.Size = new System.Drawing.Size(75, 28);
            this.btn_overlap.TabIndex = 7;
            this.btn_overlap.Text = "覆 盖";
            this.btn_overlap.UseVisualStyleBackColor = true;
            this.btn_overlap.Click += new System.EventHandler(this.Btn_overlap_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(192, 149);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(75, 28);
            this.btn_cancel.TabIndex = 8;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_msg
            // 
            this.lb_msg.AutoSize = true;
            this.lb_msg.Location = new System.Drawing.Point(12, 20);
            this.lb_msg.Name = "lb_msg";
            this.lb_msg.Size = new System.Drawing.Size(143, 12);
            this.lb_msg.TabIndex = 9;
            this.lb_msg.Text = "\"Ctrl+C\" 与下列功能冲突";
            // 
            // li_conflicts
            // 
            this.li_conflicts.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.li_conflicts.FormattingEnabled = true;
            this.li_conflicts.ItemHeight = 12;
            this.li_conflicts.Location = new System.Drawing.Point(14, 45);
            this.li_conflicts.Name = "li_conflicts";
            this.li_conflicts.Size = new System.Drawing.Size(253, 88);
            this.li_conflicts.TabIndex = 10;
            // 
            // Form_ShortCutConflict
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(279, 189);
            this.Controls.Add(this.li_conflicts);
            this.Controls.Add(this.lb_msg);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_overlap);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_ShortCutConflict";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "快捷键冲突";
            this.Controls.SetChildIndex(this.btn_overlap, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.Controls.SetChildIndex(this.lb_msg, 0);
            this.Controls.SetChildIndex(this.li_conflicts, 0);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_overlap;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_msg;
        private System.Windows.Forms.ListBox li_conflicts;
    }
}