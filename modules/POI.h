#pragma once

#include "ImGui.h"
#include <vector>
#include <utility>  // For std::pair

#include "ImBGUtil.h"




void CreateTestPOIs();
void DrawPOIs(ImDrawList* draw_list, ImVec2(*funcLatLon2VPxy)(double, double));
bool ShowPOIListDialog(bool* pOpen);
void InsertExamplePOIs();
void DeleteSelectedPOIs();

extern const char* POIDBFname;

enum POIType {
    POI_POINT = 0,
    POI_CIRCLE = 1,
    POI_POLYLINE = 2,
    POI_POLYGON = 3,
    POI_NONE = 4
};

struct POI
{
    double lat;
    double lng;

    bool loaded = true;
    bool showOnMap = true;
    bool selected = false;
    bool showLabel = true;
   
    int clipRadiusNM = 5;

    char name[40] = "POI";
    POIType POI_TYPE = POI_POINT;

  
    ImU32 color = ImColor(0, 0, 255, 255); //default colour
    ImU32 colorFill = ImColor(0, 0, 255, 255); //default colour

    // For circle
    float radiusNM = 5.0f; //default radius
    bool filled = false;

    // For polyline and polygon
    int pointIconIndex = 0;
    std::vector<std::pair<double, double>> points;

    void Draw(ImVec2(*funcLatLon2VPxy)(double, double));

    POI(struct POI *_p);

    POI() {
        lat = 0.0f;
        lng = 0.0f;
        loaded = false;
        showOnMap = false;
        selected = false;
        radiusNM = 5;
        POI_TYPE = POI_POINT;

    };
};


void DrawCircle(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi);
void DrawPolyline(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi);
void DrawPolygon(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi);