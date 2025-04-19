

#include "ImGui.h"
#include "MouseManager.h"
#include "DtedFile.hpp"
#include "ImVec2Double.h"

#include "ImBGUtil.h"
#include "GeoCalcs.h"

#include "imgui_plot.h"
extern _MouseManager g_MouseMngr;



//DTED vars
std::vector< DtedFile*> DtedList;


int DTEDAlpha = 128;
ImColor DTEDBlend = ImColor(255, 255, 255, DTEDAlpha);


DtedFile dt;
int DtedRow = 1800;
int DtedCol = 900;

bool CreatingLOSLine = false;
bool LOSLineValid = false;

ImVec2Double LOS_p1;
ImVec2Double LOS_p2;
ImVec2Double LOS_p2_Creating;

int LOS_ij_p1_x = 5;
int LOS_ij_p1_y = 5;
int LOS_ij_p2_x = 120;
int LOS_ij_p2_y = 20;

void LoadDTEDTiles();
DtedFile* FindDTEDCellAt(double lat, double lng);
bool ElevationProfileWindow();
bool LOSProfileWindow(ImVec2Double p2);
int heightAt(double lat, double lng, int& i, int& j);

DtedFile *dt_Details;

static bool showCellInfo = false;



void DrawDTEDLOS(ImVec2(*funcLatLon2VPxy)(double, double))
{
    //ImVec2 p2XY;
    if (CreatingLOSLine)
    {
        LOS_p2_Creating.x = g_MouseMngr.MouseLL.x;
        LOS_p2_Creating.y = g_MouseMngr.MouseLL.y;

        ImVec2 p2 = funcLatLon2VPxy(g_MouseMngr.MouseLL.x, g_MouseMngr.MouseLL.y);
        ImVec2 p1XY = funcLatLon2VPxy(LOS_p1.x, LOS_p1.y);
        ImGui::GetWindowDrawList()->AddLine(p1XY, p2, ImColor(255, 0, 0, 255));
    }
    
    else if (LOSLineValid)
    {
        ImVec2 p1XY = funcLatLon2VPxy(LOS_p1.x, LOS_p1.y);
        ImVec2 p2XY = funcLatLon2VPxy(LOS_p2.x, LOS_p2.y);
        ImGui::GetWindowDrawList()->AddLine(p1XY, p2XY, ImColor(255, 0, 0, 255));
    }
}

void ShowDTEDCellInfo()
{
    if (!showCellInfo) return;
    if (nullptr == dt_Details) return;

    ImGui::Begin("DTED Info", &showCellInfo);
    ImGui::Text("Fname: %s", dt_Details->filename);
    ImGui::Text("BL Corner: %0.0f   %0.0f", dt_Details->swLat, dt_Details->swLon);
    ImGui::Text("Min Elevation: %0.0f  Max Elevation: %0.0f [m] HAE", dt_Details->minElev, dt_Details->maxElev);
    ImGui::Text("Lat Spacing (degrees): %f", dt_Details->latSpacing);
    ImGui::Text("Lon Spacing (degrees): %f", dt_Details->lonSpacing);
    ImGui::Text("Num Points Lat: %d", dt_Details->nptlat);
    ImGui::Text("Num Points Lng: %d", dt_Details->nptlong);
    ImGui::End();

}

void ElevationLeftClickMenu(bool & mb_LeftClickMenuOpen, ImVec2Double& m_LeftClickLocationLL)
{
    if (ImGui::MenuItem("DTED Cell Info", NULL, false, true))
    {
        printf("Find DTED Cell at %f %f\r\n", m_LeftClickLocationLL.x, m_LeftClickLocationLL.y);
        dt_Details = FindDTEDCellAt(m_LeftClickLocationLL.x, m_LeftClickLocationLL.y);

        if (nullptr != dt_Details) showCellInfo = true;
        mb_LeftClickMenuOpen = false;  //stops this menu from displaying
    }

    if (CreatingLOSLine)
    {
        if (ImGui::MenuItem("Save LOS Line", NULL, false, true))
        {
            LOS_p2 = m_LeftClickLocationLL;
            CreatingLOSLine = false;
            LOSLineValid = true;
            mb_LeftClickMenuOpen = false;  //stops this menu from displaying
        }
        if (ImGui::MenuItem("Cancel LOS Line", NULL, false, true))
        {
            CreatingLOSLine = false;
            mb_LeftClickMenuOpen = false;  //stops this menu from displaying
        }

    }
    else
    {
        if (ImGui::MenuItem("Create LOS Line", NULL, false, true))
        {
            LOSLineValid = false;
            LOS_p1 = m_LeftClickLocationLL;
            CreatingLOSLine = true;
            mb_LeftClickMenuOpen = false;  //stops this menu from displaying
        }
    }
}


