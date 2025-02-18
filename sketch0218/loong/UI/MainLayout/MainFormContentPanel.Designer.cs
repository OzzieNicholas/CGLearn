
namespace UI
{
    partial class MainFormContentPanel
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
            this.layout_container = new UI.MainFormLayoutContainer();
            this.SuspendLayout();
            // 
            // layout_container
            // 
            this.layout_container.Dock = System.Windows.Forms.DockStyle.Fill;
            this.layout_container.Location = new System.Drawing.Point(0, 0);
            this.layout_container.Margin = new System.Windows.Forms.Padding(2);
            this.layout_container.Name = "layout_container";
            this.layout_container.Size = new System.Drawing.Size(818, 599);
            this.layout_container.TabIndex = 5;
            // 
            // MainFormContentPanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.layout_container);
            this.DoubleBuffered = true;
            this.Name = "MainFormContentPanel";
            this.Size = new System.Drawing.Size(818, 599);
            this.ResumeLayout(false);

        }

        #endregion

        private MainFormLayoutContainer layout_container;
    }
}
