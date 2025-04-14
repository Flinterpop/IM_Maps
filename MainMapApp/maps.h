#pragma once

#include "tilemanager.h"
#include "Mediator.h"
//#include "SimAircraft.h"
#include <ImGuiAPP.h>



class Maps : public ImGuiApp
{
	TileManager mngr;

	#pragma region Region_1
		// items that are saved between runs in SQL Lite DB
		bool mb_tileOutlines = true;
		bool mb_tileCoords = false;
		bool mb_ShowBigX = true;
		bool mb_showLLatMousePointer = false;
		bool mb_map_debug = true;
		bool mb_showTMStats = false;
		bool mb_showAbout = false;
		bool mb_showHelp = false;
		bool mb_showLog = false;
		bool mb_showDemoWindow = false;
		bool mb_showADSBAircraftTable = true;
		bool mb_ShowRouteList = false;
		bool mb_showOptions = false;
		bool mb_ShowPOIList = false;
		bool mb_ShowShapeFileList = false;
		bool gb_NoMap = true;
		bool mb_showCOT = false;
		bool mb_showFMV = false;
		bool mb_ShowRouterBuilder = false;
		int m_Zoom = 3;
		ImVec2 m_VP_CentreLatLng;
		bool AutoLoadAircraft = false;

		bool mShowGeoToolDialog = false;


	#pragma endregion Region_1

	char m_MGRS[25]{};
	bool m_hasRadars = false;

	char* m_dbName = "MultiMaps_APP_Ini.db";

	//m_ means member of class
	static ImVec2 m_VP_NormalCentre; //centre of viewport in normal coords
	static double m_WorldSideLengthInPixels;
	static ImVec2 m_canvas_Centre; //in pixels
	ImVec2 m_canvas_sz;  //in pixels

	ImVec2 m_WinSize;  //this is for the WinOS main window not an ImGui construct
	ImVec2 m_WinPos;   //this is for the WinOS main window not an ImGui construct

	//locations used by 5 red buttons
	char names[5][20];
	double lats[5];
	double lngs[5];
	int zooms[5];



	bool mb_isMouseOverMap = false;
	bool mb_MapIsDirty = false;
	bool mb_LeftClickMenuOpen = false;

	ImU32 g_MapBackgroundColour = IM_COL32(255, 255, 255, 255);

	bool b_CentreOnAircraft = false;
	//SimAircraft* m_AircraftToCentreOn = NULL;


	ImVec2(*FuncPtr) (double, double) {};

	ImVec2Double m_LeftClickLocationLL;

	static ImVec2 LatLngToVPxy(double lat, double lng);
	ImVec2 LatLngToTexture(double lat, double lng);

	ImVec2Double VPxyRoLatLng(float VPx, float VPy);

	void DoCentreOnLatLng(double lat, double lng);
	void DoLoadAllModulesStateFromApp_Ini();
	void DoLoadMapStateFromApp_Ini();
	void DoSaveAllModuleStateToApp_Ini();
	void DoSaveMapStateFromApp_Ini();
	void ShowAllMenuBars();
	void DrawScaleBar();
	void DrawToolBar();
	void CheckKeysPressed();
	void DrawMGRSMousePointer(ImGuiIO& io);

	void DrawTimeAndPosDisplay();
	void DrawToast();

	void LoadACImages();
	void ShowAppOptions(bool* popen);

	int NotifyMe(enum Mediator::NOTIFY_MSG m, void* data);

	void ShowGeoToolDialog(bool* pOpen);

	void ManageAndDrawMap();
		
	public: 
		void InitializeApp() override;
		void ShutDownApp() override;
		void UpdateApp() override;

};



