#include <filesystem>

#include <iostream>
#include <unordered_map>

#include "stdio.h"
#include "tilemanager.h"

#include "fonts/IconsFontAwesome5.h"


#include "ImBGUtil.h"

//extern bool gb_NoMap;


void TileManager::CreateMapTileServerList()
{
    TileServer osm;
    //https://api.mapbox.com/v4/mapbox.terrain-rgb/2/2/2.pngraw?access_token=pk.eyJ1IjoiZ3JhaGFtYm0iLCJhIjoiY2xrZXRqcnozMTU1NzNycnA1ZHB5bmdwNSJ9.8N5c2c7BKsj8Cz4Esk0xuA

    osm.name = "OSM";
    osm.maxZoom = 19;
    osm.extension = ".png";
    osm.URL = "https://a.tile.openstreetmap.org/";
    osm.TileFolder = "tiles/osm/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "OpenStreetMap Contributors";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    //example https://api.mapbox.com/v4/mapbox.terrain-rgb/7/39/40.pngraw?access_token=pk.eyJ1IjoiZ3JhaGFtYm0iLCJhIjoiY2xrZXRqcnozMTU1NzNycnA1ZHB5bmdwNSJ9.8N5c2c7BKsj8Cz4Esk0xuA

    osm.name = "MapBox";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://api.mapbox.com/v4/mapbox.terrain-rgb/";
    osm.TileFolder = "tiles/MapBox/";
    osm.APIKey = "raw?access_token=pk.eyJ1IjoiZ3JhaGFtYm0iLCJhIjoiY2xrZXRqcnozMTU1NzNycnA1ZHB5bmdwNSJ9.8N5c2c7BKsj8Cz4Esk0xuA";
    osm.needsAPIKey = true;
    osm.CopyRight = ICON_FA_COPYRIGHT "MapBox Contributors";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    // example https://api.tiles.openaip.net/api/data/openaip/7/41/40.png?raw?access_token=80c935bcfc58b850664f1d64b1062b64

    //https://api.tiles.openaip.net/api/data/openaip/7/41/40.png?apikey=2cd3d2b8e49247edbd07d779bd793322
    //https://api.tiles.openaip.net/api/data/openaip/7/37/45.png?apiKey=80c935bcfc58b850664f1d64b1062b64
    osm.name = "OpenAIP";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://api.tiles.openaip.net/api/data/openaip/";
    osm.TileFolder = "tiles/OpenAIP/";
    osm.APIKey = "?apiKey=80c935bcfc58b850664f1d64b1062b64";
    osm.needsAPIKey = true;
    osm.CopyRight = ICON_FA_COPYRIGHT "OpenAIP";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);
    
    
    //test URL "https://a.tile.opentopomap.org/1/1/1.png"
    osm.name = "OTM";
    osm.maxZoom = 19;
    osm.extension = ".png";
    osm.URL = "https://a.tile.opentopomap.org/";
    osm.TileFolder = "tiles/otm/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "OpenStreetMap Contributors";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    /*
    //Notes on STAMEN Maps  http://maps.stamen.com/#terrain-lines/11/45.2094/-75.7839
    osm.name = "Watercolor";
    osm.maxZoom = 18;
    osm.extension = ".jpg";
    osm.URL = "https://stamen-tiles.a.ssl.fastly.net/watercolor/";
    osm.TileFolder = "tiles/watercolor/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Map tiles by Stamen Design, under CC BY 3.0. Data by OpenStreetMap, under ODbL.";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    osm.name = "Terrain";
    osm.maxZoom = 19;
    osm.extension = ".jpg";
    osm.URL = "https://stamen-tiles.a.ssl.fastly.net/terrain/";
    osm.TileFolder = "tiles/terrain/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Map tiles by Stamen Design, under CC BY 3.0. Data by OpenStreetMap, under ODbL.";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    osm.name = "Toner";
    osm.maxZoom = 19;
    osm.extension = ".png";
    osm.URL = "https://stamen-tiles.a.ssl.fastly.net/toner/";
    osm.TileFolder = "tiles/toner/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Map tiles by Stamen Design, under CC BY 3.0. Data by OpenStreetMap, under ODbL.";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);
    */



    //Notes on these servers:   https://www.thunderforest.com/docs/map-tiles-api/
    // 
    //test URL "https://tile.thunderforest.com/landscape/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "Landscape";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/landscape/";
    osm.TileFolder = "tiles/landscape/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    //test URL "https://tile.thunderforest.com/cycle/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "OpenCycle";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/cycle/";
    osm.TileFolder = "tiles/cycle/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    //test URL "https://tile.thunderforest.com/transport/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "Transport";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/transport/";
    osm.TileFolder = "tiles/transport/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    osm.name = "Transport Dark";
	osm.maxZoom = 22;
	osm.extension = ".png";
	osm.URL = "https://tile.thunderforest.com/transport-dark/";
	osm.TileFolder = "tiles/transport_dark/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
	osm.needsAPIKey = true;
	osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
	osm.GoogleAPI = false;
	osm.ESRIAPI = false;
	glob_TileServerList.push_back(osm);

    //test URL "https://tile.thunderforest.com/outdoors/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "Outdoors";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/outdoors/";
    osm.TileFolder = "tiles/outdoors/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    //test URL "https://tile.thunderforest.com/atlas/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "Atlas";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/atlas/";
    osm.TileFolder = "tiles/atlas/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    //test URL "https://tile.thunderforest.com/neighbourhood/1/1/1.png?apikey=2cd3d2b8e49247edbd07d779bd793322"
    osm.name = "Neighbourhood";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://tile.thunderforest.com/neighbourhood/";
    osm.TileFolder = "tiles/neighbourhood/";
    osm.APIKey = "?apikey=2cd3d2b8e49247edbd07d779bd793322";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " Thunderforest";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    //ESRI
    // Details of feed: https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer
    // 
    //test URL "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/1/1/1.jpg"
    osm.name = "ArcGis World Imagery";
    osm.maxZoom = 20;
    osm.extension = ".jpg";
    osm.URL = "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/";
    osm.TileFolder = "tiles/satellite/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT " ARCGIS";
    osm.GoogleAPI = false;
    osm.ESRIAPI = true;
    glob_TileServerList.push_back(osm);


    //GOOGLE MAPS
    //google
    // //curl "https://mt0.google.com/vt/lyrs=s&hl=en&x=12&y=12&z=5" -o a.png
    //test URL "https://mt0.google.com/vt/lyrs=s&hl=en&x=12&y=12&z=5"
    //"lyrs" parameter in the URL :
    //Hybrid: s, h;
    //Satellite: s;
    //Streets: m;
    //Terrain: p;

    osm.name = "Google Satellite";
    osm.maxZoom = 21;
    osm.extension = ".png";
    osm.URL = "https://mt0.google.com/vt/lyrs=s";
    osm.TileFolder = "tiles/googlesatellite/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "Google";
    osm.GoogleAPI = true;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    osm.name = "Google Hybrid";
    osm.maxZoom = 21;
    osm.extension = ".png";
    osm.URL = "https://mt0.google.com/vt/lyrs=y";
    osm.TileFolder = "tiles/googleHybrid/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "Google";
    osm.GoogleAPI = true;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    osm.name = "Google Streets";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://mt0.google.com/vt/lyrs=m";
    osm.TileFolder = "tiles/googleStreets/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "Google";
    osm.GoogleAPI = true;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    osm.name = "Google Terrain";
    osm.maxZoom = 22;
    osm.extension = ".png";
    osm.URL = "https://mt0.google.com/vt/lyrs=p";
    osm.TileFolder = "tiles/googleTerrain/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "Google";
    osm.GoogleAPI = true;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);


    osm.name = "ESRI Nat Geo World";
    osm.maxZoom = 16;
    osm.extension = ".jpg";
    osm.URL = "https://server.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer/tile/";
    osm.TileFolder = "tiles/ESRI_NatGeo/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "ARCGIS";
    osm.GoogleAPI = false;
    osm.ESRIAPI = true;
    glob_TileServerList.push_back(osm);

    osm.name = "ESRI Topo";
    osm.maxZoom = 16;
    osm.extension = ".jpg";
    osm.URL = "https://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/";
    osm.TileFolder = "tiles/ESRI_Topo/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "ARCGIS";
    osm.GoogleAPI = false;
    osm.ESRIAPI = true;
    glob_TileServerList.push_back(osm);


    osm.name = "ESRI Clarity";
    osm.maxZoom = 21;
    osm.extension = ".jpg";
    osm.URL = "https://clarity.maptiles.arcgis.com/arcgis/rest/services/World_Imagery/MapServer/wmts/tile/1.0.0/World_Imagery/default/GoogleMapsCompatible/";
    osm.TileFolder = "tiles/ESRI_Clarity/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "ARCGIS";
    osm.GoogleAPI = false;
    osm.ESRIAPI = true;
    glob_TileServerList.push_back(osm);


    /*
    osm.name = "Open Topo";
    osm.maxZoom = 17;
    osm.extension = ".png";
    osm.URL = "https://c.tile.opentopomap.org/";
    osm.TileFolder = "tiles/OpenTopo/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "TBD";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);

    */


    osm.name = "Bing Maps";
    osm.maxZoom = 20;
    osm.extension = ".png";
    osm.URL = "http://r0.ortho.tiles.virtualearth.net/tiles/r";
    //https://r0.ortho.tiles.virtualearth.net/tiles/r{$q}.png?g=45</
    osm.suffix = ".png?g=45";

    osm.TileFolder = "tiles/BING_Maps/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "BIng";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    osm.QuadKey = true;
    glob_TileServerList.push_back(osm);


    osm.name = "Bing Hybrid";
    osm.maxZoom = 20;
    osm.extension = ".png";
    osm.URL = "https://ecn.t2.tiles.virtualearth.net/tiles/h";
    //osm.URL = "https://ecn.t2.tiles.virtualearth.net/tiles/h{$q}?g=761&amp;mkt=en-us";
    //osm.URL = "https://ecn.t2.tiles.virtualearth.net/tiles/h1202033313?g=761&amp;mkt=en-us";
    osm.suffix = "?g=761&amp;mkt=en-us";
    osm.TileFolder = "tiles/BING_Hybrid/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "Bing";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    osm.QuadKey = true;
    glob_TileServerList.push_back(osm);


    /*
    osm.name = "Phil";
    osm.maxZoom = 19;
    osm.extension = ".png";
    osm.URL = "http:10.254.5.20/";
    osm.TileFolder = "tiles/phil/";
    osm.APIKey = "";
    osm.needsAPIKey = false;
    osm.CopyRight = ICON_FA_COPYRIGHT "OpenStreetMap Contributors";
    osm.GoogleAPI = false;
    osm.ESRIAPI = false;
    glob_TileServerList.push_back(osm);
    */


}

