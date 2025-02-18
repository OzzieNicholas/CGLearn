using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public partial class Form_ScriptEditor : UI.ParaForm
    {
        private static Form_ScriptEditor m_instance = new Form_ScriptEditor();
        private UserCtrl.CodeEditor m_script = null;
        private TextBox m_result = null;
        private IOHandler m_ioHandler = null;

        public static Form_ScriptEditor instance {
            get => m_instance;
        }

        public Form_ScriptEditor() {
            InitializeComponent();

            this.DoubleBuffered = true;
            this.KeyPreview = true;
            this.Owner = MainForm.instance;

            m_script = new UserCtrl.CodeEditor();
            m_script.Dock = DockStyle.Fill;
            split_main.Panel1.Controls.Add(m_script);

            m_result = new TextBox();
            m_result.Dock = DockStyle.Fill;
            m_result.AutoSize = false;
            m_result.BorderStyle = BorderStyle.FixedSingle;
            m_result.BackColor = Color.LightGray;
            m_result.Multiline = true;
            m_result.ScrollBars = ScrollBars.Both;
            m_result.ReadOnly = true;
            split_main.Panel2.Controls.Add(m_result);

            m_ioHandler = new IOHandler(this);
            m_ioHandler.keyDown += onGlobalKeyDown;
        }

        public void show() {
            if (this.Visible) {
                this.Focus();
                return;
            }

            var qry = Native.getCommonCfg("CFG_COMMON_SCRIPT", "$runtime");
            if (qry != null && qry.ContainsKey("script")) {
                m_script.code = qry.GetString("script");
            }

            this.Visible = true;
            this.Focus();
        }

        protected override void OnVisibleChanged(EventArgs e) {
            base.OnVisibleChanged(e);

            if (this.Visible)
                m_ioHandler.start();
            else
                m_ioHandler.stop();
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (e.CloseReason == CloseReason.UserClosing) {
                this.save();
            }

            base.OnFormClosing(e);
        }

        private void Btn_play_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_COMMON_SCRIPT", "$runtime", new DObject { { "script", m_script.code } }, true, false);
            Native.runCommonScript(m_script.code, true, out string result, out string errMsg);
            
            if (errMsg.Length == 0)
                m_result.AppendText("[Success]  " + DateTime.Now.ToString() + "  >>>>>>>>>>\r\n");
            else
                m_result.AppendText("[Failed]  " + DateTime.Now.ToString() + "  >>>>>>>>>>\r\n");

            m_result.AppendText(result);
            if (!result.EndsWith("\n"))
                m_result.AppendText("\r\n");

            m_result.AppendText("\r\n");

            if (errMsg.Length > 0) {
                m_result.AppendText(errMsg);
                if (!errMsg.EndsWith("\n"))
                    m_result.AppendText("\r\n");

                m_result.AppendText("\r\n");
            }
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.save();
            this.hide();
        }

        private void Btn_save_Click(object sender, EventArgs e) {
            this.save();
        }

        private void save() {
            Native.setCommonCfg("CFG_COMMON_SCRIPT", "$runtime", new DObject { { "script", m_script.code } }, true, false);
        }

        private void Btn_clear_result_Click(object sender, EventArgs e) {
            m_result.Text = "";
        }

        public bool onGlobalKeyDown(Control sender, KeyEventArgs evt, int repeat) {
            if (evt.KeyCode == Keys.F4) {
                Btn_play_Click(null, null);
            }
            else if (evt.KeyCode == Keys.S && evt.Control) {
                Btn_save_Click(null, null);
            }

            return false;
        }


    }
}
