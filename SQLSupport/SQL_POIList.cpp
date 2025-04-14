#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <sqlite3.h>
#include "POI.h"

using namespace std;

extern std::vector<POI*> POIList;

const char* POIDBFname = "POI.db";

static int LoadPOICallback(void* data, int argc, char** argv, char** azColName);

struct POITableRow
{
    int FieldIndex;
    char FieldName[20];
    char FieldType[20];
    bool NotNull;
    char defaultValue[20];
};

enum TableID { NAME = 1, POILat = 2, POILng = 3, bLOADED = 4, bSHOW = 5, TYPE = 6, RADIUS = 7, COLORFILL = 8, POYINTS = 9, COLOR = 10, POINT_ICON = 11 };

POITableRow POITableStructure[] = {
    {1, "Name", "CHAR(41)", true, "'POI'"},
    {2, "POILat", "REAL", true, "44.1234"},
    {3, "POILon", "REAL", true, "-76.4321"},
    {4, "bLOADED", "BOOL", true, "false"},
    {5, "bSHOW", "BOOL", true, "true"},
    {6, "Type", "INT", true, "1"},
    {7, "RadiusNM", "REAL", false, "0.0"},
    //{8, "Filled", "BOOL", false, "false"},
    {8, "ColorFill", "INT", false, "4278190335"},  // Default blue color (0xFF0000FF)
    {9, "Points", "TEXT", false, "''"},
    {10, "Color", "INT", false, "4278190335"},  // Default blue color (0xFF0000FF)
	{11, "PointIcon", "INT", false, "0"}
};

std::string BuildCreatePOITableSQLStatement()
{
    std::string sqlBuf = "CREATE TABLE POI(ID INTEGER PRIMARY KEY AUTOINCREMENT, ";
    int s = sizeof(POITableStructure) / sizeof(*POITableStructure);
    for (int x = 0; x < s; x++)
    {
        sqlBuf += POITableStructure[x].FieldName;
        sqlBuf += " ";
        sqlBuf += POITableStructure[x].FieldType;
        if (POITableStructure[x].NotNull) sqlBuf += " NOT NULL";
        if (x < s - 1) sqlBuf += ", ";
    }
    sqlBuf += " );";

    return sqlBuf;
}

void CreatePOITable()
{
    sqlite3* DB;
    std::string sql = BuildCreatePOITableSQLStatement();

    int exit = sqlite3_open(POIDBFname, &DB);
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(DB) << std::endl;
        return;
    }

    char* messaggeError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error creating POI table: " << messaggeError << std::endl;
        sqlite3_free(messaggeError);
    }
    else {
        std::cout << "Table created successfully" << std::endl;
    }

    sqlite3_close(DB);
}

std::string SerializePoints(const std::vector<std::pair<double, double>>& points)
{
    std::string serialized;
    for (const auto& point : points)
    {
        serialized += to_string(point.first) + "," + to_string(point.second) + ";";
    }
    return serialized;
}

std::vector<std::pair<double, double>> DeserializePoints(const std::string& serialized)
{
    std::vector<std::pair<double, double>> points;
    size_t pos = 0;
    size_t next;
    while ((next = serialized.find(';', pos)) != std::string::npos)
    {
        std::string point_str = serialized.substr(pos, next - pos);
        size_t comma_pos = point_str.find(',');
        if (comma_pos != std::string::npos)
        {
            double lat = stod(point_str.substr(0, comma_pos));
            double lng = stod(point_str.substr(comma_pos + 1));
            points.emplace_back(lat, lng);
        }
        pos = next + 1;
    }
    return points;
}

int InsertPOIRecord(POI* poi)
{
    sqlite3* DB;
    char* messaggeError;
    int exit = sqlite3_open(POIDBFname, &DB);
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }

    std::string Fields = "(Name, POILat, POILon, bLOADED, bSHOW, Type, RadiusNM, ColorFill, Points, Color, PointIcon)";
    std::string Values = "VALUES (";
    
    Values += "'" + std::string(poi->name) + "', ";
    Values += std::to_string(poi->lat) + ", ";
    Values += std::to_string(poi->lng) + ", ";
    Values += std::to_string(poi->loaded) + ", ";
    Values += std::to_string(poi->showOnMap) + ", ";
    Values += std::to_string(static_cast<int>(poi->POI_TYPE)) + ", ";
    Values += std::to_string(poi->radiusNM) + ", ";
    Values += std::to_string(poi->colorFill) + ", ";
    Values += "'" + SerializePoints(poi->points) + "', ";
    Values += std::to_string(poi->color) + ", ";
    Values += std::to_string(poi->pointIconIndex) + ")";

    std::string sqlStatement = "INSERT INTO POI " + Fields + " " + Values;

    std::cout << "Executing SQL: " << sqlStatement << std::endl;

    exit = sqlite3_exec(DB, sqlStatement.c_str(), NULL, 0, &messaggeError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error during record insert: " << messaggeError << std::endl;
        sqlite3_free(messaggeError);
        sqlite3_close(DB);
        return -1;
    }
    else {
        std::cout << "Record inserted successfully" << std::endl;
    }

    sqlite3_close(DB);
    return 0;
}

