#pragma once

//Contaisn a few function prototypes required by Maps for saving and restoring main window state.

#include "imgui.h"

void SetWindowToClose();
ImVec2 GetWindowSize();
ImVec2 GetWindowPos();
void SetWindowSize(ImVec2 s);
void SetWindowPos(ImVec2 p);


bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
bool LoadMonoTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

