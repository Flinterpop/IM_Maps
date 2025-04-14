
#include <functional>

#include "maps.h"
#include "AppIni.h"

#include "hasMain.h"
//#include "ADSB.h"

//#include "RadarControl.h"
//#include "aircraftControl.h"
#include "MouseManager.h"

#include "MapsToast.h"
#include "AppLogger.h"
#include "POI.h"
#include "shapefilemodule.h"
#include "socketstuff.h"
//#include "JREAP.h"


//void DoLoadAircraftControlStateFromAppIni();
//void DoSaveAircraftControlStateToAppIni();
void LoadAllRoutes();
void LoadPOIListFromDB();


extern Mediator _m;
extern struct _MouseManager g_MouseMngr;
extern AppLog MyLog;

extern bool gb_RBL_NM;
extern int g_MGRS_Precison;

extern bool glob_bInTest;
extern bool glob_bDebug;

extern float glob_LocateLineWidth;
extern ImU32 g_LocateLineColour;


int Maps::NotifyMe(enum Mediator::NOTIFY_MSG m, void* data)
{
    if (m == Mediator::NOTIFY_MSG::MED_HAVE_LEFT_DOUBLE_CLICK_LOCATION)
    {
        printf("Maps Rx Notify %d\r\n", m);
        ImVec2Double* d = (ImVec2Double*)data;
        m_LeftClickLocationLL.x = d->x;
        m_LeftClickLocationLL.y = d->y;
        mb_LeftClickMenuOpen = true;
    }

    if (m == Mediator::NOTIFY_MSG::MED_FOLLOW_TRACK)
    {
        //this is still redundant. will become useful later
        if (NULL == data) b_CentreOnAircraft = false;
        else
        {
            //m_AircraftToCentreOn = (SimAircraft*)data;
            //printf("Maps Rx notification to follow track: %d\r\n", m_AircraftToCentreOn->TN);
            b_CentreOnAircraft = true;
        }
    }

    return 0;
}




void Maps::DoLoadAllModulesStateFromApp_Ini()
{
    DoLoadMapStateFromApp_Ini();
    //DoLoadADSBState();
    //DoLoadRadarControlState();
    //DoLoadAircraftControlStateFromAppIni();
    //DoLoadL16StateFromAppIni();

    LoadShapeFiles();
}

void Maps::DoSaveAllModuleStateToApp_Ini()
{
    DoSaveMapStateFromApp_Ini();
    //DoSaveADSBState();
    //DoSaveRadarControlState();
    //DoSaveAircraftControlStateToAppIni();
    //DoSaveL16StateFromAppIni();
}


