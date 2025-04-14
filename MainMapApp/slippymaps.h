#pragma once

#include <string>
#include <tuple>
#include <sstream>

#include "Image.h"

#include "ImVec2Double.h"
#include "tileserver.h"


// Useful Links and Resources
//
// https://operations.osmfoundation.org/policies/tiles/
// https://wiki.openstreetmap.org/wiki/Tile_servers
// https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames

#define TILE_SERVER   "https://a.tile.openstreetmap.org/" // the tile map server url
#define TILE_SIZE     256                                 // the expected size of tiles in pixels, e.g. 256x256px
#define MAX_ZOOM      21                                  // the maximum zoom level provided by the server
#define MAX_THREADS   2                                   // the maximum threads to use for downloading tiles (OSC strictly forbids more than 2)
#define USER_AGENT    "ImMaps/0.1"                        // change this to represent your own app if you extend this code

#define PI 3.14159265359


int long2tilex(double lon, int z);
int lat2tiley(double lat, int z);
double tilex2long(int x, int z);
double tiley2lat(int y, int z);







struct TileCoord {
    int z; // zoom    [0......20]
    int x; // x index [0...z^2-1]
    int y; // y index [0...z^2-1]


    //from https://github.com/Murthy10/cppGeoTile/blob/master/src/tile.cpp
    inline std::string getQuadTree(int tmsX_, int tmsY_, int zoom_) const
    {
        int tmsX = tmsX_;
        //int tmsY = (1 << zoom_) - 1 - tmsY_;
        int tmsY = tmsY_;

        std::stringstream strStream;
        for (int i = zoom_; i > 0; i--) {
            auto digit = 0;
            auto maks = 1 << (i - 1);
            if ((tmsX & maks) != 0)
            {
                digit += 1;
            }
            if ((tmsY & maks) != 0)
            {
                digit += 2;
            }
            strStream << digit;
        }
        return strStream.str();
    }



    inline std::string subdir() const { return std::to_string(z) + "/" + std::to_string(x) + "/"; }

    inline std::string dir(TileServer* ts) const { return ts->TileFolder + subdir(); }
    inline std::string file(TileServer* ts) const { return std::to_string(y) + ts->extension; } // ".png"; }
    inline std::string path(TileServer* ts) const { return dir(ts) + file(ts); }
    inline std::string label() const { return subdir() + std::to_string(y); }

    inline std::string url(TileServer* ts) const { return ts->URL + subdir() + file(ts); }
    inline std::string googleurl(TileServer* ts) const { return ts->URL + "&x=" + std::to_string(x) + "&y=" + std::to_string(y) + "&z=" + std::to_string(z); }
    inline std::string esriurl(TileServer* ts) const { return ts->URL + std::to_string(z) + "/" + std::to_string(y) + "/" + std::to_string(x) + ts->extension; }
    inline std::string bingurl(TileServer* ts,std::string suf) const
    {
        std::string ur = getQuadTree(x, y, z);
        std::string u = ts->URL + ur + suf;//"?g=761&amp;mkt=en-us";
        //printf("%s\r\n", u.c_str());
        return u;
    }

    /*
    inline std::string dir() const { return "tiles/" + subdir(); }
    inline std::string file() const { return std::to_string(y) + ".png"; }
    inline std::string path() const { return dir() + file(); }
    inline std::string url() const { return TILE_SERVER + subdir() + file(); }
    inline std::string label() const { return subdir() + std::to_string(y); }
    */
    std::tuple<ImVec2Double, ImVec2Double> bounds() const {
        double n = std::pow(2, z);
        double t = 1.0 / n;
        return {
                   { x * t     , (1 + y) * t } ,
                   { (1 + x) * t , (y)*t   }
        };
    }
};

bool operator<(const TileCoord& l, const TileCoord& r);

enum TileState : int {
    Unavailable = 0, // tile not available
    Loaded,          // tile has been loaded into  memory
    Downloading,     // tile is downloading from server
    OnDisk           // tile is saved to disk, but not loaded into memory
};


typedef Image TileImage;



struct Tile {
    Tile() : state(TileState::Unavailable) {  }
    Tile(TileState s) : state(s) { }
    TileState state;
    TileImage image;
};

size_t curl_write_cb(void* ptr, size_t size, size_t nmemb, void* userdata);



/* The following functions take their parameter and return their result in degrees */
//double y2lat_d(double y) { return RAD2DEG(atan(exp(DEG2RAD(y))) * 2 - M_PI / 2); }
//double x2lon_d(double x) { return x; }

//double lat2y_d(double lat) { return RAD2DEG(log(tan(DEG2RAD(lat) / 2 + M_PI / 4))); }
//double lon2x_d(double lon) { return lon; }

/* The following functions take their parameter in something close to meters, along the equator, and return their result in degrees */
double ProjYtoLat(double y);
double ProjX2Lng(double x);

/* The following functions take their parameter in degrees, and return their result in something close to meters, along the equator */
double Lat2ProjY(double lat);
double Lng2ProjX(double lon);

ImVec2Double ProjYX2LatLng(double y, double x);
ImVec2Double LatLng2ProjYX(double lat, double lng);


