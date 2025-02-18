using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    /**
    4x3 double matrix
    */
    [Serializable]
    public struct Mat43
    {
        public Vec3 X;
        public Vec3 Y;
        public Vec3 Z;
        public Vec3 Pos;

        public static readonly Mat43 IDENTITY = new Mat43(1.0);

        public static readonly Mat43 NULL = new Mat43(0.0);

        public Mat43(double val) {
            X = new Vec3(val, 0.0, 0.0);
            Y = new Vec3(0.0, val, 0.0);
            Z = new Vec3(0.0, 0.0, val);
            Pos = new Vec3();
        }

        public Mat43(Vec3 _x, Vec3 _y, Vec3 _z, Vec3 _pos) {
            X = _x;
            Y = _y;
            Z = _z;
            Pos = _pos;
        }

        public Mat43(double m00, double m10, double m20,
                    double m01, double m11, double m21,
                    double m02, double m12, double m22,
                    double m03, double m13, double m23) {
            X = new Vec3(m00, m10, m20);
            Y = new Vec3(m01, m11, m21);
            Z = new Vec3(m02, m12, m22);
            Pos = new Vec3(m03, m13, m23);
        }

        public Mat43(double[] m) {
            X = new Vec3(m[0], m[1], m[2]);
            Y = new Vec3(m[3], m[4], m[5]);
            Z = new Vec3(m[6], m[7], m[8]);
            Pos = new Vec3(m[9], m[10], m[11]);
        }

        public Mat43(Mat43 src) {
            X = src.X;
            Y = src.Y;
            Z = src.Z;
            Pos = src.Pos;
        }

        public override bool Equals(object obj) {
            if (!(obj is Mat43))
                return false;

            Mat43 vec = (Mat43)obj;
            return this.X == vec.X && this.Y == vec.Y && this.Z == vec.Z && this.Pos == vec.Pos;
        }

        public override int GetHashCode() {
            return Tuple.Create(X, Y, Z, Pos).GetHashCode();
        }

        public override string ToString() {
            return X.ToString() + '\n' + Y.ToString() + '\n' + Z.ToString() + '\n' + Pos.ToString();
        }

        public Mat4 ToMat4() {
            return new Mat4(X.X, X.Y, X.Z, 0.0,
                            Y.X, Y.Y, Y.Z, 0.0,
                            Z.X, Z.Y, Z.Z, 0.0,
                            Pos.X, Pos.Y, Pos.Z, 1.0);
        }

        public void Set(Vec3 _x, Vec3 _y, Vec3 _z, Vec3 _pos) {
            X = _x;
            Y = _y;
            Z = _z;
            Pos = _pos;
        }

        public void Set(double[] m) {
            X.Set(m[0], m[1], m[2]);
            Y.Set(m[3], m[4], m[5]);
            Z.Set(m[6], m[7], m[8]);
            Pos.Set(m[9], m[10], m[11]);
        }

        public void Set(double m00, double m10, double m20,
                        double m01, double m11, double m21,
                        double m02, double m12, double m22,
                        double m03, double m13, double m23) {
            X.Set(m00, m10, m20);
            Y.Set(m01, m11, m21);
            Z.Set(m02, m12, m22);
            Pos.Set(m03, m13, m23);
        }

        public void Identity() {
            X.Set(1.0, 0.0, 0.0);
            Y.Set(0.0, 1.0, 0.0);
            Z.Set(0.0, 0.0, 1.0);
            Pos.Set(0.0, 0.0, 0.0);
        }

        public Mat43 Inverse() {
            double a11 = this.X.X, a12 = this.Y.X, a13 = this.Z.X, a14 = this.Pos.X;
            double a21 = this.X.Y, a22 = this.Y.Y, a23 = this.Z.Y, a24 = this.Pos.Y;
            double a31 = this.X.Z, a32 = this.Y.Z, a33 = this.Z.Z, a34 = this.Pos.Z;
            double a41 = 0.0, a42 = 0.0, a43 = 0.0, a44 = 1.0;

            Mat43 mInv = new Mat43(1.0);
            mInv.X.X = a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a22 * a34 * a43 - a23 * a32 * a44 - a24 * a33 * a42;
            mInv.X.Y = a21 * a34 * a43 + a23 * a31 * a44 + a24 * a33 * a41 - a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43;
            mInv.X.Z = a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a21 * a34 * a42 - a22 * a31 * a44 - a24 * a32 * a41;
            mInv.Y.X = a12 * a34 * a43 + a13 * a32 * a44 + a14 * a33 * a42 - a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43;
            mInv.Y.Y = a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a11 * a34 * a43 - a13 * a31 * a44 - a14 * a33 * a41;
            mInv.Y.Z = a11 * a34 * a42 + a12 * a31 * a44 + a14 * a32 * a41 - a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42;
            mInv.Z.X = a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a12 * a24 * a43 - a13 * a22 * a44 - a14 * a23 * a42;
            mInv.Z.Y = a11 * a24 * a43 + a13 * a21 * a44 + a14 * a23 * a41 - a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43;
            mInv.Z.Z = a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a11 * a24 * a42 - a12 * a21 * a44 - a14 * a22 * a41;
            mInv.Pos.X = a12 * a24 * a33 + a13 * a22 * a34 + a14 * a23 * a32 - a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33;
            mInv.Pos.Y = a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a11 * a24 * a33 - a13 * a21 * a34 - a14 * a23 * a31;
            mInv.Pos.Z = a11 * a24 * a32 + a12 * a21 * a34 + a14 * a22 * a31 - a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32;

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

        public bool IsRHS() {
			return X.Cross(Y).Dot(Z) >= 0.0;
		}

        public Mat43 Normalize() {
            return new Mat43(X.Normalize(), Y.Normalize(), Z.Normalize(), Pos);
        }

        public void NormalizeThis() {
            this.X = this.X.Normalize();
            this.Y = this.Y.Normalize();
            this.Z = this.Z.Normalize();
        }

        public void Translate(double v) {
            Pos.X += v;
            Pos.Y += v;
            Pos.Z += v;
        }

        public void Translate(double _x, double _y, double _z) {
            Pos.X += _x;
            Pos.Y += _y;
            Pos.Z += _z;
        }

        public void Translate(Vec3 vec)
	    {
            Pos.X += vec.X;
            Pos.Y += vec.Y;
            Pos.Z += vec.Z;
	    }

        public void RotateX(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);

            //copy current mat to temp
            double m00 = X.X;
            double m10 = X.Y;
            double m20 = X.Z;
            double m01 = Y.X;
            double m11 = Y.Y;
            double m21 = Y.Z;
            double m02 = Z.X;
            double m12 = Z.Y;
            double m22 = Z.Z;
            double m03 = Pos.X;
            double m13 = Pos.Y;
            double m23 = Pos.Z;

            X.Y = fCosine * m10 - fSine * m20;
            X.Z = fSine * m10 + fCosine * m20;

            Y.Y = fCosine * m11 - fSine * m21;
            Y.Z = fSine * m11 + fCosine * m21;

            Z.Y = fCosine * m12 - fSine * m22;
            Z.Z = fSine * m12 + fCosine * m22;

            Pos.Y = fCosine * m13 - fSine * m23;
            Pos.Z = fSine * m13 + fCosine * m23;
        }

        public void RotateY(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);

            //copy current mat to temp
            double m00 = X.X;
            double m10 = X.Y;
            double m20 = X.Z;
            double m01 = Y.X;
            double m11 = Y.Y;
            double m21 = Y.Z;
            double m02 = Z.X;
            double m12 = Z.Y;
            double m22 = Z.Z;
            double m03 = Pos.X;
            double m13 = Pos.Y;
            double m23 = Pos.Z;

            X.X = fCosine * m00 + fSine * m20;
            X.Z = -fSine * m00 + fCosine * m20;

            Y.X = fCosine * m01 + fSine * m21;
            Y.Z = -fSine * m01 + fCosine * m21;

            Z.X = fCosine * m02 + fSine * m22;
            Z.Z = -fSine * m02 + fCosine * m22;

            Pos.X = fCosine * m03 + fSine * m23;
            Pos.Z = -fSine * m03 + fCosine * m23;
        }

        public void RotateZ(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);

            //copy current mat to temp
            double m00 = X.X;
            double m10 = X.Y;
            double m20 = X.Z;
            double m01 = Y.X;
            double m11 = Y.Y;
            double m21 = Y.Z;
            double m02 = Z.X;
            double m12 = Z.Y;
            double m22 = Z.Z;
            double m03 = Pos.X;
            double m13 = Pos.Y;
            double m23 = Pos.Z;

            X.X = fCosine * m00 + -fSine * m10;
            X.Y = fSine * m00 + fCosine * m10;

            Y.X = fCosine * m01 + -fSine * m11;
            Y.Y = fSine * m01 + fCosine * m11;

            Z.X = fCosine * m02 + -fSine * m12;
            Z.Y = fSine * m02 + fCosine * m12;

            Pos.X = fCosine * m03 + -fSine * m13;
            Pos.Y = fSine * m03 + fCosine * m13;
        }

        public void RotateAround(Vec3 target, Vec3 vecUp, double rad) {
            Vec3 vecn = vecUp.Normalize();
            double _cos = System.Math.Cos(rad);
            double _sin = System.Math.Sin(rad);
            double _oneMinusCos = 1.0 - System.Math.Cos(rad);

            Mat43 mat = new Mat43(vecn.X * vecn.X + (vecn.Y * vecn.Y + vecn.Z * vecn.Z) * _cos,
                                vecn.X * vecn.Y * _oneMinusCos + vecn.Z * _sin,
                                vecn.X * vecn.Z * _oneMinusCos - vecn.Y * _sin,

                                vecn.X * vecn.Y * _oneMinusCos - vecn.Z * _sin,
                                vecn.Y * vecn.Y + (vecn.X * vecn.X + vecn.Z * vecn.Z) * _cos,
                                vecn.Y * vecn.Z * _oneMinusCos + vecn.X * _sin,

                                vecn.X * vecn.Z * _oneMinusCos + vecn.Y * _sin,
                                vecn.Y * vecn.Z * _oneMinusCos - vecn.X * _sin,
                                vecn.Z * vecn.Z + (vecn.X * vecn.X + vecn.Y * vecn.Y) * _cos,

                                (target.X * (vecn.Y * vecn.Y + vecn.Z * vecn.Z) - vecn.X * (target.Y * vecn.Y + target.Z * vecn.Z)) * _oneMinusCos + (target.Y * vecn.Z - target.Z * vecn.Y) * _sin,
                                (target.Y * (vecn.X * vecn.X + vecn.Z * vecn.Z) - vecn.Y * (target.X * vecn.X + target.Z * vecn.Z)) * _oneMinusCos + (target.Z * vecn.X - target.X * vecn.Z) * _sin,
		                        (target.Z * (vecn.X * vecn.X + vecn.Y * vecn.Y) - vecn.Z * (target.X * vecn.X + target.Y * vecn.Y)) * _oneMinusCos + (target.X * vecn.Y - target.Y * vecn.X) * _sin);
		    this = mat * this;
        }

        public void Scale(double s) {
            X *= s;
            Y *= s;
            Z *= s;
            Pos *= s;
        }

        public void Scale(double sx, double sy, double sz) {
            X.X *= sx;
            X.Y *= sy;
            X.Z *= sz;

            Y.X *= sx;
            Y.Y *= sy;
            Y.Z *= sz;

            Z.X *= sx;
            Z.Y *= sy;
            Z.Z *= sz;

            Pos.X *= sx;
            Pos.Y *= sy;
            Pos.Z *= sz;
        }

        public void Scale(Vec3 vec)
	    {
		    double sx = vec.X, sy = vec.Y, sz = vec.Z;

            X.X *= sx;
            X.Y *= sy;
            X.Z *= sz;

            Y.X *= sx;
            Y.Y *= sy;
            Y.Z *= sz;

            Z.X *= sx;
            Z.Y *= sy;
            Z.Z *= sz;

            Pos.X *= sx;
            Pos.Y *= sy;
            Pos.Z *= sz;
        }

        public static Mat43 T(double x, double y, double z) {
            return new Mat43(Vec3.AXIS_X, Vec3.AXIS_Y, Vec3.AXIS_Z, new Vec3(x, y, z));
        }

        public static Mat43 T(Vec3 trans)	{
            return new Mat43(Vec3.AXIS_X, Vec3.AXIS_Y, Vec3.AXIS_Z, trans);
        }

        public static Mat43 RX(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat43(
                1.0, 0.0, 0.0,
                0.0, fCosine, fSine,
                0.0, -fSine, fCosine,
                0.0, 0.0, 0.0);
        }

        public static Mat43 RY(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat43(
                fCosine, 0.0, -fSine,
                0.0, 1.0, 0.0,
                fSine, 0.0, fCosine,
                0.0, 0.0, 0.0
            );
        }

        public static Mat43 RZ(double rad) {
            double fCosine = System.Math.Cos(rad), fSine = System.Math.Sin(rad);
            return new Mat43(
                fCosine, fSine, 0.0,
                -fSine, fCosine, 0.0,
                0.0, 0.0, 1.0,
                    0.0, 0.0, 0.0
            );
        }

        public static Mat43 R(Vec3 target, Vec3 vecUp, double rad) {

            Vec3 vecn = vecUp.Normalize();
            Mat43 mat = new Mat43(1.0);

            double _cos = System.Math.Cos(rad);
            double _sin = System.Math.Sin(rad);
            double _oneMinusCos = 1.0 - System.Math.Cos(rad);

            return new Mat43(
                vecn.X * vecn.X + (vecn.Y * vecn.Y + vecn.Z * vecn.Z) * _cos,
                vecn.X * vecn.Y * _oneMinusCos + vecn.Z * _sin,
                vecn.X * vecn.Z * _oneMinusCos - vecn.Y * _sin,

                vecn.X * vecn.Y * _oneMinusCos - vecn.Z * _sin,
                vecn.Y * vecn.Y + (vecn.X * vecn.X + vecn.Z * vecn.Z) * _cos,
                vecn.Y * vecn.Z * _oneMinusCos + vecn.X * _sin,

                vecn.X * vecn.Z * _oneMinusCos + vecn.Y * _sin,
                vecn.Y * vecn.Z * _oneMinusCos - vecn.X * _sin,
                vecn.Z * vecn.Z + (vecn.X * vecn.X + vecn.Y * vecn.Y) * _cos,

                (target.X * (vecn.Y * vecn.Y + vecn.Z * vecn.Z) - vecn.X * (target.Y * vecn.Y + target.Z * vecn.Z)) * _oneMinusCos + (target.Y * vecn.Z - target.Z * vecn.Y) * _sin,
                (target.Y * (vecn.X * vecn.X + vecn.Z * vecn.Z) - vecn.Y * (target.X * vecn.X + target.Z * vecn.Z)) * _oneMinusCos + (target.Z * vecn.X - target.X * vecn.Z) * _sin,
                (target.Z * (vecn.X * vecn.X + vecn.Y * vecn.Y) - vecn.Z * (target.X * vecn.X + target.Y * vecn.Y)) * _oneMinusCos + (target.X * vecn.Y - target.Y * vecn.X) * _sin
            );
	    }

        public static Mat43 S(double s) {
            return new Mat43(s, 0.0, 0.0,
					        0.0, s, 0.0,
					        0.0, 0.0, s,
					        0.0, 0.0, 0.0);
        }

        public static Mat43 S(double x, double y, double z) {
            return new Mat43(x, 0.0, 0.0,
					        0.0, y, 0.0,
					        0.0, 0.0, z,
					        0.0, 0.0, 0.0);
        }

        public static Mat43 TS(Vec3 trans, Vec3 scale) {
			return new Mat43(scale.X, 0.0, 0.0,
					        0.0, scale.Y, 0.0,
					        0.0, 0.0, scale.Z,
					        trans.X, trans.Y, trans.Z);
        }

        public static Mat43 CreateByXZ(Vec3 vX, Vec3 vZ) {
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

            return new Mat43(x.Normalize(), y.Normalize(), z.Normalize(), Vec3.ZERO);
        }

        public static Mat43 CreateByYZ(Vec3 vY, Vec3 vZ) {
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

            return new Mat43(x.Normalize(), y.Normalize(), z.Normalize(), Vec3.ZERO);
        }

        public static Mat43 CreateByEuler(double radX, double radY, double radZ) {
            double su = System.Math.Sin(radX), cu = System.Math.Cos(radX);
            double sv = System.Math.Sin(radZ), cv = System.Math.Cos(radZ);
            double sw = System.Math.Sin(radY), cw = System.Math.Cos(radY);
            return new Mat43(cv * cw,
                            cv * sw,
                            -sv,

                            su * sv * cw - cu * sw,
                            cu * cw + su * sv * sw,
                            su * cv,

                            su * sw + cu * sv * cw,
                            cu * sv * sw - su * cw,
                            cu * cv,

                            0.0, 0.0, 0.0);
        }

        public Vec3 this[int col] {
            get => (col == 0) ? X : (col == 1) ? Y : (col == 2) ? Z : (col == 3) ? Pos : Vec3.ZERO;
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

        public static bool operator ==(Mat43 mat0, Mat43 mat1) {
            return mat0.X == mat1.X
                    && mat0.Y == mat1.Y
                    && mat0.Z == mat1.Z
                    && mat0.Pos == mat1.Pos;
        }

        public static bool operator !=(Mat43 mat0, Mat43 mat1) {
            return mat0.X != mat1.X
                    || mat0.Y != mat1.Y
                    || mat0.Z != mat1.Z
                    || mat0.Pos != mat1.Pos;
        }

        public static Mat43 operator *(Mat43 mat1, Mat43 mat2) {
            return new Mat43(mat1.X.X * mat2.X.X + mat1.Y.X * mat2.X.Y + mat1.Z.X * mat2.X.Z,
                            mat1.X.Y * mat2.X.X + mat1.Y.Y * mat2.X.Y + mat1.Z.Y * mat2.X.Z,
                            mat1.X.Z * mat2.X.X + mat1.Y.Z * mat2.X.Y + mat1.Z.Z * mat2.X.Z,
                                                                               
                            mat1.X.X * mat2.Y.X + mat1.Y.X * mat2.Y.Y + mat1.Z.X * mat2.Y.Z,
                            mat1.X.Y * mat2.Y.X + mat1.Y.Y * mat2.Y.Y + mat1.Z.Y * mat2.Y.Z,
                            mat1.X.Z * mat2.Y.X + mat1.Y.Z * mat2.Y.Y + mat1.Z.Z * mat2.Y.Z,
                                                                               
                            mat1.X.X * mat2.Z.X + mat1.Y.X * mat2.Z.Y + mat1.Z.X * mat2.Z.Z,
                            mat1.X.Y * mat2.Z.X + mat1.Y.Y * mat2.Z.Y + mat1.Z.Y * mat2.Z.Z,
                            mat1.X.Z * mat2.Z.X + mat1.Y.Z * mat2.Z.Y + mat1.Z.Z * mat2.Z.Z,

                            mat1.X.X * mat2.Pos.X + mat1.Y.X * mat2.Pos.Y + mat1.Z.X * mat2.Pos.Z + mat1.Pos.X,
                            mat1.X.Y * mat2.Pos.X + mat1.Y.Y * mat2.Pos.Y + mat1.Z.Y * mat2.Pos.Z + mat1.Pos.Y,
                            mat1.X.Z * mat2.Pos.X + mat1.Y.Z * mat2.Pos.Y + mat1.Z.Z * mat2.Pos.Z + mat1.Pos.Z);
        }

        public static Vec3 operator *(Mat43 mat, Vec3 vec) {
            return new Vec3((mat.X.X * vec.X + mat.Y.X * vec.Y + mat.Z.X * vec.Z + mat.Pos.X),
                            (mat.X.Y * vec.X + mat.Y.Y * vec.Y + mat.Z.Y * vec.Z + mat.Pos.Y),
                            (mat.X.Z * vec.X + mat.Y.Z * vec.Y + mat.Z.Z * vec.Z + mat.Pos.Z));
        }

        public static Vec4 operator *(Mat43 mat, Vec4 vec) {
            return new Vec4((mat.X.X * vec.X + mat.Y.X * vec.Y + mat.Z.X * vec.Z + mat.Pos.X),
                            (mat.X.Y * vec.X + mat.Y.Y * vec.Y + mat.Z.Y * vec.Z + mat.Pos.Y),
                            (mat.X.Z * vec.X + mat.Y.Z * vec.Y + mat.Z.Z * vec.Z + mat.Pos.Z),
                            vec.W);
        }

    }
 
}
