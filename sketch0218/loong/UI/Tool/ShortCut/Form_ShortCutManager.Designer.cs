namespace UI
{
    partial class Form_ShortCutManager
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
            this.tb_listY = new UserCtrl.InputDouble();
            this.btn_ok = new System.Windows.Forms.Button();
            this.gb_tree = new System.Windows.Forms.GroupBox();
            this.cbo_group = new UserCtrl.ComboBoxKey();
            this.tb_filter = new UserCtrl.InputLookUp();
            this.pan_grid = new System.Windows.Forms.Panel();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_export = new System.Windows.Forms.Button();
            this.btn_import = new System.Windows.Forms.Button();
            this.btn_restore = new System.Windows.Forms.Button();
            this.gb_tree.SuspendLayout();
            this.SuspendLayout();
            // 
            // tb_listY
            // 
            this.tb_listY.CheckName = null;
            this.tb_listY.DefValue = 0D;
            this.tb_listY.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_listY.Location = new System.Drawing.Point(106, 27);
            this.tb_listY.Name = "tb_listY";
            this.tb_listY.PropName = null;
            this.tb_listY.Size = new System.Drawing.Size(179, 21);
            this.tb_listY.TabIndex = 34;
            this.tb_listY.Text = "0";
            this.tb_listY.Value = 0D;
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(240, 563);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 9;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // gb_tree
            // 
            this.gb_tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_tree.Controls.Add(this.cbo_group);
            this.gb_tree.Controls.Add(this.tb_filter);
            this.gb_tree.Controls.Add(this.pan_grid);
            this.gb_tree.Location = new System.Drawing.Point(13, 3);
            this.gb_tree.Name = "gb_tree";
            this.gb_tree.Size = new System.Drawing.Size(565, 554);
            this.gb_tree.TabIndex = 10;
            this.gb_tree.TabStop = false;
            // 
            // cbo_group
            // 
            this.cbo_group.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbo_group.CheckName = "chb_offsetH";
            this.cbo_group.Font = new System.Drawing.Font("宋体", 13F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.cbo_group.FormattingEnabled = true;
            this.cbo_group.Location = new System.Drawing.Point(13, 47);
            this.cbo_group.Name = "cbo_group";
            this.cbo_group.PropName = "offsetH_base";
            this.cbo_group.Size = new System.Drawing.Size(541, 21);
            this.cbo_group.TabIndex = 11;
            this.cbo_group.SelectionChangeCommitted += new System.EventHandler(this.cbo_group_SelectionChangeCommitted);
            // 
            // tb_filter
            // 
            this.tb_filter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_filter.BackColor = System.Drawing.Color.White;
            this.tb_filter.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tb_filter.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tb_filter.Location = new System.Drawing.Point(13, 15);
            this.tb_filter.Name = "tb_filter";
            this.tb_filter.PropName = null;
            this.tb_filter.Size = new System.Drawing.Size(541, 26);
            this.tb_filter.TabIndex = 6;
            this.tb_filter.waterMark = "  搜 索";
            // 
            // pan_grid
            // 
            this.pan_grid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pan_grid.Location = new System.Drawing.Point(13, 75);
            this.pan_grid.Name = "pan_grid";
            this.pan_grid.Size = new System.Drawing.Size(541, 473);
            this.pan_grid.TabIndex = 8;
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(498, 563);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 12;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(326, 563);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(80, 28);
            this.btn_modify.TabIndex = 13;
            this.btn_modify.Text = "应 用";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_export
            // 
            this.btn_export.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn_export.Location = new System.Drawing.Point(13, 563);
            this.btn_export.Name = "btn_export";
            this.btn_export.Size = new System.Drawing.Size(80, 28);
            this.btn_export.TabIndex = 14;
            this.btn_export.Text = "导 出";
            this.btn_export.UseVisualStyleBackColor = true;
            this.btn_export.Click += new System.EventHandler(this.btn_export_Click);
            // 
            // btn_import
            // 
            this.btn_import.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn_import.Location = new System.Drawing.Point(99, 563);
            this.btn_import.Name = "btn_import";
            this.btn_import.Size = new System.Drawing.Size(80, 28);
            this.btn_import.TabIndex = 15;
            this.btn_import.Text = "导 入";
            this.btn_import.UseVisualStyleBackColor = true;
            this.btn_import.Click += new System.EventHandler(this.btn_import_Click);
            // 
            // btn_restore
            // 
            this.btn_restore.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_restore.Location = new System.Drawing.Point(412, 563);
            this.btn_restore.Name = "btn_restore";
            this.btn_restore.Size = new System.Drawing.Size(80, 28);
            this.btn_restore.TabIndex = 9;
            this.btn_restore.Text = "恢复默认";
            this.btn_restore.UseVisualStyleBackColor = true;
            this.btn_restore.Click += new System.EventHandler(this.btn_restore_Click);
            // 
            // Form_ShortCutManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(590, 611);
            this.Controls.Add(this.btn_import);
            this.Controls.Add(this.btn_export);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.gb_tree);
            this.Controls.Add(this.btn_restore);
            this.Controls.Add(this.btn_ok);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_ShortCutManager";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "自定义快捷键";
            this.gb_tree.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btn_ok;
        private UserCtrl.InputDouble tb_listY;
        private System.Windows.Forms.GroupBox gb_tree;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Panel pan_grid;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_export;
        private System.Windows.Forms.Button btn_import;
        private UserCtrl.ComboBoxKey cbo_group;
        private UserCtrl.InputLookUp tb_filter;
        private System.Windows.Forms.Button btn_restore;
    }
}