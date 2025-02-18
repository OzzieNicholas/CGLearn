
namespace LoongCAD.UI
{
    partial class LookUpInput
    {
        /// <summary> 
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent() {
            this.tb = new System.Windows.Forms.TextBox();
            this.img = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.img)).BeginInit();
            this.SuspendLayout();
            // 
            // tb
            // 
            this.tb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tb.Location = new System.Drawing.Point(0, 6);
            this.tb.Margin = new System.Windows.Forms.Padding(0);
            this.tb.Name = "tb";
            this.tb.Size = new System.Drawing.Size(160, 14);
            this.tb.TabIndex = 0;
            // 
            // img
            // 
            this.img.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.img.BackColor = System.Drawing.Color.White;
            this.img.BackgroundImage = global::LoongCAD.Properties.Resources.input_lookup;
            this.img.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.img.Location = new System.Drawing.Point(168, 1);
            this.img.Name = "img";
            this.img.Size = new System.Drawing.Size(24, 24);
            this.img.TabIndex = 1;
            this.img.TabStop = false;
            // 
            // InputLookUp
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.img);
            this.Controls.Add(this.tb);
            this.Name = "InputLookUp";
            this.Size = new System.Drawing.Size(193, 24);
            ((System.ComponentModel.ISupportInitialize)(this.img)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tb;
        private System.Windows.Forms.PictureBox img;
    }
}
