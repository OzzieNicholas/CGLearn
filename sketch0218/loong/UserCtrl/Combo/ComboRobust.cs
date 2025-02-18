using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{
    //Combo因使用DataSource, DropDownStyle为DropDown时，会有问题
    //此类不使用DataSource, 因此更稳定
    [ToolboxItem(true)]
    public class ComboRobust : ComboBox, IUserCtrl
    {
        //common
        public enum DisplayStyle { TEXT = 0, IMAGE, IMAGE_TEXT, COLOR, COLOR_TEXT };
        private DisplayStyle m_dispStyle = DisplayStyle.TEXT;
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;
        private int m_oldSelectedIndex = -1;
        private Color m_focusColor = Color.FromArgb(225, 225, 235);

        //image mode
        private Image m_img = null; //combined image 
        private Size m_imgItemSize = new Size(0, 0);
        private Padding m_imgBorder = new Padding(0);

        //items
        private List<object> m_keys = new List<object>();
        private List<int> m_imgIndices = new List<int>();

        //render
        private List<Size> m_itemTextSize = new List<Size>();
        private StringFormat m_textFmtMid = new StringFormat();
        private StringFormat m_textFmtLeft = new StringFormat();
        private bool m_textSizeDirty = true;

        public ComboRobust() {
            base.DoubleBuffered = true;
            base.ItemHeight = (int)(15.0 * DPI.ScaleFactor);
            base.DropDownStyle = ComboBoxStyle.DropDown;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;

            m_textFmtLeft.LineAlignment = StringAlignment.Center;
            m_textFmtLeft.Alignment = StringAlignment.Near;

            m_textFmtMid.LineAlignment = StringAlignment.Center;
            m_textFmtMid.Alignment = StringAlignment.Center;
        }

        [Browsable(false), DefaultValue(ComboBoxStyle.DropDown)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            set => base.DropDownStyle = value;
        }

        [Browsable(false), DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [Browsable(false), DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.DrawMode = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            set => base.ItemHeight = value;
        }

        [Browsable(false)]
        public new ObjectCollection Items => base.Items;

        [Browsable(false), DefaultValue("text")]
        public new string DisplayMember {
            get => base.DisplayMember;
            set => base.DisplayMember = value;
        }

        [Browsable(false), DefaultValue("idx")]
        public new string ValueMember {
            get => base.ValueMember;
            set => base.ValueMember = value;
        }

        [DefaultValue(typeof(Color), "0xE1E1EB")]
        public Color focusColor {
            get => m_focusColor;
            set => m_focusColor = value;
        }

        [DefaultValue(0)]
        public DisplayStyle dispStyle {
            get => m_dispStyle;
            set {
                m_dispStyle = value;
                this.Invalidate();
            }
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

        [DefaultValue(null)]
        public Image Image {
            get => m_img;
            set => m_img = value;
        }

        [DefaultValue(0)]
        public Padding ImageBorder {
            get => m_imgBorder;
            set => m_imgBorder = value;
        }

        [DefaultValue(0)]
        public Size ImageItemSize {
            get => m_imgItemSize;
            set => m_imgItemSize = value;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set {
                base.Font = value;
                m_textSizeDirty = true;
            }
        }


        public void ClearItems() {
            this.Items.Clear();
            m_keys.Clear();
            m_imgIndices.Clear();
            m_textSizeDirty = true;
        }

        public void AddItems(Type defType, string ignorePrefix, bool img, int imgIdxOff = 0) {
            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            int off = ignorePrefix.Length;
            for (int i = 0; i < defnames.Length; ++i) {
                object key = defvals.GetValue(i);
                string text = defnames[i].Substring(off);

                m_keys.Add(key);

                if (dataType == typeof(int))
                    m_imgIndices.Add(img ? imgIdxOff + (int)key : -1);
                else if (dataType == typeof(short))
                    m_imgIndices.Add(img ? imgIdxOff + (short)key : -1);
                else if (dataType == typeof(sbyte))
                    m_imgIndices.Add(img ? imgIdxOff + (sbyte)key : -1);

                this.Items.Add(text);
            }

            m_textSizeDirty = true;
        }

        public void AddItem(object key, object val, int imgIdx) {
            var innerKey = _key(key);

            m_keys.Add(innerKey);
            m_imgIndices.Add(imgIdx);
            this.Items.Add(val.ToString());
            m_textSizeDirty = true;
        }

        public void InsertItem(int pos, object key, object val, int imgIdx) {
            pos = Math.Max(0, Math.Min(m_keys.Count, pos));

            string text = val.ToString();

            var item = new ComboItem(pos, key, text, imgIdx);

            if (pos < this.Items.Count) {
                m_keys.Insert(pos, key);
                m_imgIndices.Insert(pos, imgIdx);
                this.Items.Insert(pos, text);
            }
            else {
                m_keys.Add(key);
                m_imgIndices.Add(imgIdx);
                this.Items.Add(text);
            }

            m_textSizeDirty = true;
        }

        public bool RemoveItem(object key) {
            int idx = m_keys.IndexOf(_key(key));
            if (idx < 0)
                return false;

            m_keys.RemoveAt(idx);
            m_imgIndices.RemoveAt(idx);
            this.Items.RemoveAt(idx);

            m_textSizeDirty = true;
            return true;
        }

        public bool ContainsKey(object key) {
            if (key == null)
                return false;

            return m_keys.IndexOf(_key(key)) >= 0;
        }

        [Browsable(false), DefaultValue(null)]
        public object SelectedKey {
            get {
                if (this.SelectedIndex >= 0) {
                    return m_keys[this.SelectedIndex];
                }

                return null;
            }

            set {
                if (value == null) {
                    this.SelectedIndex = -1;
                    return;
                }

                value = _key(value);

                int idx = m_keys.IndexOf(value);
                if (idx >= 0) {
                    this.SelectedIndex = idx;
                    return;
                }

                if (this.DropDownStyle == ComboBoxStyle.DropDownList)
                    this.SelectedIndex = -1;
                else
                    this.Text = value.ToString();
            }
        }

        [Browsable(false), DefaultValue(-1)]
        public sbyte SelectedSByteKey {
            get {
                var key = this.SelectedKey;
                if (key == null)
                    return -1;

                return (sbyte)key;
            }

            set => this.SelectedKey = value;
        }

        [Browsable(false), DefaultValue(-1)]
        public short SelectedShortKey {
            get {
                var key = this.SelectedKey;
                if (key == null)
                    return -1;

                return (short)key;
            }

            set => this.SelectedKey = value;
        }

        [Browsable(false), DefaultValue(-1)]
        public int SelectedIntKey {
            get {
                var key = this.SelectedKey;
                if (key == null)
                    return -1;

                return (int)key;
            }
            set => this.SelectedKey = value;
        }

        [Browsable(false), DefaultValue(0.0)]
        public double SelectedDoubleKey {
            get {
                var key = this.SelectedKey;
                if (key == null || !(key is double))
                    return 0.0;

                return (double)key;
            }
            set => this.SelectedKey = value;
        }

        [Browsable(false), DefaultValue(null)]
        public string SelectedStrKey {
            get => (string)SelectedKey;
            set => this.SelectedKey = value;
        }

        [Browsable(false), DefaultValue(false)]
        public bool SelectedBoolKey {
            get {
                object key = this.SelectedKey;
                if (key == null)
                    return false;

                return (bool)key;
            }

            set => this.SelectedKey = value;
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            if (m_textSizeDirty) {
                this._updateTextSize();
                m_textSizeDirty = false;
            }

            //draw background
            if ((e.State & DrawItemState.Selected) == DrawItemState.Selected) {
                using (var br = new SolidBrush(m_focusColor)) {
                    e.Graphics.FillRectangle(br, e.Bounds);
                }
            }
            else {
                using (var br = new SolidBrush(this.BackColor)) {
                    e.Graphics.FillRectangle(br, e.Bounds);
                }
            }

            //draw item
            if (e.Index >= 0) {
                var text = this.Items[e.Index] as string;
                var imgIdx = m_imgIndices[e.Index];

                if (m_dispStyle == DisplayStyle.TEXT) {
                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString(text, e.Font, br, e.Bounds);
                    }
                }
                else if (m_dispStyle == DisplayStyle.IMAGE) {
                    Font font = e.Font;
                    if (m_img != null && m_imgItemSize.Width > 0 && m_imgItemSize.Height > 0) {
                        int imgH = Math.Max(2, e.Bounds.Height);
                        int imgW = m_imgItemSize.Width * imgH / m_imgItemSize.Height;

                        Rectangle dstRect = new Rectangle(e.Bounds.Left + e.Bounds.Width / 2 - imgW / 2,
                                                            e.Bounds.Top + e.Bounds.Height / 2 - imgH / 2,
                                                            imgW, imgH);
                        Rectangle srcRect = new Rectangle(m_imgBorder.Left, m_imgBorder.Top + imgIdx * m_imgItemSize.Height,
                                                            m_imgItemSize.Width, m_imgItemSize.Height);

                        e.Graphics.DrawImage(m_img, dstRect, srcRect, GraphicsUnit.Pixel);
                    }
                    else {
                        using (var br = new SolidBrush(this.ForeColor)) {
                            e.Graphics.DrawString(text, e.Font, br, e.Bounds);
                        }
                    }
                }
                else if (m_dispStyle == DisplayStyle.IMAGE_TEXT) {
                    int imgW = 0, imgH = 0;

                    //image
                    if (m_img != null && m_imgItemSize.Width > 0 && m_imgItemSize.Height > 0) {
                        imgH = Math.Max(2, e.Bounds.Height);
                        imgW = m_imgItemSize.Width * imgH / m_imgItemSize.Height;
                        Rectangle dstRect = new Rectangle(e.Bounds.Left, e.Bounds.Top, imgW, imgH);
                        Rectangle srcRect = new Rectangle(m_imgBorder.Left, m_imgBorder.Top + imgIdx * m_imgItemSize.Height, m_imgItemSize.Width, m_imgItemSize.Height);
                        e.Graphics.DrawImage(m_img, dstRect, srcRect, GraphicsUnit.Pixel);
                    }

                    //text
                    Size textSize = m_itemTextSize[e.Index];
                    int textX = e.Bounds.Left + imgW;
                    Rectangle textBound = new Rectangle(textX, e.Bounds.Top, e.Bounds.Width - textX, e.Bounds.Height);

                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString(text, e.Font, br, textBound, m_textFmtMid);
                    }
                }
                else if (m_dispStyle == DisplayStyle.COLOR) {
                    using (var br = new SolidBrush(Color.FromArgb(imgIdx))) {
                        e.Graphics.FillRectangle(br, e.Bounds);
                    }
                }
                else if (m_dispStyle == DisplayStyle.COLOR_TEXT) {
                    int padx = 2;
                    
                    int clrW = e.Bounds.Width / 4;
                    var clrRect = new Rectangle(e.Bounds.X + padx, e.Bounds.Top + 2, clrW, e.Bounds.Height - 4);

                    if ((e.State & DrawItemState.Selected) == DrawItemState.Selected) {
                        e.Graphics.FillRectangle(Brushes.Gray, clrRect.X - 1, clrRect.Top - 1, clrRect.Width + 2, clrRect.Height + 2);
                    }

                    using (var br = new SolidBrush(Color.FromArgb(imgIdx))) {
                        e.Graphics.FillRectangle(br, clrRect);
                    }

                    //text
                    int textX = e.Bounds.Left + padx + clrW + 2;
                    Rectangle textBound = new Rectangle(textX, e.Bounds.Top, e.Bounds.Width - textX, e.Bounds.Height);
                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString(text, e.Font, br, textBound, m_textFmtLeft);
                    }
                }
            }
        }

        protected override void OnSelectedIndexChanged(EventArgs e) {
            if (this.SelectedIndex == m_oldSelectedIndex)
                return;

            base.OnSelectedIndexChanged(e);

            m_oldSelectedIndex = this.SelectedIndex;
        }

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            if (this.DropDownStyle == ComboBoxStyle.DropDownList
                && this.SelectedIndex == m_oldSelectedIndex)
                return;

            base.OnSelectionChangeCommitted(e);

            if (m_checkOnChange && m_checkName != null && m_checkName.Length > 0 && this.TopLevelControl != null) {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_checkName, true);
                if (ctrls != null && ctrls.Length == 1) {
                    if (ctrls[0] is CheckBox) {
                        CheckBox chb = ctrls[0] as CheckBox;
                        if (!chb.Checked)
                            chb.Checked = true;
                    }
                }
            }
        }

        private void _updateTextSize() {
            m_itemTextSize.Clear();

            for (int i = 0; i < this.Items.Count; ++i) {
                var text = this.Items[i] as string;
                m_itemTextSize.Add(TextRenderer.MeasureText(text, this.Font));
            }
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
