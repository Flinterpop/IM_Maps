#include "windows.h"
#include <chrono>

#include "imgui.h"
#include "fonts\IconsFontAwesome5.h"

#include <string>
#include "stdio.h"




class UpTimeTracker
{
public:
    long long startTime;
    bool started = false;

    char m_StartTimeS[40];

    UpTimeTracker()
    {
        // Example of the very popular RFC 3339 format UTC time
        std::time_t time = std::time({});
        strftime(m_StartTimeS, 40, "Start DTG: %d %b %Y %Tz", std::gmtime(&time));
        printf("Application Starting at %s\r\n", m_StartTimeS);

        //startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        started = true;
    }

public:
    long long getUptimeString(char * buf)
    {
        if (false == started)
        {
            printf("Uptimer not initialized\r\n");
            return 0;
        }

        //long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        long long uptimeSecs = timeSinceEpoch - startTime;
        //int _secs = (int)(uptimeSecs / 1000);
        int _secs = (int)uptimeSecs;

        int hours = _secs / 3600;
        int mins = (_secs % 3600) / 60;
        int secs = (int)(_secs - (hours * 3600) - (mins * 60));
        sprintf(buf, "Uptime: %02d:%02d:%02d", hours, mins, secs);

        return timeSinceEpoch - startTime;
    }

    char * printStartTimeString()
    {
        if (false == started)
        {
            printf("Start Timer not initialized\r\n");
            return 0;
        }
        return m_StartTimeS;
    }



};


UpTimeTracker _UpTimeTracker;

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string.
// Useful for quick combo boxes where the choices are known locally.
bool ShowStyleSelector(const char* label)
{
    static int style_idx = 1;
    if (ImGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
    {
        switch (style_idx)
        {
        case 0: ImGui::StyleColorsDark(); break;
        case 1: ImGui::StyleColorsLight(); break;
        case 2: ImGui::StyleColorsClassic(); break;
        }
        return true;
    }
    return false;
}


void ShowAbout(bool *bAboutWindow)
{
    ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("About", bAboutWindow,_flags);
    
    //ImGui::PushItemWidth(35.0f * ImGui::GetFontSize());
        static const char* label = ICON_FA_BUG " RadarSites - " __DATE__;
        ImGui::Text(label);
        ImGui::Text("Written by B. Graham with contributions by B. Hook");
        ImGui::Text("Based on ImGui "); ImGui::SameLine(); ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui", "https://github.com/ocornut/imgui");
        ImGui::Text("Mapping code derived from"); ImGui::SameLine();ImGui::TextLinkOpenURL("https://github.com/epezent/implot_demos", "https://github.com/epezent/implot_demos");     
        ImGui::Text("MGRS derived from"); ImGui::SameLine();ImGui::TextLinkOpenURL("https://github.com/hobuinc/mgrs", "https://github.com/hobuinc/mgrs");

        ImGui::Separator();
//    ImGui::PopItemWidth();


    ImGui::PushItemWidth(12.0f * ImGui::GetFontSize());
        if (ShowStyleSelector("Colors##Selector"))
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.FrameBorderSize = 1.0;
            //ref_saved_style = style;
        }
        ImGui::ShowFontSelector("Fonts##Selector");
    ImGui::PopItemWidth();
    

    ImGui::TextWrapped("This is the third version of a Radar App for use with TDL App integration");
    ImGui::TextWrapped("It uses Web Mercator which has a variable scale: the horizontal scale changes with latitude.");
    ImGui::TextWrapped("At the equator a degree of latitude and longitude is 60nm. As latitude increase toward the poles the length of 1 degree of longitude decreases.");
    ImGui::TextWrapped("Web Mercator preserves angles at small scales. So range circles remain circular (as opposed to becoming eliptical)");
    ImGui::Separator();
    ImGui::TextWrapped("MGRS Precision (1 thru 5) 5 digits = 1m, 4 digits = 10m , 3 digits = 100m, 2 digits = 1km, 1 digit = 10km  ");
    ImGui::Separator();
    ImGui::TextWrapped("ASTERIX CAT 21 are IAW latest V2.6 which is not compatible with V2.1 or earlier as used by Sitaware.");
    ImGui::TextWrapped("");

    ImGui::Separator();

    
    
    ImGui::Text(_UpTimeTracker.printStartTimeString());
    char utime[40];
    auto ut = _UpTimeTracker.getUptimeString(utime);
    ImGui::Text(utime);

    if (ImGui::Button("Close")) *bAboutWindow = false;

    ImGui::End();
}
