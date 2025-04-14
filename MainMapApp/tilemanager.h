#pragma once

#include <curl/curl.h>
#include <filesystem>
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <atomic>


#include "slippymaps.h"

#include "tileserver.h"


namespace fs = std::filesystem;

class TileManager {

public:
    int zoom;

public:

    std::vector<TileServer> glob_TileServerList;
    TileServer* mTileServer;
    int m_TileServerIndex = 13;
	
    void SelectTileServer(int index)
    {
        m_TileServerIndex = index;
        mTileServer = &glob_TileServerList[m_TileServerIndex];
    }

    void CreateMapTileServerList();
    void DrawTileManagerMapMenu(bool &gb_NoMap);
    void ShowTileManagerStatsWindow(bool *s);
    void ClearCurrentTileFolder();
    static std::uintmax_t calculateTotalFileSize(const std::string& folderPath);
	
    TileManager() {
        start_workers();
    }

    std::string GetCurMapName()
    {
        return mTileServer->name;
    }

    inline ~TileManager() {
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_stop = true;
        }
        m_condition.notify_all();
        for (std::thread& worker : m_workers)
            worker.join();
    }

    void clear_cache() {
        m_region.clear(); m_tiles.clear();
    }

    int getQueueSize()
    {
        int s = 0;
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        s = (int)m_queue.size();
        return s;
    }

    void ClearMapThreadQueue()
    {
        while (!m_queue.empty()) m_queue.pop();

        if (getQueueSize() == 0)
        {
            Sleep(500);
            clear_cache();
            mTileServer = &glob_TileServerList[m_TileServerIndex];
            //ImVec2 pos = ImGui::GetWindowPos();  //top left corner of image viewport 
            //ImVec2 size = ImGui::GetWindowSize(); //size of image viewport
            //ImGui::SetWindowPos(pos);
            //ImGui::SetWindowSize(size);
        }

    }

    const std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>>& get_region(ImPlotRectDouble view, ImVec2 pixels, int _z) 
    {
        int zoom = _z;
        double min_x = std::clamp(view.X.Min, 0.0, 1.0);
        double min_y = std::clamp(view.Y.Min, 0.0, 1.0);
        double size_x = std::clamp(view.X.Size(), 0.0, 1.0);
        double size_y = std::clamp(view.Y.Size(), 0.0, 1.0);

        double pix_occupied_x = (pixels.x / view.X.Size()) * size_x;
        double pix_occupied_y = (pixels.y / view.Y.Size()) * size_y;
        double units_per_tile_x = view.X.Size() * (TILE_SIZE / pix_occupied_x);
        double units_per_tile_y = view.Y.Size() * (TILE_SIZE / pix_occupied_y);

        //int z = 0;
        //double r = 1.0 / pow(2, z);
        //while (r > units_per_tile_x && r > units_per_tile_y && z < MAX_ZOOM)
        //    r = 1.0 / pow(2, ++z);

        m_region.clear();
        if (!append_region(zoom, min_x, min_y, size_x, size_y) && zoom > 0) {
            append_region(--zoom, min_x, min_y, size_x, size_y);
            std::reverse(m_region.begin(), m_region.end());
        }
        return m_region;
    }

    const std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>>& get_region(ImPlotRectDouble view, ImVec2 pixels) {
        double min_x = std::clamp(view.X.Min, 0.0, 1.0);
        double min_y = std::clamp(view.Y.Min, 0.0, 1.0);
        double size_x = std::clamp(view.X.Size(), 0.0, 1.0);
        double size_y = std::clamp(view.Y.Size(), 0.0, 1.0);

        double pix_occupied_x = (pixels.x / view.X.Size()) * size_x;
        double pix_occupied_y = (pixels.y / view.Y.Size()) * size_y;
        double units_per_tile_x = view.X.Size() * (TILE_SIZE / pix_occupied_x);
        double units_per_tile_y = view.Y.Size() * (TILE_SIZE / pix_occupied_y);

        //int z    = 0;
        zoom = 0;
        double r = 1.0 / pow(2, zoom);
        while (r > units_per_tile_x && r > units_per_tile_y && zoom < MAX_ZOOM)
            r = 1.0 / pow(2, ++zoom);
        --zoom; //bit of mag (BG)
        m_region.clear();
        if (!append_region(zoom, min_x, min_y, size_x, size_y) && zoom > 0)
        {
            append_region(--zoom, min_x, min_y, size_x, size_y);
            std::reverse(m_region.begin(), m_region.end());
        }
        return m_region;
    }

    std::shared_ptr<Tile> request_tile(TileCoord coord) {
        std::lock_guard<std::mutex> lock(m_tiles_mutex);
        if (m_tiles.count(coord))
            return get_tile(coord);
        else if (fs::exists(coord.path(mTileServer)))
            return load_tile(coord);
        else
            download_tile(coord);
        return nullptr;
    }

    int tiles_loaded() const { return m_loads; }
    int tiles_downloaded() const { return m_downloads; }
    int tiles_cached() const { return m_loads - m_downloads; }
    int tiles_failed() const { return m_fails; }
    int threads_working() const { return m_working; }