static int LoadPOICallback(void* data, int argc, char** argv, char** azColName)
{
    POI* poi = new POI();

    strncpy_s(poi->name, argv[TableID::NAME], 40);
    poi->lat = atof(argv[TableID::POILat]);
    poi->lng = atof(argv[TableID::POILng]);
    poi->loaded = (bool)atoi(argv[TableID::bLOADED]);
    poi->showOnMap = (bool)atoi(argv[TableID::bSHOW]);
    poi->POI_TYPE = static_cast<POIType>(atoi(argv[TableID::TYPE]));
    poi->radiusNM = static_cast<float>(atof(argv[TableID::RADIUS]));
    //poi->filled = (bool)atoi(argv[TableID::FILLED]);
    if (argc > TableID::COLORFILL - 1 && argv[TableID::COLORFILL]) {
        poi->colorFill = static_cast<ImU32>(atoll(argv[TableID::COLORFILL]));
    }


    poi->points = DeserializePoints(argv[TableID::POYINTS]);
     
	if (argc > 10 && argv[11]) {
        poi->pointIconIndex = atoi(argv[11]);
    }

    if (argc > TableID::COLOR - 1 && argv[TableID::COLOR]) {
        poi->color = static_cast<ImU32>(atoll(argv[TableID::COLOR]));
    }

    std::cout << "Loaded POI: " << poi->name << " (" << poi->lat << ", " << poi->lng << ")" << std::endl;

    POIList.push_back(poi);

    return 0;
}

void LoadPOIListFromDB()
{
    puts("Running Load POI List Query");
    
    // Clear existing POIs first
    for (auto poi : POIList) {
        delete poi;
    }
    POIList.clear();
    
    sqlite3* DB;
    int exit = sqlite3_open(POIDBFname, &DB);
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(DB) << std::endl;
        return;
    }

    // First check if table exists
    string checkTable = "SELECT name FROM sqlite_master WHERE type='table' AND name='POI';";
    char* messageError;
    bool tableExists = false;
    
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        *static_cast<bool*>(data) = true;
        return 0;
    };
    
    sqlite3_exec(DB, checkTable.c_str(), callback, &tableExists, &messageError);
    
    if (!tableExists) {
        std::cerr << "POI table does not exist. Creating table..." << std::endl;
        CreatePOITable();
        sqlite3_close(DB);
        return;
    }

    string query = "SELECT * FROM POI;";
    exit = sqlite3_exec(DB, query.c_str(), LoadPOICallback, NULL, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error loading POI list: " << messageError << std::endl;
        sqlite3_free(messageError);
    } else {
        std::cout << "POI list loaded successfully. Total POIs loaded: " << POIList.size() << std::endl;
    }

    sqlite3_close(DB);
}

void api_ClearPOIDB()
{
    sqlite3* DB;
    int exit = sqlite3_open(POIDBFname, &DB);
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(DB) << std::endl;
        return;
    }

    string query = "DELETE FROM POI;";
    char* messaggeError;
    exit = sqlite3_exec(DB, query.c_str(), NULL, 0, &messaggeError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error clearing POI table: " << messaggeError << std::endl;
        sqlite3_free(messaggeError);
    } else {
        std::cout << "POI table cleared successfully" << std::endl;
    }

    sqlite3_close(DB);
}

void InsertExamplePOIs()
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> latDist(42.0, 56.0);
    std::uniform_real_distribution<double> lonDist(-90.0, -74.0);

    // Point POI
    POI* pointPOI = new POI();
    pointPOI->lat = latDist(rng);
    pointPOI->lng = lonDist(rng);
    pointPOI->POI_TYPE = POI_POINT;
    pointPOI->color = ImColor(255, 0, 0, 255);  // Red
    strncpy(pointPOI->name, "Example Point", sizeof(pointPOI->name) - 1);
    InsertPOIRecord(pointPOI);

    // Circle POI
    POI* circlePOI = new POI();
    circlePOI->lat = latDist(rng);
    circlePOI->lng = lonDist(rng);
    circlePOI->POI_TYPE = POI_CIRCLE;
    circlePOI->radiusNM = 5.0f;
    circlePOI->filled = true;
    circlePOI->color = ImColor(0, 255, 0, 255);  // Green
    strncpy(circlePOI->name, "Example Circle", sizeof(circlePOI->name) - 1);
    InsertPOIRecord(circlePOI);

    // Polyline POI
    POI* polylinePOI = new POI();
    polylinePOI->lat = latDist(rng);
    polylinePOI->lng = lonDist(rng);
    polylinePOI->POI_TYPE = POI_POLYLINE;
    polylinePOI->points = {{latDist(rng), lonDist(rng)}, {latDist(rng), lonDist(rng)}, {latDist(rng), lonDist(rng)}};
    polylinePOI->color = ImColor(0, 0, 255, 255);  // Blue
    strncpy(polylinePOI->name, "Example Polyline", sizeof(polylinePOI->name) - 1);
    InsertPOIRecord(polylinePOI);

    // Polygon POI
    POI* polygonPOI = new POI();
    polygonPOI->lat = latDist(rng);
    polygonPOI->lng = lonDist(rng);
    polygonPOI->POI_TYPE = POI_POLYGON;
    polygonPOI->points = {{latDist(rng), lonDist(rng)}, {latDist(rng), lonDist(rng)}, {latDist(rng), lonDist(rng)}};
    polygonPOI->filled = true;
    polygonPOI->color = ImColor(255, 0, 255, 255);  // Purple
    strncpy(polygonPOI->name, "Example Polygon", sizeof(polygonPOI->name) - 1);
    InsertPOIRecord(polygonPOI);

    std::cout << "Inserted example POIs successfully" << std::endl;

    // Clean up
    delete pointPOI;
    delete circlePOI;
    delete polylinePOI;
    delete polygonPOI;
}
