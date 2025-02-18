using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    class FU_File_Exit : FU_Main
    {
        private Form_MsgBox m_exitTip = new Form_MsgBox();
        private Timer m_delayTm = new Timer();

        public FU_File_Exit()
        {
            m_name = "file_exit";
            m_group = "file";
            m_icon = "res:tb_icon_cp_close";
            m_barStyle = "button";
            m_tip = "关闭程序";

            m_delayTm.Interval = 30;
            m_delayTm.Tick += onDelayClose;
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            //Save current document
            if (Native.isDocumentModifiedAfterLastSave()) {
                DialogResult dlgRes = MessageBox.Show("是否保存当前模型？", "保存", MessageBoxButtons.YesNoCancel);
                if (dlgRes == DialogResult.Cancel)
                    return;
                else if (dlgRes == DialogResult.Yes)
                    Native.saveDocument(true, "");
            }

            //Save UI
            CustomBarsProvision.instance.confirm(true);

            m_exitTip.showModal(null, "", "后台任务处理中，请勿中断", true);
            m_delayTm.Start();
        }

        private void onDelayClose(object sender, EventArgs e) {
            m_delayTm.Stop();

            MainForm.instance.disposing = true;
            MainForm.instance.Close();
            IFWrapper.Native.disposeApp(); //必须在MainForm Close后调用，否则会影响各个模块触发的退出保存逻辑

            if (!m_exitTip.IsDisposed)
                m_exitTip.closeModal();
        }

        public override bool isValid() {
            return true;
        }
    }
}
