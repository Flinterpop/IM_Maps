#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sqlite3.h>
#include <iostream>

#include "ImGui.h"
#include "POI.h"
#include "fonts/IconsFontAwesome5.h"
#include "libmgrs/mgrs.h"

#include "mediator.h"
#include "MouseManager.h"

#include <libmgrs/utm.h>



const char* POINT_ICONS[] = {
    ICON_FA_MAP_MARKER,           // Classic marker
    ICON_FA_MAP_MARKER_ALT,       // Alternative marker
    ICON_FA_MAP_PIN,              // Pin
    ICON_FA_CROSSHAIRS,           // Crosshair
    ICON_FA_PLUS,                 // Plus sign
    ICON_FA_DOT_CIRCLE,           // Dot circle
    ICON_FA_CIRCLE,               // Circle
    ICON_FA_PLANE,		  // Plane
    ICON_FA_FIGHTER_JET,	  // Fighter Jet
    ICON_FA_ROCKET,		  // Rocket
    ICON_FA_SHIP,		  // Ship
    ICON_FA_ANCHOR,		  // Anchor
    ICON_FA_USERS,		  // Users - Group
    ICON_FA_USER,		  // User - Person
    ICON_FA_CAR,		  // Car
    ICON_FA_TRUCK,		  // Truck
    ICON_FA_EYE,		  // Eye
    ICON_FA_FIRE,		  // Fire
    ICON_FA_BAN,		  // Restricted
    ICON_FA_BOLT,		  // Lightning
    ICON_FA_BINOCULARS	          // Binoculars
};

const char* POINT_ICON_NAMES[] = {
    "Map Marker",
    "Alt Marker",
    "Pin",
    "Crosshair",
    "Plus",
    "Dot Circle",
    "Circle",
    "Plane",
    "Fighter Jet",
    "Rocket",
    "Ship",
    "Anchor",
    "Group",
    "Person",
    "Car",
    "Truck",
    "Eye",
    "Fire",
    "Restricted",
    "Lightning",
    "Binoculars"
};


extern Mediator _m;
extern int g_MGRS_Precison;
extern struct _MouseManager g_MouseMngr;


const int NUM_POINT_ICONS = sizeof(POINT_ICONS) / sizeof(POINT_ICONS[0]);
constexpr double EARTH_RADIUS_NM = 3440.065;  // Earth's radius in nautical miles
constexpr double METERS_PER_NAUTICAL_MILE = 1852.0;  // Meters per nautical mile

static double newLat = 0.0, newLng = 0.0;
static bool b_AwaitingPosition = false;
static bool b_CreatingPOI = false;
static bool b_radiusInNM = true;
static bool b_posInMGRS = false;


static bool b_addPointOnNotify = false;

std::vector<POI*> POIList;
static struct POI *g_DraftPOI;


void LoadPOIListFromDB(); // in SQL_POIList.cpp
void CreatePOITable();
int InsertPOIRecord(POI* poi);


//copy constructor
POI::POI(struct POI *_p)
{
    strncpy(name, _p->name, sizeof(name) - 1);
    lat = _p->lat;
    lng = _p->lng;
    POI_TYPE = _p->POI_TYPE;
    radiusNM = _p->radiusNM;
    filled = _p->filled;
    color = _p->color;
};



void deletePOI(POI * poi)
{
    // Delete from database
    sqlite3* DB;
    if (sqlite3_open(POIDBFname, &DB) == SQLITE_OK) 
    {
        std::string deleteQuery = "DELETE FROM POI WHERE Name = '" + std::string(poi->name) + "';";

        char* messageError;
        if (sqlite3_exec(DB, deleteQuery.c_str(), NULL, 0, &messageError) != SQLITE_OK) 
        {
            sqlite3_free(messageError);
            printf("Error deleting POI: %s\r\n", deleteQuery.c_str());
        }
        else printf("Deleted POI\r\n");
        sqlite3_close(DB);
    }
}

