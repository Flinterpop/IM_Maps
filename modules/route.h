#pragma once


#include <vector>
#include "imgui.h"

bool ShowCreateRouteDialog(ImVec2(*LatLon2VPxy) (double, double), bool* pOpen); 
bool ShowRouteListDialog(bool* pOpen, bool &TellMapsToShowNewRouteBuilder, ImVec2(*LatLon2VPxy) (double, double));
void DrawRoutes(ImDrawList* draw_list, ImVec2(*LatLon2VPxy) (double, double));



struct waypoint
{
    char name[40];
    double lat;
    double lng;
    int altitude;
    int speed;

    int TurnRate = 2;  //degree per second     
    int ClimbRate = 100; //feet per minute
    int SpeedRate = 5; //nmph per sec
    int DB_ID;
};


struct route
{
    //items saved in DB
    int DB_ID;
    char name[80];
    int NumWayPoints;
    int Floor = 0; //lowest altitude 
    int Ceiling = 60000; //highest altitude
    ImU32 routeColour = ImColor(255, 0, 0, 255);
    
    bool showOnMap = true;

    std::vector<struct waypoint*> wpList;

    //not saved in DB
    bool loaded = false;
    bool hoverShowOnMap = false; 

};


struct route* FindRouteIDFromName(char* rname);

void DrawRoute(struct route* r, ImVec2(*LatLngToVPxy) (double, double), float width);
