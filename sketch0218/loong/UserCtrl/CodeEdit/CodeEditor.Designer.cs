namespace UserCtrl
{
    partial class CodeEditor
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
            this.tb_line = new UserCtrl.CodeLine();
            this.tb_code = new UserCtrl.CodeInput();
            this.SuspendLayout();
            // 
            // tb_line
            // 
            this.tb_line.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.tb_line.BackColor = System.Drawing.Color.Silver;
            this.tb_line.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tb_line.Enabled = false;
            this.tb_line.Font = new System.Drawing.Font("Courier New", 9F);
            this.tb_line.Location = new System.Drawing.Point(0, 0);
            this.tb_line.Margin = new System.Windows.Forms.Padding(0);
            this.tb_line.Name = "tb_line";
            this.tb_line.ReadOnly = true;
            this.tb_line.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.tb_line.scrollPosV = 0;
            this.tb_line.Size = new System.Drawing.Size(36, 489);
            this.tb_line.TabIndex = 1;
            this.tb_line.Text = "";
            this.tb_line.WordWrap = false;
            // 
            // tb_code
            // 
            this.tb_code.AcceptsTab = true;
            this.tb_code.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_code.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tb_code.Font = new System.Drawing.Font("Courier New", 9F);
            this.tb_code.isModified = false;
            this.tb_code.Location = new System.Drawing.Point(36, 0);
            this.tb_code.Margin = new System.Windows.Forms.Padding(0);
            this.tb_code.Name = "tb_code";
            this.tb_code.onLineChanged = null;
            this.tb_code.scrollPosV = 0;
            this.tb_code.Size = new System.Drawing.Size(605, 489);
            this.tb_code.TabIndex = 0;
            this.tb_code.text = "";
            this.tb_code.Text = "";
            // 
            // CodeEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.tb_line);
            this.Controls.Add(this.tb_code);
            this.Name = "CodeEditor";
            this.Size = new System.Drawing.Size(641, 489);
            this.ResumeLayout(false);

        }

        #endregion

        private UserCtrl.CodeInput tb_code;
        private UserCtrl.CodeLine tb_line;
    }
}
