using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(true)]
    public partial class CommonCfgBar : UserControl
    {
        private ParaForm m_form = null;
        private string m_cfgType = "";
        private SortedList<string, string> m_cfgs = new SortedList<string, string>();

        public Func<DObject> getClientProps = null;
        public Action<DObject> setClientProps = null;

        public CommonCfgBar() {
            InitializeComponent();
            this._initControls();
        }

        private void _initControls() {
            cbo_cfg.KeyUp += Cbo_cfg_KeyUp;

            //this._updateLayout();
        }

        //protected override void OnLayout(LayoutEventArgs e) {
        //    base.OnLayout(e);
        //
        //    var form = this.FindForm();
        //    if (form == null)
        //        return;
        //
        //    var w = this.Width;
        //    
        //    //this._updateLayout();
        //}

        private void Cbo_cfg_KeyUp(object sender, KeyEventArgs e) {
            if (e.KeyCode == Keys.Delete) {
                if (cbo_cfg.SelectedItem != null) {
                    if (MessageBox.Show("是否删除配置：" + cbo_cfg.SelectedItem + " ?", "提示", MessageBoxButtons.YesNo) == DialogResult.No)
                        return;

                    Native.deleteCommonCfg(m_cfgType, cbo_cfg.SelectedItem as string, true);

                    int idx = cbo_cfg.SelectedIndex;
                    cbo_cfg.Items.Remove(cbo_cfg.SelectedItem);
                    if (idx < cbo_cfg.Items.Count)
                        cbo_cfg.SelectedIndex = idx;
                }
            }
        }

        public void init(ParaForm form, string cfgType) {
            m_form = form;
            m_cfgType = cfgType;
        }

        public void reset() {
            m_cfgs.Clear();

            string[] cfgs = Native.getCommonCfgsByType(m_cfgType, false);
            foreach (var cfg in cfgs)
                m_cfgs.Add(cfg, cfg);

            string oldItem = cbo_cfg.SelectedItem as string;

            cbo_cfg.Items.Clear();
            cbo_cfg.Items.AddRange(m_cfgs.Values.ToArray());
            if (oldItem != null && cbo_cfg.Items.Contains(oldItem))
                cbo_cfg.SelectedItem = oldItem;
        }

        public virtual DObject clientProps {
            get {
                if (this.getClientProps != null)
                    return this.getClientProps();

                if (m_form == null)
                    return null;

                return m_form.getParasByUI(null, null, false);
            }

            set {
                if (this.setClientProps != null)
                    this.setClientProps(value);
                else if (m_form != null)
                    m_form.updateUIByParas(value);
            }
        }

        public virtual string selectedCfg {
            get => cbo_cfg.SelectedItem as string;
            set {
                cbo_cfg.SelectedItem = value;
            }
        }

        public bool save(string name) {
            if (name == null)
                name = cbo_cfg.SelectedItem as string;

            if (name == null)
                return false;

            Native.setCommonCfg(m_cfgType, name, this.clientProps, true, false);

            if (m_cfgs.IndexOfKey(name) < 0) {
                m_cfgs.Add(name, name);

                int pos = m_cfgs.IndexOfKey(name);
                cbo_cfg.Items.Insert(pos, name);

                cbo_cfg.SelectedItem = name;
            }

            return true;
        }

        public bool load(string name) {
            if (name == null)
                name = cbo_cfg.SelectedItem as string;

            if (name == null)
                return false;

            var qry = Native.getCommonCfg(m_cfgType, name);
            if (qry == null)
                return false;

            if (cbo_cfg.Items.Contains(name))
                cbo_cfg.SelectedItem = name;

            this.clientProps = qry;
            return true;
        }

        private void btn_save_Click(object sender, EventArgs e) {
            if (cbo_cfg.SelectedItem == null)
                return;

            Native.setCommonCfg(m_cfgType, cbo_cfg.SelectedItem as string, this.clientProps, true, false);
        }

        private void btn_load_Click(object sender, EventArgs e) {
            if (cbo_cfg.SelectedItem == null)
                return;

            var qry = Native.getCommonCfg(m_cfgType, cbo_cfg.SelectedItem as string);
            if (qry == null)
                return;

            this.clientProps = qry;
        }

        private void btn_new_Click(object sender, EventArgs e) {
            string name = tb_cfg_name.Text.Trim();
            if (name.IndexOf('$') >= 0) {
                MessageBox.Show("名称包含非法字符: $", "提示", MessageBoxButtons.OK);
                return;
            }

            if (name.Length == 0)
                return;

            if (m_cfgs.IndexOfKey(name) >= 0) {
                if (MessageBox.Show("同名配置已存在，是否覆盖？", "提示", MessageBoxButtons.YesNo) == DialogResult.No)
                    return;

                m_cfgs.Remove(name);
                cbo_cfg.Items.Remove(name);
            }

            Native.setCommonCfg(m_cfgType, name, this.clientProps, true, false);

            m_cfgs.Add(name, name);

            int pos = m_cfgs.IndexOfKey(name);
            cbo_cfg.Items.Insert(pos, name);
            cbo_cfg.SelectedItem = name;
        }

        //private void btn_cfg_remove_Click(object sender, EventArgs e)
        //{
        //    if (cbo_cfg.SelectedItem == null)
        //        return;
        //
        //    Native.deleteCommonCfg("CFG_SP_COLUMN_PROPS", cbo_cfg.SelectedItem as string, true);
        //    cbo_cfg.Items.Remove(cbo_cfg.SelectedItem);
        //}

        //protected void _updateLayout() {
        //    int margin = 2;
        //    int anchorSize = this.Width
        //                                - btn_save.Width - btn_load.Width - btn_new.Width //减去三个按钮的宽度
        //                                - margin * 4 //减去每个控件间的间隔
        //                                - btn_save.Location.X * 2; //减去两边的留白(因为两边留白相同，直接用左边x2)

        //    cbo_cfg.Location = new Point(btn_save.Location.X + btn_save.Width + margin, cbo_cfg.Location.Y);
        //    cbo_cfg.Width = Math.Max(anchorSize * 3 / 5, 10);
        //    btn_load.Location = new Point(cbo_cfg.Location.X + cbo_cfg.Width + margin, btn_load.Location.Y);
        //    tb_cfg_name.Location = new Point(btn_load.Location.X + btn_load.Width + margin, tb_cfg_name.Location.Y);
        //    tb_cfg_name.Width = Math.Max(anchorSize * 2 / 5, 10);
        //    btn_new.Location = new Point(tb_cfg_name.Location.X + tb_cfg_name.Width + margin, btn_new.Location.Y);
        //}
    }
}
