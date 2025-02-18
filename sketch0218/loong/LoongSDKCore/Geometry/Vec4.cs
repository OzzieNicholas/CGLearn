
using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    4-double-components vector
    */
    [Serializable]
    public struct Vec4
    {
        public double X;
        public double Y;
        public double Z;
        public double W;

        public static readonly Vec4 ZERO = new Vec4(0.0, 0.0, 0.0, 0.0);
        public static readonly Vec4 MIN = new Vec4(Double.MinValue, Double.MinValue, Double.MinValue, Double.MinValue);
        public static readonly Vec4 MAX = new Vec4(Double.MaxValue, Double.MaxValue, Double.MaxValue, Double.MaxValue);
        public static readonly Vec4 IDENTITY = new Vec4(0.0, 0.0, 0.0, 1.0);
        public static readonly Vec4 AXIS_X = new Vec4(1.0, 0.0, 0.0, 1.0);
        public static readonly Vec4 AXIS_Y = new Vec4(0.0, 1.0, 0.0, 1.0);
        public static readonly Vec4 AXIS_Z = new Vec4(0.0, 0.0, 1.0, 1.0);

        public Vec4(double _x, double _y, double _z, double _w) {
            X = _x;
            Y = _y;
            Z = _z;
            W = _w;
        }

        public Vec4(Vec4 v) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
            W = v.W;
        }

        public Vec4(Vec3 v, double _w) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
            W = _w;
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

        public Vec2 ZW {
            get => new Vec2(Z, W);
            set {
                Z = value.X;
                W = value.Y;
            }
        }

        public Vec3 XYZ {
            get => new Vec3(X, Y, Z);
            set {
                X = value.X;
                Y = value.Y;
                Z = value.Z;
            }
        }

        public override bool Equals(object obj) {
            if (!(obj is Vec4))
                return false;

            Vec4 vec = (Vec4)obj;
            return X == vec.X && Y == vec.Y && Z == vec.Z && W == vec.W;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z, W).GetHashCode();
        }

        public override string ToString() {
            return "" + X + ',' + Y + ',' + Z + ',' + W;
        }

        public void Set(double _x, double _y, double _z, double _w) {
            X = _x;
            Y = _y;
            Z = _z;
            W = _w;
        }

        public Vec4 Normalize() {
            double len = System.Math.Sqrt(X * X + Y * Y + Z * Z + W * W);
            if (len == 0.0)
                return Vec4.ZERO;

            return new Vec4(X / len, Y / len, Z / len, W / len);
        }

        void NormalizeThis() {
            double len = System.Math.Sqrt(X * X + Y * Y + Z * Z + W * W);
            if (len != 0.0) {
                X /= len;
                Y /= len;
                Z /= len;
                Z /= len;
            }
            else {
                X = 0.0;
                Y = 0.0;
                Z = 0.0;
                Z = 0.0;
            }
        }

        double Dot(Vec4 vec) {
		    return X * vec.X + Y * vec.Y + Z * vec.Z + W * vec.W;
        }

        Vec4 Cross(Vec4 vec) {
            Vec3 v3CurrentVector = new Vec3(X / W, Y / W, Z / W);
            Vec3 v3TobeCrossed = new Vec3(vec.X / vec.W, vec.Y / vec.W, vec.Z / vec.W);
            Vec3 v3Crossed = v3CurrentVector.Cross(v3TobeCrossed);
            return new Vec4(v3Crossed.X, v3Crossed.Y, v3Crossed.Z, 1.0);
        }

        public double Length => System.Math.Sqrt(X * X + Y * Y + Z * Z);
        public double LengthSqr => X * X + Y * Y + Z * Z;

        public double this[int idx] {
            get => (idx == 0) ? X : (idx == 1) ? Y : (idx == 2) ? Z : (idx == 3)? W : 0;
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
                    case 3:
                    W = value;
                    return;
                }
            }
        }

        public static bool operator ==(Vec4 a, Vec4 b) {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
        }

        public static bool operator !=(Vec4 a, Vec4 b) {
            return a.X != b.X || a.Y != b.Y || a.Z != b.Z || a.W != b.W;
        }

        public static Vec4 operator +(Vec4 vec0, Vec4 vec1) {
            return new Vec4(vec0.X + vec1.X, vec0.Y + vec1.Y, vec0.Z + vec1.Z, vec0.W + vec1.W);
        }

        public static Vec4 operator +(Vec4 vec, double v) {
            return new Vec4(vec.X + v, vec.Y + v, vec.Z + v, vec.W + v);
        }

        public static Vec4 operator +(double v, Vec4 vec) {
            return new Vec4(vec.X + v, vec.Y + v, vec.Z + v, vec.W + v);
        }

        public static Vec4 operator -(Vec4 vec) {
            return new Vec4(-vec.X, -vec.Y, -vec.Z, -vec.W);
        }

        public static Vec4 operator -(Vec4 vec0, Vec4 vec1) {
            return new Vec4(vec0.X - vec1.X, vec0.Y - vec1.Y, vec0.Z - vec1.Z, vec0.W - vec1.W);
        }

        public static Vec4 operator -(Vec4 vec, double v) {
            return new Vec4(vec.X - v, vec.Y - v, vec.Z - v, vec.W - v);
        }

        public static Vec4 operator -(double v, Vec4 vec) {
            return new Vec4(v - vec.X, v - vec.Y, v - vec.Z, v - vec.W);
        }

        public static Vec4 operator *(Vec4 vec0, Vec4 vec1) {
            return new Vec4(vec0.X * vec1.X, vec0.Y * vec1.Y, vec0.Z * vec1.Z, vec0.W * vec1.W);
        }

        public static Vec4 operator *(Vec4 vec, double v) {
            return new Vec4(vec.X * v, vec.Y * v, vec.Z * v, vec.W * v);
        }

        public static Vec4 operator *(double v, Vec4 vec) {
            return new Vec4(vec.X * v, vec.Y * v, vec.Z * v, vec.W * v);
        }

        public static Vec4 operator /(Vec4 vec0, Vec4 vec1) {
            return new Vec4(vec0.X / vec1.X, vec0.Y / vec1.Y, vec0.Z / vec1.Z, vec0.W / vec1.W);
        }

        public static Vec4 operator /(Vec4 vec, double v) {
            return new Vec4(vec.X / v, vec.Y / v, vec.Z / v, vec.W / v);
        }

        public static Vec4 operator /(double v, Vec4 vec) {
            return new Vec4(v / vec.X, v / vec.Y, v / vec.Z, v / vec.W);
        }


    }
}
