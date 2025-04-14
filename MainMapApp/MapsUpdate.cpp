
#include "ImGui.h"
#include "Fonts/font_awesome_5.h"
#include "tilemanager.h"
#include "AppIni.h"
#include "maps.h"
//#include "ADSB.h"
#include "hasMain.h" 
//#include "RadarControl.h"
//#include "aircraftControl.h"
#include "MouseManager.h"
#include "MapsToast.h"
#include "geocalcs.h"
#include "AppLogger.h"
#include "POI.h"
#include "libmgrs/mgrs.h"
#include "ImBGUtil.h"
#include "route.h"
#include <libmgrs/utm.h>

//#include "JREAP.h"

#include "shapeFileModule.h"

#include "DtedFile.hpp"
DtedFile dt;
int DTEDAlpha = 32;
ImColor DTEDBlend = ImColor(255, 255, 255, DTEDAlpha);


bool ClipCheck(int x, int y, int limit);
void ClipStatus();


ImVec2 canvas_pTL;
ImVec2 canvas_pBR;



//protoypes for external functions not covered by header files
void ShowAbout(bool* popen);   //in About.cpp
void ShowHelp(bool* popen);   //in Help.cpp


//extern JREAP _JREAP;
extern AppLog MyLog;
extern bool glob_bInTest;
extern _MouseManager g_MouseMngr;

//extern std::vector<SimAircraft*> m_AircraftList;

extern int g_MGRS_Precison;

extern ImU32 g_LocateLineColour;
//extern int TrackDeclutter;


//static class vars must be instantiated outside of class defintion
ImVec2 Maps::m_VP_NormalCentre; //centre of viewport in normal coords
double Maps::m_WorldSideLengthInPixels;
ImVec2 Maps::m_canvas_Centre;


int DtedRow = 1800;
int DtedCol = 900;

bool CreatingLOSLine = false;
bool LOSLineValid = false;
ImVec2Double LOS_p1;
ImVec2Double LOS_p2;

void CreateLOSLine(ImVec2Double m_LeftClickLocationLL)
{
    LOS_p1 = m_LeftClickLocationLL;
    CreatingLOSLine = true;
}

bool CanvasDemo(bool pOpen)
{
    if (ImGui::Begin("Canvas Demo",&pOpen))
    {
        static int sideLength = 10;
        static int numSquares = 100;
        static int start_i = 10;
        static int start_j = 10;
        ImGui::SliderInt("Col", &start_i, 0, 3600-50);
        ImGui::SliderInt("Row", &start_j, 0, 3600-50);
        // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        if (canvas_sz.x < 150.0f) canvas_sz.x = 150.0f;
        if (canvas_sz.y < 150.0f) canvas_sz.y = 150.0f;

        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        // Draw border and background color
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
        ImGuiIO& io = ImGui::GetIO();
        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool is_hovered = ImGui::IsItemHovered(); // Hovered
        const bool is_active = ImGui::IsItemActive();   // Held
       
        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);

        if (is_active)
        {
            start_i -= io.MouseDelta.x;
            if (start_i < 0) start_i = 0;
            start_j -= io.MouseDelta.y;
            if (start_j < 0) start_j = 0;
        }

        if (is_hovered) //if mouse is over this window then check for Zoom
        {
            if (0 != io.MouseWheel) //mouse wheel has moved so Zoom over and around the current mouse location
            {
                if (io.MouseWheel > 0) numSquares += 25;
                if (io.MouseWheel < 0) numSquares -= 25;
                sideLength = 1000 / numSquares;
                printf("Num Squares Per side: %d Side Length: %d\r\n", numSquares, sideLength);

            }
        }




        

        // Draw grid + all lines in the canvas
        draw_list->PushClipRect(canvas_p0, canvas_p1, true);


        for (int j =0;j< numSquares;j++)
            for (int i = 0;i < numSquares;i++)
            {
                char buf[20];
                int height = dt.columns[start_i+i][start_j+j];
                sprintf(buf, "\r\n     %d", height);
                int cval = dt.DtedImage[start_j + j][start_i + i][0];

                draw_list->AddRectFilled(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(cval,cval,cval, 255));
                draw_list->AddRect(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(255, 255, 255, 32));
                //draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(canvas_p0.x + i* sideLength, canvas_p0.y + j* sideLength), IM_COL32(255, 255, 255, 255), buf);

            }



       /*
        const float GRID_STEP = 64.0f;
        for (float x = 0; x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(255, 255, 255, 255));
        
        for (float y = 0; y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(255, 255, 255, 255));

        int i = 0, j = 0;
        for (float x = 0; x < canvas_sz.x; x += GRID_STEP)
        {
            for (float y = 0; y < canvas_sz.y; y += GRID_STEP)
            {
                char buf[20];
                sprintf(buf, "\r\n     %d", dt.columns[i][j++]);
                draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(canvas_p0.x + x, canvas_p0.y + y), IM_COL32(255, 255, 255, 255), buf);
            }
            ++i;
        }
        */
       
        if (is_hovered)
        {
            ImVec2 p1 = ImVec2(canvas_p0.x + 50, canvas_p0.y + 50);

            char buf[200];
            sprintf(buf, "%4.0f  %4.0f   %4.0f %4.0f", io.MousePos.x, io.MousePos.y, mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
            
            
            DrawBoxedText(buf, p1, IM_COL32_BLACK, IM_COL32_BLACK, IM_COL32_WHITE, 1.0f);
        }

        draw_list->PopClipRect();

        ImGui::End();
    }
    return pOpen;
}

