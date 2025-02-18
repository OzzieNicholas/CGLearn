using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Reflection;
using System.Configuration;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace LoongCAD.Util
{
    public class ExprEval
    {
        private static CodeDomProvider m_provider = CodeDomProvider.CreateProvider("CSharp");
        private CompilerParameters m_compilerParas = new CompilerParameters();
        private string m_errMsg = "";
        private string m_codePrefix = null;
        private string m_codePostfix = null;

        public ExprEval() {
            m_compilerParas.ReferencedAssemblies.Add("System.dll");
            m_compilerParas.ReferencedAssemblies.Add("System.Core.dll");
            m_compilerParas.CompilerOptions = "/t:library";
            m_compilerParas.GenerateInMemory = true;
            m_compilerParas.GenerateExecutable = false;

            StringBuilder sb = new StringBuilder();
            sb.AppendLine("using System;");
            sb.AppendLine("namespace ExprEval {");
            sb.AppendLine("public class Wrap {");
            sb.AppendLine("public static double sin(double v) {return Math.Sin(v * Math.PI / 180.0);}");
            sb.AppendLine("public static double cos(double v) {return Math.Cos(v * Math.PI / 180.0);}");
            sb.AppendLine("public static double tan(double v) {return Math.Tan(v * Math.PI / 180.0);}");
            sb.AppendLine("public static double asin(double v) {return Math.Asin(v);}");
            sb.AppendLine("public static double acos(double v) {return Math.Acos(v);}");
            sb.AppendLine("public static double abs(double v) {return Math.Abs(v);}");
            sb.AppendLine("public static double pow(double x, double y) {return Math.Pow(x, y);}");
            sb.AppendLine("public static double sqrt(double v) {return Math.Sqrt(v);}");
            sb.AppendLine("public static double round(double v) {return Math.Round(v);}");
            sb.AppendLine("public static double PI {get{return Math.PI;}}");
            sb.Append("public static object eval() {return ");

            m_codePrefix = sb.ToString();
            m_codePostfix = ";}}}";
        }

        public string ErrMsg => m_errMsg;

        public object Eval(string code) {
            m_errMsg = "";

            var cr = m_provider.CompileAssemblyFromSource(m_compilerParas, string.Concat(m_codePrefix, code, m_codePostfix));
            if (cr.Errors.Count > 0) {
                foreach (CompilerError err in cr.Errors) {
                    m_errMsg += "Line" + err.Line + "(" + err.Column + ") : ";
                    m_errMsg += err.ErrorText;
                    m_errMsg += "\n";
                }

                return null;
            }

            var ass = cr.CompiledAssembly;
            var t = ass.GetType("ExprEval.Wrap");
            MethodInfo mi = t.GetMethod("eval");
            var res = mi.Invoke(null, null);
            return res;
        }
    }
}
