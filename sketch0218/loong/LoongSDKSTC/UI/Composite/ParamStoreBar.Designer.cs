namespace LoongCAD.UI
{
    partial class ParamStoreBar
    {
        /// <summary> 
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.btn_new = new System.Windows.Forms.Button();
            this.cbo_cfg = new LoongCAD.UI.TextComboBox();
            this.btn_load = new System.Windows.Forms.Button();
            this.btn_save = new System.Windows.Forms.Button();
            this.tb_cfg_name = new LoongCAD.UI.TextInput();
            this.btn_delete = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btn_new
            // 
            this.btn_new.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_new.Location = new System.Drawing.Point(435, 0);
            this.btn_new.Name = "btn_new";
            this.btn_new.Size = new System.Drawing.Size(45, 30);
            this.btn_new.TabIndex = 29;
            this.btn_new.TabStop = false;
            this.btn_new.Text = "创建";
            this.btn_new.UseVisualStyleBackColor = true;
            this.btn_new.Click += new System.EventHandler(this.OnButtonClick_New);
            // 
            // cbo_cfg
            // 
            this.cbo_cfg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbo_cfg.CheckOnChange = false;
            this.cbo_cfg.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_cfg.FormattingEnabled = true;
            this.cbo_cfg.Location = new System.Drawing.Point(54, 4);
            this.cbo_cfg.Name = "cbo_cfg";
            this.cbo_cfg.Placeholder = "";
            this.cbo_cfg.Size = new System.Drawing.Size(146, 21);
            this.cbo_cfg.TabIndex = 28;
            this.cbo_cfg.TabStop = false;
            // 
            // btn_load
            // 
            this.btn_load.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_load.Location = new System.Drawing.Point(206, 0);
            this.btn_load.Name = "btn_load";
            this.btn_load.Size = new System.Drawing.Size(45, 30);
            this.btn_load.TabIndex = 27;
            this.btn_load.TabStop = false;
            this.btn_load.Text = "读取";
            this.btn_load.UseVisualStyleBackColor = true;
            this.btn_load.Click += new System.EventHandler(this.OnButtonClick_Load);
            // 
            // btn_save
            // 
            this.btn_save.Location = new System.Drawing.Point(3, 0);
            this.btn_save.Name = "btn_save";
            this.btn_save.Size = new System.Drawing.Size(45, 30);
            this.btn_save.TabIndex = 26;
            this.btn_save.TabStop = false;
            this.btn_save.Text = "保存";
            this.btn_save.UseVisualStyleBackColor = true;
            this.btn_save.Click += new System.EventHandler(this.OnButtonClick_Save);
            // 
            // tb_cfg_name
            // 
            this.tb_cfg_name.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_cfg_name.Location = new System.Drawing.Point(308, 4);
            this.tb_cfg_name.Name = "tb_cfg_name";
            this.tb_cfg_name.Placeholder = "";
            this.tb_cfg_name.Size = new System.Drawing.Size(125, 21);
            this.tb_cfg_name.TabIndex = 30;
            this.tb_cfg_name.TabStop = false;
            this.tb_cfg_name.Trim = false;
            // 
            // btn_delete
            // 
            this.btn_delete.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_delete.Location = new System.Drawing.Point(257, 0);
            this.btn_delete.Name = "btn_delete";
            this.btn_delete.Size = new System.Drawing.Size(45, 30);
            this.btn_delete.TabIndex = 27;
            this.btn_delete.TabStop = false;
            this.btn_delete.Text = "删除";
            this.btn_delete.UseVisualStyleBackColor = true;
            this.btn_delete.Click += new System.EventHandler(this.OnButtonClick_Delete);
            // 
            // ParamStoreBar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoSize = true;
            this.Controls.Add(this.btn_new);
            this.Controls.Add(this.btn_delete);
            this.Controls.Add(this.btn_load);
            this.Controls.Add(this.tb_cfg_name);
            this.Controls.Add(this.btn_save);
            this.Controls.Add(this.cbo_cfg);
            this.MinimumSize = new System.Drawing.Size(150, 0);
            this.Name = "ParamStoreBar";
            this.Size = new System.Drawing.Size(483, 33);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_new;
        private TextComboBox cbo_cfg;
        private System.Windows.Forms.Button btn_load;
        private System.Windows.Forms.Button btn_save;
        private TextInput tb_cfg_name;
        private System.Windows.Forms.Button btn_delete;
    }
}