bool ShowElevationProfile()
{
    ImGui::Begin("Elevation", 0);
    struct Funcs
    {
        static float DTED(void*, int j) { return dt.columns[j][DtedRow]; }
    };
    static int display_count = 3600;
    //ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::SliderInt("Row", &DtedRow, 0, 3600);
    ImGui::SliderInt("Sample count", &display_count, 1, 3600);
    float (*func)(void*, int) = Funcs::DTED;
    ImGui::PlotLines("Lines##1", func, NULL, display_count, 0, NULL, 0, 800.0f, ImVec2(1000, 400));

    ImGui::Separator();


    float (*func2)(void*, int) = Funcs::DTED;
    ImGui::PlotLines("Lines##2", func2, NULL, display_count, 0, NULL, 0, 800.0f, ImVec2(1000, 400));


    if (ImGui::Button("Grid"))
    {
        for (int j = 0;j < 10;j++)
        {
            for (int i = 0;i < 10;i++)
                printf("%03d ", dt.columns[j][i]);
                printf("\r\n");
        }

    }





    ImGui::End();

    CanvasDemo(true);
    

    return false;
}

void Maps::UpdateApp()
{
    if (mb_isMouseOverMap) CheckKeysPressed();

    //these 4 lines force the next ImGui::Begin to use the main viewport. 
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    //if (b_CentreOnAircraft) 
    //sprintf(MainWindowsTitle, "Map: %s at Zm %d  DC: %d###MainWindow", mngr.GetCurMapName().c_str(), m_Zoom, TrackDeclutter);
    sprintf(MainWindowsTitle, "Map: %s at Zm %d###MainWindow", mngr.GetCurMapName().c_str(), m_Zoom);

    ImGui::Begin(MainWindowsTitle, 0, mainWinFlags);

        ShowAllMenuBars();

        ManageAndDrawMap();

        #pragma region DrawOverlays
        if (mb_ShowBigX)
        {
            ImGui::GetWindowDrawList()->AddLine(canvas_pTL, canvas_pBR, IM_COL32(128, 0, 0, 255), 1.0f);
            ImGui::GetWindowDrawList()->AddLine(ImVec2(canvas_pBR.x, canvas_pTL.y), ImVec2(canvas_pTL.x, canvas_pBR.y), IM_COL32(128, 0, 0, 255), 1.0f);
        }

        drawShapeFiles(ImGui::GetWindowDrawList(), FuncPtr);

        if (mb_showLLatMousePointer) DrawMGRSMousePointer(ImGui::GetIO());
        
        //DrawRadarSites(ImGui::GetWindowDrawList(), FuncPtr);

        DrawPOIs(ImGui::GetWindowDrawList(), FuncPtr);
        DrawRoutes(ImGui::GetWindowDrawList(), FuncPtr);

        //DrawAllAircraftFromList(ImGui::GetWindowDrawList(), FuncPtr);
        //DrawADSBAircraft(ImGui::GetWindowDrawList(), FuncPtr);


        DrawTimeAndPosDisplay();
        DrawToolBar();
        DrawScaleBar();
        DrawToast();

        #pragma endregion DrawOverlays

    ImGui::End();

    #pragma region ShowOtherWindows
   
        //DrawSelectedAircraftEditDialogs();
        if (mb_showDemoWindow) ImGui::ShowDemoWindow(&mb_showDemoWindow);
        if (mb_showTMStats) mngr.ShowTileManagerStatsWindow(&mb_showTMStats);

        //if (mb_showADSBAircraftTable) ShowADSBAircraftTableDialog(&mb_showADSBAircraftTable);


        if (mb_showLog) MyLog.Draw("App Log", &mb_showLog);
        if (mb_showOptions) ShowAppOptions(&mb_showOptions);
        if (mb_ShowPOIList) ShowPOIListDialog(&mb_ShowPOIList);

        if (mb_ShowShapeFileList) ShowShapeFileListDialog(&mb_ShowShapeFileList);

        if (mb_ShowRouteList) mb_ShowRouteList = ShowRouteListDialog(&mb_ShowRouteList, mb_ShowRouterBuilder, LatLngToVPxy);
        if (mb_ShowRouterBuilder) ShowCreateRouteDialog(LatLngToVPxy, &mb_ShowRouterBuilder);

        if (mb_showAbout) ShowAbout(&mb_showAbout);
        if (mb_showHelp) ShowHelp(&mb_showHelp);
        if (mShowGeoToolDialog) ShowGeoToolDialog(&mShowGeoToolDialog);

        ShowElevationProfile();


    #pragma endregion OtherWindows


    if (mb_MapIsDirty) InvalidateShapeFiles();
    mb_MapIsDirty = false; //map becomes dirty if it is resized, zoomed or panned
}


