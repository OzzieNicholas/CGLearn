using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class Combo : ComboBox, IUserCtrl
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
        private bool m_keyBrowse = false;
        private string m_keyBrowseInput = "";
        private long m_keyBrowseTm = 0;

        //image mode
        private ImageList m_imgList = null; //one image per item
        private Image m_img = null; //combined image 
        private Size m_imgItemSize = new Size(0, 0);
        private Padding m_imgBorder = new Padding(0);

        //items
        private List<ComboItem> m_items = new List<ComboItem>();
        private Dictionary<object, ComboItem> m_key2Item = new Dictionary<object, ComboItem>();
        private bool m_itemDirty = false;
        private int m_tmpSelectedIndex = -1; //用于保存Items未刷新前设置的SelectedIndex

        //render
        private List<Size> m_itemTextSize = new List<Size>();
        private StringFormat m_textFmtMid = new StringFormat();
        private StringFormat m_textFmtLeft = new StringFormat();

        public Combo() {
            base.DoubleBuffered = true;
            base.ItemHeight = (int)(15.0 * DPI.ScaleFactor);
            base.DropDownStyle = ComboBoxStyle.DropDown;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);

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
            //set => base.FlatStyle = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            set => base.ItemHeight = Math.Max(1, value);
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

        [DefaultValue(null)]
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

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(true)]
        public bool CheckOnChange {
            get => m_checkOnChange;
            set => m_checkOnChange = value;
        }

        [DefaultValue(false)]
        public bool KeyBrowse {
            get => m_keyBrowse;
            set => m_keyBrowse = value;
        }

        [DefaultValue(null)]
        public ImageList ImageList {
            get => m_imgList;
            set => m_imgList = value;
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
                this._updateTextSize();
            }
        }


        public void ClearItems() {
            m_items.Clear();
            m_key2Item.Clear();
            m_itemDirty = true;
        }

        public void LoadItems(ComboItem[] items) {
            m_items = items.ToList();
            m_key2Item.Clear();

            for (int i = 0; i < m_items.Count; ++i) {
                var item = m_items[i];
                item.idx = i;

                if (item.key0 != null)
                    m_key2Item[_key(item.key0)] = item;

                if (item.key1 != null)
                    m_key2Item[_key(item.key1)] = item;
            }

            this._updateTextSize();

            if (this.DataSource != null)
                this.DataSource = null;

            this.DataSource = m_items;

            m_itemDirty = false;
        }

        public void LoadItems(Type defType, string ignorePrefix, bool img, int imgIdxOff = 0) {
            m_items.Clear();
            m_key2Item.Clear();

            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            int off = ignorePrefix.Length;
            for (int i = 0; i < defnames.Length; ++i) {
                int idx = m_items.Count;
                object key = defvals.GetValue(i);
                string text = defnames[i].Substring(off);

                ComboItem item = null;
                if (dataType == typeof(int)) {
                    item = new ComboItem(idx, key, text, img ? (imgIdxOff + (int)key) : -1);
                    m_key2Item[(int)key] = item;
                }
                else if (dataType == typeof(short)) {
                    item = new ComboItem(idx, key, text, img ? (imgIdxOff + (short)key) : -1);
                    m_key2Item[(short)key] = item;
                }
                else if (dataType == typeof(sbyte)) {
                    item = new ComboItem(idx, key, text, img ? (imgIdxOff + (sbyte)key) : -1);
                    m_key2Item[(sbyte)key] = item;
                }

                if (item != null)
                    m_items.Add(item);
            }

            this._updateTextSize();

            if (this.DataSource != null) {
                this.DataSource = null;
                base.ValueMember = "idx";
                base.DisplayMember = "text";
            }
            
            this.DataSource = m_items;
            m_itemDirty = false;
        }

        public void AddItems(Type defType, string ignorePrefix, bool img, int imgIdxOff = 0) {
            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            int off = ignorePrefix.Length;
            for (int i = 0; i < defnames.Length; ++i) {
                int idx = m_items.Count;
                object key = defvals.GetValue(i);
                string text = defnames[i].Substring(off);

                ComboItem item = null;
                if (dataType == typeof(int)) {
                    item = new ComboItem(idx, key, text, img ? imgIdxOff + (int)key : -1);
                    m_key2Item[(int)key] = item;
                }
                else if (dataType == typeof(short)) {
                    item = new ComboItem(idx, key, text, img ? imgIdxOff + (short)key : -1);
                    m_key2Item[(short)key] = item;
                }
                else if (dataType == typeof(sbyte)) {
                    item = new ComboItem(idx, key, text, img ? imgIdxOff + (sbyte)key : -1);
                    m_key2Item[(sbyte)key] = item;
                }

                if (item != null)
                    m_items.Add(item);
            }

            m_itemDirty = true;
        }

        public void AddItem(object key, object val, int imgIdx) {
            int idx = m_items.Count;
            string text = val.ToString();

            var item = new ComboItem(idx, key, text, imgIdx);
            m_items.Add(item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void AddItem(object key0, object key1, object val, int imgIdx) {
            int idx = m_items.Count;
            string text = val.ToString();

            var item = new ComboItem(idx, key0, key1, text, imgIdx);
            m_items.Add(item);
            m_key2Item[_key(key0)] = item;
            m_key2Item[_key(key1)] = item;
            m_itemDirty = true;
        }

        public void InsertItem(int pos, object key, object val, int imgIdx) {
            pos = Math.Max(0, Math.Min(m_items.Count, pos));

            string text = val.ToString();

            var item = new ComboItem(pos, key, text, imgIdx);

            if (pos < m_items.Count) {
                m_items.Insert(pos, item);
                for (int i = pos; i < m_items.Count; ++i)
                    m_items[i].idx++;
            }
            else
                m_items.Add(item);

            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public bool RemoveItem(object key) {
            ComboItem item = null;
            if (!m_key2Item.TryGetValue(_key(key), out item))
                return false;

            if (item.key0 != null)
                m_key2Item.Remove(_key(item.key0));
            if (item.key1 != null)
                m_key2Item.Remove(_key(item.key1));

            for (int i = item.idx + 1; i < m_items.Count; ++i)
                m_items[i].idx -= 1;

            m_items.RemoveAt(item.idx);

            m_itemDirty = true;
            return true;
        }

        public void UpdateItems() {
            if (m_itemDirty) {
                this._updateTextSize();

                if (this.DataSource != null) {
                    this.DataSource = null;
                    base.ValueMember = "idx";
                    base.DisplayMember = "text";
                }

                this.DataSource = m_items;
                m_itemDirty = false;
            }
        }

        public bool ContainsKey(object key) {
            if (key == null)
                return false;

            return m_key2Item.ContainsKey(_key(key));
        }

        [Browsable(false), DefaultValue(null)]
        public object SelectedKey {
            get {
                if (this.SelectedIndex >= 0) {
                    return m_items[this.SelectedIndex].key0;
                }

                return null;
            }

            set {
                
                if (value == null) {
                    this.SelectedIndex = -1;
                    return;
                }

                value = _key(value);

                ComboItem item = null;
                if (value is string)
                    m_key2Item.TryGetValue((string)value, out item);
                else if (value is int)
                    m_key2Item.TryGetValue((int)value, out item);
                else if (value is uint)
                    m_key2Item.TryGetValue((uint)value, out item);
                else if (value is long)
                    m_key2Item.TryGetValue((long)value, out item);
                else if (value is ulong)
                    m_key2Item.TryGetValue((ulong)value, out item);
                else if (value is short)
                    m_key2Item.TryGetValue((short)value, out item);
                else if (value is ushort)
                    m_key2Item.TryGetValue((ushort)value, out item);
                else if (value is sbyte)
                    m_key2Item.TryGetValue((sbyte)value, out item);
                else if (value is byte)
                    m_key2Item.TryGetValue((byte)value, out item);
                else if (value is double)
                    m_key2Item.TryGetValue((double)value, out item);
                else if (value is bool)
                    m_key2Item.TryGetValue((bool)value, out item);
                else
                    m_key2Item.TryGetValue(value, out item);

                if (item != null) {
                    this.SelectedIndex = item.idx;
                    return;
                }

                if (this.DropDownStyle == ComboBoxStyle.DropDownList)
                    this.SelectedIndex = -1;
                else
                    this.Text = value.ToString();
            }
        }

        [Browsable(false), DefaultValue((sbyte)-1)]
        public sbyte SelectedSByteKey {
            get {
                var key = _key(this.SelectedKey);
                if (key == null)
                    return -1;

                return (sbyte)key;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false), DefaultValue((short)-1)]
        public short SelectedShortKey {
            get {
                var key = _key(this.SelectedKey);
                if (key == null)
                    return -1;

                return (short)key;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false), DefaultValue(-1)]
        public int SelectedIntKey {
            get {
                var key = _key(this.SelectedKey);
                if (key == null)
                    return -1;

                if (key is int)
                    return (int)key;
                else if (key is long)
                    return (int)(long)key;
                else if (key is short)
                    return (int)(short)key;
                else if (key is sbyte)
                    return (int)(sbyte)key;

                return -1;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false), DefaultValue(0.0)]
        public double SelectedDoubleKey {
            get {
                var key = this.SelectedKey;
                if (key == null || !(key is double))
                    return 0.0;

                return (double)key;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false), DefaultValue(null)]
        public string SelectedStrKey {
            get {
                return (string)SelectedKey;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false), DefaultValue(false)]
        public bool SelectedBoolKey {
            get {
                object key = this.SelectedKey;
                if (key == null)
                    return false;

                return (bool)key;
            }

            set {
                this.SelectedKey = value;
            }
        }

        [Browsable(false)]
        public List<ComboItem> ComboItems => m_items;

        [DefaultValue(-1)]
        public new int SelectedIndex {
            get => (m_tmpSelectedIndex >= 0) ? m_tmpSelectedIndex : base.SelectedIndex;
            set {
                if (value < 0 || value >= m_items.Count) {
                    m_tmpSelectedIndex = -1;
                    base.SelectedIndex = -1;
                    return;
                }

                if (m_items.Count != this.Items.Count) { //Items还未更新(通常因为这个控件还没有被加入可见的父窗口页面引起)
                    m_tmpSelectedIndex = value;
                    return;
                }

                m_tmpSelectedIndex = -1;
                base.SelectedIndex = value;
            }
        }

        [DefaultValue(null)]
        public new object SelectedItem {
            get {
                if (m_tmpSelectedIndex < 0)
                    return base.SelectedItem;

                if (m_tmpSelectedIndex < m_items.Count)
                    return m_items[m_tmpSelectedIndex];

                return null;
            }

            set {
                if (value == null) {
                    base.SelectedItem = null;
                    m_tmpSelectedIndex = -1;
                    return;
                }

                var item = value as ComboItem;
                if (item == null) {
                    base.SelectedItem = null;
                    m_tmpSelectedIndex = -1;
                    return;
                }

                int idx = m_items.IndexOf(item);
                this.SelectedIndex = idx;
            }
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            if (!m_keyBrowse
                || m_key2Item.Count == 0
                || m_items.Count != this.Items.Count)
                return;

            if (!char.IsLetterOrDigit(e.KeyChar)
                && !char.IsSymbol(e.KeyChar))
                return;

            var tm = (long)Win32.GetTickCount64();
            var dur = tm - m_keyBrowseTm;
            m_keyBrowseTm = tm;

            if (dur > 500)
                m_keyBrowseInput = e.KeyChar.ToString();
            else
                m_keyBrowseInput += e.KeyChar;

            var key0 = m_key2Item.First().Key;
            ComboItem item = null;
            if (key0 is int) {
                int key = 0;
                if (int.TryParse(m_keyBrowseInput, out key)) 
                    m_key2Item.TryGetValue(key, out item);
            }
            else if (key0 is long) {
                long key = 0;
                if (long.TryParse(m_keyBrowseInput, out key))
                    m_key2Item.TryGetValue(key, out item);
            }
            else if (key0 is string) {
                m_key2Item.TryGetValue(m_keyBrowseInput, out item);
            }
            else if (key0 is double) {
                double key = 0;
                if (double.TryParse(m_keyBrowseInput, out key))
                    m_key2Item.TryGetValue(key, out item);
            }

            if (item == null)
                return;

            this.SelectedIndex = item.idx;
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
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
                var item = m_items[e.Index];
                if (m_dispStyle == DisplayStyle.TEXT) {
                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString(item.text, e.Font, br, e.Bounds);
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
                        Rectangle srcRect = new Rectangle(m_imgBorder.Left, m_imgBorder.Top + item.imgIdx * m_imgItemSize.Height,
                                                            m_imgItemSize.Width, m_imgItemSize.Height);

                        e.Graphics.DrawImage(m_img, dstRect, srcRect, GraphicsUnit.Pixel);
                    }
                    else if (m_imgList != null) {
                        Image img = m_imgList.Images[item.imgIdx];
                        int imgH = Math.Max(2, e.Bounds.Height);
                        int imgW = img.Width * imgH / img.Height;
                        e.Graphics.DrawImage(img, e.Bounds.Left + e.Bounds.Width / 2 - imgW / 2, e.Bounds.Top, imgW, imgH);
                    }
                    else {
                        using (var br = new SolidBrush(this.ForeColor)) {
                            e.Graphics.DrawString(item.text, e.Font, br, e.Bounds);
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
                        Rectangle srcRect = new Rectangle(m_imgBorder.Left, m_imgBorder.Top + item.imgIdx * m_imgItemSize.Height, m_imgItemSize.Width, m_imgItemSize.Height);
                        e.Graphics.DrawImage(m_img, dstRect, srcRect, GraphicsUnit.Pixel);
                    }
                    else if (m_imgList != null) {
                        Image img = m_imgList.Images[item.imgIdx];
                        imgH = Math.Max(2, e.Bounds.Height);
                        imgW = img.Width * imgH / img.Height;
                        e.Graphics.DrawImage(img, e.Bounds.Left, e.Bounds.Top, imgW, imgH);
                    }

                    //text
                    Size textSize = m_itemTextSize[e.Index];
                    int textX = e.Bounds.Left + imgW;
                    Rectangle textBound = new Rectangle(textX, e.Bounds.Top, e.Bounds.Width - textX, e.Bounds.Height);

#if false
                e.Graphics.DrawString(item.text, e.Font, UserRes.solidBrush(this.ForeColor), textBound, m_textFmtMid);
#else
                    TextFormatFlags flags = TextFormatFlags.HorizontalCenter |
                        TextFormatFlags.VerticalCenter;
                    TextRenderer.DrawText(e.Graphics, item.text, e.Font, textBound, this.ForeColor, flags);
#endif
                }
                else if (m_dispStyle == DisplayStyle.COLOR) {
                    using (var br = new SolidBrush(Color.FromArgb(item.imgIdx))) {
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

                    using (var br = new SolidBrush(Color.FromArgb(item.imgIdx))) {
                        e.Graphics.FillRectangle(br, clrRect);
                    }

                    //text
                    int textX = e.Bounds.Left + padx + clrW + 2;
                    Rectangle textBound = new Rectangle(textX, e.Bounds.Top, e.Bounds.Width - textX, e.Bounds.Height);
                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString(item.text, e.Font, br, textBound, m_textFmtLeft);
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

            if (this.DropDownStyle == ComboBoxStyle.DropDown) {
                var item = this.SelectedItem as ComboItem;
                if (item != null)
                    BeginInvoke(new Action(() => this.Text = item.text));
                else
                    BeginInvoke(new Action(() => this.Text = ""));
            }
        }

        protected override void OnInvalidated(InvalidateEventArgs e) {
            base.OnInvalidated(e);

            if (m_tmpSelectedIndex >= 0 && m_items.Count == this.Items.Count) { //有Items未刷新前设置的SelectedIndex，现在Items已创建，同步SelectedIndex
                if (m_tmpSelectedIndex < m_items.Count)
                    base.SelectedIndex = m_tmpSelectedIndex;
                else
                    base.SelectedIndex = -1;

                m_tmpSelectedIndex = -1;
                this.OnSelectedIndexChanged(null);
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

        private void _updateTextSize() {
            m_itemTextSize.Clear();

            for (int i = 0; i < m_items.Count(); ++i) {
                var item = m_items[i];
                m_itemTextSize.Add(TextRenderer.MeasureText(item.text, this.Font));
            }
        } 
    }
    
}
