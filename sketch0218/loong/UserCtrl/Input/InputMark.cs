using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class InputMark : TextBox, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private DObject m_props = new DObject();

        public InputMark() {

        }

        [EditorBrowsable(EditorBrowsableState.Never), DefaultValue(null)]
        public DObject Value {
            get => m_props;

            set {
                if (value == null) {
                    this.Text = "";
                    m_props = null;
                }
                else if (!value.ContainsKey("mark_props")
                    || !((DObject)value["mark_props"]).ContainsKey("tags")) {
                    this.Text = "";
                    m_props = value.Clone();
                }
                else {
                    DObject markProps = value["mark_props"] as DObject;
                    DObject[] tags = value.GetDObject("mark_props").GetDObjectArr("tags");

                    if (tags.Length == 0)
                        this.Text = "";
                    else if (tags.Length == 1 && tags[0].GetString("name") == "text")
                        this.Text = tags[0].GetString("text");
                    else
                        this.Text = "<<标签>>";

                    m_props = value.Clone();
                }
            }
        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        protected override void OnLostFocus(EventArgs e) {
            base.OnLostFocus(e);
            this._formatValue();
        }

        protected override void OnKeyDown(KeyEventArgs e) {
            base.OnKeyDown(e);

            if (e.KeyCode == Keys.Enter) {
                this._formatValue();
                this.OnValidated(null);
            }
        }
        
        protected override void OnValidated(EventArgs e)
        {
            base.OnValidated(e);
            this._formatValue();
        }

        protected void _formatValue() {
            string text = this.Text.Trim();
            if (text == "<<标签>>") {
                return;
            }

            if (m_props == null)
                m_props = new DObject();

            if (!m_props.ContainsKey("mark_props"))
                m_props["mark_props"] = new DObject();

            if (!m_props.GetDObject("mark_props").ContainsKey("tags"))
                m_props.GetDObject("mark_props")["tags"] = new DObject[0];

            DObject[] tags = m_props.GetDObject("mark_props").GetDObjectArr("tags");
            if (tags.Length == 1
                && (int)tags[0]["type"] == 3
                && tags[0].GetString("name") == "text") {
                tags[0]["text"] = this.Text.Trim();
            }
            else {
                m_props.GetDObject("mark_props")["tags"] = new DObject[1] {
                    new object[] {
                        "type", 3,
                        "name", "text",
                        "text", this.Text.Trim(),
                        "text_color", 0,
                        "text_size", 2.5,
                        "text_font", "Source Han Serif"
                    }
                };
            }
        }
    }
}
