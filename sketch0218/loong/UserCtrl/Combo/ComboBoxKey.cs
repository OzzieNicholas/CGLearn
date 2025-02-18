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
    [ToolboxItem(true)]
    public class ComboBoxKey : ComboBox, IUserCtrl {
        private List<ComboItem> m_items = new List<ComboItem>();
        private Dictionary<object, ComboItem> m_key2Item = new Dictionary<object, ComboItem>();
        private bool m_itemDirty = false;
        
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private bool m_checkOnChange = true;
        private int m_oldSelectedIdx = -1;

        public ComboBoxKey() {
            base.DoubleBuffered = true;
            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.FlatStyle = FlatStyle.System;
            base.DropDownStyle = ComboBoxStyle.DropDownList;
            base.ItemHeight = (int)DPI.Scale(15f);
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
        }

        [Browsable(false), DefaultValue(ComboBoxStyle.DropDownList), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new ComboBoxStyle DropDownStyle {
            get => base.DropDownStyle;
            //set => base.DropDownStyle = value;
        }

        [Browsable(false), DefaultValue(DrawMode.OwnerDrawFixed), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new DrawMode DrawMode {
            get => base.DrawMode;
            //set => base.DrawMode = value;
        }

        [Browsable(false), DefaultValue(FlatStyle.System), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new FlatStyle FlatStyle {
            get => base.FlatStyle;
            //set => base.FlatStyle = value;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new Font Font {
            get => base.Font;
            set { } 
        }

        [DefaultValue(15), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new int ItemHeight {
            get => base.ItemHeight;
            //set => base.ItemHeight = value;
        }

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public new ObjectCollection Items {
            get => base.Items;
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
            this.Items.AddRange(m_items.Select(it => (object)it.idx).ToArray());
            m_itemDirty = false;
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

            this.Items.Clear();
            this.Items.AddRange(m_items.Select(it => (object)it.idx).ToArray());
            m_itemDirty = false;
        }

        public void LoadItems(object[] arr) {
            List<ComboItem> items = new List<ComboItem>();
            if (arr != null) {
                for (int i = 0; i < arr.Length; i += 2) {
                    var item = new ComboItem(i / 2, arr[i], arr[i + 1] as string, -1);
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

                this.SelectedIndex = (item != null) ? item.idx : -1;
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
        public List<ComboItem> ComboItems => m_items;

        protected override void OnDrawItem(DrawItemEventArgs e) {
            e.DrawBackground();

            if (e.Index >= 0 && e.Index < m_items.Count) {
                var item = m_items[e.Index];
                if (item != null
                    && !String.IsNullOrEmpty(item.text)) {
                    using (var br = new SolidBrush(e.ForeColor)) {
                        e.Graphics.DrawString(item.text, e.Font, br, e.Bounds);
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

            if (m_checkOnChange && !String.IsNullOrEmpty(m_checkName) && this.TopLevelControl != null) {
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
