#pragma once

#include "geodesic.h"
#include "ImVec2Double.h"

#define METER_PER_NM 1852
static double geod_a = 6378137;
static double geod_f = 1 / 298.257223563; /* WGS84 */
static struct geod_geodesic geod;

ImVec2Double GetBearingAndRangeInNMBetweenTwoGeoPoints(double lat1, double lon1, double lat2, double lon2);
double CalcEndPointFromRangeInMetresAndBearing(double lat1, double lon1, double azi1, double s12, double& lat2, double& lon2);  //s12 is in metres

double CalcDistanceInMetresFromSpeedAndTime(double velInKnots, double travelTimeInSeconds); //vel in knots (1 Kt=1852 m/hr) or 30.8666m/minute or .5144444m per second


