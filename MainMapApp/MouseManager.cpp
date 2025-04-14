
#include "ImVec2Double.h"
#include "MouseManager.h"

#include "ImBgUtil.h"
#include "libmgrs/mgrs.h"
#include "mediator.h"


extern bool g_LeftClickMenuOpen;
extern int g_MGRS_Precison;

struct _MouseManager g_MouseMngr;

extern Mediator _m;

void _MouseManager::MouseOverMapClickHandler(ImVec2Double _MouseLL) //just handle mouse clicks - no drawing
{
    MouseLL = _MouseLL; //always update current Mouse Lat/Lng

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) //set wanted location when mouse single click
    {
        m_LatLongOfLeftSingleClick = MouseLL;
        long retVal = Convert_Geodetic_To_MGRS(DEG2RAD(m_LatLongOfLeftSingleClick.x), DEG2RAD(m_LatLongOfLeftSingleClick.y), g_MGRS_Precison, m_MGRSofLeftSingleClick);

        _m.Notify(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_SINGLE_CLICK_LOCATION,(void *)&m_LatLongOfLeftSingleClick);
    }

    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) //want to open the left click menu and remember where double click was
    {
        m_LatLongOfLeftDoubleClick = MouseLL;  //preset this even though have't selected create new track yet
        long retVal = Convert_Geodetic_To_MGRS(DEG2RAD(m_LatLongOfLeftDoubleClick.x), DEG2RAD(m_LatLongOfLeftDoubleClick.y), g_MGRS_Precison, m_MGRSofLeftDoubleClick);
        _m.Notify(Mediator::NOTIFY_MSG::MED_HAVE_LEFT_DOUBLE_CLICK_LOCATION, (void*)&m_LatLongOfLeftSingleClick);
    }

}
