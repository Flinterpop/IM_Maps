

#include <fstream>  //for time

#include "AppLogger.h"


void _HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


static inline std::string getCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80] = "";
    tstruct = *localtime(&now);
    if (s == "now") strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date") strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};



void LogToFile(std::string logMsg)
{
    std::string filePath = "RadarSites_" + getCurrentDateTime("date") + ".log";
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();

}



// Usage:
//  static AppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");

    AppLog::AppLog()
    {
        Clear();
    }

    void    AppLog::Clear()
    {
        lines.clear();
    }

    void    AppLog::_AddLog(const char *line)
    {
        if (lines.size() > 500) lines.clear();
        lines.emplace_back(std::string(line));
    }
    
    void    AppLog::AddLog(const char* fmt, ...)
    {
        if (lines.size() > 500) lines.clear();
        
        va_list args;
        va_start(args, fmt);
        char buf[200];
        vsprintf(buf, fmt, args);
        lines.emplace_back(std::string(buf));
        va_end(args);
    }


    void AppLog::Draw(const char* title, bool* p_open)
    {

        ImGui::Begin(title, p_open);

        // Main window
        if (ImGui::Button("Clear")) Clear();
        //ImGui::SameLine();
        //bool copy = ImGui::Button("Copy");

        ImGui::Separator();

        for (auto s : lines)
        {
            ImGui::TextWrapped(s.c_str());
        }


       
        ImGui::End();

    }

AppLog MyLog;



