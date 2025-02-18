using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.Serialization;

namespace LoongCAD.UI
{
    [Serializable]
    public class ComboItem : ISerializable
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

        public ComboItem(SerializationInfo info, StreamingContext context) {
            foreach (var it in info) {
                if (it.Name == "Idx")
                    m_idx = (int)it.Value;
                else if (it.Name == "ImageIdx")
                    m_imgIdx = (int)it.Value;
                //else if (it.Name == "Key0") 
                    //m_key0 = it.Value;
                //else if (it.Name == "Key1") 
                    //m_key1 = it.Value;
            }
        }

        void ISerializable.GetObjectData(SerializationInfo oInfo, StreamingContext oContext) {
            oInfo.AddValue("Idx", m_idx);
            oInfo.AddValue("Key0", m_key0);
            oInfo.AddValue("Key1", m_key1);
            oInfo.AddValue("ImageIdx", m_imgIdx);
        }

        public int Idx {
            get => m_idx;
            set => m_idx = value;
        }

        public object Key0 => m_key0;
        public object Key1 => m_key1;
        public string Text {
            get => m_text;
            set => m_text = value;
        }

        public int ImageIdx {
            get => m_imgIdx;
            set => m_imgIdx = value;
        }

        
    }
    
}
