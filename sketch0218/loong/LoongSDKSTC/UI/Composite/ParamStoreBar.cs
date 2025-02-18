using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace LoongCAD.UI
{

    /// <summary>
    /// 参数存储栏空间，一般放置在窗口顶部用来保存相关属性配置
    /// </summary>
    [ToolboxItem(true), ToolboxBitmap(typeof(Panel))]
    public partial class ParamStoreBar : UserControl
    {
        protected SortedList<string, string> m_cfgs = new SortedList<string, string>();

        protected Func<string[]> m_onResetItems = null;
        protected Func<string, bool> m_onDeleteItem = null;
        protected Func<string, bool> m_onSaveDataToItem = null;
        protected Func<string, bool> m_onLoadDataFromItem = null;
        
        /// <summary>
        /// 默认构造函数
        /// </summary>
        public ParamStoreBar() {
            InitializeComponent();
            this._initControls();
        }

        private void _initControls() {
            //this._updateLayout();
        }

        /// <summary>
        /// 布局改变响应函数
        /// </summary>
        /// <param name="e">布局改变相关事件</param>
        protected override void OnLayout(LayoutEventArgs e) {
            base.OnLayout(e);

            //this._updateLayout();
        }

        /// <summary>
        /// 重置配置回调函数
        /// </summary>
        public Func<string[]> OnResetItems {
            get => m_onResetItems;
            set => m_onResetItems = value;
        }

        /// <summary>
        /// 删除配置回调函数
        /// </summary>
        public Func<string, bool> OnDeleteItem {
            get => m_onDeleteItem;
            set => m_onDeleteItem = value;
        }

        /// <summary>
        /// 保存配置回调函数
        /// </summary>
        public Func<string, bool> OnSaveDataToItem {
            get => m_onSaveDataToItem;
            set => m_onSaveDataToItem = value;
        }

        /// <summary>
        /// 加载配置回调函数
        /// </summary>
        public Func<string, bool> OnLoadDataFromItem {
            get => m_onLoadDataFromItem;
            set => m_onLoadDataFromItem = value;
        }

        /// <summary>
        /// 重置配置
        /// </summary>
        public void Reset() {
            m_cfgs.Clear();

            string[] cfgs = null;
            if (m_onResetItems != null)
                cfgs = m_onResetItems();

            if (cfgs != null) {
                foreach (var cfg in cfgs)
                    m_cfgs.Add(cfg, cfg);
            }
            
            string oldItem = cbo_cfg.SelectedItem as string;

            cbo_cfg.Items.Clear();
            cbo_cfg.Items.AddRange(m_cfgs.Values.ToArray());
            if (oldItem != null && cbo_cfg.Items.Contains(oldItem))
                cbo_cfg.SelectedItem = oldItem;
        }

        /// <summary>
        /// 当前选择的配置名称
        /// </summary>
        public virtual string SelectedCfg {
            get => cbo_cfg.SelectedItem as string;
            set => cbo_cfg.SelectedItem = value;
        }

        private void OnButtonClick_Save(object sender, EventArgs e) {
            if (cbo_cfg.SelectedItem == null
                || m_onSaveDataToItem == null)
                return;

            m_onSaveDataToItem(cbo_cfg.SelectedItem as string);
        }

        private void OnButtonClick_Load(object sender, EventArgs e) {
            if (cbo_cfg.SelectedItem == null
                || m_onLoadDataFromItem == null)
                return;

            m_onLoadDataFromItem(cbo_cfg.SelectedItem as string);
        }

        private void OnButtonClick_Delete(object sender, EventArgs e) {
            if (cbo_cfg.SelectedItem == null
                || m_onDeleteItem == null)
                return;
            
            if (MessageBox.Show("是否删除配置：" + cbo_cfg.SelectedItem + " ?", "提示", MessageBoxButtons.YesNo) == DialogResult.No)
                return;

            if (m_onDeleteItem(cbo_cfg.SelectedItem as string)) {
                int idx = cbo_cfg.SelectedIndex;
                cbo_cfg.Items.Remove(cbo_cfg.SelectedItem);
                if (idx < cbo_cfg.Items.Count)
                    cbo_cfg.SelectedIndex = idx;
            }
        }

        private void OnButtonClick_New(object sender, EventArgs e) {
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

            if (m_onSaveDataToItem(name)) {
                m_cfgs.Add(name, name);

                int pos = m_cfgs.IndexOfKey(name);
                cbo_cfg.Items.Insert(pos, name);
                cbo_cfg.SelectedItem = name;
            }
            else
                cbo_cfg.SelectedItem = null;
        }
    
    }

}
