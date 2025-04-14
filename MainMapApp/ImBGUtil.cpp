
#include <string>
#include "libmgrs/mgrs.h"
#include "geocalcs.h"
#include "ImBGUtil.h"
#include "fonts/IconsFontAwesome5.h"


extern int g_MGRS_Precison;
extern bool gb_RBL_NM;
extern bool glob_bPlacards;

namespace fs = std::filesystem;

/*
void DrawMapPin(ImVec2(*LatLon2VPxy) (double, double), ImVec2Double PinLocationLL, ImU32 PointColour, char* optionalLabel)
{
    char MGRSs[25];
    long retVal = Convert_Geodetic_To_MGRS(DEG2RAD(PinLocationLL.x), DEG2RAD(PinLocationLL.y), g_MGRS_Precison, MGRSs);

    //draw the end point with amap marker and placard
    ImVec2Double p = LatLon2VPxy(PinLocationLL.x, PinLocationLL.y);
    //draw point to go to:
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * 0.8f, FLT_MAX, 0.0f, ICON_FA_MAP_PIN);
    ImGui::GetWindowDrawList()->AddText(ImVec2((float)(p.x - (textSize.x / 2.0f)), (float)(p.y - textSize.y)), PointColour, ICON_FA_MAP_PIN);
    char buf[50];
    if (optionalLabel != 0) sprintf(buf, "%s\r\n%7.4f\r\n%8.4f\r\n%s", optionalLabel, PinLocationLL.x, PinLocationLL.y, MGRSs);
    else sprintf(buf, "%7.4f\r\n%8.4f\r\n%s", PinLocationLL.x, PinLocationLL.y, MGRSs);
    ImVec2 pos((float)p.x + ImGui::GetFontSize(), (float)p.y + ImGui::GetFontSize());
    DrawBoxedText(buf, pos, PointColour, ImColor(0, 0, 0, 255));
    ImGui::GetWindowDrawList()->AddLine(ImVec2((float)p.x, (float)p.y), pos, PointColour, 1.0f);
}

// void SimAircraft::DrawRBL(ImVec2(*LatLon2VPxy) (double, double), ImVec2 srcLL, ImVec2 tgtLL, ImColor RBL_colour) const
void DrawRBL(ImVec2(*LatLon2VPxy) (double, double), SimAircraft* ac, ImVec2 tgtLL, ImColor RBL_colour)
{
    ImVec2Double startPoint(ac->AC_mp_x, ac->AC_mp_y);// = LatLon2VPxy(ac->ACLat, ac->ACLng);
    ImVec2Double endPoint = LatLon2VPxy(tgtLL.x, tgtLL.y);

    ImGui::GetWindowDrawList()->AddLine(ImVec2((float)startPoint.x, (float)startPoint.y), ImVec2((float)(endPoint.x), (float)(endPoint.y)), RBL_colour, 1.0f);

    ImVec2 midPoint((float)(startPoint.x + 2 * (endPoint.x - startPoint.x) / 3), (float)(startPoint.y + 2 * (endPoint.y - startPoint.y) / 3));

    //calculate and draw arrow
    constexpr double ArrowSideLength = 15;
    constexpr double S = 0.85;  //angle?
    ImVec2 ArrowHeadEndPoint((float)(midPoint.x - startPoint.x), (float)(midPoint.y - startPoint.y));

    auto angleW = std::atan2(ArrowHeadEndPoint.y, ArrowHeadEndPoint.x);
    double angleFLeft = angleW - g_PI * S;
    ArrowHeadEndPoint = ImVec2((float)(midPoint.x + ArrowSideLength * (std::cos(angleFLeft))), (float)(midPoint.y + ArrowSideLength * std::sin(angleFLeft)));
    ImGui::GetWindowDrawList()->AddLine(ImVec2((float)midPoint.x, (float)midPoint.y), ArrowHeadEndPoint, RBL_colour, 2.0f);

    double angleFRight = angleW + g_PI * S;
    ArrowHeadEndPoint = ImVec2((float)(midPoint.x + ArrowSideLength * (std::cos(angleFRight))), (float)(midPoint.y + ArrowSideLength * std::sin(angleFRight)));
    ImGui::GetWindowDrawList()->AddLine(ImVec2((float)midPoint.x, (float)midPoint.y), ArrowHeadEndPoint, RBL_colour, 2.0f);

    //Calculate RBL text 
    ImVec2Double TgtRngHdg = GetBearingAndRangeInNMBetweenTwoGeoPoints(ac->ACLat, ac->ACLng, tgtLL.x, tgtLL.y);

    //calculate Time To Go
    float TimeToGo = 0;//TTG is in hours
    if (0 != ac->speed) TimeToGo = (float)(TgtRngHdg.x / ac->speed);
    int hrs = (int)TimeToGo;
    TimeToGo -= hrs;
    int mins = (int)(TimeToGo * 60.0f);
    TimeToGo = (TimeToGo * 60.0f) - mins;
    int secs = (int)(TimeToGo * 60);

    char buf[60];
    if (gb_RBL_NM)
    {
        sprintf(buf, "%3.1fNM @ %3.1f%\xc2\xb0\r\n TTG: %02d:%02d:%02d\r\n", TgtRngHdg.x, TgtRngHdg.y, hrs, mins, secs);
    }
    else
    {
        TgtRngHdg.x = TgtRngHdg.x * 1.852;  //1.852 km per NM
        if (TimeToGo > 2.0f) sprintf(buf, "%3.1fkm @ %3.1f\xc2\xb0\r\nTTG:%4.2f mins", TgtRngHdg.x, TgtRngHdg.y, TimeToGo);
        else sprintf(buf, "%3.1fkm @ %3.1f\xc2\xb0\r\n TTG: %02d:%02d:%02d\r\n", TgtRngHdg.x, TgtRngHdg.y, hrs, mins, secs);
    }
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * 0.8f, FLT_MAX, 0.0f, buf);


    constexpr float leaderLineLength = 30.0f;
    float deltaX = (float)(endPoint.x - midPoint.x);
    float deltaY = (float)(endPoint.y - midPoint.y);
    float RxR = sqrt(deltaX * deltaX + deltaY * deltaY);
    float scale = leaderLineLength / RxR;
    deltaX = deltaX * scale;
    deltaY = deltaY * scale;


    ImVec2 ePoint;
    angleW = (float)(angleW / (g_PI / 2.0f));

    if ((angleW > 1) && (angleW < 2)) //BL
    {
        //Clockwise Line
        ImVec2 midRightAngle(midPoint.x - deltaY, midPoint.y + deltaX);
        ImGui::GetWindowDrawList()->AddLine(midPoint, midRightAngle, RBL_colour, 0.5f);
        ePoint = ImVec2(midRightAngle.x - textSize.x, midRightAngle.y - textSize.y);
    }
    else if ((angleW > -2) && (angleW < -1))  //TL
    {
        //Clockwise Line
        ImVec2 midRightAngle(midPoint.x - deltaY, midPoint.y + deltaX);
        ImGui::GetWindowDrawList()->AddLine(midPoint, midRightAngle, RBL_colour, 0.5f);
        ePoint = ImVec2(midRightAngle.x, midRightAngle.y - textSize.y);

    }
    else if ((angleW > -1) && (angleW < 0))  //TR
    {
        //Counterclockwise Line
        ImVec2 midRightAngle(midPoint.x - deltaY, midPoint.y + deltaX);
        ImGui::GetWindowDrawList()->AddLine(midPoint, midRightAngle, RBL_colour, 0.5f);
        ePoint = ImVec2(midRightAngle.x, midRightAngle.y);

    }
    else //if ((angleW > 0) && (angleW < -1))  //BR
    {
        //Counterclockwise Line
        ImVec2 midRightAngle(midPoint.x - deltaY, midPoint.y + deltaX);
        ImGui::GetWindowDrawList()->AddLine(midPoint, midRightAngle, RBL_colour, 0.5f);
        ePoint = ImVec2(midRightAngle.x - textSize.x, midRightAngle.y);
    }

    if (glob_bPlacards)  DrawBoxedText(buf, ePoint, RBL_colour, RBL_colour);
    else ImGui::GetWindowDrawList()->AddText(ePoint, RBL_colour, buf);
}

*/

