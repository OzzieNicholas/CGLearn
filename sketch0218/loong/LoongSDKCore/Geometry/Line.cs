using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
    [Serializable]
    public struct Line
    {
        public Vec3 V0;
        public Vec3 V1;

        public Line(Line src) {
            V0 = src.V0;
            V1 = src.V1;
        }

        public Line(Vec3 _v0, Vec3 _v1) {
            V0 = _v0;
            V1 = _v1;
        }

        public override bool Equals(object obj) {
            if (!(obj is Line))
                return false;

            Line other = (Line)obj;
            return V0.Equals(other.V0) && V1.Equals(other.V1);
        }

        public override int GetHashCode() {
            return Tuple.Create(V0.X, V0.Y, V0.Z, V1.X, V1.Y, V1.Z).GetHashCode();
        }

        public override string ToString() {
            return V0.ToString() + ">>" + V1.ToString();
        }

        public Vec3 Dir => V1 - V0;
        public Vec3 NormalizedDir => (V1 - V0).Normalize();
        public double Length => V0.Dist(V1);
        public double LengthSqr => V0.DistSqr(V1);

        public Vec3 Proj(Vec3 v) {
            Vec3 dir = V1 - V0;
			return V0 + dir* (v - V0).Dot(dir) / dir.LengthSqr;
		}

        public Vec3 Sample(double t) {
			return V0 + (V1 - V0) * t;
		}

        public double T(Vec3 pt) {
            Vec3 dir = V1 - V0;
	    	return dir.Dot(pt - V0) / dir.LengthSqr;
		}

        public bool HasIntersect(Line otherLine)
        {
            Vec3 dir = this.V1 - this.V0;
            double d2 = dir.LengthSqr;
            double t0 = dir.Dot(otherLine.V0 - this.V0) / d2;
            double t1 = dir.Dot(otherLine.V1 - this.V0) / d2;
            return Math.Min(t0, t1) <= 1.0 && Math.Max(t0, t1) >= 0.0;
        }

        public Vec3 this[int idx] {
            get => (idx == 0) ? V0 : (idx == 1) ? V1 : Vec3.ZERO;
        }

        public static bool operator ==(Line li0, Line li1) {
            return li0.V0 == li1.V0 && li0.V1 == li1.V1;
        }

        public static bool operator !=(Line li0, Line li1) {
            return li0.V0 != li1.V0 || li0.V1 != li1.V1;
        }
    }
    
}
