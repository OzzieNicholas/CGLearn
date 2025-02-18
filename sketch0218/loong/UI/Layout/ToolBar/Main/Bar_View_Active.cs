using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class Bar_View_Active : CustomToolBar
    {
        protected ToolStripComboBox m_cbo = new ToolStripComboBox();
        protected ToolStripButton btn_dispSettings = null;

        protected Dictionary<long, string> m_id2Name = new Dictionary<long, string>();
        protected List<TObject<long, string>> m_items = new List<TObject<long, string>>();

        public Bar_View_Active() {
            m_cbo.ComboBox.SelectionChangeCommitted += ComboBox_SelectionChangeCommitted;
            m_cbo.DropDownStyle = ComboBoxStyle.DropDownList;
            m_cbo.Size = DPI.Scale(new Size(180, 15));
            m_cbo.Font = new Font(this.Font.FontFamily, DPI.Scale(13), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);

            btn_dispSettings = new ToolStripButton();
            btn_dispSettings.ToolTipText = Lang.Local("focused_view_disp_settings");
            btn_dispSettings.ImageScaling = ToolStripItemImageScaling.None;
            btn_dispSettings.Image = Res.dpiImage("tb_icon_view_set_visible");
            btn_dispSettings.MouseDown += Btn_dispSettings_MouseDown;

            this.Items.AddRange(new ToolStripItem[] { m_cbo, btn_dispSettings });
            this.BarName = "常用视图";
        }

        public override bool IsDynamic => false;

        public Dictionary<long, string> activeViews {
            get => m_id2Name;
            set {
                if (m_cbo.IsDisposed)
                    return;

                m_cbo.Items.Clear();

                if (value == null)
                    m_id2Name.Clear();
                else
                    m_id2Name = new Dictionary<long, string>(value);

                m_items.Clear();
                foreach (var it in m_id2Name)
                    m_items.Add(new TObject<long, string>(it.Key, it.Value));
                m_items = m_items.OrderBy(x => x.val1).ToList();

                string oldSel = m_cbo.SelectedItem as string;
                m_cbo.Items.AddRange(m_items.Select(x => x.val1).ToArray());
                if (oldSel != null && m_cbo.Items.Contains(oldSel))
                    m_cbo.SelectedItem = oldSel;
            }
        }

        public long focusedId {
            get {
                if (m_cbo.IsDisposed)
                    return -1;

                if (m_cbo.SelectedIndex >= 0)
                    return m_items[m_cbo.SelectedIndex].val0;

                return -1;
            }

            set {
                if (m_cbo.IsDisposed)
                    return;

                m_cbo.SelectedIndex = m_items.FindIndex(x => x.val0 == value);
            }
        }

        private void ComboBox_SelectionChangeCommitted(object sender, EventArgs e) {
            if (m_cbo.IsDisposed || m_cbo.SelectedIndex < 0)
                return;

            long id = m_items[m_cbo.SelectedIndex].val0;
            IFWrapper.Native.setFocusedCell(id);
            MainForm.instance.viewPanel.Focus(); //NOTE: 焦点留在下拉框的话，用户可能误按快捷键，导致下拉框再次切换选中项进而视图异常切换
        }

        private void Btn_dispSettings_MouseDown(object sender, MouseEventArgs e) {
            if (String.IsNullOrEmpty(Native.documentPath())
                || Native.focusedViewId() < 0)
                return;

            if (Form_ViewDisplaySetting_Context.instance.hideFrame != Native.STCoreFrame()) {
                Form_ViewDisplaySetting_Context.instance.show(this.FindForm(), this.RectangleToScreen(this.ClientRectangle));
            }
            
        }
    }
}
