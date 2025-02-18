using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;

namespace UserCtrl
{
    public class SubMenuBase : ContextMenuStrip
    {
        protected Dictionary<string, ToolStripItem> m_namedItems = new Dictionary<string, ToolStripItem>();

        public SubMenuBase() {

        }

        public virtual void show(Control parent, int x, int y) {
            this.onShown();
            this.Show(parent, x, y);
        }

        public virtual void show(Control parent, Point pos) {
            this.onShown();
            this.Show(parent, pos);
        }

        protected virtual void onShown() {

        }

        public virtual void addItem(ToolStripItem item) {
            if (item == null)
                return;

            if (item.Tag != null) {
                m_namedItems[item.Tag as string] = item;
            }

            this.Items.Add(item);
        }

        public virtual void addItems(ToolStripItem[] items) {
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

        public ToolStripItem getItem(string name) {
            ToolStripItem item = null;
            m_namedItems.TryGetValue(name, out item);
            return item;
        }

        public ToolStripMenuItem getMenuItem(string name) {
            ToolStripItem item = null;
            m_namedItems.TryGetValue(name, out item);
            return item as ToolStripMenuItem;
        }

        public void enableItem(string name, bool b) {
            ToolStripItem item = null;
            if (!m_namedItems.TryGetValue(name, out item))
                return;

            item.Visible = b;
        }

        public ToolStripMenuItem group(string text, ToolStripItem[] children) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripMenuItem group(string name, string text, ToolStripItem[] children) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Tag = name;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripMenuItem item(string text, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem item(string name, string text, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Tag = name;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem checkedItem(string text, bool val, EventHandler onClick, string shortCut = null) {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = text;
            item.Checked = val;
            item.CheckOnClick = true;
            item.Click += onClick;

            if (shortCut != null)
                item.ShortcutKeyDisplayString = shortCut;

            return item;
        }

        public ToolStripMenuItem checkedItem(string name, string text, bool val, EventHandler onChecked, string shortCut = null) {
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

        public ToolStripSeparator sep() {
            return new ToolStripSeparator();
        }

        public ToolStripSeparator sep(string name) {
            var sep = new ToolStripSeparator();
            sep.Tag = name;
            return sep;
        }
    }

    
}
