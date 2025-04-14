



#include "Fonts/font_awesome_5.h"
#include <libmgrs/mgrs.h>
#include <libmgrs/utm.h>
#include "ImBGUtil.h"
#include "maps.h"



extern int g_MGRS_Precison;



struct GeoPos
{
    static int m_precision;
    char m_NS;
    int m_latDegree;
    int m_latMinute;
    double m_latSecond;

    char m_EW;
    int m_lngDegree;
    int m_lngMinute;
    double m_lngSecond;

    double m_latitude;
    double m_longitude;

    char m_MGRS[30];
    char MGRS_Pretty[35];

    char m_Buf[100] = "";


    //Constructors
    GeoPos(double _lat, double _lng)
    {
        m_latitude = _lat;
        m_longitude = _lng;
        DDtoDMS();
        DDtoMGRS();
    }

    GeoPos(char _NS, int _lat, int _latMin, double _latSec, char _EW, int _lng, int _lngMin, double _lngSec)
    {
        m_NS = _NS;
        m_latDegree = _lat;
        m_latMinute = _latMin;
        m_latSecond = _latSec;

        m_EW = _EW;
        m_lngDegree = _lng;
        m_lngMinute = _lngMin;
        m_lngSecond = _lngSec;

        DMStoDD();
        DDtoMGRS();

    }

    GeoPos(char* _MGRS)
    {
        strcpy(m_MGRS, _MGRS);
        int error = MGRStoDD();
        if (false == error) DDtoDMS();

    }

    bool DMStoDD()
    {
        m_latitude = m_latDegree + m_latMinute / 60.0 + m_latSecond / 3600.0;
        if (m_NS = 'S') m_latitude = -m_latitude;

        m_longitude = m_lngDegree + m_lngMinute / 60.0 + m_lngSecond / 3600.0;
        if (m_EW = 'W') m_longitude = -m_longitude;

        return false;
    }

    bool MGRStoDD()
    {
        double LatFromMGRS;
        double LngFromMGRS;
        int retVal = Convert_MGRS_To_Geodetic(m_MGRS, &LatFromMGRS, &LngFromMGRS);
        if (UTM_NO_ERROR == retVal)
        {
            m_latitude = RAD2DEG(LatFromMGRS);
            m_longitude = RAD2DEG(LngFromMGRS);
            return false;
        }
        return true;  //means error
    }

    bool DDtoDMS()
    {
        m_NS = 'S'; if (m_latitude > 0) m_NS = 'N';
        double lat = abs(m_latitude);
        
        m_latDegree = (int)lat;
        m_latMinute = (int)((lat - (double)m_latDegree) * 60.0);
        m_latSecond = (int)((lat - (double)m_latDegree - (double)m_latMinute / 60.0) * 60.0 * 60.0);

        m_EW = 'W'; if (m_longitude > 0) m_EW = 'E';
        double lon = abs(m_longitude);
        
        m_lngDegree = (int)lon;
        m_lngMinute = (int)((lon - (double)m_lngDegree) * 60.0);
        m_lngSecond = (int)((lon - (double)m_lngDegree - (double)m_lngMinute / 60.0) * 60.0 * 60.0);

        return false;
    }

    bool DDtoMGRS()
    {
        long retVal = Convert_Geodetic_To_MGRS(DEG2RAD(m_latitude), DEG2RAD(m_longitude), m_precision, m_MGRS);
        if (MGRS_NO_ERROR == retVal)
        {
            MGRSPretty(m_MGRS, MGRS_Pretty);
            return false;
        }
        else
        {
            strcpy(MGRS_Pretty, "Invalid");
            return true;
        }
    }



    char* printDD()
    {
        sprintf_s(m_Buf, sizeof(m_Buf), "%7.4f  %8.4f", m_latitude, m_longitude);
        return m_Buf;
    }


    char* printDMS()
    {
        sprintf(m_Buf, "%c%02d\xc2\xb0 %02d' %05.2f\"  %c%02d\xc2\xb0 %02d' %05.2f", m_NS, m_latDegree, (int)m_latMinute, m_latSecond, m_EW, m_lngDegree, (int)m_lngMinute, m_lngSecond);
        return m_Buf;
    }

    char* printMGRS(bool pretty = true)
    {
        if (pretty) return MGRS_Pretty;
        else return m_MGRS;
    }




};




int GeoPos::m_precision = 5;
static char ip_MGRS_1[4] = "18T";
static char ip_MGRS_2[3] = "VR";
static int ip_Easting = 500;
static int ip_Northing = 500;

