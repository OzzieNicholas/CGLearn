
namespace UI
{
    partial class MainFormTopPanel
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
            this.btn_logo = new UserCtrl.ImageButton();
            this.pan_menu = new UI.MainFormMenuPanel();
            this.pan_title = new UI.MainFormTitle();
            ((System.ComponentModel.ISupportInitialize)(this.btn_logo)).BeginInit();
            this.SuspendLayout();
            // 
            // btn_logo
            // 
            this.btn_logo.backColor = System.Drawing.Color.Transparent;
            this.btn_logo.BackColor = System.Drawing.Color.Transparent;
            this.btn_logo.backColorHL = System.Drawing.Color.Transparent;
            this.btn_logo.Cursor = System.Windows.Forms.Cursors.Hand;
            this.btn_logo.Dock = System.Windows.Forms.DockStyle.Left;
            this.btn_logo.Location = new System.Drawing.Point(0, 0);
            this.btn_logo.Name = "btn_logo";
            this.btn_logo.Size = new System.Drawing.Size(57, 57);
            this.btn_logo.TabIndex = 0;
            this.btn_logo.TabStop = false;
            // 
            // pan_menu
            // 
            this.pan_menu.BackColor = System.Drawing.SystemColors.Control;
            this.pan_menu.Location = new System.Drawing.Point(57, 32);
            this.pan_menu.Margin = new System.Windows.Forms.Padding(0);
            this.pan_menu.menu = null;
            this.pan_menu.Name = "pan_menu";
            this.pan_menu.Size = new System.Drawing.Size(761, 25);
            this.pan_menu.TabIndex = 3;
            // 
            // pan_title
            // 
            this.pan_title.BackColor = System.Drawing.SystemColors.Control;
            this.pan_title.Location = new System.Drawing.Point(57, 0);
            this.pan_title.Margin = new System.Windows.Forms.Padding(0);
            this.pan_title.Name = "pan_title";
            this.pan_title.Size = new System.Drawing.Size(761, 32);
            this.pan_title.TabIndex = 2;
            this.pan_title.title = "";
            // 
            // MainFormTopPanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.pan_menu);
            this.Controls.Add(this.pan_title);
            this.Controls.Add(this.btn_logo);
            this.DoubleBuffered = true;
            this.Name = "MainFormTopPanel";
            this.Size = new System.Drawing.Size(818, 57);
            ((System.ComponentModel.ISupportInitialize)(this.btn_logo)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private UserCtrl.ImageButton btn_logo;
        private MainFormTitle pan_title;
        private MainFormMenuPanel pan_menu;
    }
}
