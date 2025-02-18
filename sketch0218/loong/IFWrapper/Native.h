#pragma once

#include <Windows.h>
#include <string>

#include "sketch/Define.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace System::Drawing;

namespace Engine
{
	class DObject;
}

namespace IFWrapper
{
	public ref class Native
	{
	public:
		///////////////////////////// IF_CLRUtils  /////////////////////////////
		static array<unsigned char>^ toBinary(LoongCAD::Util::DObject^ dobj);
		static LoongCAD::Util::DObject^ fromBinary(array<unsigned char>^ buf);

		/////////////////////////////  IF_TK  /////////////////////////////
#ifdef _DEBUG
		//[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		//static bool cvtTKProfiles(String^ dstFile, String^ srcFile);

		//[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		//static bool cvtTKMtrls(String^ dstFile, String^ srcFile);

		//[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		//static bool cvtTKBolts(String^ dstFile, String^ srcFile);
#endif

		/////////////////////  IF_App  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void disposeApp();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void quitApp();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void appRender();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void appUpdate();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void appUpdateAndRender();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void onCanvasResize(int w, int h);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void showModalDialog(IntPtr hwnd);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long STCoreFrame();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static IntPtr mainHandle();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static IntPtr sketch_get_obj_list();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_deactive_current_controller();
        
		static void sketch_active_controller(String^ name);
        
		static void sketch_enter(String^ name);

		static void sketch_escape(String^ name);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_copy_select_to_clipbord();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_paste_clipbord();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_clear_clipbord();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_delete_select();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_close_group();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_create_group();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_disperse_group();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_weld_lines();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_disperse_weld_line();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_soft_lines();
        
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_disperse_smooth_group();

        [DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_convert_to_geodata();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_undo_focused_view();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void sketch_redo_focused_view();

		static System::Drawing::Rectangle screenWorkAreaRelativeToPrimary(IntPtr ptr);

		static void setProgress(String^ progName, float newProgress);

		/////////////////////  IF_Document  //////////////////////////
		static void queryValidDocumentEnv([Out] array<String^>^% envs);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool hasPath(String^ path);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static int testDocumentLockFile(String^ projPath);

		static String^ documentName();
		static String^ documentPath();
		static LoongCAD::Util::DObject^ documentProjProps();
		static bool modifyDocument(LoongCAD::Util::DObject^ props);
		static bool getDocumentBaseInfosFromBasePath(String^ basePath,
		                                             [Out] array<String^>^% paths,
		                                             [Out] array<long long>^% modifyTms,
		                                             [Out] array<String^>^% designers,
		                                             [Out] array<String^>^% comments);
		static bool runCommonScript(String^ script, bool debug, [Out] String^% result, [Out] String^% errMsg);

		static Object^ evalJSExpr(String^ script);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isPathHasDocument(String^ path);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isDocumentHasNewerBackup(String^ path);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isDocumentModifiedAfterLastSave();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isDocumentReadOnly();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool newDocument(String^ projPath, String^ env);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void openDocument(String^ projPath, String^ backup, bool readOnly, String^ progress);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::DocumentSaveResult saveDocument(bool async, String^ progress);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::DocumentSaveResult saveAsDocument(String^ projPath, bool async, String^ progress);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::DocumentSaveResult saveTempDocument(String^ progress);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool packageDocument();

		/////////////////////  IF_ViewManager  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::ViewManagerMode viewMgrMode();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setViewMgrMode(LoongCAD::STC::Define::ViewManagerMode mode);

		/////////////////////  IF_UILayout  //////////////////////////
		static LoongCAD::Util::DObject^ queryUILayout(String^ name);
		static LoongCAD::Util::DObject^ queryUIBar(String^ name);
		static LoongCAD::Util::DObject^ queryUIMiniBar(String^ name);
		static LoongCAD::Util::DObject^ queryUIMiniBars();
		static bool provisionBar(String^ name, bool builtIn, String^ tip, bool provisioned, String^ location, int x,
		                         int y, array<String^>^ funcUnits);
		static bool provisionLayout(String^ name, String^ tip, LoongCAD::Util::DObject^ subNodes);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool saveUIUserLayouts();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool saveUIUserBars();

		/////////////////////  IF_Settings  //////////////////////////
		static LoongCAD::Util::DObject^ settings();
		static System::Object^ settings(String^ pname);
		static void setSettings(LoongCAD::Util::DObject^ props);
		static void setSettings(String^ pname, bool pval);

		/////////////////////  IF_Options  //////////////////////////
		static LoongCAD::Util::DObject^ options();
		static System::Object^ options(String^ pname);
		//static LoongCAD::Util::DObject^ compPrefs();
		static void setOptions(LoongCAD::Util::DObject^ props);
		static void setOptions(String^ pname, bool pval);

		/////////////////////  IF_ShortCuts  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void initShortCuts();

		static bool queryShortCuts([Out] array<String^>^% fus, [Out] array<unsigned int>^% scs);
		static bool queryBuiltInShortCuts([Out] array<String^>^% fus, [Out] array<unsigned int>^% scs);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool initShortCut(String^ fu, LoongCAD::STC::Define::LayoutCellType cellType, int vk, bool ctrl,
		                         bool alt, bool shift);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setShortCut(String^ fu, LoongCAD::STC::Define::LayoutCellType cellType, int vk, bool ctrl, bool alt,
		                        bool shift);

		static bool setShortCuts(array<String^>^ fus, array<unsigned int>^ scs);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool restoreShortCuts();

		static bool importShortCuts(String^ path, [Out] array<String^>^% fus, [Out] array<unsigned int>^% scs);
		static bool exportShortCuts(String^ path, array<String^>^ fus, array<unsigned int>^ scs);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isValidShortCutKey(int vk);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isBuiltInShortCut(unsigned int sc);


		/////////////////////  IF_Layout  //////////////////////////

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long focusedCellId();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::LayoutCellType focusedCellType();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isFocusedCellsAllTopMost();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setFocusedCellTopMost(bool topMost);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void switchCellFullScr(long long id);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setFocusedCell(long long id);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void resetFocusedCellPos();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool removeCellFromLayout(long long id);

		static array<long long>^ removeCellsFromLayout(array<long long>^ ids);

		/////////////////////  IF_Revertable  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void undoFocusedCell();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redoFocusedCell();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void undoPool(long long poolId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redoPool(long long poolId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void undoScene(long long sceneId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redoScene(long long sceneId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void undoDrawing(long long drId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redoDrawing(long long drId);

		/////////////////////  IF_Controller  //////////////////////////
		static String^ controller(int slot);
		static bool activateController(String^ name);
		static bool activateController(String^ name, String^ key0, String^ val0);
		static bool activateController(String^ name, String^ key0, System::Guid val0);
		static bool activateController(String^ name, String^ key0, int val0);
		static bool activateController(String^ name, String^ key0, short val0);
		static bool activateController(String^ name, String^ key0, char val0);
		static bool activateController(String^ name, String^ key0, bool val0);
		static bool activateController(String^ name, LoongCAD::Util::DObject^ paras);
		static bool modifyController(String^ name, LoongCAD::Util::DObject^ paras);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void deactivateController(String^ name);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void popController();

		/////////////////////  IF_Selector  //////////////////////////

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isFocusedCellSnappingPoint();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::PickMode pickMode(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setPickMode(LoongCAD::STC::Define::LayoutCellType cellType, LoongCAD::STC::Define::PickMode mode,
		                        bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static unsigned int pickFlag(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setPickFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flags, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool updatePickFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flags, bool enable,
		                           bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool togglePickAll(LoongCAD::STC::Define::LayoutCellType cellType, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool togglePickPart(LoongCAD::STC::Define::LayoutCellType cellType, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::SnapMode snapMode(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setSnapMode(LoongCAD::STC::Define::LayoutCellType cellType, LoongCAD::STC::Define::SnapMode mode,
		                        bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static unsigned int ptSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setPtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flags, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool updatePtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flags, bool enable,
		                             bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static unsigned int plSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setPlSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flags, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::SnapRefPlane snapRefPlane(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setSnapRefPlane(LoongCAD::STC::Define::LayoutCellType cellType,
		                            LoongCAD::STC::Define::SnapRefPlane plane, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static unsigned int tmpPtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static unsigned int tmpUsrPtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setTmpPtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flag, bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setTmpUsrPtSnapFlag(LoongCAD::STC::Define::LayoutCellType cellType, unsigned int flag);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool revertSnap(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool completeSnap(LoongCAD::STC::Define::LayoutCellType cellType);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool toggleSnapLock(LoongCAD::STC::Define::LayoutCellType cellType,
		                           LoongCAD::STC::Define::SpaceAxis axis);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool snapPointByInput(LoongCAD::STC::Define::LayoutCellType cellType, String^ input, unsigned int flag,
		                             bool invokeUI);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setPickObjFilterCfg(String^ cfgName);

		// static String^ getPickObjFilterCfg();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool clearFocusedViewHover();

		/////////////////////  IF_View  //////////////////////////
		static bool getAllViewIdAndNames([Out] array<long long>^% ids, [Out] array<String^>^% names, bool incTemp);
		static bool getActiveViewIdAndNames([Out] array<long long>^% ids, [Out] array<String^>^% names, bool incTemp);
		static array<long long>^ focusedViewIds();
		static LoongCAD::Util::DObject^ focusedViewProps();
		static LoongCAD::Util::DObject^ focusedViewDisplaySettingProps();
		static LoongCAD::Util::DObject^ viewProps(long long id);
		static bool modifyView(long long id, LoongCAD::Util::DObject^ props);
		static bool modifyViews(array<long long>^ ids, LoongCAD::Util::DObject^ props);
		static bool modifyFocusedViews(LoongCAD::Util::DObject^ props);
		static bool modifyDisplaySettingInFocusedViews(LoongCAD::Util::DObject^ props);
		static bool modifyDisplayFilterInFocusedViews(LoongCAD::Util::DObject^ props);
		//static array<String^>^ objectFilterPropList(LoongCAD::STC::Define::ObjectFilterType type);
		//static array<long long>^ createModelViewsByCurGrid(LoongCAD::STC::Define::ViewNoRange modeX, String^ prefixX, String^ propsX,
		//                                                    LoongCAD::STC::Define::ViewNoRange modeY, String^ prefixY, String^ propsY,
		//                                                    LoongCAD::STC::Define::ViewNoRange modeZ, String^ prefixZ, String^ propsZ);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool modifyPartRenderModeInFocusedViews(LoongCAD::STC::Define::ViewRenderMode rdMode);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool modifyPartRenderModeCompInFocusedViews(LoongCAD::STC::Define::ViewRenderMode rdMode);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool modifyPointSizeInAllViews(double ptSize);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isViewNameValid(String^ name, long long id);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isTempView(long long id);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long focusedViewId();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void deleteViews(array<long long>^ ids, int nIds);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createView3DForCurObjs(String^ viewName, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createViewOrthoForCurObjs(String^ viewName, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createObjView3D(long long objId, String^ viewName, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createObjViewOrtho(long long objId, String^ viewName, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createObjViewFromModelSide(long long objId, String^ viewName,
		                                            LoongCAD::STC::Define::ObjectSide objSide, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createObjViewFromObjSide(long long objId, String^ viewName,
		                                          LoongCAD::STC::Define::ObjectSide objSide, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createModelViewFromObjSide(long long objId, String^ viewName,
		                                            LoongCAD::STC::Define::ObjectSide objSide, bool temp);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void focusCurObjsInFocusedView(LoongCAD::STC::Define::ViewFocusMode focusMode);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void hideCurObjsInFocusedView();

		static void hideObjsInFocusedView(array<long long>^ ids);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void resetFocusedViewCamPose();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void switchFocusedViewType();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redrawFocusedViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void redrawActiveViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void dirtyFocusedViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void dirtyActiveViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void moveCameraInFocusedCell(LoongCAD::STC::Define::SpaceAxis axis, int pixel);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void centerCameraInFocusedCell();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void rotateCameraInFocusedCell(double anglH, double anglV);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void scaleCameraInFocusedCell(double ratio);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void scaleViewByWorkBox();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void scaleViewByCurObjs();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setWorkAreaToModelInFocusedViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setWorkAreaToVisibleAreaInFocusedViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setWorkAreaToCurObjsInFocusedViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setWorkAreaToCurObjsInActiveViews();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createModelViewBase(LoongCAD::STC::Define::SpacePlane plane, double height);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long createModelViewByFocusedWorkPlane();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void addActiveViews(array<long long>^ ids, int nIds, bool addLayout);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void removeActiveViews(array<long long>^ ids, int nIds, bool removeLayout);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isFocusedViewWorkBoxVis();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setFocusedViewWorkBoxVis(bool b);

		/////////////////////  IF_Scene  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long focusedScene();

		static array<long long>^ curObjIds(long long sceneId);
		static array<long long>^ curObjIds();
		static array<long long>^ curObjIds(LoongCAD::STC::Define::ObjectType objType);
		static array<long long>^ curObjIds(LoongCAD::STC::Define::ObjectType objType0,
		                                   LoongCAD::STC::Define::ObjectType objType1);
		static array<long long>^ curObjIdsByBaseType(LoongCAD::STC::Define::ObjectType typeBegin,
		                                             LoongCAD::STC::Define::ObjectType typeEnd);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::ObjectType curObjsUnifiedType();

		static LoongCAD::Geometry::Mat43 focusedWorkAxis();
		static LoongCAD::Geometry::Mat43 workAxis(long long sceneId);
		static void setFocusedWorkAxis(LoongCAD::Geometry::Mat43 m);
		static void setWorkAxis(long long sceneId, LoongCAD::Geometry::Mat43 m);
		//static bool queryCurObjBaseInfos([Out] array<long long>^% ids, [Out] array<LoongCAD::STC::Define::ObjectType>^% types, [Out] array<bool>^% cutters);
		//static bool queryCurObjBaseInfos([Out] array<long long>^% ids, [Out] array<LoongCAD::STC::Define::ObjectType>^% types);
		//static bool queryNoableBaseInfos(
		//    [Out] array<long long>^% objIds, [Out] array<LoongCAD::STC::Define::ObjectType>^% objTypes,
		//    [Out] array<String^>^% noList, [Out] array<long long>^% assIds);
		//static bool queryCADRefBaseInfos([Out] array<long long>^% objIds, [Out] array<String^>^% refPaths, [Out] array<bool>^% visibles);

		//static String^ querySelectGroupInfo(array<long long>^ objIds);

		static bool setCurObjs(array<long long>^ objIds);
		//static bool setCurObjsByPhaseIds(array<int>^ phaseIds);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool setCurObjsToFocusedViewVisObjs();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool deleteCurObjs();

		static bool deleteObjs(array<long long>^ objIds);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool undoCurObjs();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool redoCurObjs();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setSceneCur(long long id);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static void setSceneCurs(array<long long>^ ids, int nIds);

		//[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		//static bool isCurSPPartsRdModeAllDetail();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool isCurCuttersAllPrepare();

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool modifyCurCuttersPrepare(bool bPrepare);

		/////////////////////  IF_SceneObject  //////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long objModifyCounter(long long sceneId, long long objId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool objDisposed(long long sceneId, long long objId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static long long objScene(long long objId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::ObjectType objType(long long sceneId, long long objId);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static uint16_t objFlags(long long sceneId, long long objId);

		static System::Guid objGuid(long long sceneId, long long objId);
		//static array<long long>^ objCtrlPointIds(long long sceneId, long long objId);
		//static array<long long>^ objChildIds(long long sceneId, long long objId);
		static LoongCAD::Geometry::Mat43 objMatrixLocal(long long sceneId, long long objId);
		static LoongCAD::Geometry::Mat43 objMatrixObj(long long sceneId, long long objId);
		static LoongCAD::Geometry::Mat43 objMatrixObjW(long long sceneId, long long objId);
		static LoongCAD::Geometry::Mat43 objMatrixWorld(long long sceneId, long long objId);
		static LoongCAD::Geometry::Mat43 objMatrixParent(long long sceneId, long long objId);

		static LoongCAD::Util::DObject^ objViewProps(long long objId);
		static LoongCAD::Util::DObject^ objProps(long long objId);
		//static LoongCAD::Util::DObject^ objPropsWithCtrlPoints(long long objId);
		//static LoongCAD::Util::DObject^ objWorkPropsWithCtrlPoints(long long objId, LoongCAD::Geometry::Mat43 workAxis);
		static LoongCAD::Util::DObject^ curObjViewProps();
		static LoongCAD::Util::DObject^ curObjProps();
		//static array<LoongCAD::Geometry::Vec3>^ objCtrlPointsW(long long objId);
		static bool setObjectGUID(long long objId, System::Guid guid);
		static bool setCurObjectGUID(System::Guid guid);
		static bool modifyObject(long long objId, LoongCAD::Util::DObject^ props);
		static bool modifyObjects(array<long long>^ objIds, LoongCAD::Util::DObject^ props);
		static bool modifyCurObjects(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objType);
		static bool modifyCurObjects(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objType0,
		                             LoongCAD::STC::Define::ObjectType objType1);
		//static bool modifyCurObjectsByBaseType(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objTypeBegin, LoongCAD::STC::Define::ObjectType objTypeEnd);
		//static bool modifyCurSPTweenBeams(LoongCAD::Util::DObject^ props, [Out] long long% idL, [Out] long long% idR);
		//static LoongCAD::Util::DObject^ queryCurTweenViewProps();

		static long long createObject(long long sceneId, LoongCAD::STC::Define::ObjectType objType,
		                              LoongCAD::Util::DObject^ props);

		static bool modifyObjHandler(long long objId, long long hid, LoongCAD::Util::DObject^ props);

		/////////////////////  IF_History  //////////////////////////
		static String^ projBaseDir();
		static LoongCAD::Util::DObject^ globalHistory(String^ name);
		static System::Object^ globalHistory(String^ name, String^ pname);
		static LoongCAD::Util::DObject^ history(String^ name);
		static System::Object^ history(String^ name, String^ pname);
		static bool saveGlobalHistory(String^ name, LoongCAD::Util::DObject^ props);
		static bool saveHistory(String^ name, LoongCAD::Util::DObject^ props);

		/////////////////////  IF_Common_Cfg  //////////////////////////
		static LoongCAD::Util::DObject^ getCommonCfg(String^ type, String^ name);
		static void setCommonCfg(String^ type, String^ name, LoongCAD::Util::DObject^ cfg, bool save, bool invokeUI);
		static array<String^>^ getCommonCfgsByType(String^ type, bool sys);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool deleteCommonCfg(String^ type, String^ name, bool save);

		static LoongCAD::Util::DObject^ getGlobalCommonCfg(String^ type, String^ name);
		static void setGlobalCommonCfg(String^ type, String^ name, LoongCAD::Util::DObject^ cfg, bool save,
		                               bool invokeUI);
		static array<String^>^ getGlobalCommonCfgsByType(String^ type, bool sys);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static bool deleteGlobalCommonCfg(String^ type, String^ name, bool save);

		//////////////////////////////  IF_Localization  ////////////////////////////
		static LoongCAD::Util::DObject^ getLanguageConfig(String^ lang);

		///////////////////////////////  IF_Edit  ///////////////////////////////
		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::MoveResult moveFocusedViewsPlaneByLineStrideInWorkSpace(
			double dx, double dy, double dz);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::MoveResult moveFocusedViewsPlaneByRotateInWorkSpace(
			double x0, double y0, double z0, LoongCAD::STC::Define::SpaceAxis axis, double angl);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::MoveResult moveCurObjsByLineStrideInWorkSpace(double dx, double dy, double dz);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::MoveResult moveCurObjsByRotateInWorkSpace(
			double x0, double y0, double z0, LoongCAD::STC::Define::SpaceAxis axis, double angl);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::MoveResult moveCurObjsByMirrorInWorkSpace(double x0, double y0, double angl);

		static LoongCAD::STC::Define::CopyResult copyCurObjsByLineStrideInWorkSpace(
			array<double>^ dx, array<double>^ dy, array<double>^ dz, int count);

		static LoongCAD::STC::Define::CopyResult copyCurObjsByRotateInWorkSpace(
			double x0, double y0, double z0, LoongCAD::STC::Define::SpaceAxis axis, array<double>^ angls, double zoff,
			int count);

		[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		static LoongCAD::STC::Define::CopyResult copyCurObjsByMirrorInWorkSpace(double x0, double y0, double angl);

		/////////////////////////////  IF_Profile  /////////////////////////////
		//static String^ profileTipImageFile(String^ type, String^ subType, float dpi);
		//static String^ profileKey(String^ name);
		//static String^ profileType(String^ name);
		//static double getProfileShapeX(String^ name);
		//static double getProfileShapeY(String^ name);
		//static double getProfileShapeWidth(String^ name);
		//static double getProfileShapeHeight(String^ name);
		//static double getProfileShapeTop(String^ name);
		//static double getProfileShapeRight(String^ name);
		//static LoongCAD::Util::DObject^ profileInitProps(String^ type);
		//static array<String^>^ queryProfileSubtypes(String^ type);
		//static array<String^>^ queryProfilePropsLayout(String^ type, String^ subType);
		//static LoongCAD::Util::DObject^ queryProfileProps(String^ name);
		//static List<String^>^ queryProfileUnitData(String^ name);
		//static String^ profileNameData2Disp(String^ name);
		//static String^ profileNameDisp2Data(String^ name);
		//static array<String^>^ queryProfilesBaseInfo();
		//static LoongCAD::Util::DObject^ queryProfileRules();
		//static bool delUserProfiles(array<String^>^ names);
		//static bool saveUserProfilesAndRules(LoongCAD::Util::DObject^ rules, array<LoongCAD::Util::DObject^>^ profiles, bool updateObjs);

		//[DllImport("sketch.dll", CharSet = CharSet::Unicode)]
		//static bool isProfileNameValid(String^ name);
	};
}
