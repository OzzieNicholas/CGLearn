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


namespace UI
{

    public class FuncUnitCall
    {
        protected FuncUnit m_funcUnit = null;
        protected string m_method = null;
        protected object m_sender = null;
        protected EventArgs m_evt = null;
        protected Keys m_ctrlKeys = Keys.None;
        protected DObject m_paras = null;

        public FuncUnitCall(FuncUnit unit, string method, object sender, EventArgs evt, Keys ctrlKeys, DObject paras) {
            m_funcUnit = unit;
            m_method = method;
            m_sender = sender;
            m_evt = evt;
            m_ctrlKeys = ctrlKeys;
            m_paras = paras;
        }

        public void invoke() {
            if (m_funcUnit == null || !m_funcUnit.isValid())
                return;

            if (m_method == "click") {
                m_funcUnit.onClick(m_sender, m_evt, m_ctrlKeys, m_paras);
            }
            else if (m_method == "double_click") {
                m_funcUnit.onDoubleClick(m_sender, m_evt, m_ctrlKeys, m_paras);
            }
            else if (m_method == "check") {
                m_funcUnit.onCheck(!m_funcUnit.isChecked, m_sender, m_evt, m_ctrlKeys, m_paras);
            }
        }
    }
}