namespace fs = std::filesystem;

void TileManager::DrawTileManagerMapMenu(bool &gb_NoMap)
{
    char buf[200];
    static float lastUpdateTime = (float)ImGui::GetTime();
    static std::unordered_map<std::string, std::uintmax_t> sizeCache;
    float currentTime = (float)ImGui::GetTime();

    if (ImGui::BeginMenu("Map Base"))
    {
        if (ImGui::MenuItem("No Map", NULL, gb_NoMap)) gb_NoMap = !gb_NoMap;
        ImGui::Separator();
        std::string cp = fs::current_path().generic_string();

        if (currentTime - lastUpdateTime >= 1.0f) // Update cache once per second
        {
            sizeCache.clear();
            lastUpdateTime = currentTime;
        }

        int selected = -1;
        for (int J = 0; J < glob_TileServerList.size(); J++)
        {
            std::string tPath = cp + "/" + glob_TileServerList[J].TileFolder;

            /*
            std::uintmax_t totalSize;
            if (sizeCache.find(tPath) == sizeCache.end())
            {
                totalSize = calculateTotalFileSize(tPath);
                sizeCache[tPath] = totalSize;
            }
            else
            {
                totalSize = sizeCache[tPath];
            }
            int size = (int)totalSize;          
            if (totalSize == 0)
            {
                sprintf(buf, "%s", glob_TileServerList[J].name.c_str());
            }
            else if (totalSize > 1048576)
            {
                size = totalSize / 1048576;
                sprintf(buf, "%s [%d MB]", glob_TileServerList[J].name.c_str(), size);
            }
            else if (totalSize > 1024)
            {
                size = totalSize / 1024;
                sprintf(buf, "%s [%d KB]", glob_TileServerList[J].name.c_str(), size);
            }
            else
            {
                sprintf(buf, "%s [%d bytes]", glob_TileServerList[J].name.c_str(), size);
            }
            */
            sprintf(buf, "%s", glob_TileServerList[J].name.c_str());
            if (ImGui::MenuItem(buf, NULL, (J == m_TileServerIndex) ? true : false)) 
            { 
                selected = J; 
            }
            
            
        }
        if (selected != -1) //new map type selected
        {
            SelectTileServer(selected);
            ClearMapThreadQueue();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Clear Map Data"))
        {
            try 
            {
                clear_cache(); // Clear the tile cache
                ClearMapThreadQueue(); // Clear any pending downloads
                std::string cp = fs::current_path().generic_string();               
                for (const auto& tileServer : glob_TileServerList) // Clear ALL map tile folders and remove them
                {
                    std::string tPath = cp + "/" + tileServer.TileFolder;
                    if (fs::exists(tPath))
                    {
                        std::error_code ec;
                        if (fs::remove_all(tPath, ec) > 0)
                        {
                            printf("Removed folder and contents: %s\n", tileServer.name.c_str());
                        }
                        
                        if (ec)
                        {
                            printf("Error removing folder %s: %s\n", 
                                   tPath.c_str(), 
                                   ec.message().c_str());
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                printf("Failed to clear tile folders: %s\n", e.what());
            }
            sizeCache.clear(); // Clear the cache to force a refresh
        }
        ImGui::EndMenu();
    }
}

std::uintmax_t TileManager::calculateTotalFileSize(const std::string& folderPath)
{
    std::uintmax_t totalSize = 0;
    try
    {
        if (fs::exists(folderPath))
        {
            for (const auto& entry : fs::recursive_directory_iterator(folderPath))
            {
                if (entry.is_regular_file())
                {
                    totalSize += fs::file_size(entry.path());
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        printf("Error calculating folder size: %s\n", e.what());
    }
    return totalSize;
}

void TileManager::ShowTileManagerStatsWindow(bool* pOpen)
{

    ImGui::Begin("Tile Manager Stats", pOpen);//, ImGuiWindowFlags_NoBackground);

    int ms = (int)m_tiles.size();
    int numT = (int)m_workers.size();
    int sizeRegion = (int)m_region.size();
    ImGui::Text("m_tiles.size: %d NumWorkers: %d size (length) Region: %d\r\n", ms, numT, sizeRegion, zoom);
    double r = 1.0 / pow(2, zoom);
    //ImGui::Text("Zoom Level is %d, 1/2^z: %f  Units Per Tile x: %f y: %f\r\n", z, r, units_per_tile_x, units_per_tile_y);
    int s = getQueueSize();
    ImGui::Text("Thread Queue size: %d\r\n", s);

    if (true) {
        int wk = threads_working();
        int dl = tiles_downloaded();
        int ld = tiles_loaded();
        int ca = tiles_cached();
        int fa = tiles_failed();
        ImGui::Text("FPS: %.2f    Working: %d    Downloads: %d    Loads: %d    Caches: %d    Fails: %d", ImGui::GetIO().Framerate, wk, dl, ld, ca, fa);
    }

    ImGui::End();

}