DtedFile *FindDTEDCellAt(double lat, double lng)
{
    int BL_Lat = (int)lat;
    int BL_Lng = (int)lng;
    --BL_Lng;

    for (auto dt : DtedList)
    {
        //printf("%d %d   %d %d\r\n", BL_Lat, BL_Lng, (int)dt->BL_Lat, (int)dt->BL_Lng);
        if (((int)dt->BL_Lat == BL_Lat) && ((int)dt->BL_Lng == BL_Lng))
        {
            return dt;;
        }
    }

    return nullptr;
}




void CheckElevationKeysPressed()
{
    if (ImGui::IsKeyPressed(ImGuiKey_O)) {
        DTEDAlpha -= 5; if (DTEDAlpha > 255) DTEDAlpha = 255;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_P)) { DTEDAlpha += 5; if (DTEDAlpha < 0) DTEDAlpha = 0;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_I)) { dt.SetScaleLocal(); }
    if (ImGui::IsKeyPressed(ImGuiKey_Y)) { LoadDTEDTiles(); }
}


int heightAt(ImVec2Double p)
{
    int i, j;
    return heightAt(p.x, p.y, i, j);
}

int heightAt(double lat, double lng, int& i, int& j)
{
    int BL_Lat = (int)lat;
    int BL_Lng = (int)lng;
    --BL_Lng;

    for (auto dt : DtedList)
    {
        //printf("%d %d   %d %d\r\n", BL_Lat, BL_Lng, (int)dt->BL_Lat, (int)dt->BL_Lng);
        if (((int)dt->BL_Lat == BL_Lat) && ((int)dt->BL_Lng == BL_Lng))
        {
            return dt->heightAt(lat, lng, i, j);
        }
    }

    i = -1;
    j = -1;
    return -99;
}

void LoadDTEDTiles()
{
    DtedList.clear();
    int Wests[] = { 75, 76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92 };
    int Norths[] = { 42,43,44,45,46,47,48,49,50,51 };

    //for (int w=75;w<=92;w++)
    //    for (int n = 42;n <= 49;n++)
    for (int w = 75;w <= 77;w++)
        for (int n = 42;n <= 44;n++)
        {
            DtedFile* dt = new DtedFile((double)-w,(double)n);
            sprintf(dt->filename, "dted2\\w%03d\\n%02d.dt2", w, n);
            if (dt->loadData(dt->filename))
            {
                dt->DTEDtoTexture();
                DtedList.push_back(dt);
                //printf("Loaded %s  %d  %d \r\n", dt->filename,(int)dt->BL_Lat, (int)dt->BL_Lng);
            }
            else std::cout << "loadData returned true (OK)\n";
        }
    //if (dt.loadData("dted2\\w075\\n42.dt2"))
    printf("Loaded %d DTED files\r\n", (int)DtedList.size());
}


void DrawElevationOverlay(ImVec2(*LatLon2VPxy) (double, double))
{
    for (auto dt : DtedList)
    {
        ImVec2  DTED_TL = LatLon2VPxy((float)(dt->BL_Lat + 1.0f), (float)dt->BL_Lng);
        ImVec2  DTED_BR = LatLon2VPxy((float)dt->BL_Lat, (float)(dt->BL_Lng  + 1.0f));
        ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)dt->DtedTexture, DTED_TL, DTED_BR, { 0,0 }, { 1,1 }, ImColor(255, 255, 255, DTEDAlpha));
        ImGui::GetWindowDrawList()->AddRect(DTED_TL, DTED_BR, ImColor(255, 255, 255, 64));
    }

    if (LOSLineValid)
    {
        LOSProfileWindow(LOS_p2);
    }
    else if (CreatingLOSLine)
        LOSProfileWindow(LOS_p2_Creating);

