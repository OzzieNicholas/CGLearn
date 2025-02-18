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
    class FU_File_AutoSave : FU_Main
    {
        public FU_File_AutoSave() {
            m_name = "file_autosave";
            m_group = "file";
            m_icon = "res:tb_icon_file_autosave";
            m_barStyle = "button";
            m_tip = "自动保存";
        }

        public override bool userUnit => false;

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            MainForm.instance.statusBar.tips = "自动保存中 ...";
            MainForm.instance.BeginInvoke(new Action(() => {
                Native.saveTempDocument("");
                MainForm.instance.statusBar.tips = "自动保存完成";
            }));
        }
    }
}
