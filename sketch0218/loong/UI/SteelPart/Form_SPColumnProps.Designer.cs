namespace UI
{
    partial class Form_SPColumnProps
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
            this.tc_props = new System.Windows.Forms.TabControl();
            this.tp_1 = new System.Windows.Forms.TabPage();
            this.gb_pos = new System.Windows.Forms.GroupBox();
            this.rb_offset_h = new UserCtrl.RadioButtons();
            this.rb_rot = new UserCtrl.RadioButtons();
            this.rb_offset_v = new UserCtrl.RadioButtons();
            this.chb_offset_h = new UserCtrl.Check();
            this.chb_rot = new UserCtrl.Check();
            this.tb_rot_angl = new UserCtrl.InputDouble();
            this.chb_offset_v = new UserCtrl.Check();
            this.tb_offset_h = new UserCtrl.InputDouble();
            this.tb_offset_v = new UserCtrl.InputDouble();
            this.gb_deform = new System.Windows.Forms.GroupBox();
            this.tb_deform_end = new UserCtrl.InputDouble();
            this.chb_deform_end = new UserCtrl.Check();
            this.tb_deform_start = new UserCtrl.InputDouble();
            this.chb_deform_start = new UserCtrl.Check();
            this.gb_height = new System.Windows.Forms.GroupBox();
            this.tb_bottom = new UserCtrl.InputDouble();
            this.chb_bottom = new UserCtrl.Check();
            this.tb_top = new UserCtrl.InputDouble();
            this.chb_top = new UserCtrl.Check();
            this.gb_props = new System.Windows.Forms.GroupBox();
            this.cbo_class = new UserCtrl.Combo();
            this.chb_class = new UserCtrl.Check();
            this.tb_finish = new UserCtrl.InputText();
            this.chb_polish = new UserCtrl.Check();
            this.btn_mtrl = new System.Windows.Forms.Button();
            this.tb_mtrl = new UserCtrl.InputText();
            this.chb_mtrl = new UserCtrl.Check();
            this.btn_profile = new System.Windows.Forms.Button();
            this.tb_profile = new UserCtrl.InputText();
            this.tb_name = new UserCtrl.InputText();
            this.chb_profile = new UserCtrl.Check();
            this.chb_name = new UserCtrl.Check();
            this.gb_no = new System.Windows.Forms.GroupBox();
            this.tb_assembly_no_start = new UserCtrl.InputText();
            this.tb_no_start = new UserCtrl.InputText();
            this.tb_assembly_no_prefix = new UserCtrl.InputText();
            this.tb_no_prefix = new UserCtrl.InputText();
            this.chb_assembly_noStart = new UserCtrl.Check();
            this.chb_part_noStart = new UserCtrl.Check();
            this.chb_assembly_no = new UserCtrl.Check();
            this.chb_part_no = new UserCtrl.Check();
            this.tp_3 = new System.Windows.Forms.TabPage();
            this.btn_ok = new System.Windows.Forms.Button();
            this.btn_modify = new System.Windows.Forms.Button();
            this.btn_pick = new System.Windows.Forms.Button();
            this.btn_cancel = new System.Windows.Forms.Button();
            this.btn_apply = new System.Windows.Forms.Button();
            this.btn_chb = new System.Windows.Forms.Button();
            this.bar_cfg = new UI.CommonCfgBar();
            this.tc_props.SuspendLayout();
            this.tp_1.SuspendLayout();
            this.gb_pos.SuspendLayout();
            this.gb_deform.SuspendLayout();
            this.gb_height.SuspendLayout();
            this.gb_props.SuspendLayout();
            this.gb_no.SuspendLayout();
            this.SuspendLayout();
            // 
            // tc_props
            // 
            this.tc_props.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tc_props.Controls.Add(this.tp_1);
            this.tc_props.Controls.Add(this.tp_3);
            this.tc_props.Location = new System.Drawing.Point(6, 45);
            this.tc_props.Name = "tc_props";
            this.tc_props.SelectedIndex = 0;
            this.tc_props.Size = new System.Drawing.Size(395, 489);
            this.tc_props.TabIndex = 0;
            // 
            // tp_1
            // 
            this.tp_1.Controls.Add(this.gb_pos);
            this.tp_1.Controls.Add(this.gb_deform);
            this.tp_1.Controls.Add(this.gb_height);
            this.tp_1.Controls.Add(this.gb_props);
            this.tp_1.Controls.Add(this.gb_no);
            this.tp_1.Location = new System.Drawing.Point(4, 22);
            this.tp_1.Name = "tp_1";
            this.tp_1.Padding = new System.Windows.Forms.Padding(3);
            this.tp_1.Size = new System.Drawing.Size(387, 463);
            this.tp_1.TabIndex = 0;
            this.tp_1.Text = "属性";
            this.tp_1.UseVisualStyleBackColor = true;
            // 
            // gb_pos
            // 
            this.gb_pos.Controls.Add(this.rb_offset_h);
            this.gb_pos.Controls.Add(this.rb_rot);
            this.gb_pos.Controls.Add(this.rb_offset_v);
            this.gb_pos.Controls.Add(this.chb_offset_h);
            this.gb_pos.Controls.Add(this.chb_rot);
            this.gb_pos.Controls.Add(this.tb_rot_angl);
            this.gb_pos.Controls.Add(this.chb_offset_v);
            this.gb_pos.Controls.Add(this.tb_offset_h);
            this.gb_pos.Controls.Add(this.tb_offset_v);
            this.gb_pos.Location = new System.Drawing.Point(6, 260);
            this.gb_pos.Name = "gb_pos";
            this.gb_pos.Size = new System.Drawing.Size(372, 110);
            this.gb_pos.TabIndex = 86;
            this.gb_pos.TabStop = false;
            this.gb_pos.Text = "位置";
            // 
            // rb_offset_h
            // 
            this.rb_offset_h.BackColor = System.Drawing.SystemColors.Control;
            this.rb_offset_h.btnCount = 3;
            this.rb_offset_h.btnSize = new System.Drawing.Size(42, 21);
            this.rb_offset_h.CheckName = "chb_offset_h";
            this.rb_offset_h.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_offset_h.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_offset_h.keys = new object[] {
        ((object)(0))};
            this.rb_offset_h.keys1 = new object[] {
        ((object)(0))};
            this.rb_offset_h.labels = new string[] {
        ""};
            this.rb_offset_h.Location = new System.Drawing.Point(59, 20);
            this.rb_offset_h.Margin = new System.Windows.Forms.Padding(0);
            this.rb_offset_h.Name = "rb_offset_h";
            this.rb_offset_h.PropName = "work_transform.offset_h_base";
            this.rb_offset_h.Size = new System.Drawing.Size(132, 21);
            this.rb_offset_h.TabIndex = 86;
            // 
            // rb_rot
            // 
            this.rb_rot.BackColor = System.Drawing.SystemColors.Control;
            this.rb_rot.btnCount = 4;
            this.rb_rot.btnSize = new System.Drawing.Size(42, 21);
            this.rb_rot.CheckName = "chb_rot";
            this.rb_rot.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_rot.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_rot.keys = new object[] {
        ((object)(0))};
            this.rb_rot.keys1 = new object[] {
        ((object)(0))};
            this.rb_rot.labels = new string[] {
        ""};
            this.rb_rot.Location = new System.Drawing.Point(59, 47);
            this.rb_rot.Margin = new System.Windows.Forms.Padding(0);
            this.rb_rot.Name = "rb_rot";
            this.rb_rot.PropName = "work_transform.rot_base";
            this.rb_rot.Size = new System.Drawing.Size(177, 21);
            this.rb_rot.TabIndex = 86;
            // 
            // rb_offset_v
            // 
            this.rb_offset_v.BackColor = System.Drawing.SystemColors.Control;
            this.rb_offset_v.btnCount = 3;
            this.rb_offset_v.btnSize = new System.Drawing.Size(42, 21);
            this.rb_offset_v.CheckName = "chb_offset_v";
            this.rb_offset_v.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rb_offset_v.ForeColor = System.Drawing.Color.SlateGray;
            this.rb_offset_v.keys = new object[] {
        ((object)(0))};
            this.rb_offset_v.keys1 = new object[] {
        ((object)(0))};
            this.rb_offset_v.labels = new string[] {
        ""};
            this.rb_offset_v.Location = new System.Drawing.Point(59, 74);
            this.rb_offset_v.Margin = new System.Windows.Forms.Padding(0);
            this.rb_offset_v.Name = "rb_offset_v";
            this.rb_offset_v.PropName = "work_transform.offset_v_base";
            this.rb_offset_v.Size = new System.Drawing.Size(132, 21);
            this.rb_offset_v.TabIndex = 86;
            // 
            // chb_offset_h
            // 
            this.chb_offset_h.AutoSize = true;
            this.chb_offset_h.Checked = true;
            this.chb_offset_h.CheckName = null;
            this.chb_offset_h.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_offset_h.Location = new System.Drawing.Point(8, 23);
            this.chb_offset_h.Name = "chb_offset_h";
            this.chb_offset_h.PropName = null;
            this.chb_offset_h.Size = new System.Drawing.Size(48, 16);
            this.chb_offset_h.TabIndex = 10;
            this.chb_offset_h.TabStop = false;
            this.chb_offset_h.Text = "横向";
            this.chb_offset_h.UseVisualStyleBackColor = true;
            // 
            // chb_rot
            // 
            this.chb_rot.AutoSize = true;
            this.chb_rot.Checked = true;
            this.chb_rot.CheckName = null;
            this.chb_rot.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_rot.Location = new System.Drawing.Point(8, 49);
            this.chb_rot.Name = "chb_rot";
            this.chb_rot.PropName = null;
            this.chb_rot.Size = new System.Drawing.Size(48, 16);
            this.chb_rot.TabIndex = 8;
            this.chb_rot.TabStop = false;
            this.chb_rot.Text = "旋转";
            this.chb_rot.UseVisualStyleBackColor = true;
            // 
            // tb_rot_angl
            // 
            this.tb_rot_angl.CheckName = "chb_rot";
            this.tb_rot_angl.DefValue = 0D;
            this.tb_rot_angl.Enob = 5;
            this.tb_rot_angl.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_rot_angl.Location = new System.Drawing.Point(246, 47);
            this.tb_rot_angl.Name = "tb_rot_angl";
            this.tb_rot_angl.PropName = "work_transform.rot_angl";
            this.tb_rot_angl.Size = new System.Drawing.Size(113, 21);
            this.tb_rot_angl.TabIndex = 40;
            this.tb_rot_angl.Text = "0";
            this.tb_rot_angl.unitData = "deg";
            this.tb_rot_angl.unitDisp = "deg";
            this.tb_rot_angl.unitEnobType = "model_angl";
            this.tb_rot_angl.Value = 0D;
            // 
            // chb_offset_v
            // 
            this.chb_offset_v.AutoSize = true;
            this.chb_offset_v.Checked = true;
            this.chb_offset_v.CheckName = null;
            this.chb_offset_v.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_offset_v.Location = new System.Drawing.Point(8, 77);
            this.chb_offset_v.Name = "chb_offset_v";
            this.chb_offset_v.PropName = null;
            this.chb_offset_v.Size = new System.Drawing.Size(48, 16);
            this.chb_offset_v.TabIndex = 6;
            this.chb_offset_v.TabStop = false;
            this.chb_offset_v.Text = "竖向";
            this.chb_offset_v.UseVisualStyleBackColor = true;
            // 
            // tb_offset_h
            // 
            this.tb_offset_h.CheckName = "chb_offset_h";
            this.tb_offset_h.DefValue = 0D;
            this.tb_offset_h.Enob = 5;
            this.tb_offset_h.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_offset_h.Location = new System.Drawing.Point(246, 20);
            this.tb_offset_h.Name = "tb_offset_h";
            this.tb_offset_h.PropName = "work_transform.offset_h";
            this.tb_offset_h.Size = new System.Drawing.Size(113, 21);
            this.tb_offset_h.TabIndex = 20;
            this.tb_offset_h.Text = "0";
            this.tb_offset_h.unitEnobType = "model_dist";
            this.tb_offset_h.Value = 0D;
            // 
            // tb_offset_v
            // 
            this.tb_offset_v.CheckName = "chb_offset_v";
            this.tb_offset_v.DefValue = 0D;
            this.tb_offset_v.Enob = 5;
            this.tb_offset_v.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_offset_v.Location = new System.Drawing.Point(246, 74);
            this.tb_offset_v.Name = "tb_offset_v";
            this.tb_offset_v.PropName = "work_transform.offset_v";
            this.tb_offset_v.Size = new System.Drawing.Size(113, 21);
            this.tb_offset_v.TabIndex = 60;
            this.tb_offset_v.Text = "0";
            this.tb_offset_v.unitEnobType = "model_dist";
            this.tb_offset_v.Value = 0D;
            // 
            // gb_deform
            // 
            this.gb_deform.Controls.Add(this.tb_deform_end);
            this.gb_deform.Controls.Add(this.chb_deform_end);
            this.gb_deform.Controls.Add(this.tb_deform_start);
            this.gb_deform.Controls.Add(this.chb_deform_start);
            this.gb_deform.Location = new System.Drawing.Point(203, 376);
            this.gb_deform.Name = "gb_deform";
            this.gb_deform.Size = new System.Drawing.Size(175, 80);
            this.gb_deform.TabIndex = 85;
            this.gb_deform.TabStop = false;
            this.gb_deform.Text = "扭转";
            // 
            // tb_deform_end
            // 
            this.tb_deform_end.CheckName = "chb_deform_end";
            this.tb_deform_end.DefValue = 0D;
            this.tb_deform_end.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_end.Location = new System.Drawing.Point(49, 46);
            this.tb_deform_end.Max = 10000D;
            this.tb_deform_end.Min = -10000D;
            this.tb_deform_end.Name = "tb_deform_end";
            this.tb_deform_end.PropName = "work_transform.deform_end";
            this.tb_deform_end.Size = new System.Drawing.Size(113, 21);
            this.tb_deform_end.TabIndex = 26;
            this.tb_deform_end.Text = "0";
            this.tb_deform_end.unitData = "deg";
            this.tb_deform_end.unitDisp = "deg";
            this.tb_deform_end.unitEnobType = "model_angl";
            this.tb_deform_end.Value = 0D;
            // 
            // chb_deform_end
            // 
            this.chb_deform_end.AutoSize = true;
            this.chb_deform_end.Checked = true;
            this.chb_deform_end.CheckName = null;
            this.chb_deform_end.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_end.Location = new System.Drawing.Point(10, 48);
            this.chb_deform_end.Name = "chb_deform_end";
            this.chb_deform_end.PropName = null;
            this.chb_deform_end.Size = new System.Drawing.Size(36, 16);
            this.chb_deform_end.TabIndex = 28;
            this.chb_deform_end.TabStop = false;
            this.chb_deform_end.Text = "尾";
            this.chb_deform_end.UseVisualStyleBackColor = true;
            // 
            // tb_deform_start
            // 
            this.tb_deform_start.CheckName = "chb_deform_start";
            this.tb_deform_start.DefValue = 0D;
            this.tb_deform_start.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_deform_start.Location = new System.Drawing.Point(49, 19);
            this.tb_deform_start.Max = 10000D;
            this.tb_deform_start.Min = -10000D;
            this.tb_deform_start.Name = "tb_deform_start";
            this.tb_deform_start.PropName = "work_transform.deform_start";
            this.tb_deform_start.Size = new System.Drawing.Size(113, 21);
            this.tb_deform_start.TabIndex = 25;
            this.tb_deform_start.Text = "0";
            this.tb_deform_start.unitData = "deg";
            this.tb_deform_start.unitDisp = "deg";
            this.tb_deform_start.unitEnobType = "model_angl";
            this.tb_deform_start.Value = 0D;
            // 
            // chb_deform_start
            // 
            this.chb_deform_start.AutoSize = true;
            this.chb_deform_start.Checked = true;
            this.chb_deform_start.CheckName = null;
            this.chb_deform_start.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_deform_start.Location = new System.Drawing.Point(10, 21);
            this.chb_deform_start.Name = "chb_deform_start";
            this.chb_deform_start.PropName = null;
            this.chb_deform_start.Size = new System.Drawing.Size(36, 16);
            this.chb_deform_start.TabIndex = 27;
            this.chb_deform_start.TabStop = false;
            this.chb_deform_start.Text = "头";
            this.chb_deform_start.UseVisualStyleBackColor = true;
            // 
            // gb_height
            // 
            this.gb_height.Controls.Add(this.tb_bottom);
            this.gb_height.Controls.Add(this.chb_bottom);
            this.gb_height.Controls.Add(this.tb_top);
            this.gb_height.Controls.Add(this.chb_top);
            this.gb_height.Location = new System.Drawing.Point(6, 376);
            this.gb_height.Name = "gb_height";
            this.gb_height.Size = new System.Drawing.Size(191, 80);
            this.gb_height.TabIndex = 84;
            this.gb_height.TabStop = false;
            this.gb_height.Text = "高度";
            // 
            // tb_bottom
            // 
            this.tb_bottom.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_bottom.CheckName = "chb_bottom";
            this.tb_bottom.DefValue = 0D;
            this.tb_bottom.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_bottom.Location = new System.Drawing.Point(70, 47);
            this.tb_bottom.Name = "tb_bottom";
            this.tb_bottom.PropName = "bottom";
            this.tb_bottom.Size = new System.Drawing.Size(107, 21);
            this.tb_bottom.TabIndex = 80;
            this.tb_bottom.Text = "0";
            this.tb_bottom.unitEnobType = "model_dist";
            this.tb_bottom.Value = 0D;
            // 
            // chb_bottom
            // 
            this.chb_bottom.AutoSize = true;
            this.chb_bottom.Checked = true;
            this.chb_bottom.CheckName = null;
            this.chb_bottom.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_bottom.Location = new System.Drawing.Point(10, 49);
            this.chb_bottom.Name = "chb_bottom";
            this.chb_bottom.PropName = null;
            this.chb_bottom.Size = new System.Drawing.Size(48, 16);
            this.chb_bottom.TabIndex = 8;
            this.chb_bottom.TabStop = false;
            this.chb_bottom.Text = "底面";
            this.chb_bottom.UseVisualStyleBackColor = true;
            // 
            // tb_top
            // 
            this.tb_top.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_top.CheckName = "chb_top";
            this.tb_top.DefValue = 0D;
            this.tb_top.Font = new System.Drawing.Font("宋体", 9F);
            this.tb_top.Location = new System.Drawing.Point(70, 20);
            this.tb_top.Name = "tb_top";
            this.tb_top.PropName = "top";
            this.tb_top.Size = new System.Drawing.Size(107, 21);
            this.tb_top.TabIndex = 70;
            this.tb_top.Text = "0";
            this.tb_top.unitEnobType = "model_dist";
            this.tb_top.Value = 0D;
            // 
            // chb_top
            // 
            this.chb_top.AutoSize = true;
            this.chb_top.Checked = true;
            this.chb_top.CheckName = null;
            this.chb_top.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_top.Location = new System.Drawing.Point(10, 22);
            this.chb_top.Name = "chb_top";
            this.chb_top.PropName = null;
            this.chb_top.Size = new System.Drawing.Size(48, 16);
            this.chb_top.TabIndex = 6;
            this.chb_top.TabStop = false;
            this.chb_top.Text = "顶面";
            this.chb_top.UseVisualStyleBackColor = true;
            // 
            // gb_props
            // 
            this.gb_props.Controls.Add(this.cbo_class);
            this.gb_props.Controls.Add(this.chb_class);
            this.gb_props.Controls.Add(this.tb_finish);
            this.gb_props.Controls.Add(this.chb_polish);
            this.gb_props.Controls.Add(this.btn_mtrl);
            this.gb_props.Controls.Add(this.tb_mtrl);
            this.gb_props.Controls.Add(this.chb_mtrl);
            this.gb_props.Controls.Add(this.btn_profile);
            this.gb_props.Controls.Add(this.tb_profile);
            this.gb_props.Controls.Add(this.tb_name);
            this.gb_props.Controls.Add(this.chb_profile);
            this.gb_props.Controls.Add(this.chb_name);
            this.gb_props.Location = new System.Drawing.Point(6, 94);
            this.gb_props.Name = "gb_props";
            this.gb_props.Size = new System.Drawing.Size(372, 160);
            this.gb_props.TabIndex = 5;
            this.gb_props.TabStop = false;
            this.gb_props.Text = "信息";
            // 
            // cbo_class
            // 
            this.cbo_class.CheckName = "chb_class";
            this.cbo_class.DisplayMember = "";
            this.cbo_class.focusColor = System.Drawing.Color.FromArgb(((int)(((byte)(225)))), ((int)(((byte)(225)))), ((int)(((byte)(235)))));
            this.cbo_class.Font = new System.Drawing.Font("宋体", 9F);
            this.cbo_class.FormattingEnabled = true;
            this.cbo_class.ImageBorder = new System.Windows.Forms.Padding(0);
            this.cbo_class.ImageItemSize = new System.Drawing.Size(0, 0);
            this.cbo_class.KeyBrowse = true;
            this.cbo_class.Location = new System.Drawing.Point(87, 128);
            this.cbo_class.Name = "cbo_class";
            this.cbo_class.PropName = "class";
            
            this.cbo_class.Size = new System.Drawing.Size(267, 21);
            this.cbo_class.TabIndex = 101;
            this.cbo_class.Text = "-1";
            this.cbo_class.ValueMember = "";
            // 
            // chb_class
            // 
            this.chb_class.AutoSize = true;
            this.chb_class.Checked = true;
            this.chb_class.CheckName = null;
            this.chb_class.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_class.Location = new System.Drawing.Point(10, 130);
            this.chb_class.Name = "chb_class";
            this.chb_class.PropName = null;
            this.chb_class.Size = new System.Drawing.Size(48, 16);
            this.chb_class.TabIndex = 24;
            this.chb_class.TabStop = false;
            this.chb_class.Text = "等级";
            this.chb_class.UseVisualStyleBackColor = true;
            // 
            // tb_finish
            // 
            this.tb_finish.CheckName = "chb_polish";
            this.tb_finish.Location = new System.Drawing.Point(87, 101);
            this.tb_finish.Name = "tb_finish";
            this.tb_finish.PropName = "finish";
            this.tb_finish.Size = new System.Drawing.Size(267, 21);
            this.tb_finish.TabIndex = 100;
            this.tb_finish.Trim = false;
            // 
            // chb_polish
            // 
            this.chb_polish.AutoSize = true;
            this.chb_polish.Checked = true;
            this.chb_polish.CheckName = null;
            this.chb_polish.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_polish.Location = new System.Drawing.Point(10, 103);
            this.chb_polish.Name = "chb_polish";
            this.chb_polish.PropName = null;
            this.chb_polish.Size = new System.Drawing.Size(48, 16);
            this.chb_polish.TabIndex = 22;
            this.chb_polish.TabStop = false;
            this.chb_polish.Text = "抛光";
            this.chb_polish.UseVisualStyleBackColor = true;
            // 
            // btn_mtrl
            // 
            this.btn_mtrl.Location = new System.Drawing.Point(301, 72);
            this.btn_mtrl.Name = "btn_mtrl";
            this.btn_mtrl.Size = new System.Drawing.Size(54, 23);
            this.btn_mtrl.TabIndex = 90;
            this.btn_mtrl.TabStop = false;
            this.btn_mtrl.Text = "选择";
            this.btn_mtrl.UseVisualStyleBackColor = true;
            this.btn_mtrl.Click += new System.EventHandler(this.btn_mtrl_Click);
            // 
            // tb_mtrl
            // 
            this.tb_mtrl.CheckName = "chb_mtrl";
            this.tb_mtrl.FilterChars = "$";
            this.tb_mtrl.Location = new System.Drawing.Point(87, 74);
            this.tb_mtrl.Name = "tb_mtrl";
            this.tb_mtrl.PropName = "mtrl";
            this.tb_mtrl.Size = new System.Drawing.Size(206, 21);
            this.tb_mtrl.TabIndex = 80;
            this.tb_mtrl.Trim = false;
            // 
            // chb_mtrl
            // 
            this.chb_mtrl.AutoSize = true;
            this.chb_mtrl.Checked = true;
            this.chb_mtrl.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_mtrl.Location = new System.Drawing.Point(10, 76);
            this.chb_mtrl.Name = "chb_mtrl";
            this.chb_mtrl.PropName = null;
            this.chb_mtrl.Size = new System.Drawing.Size(48, 16);
            this.chb_mtrl.TabIndex = 19;
            this.chb_mtrl.TabStop = false;
            this.chb_mtrl.Text = "材质";
            this.chb_mtrl.UseVisualStyleBackColor = true;
            // 
            // btn_profile
            // 
            this.btn_profile.Location = new System.Drawing.Point(301, 45);
            this.btn_profile.Name = "btn_profile";
            this.btn_profile.Size = new System.Drawing.Size(54, 23);
            this.btn_profile.TabIndex = 70;
            this.btn_profile.TabStop = false;
            this.btn_profile.Text = "选择";
            this.btn_profile.UseVisualStyleBackColor = true;
            this.btn_profile.Click += new System.EventHandler(this.btn_profile_Click);
            // 
            // tb_profile
            // 
            this.tb_profile.CheckName = "chb_profile";
            this.tb_profile.FilterChars = "$";
            this.tb_profile.Location = new System.Drawing.Point(87, 47);
            this.tb_profile.Name = "tb_profile";
            this.tb_profile.PropName = "profile";
            this.tb_profile.Size = new System.Drawing.Size(206, 21);
            this.tb_profile.TabIndex = 60;
            this.tb_profile.Trim = false;
            // 
            // tb_name
            // 
            this.tb_name.CheckName = "chb_name";
            this.tb_name.Location = new System.Drawing.Point(87, 20);
            this.tb_name.Name = "tb_name";
            this.tb_name.PropName = "name";
            this.tb_name.Size = new System.Drawing.Size(267, 21);
            this.tb_name.TabIndex = 50;
            this.tb_name.Trim = false;
            // 
            // chb_profile
            // 
            this.chb_profile.AutoSize = true;
            this.chb_profile.Checked = true;
            this.chb_profile.CheckName = null;
            this.chb_profile.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_profile.Location = new System.Drawing.Point(10, 49);
            this.chb_profile.Name = "chb_profile";
            this.chb_profile.PropName = null;
            this.chb_profile.Size = new System.Drawing.Size(72, 16);
            this.chb_profile.TabIndex = 8;
            this.chb_profile.TabStop = false;
            this.chb_profile.Text = "截面型材";
            this.chb_profile.UseVisualStyleBackColor = true;
            // 
            // chb_name
            // 
            this.chb_name.AutoSize = true;
            this.chb_name.Checked = true;
            this.chb_name.CheckName = null;
            this.chb_name.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_name.Location = new System.Drawing.Point(10, 22);
            this.chb_name.Name = "chb_name";
            this.chb_name.PropName = null;
            this.chb_name.Size = new System.Drawing.Size(48, 16);
            this.chb_name.TabIndex = 6;
            this.chb_name.TabStop = false;
            this.chb_name.Text = "名称";
            this.chb_name.UseVisualStyleBackColor = true;
            // 
            // gb_no
            // 
            this.gb_no.Controls.Add(this.tb_assembly_no_start);
            this.gb_no.Controls.Add(this.tb_no_start);
            this.gb_no.Controls.Add(this.tb_assembly_no_prefix);
            this.gb_no.Controls.Add(this.tb_no_prefix);
            this.gb_no.Controls.Add(this.chb_assembly_noStart);
            this.gb_no.Controls.Add(this.chb_part_noStart);
            this.gb_no.Controls.Add(this.chb_assembly_no);
            this.gb_no.Controls.Add(this.chb_part_no);
            this.gb_no.Location = new System.Drawing.Point(6, 6);
            this.gb_no.Name = "gb_no";
            this.gb_no.Size = new System.Drawing.Size(372, 82);
            this.gb_no.TabIndex = 4;
            this.gb_no.TabStop = false;
            this.gb_no.Text = "编号";
            // 
            // tb_assembly_no_start
            // 
            this.tb_assembly_no_start.CheckName = "chb_assembly_noStart";
            this.tb_assembly_no_start.Location = new System.Drawing.Point(237, 49);
            this.tb_assembly_no_start.Name = "tb_assembly_no_start";
            this.tb_assembly_no_start.PropName = "ass_no_start_postfix";
            this.tb_assembly_no_start.Size = new System.Drawing.Size(118, 21);
            this.tb_assembly_no_start.TabIndex = 40;
            this.tb_assembly_no_start.Text = "0";
            this.tb_assembly_no_start.Trim = false;
            this.tb_assembly_no_start.Value = "0";
            this.tb_assembly_no_start.placeholder = "起始编号";
            // 
            // tb_no_start
            // 
            this.tb_no_start.CheckName = "chb_part_noStart";
            this.tb_no_start.Location = new System.Drawing.Point(237, 20);
            this.tb_no_start.Name = "tb_no_start";
            this.tb_no_start.PropName = "no_start_postfix";
            this.tb_no_start.Size = new System.Drawing.Size(118, 21);
            this.tb_no_start.TabIndex = 20;
            this.tb_no_start.Text = "0";
            this.tb_no_start.Trim = false;
            this.tb_no_start.Value = "0";
            this.tb_no_start.placeholder = "  起始编号";
            // 
            // tb_assembly_no_prefix
            // 
            this.tb_assembly_no_prefix.CheckName = "chb_assembly_no";
            this.tb_assembly_no_prefix.Location = new System.Drawing.Point(87, 49);
            this.tb_assembly_no_prefix.Name = "tb_assembly_no_prefix";
            this.tb_assembly_no_prefix.PropName = "ass_no_prefix";
            this.tb_assembly_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_assembly_no_prefix.TabIndex = 30;
            this.tb_assembly_no_prefix.Trim = false;
            this.tb_assembly_no_prefix.placeholder = "  前缀";
            // 
            // tb_no_prefix
            // 
            this.tb_no_prefix.CheckName = "chb_part_no";
            this.tb_no_prefix.Location = new System.Drawing.Point(87, 20);
            this.tb_no_prefix.Name = "tb_no_prefix";
            this.tb_no_prefix.PropName = "no_prefix";
            this.tb_no_prefix.Size = new System.Drawing.Size(115, 21);
            this.tb_no_prefix.TabIndex = 10;
            this.tb_no_prefix.Trim = false;
            this.tb_no_prefix.placeholder = "  前缀";
            // 
            // chb_assembly_noStart
            // 
            this.chb_assembly_noStart.AutoSize = true;
            this.chb_assembly_noStart.Checked = true;
            this.chb_assembly_noStart.CheckName = null;
            this.chb_assembly_noStart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_assembly_noStart.Location = new System.Drawing.Point(214, 52);
            this.chb_assembly_noStart.Name = "chb_assembly_noStart";
            this.chb_assembly_noStart.PropName = null;
            this.chb_assembly_noStart.Size = new System.Drawing.Size(15, 14);
            this.chb_assembly_noStart.TabIndex = 14;
            this.chb_assembly_noStart.TabStop = false;
            this.chb_assembly_noStart.UseVisualStyleBackColor = true;
            // 
            // chb_part_noStart
            // 
            this.chb_part_noStart.AutoSize = true;
            this.chb_part_noStart.Checked = true;
            this.chb_part_noStart.CheckName = null;
            this.chb_part_noStart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_noStart.Location = new System.Drawing.Point(214, 25);
            this.chb_part_noStart.Name = "chb_part_noStart";
            this.chb_part_noStart.PropName = null;
            this.chb_part_noStart.Size = new System.Drawing.Size(15, 14);
            this.chb_part_noStart.TabIndex = 12;
            this.chb_part_noStart.TabStop = false;
            this.chb_part_noStart.UseVisualStyleBackColor = true;
            // 
            // chb_assembly_no
            // 
            this.chb_assembly_no.AutoSize = true;
            this.chb_assembly_no.Checked = true;
            this.chb_assembly_no.CheckName = null;
            this.chb_assembly_no.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_assembly_no.Location = new System.Drawing.Point(10, 49);
            this.chb_assembly_no.Name = "chb_assembly_no";
            this.chb_assembly_no.PropName = null;
            this.chb_assembly_no.Size = new System.Drawing.Size(48, 16);
            this.chb_assembly_no.TabIndex = 8;
            this.chb_assembly_no.TabStop = false;
            this.chb_assembly_no.Text = "构件";
            this.chb_assembly_no.UseVisualStyleBackColor = true;
            // 
            // chb_part_no
            // 
            this.chb_part_no.AutoSize = true;
            this.chb_part_no.Checked = true;
            this.chb_part_no.CheckName = null;
            this.chb_part_no.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chb_part_no.Location = new System.Drawing.Point(10, 22);
            this.chb_part_no.Name = "chb_part_no";
            this.chb_part_no.PropName = null;
            this.chb_part_no.Size = new System.Drawing.Size(48, 16);
            this.chb_part_no.TabIndex = 6;
            this.chb_part_no.TabStop = false;
            this.chb_part_no.Text = "零件";
            this.chb_part_no.UseVisualStyleBackColor = true;
            // 
            // tp_3
            // 
            this.tp_3.Location = new System.Drawing.Point(4, 22);
            this.tp_3.Name = "tp_3";
            this.tp_3.Size = new System.Drawing.Size(387, 463);
            this.tp_3.TabIndex = 3;
            this.tp_3.Text = "用户属性";
            this.tp_3.UseVisualStyleBackColor = true;
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(17, 540);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(58, 28);
            this.btn_ok.TabIndex = 500;
            this.btn_ok.TabStop = false;
            this.btn_ok.Text = "确 认";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // btn_modify
            // 
            this.btn_modify.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_modify.Location = new System.Drawing.Point(147, 540);
            this.btn_modify.Name = "btn_modify";
            this.btn_modify.Size = new System.Drawing.Size(58, 28);
            this.btn_modify.TabIndex = 520;
            this.btn_modify.TabStop = false;
            this.btn_modify.Text = "修 改";
            this.btn_modify.UseVisualStyleBackColor = true;
            this.btn_modify.Click += new System.EventHandler(this.btn_modify_Click);
            // 
            // btn_pick
            // 
            this.btn_pick.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_pick.Location = new System.Drawing.Point(211, 540);
            this.btn_pick.Name = "btn_pick";
            this.btn_pick.Size = new System.Drawing.Size(58, 28);
            this.btn_pick.TabIndex = 530;
            this.btn_pick.TabStop = false;
            this.btn_pick.Text = "获 取";
            this.btn_pick.UseVisualStyleBackColor = true;
            this.btn_pick.Click += new System.EventHandler(this.btn_pick_Click);
            // 
            // btn_cancel
            // 
            this.btn_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_cancel.Location = new System.Drawing.Point(339, 540);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(58, 28);
            this.btn_cancel.TabIndex = 550;
            this.btn_cancel.TabStop = false;
            this.btn_cancel.Text = "取 消";
            this.btn_cancel.UseVisualStyleBackColor = true;
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // btn_apply
            // 
            this.btn_apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_apply.Location = new System.Drawing.Point(81, 540);
            this.btn_apply.Name = "btn_apply";
            this.btn_apply.Size = new System.Drawing.Size(58, 28);
            this.btn_apply.TabIndex = 510;
            this.btn_apply.TabStop = false;
            this.btn_apply.Text = "应 用";
            this.btn_apply.UseVisualStyleBackColor = true;
            this.btn_apply.Click += new System.EventHandler(this.btn_apply_Click);
            // 
            // btn_chb
            // 
            this.btn_chb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_chb.BackgroundImage = global::UI.Properties.Resources.btn_check;
            this.btn_chb.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btn_chb.Location = new System.Drawing.Point(275, 540);
            this.btn_chb.Name = "btn_chb";
            this.btn_chb.Size = new System.Drawing.Size(58, 28);
            this.btn_chb.TabIndex = 540;
            this.btn_chb.TabStop = false;
            this.btn_chb.UseVisualStyleBackColor = true;
            this.btn_chb.Click += new System.EventHandler(this.btn_chb_Click);
            // 
            // bar_cfg
            // 
            this.bar_cfg.clientProps = null;
            this.bar_cfg.Location = new System.Drawing.Point(6, 3);
            this.bar_cfg.Margin = new System.Windows.Forms.Padding(4);
            this.bar_cfg.MinimumSize = new System.Drawing.Size(214, 0);
            this.bar_cfg.Name = "bar_cfg";
            this.bar_cfg.selectedCfg = null;
            this.bar_cfg.Size = new System.Drawing.Size(399, 33);
            this.bar_cfg.TabIndex = 551;
            this.bar_cfg.TabStop = false;
            // 
            // Form_SPColumnProps
            // 
            this.AcceptButton = this.btn_modify;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(406, 590);
            this.Controls.Add(this.bar_cfg);
            this.Controls.Add(this.btn_chb);
            this.Controls.Add(this.btn_apply);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_pick);
            this.Controls.Add(this.btn_modify);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.tc_props);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(422, 9999);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(422, 425);
            this.Name = "Form_SPColumnProps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = " 柱属性";
            this.tc_props.ResumeLayout(false);
            this.tp_1.ResumeLayout(false);
            this.gb_pos.ResumeLayout(false);
            this.gb_pos.PerformLayout();
            this.gb_deform.ResumeLayout(false);
            this.gb_deform.PerformLayout();
            this.gb_height.ResumeLayout(false);
            this.gb_height.PerformLayout();
            this.gb_props.ResumeLayout(false);
            this.gb_props.PerformLayout();
            this.gb_no.ResumeLayout(false);
            this.gb_no.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TabControl tc_props;
        private System.Windows.Forms.TabPage tp_1;
        private System.Windows.Forms.GroupBox gb_no;
        private UserCtrl.Check chb_assembly_noStart;
        private UserCtrl.Check chb_part_noStart;
        private UserCtrl.Check chb_assembly_no;
        private UserCtrl.Check chb_part_no;
        private UserCtrl.InputText tb_assembly_no_prefix;
        private UserCtrl.InputText tb_no_prefix;
        private UserCtrl.InputText tb_assembly_no_start;
        private UserCtrl.InputText tb_no_start;
        private System.Windows.Forms.GroupBox gb_props;
        private UserCtrl.InputText tb_profile;
        private UserCtrl.InputText tb_name;
        private UserCtrl.Check chb_profile;
        private UserCtrl.Check chb_name;
        private System.Windows.Forms.Button btn_profile;
        private System.Windows.Forms.Button btn_mtrl;
        private UserCtrl.InputText tb_mtrl;
        private UserCtrl.Check chb_mtrl;
        private UserCtrl.InputText tb_finish;
        private UserCtrl.Check chb_polish;
        private UserCtrl.Check chb_class;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.Button btn_modify;
        private System.Windows.Forms.Button btn_pick;
        private System.Windows.Forms.Button btn_cancel;
        private System.Windows.Forms.Button btn_apply;
        private System.Windows.Forms.TabPage tp_3;
        private System.Windows.Forms.Button btn_chb;
        private CommonCfgBar bar_cfg;
        private UserCtrl.Combo cbo_class;
        private System.Windows.Forms.GroupBox gb_pos;
        private UserCtrl.RadioButtons rb_offset_h;
        private UserCtrl.RadioButtons rb_rot;
        private UserCtrl.RadioButtons rb_offset_v;
        private UserCtrl.Check chb_offset_h;
        private UserCtrl.Check chb_rot;
        private UserCtrl.InputDouble tb_rot_angl;
        private UserCtrl.Check chb_offset_v;
        private UserCtrl.InputDouble tb_offset_h;
        private UserCtrl.InputDouble tb_offset_v;
        private System.Windows.Forms.GroupBox gb_deform;
        private UserCtrl.InputDouble tb_deform_end;
        private UserCtrl.Check chb_deform_end;
        private UserCtrl.InputDouble tb_deform_start;
        private UserCtrl.Check chb_deform_start;
        private System.Windows.Forms.GroupBox gb_height;
        private UserCtrl.InputDouble tb_bottom;
        private UserCtrl.Check chb_bottom;
        private UserCtrl.InputDouble tb_top;
        private UserCtrl.Check chb_top;
    }
}