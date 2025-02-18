namespace UI
{
    partial class Form_SaveProject
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
            this.btn_save = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_base_path = new System.Windows.Forms.Label();
            this.cbo_base_dir = new UI.ProjectBaseDirCombo();
            this.btn_base_dir = new System.Windows.Forms.Button();
            this.pan_proj_list = new System.Windows.Forms.Panel();
            this.lb_proj_name = new System.Windows.Forms.Label();
            this.tb_proj_name = new UserCtrl.InputText();
            this.SuspendLayout();
            // 
            // btn_save
            // 
            this.btn_save.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_save.Location = new System.Drawing.Point(384, 346);
            this.btn_save.Name = "btn_save";
            this.btn_save.Size = new System.Drawing.Size(80, 28);
            this.btn_save.TabIndex = 7;
            this.btn_save.Text = "保 存";
            this.btn_save.UseVisualStyleBackColor = true;
            this.btn_save.Click += new System.EventHandler(this.Btn_save_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(470, 346);
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
            this.lb_base_path.Text = "项目目录";
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
            this.cbo_base_dir.Size = new System.Drawing.Size(398, 21);
            this.cbo_base_dir.TabIndex = 11;
            // 
            // btn_base_dir
            // 
            this.btn_base_dir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_base_dir.Location = new System.Drawing.Point(483, 8);
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
            this.pan_proj_list.Location = new System.Drawing.Point(14, 64);
            this.pan_proj_list.Name = "pan_proj_list";
            this.pan_proj_list.Size = new System.Drawing.Size(537, 276);
            this.pan_proj_list.TabIndex = 13;
            // 
            // lb_proj_name
            // 
            this.lb_proj_name.AutoSize = true;
            this.lb_proj_name.Location = new System.Drawing.Point(14, 40);
            this.lb_proj_name.Name = "lb_proj_name";
            this.lb_proj_name.Size = new System.Drawing.Size(53, 12);
            this.lb_proj_name.TabIndex = 14;
            this.lb_proj_name.Text = "项目名称";
            // 
            // tb_proj_name
            // 
            this.tb_proj_name.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_proj_name.Location = new System.Drawing.Point(77, 37);
            this.tb_proj_name.Name = "tb_proj_name";
            this.tb_proj_name.Size = new System.Drawing.Size(398, 21);
            this.tb_proj_name.TabIndex = 15;
            this.tb_proj_name.Trim = false;
            // 
            // Form_SaveProject
            // 
            this.AcceptButton = this.btn_save;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(563, 386);
            this.Controls.Add(this.tb_proj_name);
            this.Controls.Add(this.lb_proj_name);
            this.Controls.Add(this.pan_proj_list);
            this.Controls.Add(this.btn_base_dir);
            this.Controls.Add(this.cbo_base_dir);
            this.Controls.Add(this.lb_base_path);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_save);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_SaveProject";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 保存项目";
            this.Controls.SetChildIndex(this.btn_save, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.Controls.SetChildIndex(this.lb_base_path, 0);
            this.Controls.SetChildIndex(this.cbo_base_dir, 0);
            this.Controls.SetChildIndex(this.btn_base_dir, 0);
            this.Controls.SetChildIndex(this.pan_proj_list, 0);
            this.Controls.SetChildIndex(this.lb_proj_name, 0);
            this.Controls.SetChildIndex(this.tb_proj_name, 0);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_save;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_base_path;
        private ProjectBaseDirCombo cbo_base_dir;
        private System.Windows.Forms.Button btn_base_dir;
        private System.Windows.Forms.Panel pan_proj_list;
        private System.Windows.Forms.Label lb_proj_name;
        private UserCtrl.InputText tb_proj_name;
    }
}