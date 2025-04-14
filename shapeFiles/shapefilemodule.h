#pragma once


#include <vector>
#include "imgui.h"

#include "shapefil.h"





struct ShapeFileBG
{
	bool b_Valid = false;
	bool b_ShowOnMap = false;
	std::string name = "";

	int nPrecision;
	int nEntities;
	int nShapeType;
	double adfMinBound[4];
	double adfMaxBound[4];

	ImVector < SHPObject*> EntityList; //original Shapes in Lat/:on
	ImVector < SHPObject*> EntityListLocal;  //shapes in local X,Y


	ImVector<ImVec2> pointsLatLon;  // in lat/long
	ImVector<ImVec2> sf_Index; //list of shapes (0 thru x) and the number of vertices per shape (implied they are all ARC (Poly-Line))
	//ImVector<ImVec2> pointsLocalXY; //in local x,y

	ImVector<ImVec4> pointsLocalXY; //in local x,y


	ImU32 ShapeColor;
	ImU32 ShapeFillColor;
	float shapeLineWidth = 1.0f;

};

void InvalidateShapeFiles();
bool LoadShapeFiles();
bool ShowShapeFileListDialog(bool* pOpen);


ShapeFileBG* LoadShapeFile(char* fname, ImU32 ShpColor);
void drawShapeFiles(ImDrawList* draw_list, ImVec2(*funcLatLon2VPxy)(double, double));

bool drawShapefile(ImDrawList* draw_list, ShapeFileBG* sf, ImVec2(*funcLatLon2VPxy)(double, double));