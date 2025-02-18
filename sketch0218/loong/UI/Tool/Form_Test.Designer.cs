
namespace UI
{
    partial class Form_Test
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.btn_test = new System.Windows.Forms.Button();
            this.tb_plane = new LoongCAD.UI.TextInput();
            this.lbl_msg = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_test
            // 
            this.btn_test.Location = new System.Drawing.Point(60, 208);
            this.btn_test.Name = "btn_test";
            this.btn_test.Size = new System.Drawing.Size(75, 23);
            this.btn_test.TabIndex = 0;
            this.btn_test.Text = " 测试";
            this.btn_test.UseVisualStyleBackColor = true;
            // 
            // tb_plane
            // 
            this.tb_plane.Location = new System.Drawing.Point(141, 208);
            this.tb_plane.Name = "tb_plane";
            this.tb_plane.Placeholder = "";
            this.tb_plane.Size = new System.Drawing.Size(100, 28);
            this.tb_plane.TabIndex = 1;
            this.tb_plane.Trim = false;
            // 
            // lbl_msg
            // 
            this.lbl_msg.AutoSize = true;
            this.lbl_msg.Location = new System.Drawing.Point(248, 217);
            this.lbl_msg.Name = "lbl_msg";
            this.lbl_msg.Size = new System.Drawing.Size(80, 18);
            this.lbl_msg.TabIndex = 2;
            this.lbl_msg.Text = "提示信息";
            // 
            // Form_Test
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(890, 522);
            this.Controls.Add(this.lbl_msg);
            this.Controls.Add(this.tb_plane);
            this.Controls.Add(this.btn_test);
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Name = "Form_Test";
            this.Text = " 测试窗口";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btn_test;
        private LoongCAD.UI.TextInput tb_plane;
        private System.Windows.Forms.Label lbl_msg;
    }
}