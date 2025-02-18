namespace UI
{
    partial class Form_CtrlPointCornerProps
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
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.tb_cut_x = new UserCtrl.InputDouble();
            this.chb_cut_x = new UserCtrl.Check();
            this.chb_cut_type = new UserCtrl.Check();
            this.tb_cut_y = new UserCtrl.InputDouble();
            this.chb_cut_y = new UserCtrl.Check();
            this.tb_dz1 = new UserCtrl.InputDouble();
            this.chb_dz1 = new UserCtrl.Check();
            this.tb_dz2 = new UserCtrl.InputDouble();
            this.chb_dz2 = new UserCtrl.Check();
            this.chb_cut_segs = new UserCtrl.Check();
            this.tb_cut_segs = new UserCtrl.InputInt();
            this.btn_cutxy_swap = new System.Windows.Forms.Button();
            this.cbo_cut_type = new UserCtrl.ComboBoxImage();
            this.btn_chb = new System.Windows.Forms.Button();
            this.tip_main = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(18, 194);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 60;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(146, 194);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 80;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(210, 194);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(58, 28);
            this.btn_pick.TabIndex = 90;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "获 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(338, 194);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 110;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(82, 194);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 70;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // tb_cut_x
            // 
            this.tb_cut_x.CheckName = "chb_cut_x";
            this.tb_cut_x.DefValue = 0D;
            this.tb_cut_x.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_cut_x.Location = new System.Drawing.Point(70, 46);
            this.tb_cut_x.Max = 999999D;
            this.tb_cut_x.Min = -999999D;
            this.tb_cut_x.Name = "tb_cut_x";
            this.tb_cut_x.PropName = "cut_x";
            this.tb_cut_x.Size = new System.Drawing.Size(267, 21);
            this.tb_cut_x.TabIndex = 20;
            this.tb_cut_x.Text = "0";
            this.tb_cut_x.unitEnobType = "model_dist";
            this.tb_cut_x.Value = 0D;
            // 
            // chb_cut_x
            // 
            this.chb_cut_x.AutoSize = true;
            this.chb_cut_x.Checked = true;
            this.chb_cut_x.CheckName = null;
            this.chb_cut_x.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_x.Location = new System.Drawing.Point(13, 48);
            this.chb_cut_x.Name = "chb_cut_x";
            this.chb_cut_x.PropName = null;
            this.chb_cut_x.Size = new System.Drawing.Size(30, 16);
            this.chb_cut_x.TabIndex = 8;
            this.chb_cut_x.TabStop = false;
            this.chb_cut_x.Text = "X";
            this.chb_cut_x.UseVisualStyleBackColor = true;
            // 
            // chb_cut_type
            // 
            this.chb_cut_type.AutoSize = true;
            this.chb_cut_type.Checked = true;
            this.chb_cut_type.CheckName = null;
            this.chb_cut_type.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_type.Location = new System.Drawing.Point(13, 14);
            this.chb_cut_type.Name = "chb_cut_type";
            this.chb_cut_type.PropName = null;
            this.chb_cut_type.Size = new System.Drawing.Size(48, 16);
            this.chb_cut_type.TabIndex = 33;
            this.chb_cut_type.TabStop = false;
            this.chb_cut_type.Text = "类型";
            this.chb_cut_type.UseVisualStyleBackColor = true;
            // 
            // tb_cut_y
            // 
            this.tb_cut_y.CheckName = "chb_cut_y";
            this.tb_cut_y.DefValue = 0D;
            this.tb_cut_y.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_cut_y.Location = new System.Drawing.Point(70, 73);
            this.tb_cut_y.Max = 999999D;
            this.tb_cut_y.Min = -999999D;
            this.tb_cut_y.Name = "tb_cut_y";
            this.tb_cut_y.PropName = "cut_y";
            this.tb_cut_y.Size = new System.Drawing.Size(267, 21);
            this.tb_cut_y.TabIndex = 30;
            this.tb_cut_y.Text = "0";
            this.tb_cut_y.unitEnobType = "model_dist";
            this.tb_cut_y.Value = 0D;
            // 
            // chb_cut_y
            // 
            this.chb_cut_y.AutoSize = true;
            this.chb_cut_y.Checked = true;
            this.chb_cut_y.CheckName = null;
            this.chb_cut_y.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_y.Location = new System.Drawing.Point(13, 75);
            this.chb_cut_y.Name = "chb_cut_y";
            this.chb_cut_y.PropName = null;
            this.chb_cut_y.Size = new System.Drawing.Size(30, 16);
            this.chb_cut_y.TabIndex = 35;
            this.chb_cut_y.TabStop = false;
            this.chb_cut_y.Text = "Y";
            this.chb_cut_y.UseVisualStyleBackColor = true;
            // 
            // tb_dz1
            // 
            this.tb_dz1.CheckName = "chb_dz1";
            this.tb_dz1.DefValue = 0D;
            this.tb_dz1.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dz1.Location = new System.Drawing.Point(70, 126);
            this.tb_dz1.Max = 999999D;
            this.tb_dz1.Min = -999999D;
            this.tb_dz1.Name = "tb_dz1";
            this.tb_dz1.PropName = "dz1";
            this.tb_dz1.Size = new System.Drawing.Size(323, 21);
            this.tb_dz1.TabIndex = 50;
            this.tb_dz1.Text = "0";
            this.tb_dz1.unitEnobType = "model_dist";
            this.tb_dz1.Value = 0D;
            // 
            // chb_dz1
            // 
            this.chb_dz1.AutoSize = true;
            this.chb_dz1.Checked = true;
            this.chb_dz1.CheckName = null;
            this.chb_dz1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dz1.Location = new System.Drawing.Point(13, 128);
            this.chb_dz1.Name = "chb_dz1";
            this.chb_dz1.PropName = null;
            this.chb_dz1.Size = new System.Drawing.Size(42, 16);
            this.chb_dz1.TabIndex = 37;
            this.chb_dz1.TabStop = false;
            this.chb_dz1.Text = "dz1";
            this.chb_dz1.UseVisualStyleBackColor = true;
            // 
            // tb_dz2
            // 
            this.tb_dz2.CheckName = "chb_dz2";
            this.tb_dz2.DefValue = 0D;
            this.tb_dz2.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_dz2.Location = new System.Drawing.Point(70, 153);
            this.tb_dz2.Max = 999999D;
            this.tb_dz2.Min = -999999D;
            this.tb_dz2.Name = "tb_dz2";
            this.tb_dz2.PropName = "dz2";
            this.tb_dz2.Size = new System.Drawing.Size(323, 21);
            this.tb_dz2.TabIndex = 60;
            this.tb_dz2.Text = "0";
            this.tb_dz2.unitEnobType = "model_dist";
            this.tb_dz2.Value = 0D;
            // 
            // chb_dz2
            // 
            this.chb_dz2.AutoSize = true;
            this.chb_dz2.Checked = true;
            this.chb_dz2.CheckName = null;
            this.chb_dz2.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_dz2.Location = new System.Drawing.Point(13, 155);
            this.chb_dz2.Name = "chb_dz2";
            this.chb_dz2.PropName = null;
            this.chb_dz2.Size = new System.Drawing.Size(42, 16);
            this.chb_dz2.TabIndex = 39;
            this.chb_dz2.TabStop = false;
            this.chb_dz2.Text = "dz2";
            this.chb_dz2.UseVisualStyleBackColor = true;
            // 
            // chb_cut_segs
            // 
            this.chb_cut_segs.AutoSize = true;
            this.chb_cut_segs.Checked = true;
            this.chb_cut_segs.CheckName = null;
            this.chb_cut_segs.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_cut_segs.Location = new System.Drawing.Point(13, 101);
            this.chb_cut_segs.Name = "chb_cut_segs";
            this.chb_cut_segs.PropName = null;
            this.chb_cut_segs.Size = new System.Drawing.Size(48, 16);
            this.chb_cut_segs.TabIndex = 35;
            this.chb_cut_segs.TabStop = false;
            this.chb_cut_segs.Text = "分段";
            this.chb_cut_segs.UseVisualStyleBackColor = true;
            // 
            // tb_cut_segs
            // 
            this.tb_cut_segs.CheckName = "chb_cut_segs";
            this.tb_cut_segs.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_cut_segs.Location = new System.Drawing.Point(70, 99);
            this.tb_cut_segs.Max = 999999;
            this.tb_cut_segs.Min = 0;
            this.tb_cut_segs.Name = "tb_cut_segs";
            this.tb_cut_segs.PropName = "cut_segs";
            this.tb_cut_segs.Size = new System.Drawing.Size(323, 21);
            this.tb_cut_segs.TabIndex = 40;
            this.tb_cut_segs.Text = "0";
            // 
            // btn_cutxy_swap
            // 
            this.btn_cutxy_swap.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cutxy_swap.BackgroundImage = global::UI.Properties.Resources.config_text_swap_96;
            this.btn_cutxy_swap.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_cutxy_swap.Location = new System.Drawing.Point(343, 51);
            this.btn_cutxy_swap.Name = "btn_cutxy_swap";
            this.btn_cutxy_swap.Size = new System.Drawing.Size(50, 40);
            this.btn_cutxy_swap.TabIndex = 111;
            this.btn_cutxy_swap.TabStop = false;
            this.tip_main.SetToolTip(this.btn_cutxy_swap, "互换");
            this.btn_cutxy_swap.UseVisualStyleBackColor = true;
            this.btn_cutxy_swap.Click += new System.EventHandler(this.btn_cutxy_swap_Click);
            // 
            // cbo_cut_type
            // 
            this.cbo_cut_type.CheckName = "chb_cut_type";
            this.cbo_cut_type.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_cut_type.FormattingEnabled = true;
            this.cbo_cut_type.Image = global::UI.Properties.Resources.chamfer_type;
            this.cbo_cut_type.ImageBorder = new System.Windows.Forms.Padding(1, 23, 1, 1);
            this.cbo_cut_type.ImageItemSize = new System.Drawing.Size(34, 22);
            this.cbo_cut_type.ItemHeight = 22;
            this.cbo_cut_type.Location = new System.Drawing.Point(70, 12);
            this.cbo_cut_type.Name = "cbo_cut_type";
            this.cbo_cut_type.PropName = "cut_type";
            this.cbo_cut_type.Size = new System.Drawing.Size(323, 28);
            this.cbo_cut_type.TabIndex = 10;
            this.cbo_cut_type.SelectedIndexChanged += new System.EventHandler(this.cbo_cut_type_SelectedIndexChanged);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(274, 194);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 100;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.Btn_chb_Click);
            // 
            // Form_CtrlPointCornerProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(406, 229);
            this.Controls.Add(this.btn_cutxy_swap);
            this.Controls.Add(this.cbo_cut_type);
            this.Controls.Add(this.tb_dz2);
            this.Controls.Add(this.chb_dz2);
            this.Controls.Add(this.tb_dz1);
            this.Controls.Add(this.chb_dz1);
            this.Controls.Add(this.tb_cut_segs);
            this.Controls.Add(this.chb_cut_segs);
            this.Controls.Add(this.tb_cut_y);
            this.Controls.Add(this.chb_cut_y);
            this.Controls.Add(this.chb_cut_type);
            this.Controls.Add(this.tb_cut_x);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.chb_cut_x);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_CtrlPointCornerProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 切角属性";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private UserCtrl.Check chb_cut_x;
        private UserCtrl.InputDouble tb_cut_x;
        private System.Windows.Forms.Button btn_chb;
        private UserCtrl.Check chb_cut_type;
        private UserCtrl.InputDouble tb_cut_y;
        private UserCtrl.Check chb_cut_y;
        private UserCtrl.InputDouble tb_dz1;
        private UserCtrl.Check chb_dz1;
        private UserCtrl.InputDouble tb_dz2;
        private UserCtrl.Check chb_dz2;
        private UserCtrl.ComboBoxImage cbo_cut_type;
        private UserCtrl.Check chb_cut_segs;
        private UserCtrl.InputInt tb_cut_segs;
        private System.Windows.Forms.Button btn_cutxy_swap;
        private System.Windows.Forms.ToolTip tip_main;
    }
}