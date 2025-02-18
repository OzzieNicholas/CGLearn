using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{

    [Serializable]
    public struct OBB
    {
        private Vec3 m_min;
        private Vec3 m_max;
        private Mat43 m_matrix;

        static readonly int[] m_edgeIndices = new int[]{ 0, 1, 1, 2, 2, 3, 3, 0,
                                                        4, 5, 5, 6, 6, 7, 7, 4,
                                                        0, 4, 1, 5, 2, 6, 3, 7 };

        public OBB(OBB src) {
            m_min = new Vec3(src.m_min);
            m_max = new Vec3(src.m_max);
            m_matrix = new Mat43(src.m_matrix);
        }

        public OBB(Vec3 _min, Vec3 _max, Mat43 _mat) {
            m_min = _min;
            m_max = _max;
            m_matrix = _mat;
        }

        public override bool Equals(object obj) {
            if (!(obj is OBB))
                return false;

            OBB vec = (OBB)obj;
            return m_min.Equals(vec.m_min) && m_max.Equals(vec.m_max) && m_matrix.Equals(vec.m_matrix);
        }

        public override int GetHashCode() {
            return Tuple.Create(m_min, m_max, m_matrix).GetHashCode();
        }

        public override string ToString() {
            return m_min.ToString() + ">>" + m_max.ToString() + ">>" + m_matrix.ToString();
        }

        public Vec3 Min {
            get => m_min;
            set => m_min = value;
        }

        public Vec3 Max {
            get => m_max;
            set => m_max = value;
        }

        public Mat43 Matrix {
            get => m_matrix;
            set => m_matrix = value;
        }

        public static int[] EdgeIndices => m_edgeIndices;

        public bool IsValid() {
			return m_min.X <= m_max.X && m_min.Y <= m_max.Y && m_min.Z <= m_max.Z
				&& m_min.X > Double.MinValue && m_max.X < Double.MaxValue
                && m_min.Y > Double.MinValue && m_max.Y < Double.MaxValue
                && m_min.Z > Double.MinValue && m_max.Y < Double.MaxValue
                && m_max.X - m_min.X < 1e10
				&& m_max.Y - m_min.Y < 1e10
				&& m_max.Z - m_min.Z < 1e10
				&& m_matrix.X.IsValid()
				&& m_matrix.Y.IsValid()
				&& m_matrix.Z.IsValid()
				&& m_matrix.Pos.IsValid();
		}

        public Vec3[] PointsL => new Vec3[] { m_min,
                                                new Vec3(m_max.X, m_min.Y, m_min.Z),
                                                new Vec3(m_max.X, m_max.Y, m_min.Z),
                                                new Vec3(m_min.X, m_max.Y, m_min.Z),
                                                new Vec3(m_min.X, m_min.Y, m_max.Z),
                                                new Vec3(m_max.X, m_min.Y, m_max.Z),
                                                m_max,
                                                new Vec3(m_min.X, m_max.Y, m_max.Z)};

        public Vec3[] PointsW => new Vec3[] { m_matrix * m_min,
                                                m_matrix * new Vec3(m_max.X, m_min.Y, m_min.Z),
                                                m_matrix * new Vec3(m_max.X, m_max.Y, m_min.Z),
                                                m_matrix * new Vec3(m_min.X, m_max.Y, m_min.Z),
                                                m_matrix * new Vec3(m_min.X, m_min.Y, m_max.Z),
                                                m_matrix * new Vec3(m_max.X, m_min.Y, m_max.Z),
                                                m_matrix * m_max,
                                                m_matrix * new Vec3(m_min.X, m_max.Y, m_max.Z)};

        public Vec3 MinW => m_matrix * m_min;
        public Vec3 MaxW => m_matrix * m_min;
            
        public double Volume => (m_max.X - m_min.X) * (m_max.Y - m_min.Y) * (m_max.Z - m_min.Z);
        public double VolumeW => (m_max.X - m_min.X) * m_matrix.X.Length *
                                (m_max.Y - m_min.Y) * m_matrix.Y.Length *
                                (m_max.Z - m_min.Z) * m_matrix.Z.Length;

        public Vec3 Dim => m_max - m_min;
        public Vec3 DimW => new Vec3((m_max.X - m_min.X) * m_matrix.X.Length,
                                    (m_max.Y - m_min.Y) * m_matrix.Y.Length,
                                    (m_max.Z - m_min.Z) * m_matrix.Z.Length);

        public double DimX => m_max.X - m_min.X;
        public double DimY => m_max.Y - m_min.Y;
        public double DimZ => m_max.Z - m_min.Z;
        public double DimXW => (m_max.X - m_min.X) * m_matrix.X.Length;
        public double DimYW => (m_max.Y - m_min.Y) * m_matrix.Y.Length;
        public double DimZW => (m_max.Z - m_min.Z) * m_matrix.Z.Length;

        public void Add(Vec3 v) {
		    Vec3 vL = m_matrix.Inverse() * v;
            m_min = m_min.Min(vL);
		    m_max = m_max.Max(vL);
	    }

        public void Add(OBB box) {
		    Mat43 matTF = m_matrix.Inverse() * box.m_matrix;

            Vec3 pt = matTF * box.m_min;
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_max.X, box.m_min.Y, box.m_min.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_max.X, box.m_max.Y, box.m_min.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_min.X, box.m_max.Y, box.m_min.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_min.X, box.m_min.Y, box.m_max.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_max.X, box.m_min.Y, box.m_max.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * box.m_max;
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);

		    pt = matTF * new Vec3(box.m_min.X, box.m_max.Y, box.m_max.Z);
            m_min = m_min.Min(pt);
		    m_max = m_max.Max(pt);
	    }

        public void Inflate(double x, double y, double z) {
            double sx = 1.0 / m_matrix.X.Length;
            double sy = 1.0 / m_matrix.Y.Length;
            double sz = 1.0 / m_matrix.Z.Length;

            m_min.X -= x * sx;
            m_min.Y -= y * sy;
            m_min.Z -= z * sz;
            m_max.X += x * sx;
            m_max.Y += y * sy;
            m_max.Z += z * sz;
        }

        void Inflate(Vec3 v) {
			m_min.X -= v.X;
			m_min.Y -= v.Y;
			m_min.Z -= v.Z;
			m_max.X += v.X;
			m_max.Y += v.Y;
			m_max.Z += v.Z;
		}

        public void Transform(Mat43 m) {
            m_matrix = m * m_matrix;
        }

        public Vec3 Center => new Vec3((m_min.X + m_max.X) * 0.5,
                            (m_min.Y + m_max.Y) * 0.5,
                            (m_min.Z + m_max.Z) * 0.5);

        public Vec3 CenterW => m_matrix * new Vec3((m_min.X + m_max.X) * 0.5,
                                        (m_min.Y + m_max.Y) * 0.5,
                                        (m_min.Z + m_max.Z) * 0.5);

        public bool Contains(Vec3 p) {
            Vec3 v = m_matrix.Inverse() * p;
            return (v.X >= m_min.X) && (v.X <= m_max.X) &&
                    (v.Y >= m_min.Y) && (v.Y <= m_max.Y) &&
                    (v.Z >= m_min.Z) && (v.Z <= m_max.Z);
        }

        public bool Intersect(OBB bbox) {
            var pts = bbox.PointsL;
            Vec3 vMin = Vec3.MAX, vMax = Vec3.MIN;
            var tf = m_matrix.Inverse() * bbox.m_matrix;
            for (int i = 0; i < 8; ++i) {
                Vec3 pt = tf * pts[i];
                if (pt.X >= m_min.X && pt.X <= m_max.X
                    && pt.Y >= m_min.Y && pt.Y <= m_max.Y
                    && pt.Z >= m_min.Z && pt.Z <= m_max.Z)
                    return true;

                vMin.X = System.Math.Min(vMin.X, pt.X);
                vMin.Y = System.Math.Min(vMin.Y, pt.Y);
                vMin.Z = System.Math.Min(vMin.Z, pt.Z);
                vMax.X = System.Math.Max(vMax.X, pt.X);
                vMax.Y = System.Math.Max(vMax.Y, pt.Y);
                vMax.Z = System.Math.Max(vMax.Z, pt.Z);
            }

            if (vMin.X > m_max.X || vMin.Y > m_max.Y || vMin.Z > m_max.Z
                || vMax.X < m_min.X || vMax.Y < m_min.Y || vMax.Z < m_min.Z)
                return false;

            pts = this.PointsL;

            vMin = Vec3.MAX;
            vMax = Vec3.MIN;

            tf = bbox.m_matrix.Inverse() * m_matrix;
            for (int i = 0; i < 8; ++i) {
                Vec3 pt = tf * pts[i];
                if (pt.X >= m_min.X && pt.X <= m_max.X
                    && pt.Y >= m_min.Y && pt.Y <= m_max.Y
                    && pt.Z >= m_min.Z && pt.Z <= m_max.Z)
                    return true;

                vMin.X = System.Math.Min(vMin.X, pt.X);
                vMin.Y = System.Math.Min(vMin.Y, pt.Y);
                vMin.Z = System.Math.Min(vMin.Z, pt.Z);
                vMax.X = System.Math.Max(vMax.X, pt.X);
                vMax.Y = System.Math.Max(vMax.Y, pt.Y);
                vMax.Z = System.Math.Max(vMax.Z, pt.Z);
            }

            if (vMin.X > bbox.m_max.X || vMin.Y > bbox.m_max.Y || vMin.Z > bbox.m_max.Z
                || vMax.X < bbox.m_min.X || vMax.Y < bbox.m_min.Y || vMax.Z < bbox.m_min.Z)
                return false;

            return true;
        }

        public static bool operator ==(OBB box0, OBB box1) {
            return box0.m_min == box1.m_min
                && box0.m_max == box1.m_max
                && box0.m_matrix == box1.m_matrix;
        }

        public static bool operator !=(OBB box0, OBB box1) {
            return box0.m_min != box1.m_min
                || box0.m_max != box1.m_max
                || box0.m_matrix != box1.m_matrix;
        }
    }
    
}
