// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once

#include <forward_list>

#include "FlexMenuBase.h"
#include "FlexMenuDisplay.h"

class FlexMenuDisplay;
class FlexMenuSub;

enum eDisplayMessageState
{
	eDisplayMessageState_Idle,
	eDisplayMessageState_Displaying,
	eDisplayMessageState_PostDisplaying_IgnoreInput

};

class FMM_DisplayMessage : public FlexMenuBase
{
public:

	virtual void GetTitleText(String & strTitleDestination) { strTitleDestination=strTitle; }
	virtual void GetValueText(String & strValueDestination) { strValueDestination=strValue; }

	String strTitle;
	String strValue;

	virtual eFlexMenuScreenType GetScreenType() override { return eFlexMenuScreenType_Message; }

	virtual int16_t GetNumSubItems() override { return 1; };
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { return this; }

};


class FlexMenuManager
{
public:
	FlexMenuManager();
	~FlexMenuManager();

	FlexMenuSub * Init(FlexMenuDisplay * pDisplay);	//returns the top menu!

	void Loop(bool bForceRefresh);

	void Navigate(eFlexMenuNav nav);
	
	void DisplayMessage(const String & strTitle, const String & strValue, uint32_t milliseconds);

private:

	eDisplayMessageState stateDisplayMessage=eDisplayMessageState_Idle;

	FMM_DisplayMessage dummyDisplayMessage;
	uint32_t timestampDisplayMessage;


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

