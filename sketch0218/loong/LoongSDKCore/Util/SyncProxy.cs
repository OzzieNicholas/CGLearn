using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Geometry;

namespace LoongCAD.Util
{


    /// <summary>
    /// 异步属性交互基类
    /// </summary>
    public class SyncProxy
    {

        /// <summary>
        /// 属性
        /// </summary>
        protected DObject m_props = null;

        /// <summary>
        /// 临时属性
        /// </summary>
        protected DObject m_tmpProps = new DObject();

        /// <summary>
        /// 是否需要传递
        /// </summary>
        public virtual bool NeedFlush() {
            return m_tmpProps.Count > 0;
        }

        /// <summary>
        /// 传递属性
        /// </summary>
        public virtual void Flush() {
            
        }

        /// <summary>
        /// 同步对象属性
        /// </summary>
        /// <returns>是否同步成功</returns>
        protected virtual bool _sync() {
            return true;
        }

        /// <summary>
        /// 获取属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns></returns>
        protected object _getProp(string pname, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props == null)
                return null;

            m_props.TryGetValue(pname, out object res);
            return res;
        }

        /// <summary>
        /// 获取bool类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected bool _getPropBool(string pname, bool def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToBool(res);

            return def;
        }

        /// <summary>
        /// 获取sbyte类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected sbyte _getPropSByte(string pname, sbyte def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return (sbyte)Utils.ToInt(res);

            return def;
        }

        /// <summary>
        /// 获取long类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected long _getPropLong(string pname, long def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToLong(res);

            return def;
        }

        /// <summary>
        /// 获取int类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected int _getPropInt(string pname, int def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToInt(res);

            return def;
        }

        /// <summary>
        /// 获取uint类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected uint _getPropUint(string pname, uint def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToUint(res);

            return def;
        }

        /// <summary>
        /// 获取short类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected short _getPropShort(string pname, short def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return (short)Utils.ToInt(res);

            return def;
        }

        /// <summary>
        /// 获取ushort类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected ushort _getPropUShort(string pname, ushort def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return (ushort)Utils.ToInt(res);

            return def;
        }

        /// <summary>
        /// 获取double类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected double _getPropDouble(string pname, double def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToDouble(res);

            return def;
        }

        /// <summary>
        /// 获取string类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected string _getPropString(string pname, string def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)
                && res != null)
                return res is string ? (res as string) : res.ToString();

            return def;
        }

        /// <summary>
        /// 获取Guid类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Guid _getPropGuid(string pname, Guid def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)) {
                return Utils.ToGuid(res, def);
            }

            return def;
        }

        /// <summary>
        /// 获取Color类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Color _getPropColor(string pname, Color def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)) {
                return Utils.ToColor(res, def);
            }

            return def;
        }

        /// <summary>
        /// 获取vec2类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Vec2 _getPropVec2(string pname, Vec2 def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)) {
                return Utils.ToVec2(res, def);
            }

            return def;
        }

        /// <summary>
        /// 获取vec3类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Vec3 _getPropVec3(string pname, Vec3 def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)) {
                return Utils.ToVec3(res, def);
            }

            return def;
        }

        /// <summary>
        /// 获取vec4类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Vec4 _getPropVec4(string pname, Vec4 def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)) {
                return Utils.ToVec4(res, def);
            }

            return def;
        }

        /// <summary>
        /// 获取Mat43类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Mat43 _getPropMat43(string pname, Mat43 def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)
                && res is Mat43)
                return (Mat43)res;

            return def;
        }

        /// <summary>
        /// 获取Plane类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected Plane _getPropPlane(string pname, Plane def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)
                && res is Plane)
                return (Plane)res;

            return def;
        }

        /// <summary>
        /// 获取DObject类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected DObject _getPropDObject(string pname, DObject def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)
                && res is DObject)
                return (DObject)res;

            return def;
        }

        /// <summary>
        /// 获取int[]类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected int[] _getPropIntArr(string pname, int[] def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToIntArr(res);

            return def;
        }

        /// <summary>
        /// 获取long[]类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected long[] _getPropLongArr(string pname, long[] def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToLongArr(res);

            return def;
        }

        /// <summary>
        /// 获取double类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected double[] _getPropDoubleArr(string pname, double[] def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res))
                return Utils.ToDoubleArr(res);

            return def;
        }

        /// <summary>
        /// 获取string[]类型的属性
        /// </summary>
        /// <param name="pname">属性名</param>
        /// <param name="def">属性不存在时返回的默认值</param>
        /// <param name="bSync">是否先同步再获取</param>
        /// <returns>是否获取成功</returns>
        protected string[] _getPropStringArr(string pname, string[] def, bool bSync = true) {
            if (bSync)
                this._sync();

            if (m_props != null
                && m_props.TryGetValue(pname, out object res)
                && res is string[])
                return res as string[];

            return def;
        }
    }
}
