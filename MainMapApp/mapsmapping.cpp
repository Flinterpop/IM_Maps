


#include "ImGui.h"
#include "Fonts/font_awesome_5.h"
#include "tilemanager.h"
#include "AppIni.h"
#include "maps.h"
#include "MouseManager.h"
#include "geocalcs.h"
#include "libmgrs/mgrs.h"
#include "ImBGUtil.h"
#include <libmgrs/utm.h>


extern ImVec2 canvas_pTL;
extern ImVec2 canvas_pBR;


ImVec2Double Maps::VPxyRoLatLng(float VPx, float VPy)
{
    double barLeftXFromCentre = VPx - m_canvas_Centre.x;
    double barLeftDeltaYFromCentre = VPy - m_canvas_Centre.y;
    double barLeftDeltaTextureXFromCentre = barLeftXFromCentre / m_WorldSideLengthInPixels;
    double barLeftDeltaTextureYFromCentre = barLeftDeltaYFromCentre / m_WorldSideLengthInPixels;
    double barLeftTextureX = barLeftDeltaTextureXFromCentre + m_VP_NormalCentre.x;
    double barLeftTextureY = barLeftDeltaTextureYFromCentre + m_VP_NormalCentre.y;
    double barLeftProjX = -20037508.3427892 + barLeftTextureX * 40075016.6855784;
    double barLeftProjY = +20037508.3427892 - barLeftTextureY * 40075016.6855784;

    return ImVec2Double(ProjYtoLat(barLeftProjY), ProjX2Lng(barLeftProjX));
}



ImVec2 Maps::LatLngToVPxy(double lat, double lng)
{

    double POIProjX = Lng2ProjX(lng);
    double POIProjY = Lat2ProjY(lat);

    double POITextureX = (POIProjX + 20037508.3427892) / 40075016.6855784;
    double POITextureY = -(POIProjY - 20037508.3427892) / 40075016.6855784;

    double POIDeltaTextureXFromCentre = m_VP_NormalCentre.x - POITextureX;
    double POIDeltaTextureYFromCentre = m_VP_NormalCentre.y - POITextureY;

    double POIDeltaXFromCentre = POIDeltaTextureXFromCentre * m_WorldSideLengthInPixels;
    double POIDeltaYFromCentre = POIDeltaTextureYFromCentre * m_WorldSideLengthInPixels;

    double POI_VP_X = m_canvas_Centre.x - POIDeltaXFromCentre;
    double POI_VP_Y = m_canvas_Centre.y - POIDeltaYFromCentre;
    return ImVec2((float)POI_VP_X, (float)POI_VP_Y);
}


ImVec2 Maps::LatLngToTexture(double lat, double lng)
{
    double POIProjX = Lng2ProjX(lng);
    double POIProjY = Lat2ProjY(lat);

    double POITextureX = (POIProjX + 20037508.3427892) / 40075016.6855784;
    double POITextureY = -(POIProjY - 20037508.3427892) / 40075016.6855784;

    return ImVec2((float)POITextureX, (float)POITextureY);
}



int clipIn = 0;
int clipOut = 0;
bool ClipCheck(int x, int y, int limit)
{
    ++clipOut;
    if (x + limit < canvas_pTL.x) return false;
    if (x - limit > canvas_pBR.x) return false;

    if (y + limit < canvas_pTL.y) return false;
    if (y - limit > canvas_pBR.y) return false;
    --clipOut;
    ++clipIn;
    return true;
}

void ClipStatus()
{
    printf("ClipIn/ ClipOut:  %d  / %d  \r\n", clipIn, clipOut);
}
