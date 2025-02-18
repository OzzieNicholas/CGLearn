namespace LoongCAD.UI
{
    partial class ProfileManagerDialog
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
            this.tb_listY = new DoubleInput();
            this.btn_ok = new System.Windows.Forms.Button();
            this.gb_tree = new System.Windows.Forms.GroupBox();
            this.pan_tree = new System.Windows.Forms.Panel();
            this.tb_filter = new LookUpInput();
            this.tb_name = new TextInput();
            this.lb_name = new System.Windows.Forms.Label();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.img_tips = new UI.ProfileTipImage();
            this.pan_props = new System.Windows.Forms.Panel();
            this.gb_profile_type = new System.Windows.Forms.GroupBox();
            this.cbo_profile_subType = new KeyValueComboBox();
            this.tb_profile_type = new TextInput();
            this.lb_profile_type = new System.Windows.Forms.Label();
            this.lb_profile_subType = new System.Windows.Forms.Label();
            this.tc_main = new System.Windows.Forms.TabControl();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.gb_tree.SuspendLayout();
            this.tp_1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.img_tips)).BeginInit();
            this.gb_profile_type.SuspendLayout();
            this.tc_main.SuspendLayout();
            this.SuspendLayout();
            // 
            // tb_listY
            // 
            this.tb_listY.ControlCheckName = null;
            this.tb_listY.DefaultValue = 0D;
            this.tb_listY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_listY.Location = new System.Drawing.Point(106, 27);
            this.tb_listY.Name = "tb_listY";
            this.tb_listY.ParamName = null;
            this.tb_listY.Size = new System.Drawing.Size(179, 21);
            this.tb_listY.TabIndex = 34;
            this.tb_listY.Text = "0";
            this.tb_listY.Value = 0D;
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(332, 535);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 9;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // gb_tree
            // 
            this.gb_tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_tree.Controls.Add(this.pan_tree);
            this.gb_tree.Controls.Add(this.tb_filter);
            this.gb_tree.Location = new System.Drawing.Point(13, 3);
            this.gb_tree.Name = "gb_tree";
            this.gb_tree.Size = new System.Drawing.Size(256, 526);
            this.gb_tree.TabIndex = 10;
            this.gb_tree.TabStop = false;
            // 
            // pan_tree
            // 
            this.pan_tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_tree.Location = new System.Drawing.Point(6, 47);
            this.pan_tree.Name = "pan_tree";
            this.pan_tree.Size = new System.Drawing.Size(239, 469);
            this.pan_tree.TabIndex = 8;
            // 
            // tb_filter
            // 
            this.tb_filter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_filter.BackColor = System.Drawing.Color.White;
            this.tb_filter.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tb_filter.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tb_filter.Location = new System.Drawing.Point(6, 15);
            this.tb_filter.Name = "tb_filter";
            this.tb_filter.Size = new System.Drawing.Size(239, 26);
            this.tb_filter.TabIndex = 6;
            this.tb_filter.WaterMark = "  搜 索";
            // 
            // tb_name
            // 
            this.tb_name.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_name.BackColor = System.Drawing.SystemColors.Window;
            this.tb_name.Location = new System.Drawing.Point(60, 89);
            this.tb_name.Name = "tb_name";
            this.tb_name.ParamName = null;
            this.tb_name.Size = new System.Drawing.Size(232, 21);
            this.tb_name.TabIndex = 6;
            this.tb_name.Trim = false;
            // 
            // lb_name
            // 
            this.lb_name.AutoSize = true;
            this.lb_name.Location = new System.Drawing.Point(13, 92);
            this.lb_name.Name = "lb_name";
            this.lb_name.Size = new System.Drawing.Size(29, 12);
            this.lb_name.TabIndex = 16;
            this.lb_name.Text = "名称";
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.tb_name);
            this.tp_1.Controls.Add(this.lb_name);
            this.tp_1.Controls.Add(this.img_tips);
            this.tp_1.Controls.Add(this.pan_props);
            this.tp_1.Controls.Add(this.gb_profile_type);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(309, 491);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "通用";
            this.tp_1.UseVisualStyleBackColor = true;
            // 
            // img_tips
            // 
            this.img_tips.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.img_tips.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.img_tips.Location = new System.Drawing.Point(7, 116);
            this.img_tips.Name = "img_tips";
            this.img_tips.Size = new System.Drawing.Size(291, 100);
            this.img_tips.TabIndex = 17;
            this.img_tips.TabStop = false;
            // 
            // pan_props
            // 
            this.pan_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_props.Location = new System.Drawing.Point(7, 222);
            this.pan_props.Name = "pan_props";
            this.pan_props.Size = new System.Drawing.Size(291, 263);
            this.pan_props.TabIndex = 14;
            // 
            // gb_profile_type
            // 
            this.gb_profile_type.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_profile_type.Controls.Add(this.cbo_profile_subType);
            this.gb_profile_type.Controls.Add(this.tb_profile_type);
            this.gb_profile_type.Controls.Add(this.lb_profile_type);
            this.gb_profile_type.Controls.Add(this.lb_profile_subType);
            this.gb_profile_type.Location = new System.Drawing.Point(7, 6);
            this.gb_profile_type.Name = "gb_profile_type";
            this.gb_profile_type.Size = new System.Drawing.Size(291, 77);
            this.gb_profile_type.TabIndex = 13;
            this.gb_profile_type.TabStop = false;
            this.gb_profile_type.Text = "截面类型";
            // 
            // cbo_profile_subType
            // 
            this.cbo_profile_subType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbo_profile_subType.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_profile_subType.FormattingEnabled = true;
            this.cbo_profile_subType.Location = new System.Drawing.Point(53, 47);
            this.cbo_profile_subType.Name = "cbo_profile_subType";
            this.cbo_profile_subType.Size = new System.Drawing.Size(232, 21);
            this.cbo_profile_subType.TabIndex = 18;
            this.cbo_profile_subType.SelectionChangeCommitted += new System.EventHandler(this.cbo_profile_subType_SelectionChangeCommitted);
            // 
            // tb_profile_type
            // 
            this.tb_profile_type.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_profile_type.BackColor = System.Drawing.Color.White;
            this.tb_profile_type.CaretVisible = false;
            this.tb_profile_type.Location = new System.Drawing.Point(53, 20);
            this.tb_profile_type.Name = "tb_profile_type";
            this.tb_profile_type.ParamName = null;
            this.tb_profile_type.ReadOnly = true;
            this.tb_profile_type.Size = new System.Drawing.Size(232, 21);
            this.tb_profile_type.TabIndex = 6;
            this.tb_profile_type.Trim = false;
            // 
            // lb_profile_type
            // 
            this.lb_profile_type.AutoSize = true;
            this.lb_profile_type.Location = new System.Drawing.Point(6, 23);
            this.lb_profile_type.Name = "lb_profile_type";
            this.lb_profile_type.Size = new System.Drawing.Size(41, 12);
            this.lb_profile_type.TabIndex = 16;
            this.lb_profile_type.Text = "主类型";
            // 
            // lb_profile_subType
            // 
            this.lb_profile_subType.AutoSize = true;
            this.lb_profile_subType.Location = new System.Drawing.Point(6, 50);
            this.lb_profile_subType.Name = "lb_profile_subType";
            this.lb_profile_subType.Size = new System.Drawing.Size(41, 12);
            this.lb_profile_subType.TabIndex = 17;
            this.lb_profile_subType.Text = "子类型";
            // 
            // tc_main
            // 
            this.tc_main.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_main.Controls.Add(this.tp_1);
            this.tc_main.Location = new System.Drawing.Point(275, 12);
            this.tc_main.Name = "tc_main";
            this.tc_main.SelectedIndex = 0;
            this.tc_main.Size = new System.Drawing.Size(317, 517);
            this.tc_main.TabIndex = 2;
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(504, 535);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(418, 535);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(80, 28);
            this.btn_modify.TabIndex = 9;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // Form_ProfileManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(596, 578);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.gb_tree);
            this.Controls.Add(this.tc_main);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(612, 520);
            this.Name = "Form_ProfileManager";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 截面型材管理器";
            this.Controls.SetChildIndex(this.btn_ok, 0);
            this.Controls.SetChildIndex(this.btn_modify, 0);
            this.Controls.SetChildIndex(this.tc_main, 0);
            this.Controls.SetChildIndex(this.gb_tree, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.gb_tree.ResumeLayout(false);
            this.tp_1.ResumeLayout(false);
            this.tp_1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.img_tips)).EndInit();
            this.gb_profile_type.ResumeLayout(false);
            this.gb_profile_type.PerformLayout();
            this.tc_main.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private DoubleInput tb_listY;
        private System.Windows.Forms.GroupBox gb_tree;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.TabControl tc_main;
        private System.Windows.Forms.GroupBox gb_profile_type;
        private System.Windows.Forms.Label lb_profile_subType;
        private System.Windows.Forms.Label lb_profile_type;
        private KeyValueComboBox cbo_profile_subType;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Panel pan_props;
        private System.Windows.Forms.Panel pan_tree;
        private System.Windows.Forms.Button btn_modify;
        private TextInput tb_profile_type;
        private TextInput tb_name;
        private System.Windows.Forms.Label lb_name;
        private LookUpInput tb_filter;
        private ProfileTipImage img_tips;
    }
}