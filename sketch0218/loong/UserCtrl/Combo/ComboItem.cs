using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class ComboItem
    {
        private int m_idx = -1;
        private object m_key0 = null;
        private object m_key1 = null;
        private string m_text = "";
        private int m_imgIdx = -1;

        public ComboItem(object key0, string text) {
            m_key0 = key0;
            m_text = text;
            m_imgIdx = -1;
        }

        public ComboItem(object key0, object key1, string text) {
            m_key0 = key0;
            m_key1 = key1;
            m_text = text;
            m_imgIdx = -1;
        }

        public ComboItem(object key0, string text, int imgIdx) {
            m_key0 = key0;
            m_text = text;
            m_imgIdx = imgIdx;
        }

        public ComboItem(object key0, object key1, string text, int imgIdx) {
            m_key0 = key0;
            m_key1 = key1;
            m_text = text;
            m_imgIdx = imgIdx;
        }

        public ComboItem(int idx, object key0, string text, int imgIdx) {
            m_idx = idx;
            m_key0 = key0;
            m_text = text;
            m_imgIdx = imgIdx;
        }

        public ComboItem(int idx, object key0, object key1, string text, int imgIdx) {
            m_idx = idx;
            m_key0 = key0;
            m_key1 = key1;
            m_text = text;
            m_imgIdx = imgIdx;
        }

        public int idx {
            get => m_idx;
            set => m_idx = value;
        }

        public object key0 {
            get => m_key0;
        }

        public object key1 {
            get => m_key1;
        }

        public string text {
            get => m_text;
        }

        public int imgIdx {
            get => m_imgIdx;
            set => m_imgIdx = value;
        }
    }
    
}
