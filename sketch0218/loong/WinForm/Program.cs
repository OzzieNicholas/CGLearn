using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.IO;
using System.Runtime.ExceptionServices;
using System.Runtime.InteropServices;
using System.Security;
using System.Diagnostics;
using Wrapper;
using UserCtrl;
using IFWrapper;
using LoongCAD.Common;
using LoongCAD.STC;

namespace WinForm
{
    static class Program
    {
        public static Wrapper.STWrapper m_wrapper = null;

        [DllImport("sketch.dll")]
        public static extern void appMessageLoop();

        [DllImport("sketch.dll")]
        public static extern void appUpdate();

        [HandleProcessCorruptedStateExceptions]
        [SecurityCritical]
        [STAThread]
        static void Main(string[] args) {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            m_wrapper = new Wrapper.STWrapper();

            //提示SDK UI控件当前为运行时，非设计状态
            Define.IS_RUNTIME = true;

            initCallback();

//#if RELEASE
//            try {
//#endif
            //解析启动命令
            string initProjPath = "";
            if (args.Length > 0) {
                int n = 0;
                while (n < args.Length) {
                    switch (args[n]) {
                        case "-o": {//Open project
                            if (n < args.Length - 1) {
                                initProjPath = args[n + 1].Trim();
                            }
                            
                            n++;
                        }
                        break;
                    }

                    n++;
                }
            }

            Form_Start.instance.Show();
            var mainForm = new UI.MainForm();

            if (!String.IsNullOrEmpty(initProjPath)) {
                mainForm.onInitComplete += () => {
                    UI.Form_OpenProject.instance.openProject(initProjPath);
                };
            }

            mainForm.init();
            MsgFilterManager.start();
            Application.Idle += Application_Idle; //有模态窗口时，保持Core更新

            //mainForm.Show();
            appMessageLoop();

            MsgFilterManager.stop();
//#if RELEASE
//            } catch (Exception ex) {
//                Form_ErrMsg form = new Form_ErrMsg();
//                form.msg = ex.Message + "\n" + ex.StackTrace;
//                form.ShowDialog();
//            }
//#endif
        }

        private static void Application_Idle(object sender, EventArgs e) {
            appUpdate();
        }

        static void initCallback() {
            Callback.evalJSExpr = Native.evalJSExpr;
            Callback.initCore = m_wrapper.init;
            Callback.execute = m_wrapper.execute;
            Callback.pushCommand = m_wrapper.pushCommand;
        }
    }
}
