using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    4x4 double matrix
    */
    [Serializable]
    public struct Mat4
    {
        public Vec4 X;
        public Vec4 Y;
        public Vec4 Z;
        public Vec4 Pos;
        public static readonly Mat4 IDENTITY = new Mat4(1.0);

        public Mat4(double val) {
            X = new Vec4(val, 0.0, 0.0, 0.0);
            Y = new Vec4(0.0, val, 0.0, 0.0);
            Z = new Vec4(0.0, 0.0, val, 0.0);
            Pos = new Vec4(0.0, 0.0, 0.0, val);
        }

        public Mat4(Vec4 _x, Vec4 _y, Vec4 _z, Vec4 _pos) {
            X = _x;
            Y = _y;
            Z = _z;
            Pos = _pos;
        }

        public Mat4(double[] m) {
            X = new Vec4(m[0], m[1], m[2], m[3]);
            Y = new Vec4(m[4], m[5], m[6], m[7]);
            Z = new Vec4(m[8], m[9], m[10], m[11]);
            Pos = new Vec4(m[12], m[13], m[14], m[15]);
        }

        public Mat4(double m00, double m10, double m20, double m30,
            double m01, double m11, double m21, double m31,
            double m02, double m12, double m22, double m32,
            double m03, double m13, double m23, double m33) {
            X = new Vec4(m00, m10, m20, m30);
            Y = new Vec4(m01, m11, m21, m31);
            Z = new Vec4(m02, m12, m22, m32);
            Pos = new Vec4(m03, m13, m23, m33);
        }

        public Mat4(Mat4 src) {
            X = src.X;
            Y = src.Y;
            Z = src.Z;
            Pos = src.Pos;
        }

        public override bool Equals(object obj) {
            if (!(obj is Mat4))
                return false;

            Mat4 vec = (Mat4)obj;
            return this.X == vec.X && this.Y == vec.Y && this.Z == vec.Z && this.Pos == vec.Pos;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z, Pos).GetHashCode();
        }

        public override string ToString() {
            return X.ToString() + '\n' + Y.ToString() + '\n' + Z.ToString() + '\n' + Pos.ToString();
        }

        public Mat43 ToMat43() {
            return new Mat43(X.X, X.Y, X.Z, 
                            Y.X, Y.Y, Y.Z, 
                            Z.X, Z.Y, Z.Z,
                            Pos.X, Pos.Y, Pos.Z);
        }

        public void Set(Vec4 _x, Vec4 _y, Vec4 _z, Vec4 _pos) {
            X = _x;
            Y = _y;
            Z = _z;
            Pos = _pos;
        }

        public void Set(double[] m) {
            X = new Vec4(m[0], m[1], m[2], m[3]);
            Y = new Vec4(m[4], m[5], m[6], m[7]);
            Z = new Vec4(m[8], m[9], m[10], m[11]);
            Pos = new Vec4(m[12], m[13], m[14], m[15]);
        }

        public void Set(double m00, double m10, double m20, double m30,
                double m01, double m11, double m21, double m31,
                double m02, double m12, double m22, double m32,
                double m03, double m13, double m23, double m33) {
            X.Set(m00, m10, m20, m30);
            Y.Set(m01, m11, m21, m31);
            Z.Set(m02, m12, m22, m32);
            Pos.Set(m03, m13, m23, m33);
        }

        public void Identity() {
            X.Set(1.0, 0.0, 0.0, 0.0);
            Y.Set(0.0, 1.0, 0.0, 0.0);
            Z.Set(0.0, 0.0, 1.0, 0.0);
            Pos.Set(0.0, 0.0, 0.0, 1.0);

        }

        public Mat4 Inverse() {
            double a11 = this.X.X, a12 = this.Y.X, a13 = this.Z.X, a14 = this.Pos.X;
            double a21 = this.X.Y, a22 = this.Y.Y, a23 = this.Z.Y, a24 = this.Pos.Y;
            double a31 = this.X.Z, a32 = this.Y.Z, a33 = this.Z.Z, a34 = this.Pos.Z;
            double a41 = this.X.W, a42 = this.Y.W, a43 = this.Z.W, a44 = this.Pos.W;

            Mat4 mInv = new Mat4();
            mInv.X.X = a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a22 * a34 * a43 - a23 * a32 * a44 - a24 * a33 * a42;
            mInv.X.Y = a21 * a34 * a43 + a23 * a31 * a44 + a24 * a33 * a41 - a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43;
            mInv.X.Z = a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a21 * a34 * a42 - a22 * a31 * a44 - a24 * a32 * a41;
            mInv.X.W = a21 * a33 * a42 + a22 * a31 * a43 + a23 * a32 * a41 - a21 * a32 * a43 - a22 * a33 * a41 - a23 * a31 * a42;
            mInv.Y.X = a12 * a34 * a43 + a13 * a32 * a44 + a14 * a33 * a42 - a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43;
            mInv.Y.Y = a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a11 * a34 * a43 - a13 * a31 * a44 - a14 * a33 * a41;
            mInv.Y.Z = a11 * a34 * a42 + a12 * a31 * a44 + a14 * a32 * a41 - a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42;
            mInv.Y.W = a11 * a32 * a43 + a12 * a33 * a41 + a13 * a31 * a42 - a11 * a33 * a42 - a12 * a31 * a43 - a13 * a32 * a41;
            mInv.Z.X = a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a12 * a24 * a43 - a13 * a22 * a44 - a14 * a23 * a42;
            mInv.Z.Y = a11 * a24 * a43 + a13 * a21 * a44 + a14 * a23 * a41 - a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43;
            mInv.Z.Z = a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a11 * a24 * a42 - a12 * a21 * a44 - a14 * a22 * a41;
            mInv.Z.W = a11 * a23 * a42 + a12 * a21 * a43 + a13 * a22 * a41 - a11 * a22 * a43 - a12 * a23 * a41 - a13 * a21 * a42;
            mInv.Pos.X = a12 * a24 * a33 + a13 * a22 * a34 + a14 * a23 * a32 - a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33;
            mInv.Pos.Y = a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a11 * a24 * a33 - a13 * a21 * a34 - a14 * a23 * a31;
            mInv.Pos.Z = a11 * a24 * a32 + a12 * a21 * a34 + a14 * a22 * a31 - a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32;
            mInv.Pos.W = a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a11 * a23 * a32 - a12 * a21 * a33 - a13 * a22 * a31;

            double fDet = a11 * a22 * a33 * a44 + a11 * a23 * a34 * a42 + a11 * a24 * a32 * a43
                + a12 * a21 * a34 * a43 + a12 * a23 * a31 * a44 + a12 * a24 * a33 * a41
                + a13 * a21 * a32 * a44 + a13 * a22 * a34 * a41 + a13 * a24 * a31 * a42
                + a14 * a21 * a33 * a42 + a14 * a22 * a31 * a43 + a14 * a23 * a32 * a41
                - a11 * a22 * a34 * a43 - a11 * a23 * a32 * a44 - a11 * a24 * a33 * a42
                - a12 * a21 * a33 * a44 - a12 * a23 * a34 * a41 - a12 * a24 * a31 * a43
                - a13 * a21 * a34 * a42 - a13 * a22 * a31 * a44 - a13 * a24 * a32 * a41
                - a14 * a21 * a32 * a43 - a14 * a22 * a33 * a41 - a14 * a23 * a31 * a42;

            fDet = 1.0f / fDet;
            mInv.X *= fDet;
            mInv.Y *= fDet;
            mInv.Z *= fDet;
            mInv.Pos *= fDet;
            return mInv;
        }

        public Vec4 this[int col] {
            get => (col == 0) ? X : (col == 1) ? Y : (col == 2) ? Z : (col == 3) ? Pos : Vec4.ZERO;
            set {
                switch (col) {
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
                    Pos = value;
                    return;
                }
            }
        }

        public double this[int row, int col] {
            get => (col == 0) ? X[row] : (col == 1) ? Y[row] : (col == 2) ? Z[row] : (col == 3) ? Pos[row] : 0.0;
            set {
                switch (col) {
                    case 0:
                    X[row] = value;
                    return;
                    case 1:
                    Y[row] = value;
                    return;
                    case 2:
                    Z[row] = value;
                    return;
                    case 3:
                    Pos[row] = value;
                    return;
                }
            }
        }

        public static bool operator ==(Mat4 mat0, Mat4 mat1) {
            return mat0.X == mat1.X
                    && mat0.Y == mat1.Y
                    && mat0.Z == mat1.Z
                    && mat0.Pos == mat1.Pos;
        }

        public static bool operator !=(Mat4 mat0, Mat4 mat1) {
            return mat0.X != mat1.X
                    || mat0.Y != mat1.Y
                    || mat0.Z != mat1.Z
                    || mat0.Pos != mat1.Pos;
        }

        public static Mat4 operator *(Mat4 mat1, Mat4 mat2) {
            return new Mat4(mat1.X.X * mat2.X.X + mat1.Y.X * mat2.X.Y + mat1.Z.X * mat2.X.Z + mat1.Pos.X * mat2.X.W,
                            mat1.X.Y * mat2.X.X + mat1.Y.Y * mat2.X.Y + mat1.Z.Y * mat2.X.Z + mat1.Pos.Y * mat2.X.W,
                            mat1.X.Z * mat2.X.X + mat1.Y.Z * mat2.X.Y + mat1.Z.Z * mat2.X.Z + mat1.Pos.Z * mat2.X.W,
                            mat1.X.W * mat2.X.X + mat1.Y.W * mat2.X.Y + mat1.Z.W * mat2.X.Z + mat1.Pos.W * mat2.X.W,
                                                                                                       
                            mat1.X.X * mat2.Y.X + mat1.Y.X * mat2.Y.Y + mat1.Z.X * mat2.Y.Z + mat1.Pos.X * mat2.Y.W,
                            mat1.X.Y * mat2.Y.X + mat1.Y.Y * mat2.Y.Y + mat1.Z.Y * mat2.Y.Z + mat1.Pos.Y * mat2.Y.W,
                            mat1.X.Z * mat2.Y.X + mat1.Y.Z * mat2.Y.Y + mat1.Z.Z * mat2.Y.Z + mat1.Pos.Z * mat2.Y.W,
                            mat1.X.W * mat2.Y.X + mat1.Y.W * mat2.Y.Y + mat1.Z.W * mat2.Y.Z + mat1.Pos.W * mat2.Y.W,
                                                                                                       
                            mat1.X.X * mat2.Z.X + mat1.Y.X * mat2.Z.Y + mat1.Z.X * mat2.Z.Z + mat1.Pos.X * mat2.Z.W,
                            mat1.X.Y * mat2.Z.X + mat1.Y.Y * mat2.Z.Y + mat1.Z.Y * mat2.Z.Z + mat1.Pos.Y * mat2.Z.W,
                            mat1.X.Z * mat2.Z.X + mat1.Y.Z * mat2.Z.Y + mat1.Z.Z * mat2.Z.Z + mat1.Pos.Z * mat2.Z.W,
                            mat1.X.W * mat2.Z.X + mat1.Y.W * mat2.Z.Y + mat1.Z.W * mat2.Z.Z + mat1.Pos.W * mat2.Z.W,

                            mat1.X.X * mat2.Pos.X + mat1.Y.X * mat2.Pos.Y + mat1.Z.X * mat2.Pos.Z + mat1.Pos.X * mat2.Pos.W,
                            mat1.X.Y * mat2.Pos.X + mat1.Y.Y * mat2.Pos.Y + mat1.Z.Y * mat2.Pos.Z + mat1.Pos.Y * mat2.Pos.W,
                            mat1.X.Z * mat2.Pos.X + mat1.Y.Z * mat2.Pos.Y + mat1.Z.Z * mat2.Pos.Z + mat1.Pos.Z * mat2.Pos.W,
                            mat1.X.W * mat2.Pos.X + mat1.Y.W * mat2.Pos.Y + mat1.Z.W * mat2.Pos.Z + mat1.Pos.W * mat2.Pos.W);
        }

        public static Vec3 operator *(Mat4 mat, Vec3 vec) {
            double fInvW = 1.0 / (mat.X.W * vec.X + mat.Y.W * vec.Y + mat.Z.W * vec.Z + mat.Pos.W);
            return new Vec3((mat.X.X * vec.X + mat.Y.X * vec.Y + mat.Z.X * vec.Z + mat.Pos.X) * fInvW,
                            (mat.X.Y * vec.X + mat.Y.Y * vec.Y + mat.Z.Y * vec.Z + mat.Pos.Y) * fInvW,
                            (mat.X.Z * vec.X + mat.Y.Z * vec.Y + mat.Z.Z * vec.Z + mat.Pos.Z) * fInvW);
        }

        public static Vec4 operator *(Mat4 mat, Vec4 vec) {
            return mat.X * vec.X + mat.Y * vec.Y + mat.Z * vec.Z + mat.Pos * vec.W;
        }

    }
    
}
