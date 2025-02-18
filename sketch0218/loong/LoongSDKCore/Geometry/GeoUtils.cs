using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Globalization;
using System.Net.NetworkInformation;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using LoongCAD.Geometry;

namespace LoongCAD.Util
{
    public class GeoUtils
    {
        public static Vec3 EvalNormalByPoints(Vec3[] points, double tol = 1e-3) {
			int convexIdx = 0;
			for (int i = 1; i < points.Length; ++i) {
				var cpt = points[convexIdx];
				var pt = points[i];
				var ptnext = points[(i + 1) % points.Length];
				var ptpre = (i > 0) ? points[i - 1] : points.Last();
				if ((ptnext - pt).Cross(ptpre - pt).Length < 1e-4) //point is on line, or has duplicated neighbour
					continue;

				if (Math.Abs(pt.X - cpt.X) < tol) {
					if (Math.Abs(pt.Y - cpt.Y) < tol) {
						if (pt.Z > cpt.Z)
							convexIdx = i;
					}
					else if (pt.Y > cpt.Y)
						convexIdx = i;
				}
				else if (pt.X > cpt.X)
					convexIdx = i;
			}

			var v = points[convexIdx];
			var vnext = points[(convexIdx + 1) % points.Length];
			var vpre = (convexIdx > 0) ? points[convexIdx - 1] : points.Last();
			Vec3 planeN = (vnext - v).Normalize().Cross((vpre - v).Normalize());
			if (planeN.Length < 1e-3)
				planeN = Vec3.AXIS_Z;

			return planeN.Normalize();
		}

		public static bool LineIntersected(Line line0, Line line1, out double t0, out double t1)
        {
			t0 = 0.0;
			t1 = 0.0;
            if (line0 == null || line1 == null)
                return false;
			return LineIntersected(line0.V0, line0.V1, line1.V0, line1.V1, out t0, out t1);
        }

		public static bool LineIntersected(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4, out double t0, out double t1)
        {
			t0 = double.MaxValue;
			t1 = double.MaxValue;
			if (v1 == Vec3.MAX || v2 == Vec3.MAX || v3 == Vec3.MAX || v4 == Vec3.MAX)
				return false;

			Vec3 dir0 = v2 - v1;
			Vec3 dir1 = v4 - v3;
            Vec3 n = dir0.Cross(dir1);
            double n2 = n.LengthSqr;
            if (n2 == 0.0) { // 两线平行
                return false;
            }

			Vec3 o10 = v3 - v1;
			t0 = o10.Cross(dir1).Dot(n) / n2;
			t1 = o10.Cross(dir0).Dot(n) / n2;
            return false;
        }

    }
}
