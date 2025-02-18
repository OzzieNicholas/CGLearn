
namespace UserCtrl
{
    partial class FoldPanelList
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
            this.pan_grid = new UserCtrl.FoldPanelListGrid();
            this.SuspendLayout();
            // 
            // pan_layout
            // 
            this.pan_grid.ColumnCount = 1;
            this.pan_grid.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 47.13115F));
            this.pan_grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pan_grid.Location = new System.Drawing.Point(0, 0);
            this.pan_grid.Name = "pan_layout";
            this.pan_grid.RowCount = 1;
            this.pan_grid.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 52.34375F));
            this.pan_grid.Size = new System.Drawing.Size(276, 262);
            this.pan_grid.TabIndex = 0;
            // 
            // FoldPanelList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.pan_grid);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "FoldPanelList";
            this.Size = new System.Drawing.Size(276, 262);
            this.ResumeLayout(false);

        }


        #endregion

        private UserCtrl.FoldPanelListGrid pan_grid;
    }
}
