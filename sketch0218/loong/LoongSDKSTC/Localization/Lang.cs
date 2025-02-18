using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace LoongCAD.Localization
{

    /// <summary>
    /// 多语言支持类
    /// </summary>
    public class Lang {

        /// <summary>
        /// 当前语言名称
        /// </summary>
        protected static string m_language = "chinese";

        /// <summary>
        /// 当前语言定义的词典
        /// </summary>
        protected static Dictionary<string, string> m_defWords = new Dictionary<string, string>();

        /// <summary>
        /// 当前环境定义的词典
        /// </summary>
        protected static Dictionary<string, Dictionary<string, string>> m_envWords = new Dictionary<string, Dictionary<string, string>>();

        /// <summary>
        /// 全局初始化
        /// </summary>
        /// <param name="lang"></param>
        public static void Init(string lang) {
            m_language = lang;
            _load();
        }

        /// <summary>
        /// 当前语言
        /// </summary>
        public static string Language {
            get => m_language;
            set {
                m_language = value;
                _load();
            }
        }

        private static bool _load() {
            var qry = Native.getLanguageConfig(m_language);
            if (qry == null)
                return false;

            m_defWords.Clear();
            m_envWords.Clear();
            foreach (var it in qry) {
                if (!(it.Value is DObject))
                    continue;

                string env = it.Key;

                if (env == "default") {
                    foreach (var itt in (it.Value as DObject)) {
                        if (!(itt.Value is string))
                            continue;

                        m_defWords[itt.Key] = itt.Value as string;
                    }
                }
                else {
                    var words = new Dictionary<string, string>();
                    foreach (var itt in (it.Value as DObject)) {
                        if (!(itt.Value is string))
                            continue;

                        words[itt.Key] = itt.Value as string;
                    }
                    m_envWords[env] = words;
                }
            }

            return true;
        }

        /// <summary>
        /// 将源字段转译为当前语言下的字段
        /// </summary>
        /// <param name="text">源字段</param>
        /// <param name="env">所在环境</param>
        /// <returns>转译后的字段</returns>
        public static string Local(string text, string env = null) {
            if (String.IsNullOrEmpty(env))
                env = "default";

            string locWord = null;

            if (env != "default") {
                Dictionary<string, string> words = null;
                if (m_envWords.TryGetValue(env, out words)) {
                    if (words.TryGetValue(text, out locWord)) {
                        return locWord;
                    }
                }
            }

            if (m_defWords.TryGetValue(text, out locWord))
                return locWord;

            return text;
        }

    }
}
