
using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    3-double-components vector
    */
    [Serializable]
    public struct Vec3
    {
        public double X;
        public double Y;
        public double Z;

        public static readonly Vec3 ZERO = new Vec3(0.0, 0.0, 0.0);
        public static readonly Vec3 MIN = new Vec3(Double.MinValue, Double.MinValue, Double.MinValue);
        public static readonly Vec3 MAX = new Vec3(Double.MaxValue, Double.MaxValue, Double.MaxValue);
        public static readonly Vec3 AXIS_X = new Vec3(1.0, 0.0, 0.0);
        public static readonly Vec3 AXIS_Y = new Vec3(0.0, 1.0, 0.0);
        public static readonly Vec3 AXIS_Z = new Vec3(0.0, 0.0, 1.0);


        public Vec3(double _x, double _y, double _z) {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public Vec3(Vec3 v) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
        }

        public Vec3(Vec2 xy, double _z) {
            X = xy.X;
            Y = xy.Y;
            Z = _z;
        }

        public Vec3(double _x, Vec2 yz) {
            X = _x;
            Y = yz.X;
            Z = yz.Y;
        }

        public override bool Equals(object obj) {
            if (!(obj is Vec3))
                return false;

            Vec3 vec = (Vec3)obj;
            return X == vec.X && Y == vec.Y && Z == vec.Z;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z).GetHashCode();
        }

        public override string ToString() {
            return "" + X + ',' + Y + ',' + Z;
        }

        public bool IsValid() {
		    return X > -1e200 && X < 1e200
			    && Y > -1e200 && Y < 1e200
			    && Z > -1e200 && Z < 1e200;
	    }

        public Vec3i ToVec3i() {
            return new Vec3i((int)X, (int)Y, (int)Z);
        }

        public Vec2 XY {
            get => new Vec2(X, Y);
            set {
                X = value.X;
                Y = value.Y;
            }
        }

        public Vec2 YZ {
            get => new Vec2(Y, Z);
            set {
                Y = value.X;
                Z = value.Y;
            }
        }

        public Vec2 XZ {
            get => new Vec2(X, Z);
            set {
                X = value.X;
                Z = value.Y;
            }
        }

        public void Set(double _x, double _y, double _z) {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public void Set(Vec3 vec) {
            X = vec.X;
            Y = vec.Y;
            Z = vec.Z;
        }

        public Vec3 Normalize() {
            double len = System.Math.Sqrt(X * X + Y * Y + Z * Z);
            if (len == 0.0)
                return Vec3.ZERO;

            return new Vec3(X / len, Y / len, Z / len);
        }

        void NormalizeThis() {
            double len = System.Math.Sqrt(X * X + Y * Y + Z * Z);
            if (len != 0.0) {
                X /= len;
                Y /= len;
                Z /= len;
            }
            else {
                X = 0.0;
                Y = 0.0;
                Z = 0.0;
            }
        }

        public double Dot(Vec3 vec) {
            return X * vec.X + Y * vec.Y + Z * vec.Z;
        }

        public Vec3 Cross(Vec3 vec) {
            return new Vec3(
                Y * vec.Z - Z * vec.Y,
                Z * vec.X - X * vec.Z,
                X * vec.Y - Y * vec.X
            );
        }

        public Vec3 Along(Vec3 vec) {
            return (this.Dot(vec) >= 0.0) ? this : -this;
        }

        /// <summary>
        /// 判断两个向量是否平行
        /// </summary>
        /// <param name="vec">另一个向量</param>
        /// <param name="tol">点积差最大阈值，默认以最大允许角度0.1度为基准计算</param>
        /// <returns></returns>
        public bool IsParallelTo(Vec3 vec, double tol = 1.5e-6)
        {
            double len0 = this.LengthSqr;
            double len1 = vec.LengthSqr;
            double dot = (this.X * vec.X + this.Y * vec.Y + this.Z * vec.Z) / Math.Sqrt(len0 * len1);
            return (1.0 - dot < tol || 1.0 + dot < tol);
        }

        /// <summary>
        /// 判断两个向量是否垂直
        /// </summary>
        /// <param name="vec">另一个向量</param>
        /// <param name="tol">点击差最大阈值，默认为1e-3</param>
        /// <returns></returns>
        public bool IsVerticalTo(Vec3 vec, double tol = 1e-3)
        {
            const double eps = 1e-9;
            if (this.Length < eps || vec.Length < eps)
                return false;

            return Math.Abs(this.Normalize().Dot(vec.Normalize())) < tol;
        }

        public double Length => System.Math.Sqrt(X * X + Y * Y + Z * Z);
        public double LengthSqr => X * X + Y * Y + Z * Z;

        public double Dist(Vec3 vec) {
            return System.Math.Sqrt((X - vec.X) * (X - vec.X) + (Y - vec.Y) * (Y - vec.Y) + (Z - vec.Z) * (Z - vec.Z));
        }

        public double DistSqr(Vec3 vec) {
            return (X - vec.X) * (X - vec.X) + (Y - vec.Y) * (Y - vec.Y) + (Z - vec.Z) * (Z - vec.Z);
        }

        public Vec3 Abs() {
            return new Vec3(X >= 0 ? X : -X, Y >= 0 ? Y : -Y, Z >= 0 ? Z : -Z);
        }

        public Vec3 Max(Vec3 vec) {
            return new Vec3(X > vec.X ? X : vec.X,
                            Y > vec.Y ? Y : vec.Y,
                            Z > vec.Z ? Z : vec.Z);
        }

        public Vec3 Min(Vec3 vec) {
            return new Vec3(X < vec.X ? X : vec.X,
                            Y < vec.Y ? Y : vec.Y,
                            Z < vec.Z ? Z : vec.Z);
        }

        public Vec3 Lerp(Vec3 vec, double t) {
            double t0 = 1.0 - t;
            return new Vec3(X * t0 + vec.X * t,
				            Y * t0 + vec.Y * t,
				            Z * t0 + vec.Z * t );
        }

        public double this[int idx] {
            get => (idx == 0) ? X : (idx == 1) ? Y : (idx == 2)? Z : 0;
            set {
                switch (idx) {
                    case 0:
                    X = value;
                    return;
                    case 1:
                    Y = value;
                    return;
                    case 2:
                    Z = value;
                    return;
                }
            }
        }

        public static bool operator ==(Vec3 a, Vec3 b) {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
        }

        public static bool operator !=(Vec3 a, Vec3 b) {
            return a.X != b.X || a.Y != b.Y || a.Z != b.Z;
        }

        public static Vec3 operator +(Vec3 vec0, Vec3 vec1) {
            return new Vec3(vec0.X + vec1.X, vec0.Y + vec1.Y, vec0.Z + vec1.Z);
        }

        public static Vec3 operator +(Vec3 vec, double v) {
            return new Vec3(vec.X + v, vec.Y + v, vec.Z + v);
        }

        public static Vec3 operator +(double v, Vec3 vec) {
            return new Vec3(vec.X + v, vec.Y + v, vec.Z + v);
        }

        public static Vec3 operator -(Vec3 vec) {
            return new Vec3(-vec.X, -vec.Y, -vec.Z);
        }

        public static Vec3 operator -(Vec3 vec0, Vec3 vec1) {
            return new Vec3(vec0.X - vec1.X, vec0.Y - vec1.Y, vec0.Z - vec1.Z);
        }

        public static Vec3 operator -(Vec3 vec, double v) {
            return new Vec3(vec.X - v, vec.Y - v, vec.Z - v);
        }

        public static Vec3 operator -(double v, Vec3 vec) {
            return new Vec3(v - vec.X, v - vec.Y, v - vec.Z);
        }

        public static Vec3 operator *(Vec3 vec0, Vec3 vec1) {
            return new Vec3(vec0.X * vec1.X, vec0.Y * vec1.Y, vec0.Z * vec1.Z);
        }

        public static Vec3 operator *(Vec3 vec, double v) {
            return new Vec3(vec.X * v, vec.Y * v, vec.Z * v);
        }

        public static Vec3 operator *(double v, Vec3 vec) {
            return new Vec3(vec.X * v, vec.Y * v, vec.Z * v);
        }

        public static Vec3 operator /(Vec3 vec0, Vec3 vec1) {
            return new Vec3(vec0.X / vec1.X, vec0.Y / vec1.Y, vec0.Z / vec1.Z);
        }

        public static Vec3 operator /(Vec3 vec, double v) {
            return new Vec3(vec.X / v, vec.Y / v, vec.Z / v);
        }

        public static Vec3 operator /(double v, Vec3 vec) {
            return new Vec3(v / vec.X, v / vec.Y, v / vec.Z);
        }


    }
}
