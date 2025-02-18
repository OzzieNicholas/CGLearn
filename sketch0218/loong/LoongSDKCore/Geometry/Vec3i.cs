
using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    3-int-components vector
    */
    [Serializable]
    public struct Vec3i
    {
        public int X;
        public int Y;
        public int Z;

        public static readonly Vec3i ZERO = new Vec3i(0, 0, 0);
        public static readonly Vec3i MIN = new Vec3i(Int32.MinValue, Int32.MinValue, Int32.MinValue);
        public static readonly Vec3i MAX = new Vec3i(Int32.MaxValue, Int32.MaxValue, Int32.MaxValue);
        public static readonly Vec3i AXIS_X = new Vec3i(1, 0, 0);
        public static readonly Vec3i AXIS_Y = new Vec3i(0, 1, 0);
        public static readonly Vec3i AXIS_Z = new Vec3i(0, 0, 1);

        public Vec3i(int _x, int _y, int _z) {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public Vec3i(Vec3i v) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
        }

        public override bool Equals(object obj) {
            if (!(obj is Vec3i))
                return false;

            Vec3i vec = (Vec3i)obj;
            return X == vec.X && Y == vec.Y && Z == vec.Z;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z).GetHashCode();
        }

        public override string ToString() {
            return "" + X + ',' + Y + ',' + Z;
        }

        public Vec3 ToVec3() {
            return new Vec3((double)X, (double)Y, (double)Z);
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

        public void Set(int _x, int _y, int _z) {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public void Set(Vec3i vec) {
            X = vec.X;
            Y = vec.Y;
            Z = vec.Z;
        }

        public int Dot(Vec3i vec) {
            return X * vec.X + Y * vec.Y + Z * vec.Z;
        }

        public Vec3i Cross(Vec3i vec) {
            return new Vec3i(
                Y * vec.Z - Z * vec.Y,
                Z * vec.X - X * vec.Z,
                X * vec.Y - Y * vec.X
            );
        }

        public double Length => System.Math.Sqrt((double)(X * X + Y * Y + Z * Z));
        public double LengthSqr => X * X + Y * Y + Z * Z;

        public Vec3i Abs() {
            return new Vec3i(X >= 0 ? X : -X, Y >= 0 ? Y : -Y, Z >= 0 ? Z : -Z);
        }

        public Vec3i Max(Vec3i vec) {
            return new Vec3i(X > vec.X ? X : vec.X,
                            Y > vec.Y ? Y : vec.Y,
                            Z > vec.Z ? Z : vec.Z);
        }

        public Vec3i Min(Vec3i vec) {
            return new Vec3i(X < vec.X ? X : vec.X,
                            Y < vec.Y ? Y : vec.Y,
                            Z < vec.Z ? Z : vec.Z);
        }

        public int this[int idx] {
            get => (idx == 0) ? X : (idx == 1) ? Y : (idx == 2) ? Z : 0;
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

        public static bool operator ==(Vec3i a, Vec3i b) {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
        }

        public static bool operator !=(Vec3i a, Vec3i b) {
            return a.X != b.X || a.Y != b.Y || a.Z != b.Z;
        }

        public static Vec3i operator +(Vec3i vec0, Vec3i vec1) {
            return new Vec3i(vec0.X + vec1.X, vec0.Y + vec1.Y, vec0.Z + vec1.Z);
        }

        public static Vec3i operator +(Vec3i vec, int v) {
            return new Vec3i(vec.X + v, vec.Y + v, vec.Z + v);
        }

        public static Vec3i operator +(int v, Vec3i vec) {
            return new Vec3i(vec.X + v, vec.Y + v, vec.Z + v);
        }

        public static Vec3i operator -(Vec3i vec) {
            return new Vec3i(-vec.X, -vec.Y, -vec.Z);
        }

        public static Vec3i operator -(Vec3i vec0, Vec3i vec1) {
            return new Vec3i(vec0.X - vec1.X, vec0.Y - vec1.Y, vec0.Z - vec1.Z);
        }

        public static Vec3i operator -(Vec3i vec, int v) {
            return new Vec3i(vec.X - v, vec.Y - v, vec.Z - v);
        }

        public static Vec3i operator -(int v, Vec3i vec) {
            return new Vec3i(v - vec.X, v - vec.Y, v - vec.Z);
        }

        public static Vec3i operator *(Vec3i vec0, Vec3i vec1) {
            return new Vec3i(vec0.X * vec1.X, vec0.Y * vec1.Y, vec0.Z * vec1.Z);
        }

        public static Vec3i operator *(Vec3i vec, int v) {
            return new Vec3i(vec.X * v, vec.Y * v, vec.Z * v);
        }

        public static Vec3i operator *(int v, Vec3i vec) {
            return new Vec3i(vec.X * v, vec.Y * v, vec.Z * v);
        }

        public static Vec3i operator /(Vec3i vec0, Vec3i vec1) {
            return new Vec3i(vec0.X / vec1.X, vec0.Y / vec1.Y, vec0.Z / vec1.Z);
        }

        public static Vec3i operator /(Vec3i vec, int v) {
            return new Vec3i(vec.X / v, vec.Y / v, vec.Z / v);
        }

        public static Vec3i operator /(int v, Vec3i vec) {
            return new Vec3i(v / vec.X, v / vec.Y, v / vec.Z);
        }
    }
}