void CreateTestPOIs()
{
    POI* fred = new POI();
    fred->lat = 45;
    fred->lng = -75;
    fred->POI_TYPE = POI_POINT;
    fred->color = ImColor(255, 0, 0, 255);  // Red
    fred->pointIconIndex = 0;  // Add this line
    strncpy(fred->name, "Point", 40);
    POIList.push_back(fred);

    fred = new POI();
    fred->lat = 44;
    fred->lng = -75;
    fred->POI_TYPE = POI_CIRCLE;
    fred->radiusNM = 5.0f;
    strncpy(fred->name, "Circle", 40);
    fred->color = ImColor(0, 255, 0, 255);  // Green
    POIList.push_back(fred);

    fred = new POI();
    fred->lat = 43;
    fred->lng = -75;
    fred->POI_TYPE = POI_POLYLINE;
    fred->points = {{43, -75}, {43.5, -75.5}, {44, -75}};
    strncpy(fred->name, "Polyline", 40);
    fred->color = ImColor(0, 0, 255, 255);  // Blue
    POIList.push_back(fred);

    fred = new POI();
    fred->lat = 42;
    fred->lng = -75;
    fred->POI_TYPE = POI_POLYGON;
    fred->points = {{42, -75}, {42.5, -75.5}, {43, -75}};
    fred->filled = true;
    strncpy(fred->name, "Polygon", 40);
    fred->color = ImColor(255, 0, 255, 255);  // Purple
    POIList.push_back(fred);
}

void POI::Draw(ImVec2(*funcLatLon2VPxy)(double, double))
{
    int symbolRadius = 20;
    switch (POI_TYPE)
    {
	case POI_POINT:
	{
        ImVec2 POI_xy = funcLatLon2VPxy(lat, lng);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming FontAwesome is second font
		float fontSize = 20.0f;
		ImVec2 textSize = ImGui::CalcTextSize(POINT_ICONS[pointIconIndex]);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[1],fontSize, ImVec2((float)POI_xy.x - textSize.x/2, (float)POI_xy.y - textSize.y/2),
			color,POINT_ICONS[pointIconIndex] );
		ImGui::PopFont();
        DrawBoxedText(name, ImVec2((float)POI_xy.x+10, (float)POI_xy.y+10), color, color);

        break;
	}
    case POI_CIRCLE:
        DrawCircle(funcLatLon2VPxy, this);
        break;
    case POI_POLYLINE:
        DrawPolyline(funcLatLon2VPxy, this);
        break;
    case POI_POLYGON:
        DrawPolygon(funcLatLon2VPxy, this);
        break;
    }
}

void DrawPOIs(ImDrawList* draw_list, ImVec2(*funcLatLon2VPxy)(double, double))
{
    for (auto poi : POIList)
    {
        if (!poi->showOnMap) continue;
        poi->Draw(funcLatLon2VPxy);
    }

    if (g_DraftPOI == nullptr) return;

    if (POI_NONE != g_DraftPOI->POI_TYPE)
    {
        if (g_DraftPOI->showOnMap) g_DraftPOI->Draw(funcLatLon2VPxy);
    }


}

void DeleteSelectedPOIs()
{
    auto it = std::remove_if(POIList.begin(), POIList.end(),
        [](POI* poi) {
            if (poi->selected) {
                delete poi;
                return true;
            }
            return false;
        });
    POIList.erase(it, POIList.end());
}


int POI_NotifyMe(enum Mediator::NOTIFY_MSG m, void* data)
{
    if (m == Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION) 
    {
        ImVec2Double* d = (ImVec2Double*)data; 
        if (nullptr != g_DraftPOI)
        {
            g_DraftPOI->lat = d->x;
            g_DraftPOI->lng = d->y;
        
            if (b_addPointOnNotify) //these are the points of a polyline or polygon
            {
                g_DraftPOI->points.push_back({ d->x, d->y});
                return 0;
            }
        }
        b_AwaitingPosition = false;
    }
    return 0;
}


