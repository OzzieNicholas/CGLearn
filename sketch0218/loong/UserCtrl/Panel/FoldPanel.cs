using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using System.Drawing.Design;
using System.Diagnostics;

namespace UserCtrl
{
    [ToolboxItem(true)]
    [Designer(typeof(FoldPanelDesigner))]
    public partial class FoldPanel : UserControl
    {
        private bool m_folded = false;
        private Size m_unfoldSize = new Size(0, 0);
        public Action<FoldPanel> onFold = null;
        public Action<FoldPanel> onUnfold = null;

        public FoldPanel() {
            this.DoubleBuffered = true;

            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e) {
            base.OnLoad(e);

            pan_title.Width = this.Width;
            pan_content.Size = new Size(this.Width, Math.Max(0, this.Height - pan_title.Height));

            m_unfoldSize = this.Size;
        }

        [DefaultValue("")]
        public string title {
            get => lb_title.Text;
            set => lb_title.Text = value;
        }

        [DefaultValue(typeof(Color), "0x000000")]
        public Color titleColor {
            get => lb_title.ForeColor;
            set => lb_title.ForeColor = value;
        }

        [DefaultValue(typeof(Color), "0xD3D3D3")]
        public Color titleBackColor {
            get => pan_title.BackColor;
            set => pan_title.BackColor = value;
        }

        [DefaultValue(22)]
        public int titleHeight {
            get => pan_title.Height;
            set => pan_title.Height = value;
        }

        [DefaultValue(typeof(Color), "0xFFFFFF")]
        public new Color BackColor {
            get => base.BackColor;
            set {
                base.BackColor = value;
                pan_content.BackColor = value;
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        public FoldPanelContent content => pan_content;

        public bool folded {
            get => m_folded;
            set {
                if (m_folded == value)
                    return;

                m_folded = value;

                this.SuspendLayout();
                if (m_folded) {
                    pan_content.Visible = false;
                    m_unfoldSize = this.Size;
                    this.Size = pan_title.Size;
                }
                else {
                    pan_content.Visible = true;
                    this.Size = m_unfoldSize;
                }
                this.ResumeLayout();

                if (m_folded) {
                    if (onFold != null)
                        onFold(this);
                }
                else {
                    if (onUnfold != null)
                        onUnfold(this);
                }
            }
        }

        private void pan_title_MouseDown(object sender, MouseEventArgs e) {
            this.folded = !this.folded;
            chb_fold.Image = m_folded ? global::UserCtrl.Properties.Resources.fold_panel_fold : global::UserCtrl.Properties.Resources.fold_panel_unfold;
        }
    }

    public class FoldPanelDesigner : ParentControlDesigner
    {
        public override void Initialize(IComponent component) {
            base.Initialize(component);
            this.EnableDesignMode(((FoldPanel)this.Control).content, "FoldPanelContent");
        }
    }
}
