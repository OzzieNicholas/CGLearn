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
using LoongCAD.Localization;
using IFWrapper;

namespace UI
{
    class FU_File_Save : FU_Main
    {
        //static FU_File_Save m_inst = new FU_File_Save();

        public FU_File_Save() {
            m_name = "file_save";
            m_group = "file";
            m_icon = "res:tb_icon_file_save";
            m_barStyle = "button";
            m_tip = "保存模型";
            this.initShortCut( Keys.S, true, false, false);
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            bool async = (paras == null || (paras.ContainsKey("async") && paras.GetBool("async")));
            bool showResult = (paras == null || (paras.ContainsKey("show_result") && paras.GetBool("show_result")));

            if (async) {
                var dlg = ProgressMsgBox.show("save_document", Lang.Local("progress_save_scene_message"), MainForm.instance);
                MainForm.instance.BeginInvoke(new Action(() => {
                    if (Native.saveDocument(true, "save_document") != Define.DocumentSaveResult.DOC_SAVE_SUCCESS) {
                        MessageBox.Show("模型保存失败", "保存", MessageBoxButtons.OK);
                        dlg.Close();
                        return;
                    }

                    dlg.Close();
                    FadingMsgBox.show("", "保存完成", 350, 0.65, MainForm.instance);
                }));
            }
            else {
                if (Native.saveDocument(false, "save_document") != Define.DocumentSaveResult.DOC_SAVE_SUCCESS)
                    MessageBox.Show("模型保存失败", "保存", MessageBoxButtons.OK);
            }
        }
    }
}
