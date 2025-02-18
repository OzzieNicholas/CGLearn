
using System;
using System.Drawing;

namespace LoongCAD.Geometry
{
    /**
    2-double-components vector
    */
    [Serializable]
    public struct Vec2
    {
        public double X;
        public double Y;

        public static readonly Vec2 ZERO = new Vec2(0.0, 0.0);
        public static readonly Vec2 MAX = new Vec2(Double.MaxValue, Double.MaxValue);
        public static readonly Vec2 MIN = new Vec2(Double.MinValue, Double.MinValue);
        public static readonly Vec2 AXIS_X = new Vec2(1.0, 0.0);
        public static readonly Vec2 AXIS_Y = new Vec2(0.0, 1.0);

        public Vec2(double _x, double _y) {
            X = _x;
            Y = _y;
        }

        public Vec2(Vec2 v) {
            X = v.X;
            Y = v.Y;
        }

        public override bool Equals(object obj) {
            if (!(obj is Vec2))
                return false;

            Vec2 vec = (Vec2)obj;
            return X == vec.X && Y == vec.Y;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y).GetHashCode();
        }

        public override string ToString() {
            return "" + X + ',' + Y;
        }

        public Vec2i ToVec2i() {
            return new Vec2i((int)X, (int)Y);
        }

        public Point ToPoint() {
            return new Point((int)X, (int)Y);
        }

        public PointF ToPointf() {
            return new PointF((float)X, (float)Y);
        }

        public Size ToSize() {
            return new Size((int)X, (int)Y);
        }

        public void Set(double _x, double _y) {
            X = _x;
            Y = _y;
        }

        public double Dot(Vec2 vec) {
            return X * vec.X + Y * vec.Y;
        }

        public double Cross(Vec2 vec) {
            return X * vec.Y - vec.X * Y;
        }

        public Vec2 Along(Vec2 vec) {
            return (this.Dot(vec) >= 0.0) ? this : -this;
        }

        public Vec2 Max(Vec2 vec) {
            return new Vec2(X > vec.X ? X : vec.X,
                            Y > vec.Y ? Y : vec.Y);
        }

        public Vec2 Min(Vec2 vec) {
            return new Vec2(X < vec.X ? X : vec.X,
                            Y < vec.Y ? Y : vec.Y);
        }

        public double Length => System.Math.Sqrt(X * X + Y * Y);
        public double LengthSqr() => X * X + Y * Y;
		        
        public double Dist(Vec2 vec) {
            return System.Math.Sqrt((X - vec.X) * (X - vec.X) + (Y - vec.Y) * (Y - vec.Y));
        }

        public double DistSqr(Vec3 vec) {
            return (X - vec.X) * (X - vec.X) + (Y - vec.Y) * (Y - vec.Y);
        }

        Vec2 Normalize() {
            double len = System.Math.Sqrt(X * X + Y * Y);
		    return (len > 0.0) ? new Vec2(X / len, Y / len) : Vec2.ZERO;
	    }

        void SelfNormalize() {
            double len = System.Math.Sqrt(X * X + Y * Y);
            if (len != 0.0) {
                X /= len;
                Y /= len;
            }
            else {
                X = 0.0;
                Y = 0.0;
            }
        }

        Vec2 Rotate(double rad) {
            double sine = System.Math.Sin(rad);
            double cosine = System.Math.Cos(rad);
		    return new Vec2(X * cosine - Y * sine, X * sine + Y * cosine);
        }

        double ClockRad() {
            double len0 = System.Math.Sqrt(X * X + Y * Y);
		    if (len0 < 1e-3)
			    return 0.0;

		    double nx0 = X / len0;
                double rad = System.Math.Acos(nx0); //(this->normalize()).dot(<1, 0>)

		    if (Y >= 0.0)
			    return rad;

		    return 2.0 * System.Math.PI - rad;
	    }

        public static Vec2 ClockRad(double rad) {
            return new Vec2(System.Math.Cos(rad), System.Math.Sin(rad));
        }

        public double this[int idx] {
            get => (idx == 0) ? X : (idx == 1)? Y : 0;
            set {
                switch (idx) {
                    case 0:
                    X = value;
                    return;
                    case 1:
                    Y = value;
                    return;
                }
            }
        }

        public static bool operator ==(Vec2 a, Vec2 b) {
            return a.X == b.X && a.Y == b.Y;
        }

        public static bool operator !=(Vec2 a, Vec2 b) {
            return a.X != b.X || a.Y != b.Y;
        }

        public static Vec2 operator +(Vec2 vec0, Vec2 vec1) {
            return new Vec2(vec0.X + vec1.X, vec0.Y + vec1.Y);
        }

        public static Vec2 operator +(Vec2 vec, double v) {
            return new Vec2(vec.X + v, vec.Y + v);
        }

        public static Vec2 operator +(double v, Vec2 vec) {
            return new Vec2(vec.X + v, vec.Y + v);
        }

        public static Vec2 operator -(Vec2 vec) {
            return new Vec2(-vec.X, -vec.Y);
        }

        public static Vec2 operator -(Vec2 vec0, Vec2 vec1) {
            return new Vec2(vec0.X - vec1.X, vec0.Y - vec1.Y);
        }

        public static Vec2 operator -(Vec2 vec, double v) {
            return new Vec2(vec.X - v, vec.Y - v);
        }

        public static Vec2 operator -(double v, Vec2 vec) {
            return new Vec2(v - vec.X, v - vec.Y);
        }

        public static Vec2 operator *(Vec2 vec0, Vec2 vec1) {
            return new Vec2(vec0.X * vec1.X, vec0.Y * vec1.Y);
        }

        public static Vec2 operator *(Vec2 vec, double v) {
            return new Vec2(vec.X * v, vec.Y * v);
        }

        public static Vec2 operator *(double v, Vec2 vec) {
            return new Vec2(vec.X * v, vec.Y * v);
        }

        public static Vec2 operator /(Vec2 vec0, Vec2 vec1) {
            return new Vec2(vec0.X / vec1.X, vec0.Y / vec1.Y);
        }

        public static Vec2 operator /(Vec2 vec, double v) {
            return new Vec2(vec.X / v, vec.Y / v);
        }

        public static Vec2 operator /(double v, Vec2 vec) {
            return new Vec2(v / vec.X, v / vec.Y);
        }

    }
}
