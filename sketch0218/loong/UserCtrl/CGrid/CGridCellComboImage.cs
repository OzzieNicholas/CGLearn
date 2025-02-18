using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class CGridCellComboImage : CGridCell
    {
        Dictionary<object, int> m_key2Item = new Dictionary<object, int>();

        public CGridCellComboImage(CGrid grid) : base(grid) {
            m_type = CGridCellType.COMBOBOX_IMAGE;

            m_ctrl = new Combo();
            m_ctrl.Dock = DockStyle.Fill;
            (m_ctrl as Combo).DropDownStyle = ComboBoxStyle.DropDownList;
            (m_ctrl as Combo).dispStyle = Combo.DisplayStyle.IMAGE;
            (m_ctrl as Combo).SelectionChangeCommitted += _onCellValidated;
        }

        public System.Windows.Forms.ImageList imageList {
            get => (m_ctrl as Combo).ImageList;
            set {
                Combo cbo = m_ctrl as Combo;
                cbo.ImageList = value;
                cbo.ItemHeight = Math.Max(16, cbo.ImageList.ImageSize.Height);
            }
        }

        public System.Drawing.Image image {
            get => (m_ctrl as Combo).Image;
            set {
                Combo cbo = m_ctrl as Combo;
                cbo.Image = value;
            }
        }

        public Size imageItemSize {
            get => (m_ctrl as Combo).ImageItemSize;
            set {
                Combo cbo = m_ctrl as Combo;
                cbo.ImageItemSize = value;
            }
        }

        public Padding imageBorder {
            get => (m_ctrl as Combo).ImageBorder;
            set {
                Combo cbo = m_ctrl as Combo;
                cbo.ImageBorder = value;
            }
        }

        public Dictionary<object, int> items {
            get => m_key2Item;
            set {
                Combo cbo = m_ctrl as Combo;

                object key = cbo.SelectedKey;

                m_key2Item.Clear();

                cbo.ClearItems();
                foreach (var it in value)
                    cbo.AddItem(it.Key, it.Key, it.Value);
                cbo.UpdateItems();

                if (cbo.ComboItems.Count > 0)
                    cbo.SelectedKey = key;
            }
        }

        public override object value {
            get => (m_ctrl as Combo).SelectedKey;
            set {
                Combo cbo = m_ctrl as Combo;
                cbo.SelectedKey = value;
            }
        }

        public override bool readOnly {
            get => base.readOnly;
            set {
                m_readOnly = value;
                (m_ctrl as Combo).Enabled = !value;
            }
        }

    }

    
}
