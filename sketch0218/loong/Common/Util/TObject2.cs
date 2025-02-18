using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LoongCAD.Common
{
    public class TObject<T, U>
    {
        private T m_val0;
        private U m_val1;

        public TObject() {
        }

        public TObject(T first, U second) {
            m_val0 = first;
            m_val1 = second;
        }

        public T val0 {
            get => m_val0;
            set {
                m_val0 = value;
            }
        }

        public U val1 {
            get => m_val1;
            set {
                m_val1 = value;
            }
        }
    }
}