void GetMousePointPanel(POI* _POI, bool OneShot = true)
{
    ImGui::SeparatorText("POI Location");
    float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImGui::BeginChild("Location", ImVec2(0, 6.0f * TEXT_BASE_HEIGHT), true);

    if (b_AwaitingPosition)
    {
        newLat = g_MouseMngr.MouseLL.x;
        newLng = g_MouseMngr.MouseLL.y;
    }
    ImGui::Text("Latitude:"); ImGui::SameLine(80);
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputDouble("##Lat", &newLat, 0.1, 1.0, "%8.6f")) 
    {
        _POI->lat = newLat;
        b_AwaitingPosition = false;
    }

    ImGui::Text("Longitude:"); ImGui::SameLine(80);
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputDouble("##Lng", &newLng, 0.1, 1.0, "%8.6f"))
    {
        _POI->lng = newLng;
        b_AwaitingPosition = false;
    }

    if (false == b_AwaitingPosition)
    {
        ImGui::SameLine();ImGui::Text("Select point on map"); ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CROSSHAIRS))
        {
            b_AwaitingPosition = true;
            std::function<int(Mediator::NOTIFY_MSG, void*)> f = std::bind(&POI_NotifyMe, std::placeholders::_1, std::placeholders::_2);
            _m.SubScribeForNotifications(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION, f, OneShot);
            if (!OneShot) b_addPointOnNotify = true;
        }
    }
    else
    {
        ImGui::SameLine(); ImGui::Text("waiting for mouse click"); 
        ImGui::SameLine(); if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Cancel"))
        {
            if (b_AwaitingPosition) _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
            b_AwaitingPosition = false;
        }
    }

    ImGui::Text("%7.4f  %8.4f " ICON_FA_ARROW_RIGHT, _POI->lat, _POI->lng);
    static char _mgrsIn[40];
    long retVal = Convert_Geodetic_To_MGRS(DEG2RAD(newLat), DEG2RAD(newLng), g_MGRS_Precison, _mgrsIn);
    ImGui::SameLine();
    if (UTM_NO_ERROR == retVal) ImGui::Text(_mgrsIn);
    else ImGui::Text("MGRS Error");

    #pragma region MGRS
    static char mgrsInput[100] = "";
    ImGui::Text("MGRS:"); ImGui::SameLine(80);
    ImGui::SetNextItemWidth(150);
    bool enterPressed = ImGui::InputText("##MGRS", mgrsInput, IM_ARRAYSIZE(mgrsInput),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsUppercase);

    // Add small convert button right after the input
    ImGui::SameLine(0, 2);
    bool convertClicked = ImGui::Button(ICON_FA_SYNC "##convert", ImVec2(25, 0));
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Convert MGRS to Lat/Lon");
    }
    // Process conversion if either Enter was pressed or button was clicked
    if (enterPressed || convertClicked)
{
    if (strlen(mgrsInput) > 0)
    {
        // First remove all spaces
        char noSpaces[100] = "";
        int j = 0;
        for (int i = 0; mgrsInput[i]; i++)
        {
            if (!isspace(mgrsInput[i]))
            {
                noSpaces[j] = mgrsInput[i];
                j++;
            }
        }
        noSpaces[j] = '\0';
        strcpy(mgrsInput, noSpaces);  // Put back into mgrsInput

        // Store the original prefix (grid zone and letters)
        char prefix[100] = "";
        char numbers[100] = "";
        char* pNum = mgrsInput;

        // Find where numbers start
        while (*pNum && !isdigit(*pNum)) pNum++;

        // Copy the prefix (everything before the numbers)
        strncpy(prefix, mgrsInput, pNum - mgrsInput);
        prefix[pNum - mgrsInput] = '\0';

        // Copy just the numbers
        strcpy(numbers, pNum);
        size_t numLen = strlen(numbers);

        if (numLen == 4 || numLen == 6 || numLen == 8)
        {
            size_t eastingLen = numLen / 2;
            char easting[6] = "", northing[6] = "";

            // Split into easting and northing
            strncpy(easting, numbers, eastingLen);
            easting[eastingLen] = '\0';
            strcpy(northing, numbers + eastingLen);

            // Add zeros to make them 5 digits each
            while (strlen(easting) < 5) strcat(easting, "0");
            while (strlen(northing) < 5) strcat(northing, "0");

            // Reconstruct with original prefix
            snprintf(mgrsInput, sizeof(mgrsInput), "%s%s%s", prefix, easting, northing);
        }

        // Convert to lat/lon
        double lat_rad = 0.0, lon_rad = 0.0;
        long error = Convert_MGRS_To_Geodetic(mgrsInput, &lat_rad, &lon_rad);

        if (error == 0)
        {
            newLat = lat_rad * 180.0 / g_PI;
            newLng = lon_rad * 180.0 / g_PI;
        }
    }
}

    #pragma endregion

    ImGui::EndChild();
}




