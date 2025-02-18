
namespace UserCtrl
{
    partial class FoldPanel
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
            this.pan_title = new System.Windows.Forms.Panel();
            this.lb_title = new System.Windows.Forms.Label();
            this.pan_content = new UserCtrl.FoldPanelContent();
            this.chb_fold = new System.Windows.Forms.PictureBox();
            this.pan_title.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chb_fold)).BeginInit();
            this.SuspendLayout();
            // 
            // pan_title
            // 
            this.pan_title.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_title.BackColor = System.Drawing.Color.LightGray;
            this.pan_title.Controls.Add(this.lb_title);
            this.pan_title.Controls.Add(this.chb_fold);
            this.pan_title.Location = new System.Drawing.Point(0, 0);
            this.pan_title.Margin = new System.Windows.Forms.Padding(0);
            this.pan_title.Name = "pan_title";
            this.pan_title.Size = new System.Drawing.Size(368, 22);
            this.pan_title.TabIndex = 0;
            this.pan_title.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pan_title_MouseDown);
            // 
            // lb_title
            // 
            this.lb_title.AutoSize = true;
            this.lb_title.Location = new System.Drawing.Point(5, 6);
            this.lb_title.Name = "lb_title";
            this.lb_title.Size = new System.Drawing.Size(35, 12);
            this.lb_title.TabIndex = 2;
            this.lb_title.Text = "Title";
            // 
            // pan_content
            // 
            this.pan_content.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_content.BackColor = System.Drawing.Color.Gray;
            this.pan_content.Location = new System.Drawing.Point(0, 22);
            this.pan_content.Margin = new System.Windows.Forms.Padding(0);
            this.pan_content.Name = "pan_content";
            this.pan_content.Size = new System.Drawing.Size(368, 274);
            this.pan_content.TabIndex = 1;
            // 
            // chb_fold
            // 
            this.chb_fold.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.chb_fold.Enabled = false;
            this.chb_fold.Image = global::UserCtrl.Properties.Resources.fold_panel_unfold;
            this.chb_fold.InitialImage = null;
            this.chb_fold.Location = new System.Drawing.Point(346, 2);
            this.chb_fold.Name = "chb_fold";
            this.chb_fold.Size = new System.Drawing.Size(16, 16);
            this.chb_fold.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.chb_fold.TabIndex = 1;
            this.chb_fold.TabStop = false;
            // 
            // FoldPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.pan_title);
            this.Controls.Add(this.pan_content);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "FoldPanel";
            this.Size = new System.Drawing.Size(368, 296);
            this.pan_title.ResumeLayout(false);
            this.pan_title.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chb_fold)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pan_title;
        private System.Windows.Forms.PictureBox chb_fold;
        private FoldPanelContent pan_content;
        private System.Windows.Forms.Label lb_title;
    }
}