void Maps::ManageAndDrawMap()
{
    #pragma region WorkOutAndDisplayMapTilesPlusPanZoom

        ImVec2 lastSize = m_canvas_sz;
        canvas_pTL = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        m_canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available

        if (m_canvas_sz.x < 50.0f) m_canvas_sz.x = 50.0f;
        if (m_canvas_sz.y < 50.0f) m_canvas_sz.y = 50.0f;
        canvas_pBR = ImVec2(canvas_pTL.x + m_canvas_sz.x, canvas_pTL.y + m_canvas_sz.y);
        m_canvas_Centre = ImVec2((canvas_pBR.x - canvas_pTL.x) / 2 + canvas_pTL.x, (canvas_pBR.y - canvas_pTL.y) / 2 + canvas_pTL.y);

        if ( (lastSize.x != m_canvas_sz.x) || (lastSize.y != m_canvas_sz.y)) mb_MapIsDirty = true;
        // This will catch our mouse interactions
        ImGui::InvisibleButton("canvas", m_canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        mb_isMouseOverMap = ImGui::IsItemHovered(); // Hovered
        const bool is_active = ImGui::IsItemActive();   // Held
        bool panning = false;
       
 
        /*
        if (b_CentreOnAircraft)//check if we're supposed to be following a specific aircraft on the map
        {
            if (NULL == m_AircraftToCentreOn) b_CentreOnAircraft = false;
            else
            {
                DoCentreOnLatLng(m_AircraftToCentreOn->ACLat, m_AircraftToCentreOn->ACLng); //set the centre of the map to the current location of the aircraft
                mb_MapIsDirty = true;
            }
        }
        */
        ImGuiIO& io = ImGui::GetIO();
        if (mb_isMouseOverMap) //if mouse is over this window then check for Zoom
        {
            if (0 != io.MouseWheel) //mouse wheel has moved so Zoom over and around the current mouse location
            {
                //viewport region is determined from viewport centre so need to determine new centre location based on where current mouse position is wrt curretn viewport centre
                double MouseDeltaXFromCentre = io.MousePos.x - m_canvas_Centre.x;
                double MouseDeltaYFromCentre = io.MousePos.y - m_canvas_Centre.y;
                if ((io.MouseWheel > 0) && (m_Zoom < mngr.mTileServer->maxZoom))
                {
                    //zooming in, so viewport centre is moved closer to current mouse positon by half the current distance. Half because zoom in is always a factor if 2
                    m_Zoom++;
                    m_VP_NormalCentre.x += (float)((MouseDeltaXFromCentre / (float)m_WorldSideLengthInPixels) / 2.0f);
                    m_VP_NormalCentre.y += (float)((MouseDeltaYFromCentre / (float)m_WorldSideLengthInPixels) / 2.0f);
                }
                if ((io.MouseWheel < 0) && (m_Zoom > 0))
                {
                    //zooming out so move viewport centre farther away 
                    m_Zoom--;
                    m_VP_NormalCentre.x -= (float)(MouseDeltaXFromCentre / m_WorldSideLengthInPixels);
                    m_VP_NormalCentre.y -= (float)(MouseDeltaYFromCentre / m_WorldSideLengthInPixels);
                }
                mb_MapIsDirty = true;
            }


            if (!b_CentreOnAircraft) //do not pan if we are follwoing an Aircraft
            {
                //PANNING Code
                // Pan (we use a zero mouse threshold when there's no context menu)
                // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
                const float mouse_threshold_for_pan = -0.9f;//opt_enable_context_menu ? -1.0f : 0.0f;
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, mouse_threshold_for_pan))
                {
                    m_VP_NormalCentre.x -= io.MouseDelta.x / (float)m_WorldSideLengthInPixels;
                    m_VP_NormalCentre.y -= io.MouseDelta.y / (float)m_WorldSideLengthInPixels;
                    panning = true;
                    mb_MapIsDirty = true;
                }
            }
        }

        double normalizedSideLengthPerTile = 1.0f / (1 << m_Zoom); //webmaps are square. At Zoom = 0 normalizedWidthPerTile is 1.0
        m_WorldSideLengthInPixels = 256.0f * (1 << m_Zoom);   //same value for Width and Height since web mercator is square 

        //this is the viewport size in between 0.0 and 1.0   
        ImVec2 VP_NormalizedSize((float)(m_canvas_sz.x / m_WorldSideLengthInPixels), (float)(m_canvas_sz.y / m_WorldSideLengthInPixels));

        //set viewport normalized limits around selected centre point (VP_NormalCentre)  VP is always drawn based on its centre location which pans
        //VP_LimitsNormalized is the proportion of the world map to be loaded and scrolled over. //could also use for clipping
        ImPlotRectDouble  VP_LimitsNormalized = ImPlotRectDouble(m_VP_NormalCentre.x - (VP_NormalizedSize.x / 2.0), m_VP_NormalCentre.x + (VP_NormalizedSize.x / 2.0), m_VP_NormalCentre.y - (VP_NormalizedSize.y / 2.0), m_VP_NormalCentre.y + (VP_NormalizedSize.y / 2.0));
        ImVec2 ViewPortAbsTopLeft((float)(VP_LimitsNormalized.X.Min * m_WorldSideLengthInPixels), (float)(VP_LimitsNormalized.Y.Min * m_WorldSideLengthInPixels));

        double VP_CentreProjX = -20037508.3427892 + m_VP_NormalCentre.x * 40075016.6855784;
        double VP_CentreProjY = +20037508.3427892 - m_VP_NormalCentre.y * 40075016.6855784;

        double ll1 = ProjYtoLat(VP_CentreProjY);
        double ll2 = ProjX2Lng(VP_CentreProjX);
        m_VP_CentreLatLng = ImVec2((float)ll1, (float)ll2);


        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>>  region;

        if (true == gb_NoMap)
        {
            draw_list->AddRectFilled(canvas_pTL, canvas_pBR, g_MapBackgroundColour);
        }
        else  //draw the map tiles
        {
            region = mngr.get_region(VP_LimitsNormalized, m_canvas_sz, m_Zoom);  //returns list of tiles that covers (is usually larger than) the viewport

            char buf[20];
            for (auto& pair : region) //for each tile in the region
            {
                TileCoord coord = pair.first;
                std::shared_ptr<Tile> tile = pair.second;
                auto [bmin, bmax] = coord.bounds(); //these are the normalized bounds of a specific tile (z,x,y)

                //current tile TL and BR
                ImVec2 ctTL((float)(canvas_pTL.x + (bmin.x * m_WorldSideLengthInPixels) - ViewPortAbsTopLeft.x), (float)(canvas_pTL.y + (bmax.y * (float)m_WorldSideLengthInPixels) - ViewPortAbsTopLeft.y));  //top left
                ImVec2 ctBR((float)(canvas_pTL.x + (bmax.x * m_WorldSideLengthInPixels) - ViewPortAbsTopLeft.x), (float)(canvas_pTL.y + (bmin.y * m_WorldSideLengthInPixels) - ViewPortAbsTopLeft.y));  //bottom right

                if (tile != nullptr)  draw_list->AddImage((ImTextureID)(intptr_t)tile->image.ID, ctTL, ctBR, { 0,0 }, { 1,1 }, g_MapBackgroundColour);
                else  //indicate no tile loaded/available
                {
                    draw_list->AddText(ImVec2(ctTL.x + ctBR.x / 2, ctTL.y + ctBR.y / 2), ImColor(255, 64, 0, 255), "No TILE");
                    //Draw X over missing Tile
                    draw_list->AddLine(ImVec2(ctTL.x, ctTL.y), ImVec2(ctBR.x, ctBR.y), IM_COL32(255, 0, 0, 255), 1.0f);
                    draw_list->AddLine(ImVec2(ctBR.x, ctTL.y), ImVec2(ctTL.x, ctBR.y), IM_COL32(255, 0, 0, 255), 1.0f);
                }

                if (mb_tileOutlines) draw_list->AddRect(ctTL, ctBR, IM_COL32(0, 0, 255, 255));
                if (mb_tileCoords)
                {
                    sprintf(buf, "%s\r\n%4.3f %4.3f", coord.label().c_str(), bmin.x, bmin.y);
                    draw_list->AddText(ImVec2((ctTL.x + ctBR.x) / 2, (ctTL.y + ctBR.y) / 2), ImColor(255, 0, 0, 255), buf);
                }
            }
        }

        if (0 < dt.nptlat)
        {
            ImVec2 DTED_TL = LatLngToVPxy(43.0, -76.0);
            ImVec2 DTED_BR = LatLngToVPxy(42.0, -75.0);
            draw_list->AddImage((ImTextureID)(intptr_t)dt.DtedTexture, DTED_TL, DTED_BR, { 0,0 }, { 1,1 }, ImColor(255,255,255,DTEDAlpha));

            double lat = 42.0 + DtedRow / 3600.0;
            double lng = -77.0 + DtedCol / 3600.0;
            ImVec2 p1 = LatLngToVPxy(lat, -76.0);
            ImVec2 p2 = LatLngToVPxy(lat, -75.0);


            draw_list->AddLine(p1, p2,ImColor(255, 0, 255, 255));



            if (CreatingLOSLine || LOSLineValid)
            {
                ImVec2 p1 = LatLngToVPxy(LOS_p1.x, LOS_p1.y);

                if (false == LOSLineValid)
                {
                    LOS_p2.x = g_MouseMngr.MouseLL.x;
                    LOS_p2.y = g_MouseMngr.MouseLL.y;
                }

                ImVec2 p2 = LatLngToVPxy(LOS_p2.x, LOS_p2.y);

                draw_list->AddLine(p1, p2, ImColor(255, 0, 0, 255));

            }
        }
        else //load the one DTED file
        {
            if (dt.loadData("n42.dt2"))
            {
                dt.DTEDtoTexture();
                std::cout << "loadData returned true (OK)\n";
            }
        }



    #pragma endregion WorkOutAndDisplayMapTilesPlusPanZoom

    #pragma region MouseHandling

        //mouse position calculations
        double MouseDeltaXFromCentre = io.MousePos.x - m_canvas_Centre.x;
        double MouseDeltaYFromCentre = io.MousePos.y - m_canvas_Centre.y;
        double MouseDeltaTextureXFromCentre = MouseDeltaXFromCentre / m_WorldSideLengthInPixels;
        double MouseDeltaTextureYFromCentre = MouseDeltaYFromCentre / m_WorldSideLengthInPixels;
        double MouseTextureX = MouseDeltaTextureXFromCentre + m_VP_NormalCentre.x;
        double MouseTextureY = MouseDeltaTextureYFromCentre + m_VP_NormalCentre.y;
        double MouseProjX = -20037508.3427892 + MouseTextureX * 40075016.6855784;
        double MouseProjY = +20037508.3427892 - MouseTextureY * 40075016.6855784;

        if (mb_isMouseOverMap)
        {
            //SimAircraft* ac = isMouseHoverdOnAnAircraft((int)io.MousePos.x, (int)io.MousePos.y);// sets a flag for an Aircraft if it is hovered
            //RadarSite* rs = isRadarSiteHovered((int)io.MousePos.x, (int)io.MousePos.y); //sets a flag for a RadarSite if it is hovered

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))         //Right Click: centre on mouse location. 
            {
                m_VP_NormalCentre = ImVec2((float)MouseTextureX, (float)MouseTextureY);
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) //check for single click hovers
            {
                int indexI, indexJ;
                dt.heightAt(ProjYtoLat(MouseProjY), ProjX2Lng(MouseProjX), indexI, indexJ);
                //if (ac != NULL) ac->AircraftIsClicked(); //this will force the Aircraft's popup to appear
                //if (rs != NULL) rs->RadarSiteIsClicked();
            }
            g_MouseMngr.MouseOverMapClickHandler(ImVec2Double(ProjYtoLat(MouseProjY), ProjX2Lng(MouseProjX)));  //tells mouse handler cur pos of mouse in Lat/Lng
        }

        if ((mb_LeftClickMenuOpen) && !mb_ShowRouterBuilder)
        {
            ImGui::OpenPopupOnItemClick("LeftClickMenu", ImGuiPopupFlags_MouseButtonLeft);
            if (ImGui::BeginPopup("LeftClickMenu"))
            {
                if (ImGui::MenuItem("Create Track", NULL, false, true))
                {
                    mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                    //CreateNewTrack(m_LeftClickLocationLL, false);
                }
                if (ImGui::MenuItem("Create Strobe", NULL, false, true))
                {
                    mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                    //CreateNewTrack(m_LeftClickLocationLL, true);
                }

                if (ImGui::MenuItem("Create Route", NULL, false, true))
                {
                    mb_ShowRouterBuilder = true; //causes Create Track Dialog to open
                    mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                }

                if (CreatingLOSLine)
                {
                    if (ImGui::MenuItem("Save LOS Line", NULL, false, true))
                    {
                        CreatingLOSLine = false;
                        LOSLineValid = true;
                        mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                    }
                    if (ImGui::MenuItem("Cancel LOS Line", NULL, false, true))
                    {
                        CreatingLOSLine = false;
                        mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                    }
                    
                }
                else
                {
                    if (ImGui::MenuItem("Create LOS Line", NULL, false, true))
                    {
                        LOSLineValid = false;
                        mb_LeftClickMenuOpen = false;  //stops this menu from displaying
                        CreateLOSLine(m_LeftClickLocationLL);//, true);
                    }
                }


                if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Close", NULL, false, true))
                {
                    mb_LeftClickMenuOpen = false;
                }
                ImGui::EndPopup();
            }
        }


