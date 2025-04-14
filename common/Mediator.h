//---------------------------------------------------------------------------

#ifndef MediatorH
#define MediatorH
//---------------------------------------------------------------------------

#include <vector>
#include <functional>



class Mediator
{

	static int subscriberID_Base;

public:

		enum NOTIFY_MSG { MED_HAVE_LEFT_SINGLE_CLICK_LOCATION, MED_HAVE_LEFT_DOUBLE_CLICK_LOCATION, MED_FOLLOW_TRACK, MED_ACK};

		const char * MSGNames[10] = {"MED_HAVE_LEFT_SINGLE_CLICK_LOCATION","MED_HAVE_LEFT_DOUBLE_CLICK_LOCATION","MED_FOLLOW_TRACK", "MED_ACK"};

        struct s_subscriber
		{
			int subscriber_ID = 0;
			std::function<int(NOTIFY_MSG, void *)> callback;
			NOTIFY_MSG msgNum;
			bool markForDelete = false;
			bool OneShot = true;
		};


		std::vector<s_subscriber*> subscriberlist;

		int SubScribeForNotifications(NOTIFY_MSG msgNum, std::function<int(NOTIFY_MSG, void *)> func, bool oneShot = true);

		int Notify(NOTIFY_MSG msgNum, void* optionalData  = nullptr);
		void CancelSubScription(NOTIFY_MSG msgNum);
		int EraseMarkForDelete();
};



#endif