/*
    if (0 < dt.nptlat)
    {
        ImVec2 DTED_TL = LatLon2VPxy(43.0, -76.0);
        ImVec2 DTED_BR = LatLon2VPxy(42.0, -75.0);
        //ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)dt.DtedTexture, DTED_TL, DTED_BR, { 0,0 }, { 1,1 }, ImColor(255, 255, 255, DTEDAlpha));

        double lat = 42.0 + DtedRow / 3600.0;
        double lng = -77.0 + DtedCol / 3600.0;
        ImVec2 p1 = LatLon2VPxy(lat, -76.0);
        ImVec2 p2 = LatLon2VPxy(lat, -75.0);

        //horizontal line
        ImGui::GetWindowDrawList()->AddLine(p1, p2, ImColor(255, 0, 255, 255));

        if (CreatingLOSLine || LOSLineValid)
        {
            ImVec2 p1 = LatLon2VPxy(LOS_p1.x, LOS_p1.y);

            if (false == LOSLineValid)
            {
                LOS_p2.x = g_MouseMngr.MouseLL.x;
                LOS_p2.y = g_MouseMngr.MouseLL.y;
                int indexI, indexJ;
                dt.heightAt((g_MouseMngr.MouseLL.x), (g_MouseMngr.MouseLL.y), indexI, indexJ);
                LOS_ij_p2_x = indexI;
                LOS_ij_p2_y = indexJ;
                // printf("i/j2: %d %d \r\n", indexI, indexJ);
            }

            ImVec2 p2 = LatLon2VPxy(LOS_p2.x, LOS_p2.y);


            ImGui::GetWindowDrawList()->AddLine(p1, p2, ImColor(255, 0, 0, 255));

        }
    }
    */

}


constexpr size_t buf_size = 512;

float heights[2000];
float heightsCurve[2000];
float indexX[2000];
bool LOSProfileWindow(ImVec2Double p2)
{
    int ArrayLength = 2000;
    ImGui::Begin("LOS", 0);

    struct Funcs
    {
        //static float DTED(void*, int j) { return dt.columns[j][DtedRow]; }
        static float DTED_H(void*, int j) { return heights[j]; }
    };

    ImGui::Text("P1: %7.4f %8.4f",LOS_p1.x, LOS_p1.y);
    ImGui::Text("P2: %7.4f %8.4f", p2.x, p2.y);

    ImVec2Double rb = GetBearingAndRangeInNMBetweenTwoGeoPoints(LOS_p1.x, LOS_p1.y, p2.x, p2.y);
    ImGui::Text("Range: %0.2f  Azimuth: %0.2f",rb.x, rb.y );

    int p1_El = heightAt(LOS_p1);
    int p2_El = heightAt(p2);
    ImGui::Text("Elevation p1: %d  Elevation p2: %d  [m MSL]",p1_El, p2_El);

    double deltaD = 1852.0 * rb.x / (float)ArrayLength;

    ImGui::Text("Samples: %d Spacing %0.2f [m]", ArrayLength, deltaD);

    ImGui::Separator();

    double mp_x;
    double mp_y;
    float max = 0;
    float min = 9999;
    //4/3R Earth drop: 6.08 meters per km
    for (int x = 0;x < ArrayLength;x++)
    {
        indexX[x] = (float)(x* deltaD)/1852.0f;
        double retVal = CalcEndPointFromRangeInMetresAndBearing(LOS_p1.x, LOS_p1.y, rb.y, x * deltaD, mp_x, mp_y);
        heights[x] = (float)heightAt(ImVec2Double(mp_x, mp_y));
        
        float curvatureDrop = (x * deltaD)*(6.08f/1000.0f);
        heightsCurve[x] = heights[x] - curvatureDrop;

        if (heights[x] > max) max = heights[x];
        if (heights[x] < min) min = heights[x];
    }
    
    float (*func)(void*, int) = Funcs::DTED_H;
    //PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
    //char overlay[40];
    //sprintf(overlay, "Min: %0.0f  Max: %0.0f", min, max);
    //ImGui::PlotLines("Lines##1", func, NULL, ArrayLength, 0, overlay, 0, 1000.0f, ImVec2(1000, 400));

    //////
    static const float* y_data[] = { heights, heightsCurve };
    static uint32_t selection_start = 0, selection_length = 0;
    static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0), ImColor(0, 0, 255) };
    ImGui::PlotConfig conf;
    conf.values.xs = indexX;
    conf.values.count = ArrayLength;
    conf.values.ys_list = y_data; // use ys_list to draw several lines simultaneously
    conf.values.ys_count = 2;
    conf.values.colors = colors;
    conf.scale.min = 0;
    conf.scale.max = 1000;
    conf.tooltip.show = true;
    conf.tooltip.format = "%.2f nm, %.0f m MSL";
    conf.grid_x.show = false;
    conf.grid_x.size = 100;
    conf.grid_x.subticks = 100;
    conf.grid_y.show = false;
    conf.grid_y.size = 10.0f;
    conf.grid_y.subticks = 5;
    conf.selection.show = true;
    conf.selection.start = &selection_start;
    conf.selection.length = &selection_length;
    conf.frame_size = ImVec2(1000 , 500);

    ImGui::Plot("Elevation Profile", conf);


    ImGui::End();
    return false;
}




