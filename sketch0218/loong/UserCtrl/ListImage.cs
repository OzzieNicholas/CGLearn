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
    public class ListImage : DataGridView
    {
        ImageList m_imgs = new ImageList();

        public ListImage()
        {
            this.BackgroundColor = Color.White;
            this.ColumnHeadersVisible = false;
            this.RowHeadersVisible = false;
            this.AllowUserToResizeColumns = false;
            this.AllowUserToResizeRows = false;
            this.AllowUserToAddRows = false;
            this.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            this.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            this.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            this.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            this.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            this.MultiSelect = false;
            this.ColumnCount = 1;
            this.RowCount = 1;
        }

        public int count
        {
            get
            {
                return (m_imgs != null) ?  m_imgs.Images.Count : 0;
            }
        }

        public Size imageSize
        {
            get
            {
                return m_imgs.ImageSize;
            }

            set
            {
                if (m_imgs.ImageSize.Width == value.Width && m_imgs.ImageSize.Height == value.Height)
                    return;

                m_imgs.ImageSize = value;

                this.SuspendLayout();

                foreach (DataGridViewRow row in this.Rows)
                    row.Height = m_imgs.ImageSize.Height + 2;

                this.ResumeLayout();
            }
        }

        public ImageList imageList
        {
            get
            {
                return m_imgs;
            }

            set
            {
                m_imgs = value;

                this.SuspendLayout();
                this.RowCount = Math.Max(1, m_imgs.Images.Count);
                for (int r = 0; r < m_imgs.Images.Count; ++r)
                {
                    DataGridViewImageCell cell = new DataGridViewImageCell();
                    cell.ImageLayout = DataGridViewImageCellLayout.Zoom;
                    cell.Value = m_imgs.Images[r];
                    this[0, r] = cell;
                    this.Rows[r].Height = m_imgs.ImageSize.Height + 2;
                }
                this.ResumeLayout();
            }
        }

        public void clear()
        {
            m_imgs.Images.Clear();

            this.SuspendLayout();
            this.RowCount = 1;
            this[0, 0].Value = null;
            this.ResumeLayout();
        }

        public void add(Image img)
        {
            m_imgs.Images.Add(img);

            this.SuspendLayout();

            if (this.RowCount != m_imgs.Images.Count)
                this.RowCount = m_imgs.Images.Count;

            int r = this.RowCount - 1;
            
            DataGridViewImageCell cell = new DataGridViewImageCell();
            cell.ImageLayout = DataGridViewImageCellLayout.Zoom;
            cell.Value = img;
            this[0, r] = cell;
            this.Rows[r].Height = m_imgs.ImageSize.Height + 2;

            this.ResumeLayout();
        }

        public void remove(int idx)
        {
            if (idx >= m_imgs.Images.Count)
                return;

            m_imgs.Images.RemoveAt(idx);

            this.SuspendLayout();

            for (int r = idx + 1; r < this.RowCount; ++r)
            {
                var img = this[0, r].Value;
                this[0, r].Value = null;
                this[0, r - 1].Value = img;
            }
                
            this.RowCount -= 1;
            this.ResumeLayout();
        }

        public void swap(int r0, int r1)
        {
            if (r0 == r1 || r0 >= m_imgs.Images.Count || r1 >= m_imgs.Images.Count)
                return;

            var img0 = this[0, r0].Value;
            var img1 = this[0, r1].Value;

            this.SuspendLayout();
            this[0, r0].Value = img1;
            this[0, r1].Value = img0;
            this.ResumeLayout();
        }

        public int selectedIndex
        {
            get
            {
                if (this.SelectedRows.Count == 0)
                    return -1;

                int r = this.SelectedRows[0].Index;
                if (r >= m_imgs.Images.Count)
                    return -1;

                return r;
            }

            set
            {
                this.ClearSelection();

                if (value < 0 || value >= m_imgs.Images.Count)
                    return;
                   
                this.Rows[value].Selected = true;    
            }
        }

        public Image selectedImage
        {
            get
            {
                if (this.SelectedRows.Count == 0)
                    return null;

                int r = this.SelectedRows[0].Index;
                if (r >= m_imgs.Images.Count)
                    return null;

                return m_imgs.Images[r];
            }
        }
    }

    
}
