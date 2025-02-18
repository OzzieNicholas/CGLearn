
namespace UI
{
    partial class CellTab
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
            this.btn_scrollLeft = new System.Windows.Forms.Button();
            this.btn_scrollRight = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btn_scrollLeft
            // 
            this.btn_scrollLeft.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_scrollLeft.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_scrollLeft.Location = new System.Drawing.Point(120, 1);
            this.btn_scrollLeft.Name = "btn_scrollLeft";
            this.btn_scrollLeft.Size = new System.Drawing.Size(26, 26);
            this.btn_scrollLeft.TabIndex = 0;
            this.btn_scrollLeft.Text = "<";
            this.btn_scrollLeft.UseVisualStyleBackColor = true;
            this.btn_scrollLeft.Click += new System.EventHandler(this._onScrollLeftButtonClick);
            // 
            // btn_scrollRight
            // 
            this.btn_scrollRight.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_scrollRight.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_scrollRight.Location = new System.Drawing.Point(147, 1);
            this.btn_scrollRight.Name = "btn_scrollRight";
            this.btn_scrollRight.Size = new System.Drawing.Size(26, 26);
            this.btn_scrollRight.TabIndex = 0;
            this.btn_scrollRight.Text = ">";
            this.btn_scrollRight.UseVisualStyleBackColor = true;
            this.btn_scrollRight.Click += new System.EventHandler(this._onScrollRightButtonClick);
            // 
            // CellTab
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.btn_scrollRight);
            this.Controls.Add(this.btn_scrollLeft);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "CellTab";
            this.Size = new System.Drawing.Size(176, 28);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btn_scrollLeft;
        private System.Windows.Forms.Button btn_scrollRight;
    }
}
