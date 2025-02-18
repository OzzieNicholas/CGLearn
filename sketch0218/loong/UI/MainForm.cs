using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using System.Runtime.InteropServices;
using UserCtrl;
using IFWrapper;
using Newtonsoft.Json;
using System.Linq;
using Newtonsoft.Json.Linq;

namespace UI
{
    public partial class MainForm : Form
    {
        private static MainForm m_instance = null;

        private bool m_disposing = false;

        //input
        private FlowLayoutPanel flowPanel; 
        private Label inputLabel;
        private TextBox inputTextBox;

        //tree
        private TreeView objTreeView;

        private Dictionary<string, bool> m_initJobs = new Dictionary<string, bool>(); //系统初始化时要完成的任务，当所有任务都完成时，欢迎页消失，主窗口显示
        private Dictionary<string, int> m_initJobProgress = new Dictionary<string, int>(); //系统初始化任务所占的进度条比例
        public Action onInitComplete = null;

        private MainFormTopPanel pan_top = null;
        private MainFormContentPanel pan_content = null;
        private MainStatus status_main = null;

        private Dictionary<string, ProgressMsgBox> m_progMsgBoxes = new Dictionary<string, ProgressMsgBox>();

        public static MainForm instance => m_instance;

        public bool disposing {
            get => m_disposing;
            set => m_disposing = value;
        }

        public MainForm() {
            m_instance = this;

            Win32.SetWindowTheme(this.Handle, "", "");

            InitializeComponent();

            this.SetStyle(ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint, true);
            //this.TransparencyKey = Color.FromArgb(255, 3, 2, 1);
            this.AllowTransparency = false;

            //input
            flowPanel = new FlowLayoutPanel { AutoSize = true, FlowDirection = FlowDirection.LeftToRight};
            flowPanel.Location = new System.Drawing.Point(65, 118);
            flowPanel.Size = new System.Drawing.Size(160, 30);
            inputLabel = new Label { Text = "type", Width = 50, Enabled = false, Margin = new Padding(0, 8, 0, 0) };
            inputTextBox = new TextBox { Width = 100, Height = 25, Enabled = false, Margin = new Padding(0, 5, 0, 0) };
            flowPanel.Controls.Add(inputLabel);
            flowPanel.Controls.Add(inputTextBox);
            this.Controls.Add(flowPanel);
            //tree
            objTreeView = new TreeView { Dock = DockStyle.Right, Width = 200 };
            this.Controls.Add(objTreeView);

            this.SuspendLayout();

            int borderX = (this.Width - this.ClientSize.Width) / 2;
            int borderY = (this.Height - this.ClientSize.Height) / 2;
            
            pan_top = new MainFormTopPanel();
            pan_top.Dock = DockStyle.Top;
            pan_top.SetBounds(borderX, borderY, this.ClientSize.Width, DPI.Scale(MainFormTopPanel.ORG_HEIGHT));

            status_main = new MainStatus();
            status_main.Dock = DockStyle.Bottom;

            pan_content = new MainFormContentPanel();
            pan_content.SetBounds(borderX, borderY + pan_top.Height, Math.Max(0, this.ClientSize.Width), Math.Max(0, this.ClientSize.Height - pan_top.Height - status_main.Height));
            pan_content.Padding = new Padding(this.clientBorderMargin, 0, this.clientBorderMargin, 0);
            pan_content.Dock = DockStyle.Fill;

            this.Controls.AddRange(new Control[] { pan_content, pan_top, status_main });

            this.ResumeLayout();
        }

        public void init() {
#if !DEBUG
            m_initJobs["preload_sdf"] = false;
            m_initJobProgress["preload_sdf"] = 100;
#endif

            //初始化Core层
            Callback.initCore(this.Handle, pan_content.viewPanel.Handle, pan_top.Handle, this.onCoreInit);
            Callback.regUICmdListener(this.onCommand);
        }

        //Core层初始化完成回调
        private void onCoreInit() {
            //初始化语言管理
            Lang.Init("chinese");

            //初始化FU
            FuncUnit.init();

            //因为FU需要先初始化默认快捷键，所以shortcut不能在STCore::init中加载
            Native.initShortCuts();

            //初始化动态版式管理器(管理维护主菜单、工具栏)
            var customBarsProvision = CustomBarsProvision.instance;
            LayoutManager.instance.init();
            Native.onCanvasResize(pan_content.viewPanel.Width, pan_content.viewPanel.Height);

            //初始化各个窗口
            this._initForms();

            if (m_initJobs.Count == 0)//无初始化任务时，关闭欢迎窗口
                this.onInitJobDone("");

            //Form_Comp_Editor.instance.beginEdit(new object[] {
            //    "cfg_guid", "754C23D004D645D1A53692425D0E4E73",
            //    "cfg_name", "TestCase-FullDepthS",
            //});
        }

