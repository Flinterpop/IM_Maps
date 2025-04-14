#pragma once

#include "route.h"

void api_CreateRouteTables();
void api_LoadRouteListFromDB();
int api_SaveNewRoute(route* r);
bool api_LoadWaypointsForRouteFromDB(route* r);
void api_ClearRoutesDB();

//route* api_LoadRoute(char* rname);
bool api_DeleteRoute(int Route_DB_ID);
int api_UpdateRoute(route* r);