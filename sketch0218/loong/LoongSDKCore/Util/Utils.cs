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
    public class Utils
    {
        public static readonly DateTime DATE_EPOCH = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);

        public static double Angl2Rad(double angl) {
            return angl * Math.PI / 180.0;
        }

        public static double Rad2Angl(double rad) {
            return rad * 180.0 / Math.PI;
        }

        public static bool IsEqualOrContained(Control par, Control child) {
            if (par == null || child == null)
                return false;

            if (par == child)
                return true;

            var cur = child;
            while (cur != null) {
                if (cur == par)
                    return true;

                cur = cur.Parent;
            }

            return false;
        }

        //get local MAC address
        public static string GetMacAddress() {
            const int MIN_MAC_ADDR_LENGTH = 12;
            string macAddress = string.Empty;
            long maxSpeed = -1;

            foreach (NetworkInterface nic in NetworkInterface.GetAllNetworkInterfaces()) {
                string tempMac = nic.GetPhysicalAddress().ToString();
                if (nic.Speed > maxSpeed &&
                    !string.IsNullOrEmpty(tempMac) &&
                    tempMac.Length >= MIN_MAC_ADDR_LENGTH) {
                    maxSpeed = nic.Speed;
                    macAddress = tempMac;
                }
            }

            return macAddress;
        }

        //get the exe file path of the current program
        public static string GetExePath() {
            return Application.ExecutablePath.Substring(0, Application.ExecutablePath.LastIndexOf('\\'));
        }

        public static bool RegExprMatch(string text, string expr) {
            Match mc = Regex.Match(text, expr);
            return mc.Success;
        }

        public static string ToRegExpr(string str) {
            bool slash = false;
            Stack<char> level = new Stack<char>();

            string expr = "^";
            foreach (char c in str) {
                if (slash) {
                    expr += c;
                    slash = false;
                    continue;
                }

                switch (c) {
                    case '\\':
                    expr += c;
                    slash = true;
                    break;
                    case '[':
                    level.Push(c);
                    expr += c;
                    break;
                    case ']':
                    level.Pop();
                    expr += c;
                    break;
                    case '?':
                    if (level.Count > 0)
                        expr += "\\?";
                    else
                        expr += ".{1}";
                    break;
                    case '*':
                    if (level.Count > 0)
                        expr += "\\*";
                    else
                        expr += ".*";
                    break;
                    default:
                    expr += c;
                    break;
                }
            }

            expr += '$';

            return expr;
        }

        public static bool ToBool(object v, bool def = false) {
            if (v == null)
                return def;

            if (v is bool)
                return (bool)v;
            else if (v is int)
                return (int)v != 0;
            else if (v is long)
                return (long)v != 0;
            else if (v is short)
                return (short)v != 0;
            else if (v is double)
                return (double)v != 0;
            else if (v is uint)
                return (uint)v != 0;
            else if (v is ulong)
                return (ulong)v != 0;
            else if (v is ushort)
                return (ushort)v != 0;
            else if (v is byte)
                return (byte)v != 0;
            else if (v is sbyte)
                return (sbyte)v != 0;
            else if (v is char)
                return (char)v != 0;

            return def;
        }

        public static sbyte ToSByte(object v, sbyte def = 0) {
            if (v == null)
                return def;

            if (v is sbyte)
                return (sbyte)v;
            else if (v is byte)
                return (sbyte)(byte)v;
            else if (v is int)
                return (sbyte)(int)v;
            else if (v is long)
                return (sbyte)(long)v;
            else if (v is short)
                return (sbyte)(short)v;
            else if (v is double)
                return (sbyte)(double)v;
            else if (v is uint)
                return (sbyte)(uint)v;
            else if (v is ulong)
                return (sbyte)(ulong)v;
            else if (v is ushort)
                return (sbyte)(ushort)v;
            else if (v is char)
                return (sbyte)(char)v;
            else if (v.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = v.GetType().GetEnumUnderlyingType();
                if (dataType == typeof(sbyte))
                    return (sbyte)v;
                else if (dataType == typeof(short))
                    return (sbyte)(short)v;
                else if (dataType == typeof(int))
                    return (sbyte)(int)v;
                else if (dataType == typeof(uint))
                    return (sbyte)(uint)v;
            }

            return def;
        }

        public static int ToInt(object v, int def = 0) {
            if (v == null)
                return def;

            if (v is int)
                return (int)v;
            else if (v is long)
                return (int)(long)v;
            else if (v is short)
                return (int)(short)v;
            else if (v is double)
                return (int)(double)v;
            else if (v is uint)
                return (int)(uint)v;
            else if (v is ulong)
                return (int)(ulong)v;
            else if (v is ushort)
                return (int)(ushort)v;
            else if (v is byte)
                return (int)(byte)v;
            else if (v is sbyte)
                return (int)(sbyte)v;
            else if (v is char)
                return (int)(char)v;
            else if (v.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = v.GetType().GetEnumUnderlyingType();
                if (dataType == typeof(int))
                    return (int)v;
                else if (dataType == typeof(uint))
                    return (int)(uint)v;
                else if (dataType == typeof(sbyte))
                    return (int)(sbyte)v;
                else if (dataType == typeof(short))
                    return (int)(short)v;
            }

            return def;
        }

        public static uint ToUint(object v, uint def = 0) {
            if (v == null)
                return def;

            if (v is uint)
                return (uint)v;
            else if (v is int)
                return (uint)(int)v;
            else if (v is long)
                return (uint)(long)v;
            else if (v is short)
                return (uint)(short)v;
            else if (v is double)
                return (uint)(double)v;
            else if (v is ulong)
                return (uint)(ulong)v;
            else if (v is ushort)
                return (uint)(ushort)v;
            else if (v is byte)
                return (uint)(byte)v;
            else if (v is sbyte)
                return (uint)(sbyte)v;
            else if (v is char)
                return (uint)(char)v;
            else if (v.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = v.GetType().GetEnumUnderlyingType();
                if (dataType == typeof(uint))
                    return (uint)v;
                else if (dataType == typeof(int))
                    return (uint)(int)v;
                else if (dataType == typeof(sbyte))
                    return (uint)(sbyte)v;
                else if (dataType == typeof(short))
                    return (uint)(short)v;
            }

            return def;
        }

        public static short ToShort(object v, short def = 0) {
            if (v == null)
                return def;

            if (v is short)
                return (short)v;
            else if (v is ushort)
                return (short)(ushort)v;
            else if (v is int)
                return (short)(int)v;
            else if (v is long)
                return (short)(long)v;
            else if (v is double)
                return (short)(double)v;
            else if (v is uint)
                return (short)(uint)v;
            else if (v is ulong)
                return (short)(ulong)v;
            else if (v is byte)
                return (short)(byte)v;
            else if (v is sbyte)
                return (short)(sbyte)v;
            else if (v is char)
                return (short)(char)v;
            else if (v.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = v.GetType().GetEnumUnderlyingType();
                if (dataType == typeof(short))
                    return (short)v;
                if (dataType == typeof(int))
                    return (short)(int)v;
                else if (dataType == typeof(uint))
                    return (short)(uint)v;
                else if (dataType == typeof(sbyte))
                    return (short)(sbyte)v;
            }

            return def;
        }

        public static ushort ToUshort(object v, ushort def = 0) {
            if (v == null)
                return def;

            if (v is ushort)
                return (ushort)v;
            else if (v is short)
                return (ushort)(short)v;
            else if (v is int)
                return (ushort)(int)v;
            else if (v is long)
                return (ushort)(long)v;
            else if (v is double)
                return (ushort)(double)v;
            else if (v is uint)
                return (ushort)(uint)v;
            else if (v is ulong)
                return (ushort)(ulong)v;
            else if (v is byte)
                return (ushort)(byte)v;
            else if (v is sbyte)
                return (ushort)(sbyte)v;
            else if (v is char)
                return (ushort)(char)v;
            else if (v.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                var dataType = v.GetType().GetEnumUnderlyingType();
                if (dataType == typeof(short))
                    return (ushort)v;
                if (dataType == typeof(int))
                    return (ushort)(int)v;
                else if (dataType == typeof(uint))
                    return (ushort)(uint)v;
                else if (dataType == typeof(sbyte))
                    return (ushort)(sbyte)v;
            }

            return def;
        }

        public static long ToLong(object v, long def = 0) {
            if (v == null)
                return def;

            if (v is long)
                return (long)v;
            else if (v is ulong)
                return (long)(ulong)v;
            else if (v is int)
                return (long)(int)v;
            else if (v is uint)
                return (long)(uint)v;
            else if (v is double)
                return (long)(double)v;
            else if (v is byte)
                return (long)(byte)v;
            else if (v is sbyte)
                return (long)(sbyte)v;
            else if (v is char)
                return (long)(char)v;
            else if (v is short)
                return (long)(short)v;
            else if (v is ushort)
                return (long)(ushort)v;

            return def;
        }

        public static ulong ToUlong(object v, ulong def = 0) {
            if (v == null)
                return def;

            if (v is ulong)
                return (ulong)v;
            else if (v is long)
                return (ulong)(long)v;
            else if (v is int)
                return (ulong)(int)v;
            else if (v is uint)
                return (ulong)(uint)v;
            else if (v is double)
                return (ulong)(double)v;
            else if (v is byte)
                return (ulong)(byte)v;
            else if (v is sbyte)
                return (ulong)(sbyte)v;
            else if (v is char)
                return (ulong)(char)v;
            else if (v is short)
                return (ulong)(short)v;
            else if (v is ushort)
                return (ulong)(ushort)v;

            return def;
        }

        public static double ToDouble(object v, double def = 0.0) {
            if (v == null)
                return def;

            if (v is double)
                return (double)v;
            else if (v is int)
                return (double)(int)v;
            else if (v is uint)
                return (double)(uint)v;
            else if (v is long)
                return (double)(long)v;
            else if (v is ulong)
                return (double)(ulong)v;
            else if (v is byte)
                return (double)(byte)v;
            else if (v is sbyte)
                return (double)(sbyte)v;
            else if (v is char)
                return (double)(char)v;
            else if (v is short)
                return (double)(short)v;
            else if (v is ushort)
                return (double)(ushort)v;

            return def;
        }

        public static string ToStr(object v, string def = "") {
            if (v == null)
                return def;

            if (v is string)
                return (string)v;
            else
                return v.ToString();
        }

        public static Guid ToGuid(object v, Guid def) {
            if (v == null)
                return def;

            try {
                if (v is Guid)
                    return (Guid)v;
                else if (v is string)
                    return new Guid(v as string);
            }
            catch {
                return def;
            }
            
            return def;
        }

        public static System.Drawing.Color ToColor(object v, Color def) {
            if (v == null)
                return def;

            try {
                if (v is Vec3) {
                    var vec = (Vec3)v;
                    return Color.FromArgb(255, (int)(vec.X * 255.0), (int)(vec.Y * 255.0), (int)(vec.Z * 255.0));
                }
                else if (v is Vec4) {
                    var vec = (Vec4)v;
                    return Color.FromArgb((int)(vec.W * 255.0), (int)(vec.X * 255.0), (int)(vec.Y * 255.0), (int)(vec.Z * 255.0));
                }
                else if (v is int) {
                    return Color.FromArgb((int)v);
                }
                else if (v is uint) {
                    return Color.FromArgb((int)(uint)v);
                }
                else if (v is string) {
                    var s = (v as string).Trim().ToLower();
                    if (s.StartsWith("rgbaf")) {
                        var p0 = s.IndexOf('(');
                        var p1 = s.IndexOf(')');
                        if (p0 >= 0 && p1 >= 0) {
                            var rgb = s.Substring(p0 + 1, p1 - p0 - 1).Split(',');
                            if (rgb.Length != 4)
                                return def;

                            float r = float.Parse(rgb[0]);
                            float g = float.Parse(rgb[1]);
                            float b = float.Parse(rgb[2]);
                            float a = float.Parse(rgb[3]);
                            return Color.FromArgb((int)(a * 255.0f), (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f));
                        }
                    }
                    else if (s.StartsWith("rgbf")) {
                        var p0 = s.IndexOf('(');
                        var p1 = s.IndexOf(')');
                        if (p0 >= 0 && p1 >= 0) {
                            var rgb = s.Substring(p0 + 1, p1 - p0 - 1).Split(',');
                            if (rgb.Length != 3)
                                return def;

                            float r = float.Parse(rgb[0]);
                            float g = float.Parse(rgb[1]);
                            float b = float.Parse(rgb[2]);
                            return Color.FromArgb(255, (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f));
                        }
                    }
                    else if (s.StartsWith("rgba")) {
                        var p0 = s.IndexOf('(');
                        var p1 = s.IndexOf(')');
                        if (p0 >= 0 && p1 >= 0) {
                            var rgb = s.Substring(p0 + 1, p1 - p0 - 1).Split(',');
                            if (rgb.Length != 4)
                                return def;

                            int r = int.Parse(rgb[0]);
                            int g = int.Parse(rgb[1]);
                            int b = int.Parse(rgb[2]);
                            int a = int.Parse(rgb[3]);
                            return Color.FromArgb(a, r, g, b);
                        }
                    }
                    else if (s.StartsWith("rgb")) {
                        var p0 = s.IndexOf('(');
                        var p1 = s.IndexOf(')');
                        if (p0 >= 0 && p1 >= 0) {
                            var rgb = s.Substring(p0 + 1, p1 - p0 - 1).Split(',');
                            if (rgb.Length != 3)
                                return def;

                            int r = int.Parse(rgb[0]);
                            int g = int.Parse(rgb[1]);
                            int b = int.Parse(rgb[2]);
                            return Color.FromArgb(255, r, g, b);
                        }
                    }
                    else if (s.StartsWith("#")) {
                        var rgb = ulong.Parse(s.Substring(1), NumberStyles.HexNumber);

                        int a = (int)(rgb & 0xff000000) >> 24;
                        int r = (int)(rgb & 0xff0000) >> 16;
                        int g = (int)(rgb & 0xff00) >> 8;
                        int b = (int)(rgb & 0xff);
                        return Color.FromArgb(a, r, g, b);
                    }
                }
            }
            catch {
                return def;
            }

            return def;
        }

        public static Vec2 ToVec2(object v, Vec2 def) {
            if (v == null)
                return def;

            if (v is Vec2)
                return (Vec2)v;
            else if (v is double[]) {
                var arr = v as double[];
                if (arr.Length == 2)
                    return new Vec2(arr[0], arr[1]);
            }

            return def;
        }

        public static Vec3 ToVec3(object v, Vec3 def) {
            if (v == null)
                return def;

            if (v is Vec3)
                return (Vec3)v;
            else if (v is double[]) {
                var arr = v as double[];
                if (arr.Length == 3)
                    return new Vec3(arr[0], arr[1], arr[2]);
            }

            return def;
        }

        public static Vec4 ToVec4(object v, Vec4 def) {
            if (v == null)
                return def;

            if (v is Vec4)
                return (Vec4)v;
            else if (v is double[]) {
                var arr = v as double[];
                if (arr.Length == 4)
                    return new Vec4(arr[0], arr[1], arr[2], arr[3]);
            }

            return def;
        }

        public static int[] ToIntArr(object v) {
            if (v == null)
                return new int[0];

            if (v is int[])
                return v as int[];
            else if (v is long[])
                return (v as long[]).Select(x => (int)x).ToArray();
            else if (v is double[])
                return (v as double[]).Select(x => (int)x).ToArray();
            else if (v is Int32)
                return new int[] { (int)v };
            else if (v is long)
                return new int[] { (int)(long)v };
            else if (v is double)
                return new int[] { (int)(double)v };

            return new int[0];
        }

        public static long[] ToLongArr(object v) {
            if (v == null)
                return new long[0];

            if (v is long[])
                return v as long[];
            else if (v is int[])
                return (v as int[]).Select(x => (long)x).ToArray();
            else if (v is double[])
                return (v as double[]).Select(x => (long)x).ToArray();
            else if (v is long)
                return new long[] { (long)v };
            else if (v is Int32)
                return new long[] { (long)(int)v };
            else if (v is double)
                return new long[] { (long)(double)v };

            return new long[0];
        }

        public static double[] ToDoubleArr(object v) {
            if (v == null)
                return new double[0];

            if (v is double[])
                return v as double[];
            else if (v is int[])
                return (v as int[]).Select(x => (double)x).ToArray();
            else if (v is long[])
                return (v as long[]).Select(x => (double)x).ToArray();
            else if (v is double)
                return new double[] { (double)v };
            else if (v is Int32)
                return new double[] { (double)(int)v };
            else if (v is long)
                return new double[] { (double)(long)v };

            return new double[0];
        }

        public static string Epoch2Str(long tm, string fmt = "MM/dd/yyyy HH:mm") {
            return DATE_EPOCH.AddMilliseconds(tm).ToLocalTime().ToString(fmt);
        }

        public static long EpochTm() {
            return (long)DateTime.UtcNow.Subtract(DATE_EPOCH).TotalMilliseconds;
        }

        public static long EpochTm(DateTime dt) {
            return (long)dt.Subtract(DATE_EPOCH).TotalMilliseconds;
        }

        public static DateTime FromEpochTm(long epoch) {
            return DATE_EPOCH.AddMilliseconds(epoch);
        }

        public static string Str(double v, int enob) {
            if (enob <= 0)
                return ((int)v).ToString();

            string fmt = "{0:0." + new string('#', enob) + '}';
            return String.Format(fmt, v);
        }

        public static Tuple<int, int> Decimal2AdivB(double v, int enob) {
            if (v == 0.0)
                return new Tuple<int, int>(0, 0);

            enob = System.Math.Min(System.Math.Max(1, enob), 5);

            int bmax = 1;
            for (int i = 0; i < enob; ++i)
                bmax *= 10;

            int amin = 0, bmin = 0;
            double dmin = System.Math.Abs(v);
            for (int b = 2; b <= bmax; ++b) {
                int a = (int)System.Math.Round(b * v);
                if (a == 0)
                    continue;

                if (amin != 0
                    && (b / bmin == a / amin)
                    && (b % bmin == 0)
                    && (a % amin == 0))
                    continue;

                double approx = (double)a / b;
                double d = System.Math.Abs(approx - v);
                if (d < dmin) {
                    amin = a;
                    bmin = b;
                    dmin = d;
                }
            }

            return new Tuple<int, int>(amin, bmin);
        }
    }
}