        public int initJobProgress(string job) {
            int prog = 0;
            m_initJobProgress.TryGetValue(job, out prog);
            return prog;
        }

        public void onInitJobDone(string job) {
            m_initJobs[job] = true;

            foreach (var it in m_initJobs) {
                if (it.Value == false)
                    return;
            }

            Form_Start.instance.Close();

            this.Show();
            this.WindowState = FormWindowState.Maximized;

            if (onInitComplete != null)
                onInitComplete();
        }

        [Browsable(false)]
        public int clientBorderMargin => DPI.Scale(4);

        [Browsable(false)]
        public MainFormTopPanel topPanel => pan_top;

        //工具栏的动态容器
        [Browsable(false)]
        public MainFormLayoutContainer barRoot => pan_content.layout;

        //主视图
        [Browsable(false)]
        public MainView viewPanel => pan_content.viewPanel;

        [Browsable(false)]
        public MainStatus statusBar => status_main;

        [Browsable(false)]
        public CellTab headerTab => pan_content.headerTab;

        //主菜单
        public MenuStrip mainMenu {
            get => pan_top.menu;
            set => pan_top.menu = value;
        }

        //初始化各个窗口
        protected void _initForms() {
            Form_Options.instance.onAppInit();
        }
        private void onObjChanged()
        {
            string json_info = Marshal.PtrToStringAnsi(Native.sketch_get_obj_list());

            // 假设返回的数据是一个包含 JSON 数组的字典结构，其中 "View List" 是一个 JSON 数组
            var viewList = JsonConvert.DeserializeObject<Dictionary<string, JArray>>(json_info);

            if (viewList != null && viewList.ContainsKey("View List"))
            {
                objTreeView.Nodes.Clear();

                TreeNode root = new TreeNode("View List");
                objTreeView.Nodes.Add(root);

                // 调用递归方法填充树形视图
                FillTreeViewWithJson(root, viewList["View List"]);
            }
            objTreeView.ExpandAll();
        }

        private void FillTreeViewWithJson(TreeNode parent, JArray jsonData)
        {
            if (jsonData == null)
            {
                return;
            }

            // 遍历 JArray 中的每个元素
            foreach (var item in jsonData)
            {
                // item 是 JToken 类型，需要根据其类型进一步处理
                if (item is JObject)
                {
                    var itemJson = (JObject)item;

                    // 遍历 JObject 中的键值对
                    foreach (var property in itemJson)
                    {
                        string key = property.Key;
                        TreeNode node = new TreeNode(key);
                        parent.Nodes.Add(node);

                        // 如果值是 JArray 类型，则递归调用
                        var child = property.Value;
                        if (child is JArray)
                        {
                            FillTreeViewWithJson(node, (JArray)child);
                        }
                    }
                }
            }
        }

