namespace UI
{
    partial class Form_OpenProject
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
            this.btn_open = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_base_path = new System.Windows.Forms.Label();
            this.cbo_base_dir = new UI.ProjectBaseDirCombo();
            this.btn_base_dir = new System.Windows.Forms.Button();
            this.pan_proj_list = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // btn_open
            // 
            this.btn_open.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_open.Location = new System.Drawing.Point(342, 387);
            this.btn_open.Name = "btn_open";
            this.btn_open.Size = new System.Drawing.Size(80, 28);
            this.btn_open.TabIndex = 7;
            this.btn_open.Text = "打 开";
            this.btn_open.UseVisualStyleBackColor = true;
            this.btn_open.Click += new System.EventHandler(this.Btn_open_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(429, 387);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 8;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_base_path
            // 
            this.lb_base_path.AutoSize = true;
            this.lb_base_path.Location = new System.Drawing.Point(14, 13);
            this.lb_base_path.Name = "lb_base_path";
            this.lb_base_path.Size = new System.Drawing.Size(53, 12);
            this.lb_base_path.TabIndex = 10;
            this.lb_base_path.Text = "搜索目录";
            // 
            // cbo_base_dir
            // 
            this.cbo_base_dir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbo_base_dir.dir = "";
            this.cbo_base_dir.DrawMode = System.Windows.Forms.DrawMode.Normal;
            this.cbo_base_dir.DropDownHeight = 350;
            this.cbo_base_dir.IntegralHeight = false;
            this.cbo_base_dir.Location = new System.Drawing.Point(77, 10);
            this.cbo_base_dir.Name = "cbo_base_dir";
            this.cbo_base_dir.Size = new System.Drawing.Size(358, 21);
            this.cbo_base_dir.TabIndex = 11;
            // 
            // btn_base_dir
            // 
            this.btn_base_dir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_base_dir.Location = new System.Drawing.Point(442, 8);
            this.btn_base_dir.Name = "btn_base_dir";
            this.btn_base_dir.Size = new System.Drawing.Size(67, 25);
            this.btn_base_dir.TabIndex = 12;
            this.btn_base_dir.Text = "浏 览";
            this.btn_base_dir.UseVisualStyleBackColor = true;
            this.btn_base_dir.Click += new System.EventHandler(this.Btn_base_dir_Click);
            // 
            // pan_proj_list
            // 
            this.pan_proj_list.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_proj_list.Location = new System.Drawing.Point(14, 43);
            this.pan_proj_list.Name = "pan_proj_list";
            this.pan_proj_list.Size = new System.Drawing.Size(496, 338);
            this.pan_proj_list.TabIndex = 13;
            // 
            // Form_OpenProject
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(522, 427);
            this.Controls.Add(this.pan_proj_list);
            this.Controls.Add(this.btn_base_dir);
            this.Controls.Add(this.cbo_base_dir);
            this.Controls.Add(this.lb_base_path);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_open);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(532, 249);
            this.Name = "Form_OpenProject";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 打开项目";
            this.Controls.SetChildIndex(this.btn_open, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.Controls.SetChildIndex(this.lb_base_path, 0);
            this.Controls.SetChildIndex(this.cbo_base_dir, 0);
            this.Controls.SetChildIndex(this.btn_base_dir, 0);
            this.Controls.SetChildIndex(this.pan_proj_list, 0);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_open;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_base_path;
        private ProjectBaseDirCombo cbo_base_dir;
        private System.Windows.Forms.Button btn_base_dir;
        private System.Windows.Forms.Panel pan_proj_list;
    }
}