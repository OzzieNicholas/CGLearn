using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LoongCAD.Geometry
{
    public class Mat3
    {

        public Vec3 X;
        public Vec3 Y;
        public Vec3 Z;

        public Mat3()
        {
            this.X = Vec3.AXIS_X;
            this.Y = Vec3.AXIS_Y;
            this.Z = Vec3.AXIS_Z;
        }

        public Mat3(double val)
        {
            this.X = new Vec3(val, 0.0, 0.0);
            this.Y = new Vec3(0.0, val, 0.0);
            this.Z = new Vec3(0.0, 0.0, val);
        }

        public Mat3(Vec3 _x, Vec3 _y, Vec3 _z)
        {
            this.X = _x;
            this.Y = _y;
            this.Z = _z;
        }

        public Mat3(double m00, double m10, double m20,
                    double m01, double m11, double m21,
                    double m02, double m12, double m22)
        {
            this.X = new Vec3(m00, m10, m20);
            this.Y = new Vec3(m01, m11, m21);
            this.Z = new Vec3(m02, m12, m22);
        }

        public Mat3(Mat3 src)
        {
            this.X = src.X;
            this.Y = src.Y;
            this.Z = src.Z;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Mat3)) {
                return false;
            }

            Mat3 srcMat = (Mat3)obj;
            return this.X == srcMat.X && this.Y == srcMat.Y && this.Z == srcMat.Z;
        }

        public override int GetHashCode()
        {
            return Tuple.Create(X, Y, Z).GetHashCode();
        }

        public override string ToString()
        {
            return X.ToString() + '\n' + Y.ToString() + '\n' + Z.ToString();
        }

        public Mat43 ToMat43()
        {
            return new Mat43(this.X, this.Y, this.Z, Vec3.ZERO);
        }

        public Mat4 ToMat4()
        {
            return new Mat4(
                X.X, X.Y, X.Z, 0.0,
                Y.X, Y.Y, Y.Z, 0.0,
                Z.X, Z.Y, Z.Z, 0.0,
                0.0, 0.0, 0.0, 1.0);
        }

        public void Set(Vec3 _x, Vec3 _y, Vec3 _z)
        {
            this.X = _x;
            this.Y = _y;
            this.Z = _z;
        }

        public void Set(double[] m)
        {
            if (m.Length < 9)
                return;
            X.Set(m[0], m[1], m[2]);
            Y.Set(m[3], m[4], m[5]);
            Z.Set(m[6], m[7], m[8]);
        }

        public void Set(double m00, double m10, double m20,
                        double m01, double m11, double m21,
                        double m02, double m12, double m22)
        {
            X.Set(m00, m10, m20);
            Y.Set(m01, m11, m21);
            Z.Set(m02, m12, m22);
        }

        public void Identity()
        {
            this.X.Set(1.0, 0.0, 0.0);
            this.Y.Set(0.0, 1.0, 0.0);
            this.Z.Set(0.0, 0.0, 1.0);
        }

        public Mat3 Inverse()
        {
            double a11 = this.X.X, a12 = this.X.Y, a13 = this.X.Z, a14 = 0.0;
            double a21 = this.Y.X, a22 = this.Y.Y, a23 = this.Y.Z, a24 = 0.0;
            double a31 = this.Z.X, a32 = this[1][2], a33 = this.Z.Z, a34 = 0.0;
            double a41 = 0.0, a42 = 0.0, a43 = 0.0, a44 = 1.0;

            Mat3 mInv = new Mat3();
            mInv.X.X = a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a22 * a34 * a43 - a23 * a32 * a44 - a24 * a33 * a42;
            mInv.X.Y = a12 * a34 * a43 + a13 * a32 * a44 + a14 * a33 * a42 - a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43;
            mInv.X.Z = a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a12 * a24 * a43 - a13 * a22 * a44 - a14 * a23 * a42;
            mInv.Y.X = a21 * a34 * a43 + a23 * a31 * a44 + a24 * a33 * a41 - a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43;
            mInv.Y.Y = a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a11 * a34 * a43 - a13 * a31 * a44 - a14 * a33 * a41;
            mInv.Y.Z = a11 * a24 * a43 + a13 * a21 * a44 + a14 * a23 * a41 - a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43;
            mInv.Z.X = a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a21 * a34 * a42 - a22 * a31 * a44 - a24 * a32 * a41;
            mInv.Z.Y = a11 * a34 * a42 + a12 * a31 * a44 + a14 * a32 * a41 - a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42;
            mInv.Z.Z = a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a11 * a24 * a42 - a12 * a21 * a44 - a14 * a22 * a41;

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
            return mInv;
        }

        public bool IsRHS()
        {
            return X.Cross(Y).Dot(Z) >= 0.0;
        }

        public Mat3 Normalize()
        {
            return new Mat3(X.Normalize(), Y.Normalize(), Z.Normalize());
        }

        public void NormalizeThis()
        {
            this.X = this.X.Normalize();
            this.Y = this.Y.Normalize();
            this.Z = this.Z.Normalize();
        }

        public void Scale(double s)
        {
            this.X *= s;
            this.Y *= s;
            this.Z *= s;
        }

        public void Scale(double sx, double sy, double sz)
        {
            this.X.X *= sx;
            this.X.Y *= sy;
            this.X.Z *= sz;

            this.Y.X *= sx;
            this.Y.Y *= sy;
            this.Y.Z *= sz;

            this.Z.X *= sx;
            this.Z.Y *= sy;
            this.Z.Z *= sz;
        }

        public void Scale(Vec3 vec)
        {
            double sx = vec.X, sy = vec.Y, sz = vec.Z;

            this.X.X *= sx;
            this.X.Y *= sy;
            this.X.Z *= sz;

            this.Y.X *= sx;
            this.Y.Y *= sy;
            this.Y.Z *= sz;

            this.Z.X *= sx;
            this.Z.Y *= sy;
            this.Z.Z *= sz;
        }

        public static Mat3 RX(double rad)
        {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat3(
                1.0, 0.0, 0.0,
                0.0, fCosine, fSine,
                0.0, -fSine, fCosine);
        }

        public static Mat3 RY(double rad)
        {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat3(
                fCosine, 0.0, -fSine,
                0.0, 1.0, 0.0,
                fSine, 0.0, fCosine);
        }

        public static Mat3 RZ(double rad)
        {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat3(
                fCosine, fSine, 0.0,
                -fSine, fCosine, 0.0,
                0.0, 0.0, 1.0);
        }

        public static Mat3 S(double s)
        {
            return new Mat3(
                s, 0.0, 0.0,
                0.0, s, 0.0,
                0.0, 0.0, s);
        }

        public static Mat3 S(double sx, double sy, double sz)
        {
            return new Mat3(
                sx, 0.0, 0.0,
                0.0, sy, 0.0,
                0.0, 0.0, sz);
        }

        public static Mat3 CreateByXZ(Vec3 vX, Vec3 vZ)
        {
            Vec3 x = vX;
            Vec3 z = vZ;
            Vec3 y = z.Cross(x);
            if (y.LengthSqr == 0.0) {
                x = Vec3.AXIS_X;
                y = z.Cross(x);

                if (y.LengthSqr == 0.0) {
                    x = Vec3.AXIS_Y;
                    y = z.Cross(x);
                }
            }

            x = y.Cross(z);

            return new Mat3(x.Normalize(), y.Normalize(), z.Normalize());
        }

        public static Mat3 CreateByYZ(Vec3 vY, Vec3 vZ)
        {
            Vec3 y = vY;
            Vec3 z = vZ;
            Vec3 x = y.Cross(z);
            if (x.LengthSqr < 1e-2) {
                y = Vec3.AXIS_Y;
                x = y.Cross(z);

                if (x.LengthSqr < 1e-2) {
                    y = Vec3.AXIS_Z;
                    x = y.Cross(z);
                }
            }

            y = z.Cross(x);

            return new Mat3(x.Normalize(), y.Normalize(), z.Normalize());
        }

        public static Mat3 CreateByEuler(double radX, double radY, double radZ)
        {
            double su = System.Math.Sin(radX), cu = System.Math.Cos(radX);
            double sv = System.Math.Sin(radZ), cv = System.Math.Cos(radZ);
            double sw = System.Math.Sin(radY), cw = System.Math.Cos(radY);
            return new Mat3(cv * cw,
                            cv * sw,
                            -sv,

                            su * sv * cw - cu * sw,
                            cu * cw + su * sv * sw,
                            su * cv,

                            su * sw + cu * sv * cw,
                            cu * sv * sw - su * cw,
                            cu * cv);
        }

        public Vec3 this[int col]
        {
            get => (col == 0) ? X : (col == 1) ? Y : (col == 2) ? Z : Vec3.ZERO;
            set
            {
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
                }
            }
        }

        public double this[int row, int col]
        {
            get => (col == 0) ? X[row] : (col == 1) ? Y[row] : (col == 2) ? Z[row] : 0.0;
            set
            {
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
                }
            }
        }

        public static bool operator ==(Mat3 mat0, Mat3 mat1)
        {
            return mat0.X == mat1.X
                    && mat0.Y == mat1.Y
                    && mat0.Z == mat1.Z;
        }

        public static bool operator !=(Mat3 mat0, Mat3 mat1)
        {
            return mat0.X != mat1.X
                    || mat0.Y != mat1.Y
                    || mat0.Z != mat1.Z;
        }

        public static Mat3 operator *(Mat3 mat1, Mat3 mat2)
        {
            return new Mat3(mat1.X.X * mat2.X.X + mat1.Y.X * mat2.X.Y + mat1.Z.X * mat2.X.Z,
                            mat1.X.Y * mat2.X.X + mat1.Y.Y * mat2.X.Y + mat1.Z.Y * mat2.X.Z,
                            mat1.X.Z * mat2.X.X + mat1.Y.Z * mat2.X.Y + mat1.Z.Z * mat2.X.Z,

                            mat1.X.X * mat2.Y.X + mat1.Y.X * mat2.Y.Y + mat1.Z.X * mat2.Y.Z,
                            mat1.X.Y * mat2.Y.X + mat1.Y.Y * mat2.Y.Y + mat1.Z.Y * mat2.Y.Z,
                            mat1.X.Z * mat2.Y.X + mat1.Y.Z * mat2.Y.Y + mat1.Z.Z * mat2.Y.Z,

                            mat1.X.X * mat2.Z.X + mat1.Y.X * mat2.Z.Y + mat1.Z.X * mat2.Z.Z,
                            mat1.X.Y * mat2.Z.X + mat1.Y.Y * mat2.Z.Y + mat1.Z.Y * mat2.Z.Z,
                            mat1.X.Z * mat2.Z.X + mat1.Y.Z * mat2.Z.Y + mat1.Z.Z * mat2.Z.Z);
        }

        public static Vec3 operator *(Mat3 mat, Vec3 vec)
        {
            return new Vec3((mat.X.X * vec.X + mat.Y.X * vec.Y + mat.Z.X * vec.Z),
                            (mat.X.Y * vec.X + mat.Y.Y * vec.Y + mat.Z.Y * vec.Z),
                            (mat.X.Z * vec.X + mat.Y.Z * vec.Y + mat.Z.Z * vec.Z));
        }

    }

}
