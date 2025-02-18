using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;


namespace UserCtrl
{
    [Serializable]
    public class ClipBoardData
    {
        private string m_usage = "";
        private byte[] m_data = null;

        public ClipBoardData(string usage, byte[] data) {
            m_usage = usage;
            m_data = data;
        }

        public string usage {
            get => m_usage;
            set => m_usage = value;
        }

        public byte[] data {
            get => m_data;
            set => m_data = value;
        }
    }
}
