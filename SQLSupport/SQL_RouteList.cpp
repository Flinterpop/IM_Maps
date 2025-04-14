
#include <string>
#include <mutex>
#include <vector>
#include "sqlite3.h"

#include "SQL_Common.h"
#include "SQL_RouteList.h"


extern std::vector<route*> RouteList;
const char* RouteDBFname = "Routes.db";

const int NUM_COLS_IN_ROUTES_TABLE = 7;
const int NUM_COLS_IN_WAYPOINTS_TABLE = 9;



void api_ClearRoutesDB()
{
	puts("Dropping Routes DB Tables");
	DropTable(RouteDBFname, "Waypoints");
	DropTable(RouteDBFname, "Routes");
	api_LoadRouteListFromDB();
}

void api_CreateRouteTables()
{
	sqlite3* DB;
	std::string sql =
		"CREATE TABLE Routes ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"name TEXT NOT NULL UNIQUE, "
		"numWaypoints INTEGER NOT NULL, "
		"floor INTEGER, "
		"ceiling INTEGER, "
		"colour INTEGER, "
		"showOnMap INTEGER"
		");";

	int exit = sqlite3_open(RouteDBFname, &DB);
	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		puts("Error Creating Routes Table");
		sqlite3_free(messaggeError);
	}
	else puts("Routes Table created Successfully");

	sql =
		"CREATE TABLE Waypoints ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"routeID INTEGER NOT NULL, "  //this points to the route that uses this waypoint
		"name TEXT NOT NULL, "
		"lat REAL NOT NULL, "
		"lon REAL NOT NULL, "
		"altitude INTEGER, "
		"speed INTEGER, "
		"turnrate INTEGER, "
		"climbrate INTEGER, "
		"speedrate INTEGER"
		");";

	exit = sqlite3_open(RouteDBFname, &DB);
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		puts("Error Creating Waypoints Table");
		sqlite3_free(messaggeError);
	}
	else puts("Waypoints Table created Successfully");

	sqlite3_close(DB);
}



static int LoadRouteWaypointsCallback(void* data, int argc, char** argv, char** azColName)
{
	route* r = (route*)data;
	printf("In callback for route %s\r\n argc is %d\r\n", r->name, argc);
	for (int x = 0;x < argc;x++) printf("%d   %s   %s\r\n", x, azColName[x], argv[x]);

	if (NUM_COLS_IN_WAYPOINTS_TABLE != argc)
	{
		printf("Error in LoadRouteWaypointsCallback: not enough columns\r\n");
		return 0;
	}

	waypoint* w = new waypoint();
	w->DB_ID = atoi(argv[0]);
	strncpy_s(w->name, argv[1], 40);
	w->lat = atof(argv[2]);
	w->lng = atof(argv[3]);
	w->altitude = atoi(argv[4]);
	if (NULL == argv[5]) w->speed = 0; else w->speed = atoi(argv[5]); 
	if (NULL == argv[6]) w->TurnRate = 0; else w->TurnRate = atoi(argv[6]);
	if (NULL == argv[7]) w->ClimbRate = 0; else w->ClimbRate = atoi(argv[7]);
	if (NULL == argv[8]) w->SpeedRate = 0; else w->SpeedRate = atoi(argv[8]);

	r->wpList.emplace_back(w);
	r->loaded = true;
	return 0;
}

//called second
bool api_LoadWaypointsForRouteFromDB(route* rt)
{
	printf("Running Load Waypoints for route %s Query\r\n",rt->name);
	sqlite3* DB;
	int exit = sqlite3_open(RouteDBFname, &DB);
	char buf[200];
	sprintf(buf, "SELECT wayPoints.ID, wayPoints.name, lat, lon , altitude, speed, turnrate, climbrate, speedrate FROM  waypoints INNER JOIN routes ON routes.ID = wayPoints.routeID  where routes.ID =%d;", rt->DB_ID);
	std::string query = buf; 
	sqlite3_exec(DB, query.c_str(), LoadRouteWaypointsCallback, (void*)rt, NULL);
	sqlite3_close(DB);
	return true;
}


static int LoadRouteListFromDBCallBack(void* data, int argc, char** argv, char** azColName)
{
	if (NUM_COLS_IN_ROUTES_TABLE != argc)
	{
		printf("Error in LoadRouteListCallback: not enough columns\r\n");
		return 0;
	}

	for (int x = 0;x < argc;x++) //DEBUG-LOGGING
		printf("%d   %s   %s\r\n", x, azColName[x], argv[x]);

	route* r = new route();
	r->DB_ID = atoi(argv[0]);
	strncpy_s(r->name, argv[1], 80);
	r->NumWayPoints = atoi(argv[2]);
	if (NULL == argv[3]) r->Floor = 0; else r->Floor = atoi(argv[3]);
	if (NULL == argv[4]) r->Ceiling = 600000;else r->Ceiling = atoi(argv[4]);
	if (NULL == argv[5]) r->routeColour = ImColor(255, 0, 0, 255);else r->routeColour = atoi(argv[5]);
	if (NULL == argv[6]) r->showOnMap = false; else r->showOnMap = atoi(argv[6]);

	r->loaded = false;

	RouteList.emplace_back(r);
	return 0;
}

