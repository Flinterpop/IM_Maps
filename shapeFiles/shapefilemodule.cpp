#include <vector>
#include <string>

#include "imgui.h"
#include "fonts/IconsFontAwesome5.h"

#include "shapeFileModule.h"
#include "ImBGUtil.h"
#include "shapefil.h"

#include "BGUtil.h"

bool ClipCheck(int x, int y, int limit);
void ClipStatus();

std::vector<ShapeFileBG*> m_ShapeFileList;
static bool gb_ShowShapeFileDialog;



bool LoadShapeFiles()
{
    puts(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    if (GetFileAttributes("shp") == INVALID_FILE_ATTRIBUTES)
    {
        puts("Shape Directory (\"shp\") doesn't exist");
        return true;
    }

    std::vector<std::string> fList = LoadFileNamesInFolder("shp", ".shp");


    if (m_ShapeFileList.size() > 0) m_ShapeFileList.clear();
    else
    {
        for (auto sf : fList)
        {
            ShapeFileBG* sfb;
            sfb = LoadShapeFile((char *)sf.c_str(), IM_COL32(0, 100, 255, 255));
            m_ShapeFileList.push_back(sfb);

        }

       // ShapeFileBG* sfb;
        //sfb = LoadShapeFile("shp\\CanadaExtra\\5M_shoreline_1.shp", IM_COL32(0, 100, 255, 128));
        //m_ShapeFileList.push_back(sfb);
        //sfb = LoadShapeFile("shp\\Canada\\1M_geopolitical_boundary_1.shp", IM_COL32(255, 100, 100, 184));
        //m_ShapeFileList.push_back(sfb);
    }

    return false;
}


bool ShowShapeFileListDialog(bool* pOpen)
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("ShapeFile_List", pOpen, flags);

        if (m_ShapeFileList.size() == 0)  ImGui::Text("There are no ShapeFiles Loaded");
   
        else
        {
            char buf[60];
            for (int r = 0; r < m_ShapeFileList.size(); r++)
            {
                std::string ss = m_ShapeFileList[r]->name.substr(m_ShapeFileList[r]->name.find_last_of("/\\") + 1);
                ImGui::PushID(r);
                sprintf(buf, "%s##%d", ss.c_str(), r);
                ImGui::Text(buf);
                ImGui::PopID();
                ImGui::SameLine(15.0f * ImGui::GetFontSize());
                ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 5.0f);
                sprintf(buf, "##%d", r);
                ImGui::Checkbox(buf, &m_ShapeFileList[r]->b_ShowOnMap);
                ImGui::SameLine();
                sprintf(buf, "##_%d", r);

                ImVec4 llc = ImGui::ColorConvertU32ToFloat4(m_ShapeFileList[r]->ShapeColor);
                sprintf(buf, "##__%d", r);
                if (ImGui::ColorEdit4(buf, (float*)&llc), ImGuiColorEditFlags_NoInputs) m_ShapeFileList[r]->ShapeColor = ImGui::ColorConvertFloat4ToU32(llc);

                ImGui::SameLine();
                sprintf(buf, "Width:##___%d", r);
                ImGui::SetNextItemWidth(5.0f * ImGui::GetFontSize());
                ImGui::DragFloat(buf, &m_ShapeFileList[r]->shapeLineWidth,0.1,0.1,5.0   );


            }
        }
       
 
    ImGui::End();


    if (*pOpen == false)
    {
        //do any cleanup
    }
    return *pOpen;
}

void InvalidateShapeFiles()
{
    printf("INVALID SHAPEFILES\r\n");

    for (auto sf : m_ShapeFileList) sf->b_Valid = false;  //forces recalculation of shapefile coordinates
}


/*
#define SHPT_NULL       0
#define SHPT_POINT      1
#define SHPT_ARC        3
#define SHPT_POLYGON    5
#define SHPT_MULTIPOINT 8
#define SHPT_POINTZ     11
#define SHPT_ARCZ       13
#define SHPT_POLYGONZ   15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM     21
#define SHPT_ARCM       23
#define SHPT_POLYGONM   25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31
*/

void decodeShapeFile(ShapeFileBG * sf)
{
        printf("decodeShapeFile NumShapes: %d\r\n",sf->nEntities);

        for (int y = 0; y < sf->nEntities; y++)
        {
            SHPObject* psShape = sf->EntityList[y];
            if (sf->nShapeType == SHPT_ARC)          printf("SHPT_ARC -     Num vertices this entity: %d\r\n", psShape->nVertices);
            else if (sf->nShapeType == SHPT_POLYGON) printf("SHPT_POLYGON - Num vertices this entity: %d\r\n", psShape->nVertices);
            else
            {
                printf("SHPT_UNKNOWN %d %f - Num vertices this entity: %d\r\n", sf->nShapeType, sf->name, psShape->nVertices);
                
            }
            for (int e = 0; e < psShape->nVertices; e++)
            {
                //printf("Vertex :%d  %0.3f  %0.3f\r\n", e, psShape->padfY[e], psShape->padfX[e]);
            }
        }
}