void Maps::DoLoadMapStateFromApp_Ini()
{
    int index = GetIniInt("MapIndex", 0);
    mngr.SelectTileServer(index);
    mngr.ClearMapThreadQueue();

    mb_tileOutlines = GetIniBool("tileOutlines", false);
    mb_tileCoords = GetIniBool("tileCoords", false);
    mb_ShowBigX = GetIniBool("bigX", false);
    mb_map_debug = GetIniBool("map_debug", false);

    mb_showLLatMousePointer = GetIniBool("showMouse", false);
    mb_showTMStats = GetIniBool("showTMStats", false);
    mb_showAbout = GetIniBool("showAbout", false);
    mb_showHelp = GetIniBool("showHelp", false);
    mb_showLog = GetIniBool("showLog", false);
    mb_showDemoWindow = GetIniBool("showDemoWindow", false);
    mb_showADSBAircraftTable = GetIniBool("showADSBAircraftTable", false);
    AutoLoadAircraft = GetIniBool("AutoLoadAircraft", false);
    mb_ShowRouterBuilder = GetIniBool("mb_ShowRouterBuilder", false);
    mb_ShowRouteList = GetIniBool("mb_ShowRoutList", false);

    mb_showOptions = GetIniBool("mb_showOptions", false);
    gb_RBL_NM = GetIniBool("gb_RBL_NM", false);
    g_MGRS_Precison = GetIniInt("g_MGRS_Precison", 5);

    mb_ShowPOIList = GetIniBool("mb_ShowPOIList", false);

    mShowGeoToolDialog = GetIniBool("mShowGeoToolDialog", false);
     m_Zoom = GetIniInt("Zoom", 7);

    double _lat = GetIniReal("VP_Lat", 4.1234);
    double _lng = GetIniReal("VP_Lng", -4.1234);
    //printf("Read in %7.4f  %8.4f\r\n", _lat, _lng);
    DoCentreOnLatLng(_lat, _lng);

    glob_LocateLineWidth = (float)GetIniReal("glob_LocateLineWidth", 4.1234);

    g_LocateLineColour = GetIniInt("g_LocateLineColour", 0x6767ABFF);   //1734847487
    g_MapBackgroundColour = GetIniInt("g_MapBackgroundColour", 0xffffffff);
    mb_ShowShapeFileList = GetIniBool("mb_ShowShapeFileList", false);
    gb_NoMap = GetIniBool("gb_NoMap", false);

    char buf[40];
    for (int x = 0;x < 5;x++)
    {
        sprintf(buf, "GoToLat_%d", x);
        lats[x] = GetIniReal(buf, 45.000);

        sprintf(buf, "GoToLng_%d", x);
        lngs[x] = GetIniReal(buf, 45.000);

        sprintf(buf, "GoToZoom_%d", x);
        zooms[x] = GetIniInt(buf, 7);

        sprintf(buf, "GoToName_%d", x);

        std::string s = GetIniString(buf, std::to_string(x));
        strcpy(names[x], s.c_str());

    }

    float WinPos_X = (float)GetIniReal("WinPos_X", 50);
    float WinPos_Y = (float)GetIniReal("WinPos_Y", 50);
    float WinSize_X = (float)GetIniReal("WinSize_X", 100);
    float WinSize_Y = (float)GetIniReal("WinSize_Y", 600);

    SetWindowPos(ImVec2(WinPos_X, WinPos_Y));
    SetWindowSize(ImVec2(WinSize_X, WinSize_Y));
}


void Maps::DoSaveMapStateFromApp_Ini() 
{
    UpdateIniInt("MapIndex", mngr.m_TileServerIndex);

    UpdateIniBool("tileOutlines", mb_tileOutlines);
    UpdateIniBool("tileCoords", mb_tileCoords);
    UpdateIniBool("bigX", mb_ShowBigX);
    UpdateIniBool("map_debug", mb_map_debug);

    UpdateIniBool("showMouse", mb_showLLatMousePointer);
    UpdateIniBool("showTMStats", mb_showTMStats);
    UpdateIniBool("showAbout", mb_showAbout);
    UpdateIniBool("showHelp", mb_showHelp);
    UpdateIniBool("showLog", mb_showLog);
    UpdateIniBool("showDemoWindow", mb_showDemoWindow);
    UpdateIniBool("showADSBAircraftTable", mb_showADSBAircraftTable);
    UpdateIniBool("AutoLoadAircraft", AutoLoadAircraft);

    UpdateIniBool("mb_ShowRouterBuilder", mb_ShowRouterBuilder);
    UpdateIniBool("mb_ShowRoutList", mb_ShowRouteList);

    UpdateIniBool("mb_showOptions", mb_showOptions);
    UpdateIniBool("gb_RBL_NM", gb_RBL_NM);

    UpdateIniBool("mb_ShowPOIList", mb_ShowPOIList);

    UpdateIniBool("mShowGeoToolDialog", mShowGeoToolDialog);

    UpdateIniInt("g_MGRS_Precison", g_MGRS_Precison);

    UpdateIniInt("Zoom", m_Zoom);

    UpdateIniReal("VP_Lat", m_VP_CentreLatLng.x);
    UpdateIniReal("VP_Lng", m_VP_CentreLatLng.y);

    UpdateIniReal("glob_LocateLineWidth", glob_LocateLineWidth);

    UpdateIniInt("g_LocateLineColour", g_LocateLineColour);
    UpdateIniInt("g_MapBackgroundColour", g_MapBackgroundColour);

    UpdateIniBool("mb_ShowShapeFileList", mb_ShowShapeFileList);
    UpdateIniBool("gb_NoMap", gb_NoMap);


    char buf[40];
    for (int x = 0;x < 5;x++)
    {
        sprintf(buf, "GoToLat_%d",x);
        UpdateIniReal(buf, lats[x]);

        sprintf(buf, "GoToLng_%d", x);
        UpdateIniReal(buf, lngs[x]);

        sprintf(buf, "GoToZoom_%d", x);
        UpdateIniInt(buf, zooms[x]);
        
        sprintf(buf, "GoToName_%d", x);
        UpdateIniString(buf, names[x]);
    }


    ImVec2 GWS = GetWindowSize();
    ImVec2 GWP = GetWindowPos();

    UpdateIniReal("WinSize_X", GWS.x);
    UpdateIniReal("WinSize_Y", GWS.y);
    UpdateIniReal("WinPos_X", GWP.x);
    UpdateIniReal("WinPos_Y", GWP.y);
}


