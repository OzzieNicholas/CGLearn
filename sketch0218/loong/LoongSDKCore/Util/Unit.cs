using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.NetworkInformation;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace LoongCAD.Util
{
    public class Unit
    {
        private static readonly double G = 9.80665;

        private static readonly Dictionary<string, double> m_cvtMap = new Dictionary<string, double>() {
        //length
        {"mm", 1.0 },
        {"cm", 10.0 },
        {"m", 1000.0 },
        {"in", 25.4 },
        {"ft", 304.8 },
        {"ft-in", 25.4 }, //just for display, counted by inch 
        //area
        {"mm2", 1.0 },
        {"cm2", 100.0 },
        {"m2", 1e6 },
        {"in2", 25.4 * 25.4 },
        {"ft2", 304.8 * 304.8 },
        //volume
        {"mm3", 1.0 },
        {"cm3", 1000.0 },
        {"m3", 1e9 },
        {"in3", 25.4 * 25.4 * 25.4 },
        {"ft3", 304.8 * 304.8 * 304.8 },
        //quadrent
        {"mm4", 1.0 },
        {"cm4", 10000.0 },
        {"m4", 1e12 },
        {"in4", 25.4 * 25.4 * 25.4 * 25.4 },
        {"ft4", 304.8 * 304.8 * 304.8 * 304.8 },
        //warping const
        {"mm6", 1.0 },
        {"cm6", 1000000.0 },
        {"m6", 1e18 },
        {"in6", 25.4 * 25.4 * 25.4 * 25.4 * 25.4 * 25.4 },
        {"ft6", 304.8 * 304.8 * 304.8 * 304.8 * 304.8 * 304.8 },
        //cover area
        {"mm2/m", 1.0 },
        {"cm2/m", 100.0 },
        {"m2/m", 1e6 },
        {"in2/in", 25.4 * 25.4 * 39.3700787 },
        {"in2/ft", 25.4 * 25.4 * 3.2808399 },
        {"ft2/ft", 304.8 * 304.8 * 3.2808399 },
        //weight
        {"g", 1.0 },
        {"kg", 1000.0 },
        {"t", 1000000.0 },
        {"lbs", 453.59237 },
        {"kip", 453592.37 },
        {"n",  1e3 / G },
        {"dan",  1e4 / G },
        {"kn",  1e6 / G },
        {"lbf",  4448.2216152605 / G },
        //angle
        {"deg", System.Math.PI },
        {"rad", 180.0 },
        //ratio
        {"ratio_decimal", 1.0 },
        {"%", 0.01 },
        {"‰", 0.001 },
        //load
        { "kg/m", 1.0 },
        { "kg/cm", 100.0 },
        { "kg/mm", 1000.0 },
        { "t/m", 1e3 },
        { "t/cm", 1e5 },
        { "t/mm", 1e6 },
        { "n/m", 1.0 / G },
        { "n/cm", 100.0 / G },
        { "n/mm", 1000.0 / G },
        { "kn/m", 1000.0 / G },
        { "kn/cm", 1e5 / G },
        { "kn/mm", 1e6 / G },
        { "dan/m",  10.0 / G },
        { "dan/cm", 1000.0 / G },
        { "dan/mm", 10000.0 / G },
        { "lbf/in", 4.4482216152605 * 39.3700787 / G },
        { "lbf/ft", 4.4482216152605 * 3.2808399 / G },
        { "kip/in", 453.59237 * 39.3700787 },
        { "kip/ft", 453.59237 * 3.2808399 },
        //surface load
        { "kg/m2", 1.0 },
        { "kg/cm2", 10000.0 },
        { "kg/mm2", 1000000.0 },
        { "t/m2", 1e3 },
        { "t/cm2", 1e7 },
        { "t/mm2", 1e9 },
        { "n/m2", 1.0 / G },
        { "n/cm2", 10000.0 / G },
        { "n/mm2", 1000000.0 / G },
        { "kn/m2", 1000.0 / G },
        { "kn/cm2", 1e7 / G },
        { "kn/mm2", 1e9 / G },
        { "dan/m2",  10.0 / G },
        { "dan/cm2", 100000.0 / G },
        { "dan/mm2", 10000000.0 / G },
        { "lbf/in2", 4.4482216152605 * 39.3700787 * 39.3700787 / G },
        { "lbf/ft2", 4.4482216152605 * 3.2808399 * 3.2808399 / G },
        { "kip/in2", 453.59237 * 39.3700787 * 39.3700787 },
        { "kip/ft2", 453.59237 * 3.2808399 * 3.2808399 },
        //density
        { "kg/m3", 1.0 },
        { "kg/cm3", 1e6 },
        { "kg/mm3", 1e9 },
        { "t/m3", 1e3 },
        { "t/cm3", 1e9 },
        { "t/mm3", 1e12 },
        { "n/m3", 1.0 / G },
        { "n/cm3", 1e6 / G },
        { "n/mm3", 1e9 / G },
        { "kn/m3", 1e3 / G },
        { "kn/cm3", 1e9 / G },
        { "kn/mm3", 1e12 / G },
        { "dan/m3",  10.0 / G },
        { "dan/cm3", 1e7 / G },
        { "dan/mm3", 1e10 / G },
        { "lbf/in3", 4.4482216152605 * 39.3700787 * 39.3700787 * 39.3700787 / G },
        { "lbf/ft3", 4.4482216152605 * 3.2808399 * 3.2808399 * 3.2808399 / G },
        { "kip/in3", 453.59237 * 39.3700787 * 39.3700787 * 39.3700787 },
        { "kip/ft3", 453.59237 * 3.2808399 * 3.2808399 * 3.2808399 },
        //moment
        { "kgm", 1.0 },
        { "tm", 1e3 },
        { "nm", 1.0 / G },
        { "danm", 10.0 / G },
        { "knm", 1000.0 / G },
        { "lbfin", 4.4482216152605 * 0.0254 / G },
        { "lbfft", 4.4482216152605 * 0.3048 / G  },
        { "kipin", 453.59237 * 0.0254 },
        { "kipft", 453.59237 * 0.3048 },
        //distributed moment
        { "kgm/m", 1.0 },
        { "tm/m", 1e3 },
        { "nm/m", 1.0 / G },
        { "danm/m", 10.0 / G },
        { "knm/m", 1000.0 / G },
        { "lbfft/ft", 4.4482216152605 / G  },
        { "kipft/ft", 453.59237 },
        //rot sprint constant
        { "kgm/rad", 1.0 },
        { "kgm/angl", 180.0 / System.Math.PI },
        { "tm/rad", 1000.0 },
        { "tm/angl", 1000.0 * 180.0 / System.Math.PI },
        { "nm/rad", 1.0 / G },
        { "nm/angl", 1.0 / G * 180.0 / System.Math.PI  },
        { "danm/rad", 10.0 / G },
        { "danm/angl", 10.0 / G * 180.0 / System.Math.PI },
        { "knm/rad", 1000.0 / G },
        { "knm/angl", 1000.0 / G * 180.0 / System.Math.PI  },
        { "lbfin/rad", 4.4482216152605 * 0.0254 / G },
        { "lbfin/angl", 4.4482216152605 * 0.0254 / G * 180.0 / System.Math.PI },
        { "lbfft/rad", 4.4482216152605 * 0.3048 / G },
        { "lbfft/angl", 4.4482216152605 * 0.3048 / G * 180.0 / System.Math.PI },
        { "kipin/rad", 453.59237 * 0.0254 },
        { "kipin/angl", 453.59237 * 0.0254 * 180.0 / System.Math.PI },
        { "kipft/rad", 453.59237 * 0.3048 },
        { "kipft/angl", 453.59237 * 0.3048 * 180.0 / System.Math.PI },
    };

        public static double Cvt(double v, string src, string dst) {
            //scalable units conversion
            double wsrc, wdst;
            if (m_cvtMap.TryGetValue(src, out wsrc)
                && m_cvtMap.TryGetValue(dst, out wdst))
                return v * wsrc / wdst;

            //temperature
            if (src == "k" || src == "f") {
                if (src == "k" && dst == "f")
                    return 32.0 + v * 1.8;
                else if (src == "f" && dst == "k")
                    return (v - 32.0) / 1.8;

                return v;
            }

            //thermal dilat. coeff
            if (src == "1/k" || src == "1/f") {
                if (src == "1/k" && dst == "1/f")
                    return 1.0 / (32.0 + 1.8 / v);
                else if (src == "1/f" && dst == "1/k")
                    return 1.8 / (1.0 / v - 32.0);

                return v;
            }

            return v;
        }

        public static string Str(double v, string unitSrc, string unitDst, int enob) {
            if (double.IsNaN(v))
                return "";

            if (unitSrc != "" && unitDst != "")
                v = Unit.Cvt(v, unitSrc, unitDst);

            if (unitDst == "ft-in") {
                double inch = System.Math.Truncate(v);
                double dec = System.Math.Abs(v - inch);
                int feet = (int)System.Math.Truncate(inch / 12.0);
                int lastInch = System.Math.Abs((int)System.Math.Truncate(inch - feet * 12.0));

                string s = "";
                if (feet != 0)
                    s += feet + "\'";

                if (lastInch != 0) {
                    if (s.Length > 0)
                        s += '-';

                    s += lastInch + "\"";
                }

                if (dec > 0) {
                    var ab = Utils.Decimal2AdivB(dec, System.Math.Max(1, System.Math.Min(3, enob)));
                    if (ab.Item1 != 0 && ab.Item2 != 0)
                        s += ab.Item1 + "/" + ab.Item2;
                }

                return String.IsNullOrEmpty(s) ? "0" : s;
            }
            else {
                return Utils.Str(v, enob);
            }
        }


    }
}
