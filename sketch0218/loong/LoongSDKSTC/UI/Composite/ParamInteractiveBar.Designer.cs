
namespace LoongCAD.UI
{
    partial class ParamInteractiveBar
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
        private void InitializeComponent()
        {
            this.btn_get = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btn_get
            // 
            this.btn_get.Location = new System.Drawing.Point(191, -1);
            this.btn_get.Name = "btn_get";
            this.btn_get.Size = new System.Drawing.Size(58, 28);
            this.btn_get.TabIndex = 548;
            this.btn_get.Text = "获取";
            this.btn_get.UseVisualStyleBackColor = true;
            // 
            // btn_modify
            // 
            this.btn_modify.Location = new System.Drawing.Point(127, -1);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 547;
            this.btn_modify.Text = "修改";
            this.btn_modify.UseVisualStyleBackColor = true;
            // 
            // btn_cancel
            // 
            this.btn_cancel.Location = new System.Drawing.Point(319, -1);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 546;
            this.btn_cancel.Text = "取消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            // 
            // btn_apply
            // 
            this.btn_apply.Location = new System.Drawing.Point(63, -1);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 545;
            this.btn_apply.Text = "应用";
            this.btn_apply.UseVisualStyleBackColor = true;
            // 
            // btn_ok
            // 
            this.btn_ok.Location = new System.Drawing.Point(-1, -1);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 544;
            this.btn_ok.Text = "确定";
            this.btn_ok.UseVisualStyleBackColor = true;
            // 
            // btn_chb
            // 
            this.btn_chb.BackgroundImage = global::LoongCAD.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(255, -1);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 549;
            this.btn_chb.UseVisualStyleBackColor = true;
            // 
            // ParamInteractiveBar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_get);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_ok);
            this.Name = "ParamInteractiveBar";
            this.Size = new System.Drawing.Size(376, 26);
            this.ResumeLayout(false);

        }

        #endregion

        protected System.Windows.Forms.Button btn_chb;
        protected System.Windows.Forms.Button btn_get;
        protected System.Windows.Forms.Button btn_modify;
        protected System.Windows.Forms.Button btn_cancel;
        protected System.Windows.Forms.Button btn_apply;
        protected System.Windows.Forms.Button btn_ok;
    }
}
