#pragma once


#include <filesystem>

#include "imgui.h"
#include <string>
#include "windows.h"

//#include "SimAircraft.h"


constexpr double  g_PI = 3.14159265358979323846;

#define DEG2RAD(a)   ((a) / (180 / g_PI))
#define RAD2DEG(a)   ((a) * (180 / g_PI))
constexpr double EARTH_RADIUS = 6378137;  //metres

namespace fs = std::filesystem;

std::uintmax_t calculateTotalFileSize(const fs::path& directory);

//void DrawMapPin(ImVec2(*LatLon2VPxy) (double, double), ImVec2Double PinLocationLL, ImU32 PointColour, char* optionalLabel = 0);

void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs);

int GetSecondsSinceMidnightZulu();

void MGRSPretty(char* MGRS, char* MGRSPretty);

void remove_spaces(char* s);

struct ImVec2bg
{
    float                                   x, y;
    constexpr ImVec2bg() : x(0.0f), y(0.0f) {}
    constexpr ImVec2bg(float _x, float _y) : x(_x), y(_y) {}
    float& operator[] (size_t idx) { IM_ASSERT(idx == 0 || idx == 1); return ((float*)(void*)(char*)this)[idx]; } // We very rarely use this [] operator, so the assert overhead is fine.
    float  operator[] (size_t idx) const { IM_ASSERT(idx == 0 || idx == 1); return ((const float*)(const void*)(const char*)this)[idx]; }
    ImVec2bg& operator+=(const ImVec2bg& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

};

void DrawMGRSGrid(char* MGRS, ImVec2(*LatLon2VPxy) (double, double), int numSides);
void DrawMGRSGrid(char* MGRS, ImVec2(*LatLon2VPxy) (double, double));

void RoundButton(const char* label, ImVec2& textPos);
void DrawBoxedText(const char* label, ImVec2& textPos, ImU32 FGColour = ImColor(0, 0, 0, 255), ImU32 TextColour = ImColor(0, 0, 0, 255), ImU32 BGColour = ImColor(255, 255, 255, 255),float fontScale = 0.8f);


bool RedButton(char* name, const ImVec2& size = ImVec2(0, 0));
bool GreenButton(char* name, const ImVec2& size = ImVec2(0, 0));
bool StopStartToggleButton(char* name, bool& b_toggle);


void HelpMarker(const char* desc);


void UnixTimeToFileTime(time_t t, LPFILETIME pft);


void ImGui_TextRight(std::string text, int extra = 0);
void ImGui_TextCentre(std::string text);

void HexDump(int bufLen, unsigned char* buf);


//void DrawRBL(ImVec2(*LatLon2VPxy) (double, double), SimAircraft* ac, ImVec2 tgtLL, ImColor RBL_colour);

