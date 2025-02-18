using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class KeyValueComboBox : System.Windows.Forms.ComboBox, IParamCtrl, IPlaceholderCtrl, ITabOrderCtrl
    {
        private ComboItemCollection m_items = new ComboItemCollection();
        private Dictionary<object, ComboItem> m_key2Item = new Dictionary<object, ComboItem>();
        private bool m_itemDirty = false;
        
        private string m_paramName = "";
        private string m_ctrlCheckName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;
        private object m_placeholder = null;
        private int m_oldSelectedIdx = -1;

        public KeyValueComboBox() {
            base.DoubleBuffered = true;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)DPI.Scale(15f);
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
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

        [Browsable(false), DefaultValue(FlatStyle.System)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.FlatStyle = value;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => base.Font;
            set {
            }
        }

        [DefaultValue(15)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
        }

        [Browsable(false)]
        public new ObjectCollection Items {
            get => base.Items;
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

        public virtual object Placeholder
        {
            get => m_placeholder;
            set {
                m_placeholder = null;
            }
        }

        public virtual void ClearPlaceholder()
        {
            m_placeholder = null;
        }

        public void ClearItems() {
            m_items.Clear();
            m_key2Item.Clear();
            m_itemDirty = true;
        }

        public void LoadItems(Type defType, string ignorePrefix) {
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

                var item = new ComboItem(idx, key, text, -1);
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

        public void LoadItems(ComboItem[] items) {
            m_items.Clear();
            m_items.AddRange(items);
            m_key2Item.Clear();

            for (int i = 0; i < m_items.Count; ++i) {
                var item = m_items[i];
                item.Idx = i;

                if (item.Key0 != null)
                    m_key2Item[_key(item.Key0)] = item;

                if (item.Key1 != null)
                    m_key2Item[_key(item.Key1)] = item;
            }

            this.Items.Clear();
            this.Items.AddRange(m_items.Select(it => (object)it.Idx).ToArray());
            m_itemDirty = false;
        }

        public void LoadItems(object[] arr) {
            ComboItemCollection items = new ComboItemCollection();
            if (arr != null) {
                for (int i = 0; i < arr.Length; i += 2) {
                    var item = new ComboItem(i / 2, arr[i], arr[i + 1] as string, -1);
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

        public void AddItem(object key) {
            int idx = m_items.Count;
            string text = key.ToString();

            var item = new ComboItem(idx, key, text, -1);
            m_items.Add(item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void AddItem(object key, object val) {
            int idx = m_items.Count;
            string text = val.ToString();

            var item = new ComboItem(idx, key, text, -1);
            m_items.Add(item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void AddItem(object key0, object key1, object val) {
            int idx = m_items.Count;
            string text = val.ToString();

            var item = new ComboItem(idx, key0, key1, text, -1);
            m_items.Add(item);
            m_key2Item[_key(key0)] = item;
            m_key2Item[_key(key1)] = item;
            m_itemDirty = true;
        }

        public void InsertItem(int pos, object key) {
            int idx = m_items.Count;
            string text = key.ToString();

            for (int i = pos; i < m_items.Count; ++i)
                m_items[i].Idx += 1;

            var item = new ComboItem(idx, key, text, -1);
            m_items.Insert(pos, item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void InsertItem(int pos, object key, object val) {
            int idx = pos;
            string text = val.ToString();

            for (int i = pos; i < m_items.Count; ++i)
                m_items[i].Idx += 1;

            var item = new ComboItem(idx, key, text, -1);
            m_items.Insert(pos, item);
            m_key2Item[_key(key)] = item;
            m_itemDirty = true;
        }

        public void InsertItem(int pos, object key0, object key1, object val) {
            int idx = pos;
            string text = val.ToString();

            for (int i = pos; i < m_items.Count; ++i)
                m_items[i].Idx += 1;

            var item = new ComboItem(idx, key0, key1, text, -1);
            m_items.Insert(pos, item);
            m_key2Item[_key(key0)] = item;
            m_key2Item[_key(key1)] = item;
            m_itemDirty = true;
        }


        public void AddItems(Type defType, string ignorePrefix) {
            var defnames = Enum.GetNames(defType);
            var defvals = Enum.GetValues(defType);
            var dataType = defType.GetEnumUnderlyingType();
            int off = ignorePrefix.Length;
            for (int i = 0; i < defnames.Length; ++i) {
                int idx = m_items.Count;
                object key = defvals.GetValue(i);
                string text = defnames[i].Substring(off);

                ComboItem item = new ComboItem(idx, key, text, -1);
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

        public bool SetItemText(object key, string text) {
            ComboItem item = null;
            if (!m_key2Item.TryGetValue(_key(key), out item))
                return false;

            item.Text = text;
            this.Invalidate();
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


        [Browsable(false), DefaultValue(null)]
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

                this.SelectedIndex = (item != null) ? item.Idx : -1;
            }
        }

        [Browsable(false), DefaultValue((sbyte)-1)]
        public sbyte SelectedSByteKey => Utils.ToSByte(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue((short)-1)]
        public short SelectedShortKey => Utils.ToShort(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue(-1)]
        public int SelectedIntKey => Utils.ToInt(_key(this.SelectedKey), -1);

        [Browsable(false), DefaultValue(0.0)]
        public double SelectedDoubleKey => Utils.ToDouble(_key(this.SelectedKey), 0.0);

        [Browsable(false), DefaultValue(null)]
        public string SelectedStrKey => Utils.ToStr(_key(this.SelectedKey), "");

        [Browsable(false), DefaultValue(false)]
        public bool SelectedBoolKey => Utils.ToBool(_key(this.SelectedKey), false);

        [Browsable(false)]
        public ComboItemCollection ComboItems => m_items;

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
                this.SelectedItem = null;
            }
        }

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index >= 0 && e.Index < m_items.Count) {
                var item = m_items[e.Index];
                var itemText = item.Text;
                /// FIXME: 有待完善下拉框placeholder功能实现
#if false
                string placeholderText = null;
                if (m_placeholderIndex >= 0 && m_placeholderIndex < this.Items.Count &&
                    e.Index == this.SelectedIndex && m_placeholderIndex != this.SelectedIndex) {
                    placeholderText = m_key2Item[this.Items[m_placeholderIndex]].Text;
                }
                if (!string.IsNullOrEmpty(itemText) && !string.IsNullOrEmpty(placeholderText)) {
                    itemText += "(" + placeholderText + ")";
                }
#endif
                if (item != null 
                    && !String.IsNullOrEmpty(itemText)) {
                    using (var br = new SolidBrush(e.ForeColor)) {
                        e.Graphics.DrawString(itemText, e.Font, br, e.Bounds);
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

            if (m_checkOnChange && !String.IsNullOrEmpty(m_ctrlCheckName) && this.TopLevelControl != null) {
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

    }
}