#pragma endregion MouseHandling

    if (mb_map_debug)
        {
            char buf[250];
            ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
            ImGui::Begin("Debug", &mb_map_debug, _flags);
            sprintf(buf, "Mouse Lat/Lng: %7.4f  %8.4f", g_MouseMngr.MouseLL.x, g_MouseMngr.MouseLL.y);
            ImGui::Text(buf);
            sprintf(buf, "Mouse VPO xy: %0.0f  %0.0f", io.MousePos.x, io.MousePos.y);
            ImGui::Text(buf);

            sprintf(buf, "Mouse dx: %4.1f  dy: %4.1f    %1.4f  %1.4f", MouseDeltaXFromCentre, MouseDeltaYFromCentre, MouseDeltaTextureXFromCentre, MouseDeltaTextureYFromCentre);
            ImGui::Text(buf);

            sprintf(buf, "Mouse Tx  X: %1.8f  Y: %1.8f  ProjX/Y  %1.0f  %1.0f", MouseTextureX, MouseTextureY, MouseProjX, MouseProjY);
            ImGui::Text(buf);

            sprintf(buf, "VP Centre (normalized):    %5.8f  %5.8f ", m_VP_NormalCentre.x, m_VP_NormalCentre.y);
            ImGui::Text(buf);

            sprintf(buf, "VP Centre (d.d):    %8.4f  %8.4f ", m_VP_CentreLatLng.x, m_VP_CentreLatLng.y);
            ImGui::Text(buf);

            sprintf(buf, "canvas_p0: %4.0f  %4.0f | canvas_sz:  %4.0f  %4.0f  ", canvas_pTL.x, canvas_pTL.y, m_canvas_sz.x, m_canvas_sz.y);
            ImGui::Text(buf);

            sprintf(buf, "limitsNormalized(Xmn Xmx | Ymn Ymx):  %5.4f  %5.4f |  %5.4f  %5.4f", VP_LimitsNormalized.X.Min, VP_LimitsNormalized.X.Max, VP_LimitsNormalized.Y.Min, VP_LimitsNormalized.Y.Max);
            ImGui::Text(buf);

            sprintf(buf, "Calculated WorldSideLengthInPixels: %4.0f ", m_WorldSideLengthInPixels);
            ImGui::Text(buf);

            if (false == gb_NoMap)
            {
                sprintf(buf, "manual zoom: %d numTiles: %d", m_Zoom, (int)region.size());
                ImGui::Text(buf);
            }

            ImGui::End();
        }

}






