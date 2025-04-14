#include <vector>
#include <string>

#include "imgui.h"
#include "AppLogger.h"
//#include "SimAircraft.h"
#include "MouseManager.h"
#include "fonts/IconsFontAwesome5.h"

#include "route.h"
#include "SQL_RouteList.h"
#include "ImBGUtil.h"
#include "Mediator.h"

extern Mediator _m;
extern struct _MouseManager g_MouseMngr;
std::vector<route*> RouteList;

static bool gb_ShowRouterBuilder;
static struct route* g_DraftRoute = NULL;
static int RouteStarted = false;
static bool RxNotificationForPosition = false;

static void CreateNewWaypoint(double lat, double lon, route* _r);

int NotifyMe(enum Mediator::NOTIFY_MSG m, void* data)
{
    if (m == Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION) 
    {
        ImVec2Double* d = (ImVec2Double*)data;
        CreateNewWaypoint(d->x, d->y, g_DraftRoute);
    }
    return 0;
}


void LoadAllRoutes()
{
    api_LoadRouteListFromDB();
    for (auto r : RouteList)
    {
        if (!r->loaded) r->loaded = api_LoadWaypointsForRouteFromDB(r);
    }
        
}

static void CreateNewWaypoint(double lat, double lon, route* _r)
{
    waypoint* w = new waypoint();
    sprintf(w->name, "wp%d", (int)_r->wpList.size());
    w->lat = lat;
    w->lng = lon;
    w->altitude = 15000;
    w->speed = 300;
    w->TurnRate = 2;
    w->ClimbRate = 100;
    w->SpeedRate = 5;
    _r->wpList.emplace_back(w);
}


