using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using System.Data;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class ProjectBaseDirCombo : UserCtrl.ComboOrg
    {
        public Action<string> onDirChanged = null;

        public ProjectBaseDirCombo() {
            this.DropDownStyle = ComboBoxStyle.DropDown;
        }

        public void reset() {
            this.Items.Clear();

            var dirs = Native.globalHistory("project_base_dir", "dir_arr") as string[];
            if (dirs != null && dirs.Length > 0) {
                this.Items.AddRange(dirs);
                this.SelectedIndex = 0;
            }
            else {
                this.Text = this.defaultDir;
            }

            if (onDirChanged != null)
                onDirChanged(this.dir);
        }

        public string dir {
            get => this.Text.Trim();
            set {
                if (value != null)
                    value = value.Trim();

                if (value == this.Text)
                    return;

                this.Text = String.IsNullOrEmpty(value)? this.defaultDir : value;
                this._onDirChanged();
            }
        }

        public string defaultDir {
            get {
                string dir = dir = Environment.GetFolderPath(Environment.SpecialFolder.Personal) + "\\LoongSTC\\Projects";
                dir = dir.Replace('/', '\\');

                while (dir.EndsWith("\\"))
                    dir = dir.Substring(0, dir.Length - 1);

                if (dir.EndsWith(":"))
                    dir += "\\";

                return dir;
            }
        }

        public void save() {
            List<string> dirs = new List<string>();
            for (int i = 0; i < Math.Min(this.Items.Count, 10); ++i) 
                dirs.Add(this.Items[i] as string);

            Native.saveGlobalHistory("project_base_dir", new DObject {
                { "dir_arr", dirs.ToArray() },
            });
        }

        protected void _onDirChanged() {
            var s = this.Text.Trim();
            for (int i = 0; i < this.Items.Count; ++i) {
                if (this.Items[i] as string == s) {
                    this.Items.RemoveAt(i);
                    this.Items.Insert(0, s);
                    this.SelectedIndex = 0;
                    this.save();

                    if (onDirChanged != null)
                        onDirChanged(s);

                    return;
                }
            }

            this.Items.Insert(0, s);
            this.SelectedIndex = 0;

            this.save();

            if (onDirChanged != null)
                onDirChanged(this.dir);
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            if (((Keys)e.KeyChar) == Keys.Enter) {
                this._onDirChanged();
                e.Handled = true;
            }
        }

        protected override void OnSelectionChangeCommitted(EventArgs e) {
            base.OnSelectionChangeCommitted(e);

            if (this.SelectedIndex > 0) {
                string s = this.SelectedItem as string;
                this.Items.RemoveAt(this.SelectedIndex);
                this.Items.Insert(0, s);
                this.SelectedIndex = 0;
                this.save();

                if (onDirChanged != null)
                    onDirChanged(s);
            }
        }
    }
    
}