        //处理Core层发出的UI命令
        public DObject onCommand(string cmd, DObject paras) {
            switch (cmd) {
                case "on_doc_new": {
                    var projPath = Native.documentPath();
                    if (String.IsNullOrEmpty(projPath)) { //something wrong
                        pan_top.title = "D5 Modeling";
                    }
                    else {
                        pan_top.title = "D5 Modeling - " + projPath;
                    }

                    this.onObjChanged();
                }
                break;
                case "on_doc_loaded": {
                    var projPath = Native.documentPath();

                    if (String.IsNullOrEmpty(projPath)) { //something wrong
                        pan_top.title = "D5 Modeling";
                    }
                    else {
                        pan_top.title = "D5 Modeling - " + projPath + (Native.isDocumentReadOnly() ? "[只读]" : "");
                    }

                    this.onObjChanged();
                }
                break;
                case "set_input_label": {
                        if (paras.ContainsKey("color"))
                        {
                            Vec3 color = paras.GetVec3("color");
                            color = color * 255.0;
                            inputTextBox.BackColor = Color.FromArgb(255, (int)color.X, (int)color.Y, (int)color.Z);
                        }
                        if (paras.ContainsKey("value"))
                        {
                            string value = paras.GetString("value");
                            inputTextBox.Text = value;
                        }
                        if (paras.ContainsKey("name"))
                        {
                            string name = paras.GetString("name");
                            inputLabel.Text = name;
                        }
                } break;
                case "show_right_menu":
                {
                    ContextMenuStrip contextMenu = new ContextMenuStrip();

                    if (paras.ContainsKey("close_group") && paras.GetBool("close_group"))
                    {
                        ToolStripMenuItem closeGroupItem = new ToolStripMenuItem("关闭组");
                        closeGroupItem.Click += (sender, e) =>
                        {
                            Native.sketch_close_group();
                        };
                        contextMenu.Items.Add(closeGroupItem);
                    }

                    if (paras.ContainsKey("create_group") && paras.GetBool("create_group"))
                    {
                        ToolStripMenuItem createGroupItem = new ToolStripMenuItem("创建群组");
                        createGroupItem.Click += (sender, e) =>
                        {
                            Native.sketch_create_group();
                        };
                        contextMenu.Items.Add(createGroupItem);
                    }

                    if (paras.ContainsKey("disperse_group") && paras.GetBool("disperse_group"))
                    {
                        ToolStripMenuItem disperseGroupItem = new ToolStripMenuItem("炸开模型");
                        disperseGroupItem.Click += (sender, e) =>
                        {
                            Native.sketch_disperse_group();
                        };
                        contextMenu.Items.Add(disperseGroupItem);
                    }

                    if(paras.ContainsKey("weld_line") && paras.GetBool("weld_line"))
                    {
                        ToolStripMenuItem weldLines = new ToolStripMenuItem("焊接线");
                        weldLines.Click += (sender, e) =>
                        {
                            Native.sketch_weld_lines();
                        };
                        contextMenu.Items.Add(weldLines);
                    }

                    if (paras.ContainsKey("disperse_weld_line") && paras.GetBool("disperse_weld_line"))
                    {
                        ToolStripMenuItem disperseWeldLines = new ToolStripMenuItem("分解线");
                        disperseWeldLines.Click += (sender, e) =>
                        {
                            Native.sketch_disperse_weld_line();
                        };
                        contextMenu.Items.Add(disperseWeldLines);
                    }

                    if (paras.ContainsKey("soft_line") && paras.GetBool("soft_line"))
                    {
                        ToolStripMenuItem softLines = new ToolStripMenuItem("柔化边线");
                        softLines.Click += (sender, e) =>
                        {
                            Native.sketch_soft_lines();
                        };
                        contextMenu.Items.Add(softLines);
                    }

                    if (paras.ContainsKey("disperse_smooth_group") && paras.GetBool("disperse_smooth_group"))
                    {
                        ToolStripMenuItem disperseSmoothGroup = new ToolStripMenuItem("取消柔化");
                        disperseSmoothGroup.Click += (sender, e) =>
                        {
                            Native.sketch_disperse_smooth_group();
                        };
                        contextMenu.Items.Add(disperseSmoothGroup);
                    }

                    if (paras.ContainsKey("convert_to_geodata") && paras.GetBool("convert_to_geodata"))
                    {
                        ToolStripMenuItem convertToGeoData = new ToolStripMenuItem("离散化Nurbs对象");
                        convertToGeoData.Click += (sender, e) =>
                        {
                            Native.sketch_convert_to_geodata();
                        };
                        contextMenu.Items.Add(convertToGeoData);
                    }

                    Point cursorPos = Cursor.Position;
                    contextMenu.Show(cursorPos);
                }
                break;
                case "func_unit_click": {
                    FuncUnit.invokeClick(
                            paras.GetString("func_unit_name"), null, null, Keys.None, paras);
                } break;
                case "controller_update": 
                case "sim_input": //模拟键盘输入事件
                {
                    if (paras.ContainsKey("color"))
                    {
                        Vec3 color = paras.GetVec3("color");
                        color = color * 255.0;
                        inputTextBox.BackColor = Color.FromArgb(255, (int)color.X, (int)color.Y, (int)color.Z);
                    }
                    if (paras.ContainsKey("value"))
                    {
                        string value = paras.GetString("value");
                        inputTextBox.Text = value;
                    }
                    if (paras.ContainsKey("current_param"))
                    {
                        string name = paras.GetString("current_param");
                        inputLabel.Text = name;
                    }
                } break;
                case "controller_deactive":
                {
                    //inputTextBox.BackColor = Color.White;
                    inputTextBox.Text = "";
                    inputLabel.Text = "type";
                } break;
                case "obj_list_changed":
                {
                    this.onObjChanged();
                }
                break;
            }

            return null;
        }
        protected override void OnFormClosing(FormClosingEventArgs e) {
            base.OnFormClosing(e);

            if (e.CloseReason == CloseReason.UserClosing
                && !m_disposing) {
                e.Cancel = true;
                FuncUnit.get("file_exit").onClick(this, e, Keys.None, null);
            }
        }

        protected override void OnFormClosed(FormClosedEventArgs e) {
            base.OnFormClosed(e);

            IFWrapper.Native.quitApp();
        }

        protected override void OnSizeChanged(EventArgs e) {
            base.OnSizeChanged(e);
        
            if (this.WindowState == FormWindowState.Maximized) {
                var work = Screen.FromHandle(this.Handle).WorkingArea;
                var border = Win32.formBorderSize;
                work.Inflate(border);
                if (this.Width > work.Width || this.Height > work.Height) { //somthing wrong, fix it
                    Win32.MoveWindow(this.Handle, work.X, work.Y, work.Width, work.Height, true);
                }
            }
        }
    }
}

