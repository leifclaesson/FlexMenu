// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once

#include <forward_list>

#include "FlexMenuBase.h"
#include "FlexMenuDisplay.h"

class FlexMenuDisplay;
class FlexMenuSub;

#include "FlexMenuShowMessage.h"
#include "FlexMenuItemData.h"

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

	bool Loop(bool bForceRefresh);	//return true if the display needs to be updated

	void Output();	//write to the screen

	void Navigate(eFlexMenuNav nav);
	
	void ShowPermanentErrorMessage(const String & strTitle, const String & strValue, eFlexMenuFont font);	//never goes away, disables menu

	void ShowMessage(const String & strTitle, const String & strValue, eFlexMenuFont font, uint32_t milliseconds);
	void CancelMessage();

	void IterateItems(FlexMenuManagerIterateCB fnCallback, FlexMenuBase * pStart=NULL);

	void AddDataItem(FlexMenuBase * pDataItem) { data_items.vecDataItems.push_back(pDataItem); }

private:

	FlexMenuDataItems data_items;

	void IterateItemsInternal(FlexMenuManagerIterateCB & fnCallback, FlexMenuBase * pStart);

	eShowMessageState stateShowMessage=eShowMessageState_Idle;

	FMM_ShowMessageOuter dummyShowMessage;
	uint32_t timestampShowMessage;
	bool bPermanentErrorMessage=false;


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


	bool bSkipFirstIgnoreInput=true;


	FlexMenuBase * pTopMenu=0;

	void InitialEnterMenu();
	void HandleRepeat();

	bool bWeNeedRefresh=false;

	uintptr_t vpHistoryBuffer[5];
	uint32_t lastHistoryBufTimestamp=0;
	uint8_t history_idx=0;

	uint8_t run_history=0xFF;

	void HandleHistoryBuffer();
	void ClearHistoryBuffer();

};

