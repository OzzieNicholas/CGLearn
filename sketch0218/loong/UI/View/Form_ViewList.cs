using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public partial class Form_ViewList : UI.ParaForm
    {
        public static Form_ViewList m_instance = new Form_ViewList();
        private Dictionary<string, long> m_viewIds = new Dictionary<string, long>();
        private SortedSet<string> m_inactiveViews = new SortedSet<string>();
        private SortedSet<string> m_activeViews = new SortedSet<string>();

        public static Form_ViewList instance => m_instance;

        public Form_ViewList() {
            InitializeComponent();

            this.DoubleBuffered = true;
            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            lb_all.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
            lb_active.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);

            this.updateTabOrder();
        }

        public void show() {
            if (this.Visible) {
                this.focus();
                return;
            }

            m_viewIds.Clear();
            m_inactiveViews.Clear();
            m_activeViews.Clear();
            lb_all.Items.Clear();
            lb_active.Items.Clear();

            if (!Native.getAllViewIdAndNames(out long[] ids, out string[] names, false))
                return;

            Native.getActiveViewIdAndNames(out long[] actIds, out string[] actNames, false);
            HashSet<long> actMap = (actIds != null) ? new HashSet<long>(actIds) : new HashSet<long>();

            for (int i = 0; i < names.Length; ++i) {
                m_viewIds[names[i]] = ids[i];

                if (actMap.Contains(ids[i]))
                    m_activeViews.Add(names[i]);
                else
                    m_inactiveViews.Add(names[i]);
            }

            lb_all.Items.AddRange(m_inactiveViews.ToArray());
            lb_active.SelectedIndex = -1;

            lb_active.Items.AddRange(m_activeViews.ToArray());
            lb_active.SelectedIndex = -1;

            this.Visible = true;
            this.focus();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void Btn_add_Click(object sender, EventArgs e) {
            if (lb_all.SelectedItems.Count == 0)
                return;

            string[] delItems = new string[lb_all.SelectedItems.Count];
            lb_all.SelectedItems.CopyTo(delItems, 0);

            List<long> ids = new List<long>();
            List<string> rmvNames = new List<string>();
            foreach (string viewName in lb_all.SelectedItems) {
                long id = m_viewIds[viewName];
                ids.Add(id);
                m_activeViews.Add(viewName);
                m_inactiveViews.Remove(viewName);
                rmvNames.Add(viewName);
            }

            foreach (var viewName in rmvNames)
                lb_all.Items.Remove(viewName);

            lb_active.Items.Clear();
            lb_active.Items.AddRange(m_activeViews.ToArray());
            lb_active.SelectedItems.Clear();

            Native.addActiveViews(ids.ToArray(), ids.Count, true);
        }

        private void lb_all_DoubleClick(object sender, EventArgs e) {
            Btn_add_Click(sender, e);
        }

        private void Btn_remove_Click(object sender, EventArgs e) {
            if (lb_active.SelectedItems.Count == 0)
                return;

            string[] delItems = new string[lb_active.SelectedItems.Count];
            lb_active.SelectedItems.CopyTo(delItems, 0);

            List<long> ids = new List<long>();
            List<string> rmvNames = new List<string>();
            foreach (string viewName in lb_active.SelectedItems) {
                long id = m_viewIds[viewName];
                ids.Add(id);
                m_inactiveViews.Add(viewName);
                m_activeViews.Remove(viewName);
                rmvNames.Add(viewName);
            }

            foreach (var viewName in rmvNames)
                lb_active.Items.Remove(viewName);

            lb_all.Items.Clear();
            lb_all.Items.AddRange(m_inactiveViews.ToArray());
            lb_all.SelectedItems.Clear();

            Native.removeActiveViews(ids.ToArray(), ids.Count, true);
        }

        private void lb_active_DoubleClick(object sender, EventArgs e) {
            Btn_remove_Click(sender, e);
        }

        private void Btn_del_Click(object sender, EventArgs e) {
            List<long> ids = new List<long>();

            List<string> inactDelItems = new List<string>();
            foreach (string viewName in lb_all.SelectedItems) {
                ids.Add(m_viewIds[viewName]);
                inactDelItems.Add(viewName);
                m_inactiveViews.Remove(viewName);
                m_viewIds.Remove(viewName);
            }
            foreach (string viewName in inactDelItems)
                lb_all.Items.Remove(viewName);

            List<string> actDelItems = new List<string>();
            foreach (string viewName in lb_active.SelectedItems) {
                ids.Add(m_viewIds[viewName]);
                actDelItems.Add(viewName);
                m_activeViews.Remove(viewName);
                m_viewIds.Remove(viewName);
            }
            foreach (string viewName in actDelItems)
                lb_active.Items.Remove(viewName);

            if (ids.Count > 0) {
                IFWrapper.Native.deleteViews(ids.ToArray(), ids.Count);
            }
        }
    }
}
