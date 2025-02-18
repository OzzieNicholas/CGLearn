using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    public class CustomMenuItem : ToolStripMenuItem
    {
        protected string m_mode = ""; //click, check, group
        protected FuncUnit m_funcUnit = null;
        protected DObject m_paras = null;

        public CustomMenuItem(string mode) {
            m_mode = mode;

            if (m_mode == "check")
                this.CheckOnClick = true;
        }

        public string mode {
            get => m_mode;
        }

        public FuncUnit funcUnit {
            get => m_funcUnit;
            set {
                m_funcUnit = value;
                this.refresh();
            }
        }

        public DObject paras {
            get => m_paras;
            set {
                m_paras = (value != null) ? value.Clone() : null;
            }
        }

        protected override void OnClick(EventArgs e) {
            base.OnClick(e);

            /// 点击菜单组时，更新其所有子菜单
            /// 以保证每个菜单项是最新状态
            if (m_mode == "group") {
                this.refresh();
            }

            if (m_funcUnit != null)
                FuncUnit.invokeClick(m_funcUnit, this, e, Control.ModifierKeys, m_paras);
        }

        protected override void OnMouseUp(MouseEventArgs e) {
            base.OnMouseUp(e);

            if (m_funcUnit != null)
                m_funcUnit.onCheck(this.Checked, this, e, Control.ModifierKeys, m_paras);
        }

        public void refresh() {
            if (m_mode == "group") {
                foreach (var item in this.DropDownItems) {
                    if (item is CustomMenuItem)
                        (item as CustomMenuItem).refresh();
                }
            }
            else {
                if (m_funcUnit != null) {
                    if (m_mode == "click")
                        this.Image = m_funcUnit.iconImg;

                    //快捷键
                    this.ShortcutKeyDisplayString = m_funcUnit.shortCutTip;

                    if (m_mode == "check") {//check value
                        this.Checked = m_funcUnit.isChecked;
                    }
                }
                else {
                    this.Image = null;
                    this.ShortcutKeyDisplayString = "";
                }
            }


        }
    }
}
