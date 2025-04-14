

#include <string>
#include "sqlite3.h"
#include "AppIni.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

bool AppIni_Debug = false;

char AppIni_dbName[120]; //= "MultiMaps_APP_Ini.db";




bool file_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    std::cout << p;
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
    {
        if (AppIni_Debug) printf(" exists\r\n");
        return true;
    }
    else
    {
        if (AppIni_Debug) printf(" does not exist\r\n");
        return false;
    }
}




void SetIniDBFileName(char* name)
{
    strcpy(AppIni_dbName, name);
}

sqlite3* openDB(const std::string& dbName) {
    sqlite3* db;
    if (sqlite3_open(dbName.c_str(), &db) == SQLITE_OK) {
        return db;
    }
    return nullptr;
}

bool createTable(sqlite3* db, const std::string& sql) {
    char* errorMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errorMsg) != SQLITE_OK) {
        printf("SQLite error: %s  in createTable\r\n", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    if (AppIni_Debug) printf("Created table %s", sql.c_str());
    return true;
}

// Function to close the database
bool closeDB(sqlite3* db)
{
    return sqlite3_close(db) == SQLITE_OK;
}

// Function to execute SQL statements
bool executeSQL(sqlite3* db, const std::string& sql) {
    char* errorMsg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    if (errorMsg) {
        sqlite3_free(errorMsg);
    }
    return result == SQLITE_OK;
}

// Function to prepare and execute a SQL statement
//Not used in AppIni
bool __prepareAndExecute(sqlite3* db, const std::string& sql) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Process each row, for example:
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        if (AppIni_Debug) printf("Name: %s , Age: %d\r\n", name, age);
    }
    sqlite3_finalize(stmt);
    return true;
}



