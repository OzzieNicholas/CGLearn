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
    public class ComboItemCollection : List<ComboItem>, ISerializable
    {
        public ComboItemCollection() {
            
        }

        public ComboItemCollection(SerializationInfo info, StreamingContext context) {
            var items = info.GetValue("Items", typeof(ComboItem[])) as ComboItem[];
            this.AddRange(items);
        }

        void ISerializable.GetObjectData(SerializationInfo oInfo, StreamingContext oContext) {
            oInfo.AddValue("Items", this.ToArray());
        }
    }
    
}
