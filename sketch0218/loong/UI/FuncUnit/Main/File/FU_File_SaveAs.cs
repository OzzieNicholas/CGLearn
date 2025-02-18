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
    class FU_File_SaveAs : FU_Main
    {
        //static FU_File_SaveAs m_inst = new FU_File_SaveAs();

        public FU_File_SaveAs() {
            m_name = "file_save_as";
            m_group = "file";
            m_icon = "res:tb_icon_edit_save_as";
            m_barStyle = "button";
            m_tip = "模型另存为";
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            bool async = (paras == null || (paras.ContainsKey("async") && (bool)paras["async"]));
            bool showResult = (paras == null || (paras.ContainsKey("show_result") && (bool)paras["show_result"]));

            var docName = Native.documentName();
            if (String.IsNullOrEmpty(docName))
                return;

            Form_SaveProject dlg = new Form_SaveProject();
            DialogResult dlgRet = dlg.show(docName + "_副本");
            if (dlgRet != DialogResult.OK || String.IsNullOrEmpty(dlg.selectedProjPath))
                return;

            var prog = ProgressMsgBox.show("save_as_document", Lang.Local("progress_save_scene_message"), MainForm.instance);
            MainForm.instance.BeginInvoke(new Action(() => {
                if (Native.saveAsDocument(dlg.selectedProjPath, true, "save_as_document") != Define.DocumentSaveResult.DOC_SAVE_SUCCESS)
                    MessageBox.Show("模型保存失败", "另存为", MessageBoxButtons.OK);

                prog.Close();
            }));
        }
    }
}
