
using System;
using System.Drawing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Linq;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    public class ImageComboBoxBase : System.Windows.Forms.ComboBox, IParamCtrl, ITabOrderCtrl
    {
        protected ImageList m_imgList = null;
        protected Image m_img = null; //combined image 
        protected Size m_imgItemSize = new Size(0, 0);
        protected Padding m_imgBorder = new Padding(0);
        
        protected ComboItemCollection m_items = new ComboItemCollection();
        protected Dictionary<object, LoongCAD.UI.ComboItem> m_key2Item = new Dictionary<object, LoongCAD.UI.ComboItem>();
        protected bool m_itemDirty = false;
                
        protected string m_paramName = "";
        protected string m_ctrlCheckName = "";
        protected string m_tabGroup = "";
        protected bool m_checkOnChange = true;
        protected int m_oldSelectedIdx = -1;
        
        protected Color m_focusColor = Color.FromArgb(225, 225, 235);

        public ImageComboBoxBase() {
            base.DoubleBuffered = true;
            base.ItemHeight = (int)(DPI.ScaleFactor * 15.0);
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

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new ObjectCollection Items => base.Items;

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set {
            }
        }

        [DefaultValue(typeof(Color), "0xE1E1EB")]
        public Color FocusColor {
            get => m_focusColor;
            set => m_focusColor = value;
        }

        [DefaultValue("")]
        public string ParamName {
            get => m_paramName;
            set => m_paramName = value;
        }

        [DefaultValue("")]
        public string ControlCheckName {
            get => m_ctrlCheckName;
            set => m_ctrlCheckName = value;
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
            ComboItemCollection items = new ComboItemCollection();
            if (arr != null) {
                for (int i = 0; i < arr.Length; i += 2) {
                    var item = new ComboItem(i / 2, arr[i], arr[i].ToString(), (int)arr[i + 1]);
                    items.Add(item);

                    if (item.Key0 != null)
                        m_key2Item[_key(item.Key0)] = item;
                }
            }

            m_items = items;

            this.Items.Clear();
            this.Items.AddRange(m_items.Select(it => (object)it.Idx).ToArray());
            
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
            this.Items.AddRange(m_items.Select(it => (object)it.Idx).ToArray());
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

        public void AddIntegerItems(int startValue, int imgStartIndex, int addingCount)
        {
            if (addingCount <= 0)
                return;
            
            for (int i=0; i<addingCount; ++i) {
                int curKey = startValue + i;
                int curIdx = m_items.Count;
                string curText = curKey.ToString();
                
                var curItem = new ComboItem(curIdx, curKey, curText, imgStartIndex + i);
                m_items.Add(curItem);
                m_key2Item[_key(curKey)] = curItem;
            }
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

            if (item.Key0 != null)
                m_key2Item.Remove(_key(item.Key0));
            if (item.Key1 != null)
                m_key2Item.Remove(_key(item.Key1));

            for (int i = item.Idx + 1; i < m_items.Count; ++i)
                m_items[i].Idx -= 1;

            m_items.RemoveAt(item.Idx);

            m_itemDirty = true;
            return true;
        }

        public void UpdateItems() {
            if (m_itemDirty) {
                this.Items.Clear();
                this.Items.AddRange(m_items.Select(it => (object)it.Idx).ToArray());
                m_itemDirty = false;
            }
        }

        public bool ContainsKey(object key) {
            if (key == null)
                return false;

            return m_key2Item.ContainsKey(_key(key));
        }

        [Browsable(false), DefaultValue(null), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public object SelectedKey {
            get {
                if (this.SelectedIndex >= 0) {
                    return m_items[this.SelectedIndex].Key0;
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
                    this.SelectedIndex = item.Idx;
            }
        }

        [Browsable(false), DefaultValue((sbyte)-1), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual sbyte SelectedSByteKey => Utils.ToSByte(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue((short)-1), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual short SelectedShortKey => Utils.ToShort(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue(-1), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual int SelectedIntKey => Utils.ToInt(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue(0.0), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual double SelectedDoubleKey => Utils.ToDouble(_key(this.SelectedKey), 0.0);

        [Browsable(false), DefaultValue(null), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual string SelectedStrKey => Utils.ToStr(_key(this.SelectedKey), "");

        [Browsable(false), DefaultValue(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public virtual bool SelectedBoolKey => Utils.ToBool(_key(this.SelectedKey), false);

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public ComboItemCollection ComboItems => m_items;

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
            using (var br = new SolidBrush(this.ForeColor)) {
                if (e.Index < 0)
                    e.Graphics.DrawString("Image:-1", e.Font, br, e.Bounds.Left, e.Bounds.Top);
                else if (e.Index >= m_items.Count)
                    e.Graphics.DrawString("Image:OutOfIndex", e.Font, br, e.Bounds.Left, e.Bounds.Top);
                else {
                    int imgIdx = m_items[e.Index].ImageIdx;
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
                        }
                        else {
                            /// 图片索引小于0，则绘制空白区域
                            Image firstImg = m_imgList.Images[0];
                            int imgW = firstImg.Width * e.Bounds.Height / firstImg.Height;
                            int imgH = e.Bounds.Height;
                            e.Graphics.FillRectangle(Brushes.White, e.Bounds.Left + e.Bounds.Width / 2 - imgW / 2, e.Bounds.Top, imgW, imgH);
                        }
                    }
                    else {
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

            if (m_checkOnChange && m_ctrlCheckName != null && m_ctrlCheckName.Length > 0 && this.TopLevelControl != null) {
                //检查是否需要重置关联的Check控件
                Control[] ctrls = this.TopLevelControl.Controls.Find(m_ctrlCheckName, true);
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

        public void SetParameter(object value)
        {
            this.SelectedKey = value;
        }

        public object GetParameter()
        {
            return this.SelectedKey;
        }

        public object GetParameter(List<Control> exclude, bool check)
        {
            if (exclude != null && exclude.IndexOf(this) >= 0) {
                return null;
            }
            return this.SelectedKey;
        }

        public void ClearParameter()
        {
            if (this.Items.Count > 0) {
                this.SelectedIndex = 0;
            } else {
                this.SelectedKey = null;
            }
        }

    }
}