void Maps::ShowGeoToolDialog(bool* pOpen)
{
    char buf[200];


#pragma region FROM_D_D
    static double latDdotD = 45.000;
    static double lngDdotD = -75.000;

    ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Geo Tool", pOpen, _flags);

    ImGui::SeparatorText("From D.D");

    ImGui::PushItemWidth(7.0f * ImGui::GetFontSize());

    struct GeoPos geoPos(latDdotD, lngDdotD);
    ImGui::InputDouble("Latitude", &latDdotD, 0.01f, 1.0f, "%07.4f");  ImGui::SameLine(12 * ImGui::GetFontSize()); ImGui::Text(ICON_FA_ARROW_RIGHT);ImGui::SameLine(); ImGui::Text(geoPos.printDMS());
    ImGui::InputDouble("Longitude", &lngDdotD, 0.01f, 1.0f, "%08.4f"); ImGui::SameLine(12 * ImGui::GetFontSize()); ImGui::Text(ICON_FA_ARROW_RIGHT);ImGui::SameLine(); ImGui::Text(geoPos.printMGRS());
    
    ImGui::PopItemWidth();

#pragma endregion FROM_D_D


#pragma region FROM_MGRS
    ImGui::SeparatorText("From MGRS");



    //MGRS Entry Controls
    bool MGRSItemChanged = false;

    ImGui::SetNextItemWidth(2.0f * ImGui::GetFontSize());
    if (ImGui::InputText("###m1", ip_MGRS_1, 4)) MGRSItemChanged = true; ImGui::SameLine();
    ImGui::SetNextItemWidth(2.0f * ImGui::GetFontSize());
    if (ImGui::InputText("###m2", ip_MGRS_2, 3)) MGRSItemChanged = true; ImGui::SameLine();
    ImGui::SetNextItemWidth(6.5f * ImGui::GetFontSize());
    int hiRate = 100;
    if (3 == g_MGRS_Precison) hiRate = 10;
    else if (5 == g_MGRS_Precison) hiRate = 1000;
    if (ImGui::InputInt("###me", &ip_Easting, 1, hiRate, ImGuiInputTextFlags_CharsDecimal)) MGRSItemChanged = true; ImGui::SameLine();
    ImGui::SetNextItemWidth(6.5f * ImGui::GetFontSize());
    if (ImGui::InputInt("###mn", &ip_Northing, 1, hiRate, ImGuiInputTextFlags_CharsDecimal)) MGRSItemChanged = true;


    static char DialogGoToPointMGRS[35];
    int bounds = (int)pow(10, g_MGRS_Precison) - 1;
    ip_Easting = std::clamp(ip_Easting, 0, bounds);
    ip_Northing = std::clamp(ip_Northing, 0, bounds);

    sprintf(DialogGoToPointMGRS, "%s%s%0*d%0*d", ip_MGRS_1, ip_MGRS_2, g_MGRS_Precison, ip_Easting, g_MGRS_Precison, ip_Northing);  /////////////////need to account for precision
    struct GeoPos geoPos2(DialogGoToPointMGRS);




    double LatFromMGRS = 0;
    double LngFromMGRS = 0;
    int retVal = Convert_MGRS_To_Geodetic(DialogGoToPointMGRS, &LatFromMGRS, &LngFromMGRS);
    if (UTM_NO_ERROR == retVal)
    {
        LatFromMGRS = RAD2DEG(LatFromMGRS);
        LngFromMGRS = RAD2DEG(LngFromMGRS);
        sprintf(buf, " %7.4f  %8.4f", LatFromMGRS, LngFromMGRS);

        ImGui::Text("     " ICON_FA_ARROW_RIGHT);ImGui::SameLine();
        ImGui::Text(buf);


        struct GeoPos geoPos2(LatFromMGRS, LngFromMGRS);
        ImGui::Text("     " ICON_FA_ARROW_RIGHT);ImGui::SameLine();
        ImGui::Text(geoPos2.printDMS());




    }
#pragma endregion FROM_MGRS

#pragma region FROM_DNS
    ImGui::SeparatorText("From DMS");

    static int latitide = 45;
    static int longitude = -75;
    
    static char NS = 'N';
    static char EW = 'W';
    
    static int LatDeg = 45;
    static int LatMin = 12;
    static double LatSec = 12.0;

    static int LngDeg = 75;
    static int LngMin = 12;
    static double LngSec = 14.0;

    ImGui::PushItemWidth(7.0f * ImGui::GetFontSize());
    ImGui::Text("N");  ImGui::SameLine();
    ImGui::InputInt("###1", &LatDeg, 1, 10);
    ImGui::SameLine();ImGui::InputInt("###2", &LatMin, 1, 10);
    ImGui::SameLine();ImGui::InputDouble("###3", &LatSec, 1, 10);

    ImGui::Text("W");ImGui::SameLine();
    ImGui::InputInt("###4", &LngDeg, 1, 10);
    ImGui::SameLine();ImGui::InputInt("###5", &LngMin, 1, 10);
    ImGui::SameLine();ImGui::InputDouble("###6", &LngSec, 1, 10);

    ImGui::PopItemWidth();

    
    struct GeoPos geoPos3(NS, LatDeg, LatMin, LatSec, EW, LngDeg, LngMin, LngSec);
    
    ImGui::Text("     " ICON_FA_ARROW_RIGHT);ImGui::SameLine();
    ImGui::Text(geoPos3.printDD());

    ImGui::Text("     " ICON_FA_ARROW_RIGHT);ImGui::SameLine();
    ImGui::Text(geoPos3.printMGRS());

#pragma endregion FROM_DNS


    if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Close")) *pOpen = false;

    ImGui::End();
}



