
using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    4-int-components vector
    */
    [Serializable]
    public struct Vec4i
    {
        public int X;
        public int Y;
        public int Z;
        public int W;

        public static Vec4i ZERO = new Vec4i(0, 0, 0, 0);
        public static readonly Vec4i MIN = new Vec4i(Int32.MinValue, Int32.MinValue, Int32.MinValue, Int32.MinValue);
        public static readonly Vec4i MAX = new Vec4i(Int32.MaxValue, Int32.MaxValue, Int32.MaxValue, Int32.MaxValue);
            
        public Vec4i(int _x, int _y, int _z, int _w) {
            X = _x;
            Y = _y;
            Z = _z;
            W = _w;
        }

        public Vec4i(Vec4i v) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
            W = v.W;
        }

        public Vec4i(Vec3i xyz, int _w) {
            X = xyz.X;
            Y = xyz.Y;
            Z = xyz.Z;
            W = _w;
        }

        public Vec2i XY {
            get => new Vec2i(X, Y);
            set {
                X = value.X;
                Y = value.Y;
            }
        }

        public Vec2i YZ {
            get => new Vec2i(Y, Z);
            set {
                Y = value.X;
                Z = value.Y;
            }
        }

        public Vec2i XZ {
            get => new Vec2i(X, Z);
            set {
                X = value.X;
                Z = value.Y;
            }
        }

        public Vec2i ZW {
            get => new Vec2i(Z, W);
            set {
                Z = value.X;
                W = value.Y;
            }
        }

        public Vec3i XYZ {
            get => new Vec3i(X, Y, Z);
            set {
                X = value.X;
                Y = value.Y;
                Z = value.Z;
            }
        }

        public override bool Equals(object obj) {
            if (!(obj is Vec4i))
                return false;

            Vec4i vec = (Vec4i)obj;
            return X == vec.X && Y == vec.Y && Z == vec.Z && W == vec.W;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z, W).GetHashCode();
        }

        public override string ToString() {
            return "" + X + ',' + Y + ',' + Z + ',' + W;
        }

        public void Set(int _x, int _y, int _z, int _w) {
            X = _x;
            Y = _y;
            Z = _z;
            W = _w;
        }

        public double Length => System.Math.Sqrt((double)(X * X + Y * Y + Z * Z));
        public double LengthSqr => X * X + Y * Y + Z * Z;

        public int this[int idx] {
            get => (idx == 0) ? X : (idx == 1) ? Y : (idx == 2) ? Z : (idx == 3) ? W : 0;
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

        public static bool operator ==(Vec4i a, Vec4i b) {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
        }

        public static bool operator !=(Vec4i a, Vec4i b) {
            return a.X != b.X || a.Y != b.Y || a.Z != b.Z || a.W != b.W;
        }

        public static Vec4i operator +(Vec4i vec0, Vec4i vec1) {
            return new Vec4i(vec0.X + vec1.X, vec0.Y + vec1.Y, vec0.Z + vec1.Z, vec0.W + vec1.W);
        }

        public static Vec4i operator +(Vec4i vec, int v) {
            return new Vec4i(vec.X + v, vec.Y + v, vec.Z + v, vec.W + v);
        }

        public static Vec4i operator +(int v, Vec4i vec) {
            return new Vec4i(vec.X + v, vec.Y + v, vec.Z + v, vec.W + v);
        }

        public static Vec4i operator -(Vec4i vec) {
            return new Vec4i(-vec.X, -vec.Y, -vec.Z, -vec.W);
        }

        public static Vec4i operator -(Vec4i vec0, Vec4i vec1) {
            return new Vec4i(vec0.X - vec1.X, vec0.Y - vec1.Y, vec0.Z - vec1.Z, vec0.W - vec1.W);
        }

        public static Vec4i operator -(Vec4i vec, int v) {
            return new Vec4i(vec.X - v, vec.Y - v, vec.Z - v, vec.W - v);
        }

        public static Vec4i operator -(int v, Vec4i vec) {
            return new Vec4i(v - vec.X, v - vec.Y, v - vec.Z, v - vec.W);
        }

        public static Vec4i operator *(Vec4i vec0, Vec4i vec1) {
            return new Vec4i(vec0.X * vec1.X, vec0.Y * vec1.Y, vec0.Z * vec1.Z, vec0.W * vec1.W);
        }

        public static Vec4i operator *(Vec4i vec, int v) {
            return new Vec4i(vec.X * v, vec.Y * v, vec.Z * v, vec.W * v);
        }

        public static Vec4i operator *(int v, Vec4i vec) {
            return new Vec4i(vec.X * v, vec.Y * v, vec.Z * v, vec.W * v);
        }

        public static Vec4i operator /(Vec4i vec0, Vec4i vec1) {
            return new Vec4i(vec0.X / vec1.X, vec0.Y / vec1.Y, vec0.Z / vec1.Z, vec0.W / vec1.W);
        }

        public static Vec4i operator /(Vec4i vec, int v) {
            return new Vec4i(vec.X / v, vec.Y / v, vec.Z / v, vec.W / v);
        }

        public static Vec4i operator /(int v, Vec4i vec) {
            return new Vec4i(v / vec.X, v / vec.Y, v / vec.Z, v / vec.W);
        }
    }
}
