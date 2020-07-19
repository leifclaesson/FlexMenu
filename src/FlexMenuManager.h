// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once

#include <forward_list>

#include "FlexMenuBase.h"
#include "FlexMenuDisplay.h"
#include "FlexMenuTransitionFilter.h"

class FlexMenuItemCommand;
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

	void SetBacklightDimDelay(int seconds) { iBacklightDimSeconds=seconds; };
	void SetDisplayMuteDelay(int seconds) { iDisplayMuteSeconds=seconds; };

	void Output();	//write to the screen

	void Navigate(eFlexMenuNav nav);
	
	void ShowPermanentErrorMessage(const String & strTitle, const String & strValue, eFlexMenuFont font);	//never goes away, disables menu

	void ShowMessage(const String & strTitle, const String & strValue, eFlexMenuFont font, uint32_t milliseconds);
	void CancelMessage();

	void IterateItems(FlexMenuManagerIterateCB fnCallback, FlexMenuBase * pStart=NULL);

	void AddDataItem(FlexMenuBase * pDataItem) { data_items.vecDataItems.push_back(pDataItem); }

	FlexMenuDisplay * GetDisplay() { return pDisplay; }

	void RegisterOnSettingsLoadedCallback(std::function<void(void)> fn);
	void RegisterOnLoopCallback(std::function<void(void)> fn);

	void DoOnSettingsLoadedCallback();

	void SetSaveCommand(FlexMenuItemCommand * pSaveCommand) { this->pSaveCommand=pSaveCommand; }
	FlexMenuItemCommand * GetSaveCommand() { return pSaveCommand; }

private:

	FlexMenuItemCommand * pSaveCommand=NULL;

	std::vector<std::function<void(void)>> vecFnOnSettingsLoaded;
	bool bOnSettingsLoadedDone=false;
	std::vector<std::function<void(void)>> vecFnOnLoop;
	void DoOnLoopCallback();

	int iBacklightDimSeconds=60*15;	//default 15 minutes
	int iDisplayMuteSeconds=60*60*4;	//default 4 hours

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


	uint32_t timestampLoop=0;

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

	bool bNavigated=false;

	bool HandleBacklight();

	FlexMenuTransitionFilter<1,8,10> filterBacklight;

	uint32_t lastNavigateTimestamp=0;

};

