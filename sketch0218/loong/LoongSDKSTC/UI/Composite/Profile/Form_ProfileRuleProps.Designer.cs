namespace LoongCAD.UI
{
    partial class Form_ProfileRuleProps
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
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.lb_name = new System.Windows.Forms.Label();
            this.tb_name = new TextInput();
            this.lb_target_types = new System.Windows.Forms.Label();
            this.pan_target_types = new System.Windows.Forms.Panel();
            this.lb_filter = new System.Windows.Forms.Label();
            this.tb_filter = new TextInput();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(167, 333);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 9;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(253, 333);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // lb_name
            // 
            this.lb_name.AutoSize = true;
            this.lb_name.Location = new System.Drawing.Point(31, 6);
            this.lb_name.Name = "lb_name";
            this.lb_name.Size = new System.Drawing.Size(29, 12);
            this.lb_name.TabIndex = 20;
            this.lb_name.Text = "名称";
            // 
            // tb_name
            // 
            this.tb_name.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_name.Location = new System.Drawing.Point(67, 0);
            this.tb_name.Margin = new System.Windows.Forms.Padding(0);
            this.tb_name.Name = "tb_name";
            this.tb_name.ParamName = "name";
            this.tb_name.Size = new System.Drawing.Size(254, 21);
            this.tb_name.TabIndex = 16;
            this.tb_name.Trim = false;
            // 
            // lb_target_types
            // 
            this.lb_target_types.AutoSize = true;
            this.lb_target_types.Location = new System.Drawing.Point(7, 3);
            this.lb_target_types.Name = "lb_target_types";
            this.lb_target_types.Size = new System.Drawing.Size(53, 12);
            this.lb_target_types.TabIndex = 21;
            this.lb_target_types.Text = "截面类型";
            // 
            // pan_target_types
            // 
            this.pan_target_types.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_target_types.Location = new System.Drawing.Point(66, 1);
            this.pan_target_types.Margin = new System.Windows.Forms.Padding(0);
            this.pan_target_types.Name = "pan_target_types";
            this.pan_target_types.Size = new System.Drawing.Size(254, 261);
            this.pan_target_types.TabIndex = 22;
            // 
            // lb_filter
            // 
            this.lb_filter.AutoSize = true;
            this.lb_filter.Location = new System.Drawing.Point(7, 6);
            this.lb_filter.Name = "lb_filter";
            this.lb_filter.Size = new System.Drawing.Size(53, 12);
            this.lb_filter.TabIndex = 23;
            this.lb_filter.Text = "截面过滤";
            // 
            // tb_filter
            // 
            this.tb_filter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_filter.Location = new System.Drawing.Point(66, 0);
            this.tb_filter.Margin = new System.Windows.Forms.Padding(0);
            this.tb_filter.Name = "tb_filter";
            this.tb_filter.ParamName = "name_filter";
            this.tb_filter.Size = new System.Drawing.Size(254, 21);
            this.tb_filter.TabIndex = 24;
            this.tb_filter.Trim = false;
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.tb_name);
            this.panel1.Controls.Add(this.lb_name);
            this.panel1.Location = new System.Drawing.Point(12, 7);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(325, 24);
            this.panel1.TabIndex = 26;
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.pan_target_types);
            this.panel2.Controls.Add(this.lb_target_types);
            this.panel2.Location = new System.Drawing.Point(13, 37);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(325, 262);
            this.panel2.TabIndex = 27;
            // 
            // panel3
            // 
            this.panel3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel3.Controls.Add(this.tb_filter);
            this.panel3.Controls.Add(this.lb_filter);
            this.panel3.Location = new System.Drawing.Point(13, 305);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(325, 22);
            this.panel3.TabIndex = 28;
            // 
            // Form_ProfileRuleProps
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(345, 378);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_ProfileRuleProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "截面标准属性";
            this.Controls.SetChildIndex(this.btn_ok, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.Controls.SetChildIndex(this.panel1, 0);
            this.Controls.SetChildIndex(this.panel2, 0);
            this.Controls.SetChildIndex(this.panel3, 0);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Label lb_name;
        private TextInput tb_name;
        private System.Windows.Forms.Label lb_target_types;
        private System.Windows.Forms.Panel pan_target_types;
        private System.Windows.Forms.Label lb_filter;
        private TextInput tb_filter;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
    }
}