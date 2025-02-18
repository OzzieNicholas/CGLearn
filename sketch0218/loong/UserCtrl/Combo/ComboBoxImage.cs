using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ComboBoxImage : ComboBox, IUserCtrl
    {
        private ImageList m_imgList = null;
        private Image m_img = null; //combined image 
        private Size m_imgItemSize = new Size(0, 0);
        private Padding m_imgBorder = new Padding(0);

        private List<ComboItem> m_items = new List<ComboItem>();
        private Dictionary<object, ComboItem> m_key2Item = new Dictionary<object, ComboItem>();
        private bool m_itemDirty = false;
        
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;
        private int m_oldSelectedIdx = -1;

        private Color m_focusColor = Color.FromArgb(225, 225, 235);
        
        public ComboBoxImage() {
            base.DoubleBuffered = true;
            base.ItemHeight = this.ItemHeight = (int)(DPI.ScaleFactor * 15.0);
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.Font = new Font(this.Font.FontFamily, 13F * DPI.ScaleFactor, this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [Browsable(false), DefaultValue(ComboBoxStyle.DropDownList)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            //set => base.DropDownStyle = value;
        }

        [Browsable(false), DefaultValue(DrawMode.OwnerDrawFixed)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.DrawMode = value;
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            set => base.ItemHeight = Math.Max(1, value);
        }

        [Browsable(false)]
        public new ObjectCollection Items {
            get => base.Items;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set { }
        }

        [DefaultValue(typeof(Color), "0xE1E1EB")]
        public Color focusColor {
            get => m_focusColor;
            set {
                m_focusColor = value;
            }
        }

        [DefaultValue(null)]
        public ImageList ImageList {
            get => m_imgList;
            set {
                m_imgList = value;
                if (!Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(null)]
        public Image Image {
            get => m_img;
            set {
                m_img = value;
                if (!Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(0)]
        public Padding ImageBorder {
            get => m_imgBorder;
            set {
                m_imgBorder = value;
                if (!Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(0)]
        public Size ImageItemSize {
            get => m_imgItemSize;
            set {
                m_imgItemSize = value;
                if (!Define.IS_RUNTIME)
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

        public void ClearItems() {
            m_items.Clear();
            m_key2Item.Clear();
            m_itemDirty = true;
        }

        public void LoadItems(object[] arr) {
            List<ComboItem> items = new List<ComboItem>();
            if (arr != null) {
                for (int i = 0; i < arr.Length; i += 2) {
                    var item = new ComboItem(i / 2, arr[i],  arr[i].ToString(), (int)arr[i + 1]);
                    items.Add(item);

                    if (item.key0 != null)
                        m_key2Item[_key(item.key0)] = item;
                }
            }

            m_items = items;

            this.Items.Clear();
            this.Items.AddRange(m_items.Select(it => (object)it.idx).ToArray());

            m_itemDirty = false;
        }

        public void LoadItems(Type defType, int imgIdxOff = 0) {
            m_items.Clear();
            m_key2Item.Clear();

            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            for (int i = 0; i < defnames.Length; ++i) {
                int idx = m_items.Count;
                object key = defvals.GetValue(i);

                var item = new ComboItem(idx, key, key.ToString(), imgIdxOff + i);
                m_items.Add(item);

                if (dataType == typeof(int))
                    m_key2Item[(int)key] = item;
                else if (dataType == typeof(short))
                    m_key2Item[(short)key] = item;
                else if (dataType == typeof(sbyte))
                    m_key2Item[(sbyte)key] = item;
            }

            this.Items.Clear();
            this.Items.AddRange(m_items.Select(it => (object)it.idx).ToArray());
            m_itemDirty = false;
        }

        public void AddItem(object key, int imgIdx) {
            int idx = m_items.Count;
            string text = key.ToString();

            var item = new ComboItem(idx, key, text, imgIdx);
            m_items.Add(item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void AddItem(object key0, object key1, int imgIdx) {
            int idx = m_items.Count;
            string text = key0.ToString();

            var item = new ComboItem(idx, key0, key1, text, imgIdx);
            m_items.Add(item);
            m_key2Item[_key(key0)] = item;
            m_key2Item[_key(key1)] = item;
            m_itemDirty = true;
        }

        public void AddItems(Type defType, int imgIdxOff = 0) {
            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            for (int i = 0; i < defnames.Length; ++i) {
                int idx = m_items.Count;
                object key = defvals.GetValue(i);

                var item = new ComboItem(idx, key, key.ToString(), imgIdxOff + i);
                m_items.Add(item);

                if (dataType == typeof(int))
                    m_key2Item[(int)key] = item;
                else if (dataType == typeof(short))
                    m_key2Item[(short)key] = item;
                else if (dataType == typeof(sbyte))
                    m_key2Item[(sbyte)key] = item;
            }

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
                this.Items.Clear();
                this.Items.AddRange(m_items.Select(it => (object)it.idx).ToArray());
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

                if (item != null)
                    this.SelectedIndex = item.idx;
            }
        }

        [Browsable(false), DefaultValue((sbyte)-1)]
        public virtual sbyte SelectedSByteKey => Utils.ToSByte(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue((short)-1)]
        public virtual short SelectedShortKey => Utils.ToShort(_key(this.SelectedKey), -1); 

        [Browsable(false), DefaultValue(-1)]
        public virtual int SelectedIntKey => Utils.ToInt(_key(this.SelectedKey), -1); 

        [Browsable(false), DefaultValue(0.0)]
        public virtual double SelectedDoubleKey => Utils.ToDouble(_key(this.SelectedKey), 0.0); 

        [Browsable(false), DefaultValue(null)]
        public virtual string SelectedStrKey => Utils.ToStr(_key(this.SelectedKey), null);

        [Browsable(false), DefaultValue(false)]
        public virtual bool SelectedBoolKey => Utils.ToBool(_key(this.SelectedKey), false);

        [Browsable(false)]
        public List<ComboItem> ComboItems => m_items;

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
            if (e.Index < 0) {
                using (var br = new SolidBrush(this.ForeColor)) {
                    e.Graphics.DrawString("Image:-1", e.Font, br, e.Bounds.Left, e.Bounds.Top);
                }
            }
            else if (e.Index >= m_items.Count) {
                using (var br = new SolidBrush(this.ForeColor)) {
                    e.Graphics.DrawString("Image:OutOfIndex", e.Font, br, e.Bounds.Left, e.Bounds.Top);
                }
            }
            else {
                int imgIdx = m_items[e.Index].imgIdx;
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
                else if (m_imgList != null) {
                    if (imgIdx >= 0) {
                        Image img = m_imgList.Images[imgIdx];
                        int imgW = img.Width * e.Bounds.Height / img.Height;
                        int imgH = e.Bounds.Height;
                        e.Graphics.DrawImage(img, e.Bounds.Left + e.Bounds.Width / 2 - imgW / 2, e.Bounds.Top, imgW, imgH);
                    } else {
                        /// 图片索引小于0，则绘制空白区域
                        Image firstImg = m_imgList.Images[0];
                        int imgW = firstImg.Width * e.Bounds.Height / firstImg.Height;
                        int imgH = e.Bounds.Height;
                        e.Graphics.FillRectangle(Brushes.White, e.Bounds.Left + e.Bounds.Width / 2 - imgW / 2, e.Bounds.Top, imgW, imgH);
                    }
                }
                else {
                    using (var br = new SolidBrush(this.ForeColor)) {
                        e.Graphics.DrawString("Image:" + imgIdx, e.Font, br, e.Bounds.Left, e.Bounds.Top);
                    }
                }
            }
        }

        protected override void OnSelectedIndexChanged(EventArgs e) {
            if (this.SelectedIndex == m_oldSelectedIdx)
                return;

            base.OnSelectedIndexChanged(e);

            m_oldSelectedIdx = this.SelectedIndex;
        }

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            if (this.SelectedIndex == m_oldSelectedIdx)
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