private:

    bool append_region(int z, double min_x, double min_y, double size_x, double size_y) {
        int k = (int)pow(2, z);
        double r = 1.0 / k;
        int xa = (int)(min_x * k);
        int xb = (int)(xa + ceil(size_x / r) + 1);
        int ya = (int)(min_y * k);
        int yb = (int)(ya + ceil(size_y / r) + 1);
        xb = std::clamp(xb, 0, k);
        yb = std::clamp(yb, 0, k);
        bool covered = true;
        for (int x = xa; x < xb; ++x) {
            for (int y = ya; y < yb; ++y) {
                TileCoord coord{ z,x,y };
                std::shared_ptr<Tile> tile = request_tile(coord);
                m_region.push_back({ coord,tile });
                if (tile == nullptr || tile->state != TileState::Loaded)
                    covered = false;
            }
        }
        return covered;
    }

    void download_tile(TileCoord coord) {
        auto dir = coord.dir(mTileServer);
        fs::create_directories(dir);
        if (fs::exists(dir)) {
            m_tiles[coord] = std::make_shared<Tile>(Downloading);
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_queue.emplace(coord);
            }
            m_condition.notify_one();
        }
    }

    std::shared_ptr<Tile> get_tile(TileCoord coord) {
        if (m_tiles[coord]->state == Loaded)
            return m_tiles[coord];
        else if (m_tiles[coord]->state == OnDisk)
            return load_tile(coord);
        return nullptr;
    }

    std::shared_ptr<Tile> load_tile(TileCoord coord) {
        auto path = coord.path(mTileServer);
        if (!m_tiles.count(coord))
            m_tiles[coord] = std::make_shared<Tile>();
        if (m_tiles[coord]->image.LoadFromFile(path.c_str())) {
            m_tiles[coord]->state = TileState::Loaded;
            m_loads++;
            return m_tiles[coord];
        }
        m_fails++;
        printf("TileManager[00]: Failed to load \"%s\"\n", path.c_str());
        if (!fs::remove(path))
            printf("TileManager[00]: Failed to remove \"%s\"\n", path.c_str());
        printf("TileManager[00]: Removed \"%s\"\n", path.c_str());
        m_tiles.erase(coord);
        return nullptr;
    }

    void start_workers() {
        for (int thrd = 1; thrd < MAX_THREADS + 1; ++thrd) {
            m_workers.emplace_back(
                [this, thrd] {
                    printf("TileManager[%02d]: Thread started\n", thrd);
                    CURL* curl = curl_easy_init();
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
                    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
                    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
                    for (;;)
                    {
                        TileCoord coord;
                        {
                            std::unique_lock<std::mutex> lock(m_queue_mutex);
                            m_condition.wait(lock,
                                [this] { return m_stop || !m_queue.empty(); });
                            if (m_stop && m_queue.empty()) {
                                curl_easy_cleanup(curl);
                                printf("TileManager[%02d]: Thread terminated\n", thrd);
                                return;
                            }
                            coord = std::move(m_queue.front());
                            m_queue.pop();
                        }
                        m_working++;
                        bool success = true;
                        auto dir = coord.dir(mTileServer);
                        auto path = coord.path(mTileServer);
                        
                        std::string url;
                        if (mTileServer->GoogleAPI == true) url = coord.googleurl(mTileServer);
                        else url = coord.url(mTileServer) + mTileServer->APIKey;

                        if (mTileServer->ESRIAPI) url = coord.esriurl(mTileServer);

                        if (mTileServer->QuadKey) url = coord.bingurl(mTileServer, mTileServer->suffix);

                        //printf("Url:%s\r\n" , url.c_str()  );

                        //auto url = coord.url(mTileServer);
                        FILE* fp = fopen(coord.path(mTileServer).c_str(), "wb");
                        if (fp) {
                            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                            CURLcode cc = curl_easy_perform(curl);
                            fclose(fp);
                            if (cc != CURLE_OK) {
                                printf("TileManager[%02d]: Failed to download: \"%s\"\n", thrd, url.c_str());
                                long rc = 0;
                                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
                                if (!((rc == 200 || rc == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
                                    printf("TileManager[%02d]: Response code: %d\n", thrd, rc);
                                fs::remove(coord.path(mTileServer));
                                success = false;
                            }
                        }
                        else {
                            printf("TileManager[%02d]: Failed to open or create file \"%s\"\n", thrd, path.c_str());
                            success = false;
                        }
                        if (success) {
                            m_downloads++;
                            std::lock_guard<std::mutex> lock(m_tiles_mutex);
                            m_tiles[coord]->state = OnDisk;
                        }
                        else {
                            m_fails++;
                            std::lock_guard<std::mutex> lock(m_tiles_mutex);
                            m_tiles.erase(coord);
                        }
                        m_working--;
                    }
                }
            );
        }
    }

    std::atomic<int> m_loads = 0;
    std::atomic<int> m_downloads = 0;
    std::atomic<int> m_fails = 0;
    std::atomic<int> m_working = 0;
    std::map<TileCoord, std::shared_ptr<Tile>> m_tiles;
    std::mutex m_tiles_mutex;
    std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>> m_region;
    std::vector<std::thread> m_workers;
    std::queue<TileCoord> m_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    bool m_stop = false;
};






