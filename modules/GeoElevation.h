#pragma once

#include "ImGui.h"

void ShowDTEDCellInfo();
void DrawDTEDLOS(ImVec2(*funcLatLon2VPxy)(double, double));

void LoadDTEDTiles();

void ElevationLeftClickMenu(bool & mb_LeftClickMenuOpen, ImVec2Double& m_LeftClickLocationLL);

void CheckElevationKeysPressed();

void DrawElevationOverlay(ImVec2(*LatLon2VPxy) (double, double));


bool ElevationTileWindow(bool pOpen);
bool ElevationProfileWindow();


int heightAt(double lat, double lng, int& i, int& j);

int heightAt(ImVec2Double p);
