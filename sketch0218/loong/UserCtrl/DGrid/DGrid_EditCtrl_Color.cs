using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserCtrl
{
    [ToolboxItem(false)]
    public class DGrid_EditCtrl_Color: ColorDialog
    {
        private static DGrid_EditCtrl_Color m_instance = null;
        private DGridCellColor m_cell = null;

        public DGrid_EditCtrl_Color()
        {
           
        }

        public static DGrid_EditCtrl_Color instance
        {
            get
            {
                if (m_instance == null)
                    m_instance = new DGrid_EditCtrl_Color();

                return m_instance;
            }
        }

        public DialogResult show(DGridCellColor cell)       {
            m_cell = cell;

            if (m_cell != null)
            {
                this.Color = m_cell.color;
            }
            else
            {
                this.Color = Color.White;
            }

            return this.ShowDialog(cell.DataGridView);
        }
    }
}
