using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using System.Drawing.Design;
using System.Diagnostics;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public partial class RadioButtons : UserControl, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private bool m_checkOnChange = true;
        private string m_tabGroup = "";

        private int m_btnCount = 1;
        private Size m_btnSize = new Size(50, 28);
        private int m_btnMargin = 3;
        private Color m_btnColor = Color.LightGray;
        private Color m_hoveredBtnColor = Color.LightSteelBlue;
        private Color m_selectedBtnColor = Color.SteelBlue;
        private Color m_selectedForeColor = Color.White;
        private string[] m_btnLabels = new string[] { "" };
        private object[] m_btnKeys = new object[] { 0 };
        private object[] m_btnKeys1 = new object[] { 0 };

        public Action<object, EventArgs> onUserModified = null;
        public Action<object, EventArgs> onButtonMouseDown = null;

        public RadioButtons() {
            InitializeComponent();

            this.ForeColor = Color.SlateGray;
        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue(true)]
        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(1)]
        public int btnCount {
            get => m_btnCount;
            set {
                m_btnCount = Math.Max(1, value);
        
                if (!Define.IS_RUNTIME)
                    this.refresh();
            }
        }
        
        public Size btnSize {
            get => m_btnSize;
            set {
                m_btnSize = value;
        
                if (!Define.IS_RUNTIME) {
                    this.SuspendLayout();
                    int x = 0;
                    foreach (CheckBox btn in this.Controls) {
                        btn.SetBounds(x, 0, m_btnSize.Width, m_btnSize.Height);
                        x += m_btnSize.Width + m_btnMargin;
                    }
                    this.Size = new Size(m_btnSize.Width * m_btnCount + m_btnMargin * (m_btnCount - 1),  m_btnSize.Height);
                    this.ResumeLayout();
                }
            }
        }
        
        [DefaultValue(3)]
        public int btnMargin {
            get => m_btnMargin;
            set {
                m_btnMargin = value;
        
                if (!Define.IS_RUNTIME) {
                    this.SuspendLayout();
                    int x = 0;
                    foreach (CheckBox btn in this.Controls) {
                        btn.SetBounds(x, 0, m_btnSize.Width, m_btnSize.Height);
                        x += m_btnSize.Width + m_btnMargin;
                    }
                    this.ResumeLayout();
                }
            }
        }

        [DefaultValue(typeof(Color), "0xD3D3D3")]
        public Color btnColor {
            get => m_btnColor;
            set {
                m_btnColor = value;
        
                if (!Define.IS_RUNTIME) {
                    foreach (CheckBox btn in this.Controls) 
                        btn.BackColor = m_btnColor;
                }
            }
        }

        [DefaultValue(typeof(Color), "0xB0C4DE")]
        public Color hoveredBtnColor {
            get => m_hoveredBtnColor;
            set => m_hoveredBtnColor = value;
        }

        [DefaultValue(typeof(Color), "0xFFFFFF")]
        public Color selectedForeColor {
            get => m_selectedForeColor;
            set => m_selectedForeColor = value;
        }

        [DefaultValue(typeof(Color), "0x4682B4")]
        public Color selectedBtnColor {
            get => m_selectedBtnColor;
            set => m_selectedBtnColor = value;
        }

        public string[] labels {
            get => m_btnLabels;
            set {
                m_btnLabels = value;
        
                if (!Define.IS_RUNTIME) {
                    int n = 0;
                    foreach (CheckBox btn in this.Controls) {
                        btn.Text = (m_btnLabels != null && n < m_btnLabels.Length) ? m_btnLabels[n] : "";
                        n++;
                    }
                }
            }
        }
        
        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set {
                base.Font = value;
        
                if (!Define.IS_RUNTIME) {
                    foreach (CheckBox btn in this.Controls)
                        btn.Font = base.Font;
                }
            }
        }

        [Browsable(false)]
        public object[] keys {
            get => m_btnKeys;
            set => m_btnKeys = value;
        }

        [Browsable(false)]
        public object[] keys1 {
            get => m_btnKeys1;
            set => m_btnKeys1 = value;
        }

        [DefaultValue(null), Browsable(false)]
        public object Value {
            get {
                for (int i = 0; i < this.Controls.Count; ++i) {
                    var btn = this.Controls[i] as CheckBox;
                    if (btn.Checked && i < m_btnKeys.Length)
                        return m_btnKeys[i];
                }
        
                return null;
            }
        
            set {
                var dstkey = _key(value);
                
                CheckBox selBtn = null;
                for (int i = 0; i < m_btnKeys.Length; ++i) {
                    if (i >= this.Controls.Count)
                        break;
                
                    var key = _key(m_btnKeys[i]);
                    if (dstkey != null && key.Equals(dstkey)) {
                        selBtn = this.Controls[i] as CheckBox;
                        break;                    
                    }
                }

                if (selBtn == null) {
                    for (int i = 0; i < m_btnKeys1.Length; ++i) {
                        if (i >= this.Controls.Count)
                            break;

                        var key = _key(m_btnKeys1[i]);
                        if (dstkey != null && key.Equals(dstkey)) {
                            selBtn = this.Controls[i] as CheckBox;
                            break;
                        }
                    }
                }
                
                foreach (CheckBox btn in this.Controls) {
                    bool b = (btn == selBtn);
                    if (btn.Checked != b) {
                        btn.Checked = b;
                        btn.ForeColor = b ? m_selectedForeColor : this.ForeColor;
                    }
                }
            }
        }

        public void refresh() {
            this.SuspendLayout();

            var btns = new CheckBox[this.Controls.Count];
            if (this.Controls.Count > 0)
                this.Controls.CopyTo(btns, 0);
            this.Controls.Clear();

            int scaledMargin = DPI.Scale(m_btnMargin);
            Size scaledBtnSize = DPI.Scale(m_btnSize);

            List<CheckBox> newBtns = new List<CheckBox>();
            int x = 0;
            for (int i = 0; i < m_btnCount; ++i) {
                var btn = (i < btns.Length) ? btns[i] : new CheckBox();
                if (i >= btns.Length) {
                    btn.AutoCheck = false;
                    btn.Checked = false;
                    btn.Appearance = Appearance.Button;
                    btn.FlatStyle = FlatStyle.Flat;
                    btn.FlatAppearance.BorderSize = 0;
                    btn.TextAlign = ContentAlignment.MiddleCenter;
                    btn.MouseDown += Btn_MouseDown;
                    btn.TabStop = false;
                }

                btn.BackColor = m_btnColor;
                btn.ForeColor = btn.Checked? m_selectedForeColor : this.ForeColor;
                btn.FlatAppearance.CheckedBackColor = m_selectedBtnColor;
                btn.FlatAppearance.MouseOverBackColor = m_hoveredBtnColor;
                btn.Font = this.Font;
                btn.Text = (m_btnLabels != null && i < m_btnLabels.Length) ? m_btnLabels[i] : "";
                btn.SetBounds(x, 0, scaledBtnSize.Width, scaledBtnSize.Height);
                newBtns.Add(btn);

                x += scaledMargin + scaledBtnSize.Width;
            }
            this.Controls.AddRange(newBtns.ToArray());

            for (int i = m_btnCount; i < btns.Length; ++i)
                btns[i].Dispose();

            this.Size = new Size(scaledBtnSize.Width * m_btnCount + scaledMargin * (m_btnCount - 1), scaledBtnSize.Height);
            this.ResumeLayout();
        }

        private void Btn_MouseDown(object sender, MouseEventArgs e) {
            bool bModified = false;
            foreach (CheckBox btn in this.Controls) {
                bool b = (btn == sender);
                if (btn.Checked != b) {
                    btn.Checked = b;
                    btn.ForeColor = b ? m_selectedForeColor : this.ForeColor;
                    bModified = true;
                }
            }

            if (!Define.IS_RUNTIME)
                return;

            if (onButtonMouseDown != null)
                onButtonMouseDown(this, e);

            if (!bModified)
                return;

            if (m_checkOnChange && !String.IsNullOrEmpty(m_checkName) && this.Parent != null) {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.Parent.Controls.Find(m_checkName, true);
                if (ctrls != null && ctrls.Length == 1) {
                    if (ctrls[0] is CheckBox) {
                        CheckBox chb = ctrls[0] as CheckBox;
                        if (!chb.Checked)
                            chb.Checked = true;
                    }
                }
            }

            if (onUserModified != null)
                onUserModified(this, e);
        }

        private object _key(object key) {
            if (key == null)
                return key;

            var keyType = key.GetType();
            if (keyType.FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = keyType.GetEnumUnderlyingType();
                if (dataType == typeof(int))
                    return (int)key;
                else if (dataType == typeof(short))
                    return (short)key;
                else if (dataType == typeof(sbyte))
                    return (sbyte)key;
            }

            return key;
        }

    }

}
