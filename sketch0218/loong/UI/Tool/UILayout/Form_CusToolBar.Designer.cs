namespace UI
{
    partial class Form_CusToolBar
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
            this.btn_add = new System.Windows.Forms.Button();
            this.btn_remove = new System.Windows.Forms.Button();
            this.gb_tree = new System.Windows.Forms.GroupBox();
            this.tb_filter = new UserCtrl.InputLookUp();
            this.func_unit_group = new UserCtrl.ComboBoxKey();
            this.pan_inventory_tree = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btn_add_separator = new System.Windows.Forms.Button();
            this.btn_move_up = new System.Windows.Forms.Button();
            this.btn_move_down = new System.Windows.Forms.Button();
            this.new_bar_input = new UserCtrl.InputText();
            this.pan_provision_tree = new System.Windows.Forms.Panel();
            this.new_bar_button = new System.Windows.Forms.Button();
            this.new_bar_label = new System.Windows.Forms.Label();
            this.btn_modify = new System.Windows.Forms.Button();
            this.del_bar_button = new System.Windows.Forms.Button();
            this.gb_tree.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(417, 545);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 3;
            this.btn_ok.Text = "确 定";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.Btn_ok_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(618, 545);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 4;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.Btn_cancel_Click);
            // 
            // btn_add
            // 
            this.btn_add.Location = new System.Drawing.Point(335, 237);
            this.btn_add.Name = "btn_add";
            this.btn_add.Size = new System.Drawing.Size(62, 29);
            this.btn_add.TabIndex = 11;
            this.btn_add.Text = ">>";
            this.btn_add.UseVisualStyleBackColor = true;
            this.btn_add.Click += new System.EventHandler(this.Btn_add_Click);
            // 
            // btn_remove
            // 
            this.btn_remove.Location = new System.Drawing.Point(335, 272);
            this.btn_remove.Name = "btn_remove";
            this.btn_remove.Size = new System.Drawing.Size(62, 29);
            this.btn_remove.TabIndex = 12;
            this.btn_remove.Text = "<<";
            this.btn_remove.UseVisualStyleBackColor = true;
            this.btn_remove.Click += new System.EventHandler(this.Btn_remove_Click);
            // 
            // gb_tree
            // 
            this.gb_tree.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.gb_tree.Controls.Add(this.tb_filter);
            this.gb_tree.Controls.Add(this.func_unit_group);
            this.gb_tree.Controls.Add(this.pan_inventory_tree);
            this.gb_tree.Location = new System.Drawing.Point(13, 12);
            this.gb_tree.Name = "gb_tree";
            this.gb_tree.Size = new System.Drawing.Size(317, 525);
            this.gb_tree.TabIndex = 21;
            this.gb_tree.TabStop = false;
            this.gb_tree.Text = "功能项";
            // 
            // tb_filter
            // 
            this.tb_filter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_filter.BackColor = System.Drawing.Color.White;
            this.tb_filter.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tb_filter.btnWidth = 36;
            this.tb_filter.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tb_filter.Location = new System.Drawing.Point(6, 20);
            this.tb_filter.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tb_filter.Name = "tb_filter";
            this.tb_filter.PropName = null;
            this.tb_filter.Size = new System.Drawing.Size(302, 26);
            this.tb_filter.TabIndex = 6;
            this.tb_filter.waterMark = "  搜 索";
            // 
            // func_unit_group
            // 
            this.func_unit_group.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.func_unit_group.CheckName = "chb_offsetH";
            this.func_unit_group.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.func_unit_group.FormattingEnabled = true;
            this.func_unit_group.Location = new System.Drawing.Point(6, 53);
            this.func_unit_group.Name = "func_unit_group";
            this.func_unit_group.PropName = "offsetH_base";
            this.func_unit_group.Size = new System.Drawing.Size(302, 21);
            this.func_unit_group.TabIndex = 15;
            this.func_unit_group.SelectedIndexChanged += new System.EventHandler(this.func_unit_group_SelectedIndexChanged);
            // 
            // pan_inventory_tree
            // 
            this.pan_inventory_tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_inventory_tree.BackColor = System.Drawing.SystemColors.Control;
            this.pan_inventory_tree.Location = new System.Drawing.Point(6, 80);
            this.pan_inventory_tree.Name = "pan_inventory_tree";
            this.pan_inventory_tree.Size = new System.Drawing.Size(302, 439);
            this.pan_inventory_tree.TabIndex = 7;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.del_bar_button);
            this.groupBox1.Controls.Add(this.btn_add_separator);
            this.groupBox1.Controls.Add(this.btn_move_up);
            this.groupBox1.Controls.Add(this.btn_move_down);
            this.groupBox1.Controls.Add(this.new_bar_input);
            this.groupBox1.Controls.Add(this.pan_provision_tree);
            this.groupBox1.Controls.Add(this.new_bar_button);
            this.groupBox1.Controls.Add(this.new_bar_label);
            this.groupBox1.Location = new System.Drawing.Point(403, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(301, 525);
            this.groupBox1.TabIndex = 22;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "工具栏";
            // 
            // btn_add_separator
            // 
            this.btn_add_separator.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_add_separator.Location = new System.Drawing.Point(193, 494);
            this.btn_add_separator.Name = "btn_add_separator";
            this.btn_add_separator.Size = new System.Drawing.Size(102, 25);
            this.btn_add_separator.TabIndex = 20;
            this.btn_add_separator.Text = "添加分隔线";
            this.btn_add_separator.UseVisualStyleBackColor = true;
            this.btn_add_separator.Click += new System.EventHandler(this.btn_add_separator_Click);
            // 
            // btn_move_up
            // 
            this.btn_move_up.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_move_up.Location = new System.Drawing.Point(58, 494);
            this.btn_move_up.Name = "btn_move_up";
            this.btn_move_up.Size = new System.Drawing.Size(61, 25);
            this.btn_move_up.TabIndex = 18;
            this.btn_move_up.Text = "上移";
            this.btn_move_up.UseVisualStyleBackColor = true;
            this.btn_move_up.Click += new System.EventHandler(this.btn_move_up_Click);
            // 
            // btn_move_down
            // 
            this.btn_move_down.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_move_down.Location = new System.Drawing.Point(126, 494);
            this.btn_move_down.Name = "btn_move_down";
            this.btn_move_down.Size = new System.Drawing.Size(61, 25);
            this.btn_move_down.TabIndex = 19;
            this.btn_move_down.Text = "下移";
            this.btn_move_down.UseVisualStyleBackColor = true;
            this.btn_move_down.Click += new System.EventHandler(this.btn_move_down_Click);
            // 
            // new_bar_input
            // 
            this.new_bar_input.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.new_bar_input.Location = new System.Drawing.Point(82, 20);
            this.new_bar_input.Name = "new_bar_input";
            this.new_bar_input.PropName = null;
            this.new_bar_input.Size = new System.Drawing.Size(99, 21);
            this.new_bar_input.TabIndex = 15;
            this.new_bar_input.TabStop = false;
            this.new_bar_input.Trim = false;
            // 
            // pan_provision_tree
            // 
            this.pan_provision_tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_provision_tree.BackColor = System.Drawing.SystemColors.Control;
            this.pan_provision_tree.Location = new System.Drawing.Point(6, 52);
            this.pan_provision_tree.Name = "pan_provision_tree";
            this.pan_provision_tree.Size = new System.Drawing.Size(288, 436);
            this.pan_provision_tree.TabIndex = 8;
            // 
            // new_bar_button
            // 
            this.new_bar_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.new_bar_button.Location = new System.Drawing.Point(187, 17);
            this.new_bar_button.Name = "new_bar_button";
            this.new_bar_button.Size = new System.Drawing.Size(51, 25);
            this.new_bar_button.TabIndex = 16;
            this.new_bar_button.Text = "新建";
            this.new_bar_button.UseVisualStyleBackColor = true;
            this.new_bar_button.Click += new System.EventHandler(this.new_bar_button_Click);
            // 
            // new_bar_label
            // 
            this.new_bar_label.AutoSize = true;
            this.new_bar_label.Location = new System.Drawing.Point(6, 24);
            this.new_bar_label.Name = "new_bar_label";
            this.new_bar_label.Size = new System.Drawing.Size(65, 12);
            this.new_bar_label.TabIndex = 17;
            this.new_bar_label.Text = "新建工具栏";
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(518, 545);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(80, 28);
            this.btn_modify.TabIndex = 23;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // del_bar_button
            // 
            this.del_bar_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.del_bar_button.Location = new System.Drawing.Point(244, 17);
            this.del_bar_button.Name = "del_bar_button";
            this.del_bar_button.Size = new System.Drawing.Size(51, 25);
            this.del_bar_button.TabIndex = 21;
            this.del_bar_button.Text = "删除";
            this.del_bar_button.UseVisualStyleBackColor = true;
            this.del_bar_button.Click += new System.EventHandler(this.del_bar_button_Click);
            // 
            // Form_CusToolBar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(714, 593);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.gb_tree);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_remove);
            this.Controls.Add(this.btn_add);
            this.Controls.Add(this.btn_ok);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(730, 1994);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(730, 494);
            this.Name = "Form_CusToolBar";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = " 自定义工具栏";
            this.gb_tree.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_add;
        private System.Windows.Forms.Button btn_remove;
        private System.Windows.Forms.GroupBox gb_tree;
        private System.Windows.Forms.Panel pan_inventory_tree;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel pan_provision_tree;
        private UserCtrl.InputText new_bar_input;
        private System.Windows.Forms.Label new_bar_label;
        private UserCtrl.ComboBoxKey func_unit_group;
        private System.Windows.Forms.Button new_bar_button;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_add_separator;
        private System.Windows.Forms.Button btn_move_down;
        private System.Windows.Forms.Button btn_move_up;
        private UserCtrl.InputLookUp tb_filter;
        private System.Windows.Forms.Button del_bar_button;
    }
}