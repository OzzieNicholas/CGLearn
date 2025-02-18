using System;
using System.Collections.Generic;
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
    public partial class Form_Handler_Edit_Dist : UI.ParaForm
    {
        public static Form_Handler_Edit_Dist m_instance = new Form_Handler_Edit_Dist();
        public static Form_Handler_Edit_Dist instance = m_instance;

        protected long m_objId = -1;
        protected long m_hid = -1;
        protected Define.ObjectSide m_dir = Define.ObjectSide.SIDE_LEFT;

        public Form_Handler_Edit_Dist() {
            InitializeComponent();
            this._initControls();
        }

        private void _initControls() {
            tb_dist.KeyDown += Tb_dist_KeyDown;
            chb_left.Checked = true;
            chb_both.Checked = true;
            chb_right.Checked = true;
            chb_left.Click += onDirClick;
            chb_both.Click += onDirClick;
            chb_right.Click += onDirClick;
        }

        private void onDirClick(object sender, EventArgs e) {
            if (sender == chb_left) {
                chb_left.Checked = true;
                chb_both.Checked = false;
                chb_right.Checked = false;
            }
            else if (sender == chb_both) {
                chb_left.Checked = true;
                chb_both.Checked = true;
                chb_right.Checked = true;
            }
            else if (sender == chb_right) {
                chb_left.Checked = false;
                chb_both.Checked = false;
                chb_right.Checked = true;
            }

            tb_dist.Focus();
            tb_dist.SelectAll();
        }

        public void show(long objId, long hid, double dist, Point pos, Define.ObjectSide dir, float angl) {
            if (this.Visible)
                return;

            this.updateDispUnitEnob(Form_Options.instance.props);

            m_objId = objId;
            m_hid = hid;
            m_dir = dir;

            angl -= 180.0f; //底层传上来的角度相对于屏幕+X轴
            chb_left.imageRotation = angl;
            chb_right.imageRotation = angl;

            tb_dist.Value = dist;
            
            this.Location = MainForm.instance.viewPanel.PointToScreen(pos);
            this.CreateHandle();
            this.BeginInvoke(new Action(() => {
                if (this.IsDisposed)
                    return;

                tb_dist.SelectAll();
                this.ShowDialog(MainForm.instance);
            }));
        }

        private void Tb_dist_KeyDown(object sender, KeyEventArgs e) {
            if (e.KeyCode == Keys.Enter) {
                if (m_objId >= 0) {
                    var paras = new DObject {
                        {"dist", tb_dist.Value},
                    };

                    //m_dir: 0:both, 1:head, 2:tail
                    if (chb_both.Checked)
                        paras["dir"] = 0;
                    else if (chb_left.Checked)
                        paras["dir"] = (m_dir == Define.ObjectSide.SIDE_LEFT) ? 1 : 2;
                    else if (chb_right.Checked)
                        paras["dir"] = (m_dir == Define.ObjectSide.SIDE_LEFT) ? 2 : 1;

                    //Native.modifyObjHandler(m_objId, m_hid, paras);
                }

                this.hide();
            }
        }

        protected override void OnDeactivate(EventArgs e) {
            base.OnDeactivate(e);
            this.hide();
        }
    }
}
