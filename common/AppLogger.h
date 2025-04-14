#pragma once

#include <vector>

#include <imgui.h>
#include <string>


void HelpMarker(const char* desc);


void LogToFile(std::string logMsg);



// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct AppLog
{
    std::vector<std::string> lines;
    //ImGuiTextBuffer     Buf;
    //ImGuiTextFilter     Filter;
    //ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    //bool                AutoScroll;  // Keep scrolling if already at the bottom.

    AppLog();
    void    Clear();
    void    _AddLog(const char* fmt);
    void    AddLog(const char* fmt, ...);
    void    Draw(const char* title, bool* p_open = NULL);
};



