namespace UI
{
    partial class Form_Pallete_New
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
            this.btn_new = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_proj_name = new System.Windows.Forms.Label();
            this.tb_name = new UserCtrl.InputText();
            this.cbo_src = new UserCtrl.ComboBoxText();
            this.lb_src = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_new
            // 
            this.btn_new.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_new.Location = new System.Drawing.Point(161, 72);
            this.btn_new.Name = "btn_new";
            this.btn_new.Size = new System.Drawing.Size(80, 28);
            this.btn_new.TabIndex = 7;
            this.btn_new.Text = "新 建";
            this.btn_new.UseVisualStyleBackColor = true;
            this.btn_new.Click += new System.EventHandler(this.Btn_new_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(247, 72);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 8;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_proj_name
            // 
            this.lb_proj_name.AutoSize = true;
            this.lb_proj_name.Location = new System.Drawing.Point(14, 15);
            this.lb_proj_name.Name = "lb_proj_name";
            this.lb_proj_name.Size = new System.Drawing.Size(29, 12);
            this.lb_proj_name.TabIndex = 13;
            this.lb_proj_name.Text = "名称";
            // 
            // tb_name
            // 
            this.tb_name.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_name.Location = new System.Drawing.Point(63, 12);
            this.tb_name.Name = "tb_name";
            this.tb_name.PropName = null;
            this.tb_name.Size = new System.Drawing.Size(264, 21);
            this.tb_name.TabIndex = 14;
            this.tb_name.Trim = false;
            // 
            // cbo_src
            // 
            this.cbo_src.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_src.FormattingEnabled = true;
            this.cbo_src.Location = new System.Drawing.Point(63, 39);
            this.cbo_src.Name = "cbo_src";
            this.cbo_src.Size = new System.Drawing.Size(264, 21);
            this.cbo_src.TabIndex = 25;
            // 
            // lb_src
            // 
            this.lb_src.AutoSize = true;
            this.lb_src.Location = new System.Drawing.Point(14, 43);
            this.lb_src.Name = "lb_src";
            this.lb_src.Size = new System.Drawing.Size(29, 12);
            this.lb_src.TabIndex = 13;
            this.lb_src.Text = "模板";
            // 
            // Form_Pallete_New
            // 
            this.AcceptButton = this.btn_new;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(340, 112);
            this.Controls.Add(this.cbo_src);
            this.Controls.Add(this.tb_name);
            this.Controls.Add(this.lb_src);
            this.Controls.Add(this.lb_proj_name);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_new);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Pallete_New";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = " 新建配色方案";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_new;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_proj_name;
        private UserCtrl.InputText tb_name;
        private UserCtrl.ComboBoxText cbo_src;
        private System.Windows.Forms.Label lb_src;
    }
}