void DrawCircle(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi)
{
    ImVec2 center = funcLatLon2VPxy(poi->lat, poi->lng);

    // Calculate points for north and east to determine scale
    ImVec2 north = funcLatLon2VPxy(poi->lat + (poi->radiusNM / 60.0), poi->lng); // 1 degree = 60 nautical miles
    float pixelRadius = abs(north.y - center.y); // Use the vertical distance as radius

    ImGui::GetWindowDrawList()->AddCircleFilled(center, pixelRadius, poi->colorFill);
    ImGui::GetWindowDrawList()->AddCircle(center, pixelRadius, poi->color);
}

void DrawPolyline(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi)
{
    if (poi->points.size() < 2) return;

    std::vector<ImVec2> screenPoints;
    for (const auto& point : poi->points)
    {
        screenPoints.push_back(funcLatLon2VPxy(point.first, point.second));
    }

    for (size_t i = 0; i < screenPoints.size() - 1; i++)
    {
        ImGui::GetWindowDrawList()->AddLine(screenPoints[i], screenPoints[i + 1], poi->color, 2.0f);
    }


    int i = 0;
    char buf[20];
    if (poi->showLabel)
    {
        for (i = 0; i < screenPoints.size() - 1; i++)
        {
            ImGui::GetWindowDrawList()->AddLine(screenPoints[i], screenPoints[i + 1], poi->color, 2.0f);
            sprintf(buf, "%d", (int)i);
            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, buf);
            DrawBoxedText(buf, ImVec2(screenPoints[i].x - textSize.x / 2, screenPoints[i].y - textSize.y / 2), poi->color, poi->color);
        }
    }

    sprintf(buf, "%d", (int)screenPoints.size() - 1);
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, buf);
    DrawBoxedText(buf, ImVec2(screenPoints[i].x - textSize.x / 2, screenPoints[i].y - textSize.y / 2), poi->color, poi->color);


}

void DrawPolygon(ImVec2(*funcLatLon2VPxy)(double, double), POI* poi)
{
    if (poi->points.size() < 1) return;

    std::vector<ImVec2> screenPoints;
    for (const auto& point : poi->points)
    {
        screenPoints.push_back(funcLatLon2VPxy(point.first, point.second));
    }

    ImGui::GetWindowDrawList()->AddConvexPolyFilled(screenPoints.data(), static_cast<int>(screenPoints.size()), poi->colorFill);
    for (size_t i = 0; i < screenPoints.size(); i++)
    {
        size_t next = (i + 1) % screenPoints.size();
        ImGui::GetWindowDrawList()->AddLine(screenPoints[i], screenPoints[next], poi->color, 2.0f);
    }

    int i = 0;
    char buf[20];
    if (poi->showLabel)
    {
        for (i = 0; i < screenPoints.size() - 1; i++)
        {
            ImGui::GetWindowDrawList()->AddLine(screenPoints[i], screenPoints[i + 1], poi->color, 2.0f);
            sprintf(buf, "%d", (int)i);
            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, buf);
            DrawBoxedText(buf, ImVec2(screenPoints[i].x - textSize.x / 2, screenPoints[i].y - textSize.y / 2), poi->color, poi->color);
        }
    }

    sprintf(buf, "%d", (int)screenPoints.size() - 1);
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, buf);
    DrawBoxedText(buf, ImVec2(screenPoints[i].x - textSize.x / 2, screenPoints[i].y - textSize.y / 2), poi->color, poi->color);

}


static POIType selectedType = POI_POINT;
static int selectedIcon = 0;
std::string previewValue = std::string(POINT_ICONS[selectedIcon]) + " " + POINT_ICON_NAMES[selectedIcon];


