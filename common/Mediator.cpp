#include "Mediator.h"


int Mediator::subscriberID_Base = 1;

//called by clients (not necessarilty subscribers)
int Mediator::Notify(NOTIFY_MSG msgNum, void* optionalData)
{
	int numSubs =   (int)subscriberlist.size();
	printf("Mediator Rx Notification %d\r\n", msgNum);

	if (msgNum==Mediator::NOTIFY_MSG::MED_ACK) return 0;

	for (auto a:  subscriberlist) 
	{
		printf("Msg Numbs %d %d\r\n", a->msgNum ,msgNum);
		if ((a->msgNum == msgNum) && (true))
		{
			a->callback(msgNum, optionalData);//this sends the notification for this message to the subscriber
			a->markForDelete = a->OneShot;
			printf("Notifing %d subscribers with %s   One shot is %d\r\n", numSubs, MSGNames[(int)msgNum], a->markForDelete);
		}
		else puts("No subscriber");
	}

	subscriberlist.erase(std::remove_if(subscriberlist.begin(), subscriberlist.end(), [](s_subscriber* obj) { return (obj->markForDelete); }), subscriberlist.end());
    
	return 0;
}


int Mediator::SubScribeForNotifications(NOTIFY_MSG msgNum,  std::function<int(NOTIFY_MSG msgNum, void *)> func, bool oneShot)
{
	printf("Rx Subscription for %d, One shot = %d\r\n", msgNum, oneShot);
	struct s_subscriber *s = new s_subscriber;
	s->callback = func;
	s->msgNum = msgNum;
	s->OneShot = oneShot;
	s->subscriber_ID = subscriberID_Base++;

   subscriberlist.push_back(s);
   return s->subscriber_ID;
}



int Mediator::EraseMarkForDelete() //delete max of one aircraft then return
{
	for (int x = 0;x < subscriberlist.size();x++)
	{
		if (true == subscriberlist[x]->markForDelete)
		{
			subscriberlist.erase(subscriberlist.begin() + x);
			return 1; //deleted one
		}
	}
	return 0;
}




void Mediator::CancelSubScription(NOTIFY_MSG msgNum)
{
	printf("Rx Cancel for MsgNum = %d\r\n", msgNum);

	for (auto a : subscriberlist)
	{
		printf("Msg Numbs %d %d\r\n", a->msgNum, msgNum);
		if (a->msgNum == msgNum)
		{
			a->markForDelete = true;
		}
		else puts("No subscriber");
	}

	int retVal = EraseMarkForDelete();  //only erases one element at a time
	while (0 < retVal) retVal = EraseMarkForDelete();
	
}