void Maps::CheckKeysPressed()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) { mb_LeftClickMenuOpen = false; }
    if (ImGui::IsKeyPressed(ImGuiKey_M))    mb_showLLatMousePointer = !mb_showLLatMousePointer;
    if (ImGui::IsKeyPressed(ImGuiKey_Y))    glob_bInTest = !glob_bInTest;
    //if (ImGui::IsKeyPressed(ImGuiKey_R))    m_VP_NormalCentre = ImVec2(0.5, 0.5); //reset viewg

    if (ImGui::IsKeyPressed(ImGuiKey_Z)) { MyLog.AddLog("Test"); }
    if (ImGui::IsKeyPressed(ImGuiKey_P)) { g_SetToastMessage("Toast Test"); }
    if (ImGui::IsKeyPressed(ImGuiKey_Q)) { b_CentreOnAircraft = false; }

    if (ImGui::IsKeyPressed(ImGuiKey_G)) { mShowGeoToolDialog = !mShowGeoToolDialog; }
    if (ImGui::IsKeyPressed(ImGuiKey_O)) {
        DTEDAlpha -= 5; if (DTEDAlpha > 255) DTEDAlpha = 255;
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_P)) { DTEDAlpha += 5; if (DTEDAlpha < 0) DTEDAlpha = 0;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_I)) { dt.SetScaleLocal(); }

    if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        LoadShapeFiles();
    }
   
}


