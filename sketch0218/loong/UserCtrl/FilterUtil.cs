using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace UserCtrl
{
    public class FilterUtil
    {
        private FilterUtil() {
            
        }

        public static string likeValue(string s) {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < s.Length; i++) {
                char c = s[i];
                if (c == '*' || c == '%' || c == '[' || c == ']')
                    sb.Append("[").Append(c).Append("]");
                else if (c == '\'')
                    sb.Append("''");
                else
                    sb.Append(c);
            }
            return sb.ToString();
        }

        public static string opEqual(string var, int filter) {
            var s = String.Format("{0} = {1}", var, filter);
            return s;
        }

        public static string opContain(string var, string filter) {
            var s = String.Format("{0} LIKE '%{1}%'", var, likeValue(filter));
            return s;
        }

        public static string opIn(string var, List<string> filters) {
            StringBuilder sb = new StringBuilder();
            sb.Append(var).Append(" IN (");
            for (int i = 0; i < filters.Count; ++i) {
                sb.AppendFormat((i > 0)? ",'{0}'" : "'{0}'", filters[i]);
            }
            sb.Append(")");

            return sb.ToString();
        }

        public static string opIn(string var, List<int> filters, bool bStr) {
            StringBuilder sb = new StringBuilder();
            sb.Append(var).Append(" IN (");
            if (bStr) {
                for (int i = 0; i < filters.Count; ++i) {
                    sb.AppendFormat((i > 0) ? ",'{0}'" : "'{0}'", filters[i]);
                }
            }
            else {
                for (int i = 0; i < filters.Count; ++i) {
                    sb.AppendFormat((i > 0) ? ",{0}" : "{0}", filters[i]);
                }
            }
            
            sb.Append(")");

            return sb.ToString();
        }
    }
}
