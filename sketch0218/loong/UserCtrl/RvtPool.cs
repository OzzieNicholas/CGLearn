using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace UserCtrl
{
    public class RvtPool
    {
        //undo/redo
        protected LinkedList<Revertable> m_undo = new LinkedList<Revertable>();
        protected LinkedList<Revertable> m_redo = new LinkedList<Revertable>();
        protected int m_maxRvtQueque = -1;
        protected int m_rvtQueueNum = 0;
        protected int m_freezeRvt = 0;
        protected int m_rvtQueueDepth = 0;

        public RvtPool() {

        }

        public bool isRvtFreezed() {
            return m_freezeRvt > 0;
        }

        public void freezeRvt() {
            m_freezeRvt += 1;
        }

        public void unfreezeRvt() {
            if (m_freezeRvt > 0)
                m_freezeRvt -= 1;
        }

        public void clearRvts() {
            m_undo.Clear();
            m_redo.Clear();

            m_rvtQueueDepth = 0;
            m_rvtQueueNum = 0;
        }

        public void beginRvtQueue() {
            //check if exceed the max length
            m_rvtQueueDepth += 1;
            if (m_rvtQueueDepth == 1) {//top level queue
                m_rvtQueueNum += 1;

                if (m_maxRvtQueque > 0 && m_rvtQueueNum > m_maxRvtQueque) {
                    int depth = 0;
                    while (m_undo.Count > 0) {
                        Revertable pRvt = m_undo.First();
                        m_undo.RemoveFirst();

                        if (pRvt is Rvt_Begin) {
                            depth += 1;
                        }
                        else if (pRvt is Rvt_End) {
                            depth -= 1;
                            if (depth == 0) { //top level queue
                                m_rvtQueueNum -= 1;
                                if (m_rvtQueueNum <= m_maxRvtQueque)
                                    break;
                            }
                        }
                    }
                }
            }

            //push begin flag to undo queue
            m_undo.AddLast(new Rvt_Begin());
        }
        public void pushRvt(Revertable pRvt) {
            m_undo.AddLast(pRvt);
        }

        public void endRvtQueue() {
            if (this.isRvtFreezed())
                return;

            bool bValid = false;
            foreach (var rvt in m_undo.Reverse()) {
                if (rvt is Rvt_Begin) {
                    break;
                }
                else if (rvt.isValid()) {
                    bValid = true;
                    break;
                }
            }

            if (bValid) {//有效的queue
                m_redo.Clear();
                m_undo.AddLast(new Rvt_End());
            }
            else {//无效的queue，pop出来
                while (m_undo.Count > 0) {
                    var rvt = m_undo.Last();
                    
                    m_undo.RemoveLast();
                    
                    if (rvt is Rvt_Begin)
                        break;
                }
            }

            m_rvtQueueDepth -= 1;
        }

        public virtual void undo() {
            this.freezeRvt();

            int m_depth = 0;
            while (m_undo.Count > 0) {
                Revertable rvt = m_undo.Last();
                m_undo.RemoveLast();

                m_redo.AddLast(rvt);

                if (rvt is Rvt_End) {
                    m_depth += 1;
                }
                else if (rvt is Rvt_Begin) {
                    m_depth -= 1;
                    if (m_depth <= 0)
                        break;
                }
                else
                    rvt.undo();
            }

            this.unfreezeRvt();
        }

        public virtual void redo() {
            this.freezeRvt();

            int m_depth = 0;
            while (m_redo.Count > 0) {
                Revertable rvt = m_redo.Last();
                m_redo.RemoveLast();

                m_undo.AddLast(rvt);

                if (rvt is Rvt_Begin) {
                    m_depth += 1;
                }
                else if (rvt is Rvt_End) {
                    m_depth -= 1;
                    if (m_depth <= 0)
                        break;
                }
                else
                    rvt.redo();
            }

            this.unfreezeRvt();
        }
    }
}