void Maps::DrawMGRSMousePointer(ImGuiIO& io)
{
    DrawMGRSGrid(m_MGRS, FuncPtr);

    ImVec2 centre(canvas_pTL.x + m_canvas_sz.x / 2, canvas_pTL.y + m_canvas_sz.y / 2);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(io.MousePos.x - 5, io.MousePos.y - 5), ImVec2(io.MousePos.x + 5, io.MousePos.y + 5), IM_COL32(250, 0, 0, 255));
    ImGui::GetWindowDrawList()->AddLine(ImVec2(io.MousePos.x + 5, io.MousePos.y - 5), ImVec2(io.MousePos.x - 5, io.MousePos.y + 5), IM_COL32(250, 0, 0, 255));
    char MGRS_Pretty[30];
    MGRSPretty(m_MGRS, MGRS_Pretty);
    ImGui::GetWindowDrawList()->AddText(ImVec2(io.MousePos.x + 15, io.MousePos.y - 20), ImColor(255, 0, 0, 255), MGRS_Pretty);
}



void Maps::ShowAllMenuBars()
{
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save AppIni")) {
                    ImGui::SaveIniSettingsToDisk("RadarIni.ini");
                }
                if (ImGui::MenuItem("Load AppIni")) {
                    ImGui::LoadIniSettingsFromDisk("RadarIni.ini");
                }
                


                if (ImGui::MenuItem("Re-Load App State")) {
                    DoLoadAllModulesStateFromApp_Ini();
                }
                ImGui::SetItemTooltip("Loads app.ini file.");

                if (ImGui::MenuItem("Save App State")) {
                    CreateAppIni();
                    DoSaveAllModuleStateToApp_Ini();
                }
                ImGui::SetItemTooltip("Saves App (not scenario) state.  Creates new app.ini file if one does not exist.");

                /*
                if (ImGui::MenuItem("Load Sim A/C List")) {
                    LoadAircraftList();
                }
                ImGui::SetItemTooltip("Loads pre-saved aircraft list.");
            
                if (ImGui::MenuItem("Save Sim A/C List")) {
                    SaveAircraftList();
                }
                ImGui::SetItemTooltip("Saves current Sim Aircraft list to file.");
                */

                if (ImGui::MenuItem("Auto Save/Load A/C List",NULL, AutoLoadAircraft)) {
                    AutoLoadAircraft = !AutoLoadAircraft;
                }
                ImGui::SetItemTooltip("Saves Aircraft list on exit and restores on next start.");

                ImGui::Separator();
                if (ImGui::MenuItem("App Options", NULL, mb_showOptions)) { mb_showOptions = !mb_showOptions; }
                if (ImGui::MenuItem("Geo Tool", NULL, mShowGeoToolDialog)) { mShowGeoToolDialog = !mShowGeoToolDialog; }


                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) {
                    SetWindowToClose();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Map Presets"))
            {
                if (ImGui::MenuItem("Show Route List", NULL, mb_ShowRouteList)) { mb_ShowRouteList = !mb_ShowRouteList; }
                if (ImGui::MenuItem("Show POI List", NULL, mb_ShowPOIList)) { mb_ShowPOIList = !mb_ShowPOIList; }
                if (ImGui::MenuItem("Show ShapeFile List", NULL, mb_ShowShapeFileList)) { mb_ShowShapeFileList = !mb_ShowShapeFileList; }

                ImGui::Separator();

                if (ImGui::MenuItem("3 Wing", NULL)) { DoCentreOnLatLng(48.5705, -70.7488); m_Zoom = 7; }
                if (ImGui::MenuItem("4 Wing", NULL)) { DoCentreOnLatLng(54.4327, -110.2845); m_Zoom = 7; }
                //if (ImGui::MenuItem("Sceptre Way", NULL)) { DoCentreOnLatLng(45.276350, -75.884260); m_Zoom = 20; }
                if (ImGui::MenuItem("CYOW", NULL)) { DoCentreOnLatLng(45.3189, -75.6683); m_Zoom = 12; }
                if (ImGui::MenuItem("Falklands", NULL)) { DoCentreOnLatLng(-51.6678, -57.8611); m_Zoom = 11; }
                if (ImGui::MenuItem("Gagetown", NULL)) { DoCentreOnLatLng(45.6272, -66.2904); m_Zoom = 11; }
                if (ImGui::MenuItem("Kananaskis", NULL)) { DoCentreOnLatLng(51.0742, -115.1271); m_Zoom = 14; }

                ImGui::EndMenu();
            }

            mngr.DrawTileManagerMapMenu(gb_NoMap);
            //DrawADSBMenuBar(&mb_showADSBAircraftTable);
            //DrawRadarMenu();

            //DrawAircraftMenu();
            //_JREAP.DrawL16Menu();

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("TileManagerStats", NULL, mb_showTMStats)) { mb_showTMStats = !mb_showTMStats; }
                if (ImGui::MenuItem("Show Demo Window", NULL, mb_showDemoWindow)) { mb_showDemoWindow = !mb_showDemoWindow; }

                if (ImGui::MenuItem("Log", NULL, mb_showLog)) { mb_showLog = !mb_showLog; }
                if (ImGui::MenuItem("Debug", NULL, mb_map_debug)) { mb_map_debug = !mb_map_debug; }
                if (ImGui::MenuItem("Help", NULL, mb_showHelp)) { mb_showHelp = !mb_showHelp; }
                if (ImGui::MenuItem("About", NULL, mb_showAbout)) { mb_showAbout = !mb_showAbout; }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

}


