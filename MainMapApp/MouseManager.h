#pragma once
#include "imgui.h"
#include "ImVec2Double.h"

struct _MouseManager
{
	ImVec2(*FPLatLngToVPxy) (double, double);

	ImVec2Double m_LatLongOfLeftSingleClick;
	char m_MGRSofLeftSingleClick[25];

	ImVec2Double m_LatLongOfLeftDoubleClick;
	char m_MGRSofLeftDoubleClick[25];

	bool WantALocation = false;
	bool WantedLocationIsMarked = false;

	ImVec2Double MouseLL;
	int mouse_x;
	int mouse_y;
	
	void MouseOverMapClickHandler(ImVec2Double _MouseLL);


};


