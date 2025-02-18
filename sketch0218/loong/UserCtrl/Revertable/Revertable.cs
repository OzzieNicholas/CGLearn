using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UserCtrl
{
    public class Revertable
    {
        public Revertable()
        {

        }

        public virtual bool isValid()
        {
            return true;
        }

        public virtual void undo()
        {

        }

        public virtual void redo()
        {

        }
    }
}
