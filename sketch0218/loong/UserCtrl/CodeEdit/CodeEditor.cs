using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserCtrl
{
    public partial class CodeEditor : UserControl
    {
        public CodeEditor()
        {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls()
        {
            tb_line.setCodeInput(tb_code);
            tb_line.Font = tb_code.Font;
        }

        public float fontSize {
            get => tb_code.Font.Size;
            set {
                tb_code.Font = new Font(tb_code.Font.FontFamily, value);
                tb_line.Font = tb_code.Font;
            }
        }

        public void clear() {
            this.code = "";
        }

        public string code {
            get => tb_code.Text;
            set => tb_code.text = value;
        }

        public bool isModified {
            get => tb_code.isModified;
            set => tb_code.isModified = value;
        }

    }
}