void decodeAllShapeFiles()
{
    for (int x = 0; x < m_ShapeFileList.size(); x++)
    {
        ShapeFileBG* sf = m_ShapeFileList[x];
        printf("decodeShapeFile x: %d NumShapes: %d", x, sf->nEntities);

        for (int y = 0; y < sf->nEntities; y++)
        {
            SHPObject* psShape = sf->EntityList[y];
            printf("Num vertices this entity: %d", psShape->nVertices);
            for (int e = 0; e < psShape->nVertices; e++)
            {
                printf("Vertex :%d  %0.3f  %0.3f", e, psShape->padfY[e], psShape->padfX[e]);
            }
            //Vector2dVector r = tesselate(psShape);
        }
    }
}





void drawShapeFiles(ImDrawList * draw_list, ImVec2(*funcLatLon2VPxy)(double, double))
{
    for (ShapeFileBG* sf : m_ShapeFileList)
        if (sf->b_ShowOnMap) drawShapefile(draw_list, sf, funcLatLon2VPxy);
}

bool drawShapefile(ImDrawList * draw_list, ShapeFileBG * sf, ImVec2(*funcLatLon2VPxy)(double, double))
{
    if ((sf->nShapeType == SHPT_ARC))// || (sf->nShapeType == SHPT_POLYGON))
    {
        if (false == sf->b_Valid)
        {
            //decodeShapeFile(sf);

            sf->pointsLocalXY.clear();
            for (int y = 0; y < sf->nEntities; y++)
            {
                SHPObject* psShape = sf->EntityList[y];
                //printf("Shp ARC: Entity# %d, num points: %d\r\n", y, psShape->nVertices);
                for (int e = 0; e < psShape->nVertices - 1; e++)
                {
                    ImVec2 p1 = funcLatLon2VPxy(psShape->padfY[e], psShape->padfX[e]);
                    ImVec2 p2 = funcLatLon2VPxy(psShape->padfY[e + 1], psShape->padfX[e + 1]);

                    if (true == ClipCheck(p1.x, p1.y, 1))  sf->pointsLocalXY.push_back(ImVec4(p1.x, p1.y, p2.x, p2.y));
                }
            }
            sf->b_Valid = true;
            ClipStatus();
        }

        else
        {
            for (auto imv4 : sf->pointsLocalXY) draw_list->AddLine(ImVec2(imv4.x, imv4.y), ImVec2(imv4.z, imv4.w), sf->ShapeColor, sf->shapeLineWidth);
        }
    }

    if (sf->nShapeType == SHPT_POLYGON)
    {
        if (false == sf->b_Valid)
        {
            //decodeShapeFile(sf);
            sf->pointsLocalXY.clear();
            for (int y = 0; y < sf->nEntities; y++)
            {
                SHPObject* psShape = sf->EntityList[y];
                //printf("Shp Poly: Entity# %d, num points: %d\r\n", y, psShape->nVertices);
                for (int e = 0; e < psShape->nVertices - 1; e++)
                {
                    ImVec2 p1 = funcLatLon2VPxy(psShape->padfY[e], psShape->padfX[e]);
                    ImVec2 p2 = funcLatLon2VPxy(psShape->padfY[e + 1], psShape->padfX[e + 1]);

                    if (true == ClipCheck(p1.x, p1.y, 1))  sf->pointsLocalXY.push_back(ImVec4(p1.x, p1.y, p2.x, p2.y));
                }
            }
            sf->b_Valid = true;
            ClipStatus();
        }

        else
        {
            for (auto imv4 : sf->pointsLocalXY) draw_list->AddLine(ImVec2(imv4.x, imv4.y), ImVec2(imv4.z, imv4.w), sf->ShapeColor, sf->shapeLineWidth);
        }
    }




    return false;
}



ShapeFileBG* LoadShapeFile(char* fname, ImU32 ShpColor)
{
    ShapeFileBG* _SF = new ShapeFileBG();
    _SF->name = std::string(fname);
    SHPHandle hSHP = SHPOpen(fname, "rb");

    if (hSHP == NULL)
    {
        printf("Unable to open: %s", fname);
        return _SF;
    }

    printf("opened shapefile: %s", fname);
    _SF->nPrecision = 5;

    SHPGetInfo(hSHP, &_SF->nEntities, &_SF->nShapeType, _SF->adfMinBound, _SF->adfMaxBound);
    printf("Shapefile Type: %s   # of Shapes: %d\r\n", SHPTypeName(_SF->nShapeType), _SF->nEntities);
    printf("ShapeType: %d\r\n", _SF->nShapeType);


    /*	Iterate over the list of shapes/Entities                        	*/
    for (int i = 0; i < _SF->nEntities; i++)
    {
        //if (true) printf("Reading Shape %d\r\n", i);
        SHPObject* psShape = SHPReadObject(hSHP, i);
        //auto name = _SF->nEntities

        if (psShape == NULL)
        {
            printf("Unable to read shape # %d, terminating object reading.\r\n", i);
            break;
        }

        _SF->EntityList.push_back(psShape);

    }
    SHPClose(hSHP);

    _SF->ShapeColor = ShpColor;
    //_SF->b_ShowOnMap = true;
    return _SF;
}

