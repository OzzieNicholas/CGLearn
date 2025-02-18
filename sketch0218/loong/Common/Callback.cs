using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Resources;
using LoongCAD.Util;

namespace LoongCAD.Common
{
    public class Callback
    {
        public static Func<string, object> evalJSExpr = null;

        //wrapper callbacks
        public static Action<IntPtr, IntPtr, IntPtr, Action> initCore = null;

        //command methods
        public static Action<string, DObject, Action<DObject>> pushCommand = null;
        public static Func<string, DObject, DObject> execute = null;

        public static void regUICmdListener(Func<string, DObject, DObject> cbFunc, string cmd = "") {
            if (m_uiCmdListeners.ContainsKey(cmd))
                m_uiCmdListeners[cmd] += cbFunc; //NOTE: can not use tryGetValue, adding to returned delegate has no effect
            else
                m_uiCmdListeners[cmd] = cbFunc;
        }

        public static void removeUICmdListener(Func<string, DObject, DObject> cbFunc, string cmd = "") {
            Func<string, DObject, DObject> cbFuncs = null;
            if (m_uiCmdListeners.TryGetValue(cmd, out cbFuncs)) {
                cbFuncs -= cbFunc;
                if (cbFuncs == null)
                    m_uiCmdListeners.Remove(cmd);
            }
        }

        public static DObject onUICommand(string cmd, DObject paras) {
            Func<string, DObject, DObject> cbFunc = null;
            DObject finalRet = null;

            if (m_uiCmdListeners.TryGetValue("", out cbFunc)) {//监听所有消息的监听器
                var ret = cbFunc(cmd, paras);
                if (ret != null)
                    finalRet = ret;
            }

            if (m_uiCmdListeners.TryGetValue(cmd, out cbFunc)) {//监听当前消息的监听器
                var ret = cbFunc(cmd, paras);
                if (ret != null)
                    finalRet = ret;
            }

            return finalRet;
        }

        protected static Dictionary<string, Func<string, DObject, DObject>> m_uiCmdListeners = new Dictionary<string, Func<string, DObject, DObject>>();
    }
}
