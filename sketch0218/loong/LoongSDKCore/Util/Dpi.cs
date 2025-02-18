using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace LoongCAD.Util
{
    public class DPI
    {
        static private float m_dpi = 96.0f;
        static private float m_scaleFactor = 1.0f;

        static DPI() {
            using (var g = new Panel().CreateGraphics()) {
                m_dpi = g.DpiY;
                m_scaleFactor = m_dpi / 96.0f;
            }
        }

        static public float Dpi => m_dpi;
        static public float ScaleFactor => m_scaleFactor;

        static public int Scale(int v) {
            return (int)(m_scaleFactor * (float)v);
        }

        static public float Scale(float v) {
            return m_scaleFactor * v;
        }

        static public Point Scale(Point p) {
            return new Point((int)(m_scaleFactor * p.X), (int)(m_scaleFactor * p.Y));
        }

        static public Point Restore(Point p) {
            return new Point((int)(p.X / m_scaleFactor), (int)(p.Y / m_scaleFactor));
        }

        static public Size Scale(Size s) {
            return new Size((int)(m_scaleFactor * s.Width), (int)(m_scaleFactor * s.Height));
        }

        static public Size Restore(Size s) {
            return new Size((int)(s.Width / m_scaleFactor), (int)(s.Height / m_scaleFactor));
        }

        static public Rectangle Scale(Rectangle r) {
            return new Rectangle((int)(m_scaleFactor * r.X), (int)(m_scaleFactor * r.Y),
                                (int)(m_scaleFactor * r.Width), (int)(m_scaleFactor * r.Height));
        }

        static public Point ScalePoint(int x, int y) {
            return new Point((int)(m_scaleFactor * x), (int)(m_scaleFactor * y));
        }

        static public Size ScaleSize(int w, int h) {
            return new Size((int)(m_scaleFactor * w), (int)(m_scaleFactor * h));
        }

    }
}