void DrawMGRSGrid(char* MGRS, ImVec2(*LatLon2VPxy) (double, double), int numSides)
{
    if (1 == numSides)
    {
        DrawMGRSGrid(MGRS, LatLon2VPxy);
        return;
    }

    char mg[25];
    strcpy(mg,MGRS);
    char m1 = mg[3];
    char m2 = mg[4];
    int startIndex = (numSides - 1) / 2;

    for (int x = 0;x < numSides;x++)
    {
        for (int y = 0;y < numSides;y++)
        {
            mg[3] = m1 + x - startIndex;
            mg[4] = m2 + y - startIndex;
            DrawMGRSGrid(mg, LatLon2VPxy);
        }
    }
 }


void DrawMGRSGrid(char* MGRS, ImVec2(*LatLon2VPxy) (double, double))
{
    double lats[5];
    double lngs[5];
    ImVec2 xy[5];

    char A[20] = "-----0000000000";
    char B[20] = "-----0000099999";
    char C[20] = "-----9999999999";
    char D[20] = "-----9999900000";

    char First5[6];  //GridZone [3] and 10kmSquare[2]
    for (int x = 0;x < 5;x++)
    {
        First5[x] = MGRS[x];
        A[x] = MGRS[x];
        B[x] = MGRS[x];
        C[x] = MGRS[x];
        D[x] = MGRS[x];
    }

    int prePrecision = g_MGRS_Precison;
    g_MGRS_Precison = 5;
    long retVal;
    char buf[20];

    retVal = Convert_MGRS_To_Geodetic(A, &lats[0], &lngs[0]);
    retVal = Convert_MGRS_To_Geodetic(B, &lats[1], &lngs[1]);
    retVal = Convert_MGRS_To_Geodetic(C, &lats[2], &lngs[2]);
    retVal = Convert_MGRS_To_Geodetic(D, &lats[3], &lngs[3]);

    for (int corner = 0;corner< 4;corner++) //four corners
    {
        lats[corner] = RAD2DEG(lats[corner]);
        lngs[corner] = RAD2DEG(lngs[corner]);
        xy[corner] = LatLon2VPxy(lats[corner], lngs[corner]);
    }

    ImGui::GetWindowDrawList()->AddQuadFilled(xy[0], xy[1], xy[2], xy[3], ImColor(255, 0, 0, 10));
    ImGui::GetWindowDrawList()->AddQuad(xy[0], xy[1], xy[2], xy[3], ImColor(0, 0, 0, 255));
    sprintf(buf, "%c%c%c %c%c", MGRS[0], MGRS[1], MGRS[2], MGRS[3], MGRS[4]);
    ImGui::GetWindowDrawList()->AddText(ImVec2(xy[0].x + 5, xy[0].y - 20), IM_COL32_BLACK, buf);

    g_MGRS_Precison = prePrecision;
}


