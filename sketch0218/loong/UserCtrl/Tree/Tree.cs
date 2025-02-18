using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;

namespace UserCtrl
{
    public class Tree : TreeView
    {
        public Tree() {
            this.ItemHeight = this.FontHeight + 6;
            this.HideSelection = false;
            this.ShowNodeToolTips = true;
            this.DrawMode = TreeViewDrawMode.OwnerDrawText;
        }

        protected override void OnDrawNode(DrawTreeNodeEventArgs e) {
            var node = e.Node;
            if (!node.IsVisible)
                return;

            TreeNodeStates state = e.State;
            Font font = node.NodeFont != null ? node.NodeFont : node.TreeView.Font;
            Color fore = node.ForeColor;
            if (fore == Color.Empty)
                fore = node.TreeView.ForeColor;
            if (node == node.TreeView.SelectedNode) {
                fore = SystemColors.HighlightText;
                e.Graphics.FillRectangle(SystemBrushes.Highlight, e.Bounds);
                ControlPaint.DrawFocusRectangle(e.Graphics, e.Bounds, fore, SystemColors.Highlight);
                TextRenderer.DrawText(e.Graphics, node.Text, font, e.Bounds, fore, TextFormatFlags.VerticalCenter | TextFormatFlags.GlyphOverhangPadding);
            }
            else {
                e.Graphics.FillRectangle(SystemBrushes.Window, e.Bounds);
                TextRenderer.DrawText(e.Graphics, node.Text, font, e.Bounds, fore, TextFormatFlags.VerticalCenter | TextFormatFlags.GlyphOverhangPadding);
            }
        }

    }

}