void Maps::InitializeApp()
{
    std::srand((unsigned int)std::time({})); //seed the random number generator

    char CWD[200];
    LPTSTR lp = &CWD[0];
    DWORD rv = GetCurrentDirectory(200, lp);
    std::string s = "Current folder: " + std::string(CWD);
    LogToFile(s);

    printf("Current folder: %s\r\n", CWD);
    g_SetToastMessage((char*)CWD, 10);

    FuncPtr = LatLngToVPxy;
    g_MouseMngr.FPLatLngToVPxy = LatLngToVPxy;

    if (glob_bDebug) getAdapterInfo();
    bool retVal = initialise_winsock();  ///add error handling

    mngr.CreateMapTileServerList();

    std::function<int(Mediator::NOTIFY_MSG, void*)> f = std::bind(&Maps::NotifyMe, this, std::placeholders::_1, std::placeholders::_2);
    _m.SubScribeForNotifications(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_DOUBLE_CLICK_LOCATION, f,false);
    _m.SubScribeForNotifications(Mediator::NOTIFY_MSG::MED_FOLLOW_TRACK, f, false);


    //retVal = std::filesystem::exists("RadarSites.db");
    //if (retVal) m_hasRadars = loadRadarSites("RadarSites.db");
    //if (m_hasRadars) StartRadarProcessTimer();

    SetIniDBFileName(m_dbName);
    DoLoadAllModulesStateFromApp_Ini(); //this will run submodule (like RadarControl) restore state as well as maps

    LoadAllRoutes();  //must be done before loading A/C
    LoadPOIListFromDB();
    
    //ClearTrackList();  //reserves space for 100 tracks
    //if (AutoLoadAircraft) LoadAircraftList();
    //StartAircraftSimTimer();
}

void Maps::ShutDownApp()
{
    printf("Shutting down gracefully....\r\n");
    DoSaveAllModuleStateToApp_Ini();

    //if (AutoLoadAircraft) SaveAircraftList();

    //if (m_hasRadars) StopAllRadarSites();
    //ShutdownRadarModule();
    
    //ShutDownADSB();

    closeandclean_winsock();
}




void Maps::DoCentreOnLatLng(double lat, double lng)
{
    m_VP_NormalCentre = LatLngToTexture(lat, lng);

}





static GLuint jet_texture = 0;
static int jet_width = 0;
static int jet_height = 0;

static GLuint csi_texture = 0;
static int csi_width = 0;
static int csi_height = 0;

static bool ImagesLoaded = false;

//Not used yet
void Maps::LoadACImages()
{

    //static int jet_width = 0;
    //static int jet_height = 0;
    //static GLuint jet_texture = 0;

    //static int csi_width = 0;
    //static int csi_height = 0;
    //static GLuint csi_texture = 0;

    //if (ImGui::IsKeyPressed(ImGuiKey_J))
    {
        bool jet = LoadTextureFromFile("airbus4545.png", &jet_texture, &jet_width, &jet_height);
        if (jet == false) printf("Could not load jet image\r\n");
        else
        {
            printf("Loaded airbus4545.png image\r\n");
            ImagesLoaded = true;
        }

        bool csi = LoadMonoTextureFromFile("airbus4545.flat", &csi_texture, &csi_width, &csi_height);
        if (csi == false) printf("Could not load csi image\r\n");
        else
        {
            printf("airbus4545.flat image\r\n");
            ImagesLoaded = true;
        }
    }

}





