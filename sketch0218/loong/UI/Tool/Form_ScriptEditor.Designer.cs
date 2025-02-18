namespace UI
{
    partial class Form_ScriptEditor
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.btn_play = new System.Windows.Forms.Button();
            this.split_main = new System.Windows.Forms.SplitContainer();
            this.btn_clear_result = new System.Windows.Forms.Button();
            this.btn_save = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.tip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.split_main)).BeginInit();
            this.split_main.Panel2.SuspendLayout();
            this.split_main.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_play
            // 
            this.btn_play.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_play.Location = new System.Drawing.Point(694, 5);
            this.btn_play.Name = "btn_play";
            this.btn_play.Size = new System.Drawing.Size(90, 31);
            this.btn_play.TabIndex = 8;
            this.btn_play.Text = "运行(F4)";
            this.btn_play.UseVisualStyleBackColor = true;
            this.btn_play.Click += new System.EventHandler(this.Btn_play_Click);
            // 
            // split_main
            // 
            this.split_main.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.split_main.Location = new System.Drawing.Point(13, 42);
            this.split_main.Name = "split_main";
            this.split_main.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // split_main.Panel2
            // 
            this.split_main.Panel2.Controls.Add(this.btn_clear_result);
            this.split_main.Size = new System.Drawing.Size(770, 464);
            this.split_main.SplitterDistance = 295;
            this.split_main.TabIndex = 9;
            // 
            // btn_clear_result
            // 
            this.btn_clear_result.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_clear_result.Location = new System.Drawing.Point(681, 3);
            this.btn_clear_result.Name = "btn_clear_result";
            this.btn_clear_result.Size = new System.Drawing.Size(66, 24);
            this.btn_clear_result.TabIndex = 8;
            this.btn_clear_result.Text = "清除结果";
            this.btn_clear_result.UseVisualStyleBackColor = true;
            this.btn_clear_result.Click += new System.EventHandler(this.Btn_clear_result_Click);
            // 
            // btn_save
            // 
            this.btn_save.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_save.Location = new System.Drawing.Point(693, 512);
            this.btn_save.Name = "btn_save";
            this.btn_save.Size = new System.Drawing.Size(90, 31);
            this.btn_save.TabIndex = 8;
            this.btn_save.Tag = "";
            this.btn_save.Text = "保 存";
            this.tip.SetToolTip(this.btn_save, "Ctrl+S");
            this.btn_save.UseVisualStyleBackColor = true;
            this.btn_save.Click += new System.EventHandler(this.Btn_save_Click);
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(597, 512);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(90, 31);
            this.btn_ok.TabIndex = 8;
            this.btn_ok.Tag = "确 定";
            this.btn_ok.Text = "确 定";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // Form_ScriptEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(796, 555);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.btn_save);
            this.Controls.Add(this.split_main);
            this.Controls.Add(this.btn_play);
            this.Name = "Form_ScriptEditor";
            this.Text = " 运行脚本";
            this.Controls.SetChildIndex(this.btn_play, 0);
            this.Controls.SetChildIndex(this.split_main, 0);
            this.Controls.SetChildIndex(this.btn_save, 0);
            this.Controls.SetChildIndex(this.btn_ok, 0);
            this.split_main.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.split_main)).EndInit();
            this.split_main.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_play;
        private System.Windows.Forms.SplitContainer split_main;
        private System.Windows.Forms.Button btn_clear_result;
        private System.Windows.Forms.Button btn_save;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.ToolTip tip;
        
    }
}