namespace UserCtrl
{
    partial class CGrid_Ctrl_TextButton
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
            this.btn_func = new System.Windows.Forms.Button();
            this.tb_value = new UserCtrl.InputText();
            this.SuspendLayout();
            // 
            // btn_func
            // 
            this.btn_func.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_func.Location = new System.Drawing.Point(163, 1);
            this.btn_func.Name = "btn_func";
            this.btn_func.Size = new System.Drawing.Size(32, 23);
            this.btn_func.TabIndex = 14;
            this.btn_func.Text = "...";
            this.btn_func.UseVisualStyleBackColor = true;
            // 
            // tb_value
            // 
            this.tb_value.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_value.BackColor = System.Drawing.Color.White;
            this.tb_value.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tb_value.CheckName = "chb_name";
            this.tb_value.CheckOnChange = true;
            this.tb_value.FilterChars = "";
            this.tb_value.Location = new System.Drawing.Point(0, 6);
            this.tb_value.Margin = new System.Windows.Forms.Padding(0);
            this.tb_value.Name = "tb_value";
            this.tb_value.PropName = "";
            this.tb_value.Size = new System.Drawing.Size(157, 14);
            this.tb_value.TabIndex = 13;
            this.tb_value.Trim = false;
            this.tb_value.Value = "";
            // 
            // CGrid_Ctrl_TextButton
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Transparent;
            this.Controls.Add(this.btn_func);
            this.Controls.Add(this.tb_value);
            this.Name = "CGrid_Ctrl_TextButton";
            this.Size = new System.Drawing.Size(198, 24);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private InputText tb_value;
        private System.Windows.Forms.Button btn_func;
    }
}
