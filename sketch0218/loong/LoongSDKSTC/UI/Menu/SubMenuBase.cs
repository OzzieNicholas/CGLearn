using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;

namespace LoongCAD.UI
{
    public class SubMenuBase : ContextMenuStrip
    {
        protected Dictionary<string, ToolStripItem> m_namedItems = new Dictionary<string, ToolStripItem>();

        public SubMenuBase() {

        }

        public new void Show(Control parent, int x, int y) {
            this._onShown();
            base.Show(parent, x, y);
        }

        public new void Show(Control parent, Point pos) {
            this._onShown();
            base.Show(parent, pos);
        }

        protected virtual void _onShown() {

        }

        public virtual void AddItem(ToolStripItem item) {
            if (item == null)
                return;

            if (item.Tag != null) {
                m_namedItems[item.Tag as string] = item;
            }

            this.Items.Add(item);
        }

        public virtual void AddItems(ToolStripItem[] items) {
            Stack<ToolStripItem> stack = new Stack<ToolStripItem>();
            foreach (var item in items)
                stack.Push(item);

            while (stack.Count > 0) {
                var item = stack.Pop();
                if (item == null)
                    continue;

                if (item.Tag != null)
                    m_namedItems[item.Tag as string] = item;

                if (item is ToolStripMenuItem) {
                    var mitem = item as ToolStripMenuItem;
                    foreach (ToolStripItem it in mitem.DropDownItems)
                        stack.Push(it);
                }
            }

            this.Items.AddRange(items);
        }

        public ToolStripItem GetItem(string name) {
            ToolStripItem item = null;
            m_namedItems.TryGetValue(name, out item);
            return item;
        }

        public ToolStripMenuItem GetMenuItem(string name) {
            ToolStripItem item = null;
            m_namedItems.TryGetValue(name, out item);
            return item as ToolStripMenuItem;
        }

        public void EnableItem(string name, bool b) {
            ToolStripItem item = null;
            if (!m_namedItems.TryGetValue(name, out item))
                return;

            item.Visible = b;
        }

        public ToolStripMenuItem NewGroup(string text, ToolStripItem[] children) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripMenuItem NewGroup(string name, string text, ToolStripItem[] children) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Tag = name;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripMenuItem NewItem(string text, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem NewItem(string name, string text, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Tag = name;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem NewCheckedItem(string text, bool val, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Checked = val;
            item.CheckOnClick = true;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem NewCheckedItem(string name, string text, bool val, EventHandler onChecked, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Tag = name;
            item.Checked = val;
            item.CheckOnClick = true;
            item.CheckStateChanged += onChecked;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripSeparator NewSep() {
            return new ToolStripSeparator();
        }

        public ToolStripSeparator NewSep(string name) {
            var sep = new ToolStripSeparator();
            sep.Tag = name;
            return sep;
        }
    }

    
}