bool ShowCreateRouteDialog(ImVec2(*LatLon2VPxy) (double, double), bool *pOpen)
{
    static char rname[40] = "";

    if (false == RouteStarted)
    {
        sprintf(rname, "Route_%02d", (int)RouteList.size());
        g_DraftRoute = new route();
        RouteStarted = true;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Create_Route", pOpen, flags); //Dialog for creating a new track

        ImGuiInputTextFlags f = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank;
        ImGui::Text("Name:");ImGui::SameLine();
        ImGui::SetNextItemWidth(10.0f * ImGui::GetFontSize());
        ImGui::InputText("##", rname, IM_ARRAYSIZE(rname), f);
       
        ImGui::SameLine();ImGui::Text("Select point on map"); ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CROSSHAIRS))
        {
            RxNotificationForPosition = false; //not used???????????
            std::function<int(Mediator::NOTIFY_MSG, void*)> f = std::bind(&NotifyMe, std::placeholders::_1, std::placeholders::_2);
            _m.SubScribeForNotifications(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION, f, false);
        }
        
        ImGui::Text("Route Colour");ImGui::SameLine();
        static float col2[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
        ImGui::ColorEdit4("##", col2, ImGuiColorEditFlags_InputRGB);
        g_DraftRoute->routeColour = ImGui::ColorConvertFloat4ToU32(ImVec4(col2[0], col2[1], col2[2], col2[3]));
        

        ImGui::Separator();
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        int hovered_column = -1;
        //static ImGuiTableFlags tflags = ImGuiTableFlags_SortMulti | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;
        static ImGuiTableFlags tflags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
        char* ColTit[] = { "Name", "Lat", "Lon","Altitude","Speed","Turn Rate", "Climb Rate", "Speed Rate"};
        int numCol = 8;
        if (ImGui::BeginTable("table1", numCol, tflags))
        {
            ImGui::TableSetupColumn(ColTit[0], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn(ColTit[1], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn(ColTit[2], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 13.0f);
            ImGui::TableSetupColumn(ColTit[3], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 11.0f);
            ImGui::TableSetupColumn(ColTit[4], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 11.0f);
            ImGui::TableSetupColumn(ColTit[5], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10.0f);
            ImGui::TableSetupColumn(ColTit[6], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10.0f);
            ImGui::TableSetupColumn(ColTit[7], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10.0f);

            ImGui::TableHeadersRow();
            ImGui::TableNextRow();

            int row = 0;
            for (auto w : g_DraftRoute->wpList)
            {
                int index = 0;
                ImGui::TableSetColumnIndex(index++); //ImGui::Text("%s", w->name);
                ImGui::PushID(row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
                ImGui::InputText("##cell", w->name, IM_ARRAYSIZE(w->name));
                ImGui::PopID();

                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
                ImGui::TableSetColumnIndex(index++);
                ImGui::PushID(row+20);
                ImGui::InputDouble("##cell", &w->lat, 0.0001, 0.01, "%7.4f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
                ImGui::PopID();

                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 13.0f);
                ImGui::TableSetColumnIndex(index++);
                ImGui::PushID(row+40);
                ImGui::InputDouble("##cell", &w->lng, 0.0001, 0.01, "%8.4f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(index++); 
                ImGui::PushID(60+row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 11.0f);
                ImGui::InputInt("##cell", &w->altitude,10,100);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(index++); 
                ImGui::PushID(80 + row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 11.0f);
                ImGui::InputInt("##cell", &w->speed, 1, 100);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(index++);
                ImGui::PushID(100 + row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
                ImGui::InputInt("##cell", &w->TurnRate, 1, 10);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(index++);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
                ImGui::PushID(120 + row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
                ImGui::InputInt("##cell", &w->ClimbRate, 10, 100);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(index++);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
                ImGui::PushID(140 + row);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
                ImGui::InputInt("##cell", &w->SpeedRate, 1, 100);
                ImGui::PopID();


                ImGui::TableNextRow();
                ++row;
            }

            ImGui::EndTable();
        }
        switch (hovered_column)
        {
        case 5:
            ImGui::Text("Turn Rate in degrees per second");
            break;
        case 6:
            ImGui::Text("Climb Rate in feet per minute");
            break;
        case 7:
            ImGui::Text("Speed Rate in knots per second");
            break;
        default:
            ImGui::Text("Hovered column: %d", hovered_column);
        }
        

        ImGui::Separator();


        if (ImGui::Button("Save"))
        {
            strcpy(g_DraftRoute->name, rname);
            g_DraftRoute->showOnMap = true;
            g_DraftRoute->loaded = true;
            RouteList.emplace_back(g_DraftRoute);
            api_SaveNewRoute(g_DraftRoute);
            //LoadAllRoutes(); //force reload so all Database IDs are present in the Route Data Structures
            *pOpen = false;
        }

        if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Cancel/Close"))
        {
            g_DraftRoute->showOnMap = false;
            delete g_DraftRoute;
            *pOpen = false;
        }

    ImGui::End();

    if (*pOpen == false)
    {
        RouteStarted = false;
        _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
    }

    if (g_DraftRoute->showOnMap) //draw draft route
    {
        DrawRoute(g_DraftRoute, LatLon2VPxy, 2.0f); //default width is 1.0f
    }


    return *pOpen;
}



static int ShowRouteDetailsPanel(int routeIndex, ImVec2(*LatLon2VPxy) (double, double))
{
    bool bStayOpen = true;
    route* rte = RouteList[routeIndex];
    rte->hoverShowOnMap = true;
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    //const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Route_Details", &bStayOpen, flags);

    bool is_MouseOverDialog = ImGui::IsItemHovered(); // Hovered
    ImVec2 CurWinPos = ImGui::GetWindowPos();

    ImGui::Text("Route Name: %s", rte->name);
    ImGui::Text("DataBase ID: %02d", rte->DB_ID);
    ImGui::Text("Number of Waypoints: %02d", rte->wpList.size());
    
    ImVec4 color = ImGui::ColorConvertU32ToFloat4(rte->routeColour);
    ImGui::ColorEdit4("MyColor##1", (float*)&color);
    rte->routeColour = ImGui::ColorConvertFloat4ToU32(color);

    ImGui::Checkbox("Show on Map", &rte->showOnMap);
    ImGui::SeparatorText("Waypoint List");


    
    //static ImGuiTableFlags tflags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
    static ImGuiTableFlags tflags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
    char* ColTit[] = { "Name", "Lat","Lng","Alt", "Speed", "Turn rate", "Climb rate", "Accel" ,"Delete"};
    int numCol = 9;
    if (ImGui::BeginTable("tableRD", numCol, tflags))
    {
        ImGui::TableSetupColumn(ColTit[0], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10.0f); 
        ImGui::TableSetupColumn(ColTit[1], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f); 
        ImGui::TableSetupColumn(ColTit[2], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 13.0f); 
        ImGui::TableSetupColumn(ColTit[3], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f); 
        ImGui::TableSetupColumn(ColTit[4], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn(ColTit[5], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn(ColTit[6], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn(ColTit[7], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn(ColTit[8], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6.5f);

        ImGui::TableHeadersRow();
        ImGui::TableNextRow();

        int row = 0;
        for (waypoint *w : rte->wpList)
        {
            int index = 0;
            ImGui::TableSetColumnIndex(index++); 
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
            ImGui::PushID(row);
                ImGui::InputText("##cell", w->name, IM_ARRAYSIZE(w->name));
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++); 
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::PushID(100+ row);
            ImGui::InputDouble("##cell", &w->lat, 0.0001, 0.01, "%7.4f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
            ImGui::PopID();
            
            ImGui::TableSetColumnIndex(index++); //ImGui::Text("%08.4f", w->lng);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 13.0f);
            ImGui::PushID(200 + row);
            ImGui::InputDouble("##cell", &w->lng, 0.0001, 0.01, "%8.4f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::PushID(300 + row);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::InputInt("##cell", &w->altitude, 10, 100);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::PushID(400 + row);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::InputInt("##cell", &w->speed, 1, 100);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::PushID(600 + row);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::InputInt("##cell", &w->TurnRate, 1, 10);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::PushID(800 + row);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::InputInt("##cell", &w->ClimbRate, 10, 100);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::PushID(1000 + row);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 12.0f);
            ImGui::InputInt("##cell", &w->SpeedRate, 1, 100);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(index++);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 6.5f);
            char buf[40];
            sprintf(buf, ICON_FA_TRASH " Delete##%d", row);  
            if (ImGui::SmallButton(buf))
            {
                rte->wpList.erase(rte->wpList.begin() + row);
            }


            ImGui::TableNextRow();
            ++row;
        }
        ImGui::EndTable();
        
        if (ImGui::Button("Update DB"))
        {
            printf("Update route : %s\r\n", rte->name);
            api_UpdateRoute(rte);
        }
        ImGui::SameLine();ImGui::Text("Changes to Waypoints are immediate but are not persisted until Update DB button is pressed.");
        ImGui::SameLine();if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Close"))
        {
            routeIndex = -1;
        }
    }
    ImVec2 cws = ImGui::GetWindowSize();
    ImGui::End();

    if (is_MouseOverDialog && !rte->wpList.empty())
    {
        int dx = (int)cws.x;
        ImVec2 p = LatLon2VPxy(rte->wpList[0]->lat, rte->wpList[0]->lng);
        if (CurWinPos.x > (p.x  - cws.x)) dx = 0;
        int dy = (int)cws.y;
        if (CurWinPos.y > (p.y - cws.y)) dy = 0;
        ImGui::GetForegroundDrawList()->AddLine(p, ImVec2(CurWinPos.x + dx, CurWinPos.y + dy), ImColor(0X67, 0X67, 0xAB, 255), 2.0f);
    }


    if (false == bStayOpen)
    {
        rte->hoverShowOnMap = false;
        return -1;
    }
    return routeIndex;
}

bool ShowRouteListDialog(bool* pOpen, bool &TellMapsToShowNewRouteBuilder, ImVec2(*LatLon2VPxy) (double, double))
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Route_List", pOpen, flags);

    if (ImGui::Button(ICON_FA_DATABASE " Create Route DB")) api_CreateRouteTables();
    ImGui::SameLine(); 
    if (ImGui::Button(ICON_FA_TRASH " Delete Route DB")) api_ClearRoutesDB();
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_DOWNLOAD " Create Route"))
    {
        gb_ShowRouterBuilder = true; 
        TellMapsToShowNewRouteBuilder = true;
    }

    if (RouteList.size() == 0)  ImGui::Text("There are no routes Loaded");

    static ImGuiTableFlags tflags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV ;
    char* ColTit[] = { "Edit", "Show", "Name", "Delete"};
    int numCol = 4;
    static int selected_route = -1;
    if (ImGui::BeginTable("table1", numCol, tflags))
    {
        ImGui::TableSetupColumn(ColTit[0], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 5.0f);
        ImGui::TableSetupColumn(ColTit[1], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 5.0f);
        ImGui::TableSetupColumn(ColTit[2], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
        ImGui::TableSetupColumn(ColTit[3], ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10.0f);

        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        char buf[20];
        for (int r = 0; r < RouteList.size(); r++)  
        {
            RouteList[r]->hoverShowOnMap = false;
            int index = 0;
            ImGui::TableSetColumnIndex(index++); 
            ImGui::PushID(r);
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 5.0f);
                ImGui::RadioButton("###", &selected_route, r);
            ImGui::PopID();
               
            ImGui::TableSetColumnIndex(index++); 
            ImGui::PushID(100+r);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 5.0f);
            ImGui::Checkbox("###" , &RouteList[r]->showOnMap);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone))  RouteList[r]->hoverShowOnMap = true;

            ImGui::PopID();
            
  


              
            ImGui::TableSetColumnIndex(index++); 
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 15.0f);
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(RouteList[r]->routeColour);
            ImGui::TextColored(color,RouteList[r]->name);

            ImGui::TableSetColumnIndex(index++);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 10.0f);
            sprintf(buf, ICON_FA_TRASH " Delete##%d", r);  if (ImGui::SmallButton(buf)) 
            { 
                bool retVal = api_DeleteRoute(RouteList[r]->DB_ID); 
                if (!retVal)
                {
                    RouteList.erase(RouteList.begin() + r);
                }
            }
 
            ImGui::TableNextRow();

        }
        ImGui::EndTable();
    }
 
    ImGui::End();


    if (selected_route >= RouteList.size()) selected_route = -1;
    if (-1 != selected_route) selected_route = ShowRouteDetailsPanel(selected_route, LatLon2VPxy);
 



    if (*pOpen == false)
    {
        //do any cleanup
    }
    return *pOpen;
}


void DrawRoute(route * r, ImVec2(*LatLngToVPxy) (double, double), float width)
{
    for (int i = 0;i < r->wpList.size();i++)
    {
        ImVec2 p2 = LatLngToVPxy(r->wpList[i]->lat, r->wpList[i]->lng);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p2.x - 5, p2.y - 5), ImVec2(p2.x + 5, p2.y + 5), r->routeColour, 1.0);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p2.x + 5, p2.y - 5), ImVec2(p2.x - 5, p2.y + 5), r->routeColour, 1.0);
        //ImGui::GetWindowDrawList()->AddText(ImVec2(p2.x + 5, p2.y + 5), r->routeColour, r->wpList[i]->name);
        if (i > 0)
        {
            ImVec2 p1 = LatLngToVPxy(r->wpList[i - 1]->lat, r->wpList[i - 1]->lng);
            ImGui::GetWindowDrawList()->AddLine(p1, p2, r->routeColour, width);
        }
        //DrawBoxedText(r->wpList[i]->name, ImVec2(p2.x, p2.y), r->routeColour, r->routeColour);
    }

    for (int i = 0; i < r->wpList.size(); i++)
    {
        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() , FLT_MAX, 0.0f, r->wpList[i]->name);

        ImVec2 p2 = LatLngToVPxy(r->wpList[i]->lat, r->wpList[i]->lng);
        DrawBoxedText(r->wpList[i]->name, ImVec2(p2.x- textSize.x/2, p2.y - textSize.y / 2), r->routeColour, r->routeColour);
    }



}

void DrawRoutes(ImDrawList* draw_list, ImVec2(*LatLngToVPxy) (double, double))
{
    for (auto r : RouteList)
    {
        if (r->showOnMap || r->hoverShowOnMap) DrawRoute(r, LatLngToVPxy, 5.0f); //default width is 1.0f
    }
}


route * FindRouteIDFromName(char* rname)
{
    for (auto r : RouteList)
    {
        if (0 == strncmp(r->name, rname,40))
            return r;
    }
    return NULL;
}