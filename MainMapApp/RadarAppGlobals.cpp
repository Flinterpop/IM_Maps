
#include "Mediator.h"

#include "imgui.h"
Mediator _m;


bool glob_bPlacards = true;
bool glob_bHAFUSymbols = true;

bool gb_RBL_NM = true;
int g_MGRS_Precison = 5;

bool glob_bInTest = false;
bool glob_bDebug = false;


float glob_LocateLineWidth = 1.0f;
ImU32 g_LocateLineColour = ImColor(0X67, 0X67, 0xAB, 255);
ImU32 g_ACTrailColour = ImColor(0, 0, 0, 255);




//other globals are in AircraftControl.cpp