//called first
void api_LoadRouteListFromDB()
{
	puts("Running Load Route List Query");
	RouteList.clear();
	sqlite3* DB;
	int exit = sqlite3_open(RouteDBFname, &DB);

	char buf[200];
	//sprintf(buf, "select * from routes;");
	sprintf(buf, "select ID, NAME, NUMWAYPOINTS, FLOOR, CEILING, COLOUR, showOnMap from routes;");
	std::string query = buf; 
	sqlite3_exec(DB, query.c_str(), LoadRouteListFromDBCallBack, NULL, NULL);
	sqlite3_close(DB);
}







bool api_DeleteRoute(int Route_DB_ID)
{
	sqlite3* DB;
	std::string sql = "DELETE FROM Waypoints WHERE Waypoints.routeID = " + std::to_string(Route_DB_ID) + ";";
	printf("Built:%s\r\n", sql.c_str());
	int exit = sqlite3_open(RouteDBFname, &DB);
	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		puts("Error deleting waypoint records");
		sqlite3_free(messaggeError);
	}
	else printf("Waypoints for route ID %d deleted\r\n", Route_DB_ID);

	sql = "DELETE FROM Routes WHERE ID = " + std::to_string(Route_DB_ID) + ";";
	printf("Built:%s\r\n", sql.c_str());
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		puts("Error deleting route record");
		sqlite3_free(messaggeError);
	}
	else printf("route ID %d deleted\r\n", Route_DB_ID);

	sqlite3_close(DB);
	return false;
}



int api_SaveNewRoute(route* r)
{
	sqlite3* DB;
	char* messaggeError;
	int exit = sqlite3_open(RouteDBFname, &DB);
	std::string Fields = "(";
	std::string Values = "VALUES (";

	char buf[200];
	sprintf(buf, "'%s',  %d, %d, %d, %d, %d )", r->name, (int)r->wpList.size(), r->Floor, r->Ceiling, r->routeColour, r->showOnMap);
	std::string sqlStatement = "INSERT INTO ROUTES (NAME, NUMWAYPOINTS, FLOOR, CEILING, COLOUR, showOnMap) VALUES (" + std::string(buf);

	printf("Built:%s\r\n", sqlStatement.c_str());
	exit = sqlite3_exec(DB, sqlStatement.c_str(), NULL, 0, &messaggeError);
	if (exit != SQLITE_OK) {
		printf("Error during record insert: %s\r\n", messaggeError);
		sqlite3_free(messaggeError);
		return -1;
	}
	else puts("Save to Routes table !");

	//int routeID = FindIDforRouteName(r->name);
	int routeID = FindIDforItemInTable(RouteDBFname, "routes", r->name);

	if (-1 == routeID) return -1;

	for (auto w : r->wpList)
	{
		// insert into wayPoints(routeID, name, lat, lon) values(1, 'wp4', 44.4, -75.3);
		sprintf(buf, "insert into wayPoints(routeID, name, lat, lon, altitude, speed, turnrate, climbrate, speedrate) values(%d ,'%s', %f, %f, %d, %d, %d, %d, %d);", 
			routeID, w->name, w->lat, w->lng, w->altitude,w->speed,  w->TurnRate, w->ClimbRate, w->SpeedRate);
		std::string sqlStatement = buf;

		printf("Built:%s\r\n", sqlStatement.c_str());
		exit = sqlite3_exec(DB, sqlStatement.c_str(), NULL, 0, &messaggeError);
		if (exit != SQLITE_OK) {
			printf("Error during record insert: %s\r\n", messaggeError);
			sqlite3_free(messaggeError);
			return -1;
		}
		else puts("Save to Waypoints!");
	}

	sqlite3_close(DB);
	return (0);
}



int api_UpdateRoute(route* r)
{
	sqlite3* DB;
	char* messageError;
	int exit = sqlite3_open(RouteDBFname, &DB);
	char buf[300];
	sprintf(buf, "UPDATE ROUTES SET floor = %d, ceiling = %d, colour = %d WHERE ID = %d",r->Floor, r->Ceiling, r->routeColour, r->DB_ID);

	printf("Built:%s\r\n", buf);
	exit = sqlite3_exec(DB, buf, NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		printf("Error during record update: %s\r\n", messageError);
		sqlite3_free(messageError);
		return -1;
	}
	else puts("Updated Routes table !");
	
	//delete entire waypoint list then re-save new list
	std::string sql = "DELETE FROM Waypoints WHERE Waypoints.routeID = " + std::to_string(r->DB_ID) + ";";
	printf("Built:%s\r\n", sql.c_str());
	exit = sqlite3_open(RouteDBFname, &DB);
	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		puts("Error deleting waypoint records");
		sqlite3_free(messaggeError);
	}
	else printf("Waypoints for route ID %d deleted\r\n", r->DB_ID);



	for (auto w : r->wpList)
	{
		sprintf(buf, "UPDATE WAYPOINTS SET name = '%s', lat = %f, lon = %f , altitude = %d, speed = %d, turnrate = %d, climbrate = %d, speedrate = %d WHERE ID = %d", w->name, w->lat, w->lng, w->altitude, w->speed, w->TurnRate, w->ClimbRate, w->SpeedRate, w->DB_ID);
		printf("Built:%s\r\n", buf);
		exit = sqlite3_exec(DB, buf, NULL, 0, &messageError);
		if (exit != SQLITE_OK) {
			printf("Error during waypoint update: %s\r\n", messageError);
			sqlite3_free(messageError);
			return -1;
		}
		else puts("Updated Waypoints!");
	}
	
	sqlite3_close(DB);
	return (0);
}