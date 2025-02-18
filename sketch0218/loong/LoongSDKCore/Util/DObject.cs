using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using LoongCAD.Geometry;

namespace LoongCAD.Util
{

    public class DObject : Dictionary<string, object>
    {
        public static readonly DObject EMPTY = new DObject();

        public DObject() {

        }

        public DObject(object[] paras) {
            if (paras == null)
                return;

            for (int i = 0; i < paras.Length; i += 2) {
                string key = paras[i] as string;
                var val = paras[i + 1];

                if (val is DObject)
                    this.Add(key, (val as DObject).Clone());
                else
                    this.Add(key, val);
            }
        }

        public static implicit operator DObject(object[] paras) {
            return new DObject(paras);
        }

        public override bool Equals(object v) {
            if (v == null || !(v is DObject))
                return false;

            DObject obj = v as DObject;
            if (this.Count != obj.Count)
                return false;

            if (this == obj)
                return true;

            foreach (var it in obj) {
                if (!this.ContainsKey(it.Key))
                    return false;

                object val0 = this[it.Key];
                object val1 = it.Value;

                if (val0 == null && val1 == null)
                    continue;

                if (val0 == null || val1 == null)
                    return false;

                if (val0 is Int32
                    || val0 is UInt32
                    || val0 is Int64
                    || val0 is UInt64
                    || val0 is Double
                    || val0 is Boolean
                    || val0 is Byte
                    || val0 is SByte
                    || val0 is short
                    || val0 is ushort
                    || val0 is char
                    || val0 is Guid
                    || val0 is string
                    || val0 is Vec2
                    || val0 is Vec3
                    || val0 is Vec4
                    || val0 is Vec2i
                    || val0 is Vec3i
                    || val0 is Vec4i
                    || val0 is Mat43
                    || val0 is Mat4
                    || val0 is AABB
                    || val0 is OBB
                    || val0 is Plane
                    || val0 is Line
                    || val0 is DObject) {
                    if (!val0.Equals(val1))
                        return false;
                }
                else if (val0.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                    if (!val1.GetType().FullName.StartsWith("LoongCAD.STC.Define")
                        || Utils.ToInt(val0) != Utils.ToInt(val1))
                        return false;
                }
                else if (val0 is IEnumerable<bool>) {
                    if (!(val1 is IEnumerable<bool>))
                        return false;

                    IEnumerable<bool> arr0 = val0 as IEnumerable<bool>;
                    IEnumerable<bool> arr1 = val1 as IEnumerable<bool>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<int>) {
                    if (!(val1 is IEnumerable<int>))
                        return false;

                    IEnumerable<int> arr0 = val0 as IEnumerable<int>;
                    IEnumerable<int> arr1 = val1 as IEnumerable<int>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<long>) {
                    if (!(val1 is IEnumerable<long>))
                        return false;

                    IEnumerable<long> arr0 = val0 as IEnumerable<long>;
                    IEnumerable<long> arr1 = val1 as IEnumerable<long>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<double>) {
                    if (!(val1 is IEnumerable<double>))
                        return false;

                    IEnumerable<double> arr0 = val0 as IEnumerable<double>;
                    IEnumerable<double> arr1 = val1 as IEnumerable<double>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<string>) {
                    if (!(val1 is IEnumerable<string>))
                        return false;

                    IEnumerable<string> arr0 = val0 as IEnumerable<string>;
                    IEnumerable<string> arr1 = val1 as IEnumerable<string>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec2>) {
                    if (!(val1 is IEnumerable<Vec2>))
                        return false;

                    IEnumerable<Vec2> arr0 = val0 as IEnumerable<Vec2>;
                    IEnumerable<Vec2> arr1 = val1 as IEnumerable<Vec2>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec3>) {
                    if (!(val1 is IEnumerable<Vec3>))
                        return false;

                    IEnumerable<Vec3> arr0 = val0 as IEnumerable<Vec3>;
                    IEnumerable<Vec3> arr1 = val1 as IEnumerable<Vec3>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec4>) {
                    if (!(val1 is IEnumerable<Vec4>))
                        return false;

                    IEnumerable<Vec4> arr0 = val0 as IEnumerable<Vec4>;
                    IEnumerable<Vec4> arr1 = val1 as IEnumerable<Vec4>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec2i>) {
                    if (!(val1 is IEnumerable<Vec2i>))
                        return false;

                    IEnumerable<Vec2i> arr0 = val0 as IEnumerable<Vec2i>;
                    IEnumerable<Vec2i> arr1 = val1 as IEnumerable<Vec2i>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec3i>) {
                    if (!(val1 is IEnumerable<Vec3i>))
                        return false;

                    IEnumerable<Vec3i> arr0 = val0 as IEnumerable<Vec3i>;
                    IEnumerable<Vec3i> arr1 = val1 as IEnumerable<Vec3i>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Vec4i>) {
                    if (!(val1 is IEnumerable<Vec4i>))
                        return false;

                    IEnumerable<Vec4i> arr0 = val0 as IEnumerable<Vec4i>;
                    IEnumerable<Vec4i> arr1 = val1 as IEnumerable<Vec4i>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Mat43>) {
                    if (!(val1 is IEnumerable<Mat43>))
                        return false;

                    IEnumerable<Mat43> arr0 = val0 as IEnumerable<Mat43>;
                    IEnumerable<Mat43> arr1 = val1 as IEnumerable<Mat43>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Mat4>) {
                    if (!(val1 is IEnumerable<Mat4>))
                        return false;

                    IEnumerable<Mat4> arr0 = val0 as IEnumerable<Mat4>;
                    IEnumerable<Mat4> arr1 = val1 as IEnumerable<Mat4>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Line>) {
                    if (!(val1 is IEnumerable<Line>))
                        return false;

                    IEnumerable<Line> arr0 = val0 as IEnumerable<Line>;
                    IEnumerable<Line> arr1 = val1 as IEnumerable<Line>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<Plane>) {
                    if (!(val1 is IEnumerable<Plane>))
                        return false;

                    IEnumerable<Plane> arr0 = val0 as IEnumerable<Plane>;
                    IEnumerable<Plane> arr1 = val1 as IEnumerable<Plane>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<OBB>) {
                    if (!(val1 is IEnumerable<OBB>))
                        return false;

                    IEnumerable<OBB> arr0 = val0 as IEnumerable<OBB>;
                    IEnumerable<OBB> arr1 = val1 as IEnumerable<OBB>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<AABB>) {
                    if (!(val1 is IEnumerable<AABB>))
                        return false;

                    IEnumerable<AABB> arr0 = val0 as IEnumerable<AABB>;
                    IEnumerable<AABB> arr1 = val1 as IEnumerable<AABB>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
                else if (val0 is IEnumerable<DObject>) {
                    if (!(val1 is IEnumerable<DObject>))
                        return false;

                    IEnumerable<DObject> arr0 = val0 as IEnumerable<DObject>;
                    IEnumerable<DObject> arr1 = val1 as IEnumerable<DObject>;
                    if (!arr0.SequenceEqual(arr1))
                        return false;
                }
            }

            return true;
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }

        public DObject Clone() {
            DObject copy = new DObject();
            copy.CopyFrom(this);
            return copy;
        }

        public void CopyFrom(DObject src) {
            if (src == null)
                return;

            foreach (var it in src) {
                if (it.Value is DObject) {
                    if (!this.ContainsKey(it.Key))
                        this[it.Key] = _cloneValue(it.Value);
                    else
                        this.GetDObject(it.Key).CopyFrom(it.Value as DObject);
                }
                else
                    this[it.Key] = _cloneValue(it.Value);
            }
        }

        public bool TryGet(string key, ref double val) {
            object tmpVal = null;
            if (!TryGetValue(key, out tmpVal)) {
                return false;
            }

            if (tmpVal is double)
                val = (double)tmpVal;

            return true;
        }

        public bool TryGet(string key, ref string val) {
            object tmpVal = null;
            if (!TryGetValue(key, out tmpVal)) {
                return false;
            }

            val = (tmpVal != null) ? tmpVal.ToString() : null;
            return true;
        }

        private Object _cloneValue(Object pObj) {
            if (pObj == null) {
                return null;
            }
            else if (pObj is Int32
                || pObj is UInt32
                || pObj is Int64
                || pObj is UInt64
                || pObj is Int16
                || pObj is UInt16
                || pObj is Double
                || pObj is Boolean
                || pObj is Char
                || pObj is string
                || pObj is SByte
                || pObj is Byte
                || pObj is Vec2
                || pObj is Vec3
                || pObj is Vec4
                || pObj is Vec2i
                || pObj is Vec3i
                || pObj is Vec4i
                || pObj is Mat43
                || pObj is Mat4
                || pObj is OBB
                || pObj is AABB
#if false
                || pObj is Plane
#endif
                || pObj is Line) {
                return pObj;
            }
            else if (pObj is Plane) {
                return (pObj as Plane).Clone();
            }
            else if (pObj is DObject) {
                return (pObj as DObject).Clone();
            }
            else if (pObj.GetType().FullName.StartsWith("LoongCAD.STC.Define")) {
                return pObj;
            }
            else if (pObj is IEnumerable<double>) {
                return (pObj as IEnumerable<double>).ToArray();
            }
            else if (pObj is IEnumerable<string>) {
                return (pObj as IEnumerable<string>).ToArray();
            }
            else if (pObj is IEnumerable<int>) {
                return (pObj as IEnumerable<int>).ToArray();
            }
            else if (pObj is IEnumerable<uint>) {
                return (pObj as IEnumerable<uint>).ToArray();
            }
            else if (pObj is IEnumerable<long>) {
                return (pObj as IEnumerable<long>).ToArray();
            }
            else if (pObj is IEnumerable<ulong>) {
                return (pObj as IEnumerable<ulong>).ToArray();
            }
            else if (pObj is IEnumerable<short>) {
                return (pObj as IEnumerable<short>).ToArray();
            }
            else if (pObj is IEnumerable<ushort>) {
                return (pObj as IEnumerable<ushort>).ToArray();
            }
            else if (pObj is IEnumerable<bool>) {
                return (pObj as IEnumerable<bool>).ToArray();
            }
            else if (pObj is IEnumerable<char>) {
                return (pObj as IEnumerable<char>).ToArray();
            }
            else if (pObj is IEnumerable<Vec2>) {
                return (pObj as IEnumerable<Vec2>).ToArray();
            }
            else if (pObj is IEnumerable<Vec3>) {
                return (pObj as IEnumerable<Vec3>).ToArray();
            }
            else if (pObj is IEnumerable<Vec4>) {
                return (pObj as IEnumerable<Vec4>).ToArray();
            }
            else if (pObj is IEnumerable<Vec2i>) {
                return (pObj as IEnumerable<Vec2i>).ToArray();
            }
            else if (pObj is IEnumerable<Vec3i>) {
                return (pObj as IEnumerable<Vec3i>).ToArray();
            }
            else if (pObj is IEnumerable<Vec4i>) {
                return (pObj as IEnumerable<Vec4i>).ToArray();
            }
            else if (pObj is IEnumerable<Mat43>) {
                return (pObj as IEnumerable<Mat43>).ToArray();
            }
            else if (pObj is IEnumerable<Mat4>) {
                return (pObj as IEnumerable<Mat4>).ToArray();
            }
#if false
            else if (pObj is IEnumerable<Plane>) {
                return (pObj as IEnumerable<Plane>).ToArray();
            }
#else
            else if(pObj is IEnumerable<Plane>)
            {
                IEnumerable<Plane> pArrSrc = pObj as IEnumerable<Plane>;
                Plane[] pArr = new Plane[pArrSrc.Count()];

                int n = 0;
                foreach (var val in pArrSrc) {
                    pArr[n++] = val.Clone();
                }

                return pArr;
            }
#endif
            else if (pObj is IEnumerable<Line>) {
                return (pObj as IEnumerable<Line>).ToArray();
            }
            else if (pObj is IEnumerable<OBB>) {
                return (pObj as IEnumerable<OBB>).ToArray();
            }
            else if (pObj is IEnumerable<AABB>) {
                return (pObj as IEnumerable<AABB>).ToArray();
            }
            else if (pObj is IEnumerable<DObject>) {
                IEnumerable<DObject> pArrSrc = pObj as IEnumerable<DObject>;
                DObject[] pArr = new DObject[pArrSrc.Count()];

                int n = 0;
                foreach (var val in pArrSrc) {
                    pArr[n++] = val.Clone();
                }

                return pArr;
            }
            else if (pObj is IEnumerable<Object>) {
                IEnumerable<Object> pArrSrc = pObj as IEnumerable<Object>;
                Object[] pArr = new Object[pArrSrc.Count()];

                int n = 0;
                foreach (var val in pArrSrc) {
                    pArr[n++] = _cloneValue(val);
                }

                return pArr;
            }

            return null;
        }

        public DObject GetOrCreate(string key) {
            if (this.TryGetValue(key, out object res)
                && res is DObject)
                return res as DObject;

            var child = new DObject();
            this[key] = child;
            return child;
        }

        public object Get(string key, object def = null) {
            object val = null;
            if (this.TryGetValue(key, out val))
                return val;

            return def;
        }

        public string GetString(string key, string def = "") {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return val as string;
        }

        public bool GetBool(string key, bool def = false) {
            object val = null;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToBool(val, def);
        }

        public bool[] GetBoolArr(string key) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return new bool[0];

            return val as bool[];
        }


