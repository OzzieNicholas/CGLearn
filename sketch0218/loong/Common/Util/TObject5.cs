using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LoongCAD.Common
{
    public class TObject<T, U, V, W, M>
    {
        private T m_val0;
        private U m_val1;
        private V m_val2;
        private W m_val3;
        private M m_val4;

        public TObject()
        {
        }

        public TObject(T v0, U v1, V v2, W v3, M v4)
        {
            m_val0 = v0;
            m_val1 = v1;
            m_val2 = v2;
            m_val3 = v3;
            m_val4 = v4;
        }

        public T val0 { 
            get
            {
                return m_val0;
            } 

            set
            {
                m_val0 = value;
            }
        }

        public U val1 {
            get
            {
                return m_val1;
            }

            set
            {
                m_val1 = value;
            }
        }

        public V val2
        {
            get
            {
                return m_val2;
            }

            set
            {
                m_val2 = value;
            }
        }

        public W val3
        {
            get
            {
                return m_val3;
            }

            set
            {
                m_val3 = value;
            }
        }

        public M val4
        {
            get
            {
                return m_val4;
            }

            set
            {
                m_val4 = value;
            }
        }
    };
}

