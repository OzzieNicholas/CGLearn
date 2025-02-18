namespace UI
{
    partial class Form_PalleteManager
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.tb_listY = new UserCtrl.InputDouble();
            this.btn_apply = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.li_colors = new UI.PalleteColorGrid();
            this.cbo_pallete = new UserCtrl.ComboBoxText();
            this.btn_add = new System.Windows.Forms.Button();
            this.btn_remove = new System.Windows.Forms.Button();
            this.btn_reset = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.li_colors)).BeginInit();
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
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(252, 507);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(80, 28);
            this.btn_apply.TabIndex = 21;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(424, 507);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(80, 28);
            this.btn_cancel.TabIndex = 20;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(166, 507);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(80, 28);
            this.btn_ok.TabIndex = 17;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // li_colors
            // 
            this.li_colors.AllowUserToAddRows = false;
            this.li_colors.AllowUserToDeleteRows = false;
            this.li_colors.AllowUserToResizeRows = false;
            this.li_colors.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.li_colors.BackgroundColor = System.Drawing.Color.White;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.li_colors.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.li_colors.ColumnHeadersHeight = 29;
            this.li_colors.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.li_colors.ColumnHeadersVisible = false;
            this.li_colors.EnableHeadersVisualStyles = false;
            this.li_colors.Location = new System.Drawing.Point(13, 41);
            this.li_colors.MultiSelect = false;
            this.li_colors.Name = "li_colors";
            this.li_colors.RowHeadersVisible = false;
            this.li_colors.RowHeadersWidth = 51;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.li_colors.RowsDefaultCellStyle = dataGridViewCellStyle2;
            this.li_colors.RowTemplate.Height = 23;
            this.li_colors.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.li_colors.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.li_colors.ShowEditingIcon = false;
            this.li_colors.Size = new System.Drawing.Size(489, 460);
            this.li_colors.TabIndex = 23;
            this.li_colors.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.li_colors_CellDoubleClick);
            this.li_colors.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.li_colors_CellEndEdit);
            // 
            // cbo_pallete
            // 
            this.cbo_pallete.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            
            this.cbo_pallete.FormattingEnabled = true;
            this.cbo_pallete.Location = new System.Drawing.Point(13, 12);
            this.cbo_pallete.Name = "cbo_pallete";
            this.cbo_pallete.Size = new System.Drawing.Size(401, 21);
            this.cbo_pallete.TabIndex = 24;
            this.cbo_pallete.SelectionChangeCommitted += new System.EventHandler(this.cbo_pallete_SelectionChangeCommitted);
            // 
            // btn_add
            // 
            this.btn_add.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_add.Location = new System.Drawing.Point(420, 12);
            this.btn_add.Name = "btn_add";
            this.btn_add.Size = new System.Drawing.Size(34, 23);
            this.btn_add.TabIndex = 25;
            this.btn_add.Text = "+";
            this.btn_add.UseVisualStyleBackColor = true;
            this.btn_add.Click += new System.EventHandler(this.btn_add_Click);
            // 
            // btn_remove
            // 
            this.btn_remove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_remove.Location = new System.Drawing.Point(461, 12);
            this.btn_remove.Name = "btn_remove";
            this.btn_remove.Size = new System.Drawing.Size(34, 23);
            this.btn_remove.TabIndex = 25;
            this.btn_remove.Text = "-";
            this.btn_remove.UseVisualStyleBackColor = true;
            this.btn_remove.Click += new System.EventHandler(this.btn_remove_Click);
            // 
            // btn_reset
            // 
            this.btn_reset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_reset.Location = new System.Drawing.Point(338, 507);
            this.btn_reset.Name = "btn_reset";
            this.btn_reset.Size = new System.Drawing.Size(80, 28);
            this.btn_reset.TabIndex = 21;
            this.btn_reset.Text = "重置";
            this.btn_reset.UseVisualStyleBackColor = true;
            this.btn_reset.Click += new System.EventHandler(this.btn_reset_Click);
            // 
            // Form_PalleteManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(516, 552);
            this.Controls.Add(this.li_colors);
            this.Controls.Add(this.btn_remove);
            this.Controls.Add(this.btn_add);
            this.Controls.Add(this.cbo_pallete);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_ok);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(532, 1998);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(532, 498);
            this.Name = "Form_PalleteManager";
            this.Text = " 系统配色";
            this.Controls.SetChildIndex(this.btn_ok, 0);
            this.Controls.SetChildIndex(this.btn_cancel, 0);
            this.Controls.SetChildIndex(this.btn_apply, 0);
            this.Controls.SetChildIndex(this.btn_reset, 0);
            this.Controls.SetChildIndex(this.cbo_pallete, 0);
            this.Controls.SetChildIndex(this.btn_add, 0);
            this.Controls.SetChildIndex(this.btn_remove, 0);
            this.Controls.SetChildIndex(this.li_colors, 0);
            ((System.ComponentModel.ISupportInitialize)(this.li_colors)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private UserCtrl.InputDouble tb_listY;
        private System.Windows.Forms.Button btn_apply;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_ok;
        private UI.PalleteColorGrid li_colors;
        private UserCtrl.ComboBoxText cbo_pallete;
        private System.Windows.Forms.Button btn_add;
        private System.Windows.Forms.Button btn_remove;
        private System.Windows.Forms.Button btn_reset;
        
    }
}