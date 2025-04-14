
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION   //required to instantiate image.h
#include "slippymaps.h"


int long2tilex(double lon, int z) {
	return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z) {
	double latrad = lat * PI / 180.0;
	return (int)(floor((1.0 - asinh(tan(latrad)) / PI) / 2.0 * (1 << z)));
}

double tilex2long(int x, int z) {
	return x / (double)(1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z) {
	double n = PI - 2.0 * PI * y / (double)(1 << z);
	return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}



bool operator<(const TileCoord& l, const TileCoord& r) {
    if (l.z < r.z)  return true;
    if (l.z > r.z)  return false;
    if (l.x < r.x)  return true;
    if (l.x > r.x)  return false;
    if (l.y < r.y)  return true;
    if (l.y > r.y)  return false;
    return false;
}




size_t curl_write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    FILE* stream = (FILE*)userdata;
    if (!stream) {
        printf("No stream\n");
        return 0;
    }
    size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
    return written;
}




// Mercator Projection to/from Lat/Lon
#define M_PI       3.14159265358979323846   // pi

//from https://wiki.openstreetmap.org/wiki/Mercator#C

#define DEG2RAD(a)   ((a) / (180 / M_PI))
#define RAD2DEG(a)   ((a) * (180 / M_PI))
#define EARTH_RADIUS 6378137


/* The following functions take their parameter and return their result in degrees */
//double y2lat_d(double y) { return RAD2DEG(atan(exp(DEG2RAD(y))) * 2 - M_PI / 2); }
//double x2lon_d(double x) { return x; }

//double lat2y_d(double lat) { return RAD2DEG(log(tan(DEG2RAD(lat) / 2 + M_PI / 4))); }
//double lon2x_d(double lon) { return lon; }

/* The following functions take their parameter in something close to meters, along the equator, and return their result in degrees */
//double y2lat_m(double y) { return RAD2DEG(2 * atan(exp(y / EARTH_RADIUS)) - M_PI / 2); }
//double x2lon_m(double x) { return RAD2DEG(x / EARTH_RADIUS); }
//renamed
double ProjYtoLat(double y) { return RAD2DEG(2 * atan(exp(y / EARTH_RADIUS)) - M_PI / 2); }
double ProjX2Lng(double x) { return RAD2DEG(x / EARTH_RADIUS); }
ImVec2Double ProjYX2LatLng(double y, double x) { return ImVec2Double(RAD2DEG(2 * atan(exp(y / EARTH_RADIUS)) - M_PI / 2), RAD2DEG(x / EARTH_RADIUS)); }


/* The following functions take their parameter in degrees, and return their result in something close to meters, along the equator */
//double lat2y_m(double lat) { return log(tan(DEG2RAD(lat) / 2 + M_PI / 4)) * EARTH_RADIUS; }
//double lon2x_m(double lon) { return          DEG2RAD(lon) * EARTH_RADIUS; }
//rename
double Lat2ProjY(double lat) { return log(tan(DEG2RAD(lat) / 2 + M_PI / 4)) * EARTH_RADIUS; }
double Lng2ProjX(double lng) { return          DEG2RAD(lng) * EARTH_RADIUS; }
ImVec2Double LatLng2ProjYX(double lat, double lng) { return ImVec2Double(log(tan(DEG2RAD(lat) / 2 + M_PI / 4)) * EARTH_RADIUS, DEG2RAD(lng) * EARTH_RADIUS); }

