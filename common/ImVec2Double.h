#pragma once

#include "ImGui.h"


struct ImVec2Double {
    double x, y;
    constexpr ImVec2Double() : x(0.0), y(0.0) { }
    constexpr ImVec2Double(double _x, double _y) : x(_x), y(_y) { }
    constexpr ImVec2Double(const ImVec2& p) : x((double)p.x), y((double)p.y) { }
    double& operator[] (size_t idx) { IM_ASSERT(idx == 0 || idx == 1); return ((double*)(void*)(char*)this)[idx]; }
    double  operator[] (size_t idx) const { IM_ASSERT(idx == 0 || idx == 1); return ((const double*)(const void*)(const char*)this)[idx]; }
};


// Range defined by a min/max value.
struct ImPlotRangeDouble {
    double Min, Max;
    constexpr ImPlotRangeDouble() : Min(0.0), Max(0.0) { }
    constexpr ImPlotRangeDouble(double _min, double _max) : Min(_min), Max(_max) { }
    bool Contains(double value) const { return value >= Min && value <= Max; }
    double Size() const { return Max - Min; }
    double Clamp(double value) const { return (value < Min) ? Min : (value > Max) ? Max : value; }
};


// Combination of two range limits for X and Y axes. Also an AABB defined by Min()/Max().
struct ImPlotRectDouble {
    ImPlotRangeDouble X, Y;
    constexpr ImPlotRectDouble() : X(0.0, 0.0), Y(0.0, 0.0) { }
    constexpr ImPlotRectDouble(double x_min, double x_max, double y_min, double y_max) : X(x_min, x_max), Y(y_min, y_max) { }
    bool Contains(const ImVec2Double& p) const { return Contains(p.x, p.y); }
    bool Contains(double x, double y) const { return X.Contains(x) && Y.Contains(y); }
    ImVec2Double Size() const { return ImVec2Double(X.Size(), Y.Size()); }
    ImVec2Double Clamp(const ImVec2Double& p) { return Clamp(p.x, p.y); }
    ImVec2Double Clamp(double x, double y) { return ImVec2Double(X.Clamp(x), Y.Clamp(y)); }
    ImVec2Double Min() const { return ImVec2Double(X.Min, Y.Min); }
    ImVec2Double Max() const { return ImVec2Double(X.Max, Y.Max); }
};


