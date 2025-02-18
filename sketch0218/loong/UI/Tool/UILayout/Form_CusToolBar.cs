using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Windows.Forms;

namespace UI
{
    public partial class Form_CusToolBar : UI.ParaForm
    {
        public static Form_CusToolBar m_instance = new Form_CusToolBar();
        private CusToolBarTree m_provision = null;
        private CusToolBarGrid m_inventory = null;

        public static Form_CusToolBar instance => m_instance;

        public Form_CusToolBar() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            initGroups();
        }

        private void _initControls() {
            tb_filter.onLookUp = onFilter;

            m_provision = new CusToolBarTree(this);
            pan_provision_tree.Controls.Add(m_provision);
            m_provision.reset();

            m_inventory = new CusToolBarGrid(this, pan_inventory_tree.Width);
            pan_inventory_tree.Controls.Add(m_inventory);

            this.updateTabOrder();
        }

        private void initGroups() {
            Dictionary<string, string> mapping = CustomBarsProvision.FuncUnitGroupMapping;

            this.func_unit_group.ClearItems();
            foreach (string key in mapping.Keys) {
                this.func_unit_group.AddItem(key, mapping[key]);
            }
            this.func_unit_group.UpdateItems();
            this.func_unit_group.SelectedKey = "ALL";
        }

        private void refreshDataView()
        {
            m_provision.reset();
            m_inventory.refreshItems();
        }

        public void show() {
            //CustomBarsProvision.instance.backupPrevious();
            CustomBarsProvision.instance.confirm();
            refreshDataView();
            this.Visible = true;
        }

        private void Btn_ok_Click(object sender, EventArgs e) {
            LayoutManager.instance.refreshLayout();
            this.hide();
            refreshDataView();
            CustomBarsProvision.instance.confirm(true);
        }

        private void btn_modify_Click(object sender, EventArgs e)
        {
            LayoutManager.instance.refreshLayout();
            refreshDataView();
            CustomBarsProvision.instance.confirm(true);
        }

        private void Btn_cancel_Click(object sender, EventArgs e) {
            //CustomBarsProvision.instance.restorePrevious();
            this.hide();
            refreshDataView();
        }

        private void onFilter(UserCtrl.InputLookUp sender, string text) {
            //System.Console.WriteLine("Btn_add_Click - tb_filter.Text: " + tb_filter.Text);
            this.m_inventory.filter = text;
        }

        private void Btn_add_Click(object sender, EventArgs e) {
            if (m_inventory.SelectedCells[0] == null)
                return;
            if (m_provision.SelectedNode == null)
                return;
            string funcUnitName = m_inventory.getSelectedButtonNode();
            if (funcUnitName != null)
            {
                ButtonNode btn = m_provision.AddTreeButtonNode(funcUnitName);
                if (btn != null)
                    m_inventory.addButtonNodeInSelected(btn);
            }
        }

        private void new_bar_button_Click(object sender, EventArgs e) {
            m_provision.AddTreeBarNode(new_bar_input.Text);
            m_provision.Focus();
        }

        private void func_unit_group_SelectedIndexChanged(object sender, EventArgs e) {
            if (this.func_unit_group.SelectedIndex == -1)
                return;

            this.m_inventory.group = this.func_unit_group.SelectedStrKey;
        }

        private void btn_move_up_Click(object sender, EventArgs e) {
            m_provision.MoveUpSelected();
            m_provision.Focus();
        }

        private void btn_move_down_Click(object sender, EventArgs e) {
            m_provision.MoveDownSelected();
            m_provision.Focus();
        }

        private void btn_add_separator_Click(object sender, EventArgs e) {
            m_provision.AddSeparator();
            m_provision.Focus();
        }

        private void Btn_remove_Click(object sender, EventArgs e)
        {
            if (m_provision.SelectedNode == null)
                return;
            CustomBarsNodeBase node = m_provision.RemoveTreeBtnNode();
            if (node != null && node.GetType() == typeof(ButtonNode))
            {
                m_inventory.removeButtonNode(node as ButtonNode);
            }
            m_provision.Focus();
        }

        private void del_bar_button_Click(object sender, EventArgs e)
        {
            if (m_provision.SelectedNode == null)
                return;
            CustomBarsNodeBase node = m_provision.RemoveTreeBarNode();
            if (node != null && node.GetType() == typeof(BarNode))
            {
                CustomBarsNodeBase[] children = node.Children.ToArray();
                foreach (CustomBarsNodeBase n in children)
                {
                    n.remove();
                    m_inventory.removeButtonNode(n as ButtonNode);
                }
                node.Children.Clear();
                node.remove();
                m_provision.Focus();
            }
        }
    }
}
