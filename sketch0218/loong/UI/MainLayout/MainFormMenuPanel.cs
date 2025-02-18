using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace UI
{
    [ToolboxItem(false)]
    public class MainFormMenuPanel : System.Windows.Forms.Panel
    {
        public static readonly int ORG_HEIGHT = 25;

        public MainFormMenuPanel() {
            this.DoubleBuffered = true;
            base.BorderStyle = BorderStyle.None;
            base.BackColor = Color.Transparent;
        }

        public MenuStrip menu {
            get {
                if (this.Controls.Count == 0)
                    return null;

                return this.Controls[0] as MenuStrip;
            }

            set {
                if (value == null)
                    return;

                if (this.Controls.Count > 0) {
                    if (this.Controls[0] == value)
                        return;

                    this.Controls.Clear();
                }

                value.Dock = DockStyle.Fill;
                this.Controls.Add(value);
            }
        }
    }
}
