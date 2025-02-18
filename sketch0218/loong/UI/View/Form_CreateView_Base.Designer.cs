namespace UI
{
    partial class Form_CreateView_Base
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
            this.btn_create = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_height = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tb_height = new UserCtrl.InputDouble();
            this.cbo_work_plane = new UserCtrl.ComboBoxKey();
            this.lb_work_plane = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_create
            // 
            this.btn_create.Location = new System.Drawing.Point(48, 112);
            this.btn_create.Name = "btn_create";
            this.btn_create.Size = new System.Drawing.Size(80, 28);
            this.btn_create.TabIndex = 9;
            this.btn_create.Text = "创 建";
            this.btn_create.UseVisualStyleBackColor = true;
            this.btn_create.Click += new System.EventHandler(this.btn_create_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Location = new System.Drawing.Point(134, 112);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_height
            // 
            this.lb_height.AutoSize = true;
            this.lb_height.Location = new System.Drawing.Point(10, 56);
            this.lb_height.Name = "lb_height";
            this.lb_height.Size = new System.Drawing.Size(29, 12);
            this.lb_height.TabIndex = 13;
            this.lb_height.Text = "坐标";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tb_height);
            this.groupBox1.Controls.Add(this.cbo_work_plane);
            this.groupBox1.Controls.Add(this.lb_work_plane);
            this.groupBox1.Controls.Add(this.lb_height);
            this.groupBox1.Location = new System.Drawing.Point(13, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(202, 94);
            this.groupBox1.TabIndex = 19;
            this.groupBox1.TabStop = false;
            // 
            // tb_height
            // 
            this.tb_height.DefValue = 0D;
            this.tb_height.Location = new System.Drawing.Point(80, 53);
            this.tb_height.Name = "tb_height";
            this.tb_height.PropName = "height";
            this.tb_height.Size = new System.Drawing.Size(106, 21);
            this.tb_height.TabIndex = 21;
            this.tb_height.Text = "0";
            this.tb_height.unitEnobType = "model_dist";
            this.tb_height.Value = 0D;
            // 
            // cbo_work_plane
            // 
            
            this.cbo_work_plane.FormattingEnabled = true;
            this.cbo_work_plane.Location = new System.Drawing.Point(80, 20);
            this.cbo_work_plane.Name = "cbo_work_plane";
            this.cbo_work_plane.PropName = "work_plane";
            this.cbo_work_plane.Size = new System.Drawing.Size(106, 21);
            this.cbo_work_plane.TabIndex = 20;
            // 
            // lb_work_plane
            // 
            this.lb_work_plane.AutoSize = true;
            this.lb_work_plane.Location = new System.Drawing.Point(10, 23);
            this.lb_work_plane.Name = "lb_work_plane";
            this.lb_work_plane.Size = new System.Drawing.Size(53, 12);
            this.lb_work_plane.TabIndex = 19;
            this.lb_work_plane.Text = "工作平面";
            // 
            // Form_CreateView_Base
            // 
            this.AcceptButton = this.btn_create;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(227, 152);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_create);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_CreateView_Base";
            this.Text = " 创建基本视图";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button btn_create;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_height;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lb_work_plane;
        private UserCtrl.ComboBoxKey cbo_work_plane;
        private UserCtrl.InputDouble tb_height;
    }
}