        public sbyte GetSByte(string key, sbyte def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToSByte(val, def);
        }

        public short GetShort(string key, short def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToShort(val, def);
        }

        public ushort GetUshort(string key, ushort def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToUshort(val, def);
        }

        public int GetInt(string key, int def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToInt(val, def);
        }

        public uint GetUint(string key, uint def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToUint(val, def);
        }

        public long GetLong(string key, long def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToLong(val, def);
        }

        public ulong GetUlong(string key, ulong def = 0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToUlong(val, def);
        }

        public double GetDouble(string key, double def = 0.0) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return def;

            return Utils.ToDouble(val, def);
        }

        public Guid GetGuid(string key) {
            object val;
            if (!this.TryGetValue(key, out val)
                || val == null)
                return Guid.Empty;

            return Utils.ToGuid(val, Guid.Empty);
        }

        public Color GetColor(string key) {
            object val;
            if (!this.TryGetValue(key, out val))
                return Color.Black;

            return Utils.ToColor(val, Color.Black);
        }

        public Vec2 GetVec2(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec2))
                return new Vec2(0.0, 0.0);

            return (Vec2)val;
        }

        public Vec3 GetVec3(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec3))
                return new Vec3(0.0, 0.0, 0.0);

            return (Vec3)val;
        }

        public Vec2i GetVec2i(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec2i))
                return new Vec2i(0, 0);

            return (Vec2i)val;
        }

        public Point GetPoint(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec2i))
                return new Point(0, 0);

            Vec2i vec2 = (Vec2i)val;
            return new Point(vec2.X, vec2.Y);
        }

        public Vec3i GetVec3i(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec3i))
                return new Vec3i(0, 0, 0);

            return (Vec3i)val;
        }

        public Vec4i GetVec4i(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec4i))
                return new Vec4i(0, 0, 0, 0);

            return (Vec4i)val;
        }

        public Line GetLine(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Line))
                return new Line();

            return (Line)val;
        }

        public Rectangle GetRect(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Vec4i))
                return new Rectangle(0, 0, 0, 0);

            Vec4i vec = (Vec4i)val;
            return new Rectangle(vec.X, vec.Y, vec.Z, vec.W);
        }

        public Mat43 GetMat43(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Mat43))
                return Mat43.IDENTITY;

            return (Mat43)val;
        }

        public Mat4 GetMat4(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Mat4))
                return Mat4.IDENTITY;

            return (Mat4)val;
        }

        public AABB GetAABB(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is AABB))
                return new AABB(Vec3.ZERO, Vec3.ZERO);

            return (AABB)val;
        }

        public OBB GetOBB(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is OBB))
                return new OBB(Vec3.ZERO, Vec3.ZERO, Mat43.IDENTITY);

            return (OBB)val;
        }

        public Plane GetPlane(string key) {
            object val;
            if (!this.TryGetValue(key, out val) || !(val is Plane))
                return new Plane(Mat43.IDENTITY);

            return (Plane)val;
        }

        public int[] GetIntArr(string key) {
            object val;
            if (!this.TryGetValue(key, out val))
                return new int[0];

            return val as int[];
        }

        public long[] GetLongArr(string key) {
            object val;
            if (!this.TryGetValue(key, out val))
                return new long[0];

            return val as long[];
        }

        public double[] GetDoubleArr(string key) {
            object val;
            if (!this.TryGetValue(key, out val))
                return new double[0];

            return val as double[];
        }

        public string[] GetStringArr(string key) {
            object val;
            if (!this.TryGetValue(key, out val))
                return new string[0];

            return val as string[];
        }

        public DObject GetDObject(string key, DObject def = null) {
            object val = null;
            if (!this.TryGetValue(key, out val))
                return def;

            if (val is DObject)
                return val as DObject;

            return def;
        }

        public DObject[] GetDObjectArr(string key) {
            object val = null;
            if (!this.TryGetValue(key, out val))
                return null;

            if (val is DObject[])
                return val as DObject[];

            return null;
        }
    }
}
