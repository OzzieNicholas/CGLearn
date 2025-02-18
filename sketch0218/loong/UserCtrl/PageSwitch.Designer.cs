namespace UserCtrl
{
    partial class PageSwitch
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
            this.btn_next = new System.Windows.Forms.Button();
            this.btn_end = new System.Windows.Forms.Button();
            this.btn_pre = new System.Windows.Forms.Button();
            this.btn_begin = new System.Windows.Forms.Button();
            this.lb_page = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_next
            // 
            this.btn_next.Location = new System.Drawing.Point(121, 2);
            this.btn_next.Margin = new System.Windows.Forms.Padding(2);
            this.btn_next.Name = "btn_next";
            this.btn_next.Size = new System.Drawing.Size(32, 23);
            this.btn_next.TabIndex = 72;
            this.btn_next.Text = ">";
            this.btn_next.UseVisualStyleBackColor = true;
            this.btn_next.Click += new System.EventHandler(this.btn_next_Click);
            // 
            // btn_end
            // 
            this.btn_end.Location = new System.Drawing.Point(157, 2);
            this.btn_end.Margin = new System.Windows.Forms.Padding(2);
            this.btn_end.Name = "btn_end";
            this.btn_end.Size = new System.Drawing.Size(32, 23);
            this.btn_end.TabIndex = 72;
            this.btn_end.Text = ">|";
            this.btn_end.UseVisualStyleBackColor = true;
            this.btn_end.Click += new System.EventHandler(this.btn_end_Click);
            // 
            // btn_pre
            // 
            this.btn_pre.Location = new System.Drawing.Point(38, 3);
            this.btn_pre.Margin = new System.Windows.Forms.Padding(2);
            this.btn_pre.Name = "btn_pre";
            this.btn_pre.Size = new System.Drawing.Size(32, 23);
            this.btn_pre.TabIndex = 72;
            this.btn_pre.Text = "<";
            this.btn_pre.UseVisualStyleBackColor = true;
            this.btn_pre.Click += new System.EventHandler(this.btn_pre_Click);
            // 
            // btn_begin
            // 
            this.btn_begin.Location = new System.Drawing.Point(2, 3);
            this.btn_begin.Margin = new System.Windows.Forms.Padding(2);
            this.btn_begin.Name = "btn_begin";
            this.btn_begin.Size = new System.Drawing.Size(32, 23);
            this.btn_begin.TabIndex = 72;
            this.btn_begin.Text = "|<";
            this.btn_begin.UseVisualStyleBackColor = true;
            this.btn_begin.Click += new System.EventHandler(this.btn_begin_Click);
            // 
            // lb_page
            // 
            this.lb_page.AutoSize = true;
            this.lb_page.Location = new System.Drawing.Point(75, 8);
            this.lb_page.Name = "lb_page";
            this.lb_page.Size = new System.Drawing.Size(41, 12);
            this.lb_page.TabIndex = 73;
            this.lb_page.Text = "11/280";
            // 
            // PageSwitch
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lb_page);
            this.Controls.Add(this.btn_begin);
            this.Controls.Add(this.btn_end);
            this.Controls.Add(this.btn_pre);
            this.Controls.Add(this.btn_next);
            this.Name = "PageSwitch";
            this.Size = new System.Drawing.Size(191, 29);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btn_next;
        private System.Windows.Forms.Button btn_end;
        private System.Windows.Forms.Button btn_pre;
        private System.Windows.Forms.Button btn_begin;
        private System.Windows.Forms.Label lb_page;
    }
}
