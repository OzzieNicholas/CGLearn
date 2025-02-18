using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    
    /// <summary>
    /// 带坐标系的面
    /// </summary>
    [Serializable]
    public class Plane {

        /// <summary>
        /// 空坐标系面
        /// </summary>
        public static readonly Plane NULL = new Plane(new Mat43(0.0));
        public static readonly Plane IDENTITY = new Plane(Mat43.IDENTITY);

        private Mat43 m_matrix;
        private double m_d;

        /// <summary>
        /// 默认构造函数，此构造函数会构造一个元素全为0的无效坐标面(等价于Plane.NULL)
        /// </summary>
        public Plane()
        {
            m_matrix = new Mat43(Vec3.ZERO, Vec3.ZERO, Vec3.ZERO, Vec3.ZERO);
            m_d = 0.0;
        }

        /// <summary>
        /// 通过原点、法线构造一个坐标系面
        /// </summary>
        /// <param name="o">坐标系原点</param>
        /// <param name="n">坐标系法线</param>
        public Plane(Vec3 o, Vec3 n) {
            Vec3 z = n.Normalize();
            double dotX = System.Math.Abs(z.Dot(Vec3.AXIS_X));
            double dotY = System.Math.Abs(z.Dot(Vec3.AXIS_Y));
            double dotZ = System.Math.Abs(z.Dot(Vec3.AXIS_Z));
            double maxDot = System.Math.Max(dotX, System.Math.Max(dotY, dotZ));

            Vec3 x = (maxDot == dotX) ? Vec3.AXIS_Y : Vec3.AXIS_X;
            Vec3 y = z.Cross(x);
            x = y.Cross(z);

            m_matrix = new Mat43(x.Normalize(), y.Normalize(), z, o);
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        /// <summary>
        /// 通过原点及X、Y轴构造一个坐标系面
        /// </summary>
        /// <param name="o">坐标系原点</param>
        /// <param name="x">坐标系X轴方向</param>
        /// <param name="y">坐标系Y轴方向</param>
        public Plane(Vec3 o, Vec3 x, Vec3 y) {
            Vec3 z = x.Cross(y);
            y = z.Cross(x);
            m_matrix = new Mat43(x.Normalize(), y.Normalize(), z.Normalize(), o);
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane(double m00, double m10, double m20,
                    double m01, double m11, double m21,
                    double m02, double m12, double m22,
                    double m03, double m13, double m23) {
            m_matrix = new Mat43(m00, m10, m20,
                                    m01, m11, m21,
                                    m02, m12, m22,
                                    m03, m13, m23);
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane(double[] m) {
            m_matrix = new Mat43(m);
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane(Mat43 m) {
            m_matrix = m;
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane(Plane src) {
            m_matrix = src.m_matrix;
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane Clone()
        {
            return new Plane(this);
        }

        public static Plane CreateByXZ(Vec3 o, Vec3 x, Vec3 z) {
            Vec3 y = z.Cross(x);
            x = y.Cross(z);
            return new Plane(new Mat43(x.Normalize(), y.Normalize(), z.Normalize(), o));
        }

        public Vec3 Origin {
            get => m_matrix.Pos;
            set {
                m_matrix.Pos = value;
                m_d = -m_matrix.Z.Dot(m_matrix.Pos);
            }
        }

        public Mat43 Matrix {
            get => m_matrix;
            set {
                m_matrix = value;
                m_d = -m_matrix.Z.Dot(m_matrix.Pos);
            }
        }

        public Vec3 Normal => m_matrix.Z;
        public Vec4 Coeff => new Vec4(m_matrix.Z, m_d);

        public Plane Normalize() {
            return new Plane(m_matrix.Pos, m_matrix.X.Normalize(), m_matrix.Y.Normalize());
        }

        public void NormalizeThis() {
            m_matrix.NormalizeThis();
        }

        public override bool Equals(object obj) {
            if (!(obj is Plane))
                return false;

            var pl = (Plane)obj;
            return m_matrix.Equals(pl.m_matrix);
        }

        public override int GetHashCode() {
            return m_matrix.GetHashCode();
        }

        public void SetXY(Vec3 x, Vec3 y) {
            m_matrix.X = x.Normalize();
            m_matrix.Y = y.Normalize();
            m_matrix.Z = x.Cross(y).Normalize();
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane Transform(Mat43 m) {
            return new Plane(m * m_matrix);
        }

        public void TranformThis(Mat43 m)
        {
            m_matrix = m * m_matrix;

            if (m_matrix.IsRHS())
                m_matrix.Z = m_matrix.X.Cross(m_matrix.Y).Normalize();
            else
                m_matrix.Z = m_matrix.Y.Cross(m_matrix.X).Normalize();

            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane Move(double val)
        {
            Vec3 deltaOrigin = m_matrix.Z * val;
            return new Plane(Mat43.T(deltaOrigin) * m_matrix);
        }

        public void MoveThis(double val)
        {
            m_matrix = Mat43.T(m_matrix.Z * val) * m_matrix;
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Plane Invert()
        {
            Plane newPlane = this.Clone();
            newPlane.InvertThis();
            return newPlane;
        }

        public void InvertThis()
        {
            m_matrix.X = -m_matrix.X;
            m_matrix.Z = -m_matrix.Z;
            m_d = -m_matrix.Z.Dot(m_matrix.Pos);
        }

        public Vec2 ToPoint2d(Vec3 v) {
            double dist = m_matrix.Z.Dot(v) + m_d;
            Vec3 projV = v - m_matrix.Z * dist;
            Vec3 vec2D = projV - m_matrix.Pos;
            double x = vec2D.Dot(m_matrix.X) / m_matrix.X.LengthSqr;
            double y = vec2D.Dot(m_matrix.Y) / m_matrix.Y.LengthSqr;
            return new Vec2(x, y);
        }

        public Vec3 FromPoint2d(Vec2 v) {
            return m_matrix.Pos + m_matrix.X * v.X + m_matrix.Y * v.Y;
        }

        public Vec2 ToDir2d(Vec3 vec) {
            return this.ToPoint2d(m_matrix.Pos + vec);
        }

        public Vec3 FromDir2d(Vec2 v) {
            return m_matrix.X * v.X + m_matrix.Y * v.Y;
        }

        public double Dist(Vec3 v) {
            return System.Math.Abs(m_matrix.Z.Dot(v) + m_d);
        }

        public double SignedDist(Vec3 v) {
            return m_matrix.Z.Dot(v) + m_d;
        }

        public AABB ToPlaneSpace(OBB bbox) {
            AABB ret = new AABB(Vec3.MAX, Vec3.MIN);
            if (bbox.IsValid()) {
                Mat43 mInv = m_matrix.Inverse();
                var pts = bbox.PointsW;
                for (int i = 0; i < 8; ++i) {
                    ret.Add(mInv * pts[i]);
                }
            }
            return ret;
        }

        public AABB ToPlaneSpace(AABB bbox) {
            AABB ret = new AABB(Vec3.MAX, Vec3.MIN);
            if (bbox.IsValid()) {
                Mat43 mInv = m_matrix.Inverse();
                var pts = bbox.Points;
                for (int i = 0; i < 8; ++i) {
                    ret.Add(mInv * pts[i]);
                }
            }
            return ret;
        }

        public AABB FromPlaneSpace(AABB bbox) {
            AABB ret = new AABB(Vec3.MAX, Vec3.MIN);
            var pts = bbox.Points;
            for (int i = 0; i < 8; ++i) {
                ret.Add(m_matrix * pts[i]);
            }
            return ret;
        }

        public bool IntersectRay(Vec3 vOrigin, Vec3 vDir, out double ret_t) {
		    double dot = vDir.X * m_matrix.Z.X + vDir.Y * m_matrix.Z.Y + vDir.Z * m_matrix.Z.Z;
		    if (dot * dot < vDir.LengthSqr * 1e-8) {//parallel
                ret_t = 0.0;
			    return false;
		    }

		    double t = m_matrix.Z.Dot(m_matrix.Pos - vOrigin) / dot;
            ret_t = t;
		    return true;
	    }

        public Line Intersect(Plane p) {
            Vec3 p1_normal = m_matrix.Z;
		    Vec3 p2_normal = p.m_matrix.Z;
		    Vec3 p3_normal = p1_normal.Cross(p2_normal);
            double det = p3_normal.LengthSqr;

		    if (det > 0.0) {
			    // calculate the final (point, normal)
			    var v0 = ((p3_normal.Cross(p2_normal)* m_d) +
                                (p1_normal.Cross(p3_normal)* p.m_d)) / det;
			    var v1 = v0 + p3_normal;
                return new Line(v0, v1);
		    }

		    return new Line(Vec3.ZERO, Vec3.ZERO);
        }

        public Vec3 Intersect(Plane pl0, Plane pl1) {
            Vec3 p1_normal = m_matrix.Z;
		    Vec3 p2_normal = pl0.m_matrix.Z;
		    Vec3 p3_normal = p1_normal.Cross(p2_normal);
            double det = p3_normal.LengthSqr;

		    if (det > 0.0) {
			    // calculate the final (point, normal)
			    Vec3 pt0 = ((p3_normal.Cross(p2_normal) * m_d) +
                            (p1_normal.Cross(p3_normal) * pl0.m_d)) / det;

			    if (!pl1.IntersectRay(pt0, p3_normal, out double t))
				    return Vec3.MAX;

			    return pt0 + p3_normal* t;
            }

		    return Vec3.MAX;
	    }

        bool IntersectTri(Vec3 pt0, Vec3 pt1, Vec3 pt2, out Line ret) {
            Vec3 ray0 = pt1 - pt0;
            Vec3 ray1 = pt2 - pt1;
            Vec3 ray2 = pt0 - pt2;

            double t0 = 0.0, t1 = 0.0, t2 = 0.0;
            bool ret0 = IntersectRay(pt0, ray0, out t0) && t0 >= 0.0 && t0 <= 1.0;
            bool ret1 = IntersectRay(pt1, ray1, out t1) && t1 >= 0.0 && t1 <= 1.0;
            bool ret2 = IntersectRay(pt2, ray2, out t2) && t2 >= 0.0 && t2 <= 1.0;

		    if (ret0 && ret1) {
                ret = new Line(pt0 + ray0 * t0, pt1 + ray1 * t1);
			    return true;
		    }
		    else if (ret1 && ret2) {
                ret = new Line(pt1 + ray1 * t1, pt2 + ray2 * t2);
			    return true;
		    }
		    else if (ret0 && ret2) {
                ret = new Line(pt0 + ray0 * t0, pt2 + ray2 * t2);
			    return true;
		    }

            ret = new Line(Vec3.ZERO, Vec3.ZERO);
            return false;
	    }

        public bool IntersectOBox(OBB bbox, Vec2 rMin, Vec2 rMax) {
            var vMin = bbox.Min;
		    var vMax = bbox.Max;
		    var mat = bbox.Matrix;

		    Vec3[] v = new Vec3[]{ vMin,
                                new Vec3(vMax.X, vMin.Y, vMin.Z ),
                                new Vec3(vMax.X, vMin.Y, vMax.Z ),
                                new Vec3(vMin.X, vMin.Y, vMax.Z ),
                                new Vec3(vMin.X, vMax.Y, vMin.Z ),
                                new Vec3(vMax.X, vMax.Y, vMin.Z ),
                                vMax,
                                new Vec3(vMin.X, vMax.Y, vMax.Z ) };

		    for (int i = 0; i< 8; ++i) {
			    v[i] = mat* v[i];
            }

            rMin = Vec2.MAX;
		    rMax = Vec2.MIN;
		    for (int i = 0; i < 24; i += 2) {
                Vec3 v0 = v[OBB.EdgeIndices[i]];
                Vec3 v1 = v[OBB.EdgeIndices[i + 1]];
                Vec3 dir = v1 - v0;

			    if (this.IntersectRay(v0, dir, out double t)) {
                    if (t < 0.0 || t > 1.0)
                        continue;

                    Vec2 vp = this.ToPoint2d(v0 + dir * t);
                    rMin = rMin.Min(vp);
                    rMax = rMax.Max(vp);
                }
			    else { //parallel
                    if (this.SignedDist(v0) == 0.0) {
                        Vec2 vp0 = this.ToPoint2d(v0);
                        Vec2 vp1 = this.ToPoint2d(v1);

                        rMin = rMin.Min(vp0);
                        rMin = rMin.Min(vp1);
                        rMax = rMax.Max(vp0);
                        rMax = rMax.Max(vp1);
                    }
                }
            }

		    return rMin.X < Double.MaxValue;
	    }

#if false
        public static bool operator == (Plane pl0, Plane pl1) {
		    return pl0.m_matrix == pl1.m_matrix;
	    }


        public static bool operator !=(Plane pl0, Plane pl1) {
            return pl0.m_matrix != pl1.m_matrix;
        }
#endif

        public static Plane operator *(Mat43 m, Plane pl) {
            var tf = m * pl.m_matrix;
            if (tf.IsRHS())
                tf.Z = tf.X.Cross(tf.Y).Normalize();
            else
                tf.Z = tf.Y.Cross(tf.X).Normalize();

            return new Plane(tf);
        }
    }
    
}