void POICommonParams(POI* _POI, bool Edit = false)
{
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    if (Edit) flags |= ImGuiInputTextFlags_ReadOnly;

    #pragma region NameColourType
        ImGui::Text("POI Name:"); ImGui::SameLine(80);
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##Name", _POI->name, IM_ARRAYSIZE(_POI->name), flags);

        ImGui::SameLine(250);
        ImGui::Text("Colour:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(170);
        ImVec4 color = ImGui::ColorConvertU32ToFloat4(_POI->color);
        ImGui::ColorEdit4("##1", (float*)&color);
        _POI->color = ImGui::ColorConvertFloat4ToU32(color);

        ImGui::Text("Type:"); ImGui::SameLine(80);
        ImGui::SetNextItemWidth(150);
        const char* types[] = { "Point", "Circle", "Polyline", "Polygon" };
        if (Edit) ImGui::Text(types[_POI->POI_TYPE], flags);
        else ImGui::Combo("##Type", (int*)&_POI->POI_TYPE, types, IM_ARRAYSIZE(types), flags);
    #pragma  endregion
}

bool POI_PointTypes(POI* _POI, bool Edit = false)
{
    if (POI_POINT == _POI->POI_TYPE)
    {
        ImGui::SameLine(250); ImGui::Text("Icon:");
        ImGui::SameLine(305); ImGui::SetNextItemWidth(170);
        // Create a preview label that shows both icon and name
        std::string previewValue = std::string(POINT_ICONS[selectedIcon]) + " " + POINT_ICON_NAMES[selectedIcon];

        if (ImGui::BeginCombo("##PointIcon", previewValue.c_str()))
        {
            for (int i = 0; i < NUM_POINT_ICONS; i++)
            {
                const bool is_selected = (selectedIcon == i);
                std::string combo_label = std::string(POINT_ICONS[i]) + " " + POINT_ICON_NAMES[i];
                if (ImGui::Selectable(combo_label.c_str(), is_selected))
                {
                    selectedIcon = i;
                    _POI->pointIconIndex = i;
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (true == Edit) g_DraftPOI = _POI;
        GetMousePointPanel(_POI);
    }

    else if (POI_CIRCLE == _POI->POI_TYPE)
    {
        ImGui::SameLine(250);
        if (b_radiusInNM)
        {
            ImGui::Text("Radius [NM]:"); ImGui::SameLine(335);
            ImGui::SetNextItemWidth(110);
            ImGui::InputFloat("##Radius", &_POI->radiusNM, 0.1f, 1.0f, "%4.2f");
            ImGui::SameLine();
            if (ImGui::SmallButton("NM")) b_radiusInNM = false;
        }
        else
        {
            ImGui::Text("Radius [km]:"); ImGui::SameLine(335);
            ImGui::SetNextItemWidth(120);
            float km = _POI->radiusNM / 0.539957f;
            ImGui::InputFloat("##Radius", &km, 0.1f, 1.0f, "%.1f");
            _POI->radiusNM = km * 0.539957f;
            ImGui::SameLine();
            if (ImGui::SmallButton("km")) b_radiusInNM = true;
        }

        ImGui::Text("");
        ImGui::SameLine(250);

        ImGui::Text("Fill Colour:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(170);
        ImVec4 color = ImGui::ColorConvertU32ToFloat4(_POI->colorFill);
        ImGui::ColorEdit4("##2", (float*)&color);
        _POI->colorFill = ImGui::ColorConvertFloat4ToU32(color);
        if (true == Edit) g_DraftPOI = _POI;
        GetMousePointPanel(_POI);
    }



    if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Cancel"))
    {
        b_CreatingPOI = false;
        if (b_AwaitingPosition) _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        b_AwaitingPosition = false;
        return true;
    }

    ImGui::SameLine();

    if (false == Edit)
    {
        if (ImGui::Button(ICON_FA_SAVE " Save POI"))
        {
            _POI->showOnMap = true;
            POIList.push_back(_POI);
            InsertPOIRecord(_POI);
            _POI = nullptr;
            b_AwaitingPosition = false;
            b_CreatingPOI = false;
            _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        }
    }
    else
    {
        if (ImGui::Button(ICON_FA_SAVE " Update POI"))
        {
            deletePOI(_POI);
            _POI->showOnMap = true;
            //POIList.push_back(_POI);
            InsertPOIRecord(_POI);
            _POI = nullptr;
            b_AwaitingPosition = false;
            b_CreatingPOI = false;
            _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        }
    }
    return false;
}


bool POI_SegmentTypes(POI* _POI, bool Edit = false)
{
    ImGui::Checkbox("Label", &_POI->showLabel);
    if (POI_POLYGON == _POI->POI_TYPE)
    {
        ImGui::SameLine(250);
        ImGui::Text("Fill Colour:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(170);
        ImVec4 color = ImGui::ColorConvertU32ToFloat4(_POI->colorFill);
        ImGui::ColorEdit4("##2", (float*)&color);
        _POI->colorFill = ImGui::ColorConvertFloat4ToU32(color);
    }

    if (!Edit) GetMousePointPanel(_POI, false);

    ImGui::Text("Points:");
    ImGui::PushItemWidth(9.0f * ImGui::GetFontSize());
    for (size_t i = 0; i < _POI->points.size(); ++i)
    {
        ImGui::PushID((int)i);
        //ImGui::Text("(%f, %f)", _POI->points[i].first, _POI->points[i].second);
        ImGui::InputDouble("##lat", &_POI->points[i].first, 0.01, 0.0001, "%7.4f");
        ImGui::SameLine();
        ImGui::InputDouble("##lng", &_POI->points[i].second, 0.01, 0.0001, "%8.4f");
        ImGui::PopID();
    }
    ImGui::PopItemWidth();



    if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Cancel"))
    {
        if (!Edit)
        {
            _POI->points.clear();
            delete _POI;
            _POI = nullptr;
        }
        b_CreatingPOI = false;
        if (b_AwaitingPosition) _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        b_AwaitingPosition = false;
        return true;
    }

    ImGui::SameLine();

    if (false == Edit)
    {
        if (ImGui::Button(ICON_FA_SAVE " Save POI"))
        {
            _POI->showOnMap = true;
            POIList.push_back(_POI);
            InsertPOIRecord(_POI);
            _POI = nullptr;
            b_AwaitingPosition = false;
            b_CreatingPOI = false;
            _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        }
    }
    else
    {
        if (ImGui::Button(ICON_FA_SAVE " Update POI"))
        {
            deletePOI(_POI);
            _POI->showOnMap = true;
            //POIList.push_back(_POI);
            InsertPOIRecord(_POI);
            _POI = nullptr;
            b_AwaitingPosition = false;
            b_CreatingPOI = false;
            _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        }

    }

    return false;

}


bool ShowPOICreateEditPanel(POI *_POI, bool Edit = false)
{
    POICommonParams(_POI, Edit);

    if ((POI_POINT == _POI->POI_TYPE) || (POI_CIRCLE == _POI->POI_TYPE)) return POI_PointTypes(_POI, Edit);

    if (_POI->POI_TYPE == POI_POLYLINE || _POI->POI_TYPE == POI_POLYGON) return POI_SegmentTypes(_POI, Edit);

    return false;

}



int ShowEditPOIDialog(int index)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    static bool pOpen = false;
    ImGui::Begin("Edit POI", 0, flags);

    pOpen = ShowPOICreateEditPanel(POIList[index], true);
    
    ImGui::End();


    if (pOpen) return -1;
    return index;   
}



void ShowPOIListPanel()
{
    ImGui::SeparatorText("POI List");
    if (POIList.empty())
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            ICON_FA_INFO_CIRCLE " No POIs available. Create one using the form above.");
    }
    else
    {
        ImGui::BeginChild("POIList", ImVec2(0, 400), true);
        static int selected_POI = -1;
        int r = 0;
        for (auto poi : POIList)
        {
            ImGui::PushID(poi);

            // Main POI info
            ImGui::BeginGroup();

            ImGui::PushID(r);
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5.0f);
            ImGui::RadioButton("###", &selected_POI, r);
            ImGui::PopID();
            ImGui::SameLine();


            ImGui::Checkbox("##Show", &poi->showOnMap);
            ImGui::SameLine();

            // POI Type icon
            const char* icon;
            switch (poi->POI_TYPE) 
            {
                case POI_POINT: icon = ICON_FA_MAP_PIN; break;
                case POI_CIRCLE: icon = ICON_FA_CIRCLE; break;
                case POI_POLYLINE: icon = ICON_FA_WAVE_SQUARE; break;
                case POI_POLYGON: icon = ICON_FA_DRAW_POLYGON; break;
                default: icon = ICON_FA_QUESTION_CIRCLE;
            }

            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(poi->color), "%s %s", icon, poi->name);
            ImGui::SameLine();
            ImGui::TextDisabled("(%.4f, %.4f)", poi->lat, poi->lng);

            // Additional info based on type
            if (poi->POI_TYPE == POI_CIRCLE)
            {
                ImGui::SameLine();
                ImGui::Text("| Radius: %.1f NM", poi->radiusNM);
            }
            else if (poi->POI_TYPE == POI_POLYLINE || poi->POI_TYPE == POI_POLYGON)
            {
                ImGui::SameLine();
                ImGui::Text("| Points: %zu", poi->points.size());
            }
            ImGui::EndGroup();

            // Action buttons
            ImGui::SameLine(ImGui::GetWindowWidth() - 85);  // Adjusted position for single button
            if (ImGui::Button(ICON_FA_TRASH "##Delete", ImVec2(45, 0)))
            {
                // Delete from database
                deletePOI(poi);
                /*
                sqlite3* DB;
                if (sqlite3_open(POIDBFname, &DB) == SQLITE_OK) {
                    std::string deleteQuery = "DELETE FROM POI WHERE Name = '" +
                        std::string(poi->name) +
                        "' AND POILat = " + std::to_string(poi->lat) +
                        " AND POILon = " + std::to_string(poi->lng) + ";";

                    char* messageError;
                    if (sqlite3_exec(DB, deleteQuery.c_str(), NULL, 0, &messageError) != SQLITE_OK) {
                        sqlite3_free(messageError);
                    }
                    sqlite3_close(DB);
                }
                */
                // Mark for deletion
                poi->selected = true;
                DeleteSelectedPOIs();
                ImGui::PopID();
                break;  // Break since we modified the list
            }

            ImGui::Separator();
            ImGui::PopID();
            ++r;
        }
        ImGui::EndChild();

        //printf("Selected POI: %d\r\n", selected_POI);
        if (-1 != selected_POI) selected_POI = ShowEditPOIDialog(selected_POI);
    }
}



bool ShowPOIListDialog(bool* pOpen)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver); 
    ImGui::Begin("POI Management", pOpen, flags);

    // Database Controls Section
    {
        ImGui::BeginGroup();
        if (ImGui::Button(ICON_FA_DATABASE " Create POI DB", ImVec2(160, 0))) CreatePOITable(); 
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_DOWNLOAD " Load POI DB", ImVec2(160, 0))) LoadPOIListFromDB(); 
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLUS_CIRCLE " Example POIs", ImVec2(160, 0))) InsertExamplePOIs();
        ImGui::EndGroup();
    }

    ImGui::Separator();


    if (b_CreatingPOI)
    {
        ImGui::SeparatorText("Creating New POI");
        ShowPOICreateEditPanel(g_DraftPOI);
    }
    else  // show New POI button and POI List
    {
        if (ImGui::Button(ICON_FA_PLUS " New POI"))
        {
            g_DraftPOI = new POI();
            strncpy(g_DraftPOI->name, "POI_", sizeof(g_DraftPOI->name));
            g_DraftPOI->showOnMap = true;
            g_DraftPOI->points.clear();
            b_CreatingPOI = true;
        }
        ShowPOIListPanel();
    }

    ImGui::End();


    if (*pOpen == false)
    {
        // Clear form
        if (nullptr != g_DraftPOI)
        {
            g_DraftPOI->points.clear();
            delete g_DraftPOI;
            g_DraftPOI = nullptr;
        }   
        b_CreatingPOI = false;
        if (b_AwaitingPosition) _m.CancelSubScription(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION);
        b_AwaitingPosition = false;
    }
    return true;
}
