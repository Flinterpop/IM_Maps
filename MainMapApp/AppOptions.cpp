#include "imgui.h"
#include "fonts\IconsFontAwesome5.h"
#include "ImBGutil.h"
#include "maps.h"

extern bool glob_bInTest;

extern bool gb_RBL_NM;
extern int g_MGRS_Precison ;

extern bool glob_bPlacards;
extern bool glob_bHAFUSymbols;

extern float glob_LocateLineWidth;
extern ImU32 g_LocateLineColour;
extern ImU32 g_ACTrailColour;

void Maps::ShowAppOptions(bool *bOptionsWindow)
{
    ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Application Level Options ", bOptionsWindow,_flags);
    
    static const char* label = ICON_FA_BUG " RadarSites - " __DATE__;
    
    ImGui::Text(label);

    ImGui::Checkbox("RBL in NM or km", &gb_RBL_NM);
    ImGui::Checkbox("Tile Outlines", &mb_tileOutlines);
    ImGui::Checkbox("Tile Coords", &mb_tileCoords);
    ImGui::Checkbox("Big X", &mb_ShowBigX);
    ImGui::Checkbox("Test Mode", &glob_bInTest);

    ImGui::Checkbox("Placards", &glob_bPlacards);ImGui::SameLine();HelpMarker("Track Info Ovelerlays have rectangular background, or not.");
    ImGui::Checkbox("Ident Symbols", &glob_bHAFUSymbols);ImGui::SameLine();HelpMarker("Unique Symbols based on Identification");

    ImGui::PushItemWidth(5.0f * ImGui::GetFontSize());
    if (ImGui::InputInt("MGRS Precision (1-5)", &g_MGRS_Precison)) { g_MGRS_Precison = std::clamp(g_MGRS_Precison, 1, 5); }
    ImGui::PopItemWidth();


    ImVec4 llc = ImGui::ColorConvertU32ToFloat4(g_LocateLineColour);
    if (ImGui::ColorEdit4("Locate Line Colour", (float*)&llc)) g_LocateLineColour = ImGui::ColorConvertFloat4ToU32(llc);
    ImGui::SetNextItemWidth(5.0f * ImGui::GetFontSize());
    if (ImGui::InputFloat("Locate Line Width (0.2 - 2.0)", &glob_LocateLineWidth, 0.1f, 1.0f, "%.1f")) { glob_LocateLineWidth = std::clamp(glob_LocateLineWidth, 0.2f, 5.0f); }

    llc = ImGui::ColorConvertU32ToFloat4(g_ACTrailColour);
    if (ImGui::ColorEdit4("Track Trail Line Colour", (float*)&llc)) g_ACTrailColour = ImGui::ColorConvertFloat4ToU32(llc);

    llc = ImGui::ColorConvertU32ToFloat4(g_MapBackgroundColour);
    if (ImGui::ColorEdit4("Map Background Colour", (float*)&llc)) g_MapBackgroundColour = ImGui::ColorConvertFloat4ToU32(llc);


    if (ImGui::Button("Texture Test")) {
        LoadACImages();
    }

    ImGui::Separator();
    if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Close")) *bOptionsWindow = false;

    ImGui::End();
}

