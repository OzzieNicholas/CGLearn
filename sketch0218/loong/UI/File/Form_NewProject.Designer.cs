namespace UI
{
    partial class Form_NewProject
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
            this.lb_base_path = new System.Windows.Forms.Label();
            this.btn_base_dir = new System.Windows.Forms.Button();
            this.lb_proj_name = new System.Windows.Forms.Label();
            this.tb_proj_name = new UserCtrl.InputText();
            this.cbo_base_dir = new UI.ProjectBaseDirCombo();
            this.cbo_env = new UserCtrl.ComboBoxKey();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_new
            // 
            this.btn_new.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_new.Location = new System.Drawing.Point(250, 123);
            this.btn_new.Name = "btn_new";
            this.btn_new.Size = new System.Drawing.Size(80, 32);
            this.btn_new.TabIndex = 7;
            this.btn_new.Text = "新 建";
            this.btn_new.UseVisualStyleBackColor = true;
            this.btn_new.Click += new System.EventHandler(this.Btn_new_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(336, 123);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 32);
            this.btn_cancel.TabIndex = 8;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_base_path
            // 
            this.lb_base_path.AutoSize = true;
            this.lb_base_path.Location = new System.Drawing.Point(24, 25);
            this.lb_base_path.Name = "lb_base_path";
            this.lb_base_path.Size = new System.Drawing.Size(53, 12);
            this.lb_base_path.TabIndex = 10;
            this.lb_base_path.Text = "项目目录";
            // 
            // btn_base_dir
            // 
            this.btn_base_dir.Location = new System.Drawing.Point(388, 20);
            this.btn_base_dir.Name = "btn_base_dir";
            this.btn_base_dir.Size = new System.Drawing.Size(28, 25);
            this.btn_base_dir.TabIndex = 12;
            this.btn_base_dir.Text = "...";
            this.btn_base_dir.UseVisualStyleBackColor = true;
            this.btn_base_dir.Click += new System.EventHandler(this.Btn_base_dir_Click);
            // 
            // lb_proj_name
            // 
            this.lb_proj_name.AutoSize = true;
            this.lb_proj_name.Location = new System.Drawing.Point(24, 57);
            this.lb_proj_name.Name = "lb_proj_name";
            this.lb_proj_name.Size = new System.Drawing.Size(53, 12);
            this.lb_proj_name.TabIndex = 13;
            this.lb_proj_name.Text = "项目名称";
            // 
            // tb_proj_name
            // 
            this.tb_proj_name.FilterChars = "/\\$";
            this.tb_proj_name.Location = new System.Drawing.Point(87, 54);
            this.tb_proj_name.Name = "tb_proj_name";
            this.tb_proj_name.PropName = null;
            this.tb_proj_name.Size = new System.Drawing.Size(295, 21);
            this.tb_proj_name.TabIndex = 14;
            this.tb_proj_name.Trim = false;
            // 
            // cbo_base_dir
            // 
            this.cbo_base_dir.dir = "";
            this.cbo_base_dir.DrawMode = System.Windows.Forms.DrawMode.Normal;
            this.cbo_base_dir.DropDownHeight = 350;
            this.cbo_base_dir.IntegralHeight = false;
            this.cbo_base_dir.Location = new System.Drawing.Point(87, 22);
            this.cbo_base_dir.Name = "cbo_base_dir";
            this.cbo_base_dir.Size = new System.Drawing.Size(295, 21);
            this.cbo_base_dir.TabIndex = 11;
            // 
            // cbo_env
            // 
            this.cbo_env.FormattingEnabled = true;
            this.cbo_env.Location = new System.Drawing.Point(87, 85);
            this.cbo_env.Name = "cbo_env";
            this.cbo_env.Size = new System.Drawing.Size(295, 21);
            this.cbo_env.TabIndex = 51;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 89);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 13;
            this.label1.Text = "项目环境";
            // 
            // Form_NewProject
            // 
            this.AcceptButton = this.btn_new;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(437, 167);
            this.Controls.Add(this.cbo_env);
            this.Controls.Add(this.tb_proj_name);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lb_proj_name);
            this.Controls.Add(this.btn_base_dir);
            this.Controls.Add(this.cbo_base_dir);
            this.Controls.Add(this.lb_base_path);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_new);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_NewProject";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 新建项目";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_new;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_base_path;
        private ProjectBaseDirCombo cbo_base_dir;
        private System.Windows.Forms.Button btn_base_dir;
        private System.Windows.Forms.Label lb_proj_name;
        private UserCtrl.InputText tb_proj_name;
        private UserCtrl.ComboBoxKey cbo_env;
        private System.Windows.Forms.Label label1;
    }
}