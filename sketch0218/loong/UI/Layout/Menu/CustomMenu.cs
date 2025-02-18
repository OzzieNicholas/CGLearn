using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;

namespace UI
{
    [ToolboxItem(false)]
    public class CustomMenu : MenuStrip
    {
        Dictionary<string, ToolStripItem> m_namedItems = new Dictionary<string, ToolStripItem>();

        public CustomMenu()
        {
            
        }

        public void addItem(ToolStripItem item)
        {
            if (item == null)
                return;

            if (item is ToolStripSeparator && item.Tag != null)
            {
                m_namedItems[item.Tag as string] = item;
            }
            else if (item is CustomMenuItem)
            {
                var citem = item as CustomMenuItem;
                if (citem.funcUnit != null)
                    m_namedItems[citem.funcUnit.name] = item;
            }

            this.Items.Add(item);
        }

        public void addItems(ToolStripItem[] items)
        {
            Stack<ToolStripItem> stack = new Stack<ToolStripItem>();
            foreach (var item in items)
                stack.Push(item);

            while(stack.Count > 0)
            {
                var item = stack.Pop();
                if (item == null)
                    continue;

                if (item is ToolStripSeparator)
                {
                    if (item.Tag != null)
                        m_namedItems[item.Tag as string] = item;
                }
                else if (item is CustomMenuItem)
                {
                    var citem = item as CustomMenuItem;
                    if (citem.funcUnit != null)
                        m_namedItems[citem.funcUnit.name] = item;

                    foreach (ToolStripItem it in citem.DropDownItems)
                        stack.Push(it);
                }
            }

            this.Items.AddRange(items);
        }

        public void enableItem(string name, bool b)
        {
            ToolStripItem item = null;
            if (!m_namedItems.TryGetValue(name, out item))
                return;

            item.Visible = b;
        }

        public void refresh()
        {
            foreach (var item in this.Items)
            {
                if (item is CustomMenuItem)
                    (item as CustomMenuItem).refresh();
            }
        }

        public CustomMenuItem group(string text, ToolStripItem[] children = null)
        {
            CustomMenuItem item = new CustomMenuItem("group");
            item.Text = text;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public CustomMenuItem group(string text, string name, ToolStripItem[] children = null) {
            CustomMenuItem item = new CustomMenuItem("group");
            item.Text = text;
            item.Tag = name;

            if (children != null)
                item.DropDownItems.AddRange(children);

            return item;
        }

        public ToolStripSeparator sep()
        {
            return new ToolStripSeparator();
        }

        public ToolStripSeparator sep(string name)
        {
            var sep = new ToolStripSeparator();
            sep.Tag = name;
            return sep;
        }

        //Click item
        public CustomMenuItem fu(string text, string funcName, DObject paras = null)
        {
            CustomMenuItem item = new CustomMenuItem("click");
            item.funcUnit = FuncUnit.get(funcName); 
            item.paras = paras;
            item.Text = text;
            return item;
        }

        //Checked item
        public CustomMenuItem cfu(string text, string funcName, DObject paras = null)
        {
            CustomMenuItem item = new CustomMenuItem("check");
            item.funcUnit = FuncUnit.get(funcName);
            item.paras = paras;
            item.Text = text;
            return item;
        }

        protected override void WndProc(ref Message m) {
            base.WndProc(ref m);

            if (m.Msg == Win32.WM_MOUSEACTIVATE &&
                m.Result == (IntPtr)Win32.MA_ACTIVATEANDEAT) {
                m.Result = (IntPtr)Win32.MA_ACTIVATE;
            }
        }
    }
}
