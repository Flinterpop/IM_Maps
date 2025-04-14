
#include "ImVec2Double.h"
#include "geocalcs.h"


ImVec2Double GetBearingAndRangeInNMBetweenTwoGeoPoints(double lat1, double lon1, double lat2, double lon2)
{
    geod_init(&geod, geod_a, geod_f);
    double ps12; //range from pt1 to pt2
    double pazi1;//azimuth to pt2 from pt1
    double pazi2;//azimuth to pt1 from pt2
    geod_inverse(&geod, lat1, lon1, lat2, lon2, &ps12, &pazi1, &pazi2);
    if (pazi1 < 0) pazi1 += 360;
    return ImVec2Double(ps12 / METER_PER_NM, pazi1); //return range in nm and Bearing from pt1 to pt2
}



double CalcEndPointFromRangeInMetresAndBearing(double lat1, double lon1, double azi1, double s12, double& lat2, double& lon2)  //s12 is in metres
{
    geod_init(&geod, geod_a, geod_f);
    double azi2;
    geod_direct(&geod, lat1, lon1, azi1, s12, &lat2, &lon2, &azi2);
    return azi2;
}


double CalcDistanceInMetresFromSpeedAndTime(double velInKnots, double travelTimeInSeconds) //vel in knots (1 Kt=1852 m/hr) or 30.8666m/minute or .5144444m per second
{
    return travelTimeInSeconds * velInKnots * METER_PER_NM / 3600;
}




