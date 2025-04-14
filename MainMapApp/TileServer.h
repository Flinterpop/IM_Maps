#pragma once


#include<string>
#include<vector>


typedef struct aTileServer {
public:
    std::string name = "OSM";
    bool GoogleAPI = false;
    bool ESRIAPI = false;
    int maxZoom = 19;
    std::string extension = ".png";
    std::string URL = "https://a.tile.openstreetmap.org/";
    std::string suffix = "";
    std::string TileFolder = "osm/";
    std::string APIKey;
    bool needsAPIKey = false;
    std::string CopyRight = "";
    bool ARCGISOrder = false;
    bool QuadKey = false;
} TileServer;