bool ElevationTileWindow(bool pOpen)
{
    if (ImGui::Begin("Canvas Demo", &pOpen))
    {
        static int sideLength = 10;
        static int numSquares = 100;
        static int start_i = 10;
        static int start_j = 10;
        if (true)//LOSLineValid)
        {
            char buf[100];
            sprintf(buf, "i/j2  %d %d  i/j2 %d %d", LOS_ij_p1_x, LOS_ij_p1_y, LOS_ij_p2_x, LOS_ij_p2_y);
            ImGui::Text(buf);
        }
        ImGui::SliderInt("Col", &start_i, 0, 3600 - 50);
        ImGui::SliderInt("Row", &start_j, 0, 3600 - 50);
        // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        if (canvas_sz.x < 150.0f) canvas_sz.x = 150.0f;
        if (canvas_sz.y < 150.0f) canvas_sz.y = 150.0f;

        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        // Draw border and background color
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
        ImGuiIO& io = ImGui::GetIO();
        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool is_hovered = ImGui::IsItemHovered(); // Hovered
        const bool is_active = ImGui::IsItemActive();   // Held

        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);

        if (is_active)
        {
            start_i -= io.MouseDelta.x;
            if (start_i < 0) start_i = 0;
            start_j -= io.MouseDelta.y;
            if (start_j < 0) start_j = 0;
        }

        if (is_hovered) //if mouse is over this window then check for Zoom
        {
            if (0 != io.MouseWheel) //mouse wheel has moved so Zoom over and around the current mouse location
            {
                if (io.MouseWheel > 0) numSquares += 25;
                if (io.MouseWheel < 0) numSquares -= 25;
                sideLength = 1000 / numSquares;
                printf("Num Squares Per side: %d Side Length: %d\r\n", numSquares, sideLength);

            }
        }






        // Draw grid + all lines in the canvas
        draw_list->PushClipRect(canvas_p0, canvas_p1, true);


        for (int j = 0;j < numSquares;j++)
            for (int i = 0;i < numSquares;i++)
            {
                char buf[20];
                int height = dt.columns[start_i + i][start_j + j];
                sprintf(buf, "\r\n     %d", height);
                int cval = dt.DtedImage[start_j + j][start_i + i][0];

                draw_list->AddRectFilled(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(cval, cval, cval, 255));
                draw_list->AddRect(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(255, 255, 255, 32));
                //draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(canvas_p0.x + i* sideLength, canvas_p0.y + j* sideLength), IM_COL32(255, 255, 255, 255), buf);

            }



        int j = LOS_ij_p1_x; int i = LOS_ij_p1_y;
        int cval = dt.DtedImage[start_j + j][start_i + i][0];
        draw_list->AddRectFilled(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(0, 255, 0, 255));
        draw_list->AddRect(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(255, 255, 255, 32));

        j = LOS_ij_p2_x;  i = LOS_ij_p2_y;
        cval = dt.DtedImage[start_j + j][start_i + i][0];
        draw_list->AddRectFilled(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(0, 0, 255, 255));
        draw_list->AddRect(ImVec2(canvas_p0.x + (i * sideLength), canvas_p0.y + (j * sideLength)), ImVec2(canvas_p0.x + (i + 1) * sideLength, canvas_p0.y + (j + 1) * sideLength), ImColor(255, 255, 255, 32));

        int width = LOS_ij_p2_x - LOS_ij_p1_x;
        int height = LOS_ij_p2_y - LOS_ij_p1_y;

        int m_new = 2 * (LOS_ij_p2_y - LOS_ij_p1_y);
        int slope_error_new = m_new - (LOS_ij_p2_x - LOS_ij_p1_x);
        for (int x = LOS_ij_p1_x, y = LOS_ij_p1_y; x <= LOS_ij_p2_x; x++)
        {
            //printf("x/y: %d %d\r\n", x, y);
            draw_list->AddRectFilled(ImVec2(canvas_p0.x + (x * sideLength), canvas_p0.y + (y * sideLength)), ImVec2(canvas_p0.x + (x + 1) * sideLength, canvas_p0.y + (y + 1) * sideLength), ImColor(0, 0, 255, 255));

            // Add slope to increment angle formed 
            slope_error_new += m_new;

            // Slope error reached limit, time to 
            // increment y and update slope error. 
            if (slope_error_new >= 0) {
                y++;
                slope_error_new -= 2 * (LOS_ij_p2_x - LOS_ij_p1_x);
            }
        }





        /*
         const float GRID_STEP = 64.0f;
         for (float x = 0; x < canvas_sz.x; x += GRID_STEP)
             draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(255, 255, 255, 255));

         for (float y = 0; y < canvas_sz.y; y += GRID_STEP)
             draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(255, 255, 255, 255));

         int i = 0, j = 0;
         for (float x = 0; x < canvas_sz.x; x += GRID_STEP)
         {
             for (float y = 0; y < canvas_sz.y; y += GRID_STEP)
             {
                 char buf[20];
                 sprintf(buf, "\r\n     %d", dt.columns[i][j++]);
                 draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(canvas_p0.x + x, canvas_p0.y + y), IM_COL32(255, 255, 255, 255), buf);
             }
             ++i;
         }
         */

        if (is_hovered)
        {
            ImVec2 p1 = ImVec2(canvas_p0.x + 350, canvas_p0.y + 50);

            char buf[200];
            sprintf(buf, "%4.0f  %4.0f   %4.0f %4.0f\r\n%d %d ", io.MousePos.x, io.MousePos.y, mouse_pos_in_canvas.x, mouse_pos_in_canvas.y, (int)(mouse_pos_in_canvas.x / sideLength), (int)(mouse_pos_in_canvas.y / sideLength));


            DrawBoxedText(buf, p1, IM_COL32_BLACK, IM_COL32_BLACK, IM_COL32_WHITE, 1.0f);
        }

        draw_list->PopClipRect();

        ImGui::End();
    }
    return pOpen;
}


bool ElevationProfileWindow()
{
    ImGui::Begin("Elevation Profile", 0);
    struct Funcs
    {
        static float DTED(void*, int j) { return dt.columns[j][DtedRow]; }
    };

    static int display_count = 3600;
    //ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::SliderInt("Row", &DtedRow, 0, 3600);
    ImGui::SliderInt("Sample count", &display_count, 1, 3600);
    float (*func)(void*, int) = Funcs::DTED;
    ImGui::PlotLines("Lines##1", func, NULL, display_count, 0, NULL, 0, 800.0f, ImVec2(1000, 400));

    ImGui::Separator();


    float (*func2)(void*, int) = Funcs::DTED;
    ImGui::PlotLines("Lines##2", func2, NULL, display_count, 0, NULL, 0, 800.0f, ImVec2(1000, 400));


    if (ImGui::Button("Grid"))
    {
        for (int j = 0;j < 10;j++)
        {
            for (int i = 0;i < 10;i++)
                printf("%03d ", dt.columns[j][i]);
            printf("\r\n");
        }

    }


    ImGui::End();

    return false;
}