void Maps::DrawScaleBar()
{
    const int barlength = 300;
    const int barIndent = 20;
    const int barUpdent = 50;


    float bar_left = canvas_pTL.x + barIndent;
    float bar_Y = canvas_pBR.y - barUpdent;
    ImVec2 start(bar_left, bar_Y);
    ImVec2 end(bar_left + barlength, bar_Y);

    ImVec2Double barLeftLL = VPxyRoLatLng(bar_left, bar_Y);
    ImVec2Double barRightLL = VPxyRoLatLng(bar_left + barlength, bar_Y);

    ImVec2Double br = GetBearingAndRangeInNMBetweenTwoGeoPoints(barLeftLL.x, barLeftLL.y, barRightLL.x, barRightLL.y);

  
    ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(0, 0, 0, 255), 2.0f);
    for (int q = 0; q <= 300; q += 75)
    {
        ImGui::GetWindowDrawList()->AddLine(ImVec2(start.x + q, start.y - 7), ImVec2(start.x + q, start.y + 7), ImColor(0, 0, 0, 255), 1.0f);       
    }

    ImVec2 zeroSize = ImGui::CalcTextSize("0");


    ImGui::GetWindowDrawList()->AddText(ImVec2(start.x - zeroSize.x/3, start.y + 10) , IM_COL32_BLACK, "0");
    zeroSize = ImGui::CalcTextSize("Average Scale");
    ImGui::GetWindowDrawList()->AddText(ImVec2(start.x + barlength/2 - zeroSize.x / 2, start.y + 10), IM_COL32_BLACK, "Average Scale");
    char buf[50];
    if (br.x>50) sprintf_s(buf, "%0.0f", br.x);
    else sprintf_s(buf, "%0.2f", br.x);
    zeroSize = ImGui::CalcTextSize(buf);
    ImGui::GetWindowDrawList()->AddText(ImVec2(end.x - zeroSize.x / 3, end.y +10), IM_COL32_BLACK, buf);
}




