// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once

#include <forward_list>

#include "FlexMenuBase.h"
#include "FlexMenuDisplay.h"

class FlexMenuDisplay;
class FlexMenuSub;

#include "FlexMenuShowMessage.h"

enum eShowMessageState
{
	eShowMessageState_Idle,
	eShowMessageState_Displaying,
	eShowMessageState_PostDisplaying_IgnoreInput
};


typedef std::function<bool(FlexMenuBase *, FlexMenuManager *)> FlexMenuManagerIterateCB;	//return false to stop iterating


class FlexMenuManager
{
public:
	FlexMenuManager();
	~FlexMenuManager();

	FlexMenuSub * Init(FlexMenuDisplay * pDisplay);	//returns the top menu!

	void Loop(bool bForceRefresh);

	void Navigate(eFlexMenuNav nav);
	
	void ShowMessage(const String & strTitle, const String & strValue, uint32_t milliseconds);

	void IterateItems(FlexMenuManagerIterateCB fnCallback, FlexMenuBase * pStart=NULL);

private:

	void IterateItemsInternal(FlexMenuManagerIterateCB & fnCallback, FlexMenuBase * pStart);

	eShowMessageState stateShowMessage=eShowMessageState_Idle;

	FMM_ShowMessageOuter dummyShowMessage;
	uint32_t timestampShowMessage;


	FlexMenuDisplay * pDisplay=NULL;
	FlexMenuBase * pCurMenu=NULL;

	int menustack_count=0;
	std::forward_list<FlexMenuBase *> menustack;

	void DoLeave();

	//display state
	FlexMenuBase * pLastMenu=NULL;
	int iLastItem=-1;
	int iLastScrollPos=-1;

	void ClearVisible();

	std::vector<FlexMenuBase *> vecUpdateStatus;


	uint8_t HandleAcceleration(int8_t direction);
	int8_t last_direction=0;
	uint32_t last_timestamp=0;
	uint8_t accel_counter=0;

	uint8_t countRepeat=0;
	uint32_t timestampRepeat=0;




	FlexMenuBase * pTopMenu=0;

	void InitialEnterMenu();
	void HandleRepeat();

	bool bWeNeedRefresh=false;


};

