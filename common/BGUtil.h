#pragma once

#include <Windows.h>
#include <string>

#include <filesystem>

namespace fs = std::filesystem;

void UnixTimeToFileTime(time_t t, LPFILETIME pft);
std::uintmax_t calculateTotalFileSize(const fs::path& directory);

int GetSecondsSinceMidnightZulu();
void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs);

bool getFileNameNoExtension(const char* path, char* dest);
bool getFileNameExtension(const char* path, char* dest);

std::vector<std::string> LoadFileNamesInFolder(char* folderName, char* fileType);