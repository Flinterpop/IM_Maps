

#include "BGUtil.h"



void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
    // Note that LONGLONG is a 64-bit value
    LONGLONG ll;

    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
}



std::uintmax_t calculateTotalFileSize(const fs::path& directory)
{
    fs::file_status s = fs::file_status{};
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(directory))
    {
        std::uintmax_t totalSize = 0;
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) {
                totalSize += fs::file_size(entry);
            }
        }
        return totalSize;
    }


    return 0;
}




int GetSecondsSinceMidnightZulu()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    //printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);
    double elapsed = st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
    return (int)elapsed;
}


void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    //printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);
    double elapsed = st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
    hrs = st.wHour;
    mins = st.wMinute;
    secs = st.wSecond;

}


bool getFileNameNoExtension(const char* path, char* dest)
{
    char path_buffer[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    errno_t err;

    err = _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    if (err != 0)
    {
        printf("Error splitting the path. Error code %d.\n", err);
        return true;
    }
    //printf("Path extracted with _splitpath_s:\n");
    //printf("   Drive: %s\n", drive);
    //printf("   Dir: %s\n", dir);
    //printf("   Filename: %s\n", fname);
    //printf("   Ext: %s\n", ext);

    strcpy(dest, fname);

    return false;
}


bool getFileNameExtension(const char* path, char* dest)
{
    char path_buffer[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    errno_t err;

    err = _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    if (err != 0)
    {
        printf("Error splitting the path. Error code %d.\n", err);
        return true;
    }
    //printf("Path extracted with _splitpath_s:\n");
    //printf("   Drive: %s\n", drive);
    //printf("   Dir: %s\n", dir);
    //printf("   Filename: %s\n", fname);
    //printf("   Ext: %s\n", ext);

    strcpy(dest, ext);

    return false;
}



std::vector<std::string> LoadFileNamesInFolder(char * folderName, char * fileType)
{

    std::vector<std::string> fileList;
    char CurDir[200];
    LPTSTR lp = &CurDir[0];
    DWORD rv = GetCurrentDirectory(200, lp);
    //printf("Current folder: %s\r\n", CurDir);

    strcat(CurDir, "\\");
    strcat(CurDir, folderName);
   
    //strcat(CurDir, "\\");
    //strcat(CurDir, fileType);
    
    //printf("\Folder To Search folder is %s\n\n", CurDir);


    //from https://www.geeksforgeeks.org/cpp-program-to-get-the-list-of-files-in-a-directory/
    // This structure would distinguish a file from a directory
    struct stat sb;
    // Looping until all the items of the directory are exhausted
    int count = 0;
    //for (const auto& entry : std::filesystem::directory_iterator(CurDir))
    for (const auto& entry : std::filesystem::recursive_directory_iterator(CurDir))
    {
        // Converting the path to const char * in the subsequent lines
        std::filesystem::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();

        // Testing whether the path points to a non-directory or not If it does, displays path
        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
        {
            //printf("%03d  %s\r\n", count++, path);
            //route* rte = new route();
            //rte->pathWithFname = path;
            //char justTheFname[40];
            char justTheExtension[40] = "";
            getFileNameExtension(path, justTheExtension);
            if (!strncmp(justTheExtension, fileType, 4))
            {
                printf("Filename: %s\n", path);
                fileList.push_back(path);
            }
            //RouteList.push_back(rte);

            //RouteFileList.push_back(std::string(path));
        }
    }

    /*
    int c = 0;
    for (auto s : RouteFileList)
    {
        printf("%2d %s\r\n", c++, s.c_str());
        char dest[40];
        getFileNameNoExtension(s.c_str(), dest);
        printf("Extracted Filename: %s\n", dest);
    }
    */
    return fileList;
}