void Maps::DrawToolBar()
{
    ImGui::SetNextWindowPos(ImVec2(canvas_pTL.x + 250, canvas_pTL.y));
    //ImGui::SetNextWindowSize(ImVec2(6 * 30, 30));

    ImGuiWindowFlags _flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Tool Bar", 0, _flags);


    char buf[40];

    for (int x = 0;x < 5;x++)
    {
        sprintf(buf, "%d", x + 1);
        if (RedButton(buf, ImVec2((float)(ImGui::GetFontSize() * 1.5), (float)(ImGui::GetFontSize() * 1.5))))
        {
            DoCentreOnLatLng(lats[x], lngs[x]); m_Zoom = zooms[x];
        }

        if (ImGui::BeginPopupContextItem())
        {
            //static char name[32] = "Label1";
            ImGui::Text("Edit name:");
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7);
            ImGui::InputText("##edit", names[x], IM_ARRAYSIZE(names[x]));
            if (ImGui::Button("Set"))
            {
                lats[x] = m_VP_CentreLatLng.x;
                lngs[x] = m_VP_CentreLatLng.y;
                zooms[x] = m_Zoom;
                //strcpy(names[x], name);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        sprintf(buf, "%s: %7.4f, %8.4f at Zoom %d", names[x], lats[x], lngs[x], zooms[x]);
        ImGui::SetItemTooltip(buf);
        ImGui::SameLine();
    }




    ImGui::End();


    //DrawMGRSGrid("17TQK34210826", FuncPtr, 5);
    //DrawMGRSGrid("18TTQ78360957", FuncPtr, 5);

}

void Maps::DrawTimeAndPosDisplay()
{
    char timeBuf[100];
    SYSTEMTIME st;
    GetSystemTime(&st);
    char* months[] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
    sprintf_s(timeBuf, "%d %s %d  %02d:%02d:%02d Z\r\n", st.wDay, months[st.wMonth - 1], st.wYear, st.wHour, st.wMinute, st.wSecond);

    //Coordinate display
    int latDeg = (int)g_MouseMngr.MouseLL.x;
    float latMin = (float)(60.0 * (g_MouseMngr.MouseLL.x - latDeg));
    double latSec = 60.0 * (latMin - (int)latMin);

    char NS = 'S'; if (g_MouseMngr.MouseLL.x > 0) NS = 'N';
    char EW = 'W'; if (g_MouseMngr.MouseLL.y > 0) EW = 'E';
    double LatInDeg = abs(g_MouseMngr.MouseLL.y);
    int lngDeg = (int)LatInDeg;
    float lngMin = (float)(60.0 * (LatInDeg - lngDeg));
    double lngSec = (float)(60.0 * (lngMin - (int)lngMin));

    double latRad = DEG2RAD(g_MouseMngr.MouseLL.x);// *PI / 180;
    double lngRad = DEG2RAD(g_MouseMngr.MouseLL.y);// *PI / 180;
    long retVal = Convert_Geodetic_To_MGRS(latRad, lngRad, g_MGRS_Precison, m_MGRS);

    int indexI, indexJ;
    int heightInMetres = dt.heightAt(g_MouseMngr.MouseLL.x, g_MouseMngr.MouseLL.y,indexI, indexJ);

    char MGRS_Pretty[30];
    MGRSPretty(m_MGRS, MGRS_Pretty);

    char buf[350];
    sprintf(buf, "%s\r\n%7.4f %8.4f\r\n%c%02d\xc2\xb0%02d'%05.2f\" %c%02d\xc2\xb0 %02d'%05.2f\r\n%s\r\ni/j: %d, %d HAE:%d m", timeBuf, g_MouseMngr.MouseLL.x, g_MouseMngr.MouseLL.y, NS, latDeg, (int)latMin, latSec, EW, lngDeg, (int)lngMin, lngSec, MGRS_Pretty, indexI, indexJ,heightInMetres);

    ImVec2 label_size = ImGui::CalcTextSize(buf);
    ImVec2 p1(canvas_pTL.x + 10, canvas_pTL.y + 10);
    DrawBoxedText(buf, p1, IM_COL32_BLACK, IM_COL32_BLACK, IM_COL32_WHITE, 1.0f);



}




static char ToastBuf[100] = "Toast Test";
static int ToastCounter = 240; 

void Maps::DrawToast()
{
    if (--ToastCounter<=0) return;

    ImVec2 label_size = ImGui::CalcTextSize(ToastBuf);
    ImVec2 label_off(10, 10);
    ImVec2 p1(canvas_pTL.x + label_off.x, canvas_pTL.y + 8 * label_size.y - label_off.y);
    DrawBoxedText(ToastBuf, p1, IM_COL32_BLACK, IM_COL32_BLACK, IM_COL32(0xFF, 0xA5, 0, 255), 1.0f);
}

void g_SetToastMessage(char* m, int PersistTimeInSeconds)
{
    strncpy(ToastBuf, m, 100);
    ToastCounter = 60 * PersistTimeInSeconds;
}