//Create and intialize Application Database with 3 tables
bool CreateAppIni()
{
    printf("\r\nCreating new App Ini Data base %s\r\n", AppIni_dbName);

    const fs::path sandbox{ AppIni_dbName };
    if (file_exists(sandbox))
    {
        printf("Database %s already exists\r\n", AppIni_dbName);
        return false;
    }

    sqlite3* db;
    char* zErrMsg = 0;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char* errorMsg = nullptr;

    //Create Application  INT table
    std::string sql =
        "CREATE TABLE Integers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value INTEGER NOT NULL"
        ");";

    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        printf("SQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else puts("Created Integer Table");


    //Create Application  String table
    std::string sql2 =
        "CREATE TABLE Strings ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value TEXT NOT NULL"
        ");";

    if (sqlite3_exec(db, sql2.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        printf("SQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else puts("Created Strings Table");


    //Create Application float table
    std::string sql3 =
        "CREATE TABLE REAL ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value REAL NOT NULL"
        ");";

    if (sqlite3_exec(db, sql3.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        printf("SQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else puts("Created Reals Table");


    printf(">> Created DB: %s\r\n", AppIni_dbName);

    return true;
}


//also used for bools which are stored as Integers
bool DoesIntegerExistInIntegersTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    snprintf(query, 100,"SELECT exists(SELECT 1 FROM integers WHERE name = '%s') AS row_exists;", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep in DoesIntExist\r\n");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) printf("DoesIntExist?: %s val: %s\r\n", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}

bool DoesRealExistInRealsTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    sprintf(query, "SELECT exists(SELECT 1 FROM real WHERE name = '%s') AS row_exists;", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep in DoesRealExist\r\n");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) printf("DoesRealExist?: %s val: %s\r\n", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}



bool DoesStringExistInStringsTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    sprintf(query, "SELECT exists(SELECT 1 FROM Strings WHERE name = '%s') AS row_exists;", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep in DoesStringExist\r\n");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) printf("DoesStringExist?: %s val: %s\r\n", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}



//INTEGER
int GetIniInt(char* name, int _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM INTEGERS WHERE NAME = '%s';", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep error in GetIniInt\r\n");
        sqlite3_close(db);
        return _default;
    }

    int val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) printf("Retrieved Name : % s val : % d\r\n", name, val);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}

bool SetIniInt(char* name, int val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Integers (NAME, VALUE ) VALUES('%s', %d);", name, val);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("Integer insertion complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("Integer insertion failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool _UpdateIniInt(char* name, int val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Integers SET VALUE = %d WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("Integer Update complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("Integer Update failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniInt(char* name, int val)
{
    if (false == DoesIntegerExistInIntegersTable(name))
    {
        bool retVal =  SetIniInt(name, val);
        if (false == retVal) printf("Failed to SetIniInt %s\r\n", name);
        return retVal;

    }
    int retVal = _UpdateIniInt(name, val);
    if (false == retVal) printf("Failed to UpdateIniInt %s\r\n", name);


    return retVal;
}


//BOOL
int GetIniBool(char* name, bool _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM INTEGERS WHERE NAME = '%s';", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep error in GetIniBool\r\n");
        sqlite3_close(db);
        return _default;
    }

    int val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) printf("Retrived Bool as Int: Name: %s val: %d -> %s\r\n", name, val, val == 1 ? "true" : "false");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (0==val)?false:true;
}

bool SetIniBool(char* name, bool _val)
{
    bool val=false;
    if (_val != 0) val = true;

    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Integers(NAME, VALUE ) VALUES('%s', %d);", name, val);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("Bool as Int insertion complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("Bool as Int insertion failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool _UpdateIniBool(char* name, bool _val)
{
    bool val = false;
    if (_val != 0) val = true;

    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Integers SET VALUE = '%d' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);

     if (insert_successful)
    {
         if (AppIni_Debug) printf("Update bool as Int insertion complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
         if (AppIni_Debug) printf("Update bool as Int failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniBool(char* name, bool val)
{
    if (false == DoesIntegerExistInIntegersTable(name))
    {
        bool retVal = SetIniInt(name, val);
        if (false == retVal) printf("Failed to SetIniBool %s\r\n", name);
        return retVal;
    }
    bool retVal = _UpdateIniBool(name, val);
    if (false == retVal) printf("Failed to UpdateIniBools %s\r\n", name);
    return retVal;
}


// REAL
double GetIniReal(char* name, double _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM REAL WHERE NAME = '%s';", name);
    if (AppIni_Debug)  printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep error in GetIniReal\r\n");
        sqlite3_close(db);
        return _default;
    }

    double val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_double(stmt, 0);
        if (AppIni_Debug) printf("Retrieved REAL Name: %s val: %f\r\n", name, val);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}

bool SetIniReal(char* name, double val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database for Real Insert: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO REAL(NAME, VALUE ) VALUES('%s', %f);", name, val);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("Real Data insertion complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("Real Data insertion failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool _UpdateIniReal(char* name, double val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE REAL SET VALUE = '%f' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("Real update complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("Real update failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniReal(char* name, double val)
{
    if (false == DoesRealExistInRealsTable(name))
    {
        bool retVal = SetIniReal(name, val);
        if (false == retVal) printf("Failed to SetIniReal %s\r\n", name);
        return retVal;
    }

    bool retVal = _UpdateIniReal(name, val);
    if (false == retVal) printf("Failed to UpdateIniReal %s\r\n", name);
    return retVal;
}


//STRING
std::string GetIniString(char* name, std::string _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM Strings WHERE NAME = '%s';", name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) printf("SQL Prep error in GetIniInt\r\n");
        sqlite3_close(db);
        return _default;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (AppIni_Debug) printf("Retrieved String Value: %s\r\n", name);
        _default = std::string(name);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return _default;
}


bool SetIniString(char* name, char* val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Strings (NAME, VALUE ) VALUES('%s', '%s');", name, val);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("String insertion complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("String insertion failed\r\n");
        sqlite3_close(db);
        return false;
    }
}

bool _UpdateIniString(char* name, char* val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Strings SET VALUE = '%s' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) printf("Running: %s\r\n", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) printf("String Update complete\r\n");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) printf("String Update failed\r\n");
        sqlite3_close(db);
        return false;
    }
}



bool UpdateIniString(char* name, char * val)
{
    if (false == DoesStringExistInStringsTable(name))
    {
        bool retVal = SetIniString(name, val);
        if (false == retVal) printf("Failed to SetIniString %s\r\n", name);
        return retVal;
    }

    bool retVal = _UpdateIniString(name, val);
    if (false == retVal) printf("Failed to UpdateIniString %s\r\n", name);
    return retVal;
}