void MGRSPretty(char* MGRS, char * MGRSPretty)
{
    MGRSPretty[0] = MGRS[0];
    MGRSPretty[1] = MGRS[1];
    MGRSPretty[2] = MGRS[2];
    MGRSPretty[3] = ' ';

    MGRSPretty[4] = MGRS[3];
    MGRSPretty[5] = MGRS[4];
    MGRSPretty[6] = ' ';


    int prec = (int)(strlen(MGRS) - 5) >> 1;
    char east[6];
    char north[6];
    for (int x = 0;x < prec;x++)
    {
        east[x] = MGRS[5 + x];
        north[x] = MGRS[5 + prec + x];
    }
    east[prec] = 0;
    north[prec] = 0;
    sprintf((char*)&MGRSPretty[7], "%s %s", east, north);
}


void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}


void RoundButton(const char* label, ImVec2& textPos)
{
    // Calculate text size for background rectangle
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, label);

    // Draw white background rectangle with purple border
    ImGui::GetWindowDrawList()->AddCircleFilled(textPos, 20, 0xFF0000FF);
    ImGui::GetWindowDrawList()->AddCircle(textPos, 20, 0x000000FF);

    ImGui::GetWindowDrawList()->AddText(textPos, 0x000000FF, label);
}




void DrawBoxedText(const char* label, ImVec2& textPos, ImU32 FGColour, ImU32 TextColour, ImU32 BGColour, float fontScale)
{
    // Calculate text size for background rectangle
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * fontScale, FLT_MAX, 0.0f, label);

    // Draw background rectangle with  border
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(textPos.x - 2, textPos.y - 2),
        ImVec2(textPos.x + textSize.x + 2, textPos.y + textSize.y + 2),BGColour);

    ImGui::GetWindowDrawList()->AddRect(ImVec2(textPos.x - 2, textPos.y - 2),
        ImVec2(textPos.x + textSize.x + 2, textPos.y + textSize.y + 2),
        FGColour, //border
        0.0f,  // rounding
        0,     // flags
        1.0f   // thickness
    );

    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize()* fontScale, textPos, TextColour, label);
}



//IMGUI HELPERS
void ImGui_TextRight(std::string text, int extra)
{
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - extra - ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(posX);
    ImGui::Text("%s", text);
}

void ImGui_TextCentre(std::string text)
{
    //auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 - ImGui::CalcTextSize(text.c_str()).x / 2 - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);

    if (posX > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(posX);
    ImGui::Text("%s", text);
}


bool StopStartToggleButton(char* name, bool& b_toggle)
{
    char buf[30];
    if (b_toggle)
    {
        sprintf(buf, "Stop %s", name);
        if (RedButton(buf)) b_toggle = false;
    }
    else
    {
        sprintf(buf, "Start %s", name);
        if (GreenButton(buf)) b_toggle = true; 
    }
    return b_toggle;
}


bool RedButton(char* name, const ImVec2& size_arg)
{
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    auto retVal = ImGui::Button(name, size_arg);
    ImGui::PopStyleColor(4);
    return retVal;
}


bool GreenButton(char* name, const ImVec2& size_arg)
{
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
    auto retVal = ImGui::Button(name, size_arg);
    ImGui::PopStyleColor(3);
    return retVal;
}



void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}



// GENERIC FUNCTIONS


void HexDump(int bufLen, unsigned char* buf)
{

    bool error = false;
    printf("\r\Dumping %d bytes.\r\n", bufLen);
    char textLine[100]="";
    for (int x = 0; x < bufLen; x++)
    {
        char c = (char)buf[x];
        //if (isalnum(c))
        //    textLine[(x % 16)] = c;
        //else
            textLine[(x % 16)] = '.';

        if (!((x) % 16)) printf("%02X: ", x);
        printf("%02X ", (unsigned short)buf[x]);

        if (!((x + 1) % 16)) {
            textLine[(x % 16) + 1] = 0;
            printf("   %s", textLine);
            printf("\r\n");
        }
    }
    printf("\r\n");
}


